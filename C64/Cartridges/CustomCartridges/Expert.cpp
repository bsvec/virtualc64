/*!
 * @file        Expert.cpp
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann. All rights reserved.
 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

// This implementation is based mainly by the following documents:
// Schematics and explanation by Martin Sikström:
// https://people.kth.se/~e93_msi/c64/expert.html

#include "C64.h"

Expert::Expert(C64 *c64) : Cartridge(c64)
{
    setDescription("Expert");
    
    active = false;
    setSwitch(0);
    newSwitchPos = 1;
    
    // Allocate 8KB bytes persistant RAM
    setRamCapacity(0x2000);
    setPersistentRam(true);
    
    debug("Expert cartridge created\n");
}

void
Expert::reset()
{
    Cartridge::reset();
}

void
Expert::dump()
{
    Cartridge::dump();
    
    msg("               active: %d\n", active);
    msg("             switch: %d ", getSwitch());
    if (switchInPrgPosition()) msg("(PRG)\n");
    if (switchInOffPosition()) msg("(OFF)\n");
    if (switchInOnPosition()) msg("(ON)\n");
    msg("         NMI vector: %04X\n", LO_HI(peekRAM(0x1FFA), peekRAM(0x1FFB)));
    msg("         IRQ vector: %04X\n", LO_HI(peekRAM(0x1FFE), peekRAM(0x1FFF)));
    msg("       Reset vector: %04X\n", LO_HI(peekRAM(0x1FFC), peekRAM(0x1FFD)));
}

size_t
Expert::stateSize()
{
    return Cartridge::stateSize() + 2;
}

void
Expert::didLoadFromBuffer(uint8_t **buffer)
{
    Cartridge::didLoadFromBuffer(buffer);
    active = read8(buffer);
    newSwitchPos = (int8_t)read8(buffer);
}

void
Expert::didSaveToBuffer(uint8_t **buffer)
{
    Cartridge::didSaveToBuffer(buffer);
    write8(buffer, (uint8_t)active);
    write8(buffer, (uint8_t)newSwitchPos);
}

void
Expert::loadChip(unsigned nr, CRTFile *c)
{
    uint16_t chipSize = c->chipSize(nr);
    uint16_t chipAddr = c->chipAddr(nr);
    uint8_t *chipData = c->chipData(nr);
    
    if (nr != 0 || chipSize != 0x2000 || chipAddr != 0x8000) {
        warn("Corrupted CRT file. Aborting.");
        return;
    }

    assert(getRamCapacity() == chipSize);

    // Initialize RAM with data from CRT file
    debug("Copying file contents into Expert RAM\n");
    for (unsigned i = 0; i < chipSize; i++) {
        pokeRAM(i, chipData[i]);
    }
}

void
Expert::pressFreezeButton() {
    
    debug("Expert::pressFreezeButton\n");
    
    c64->suspend();
    
    if (switchInOnPosition()) {
        debug("Switching on cartridge\n");
        active = true;
    }
    
    // The Expert cartridge uses two three-state buffers in parallel to force
    // the NMI line high, even if a program leaves it low to protect itself
    // against freezers. The following code is surely not accurate, but it
    // forces an NMI a trigger, regardless of the current value of the NMI
    // line.
    
    uint8_t oldLine = c64->cpu.nmiLine;
    uint8_t newLine = oldLine | CPU::INTSRC_EXPANSION;
    
    c64->cpu.releaseNmiLine((CPU::IntSource)0xFF);
    c64->cpu.pullDownNmiLine((CPU::IntSource)newLine);
    c64->cpu.releaseNmiLine(CPU::INTSRC_EXPANSION);
    
    /*
    c64->cpu.pullDownNmiLine(CPU::INTSRC_EXPANSION);
    c64->cpu.releaseNmiLine(CPU::INTSRC_EXPANSION);
    */
    c64->resume();
}

