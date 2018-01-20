/*!
 * @header      ExpansionPort.h
 * @author      Written by Dirk Hoffmann based on the original code by A. Carl Douglas.
 * @copyright   All rights reserved.
 */
/* This program is free software; you can redistribute it and/or modify
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

/*
 * For more information: http://www.c64-wiki.com/index.php/Cartridge
 *
 * "The cartridge system implemented in the C64 provides an easy way to
 *  hook 8 or 16 kilobytes of ROM into the computer's address space:
 *  This allows for applications and games up to 16 K, or BASIC expansions
 *  up to 8 K in size and appearing to the CPU along with the built-in
 *  BASIC ROM. In theory, such a cartridge need only contain the
 *  ROM circuit without any extra support electronics."
 *
 *  Also: http://www.c64-wiki.com/index.php/Bankswitching
 *
 *  As well read the Commodore 64 Programmers Reference Guide pages 260-267.
 */

#ifndef _EXPANSIONPORT_H
#define _EXPANSIONPORT_H

#include "Cartridge.h"

class ExpansionPort : public VirtualComponent {
 
private:
    
    /*! @brief    Attached cartridge
     *  @details  NULL, if no cartridge is plugged in.
     */
    Cartridge *cartridge;
    
public:
    
    //! @brief    Constructor
    ExpansionPort();
    
    //! @brief    Destructor
    ~ExpansionPort();
        
    //! @brief    Resets the expansion port
    void resetPort();
    
    //! @brief    Reverts expansion port to its initial state, but keeps cartridge data in place
    void softreset();

    //! @brief    Dumps the current configuration into the message queue
    void ping();

    //! @brief    Returns the size of the internal state
    uint32_t stateSize();

    //! @brief    Loads the current state from a buffer
    void loadFromBuffer(uint8_t **buffer);
    
    //! @brief    Save the current state into a buffer
    void saveToBuffer(uint8_t **buffer);
    
    //! @brief    Prints debugging information
    void dumpState();	
    
    //! @brief    Returns true if cartride ROM is blended in at the specified location
    bool romIsBlendedIn(uint16_t addr);
    
    //! @brief    Peek fallthrough
    uint8_t peek(uint16_t addr);
    
    //! @brief    Poke fallthrough
    void poke(uint16_t addr, uint8_t value);
    
    //! @brief    Returns the cartridge type
    Cartridge::CartridgeType getCartridgeType();
    
    //! @brief    Returns the state of the game line
    bool getGameLine();
    
    //! @brief    Informs the expansion port of a game line change
    void gameLineHasChanged();
    
    //! @brief    Returns the state of the exrom line
    bool getExromLine();
    
    //! @brief    Informs the expansion port of a game line change
    void exromLineHasChanged();
    
    //! @brief    Returns true if a cartridge is attached to the expansion port
    inline bool getCartridgeAttached() { return cartridge != NULL; }

    //! @brief    Attaches a cartridge to the expansion port
    bool attachCartridge(Cartridge *c);

    //! @brief    Attaches a cartridge to the expansion port
    /*! @details  Cartridge is provided by a data buffer
     */
    bool attachCartridge(uint8_t **buffer, Cartridge::CartridgeType type);

    //! @brief    Attaches a cartridge to the expansion port
    /*! @details  Cartridge is provided by a CRTContainer
     */
    bool attachCartridge(CRTContainer *c);

    //! @brief    Removes a cartridge from the expansion port (if any)
    void detachCartridge();

};
    
#endif