void
Expert::releaseFreezeButton()
{
    debug("Expert::releaseFreezeButton\n");
}

void
Expert::pressResetButton()
{
    debug("Expert::pressResetButton\n");

    c64->suspend();

    dump();
    
    if (switchInOnPosition()) {
        debug("Switching on cartridge\n");
        active = true;
    }
    Cartridge::pressResetButton();
    dump();
    
    c64->resume();
}

uint8_t
Expert::peek(uint16_t addr)
{
    if (cartridgeRamIsVisible(addr)) {
        
        // Get value from cartridge RAM
        return peekRAM(addr & 0x1FFF);
        
    } else {
        
        // Get value as if no cartridge was attached
        return c64->mem.peek(addr, 1, 1);
    }
}

uint8_t
Expert::peekIO1(uint16_t addr)
{
    assert(addr >= 0xDE00 && addr <= 0xDEFF);
    
    // Any IO1 access disables the cartridge
    active = false;
    
    return 0;
}

void
Expert::poke(uint16_t addr, uint8_t value)
{
    if (cartridgeRamIsVisible(addr)) {
        
        // Write value into cartridge RAM if it is write enabled
        if (cartridgeRamIsWritable(addr)) {
            // debug("Writing RAM cell %04X with %02X\n", addr & 0x1FFF, value);
            pokeRAM(addr & 0x1FFF, value);
        }
    
    } else {
    
        // Write value as if no cartridge was attached
        c64->mem.poke(addr, value, 1, 1);
    }
}

void
Expert::pokeIO1(uint16_t addr, uint8_t value)
{
    assert(addr >= 0xDE00 && addr <= 0xDEFF);
    
    debug("Expert::pokeIO1\n");
    
    // Any IO1 access disabled the cartridge
    active = false;
}

const char *
Expert::getSwitchDescription(int8_t pos)
{
    return (pos < 0) ? "Prg" : (pos > 0) ? "On" : "Off";
}

void
Expert::setSwitch(int8_t pos)
{
    debug("Setting switch to %d\n", pos);

    Cartridge::setSwitch(pos);
}

void
Expert::toggleSwitch()
{
    if (newSwitchPos < 0) {
        newSwitchPos = 0;
        setSwitch(-1);
    } else if (newSwitchPos > 0) {
        newSwitchPos = 0;
        setSwitch(1);
    } else {
        newSwitchPos = (getSwitch() < 0) ? 1 : -1;
        setSwitch(0);
    }
    c64->putMessage(MSG_CART_SWITCH);
}

bool
Expert::cartridgeRamIsVisible(uint16_t addr)
{
    if (addr < 0x8000) {
        assert(false); // Should never be called for this address space
        return false;
    }
    if (addr < 0xA000) {
        return switchInPrgPosition() || (switchInOnPosition() && active);
    }
    if (addr < 0xE000) {
        return false;
    }
    return switchInOnPosition() && active;
}

bool
Expert::cartridgeRamIsWritable(uint16_t addr)
{
    return isROMLaddr(addr);
}

void
Expert::updatePeekPokeLookupTables()
{
    // Setting up faked Ultimax mode. We let the Game and Exrom line as they
    // are, but reroute all access to ROML and ROMH into the cartridge.
    
    // Reroute ROML
     c64->mem.peekSrc[0x8] = c64->mem.pokeTarget[0x8] = M_CRTLO;
     c64->mem.peekSrc[0x9] = c64->mem.pokeTarget[0x9] = M_CRTLO;

    // Reroute ROMH
     c64->mem.peekSrc[0xE] = c64->mem.pokeTarget[0xE] = M_CRTLO;
     c64->mem.peekSrc[0xF] = c64->mem.pokeTarget[0xF] = M_CRTLO;
}

void
Expert::nmiWillTrigger()
{
    // Activate cartridge if switch is in 'ON' position
    if (switchInOnPosition()) { active = 1; }
}
