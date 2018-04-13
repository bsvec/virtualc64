/*
 * (C) 2006 - 2018 Dirk W. Hoffmann. All rights reserved.
 *
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

#include "C64.h"

CIA::CIA()
{
	setDescription("CIA");
    
    // Register sub components
    VirtualComponent *subcomponents[] = { &tod, NULL };
    registerSubComponents(subcomponents, sizeof(subcomponents));

    // Register snapshot items
    SnapshotItem items[] = {
        
        { &counterA,        sizeof(counterA),       CLEAR_ON_RESET },
        { &latchA,          sizeof(latchA),         CLEAR_ON_RESET },
        { &counterB,        sizeof(counterB),       CLEAR_ON_RESET },
        { &latchB,          sizeof(latchB),         CLEAR_ON_RESET },
        { &delay,           sizeof(delay),          CLEAR_ON_RESET },
        { &feed,            sizeof(feed),           CLEAR_ON_RESET },
        { &CRA,             sizeof(CRA),            CLEAR_ON_RESET },
        { &CRB,             sizeof(CRB),            CLEAR_ON_RESET },
        { &ICR,             sizeof(ICR),            CLEAR_ON_RESET },
        { &IMR,             sizeof(IMR),            CLEAR_ON_RESET },
        { &PB67TimerMode,   sizeof(PB67TimerMode),  CLEAR_ON_RESET },
        { &PB67TimerOut,    sizeof(PB67TimerOut),   CLEAR_ON_RESET },
        { &PB67Toggle,      sizeof(PB67Toggle),     CLEAR_ON_RESET },
        { &PALatch,         sizeof(PALatch),        CLEAR_ON_RESET },
        { &PBLatch,         sizeof(PBLatch),        CLEAR_ON_RESET },
        { &DDRA,            sizeof(DDRA),           CLEAR_ON_RESET },
        { &DDRB,            sizeof(DDRB),           CLEAR_ON_RESET },
        { &SDR,             sizeof(SDR),            CLEAR_ON_RESET },
        { &serClk,          sizeof(serClk),         CLEAR_ON_RESET },
        { &serCounter,      sizeof(serCounter),     CLEAR_ON_RESET },
        { &PA,              sizeof(PA),             CLEAR_ON_RESET },
        { &PB,              sizeof(PB),             CLEAR_ON_RESET },
        { &CNT,             sizeof(CNT),            CLEAR_ON_RESET },
        { &INT,             sizeof(INT),            CLEAR_ON_RESET },
        { &tiredness,       sizeof(tiredness),      CLEAR_ON_RESET },
        { NULL,             0,                      0 }};

    registerSnapshotItems(items, sizeof(items));
}

CIA::~CIA()
{
}

void
CIA::reset()
{
    VirtualComponent::reset();
    
    PA = 0xFF;
    PB = 0xFF;

	CNT = true;
	INT = 1;
	
	latchA = 0xFFFF;
	latchB = 0xFFFF;
}

void
CIA::triggerRisingEdgeOnFlagPin()
{
    // ICR &= ~0x10; // Note: FLAG pin is inverted
}

void
CIA::triggerFallingEdgeOnFlagPin()
{
    ICR |= 0x10; // Note: FLAG pin is inverted
        
    // Trigger interrupt, if enabled
    if (IMR & 0x10) {
        INT = 0;
        ICR |= 0x80;
        pullDownInterruptLine();
    }
}

uint8_t
CIA::peek(uint16_t addr)
{
	uint8_t result;
	
    wakeUp();
    
	switch(addr) {
            
        case 0x00: // CIA_DATA_PORT_A
            
            result = peekDataPortA();
            break;
            
        case 0x01: // CIA_DATA_PORT_B
            
            result = peekDataPortB();
            break;
            
        case 0x02: // CIA_DATA_DIRECTION_A

			result = DDRA;
			break;

        case 0x03: // CIA_DATA_DIRECTION_B

			result = DDRB;
			break;
			
        case 0x04: // CIA_TIMER_A_LOW
			
			result = getCounterALo();
			break;
			
        case 0x05: // CIA_TIMER_A_HIGH
			result = getCounterAHi();
			break;
			
        case 0x06: // CIA_TIMER_B_LOW

			result = getCounterBLo();
			break;
			
        case 0x07: // CIA_TIMER_B_HIGH
			
			result = getCounterBHi();
			break;
			
        case 0x08: // CIA_TIME_OF_DAY_SEC_FRAC
			
			result = tod.getTodTenth();
			break;
		
        case 0x09: // CIA_TIME_OF_DAY_SECONDS
			
			result = tod.getTodSeconds();
			break;
			
        case 0x0A: // CIA_TIME_OF_DAY_MINUTES
			
			result = tod.getTodMinutes();
			break;
			
        case 0x0B: // CIA_TIME_OF_DAY_HOURS

			result = tod.getTodHours();
			break;
			
        case 0x0C: // CIA_SERIAL_DATA_REGISTER
			
			result = SDR;
			break;
			
        case 0x0D: // CIA_INTERRUPT_CONTROL
		
			result = ICR;
            
			// Release interrupt request
			if (INT == 0) {
				INT = 1;
                releaseInterruptLine();
			}
			
			// Discard pending interrupts
			delay &= ~(Interrupt0 | Interrupt1);

			// Clear all bits except bit 7
			ICR &= 0x80;
            
            // Schedule bit 7 to be cleared in the next cycle and remember the read access
            delay |= (ClearIcr0 | ReadIcr0);
            
			break;

        case 0x0E: // CIA_CONTROL_REG_A

			result = (uint8_t)(CRA & ~0x10); // Bit 4 is always 0 when read
			break;
			
        case 0x0F: // CIA_CONTROL_REG_B
			
			result = (uint8_t)(CRB & ~0x10); // Bit 4 is always 0 when read
			break;
			
		default:
			result = 0;
			panic("Unknown CIA address %04X\n", addr);
			break;
	}
	
	return result;
}

void CIA::poke(uint16_t addr, uint8_t value)
{
    wakeUp();
    
	switch(addr) {
		
        case 0x00: // CIA_DATA_PORT_A
            
            pokeDataPortA(value);
            return;
            
        case 0x01: // CIA_DATA_PORT_B
            
            pokeDataPortB(value);
            return;
            
        case 0x02: // CIA_DATA_DIRECTION_A
            
            pokeDataPortDirectionA(value);
            return;
            
        case 0x03: // CIA_DATA_DIRECTION_B
            
            pokeDataPortDirectionB(value);
            return;
            
        case 0x04: // CIA_TIMER_A_LOW
			
			setLatchALo(value);
			if (delay & LoadA2) {
				setCounterALo(value);
			}
			return;
			
        case 0x05: // CIA_TIMER_A_HIGH
						
			setLatchAHi(value);		
            if (delay & LoadA2) {
                setCounterAHi(value);
            }
            
			// Load counter if timer is stopped
			if (!(CRA & 0x01)) {
				delay |= LoadA0;
			}
			return;
			
        case 0x06: // CIA_TIMER_B_LOW

			setLatchBLo(value);
			if (delay & LoadB2) {
				setCounterBLo(value);
			}			
			return;
			
        case 0x07: // CIA_TIMER_B_HIGH
			
			setLatchBHi(value);
            if (delay & LoadB2) {
                setCounterBHi(value);
            }
            
			// Load counter if timer is stopped
			if ((CRB & 0x01) == 0) {
				delay |= LoadB0;
			}
			return;
			
        case 0x08: // CIA_TIME_OF_DAY_SEC_FRAC
            
			if (CRB & 0x80) {
				tod.setAlarmTenth(value);
                checkForTODInterrupt();
			} else { 
				tod.setTodTenth(value);
                checkForTODInterrupt();
			}
			return;
			
        case 0x09: // CIA_TIME_OF_DAY_SECONDS
            
            if (CRB & 0x80) {
				tod.setAlarmSeconds(value);
                checkForTODInterrupt();
            } else {
				tod.setTodSeconds(value);
                checkForTODInterrupt();
            }
			return;
			
        case 0x0A: // CIA_TIME_OF_DAY_MINUTES
            
            if (CRB & 0x80) {
				tod.setAlarmMinutes(value);
                checkForTODInterrupt();
            } else {
				tod.setTodMinutes(value);
                checkForTODInterrupt();
            }
			return;
			
        case 0x0B: // CIA_TIME_OF_DAY_HOURS
			
			if (CRB & 0x80) {
				tod.setAlarmHours(value);
                checkForTODInterrupt();
			} else {
                // Writing 12 pm into hour register turns to 12 am and vice versa.
				if ((value & 0x1F) == 0x12)
					value ^= 0x80;
				tod.setTodHours(value);
                checkForTODInterrupt();
			}
			return;
			
        case 0x0C: // CIA_DATA_REGISTER
            
            SDR = value;
            delay |= SerLoad0;
            feed |= SerLoad0;
            // delay &= ~SerLoad1;
			return;
			
        case 0x0D: // CIA_INTERRUPT_CONTROL
			
			// Bit 7 means set (1) or clear (0) the other bits
			if ((value & 0x80) != 0) {
				IMR |= (value & 0x1F);
			} else {
				IMR &= ~(value & 0x1F);
			}
            
			// Raise an interrupt in the next cycle if conditions match
			if ((IMR & ICR & 0x1F) && INT) {
                delay |= (Interrupt0 | SetIcr0);
			}
            
            // Clear pending interrupt if a write has occurred in the previous cycle
            // Solution is taken from Hoxs64. It fixes dd0dtest (11)
            else if (delay & ClearIcr2) {
                delay &= ~(Interrupt1 | SetIcr1);
            }
            
			return;
			
        case 0x0E: // CIA_CONTROL_REG_A
		
            // -------0 : Stop timer
            // -------1 : Start timer
            if (value & 0x01) {
                delay |= CountA1 | CountA0;
                feed |= CountA0;
                if (!(CRA & 0x01))
                    PB67Toggle |= 0x40; // Toggle is high on start
            } else {
                delay &= ~(CountA1 | CountA0);
                feed &= ~CountA0;
            }
            
            // ------0- : Don't indicate timer underflow on port B
            // ------1- : Indicate timer underflow on port B bit 6
            if (value & 0x02) {
                PB67TimerMode |= 0x40;
                if (!(value & 0x04)) {
                    if ((delay & PB7Low1) == 0) {
                        PB67TimerOut &= ~0x40;
                    } else {
                        PB67TimerOut |= 0x40;
                    }
                } else {
                    PB67TimerOut = (PB67TimerOut & ~0x40) | (PB67Toggle & 0x40);
                }
            } else {
                PB67TimerMode &= ~0x40;
            }
            
            // -----0-- : Upon timer underflow, invert port B bit 6
            // -----1-- : Upon timer underflow, generate a positive edge
            //            on port B bit 6 for one cycle

            // ----0--- : Timer restarts upon underflow
            // ----1--- : Timer stops upon underflow (One shot mode)
            if (value & 0x08) {
                feed |= OneShotA0;
            } else {
                feed &= ~OneShotA0;
            }
            
            // ---0---- : Nothing to do
            // ---1---- : Load start value into timer
            if (value & 0x10) {
                delay |= LoadA0;
            }

            // --0----- : Timer counts system cycles
            // --1----- : Timer counts positive edges on CNT pin
            if (value & 0x20) {
                delay &= ~(CountA1 | CountA0);
                feed &= ~CountA0;
            }
    
            // -0------ : Serial shift register in input mode (read)
            // -1------ : Serial shift register in output mode (write)
            if ((value ^ CRA) & 0x40)
            {
                //serial direction changing
                delay &= ~(SerLoad0 | SerLoad1);
                feed &= ~SerLoad0;
                serCounter = 0;
            
                delay &= ~(SerClk0 | SerClk1 | SerClk2);
                feed &= ~SerClk0;
            }
            
            // 0------- : TOD speed = 60 Hz
            // 1------- : TOD speed = 50 Hz
            // TODO: We need to react on a change of this bit
            tod.setHz((value & 0x80) ? 5 /* 50 Hz */ : 6 /* 60 Hz */);
            
			PB = ((PBLatch | ~DDRB) & ~PB67TimerMode) | (PB67TimerOut & PB67TimerMode);
			CRA = value;
			
			return;
			
        case 0x0F: // CIA_CONTROL_REG_B
		{
            // -------0 : Stop timer
            // -------1 : Start timer
            if (value & 0x01) {
                delay |= CountB1 | CountB0;
                feed |= CountB0;
                if (!(CRB & 0x01))
                    PB67Toggle |= 0x80; // Toggle is high on start
            } else {
                delay &= ~(CountB1 | CountB0);
                feed &= ~CountB0;
            }
            
            // ------0- : Don't indicate timer underflow on port B
            // ------1- : Indicate timer underflow on port B bit 7
            if (value & 0x02) {
                PB67TimerMode |= 0x80;
                if ((value & 0x04) == 0) {
                    if ((delay & PB7Low1) == 0) {
                        PB67TimerOut &= ~0x80;
                    } else {
                        PB67TimerOut |= 0x80;
                    }
                } else {
                    PB67TimerOut = (PB67TimerOut & ~0x80) | (PB67Toggle & 0x80);
                }
            } else {
                PB67TimerMode &= ~0x80;
            }
            
            // -----0-- : Upon timer underflow, invert port B bit 7
            // -----1-- : Upon timer underflow, generate a positive edge
            //            on port B bit 7 for one cycle
            
            // ----0--- : Timer restarts upon underflow
            // ----1--- : Timer stops upon underflow (One shot mode)
            if (value & 0x08) {
                feed |= OneShotB0;
            } else {
                feed &= ~OneShotB0;
            }
            
            // ---0---- : Nothing to do
            // ---1---- : Load start value into timer
            if (value & 0x10) {
                delay |= LoadB0;
            }
            
            // -00----- : Timer counts system cycles
            // -01----- : Timer counts positive edges on CNT pin
            // -10----- : Timer counts underflows of timer A
            // -11----- : Timer counts underflows of timer A occurring along with a
            //            positive edge on CNT pin
            if (value & 0x60) {
                delay &= ~(CountB1 | CountB0);
                feed &= ~CountB0;
            }
            
            // 0------- : Writing into TOD registers sets TOD
            // 1------- : Writing into TOD registers sets alarm time
            
            
			PB = ((PBLatch | ~DDRB) & ~PB67TimerMode) | (PB67TimerOut & PB67TimerMode);
			CRB = value;
			
			return;			
		}
			
		default:
			panic("PANIC: Unknown CIA address (poke) %04X\n", addr);
	}	
}

void
CIA::incrementTOD()
{
    wakeUp();
    tod.increment();
    checkForTODInterrupt();
}

void
CIA::checkForTODInterrupt()
{
    if (tod.alarming()) {
        delay |= TODInt0;
    }
}

void CIA::dumpTrace()
{
	const char *indent = "                                                                      ";

	if (!tracingEnabled()) 
		return;
	
	debug(1, "%sICR: %02X IMR: %02X ", indent, ICR, IMR);
	debug(1, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n",
			delay & CountA0 ? "CntA0 " : "",
			delay & CountA1 ? "CntA1 " : "",
			delay & CountA2 ? "CntA2 " : "",
			delay & CountA3 ? "CntA3 " : "",
			delay & CountB0 ? "CntB0 " : "",
			delay & CountB1 ? "CntB1 " : "",
			delay & CountB2 ? "CntB2 " : "",
			delay & CountB3 ? "CntB3 " : "",
			delay & LoadA0 ? "LdA0 " : "",
			delay & LoadA1 ? "LdA1 " : "",
			delay & LoadA2 ? "LdA2 " : "",
			delay & LoadB0 ? "LdB0 " : "",
			delay & LoadB1 ? "LdB1 " : "",
			delay & LoadB1 ? "LdB2 " : "",
			delay & PB6Low0 ? "PB6Lo0 " : "",
			delay & PB6Low1 ? "PB6Lo1 " : "",
			delay & PB7Low0 ? "PB7Lo0 " : "",
			delay & PB7Low1 ? "PB7Lo1 " : "",
			delay & Interrupt0 ? "Int0 " : "",
			delay & Interrupt1 ? "Int1 " : "",
			delay & OneShotA0 ? "1ShotA0 " : "",
			delay & OneShotB0 ? "1ShotB0 " : "");

	debug(1, "%sA: %04X (%04X) PA: %02X (%02X) DDRA: %02X CRA: %02X\n",
		  indent, counterA, latchA, PA, PALatch, DDRA, CRA);
	debug(1, "%sB: %04X (%04X) PB: %02X (%02X) DDRB: %02X CRB: %02X\n",
		  indent, counterB, latchB, PB, PBLatch, DDRB, CRB);
}

void CIA::dumpState()
{
	msg("              Counter A : %02X\n", getCounterA());
	msg("                Latch A : %02X\n", getLatchA());
	msg("            Data port A : %02X\n", getDataPortA());
	msg("  Data port direction A : %02X\n", getDataPortDirectionA());
	msg("     Control register A : %02X\n", getControlRegA());
	msg("     Timer A interrupts : %s\n", isInterruptEnabledA() ? "enabled" : "disabled");	
	msg("\n");
	msg("              Counter B : %02X\n", getCounterB());
	msg("                Latch B : %02X\n", getLatchB());
	msg("            Data port B : %02X\n", getDataPortB());
	msg("  Data port direction B : %02X\n", getDataPortDirectionB());
	msg("     Control register B : %02X\n", getControlRegB());
	msg("     Timer B interrupts : %s\n", isInterruptEnabledB() ? "enabled" : "disabled");	
	msg("\n");
	msg("  Interrupt control reg : %02X\n", ICR);
	msg("     Interrupt mask reg : %02X\n", IMR);
	msg("\n");	
	tod.dumpState();
}

void CIA::executeOneCycle()
{
    wakeUp();
    
    uint64_t oldDelay = delay;
    uint64_t oldFeed  = feed;

    //
	// Layout of timer (A and B)
	//

    // Source: "A Software Model of the CIA6526" by Wolfgang Lorenz
	//
    //                           Phi2            Phi2                  Phi2
	//                            |               |                     |
	// timerA      -----    ------v------   ------v------     ----------v---------
	// input  ---->| & |--->| dwDelay & |-X-| dwDelay & |---->| decrement counter|
	//         --->|   |    |  CountA2  | | |  CountA3  |     |        (1)       |
	//         |   -----    ------------- | -------------     |                  |
	// -----------------          ^ Clr   |                   |                  |
	// | bCRA & 0x01   | Clr (3)  |       | ------------------| new counter = 0? |
	// | timer A start |<----     |       | |                 |                  |
	// -----------------    |     |       v v                 |                  |
 	//                    -----   |      -----                |      timer A     |
	//                    | & |   |      | & |                |  16 bit counter  |
	//                    |   |   |      |   |                |     and latch    |
	//                    -----   |      -----                |                  |
    //                     ^ ^    |        |(2)               |                  |
    //                     | |    ---------|-------------     |                  |
    //                     | |             |            |     |                  |
	// timer A             | |             |    -----   |     |                  |
	// output  <-----------|-X-------------X--->|>=1|---X---->| load from latch  |
	//                     |                --->|   |         |        (4)       |
	//                    -----             |   -----         --------------------
	//                    |>=1|             |
	//                    |   |             |       Phi2
	//                    -----             |        |
	//                     ^ ^              |  ------v------    ----------------
	//                     | |              ---| dwDelay & |<---| bcRA & 0x10  |
	//                     | ----------------  |  LoadA1   |    | force load   |
	//                     |       Phi2     |  -------------    ----------------
    //                     |        |       |                            ^ Clr
	// -----------------   |  ------v------ |                            |
	// | bCRA & 0x08   |   |  | dwDelay & | |                           Phi2
	// | one shot      |---X->| oneShotA0 |--
	// -----------------      -------------

				
	// Timer A

	// Decrement counter

	if (delay & CountA3)
		counterA--; // (1)
	
	// Check underflow condition
	bool timerAOutput = (counterA == 0 && (delay & CountA2)); // (2)
	
	if (timerAOutput) {
		
		// Stop timer in one shot mode
		if ((delay | feed) & OneShotA0) { // (3)
			CRA &= ~0x01;
			delay &= ~(CountA2 | CountA1 | CountA0);
			feed &= ~CountA0;
		}
		
		// Timer A output to timer B in cascade mode
		if ((CRB & 0x61) == 0x41 || ((CRB & 0x61) == 0x61 && CNT)) {
			delay |= CountB1;
		}
        
        // Reload counter immediately
		delay |= LoadA1;
	}
    
	// Load counter
	if (delay & LoadA1) // (4)
		reloadTimerA(); 
	
	// Timer B
	
	// Decrement counter
	if (delay & CountB3) {
		counterB--; // (1)
        // debug("Counter B down to %04X \n", counterB);
	}

	// Check underflow condition
	bool timerBOutput = (counterB == 0 && (delay & CountB2)); // (2)
	
	if (timerBOutput) {
						
		// Stop timer in one shot mode
		if ((delay | feed) & OneShotB0) { // (3)
			CRB &= ~0x01;
			delay &= ~(CountB2 | CountB1 | CountB0);
			feed &= ~CountB0;
		}
		delay |= LoadB1;
	}
	
	// Load counter
	if (delay & LoadB1) // (4)
		reloadTimerB();
		
    //
    // Serial register
    //
    
    // Generate clock signal
    if (timerAOutput && (CRA & 0x40) /* output mode */ ) {
        
        if (serCounter) {
            
            // Toggle serial clock signal
            feed ^= SerClk0;
            
        } else if (delay & SerLoad1) {
            
            // Load shift register
            delay &= ~(SerLoad1 | SerLoad0);
            feed &= ~SerLoad0;
            serCounter = 8;
            feed ^= SerClk0;
        }
    }
    
    // Run shift register with generated clock signal
    if (serCounter) {
        if ((delay & (SerClk2 | SerClk1)) == SerClk1) {      // Positive edge
            if (serCounter == 1) {
                delay |= SerInt0; // Trigger interrupt
            }
        }
        else if ((delay & (SerClk2 | SerClk1)) == SerClk2) { // Negative edge
            serCounter--;
        }
    }
	
	//
	// Timer output to PB6 (timer A) and PB7 (timer B)
    // 
	
	// Source: "A Software Model of the CIA6526" by Wolfgang Lorenz
	//
	//                     (7)            -----------------
    //         -------------------------->| bCRA & 0x04   |
    //         |                          | timer mode    |  ----------------
	//         |                          | 0x00: pulse   |->| 0x02 (timer) |
	// timerA  | Flip ---------------     |       (7)     |  |              |
    // output -X----->| bPB67Toggle |---->| 0x04: toggle  |  | bCRA & 0x02  |
	//            (5) |  ^ 0x40     |     |       (8)     |  | output mode  |-> PB6 out
	//                ---------------     -----------------  |              |
	//                       ^ Set        -----------------  | 0x00 (port)  |
	//                       |            | port B bit 6  |->|              |
	// ----------------- 0->1|            |    output     |  ----------------
	// | bCRA & 0x01   |------            -----------------
	// | timer A start |
	// -----------------

	// Timer A output to PB6
	
	if (timerAOutput) {
		
		PB67Toggle ^= 0x40; // (5) toggle underflow counter bit
		
		if (CRA & 0x02) { // (6)

			if ((CRA & 0x04) == 0) { 
				// (7) set PB6 high for one clock cycle
				PB67TimerOut |= 0x40;
				delay |= PB6Low0;
				delay &= ~PB6Low1;
			} else { 
				// (8) toggle PB6 (copy bit 6 from PB67Toggle)
				// PB67TimerOut = (PB67TimerOut & 0xBF) | (PB67Toggle & 0x40);
                PB67TimerOut ^= 0x40;
			}
		}
	}

	// Timer B output to PB7
	
	if (timerBOutput) {
		
		PB67Toggle ^= 0x80; // (5) toggle underflow counter bit
	
		if (CRB & 0x02) { // (6)
		
			if ((CRB & 0x04) == 0) {
				// (7) set PB7 high for one clock cycle
				PB67TimerOut |= 0x80;
				delay |= PB7Low0;
				delay &= ~PB7Low1;				
			} else {
				// (8) toggle PB7 (copy bit 7 from PB67Toggle)
				// PB67TimerOut = (PB67TimerOut & 0x7F) | (PB67Toggle & 0x80);
                PB67TimerOut ^= 0x80;
			}
		}
	}
	
	// Set PB67 back to low
	if (delay & PB6Low1) 
		PB67TimerOut &= ~0x40;

	if (delay & PB7Low1)
		PB67TimerOut &= ~0x80;

	
	// Write new PB 
	PB = ((PBLatch | ~DDRB) & ~PB67TimerMode) | (PB67TimerOut & PB67TimerMode);
	
	
	//
	// Interrupt logic
    //
	
	// Source: "A Software Model of the CIA6526" by Wolfgang Lorenz
	//
	//                      ----------
	//                      | bIMR & |----
	//                      |  0x01  |   |    -----
	//                      ----------   ---->| & |----
	// timerA       (9) Set ----------   ---->|   |   |
	// output  ------------>| bICR & |   |    -----   |
	//           ---------->|  0x01  |----            |  -----
	//           |      Clr ----------                -->|>=1|---
	//           |          ----------                -->|   |  |
	//           |          | bIMR & |----            |  -----  |
	//           |          |  0x02  |   |    -----   |         |
	//           |          ----------   ---->| & |----         |
	// timerB    | (10) Set ----------   ---->|   |             |
	// output  --|--------->| bICR & |   |    -----             |
	//           X--------->|  0x01  |----                      |
	//           |      Clr ----------       	                |
	// read      |                                              |
	// ICR ------X---------------X-------------------           |
	//                           | (12)             |           |
	//                           v Clr              v Clr       |
	//           ------      ----------      ----------------   | (11)
	// Int    <--| -1 |<-----| bICR & |<-----|   dwDelay &  |<---
	// ouptput   |    |      |  0x80  | Set  |  Interrupt1  |
	// (14)      ------      ---------- (13) -------^--------
	//                                              |
	//                                             Phi2
    
	if (timerAOutput) { // (9)
		// On a real C64, there is a race condition here. If ICR is currently read, 
		// the read access occurs *before* timer A sets bit 1. Hence, bit 1 always shows up.
		ICR |= 0x01;
	}
	
	if (timerBOutput && !(delay & ReadIcr0)) { // (10)
		// On a real C64, there is a race condition here. If ICR is currently read, 
		// the read access occurs *after* timer B sets bit 2. Hence, bit 2 won't show up.
		ICR |= 0x02;
	}
    
    // Check for timer interrupt
    if ((timerAOutput && (IMR & 0x01)) || (timerBOutput && (IMR & 0x02))) { // (11)
		delay |= Interrupt0;
        delay |= SetIcr0;
    }

    // Check for TOD interrupt
    if (delay & TODInt0) {
        ICR |= 0x04;
        if (IMR & 0x04) {
            delay |= Interrupt0;
            delay |= SetIcr0;
        }
    }
    
    // Check for Serial interrupt
    if (delay & SerInt2) {
        ICR |= 0x08;
        if (IMR & 0x08) {
            delay |= Interrupt0;
            delay |= SetIcr0;
        }
    }
    
    if (delay & (ClearIcr1 | SetIcr1 | Interrupt1)) {
        
        if (delay & ClearIcr1) { // (12)
            ICR &= 0x7F;
        }
        if (delay & SetIcr1) { // (13)
            ICR |= 0x80;
        }
        if (delay & Interrupt1) { // (14)
            INT = 0;
            pullDownInterruptLine();
        }
    }
    
	// Move delay flags left and feed in new bits
	delay = ((delay << 1) & DelayMask) | feed;
    
    // Go into idle state if possible
    if (oldDelay == delay && oldFeed == feed) {
        if (++tiredness > 8) {
            sleep();
            tiredness = 0;
        }
    } else {
        tiredness = 0;
    }
}

void
CIA::sleep()
{
    assert(idleCounter() == 0);
    
    // Determine maximum possible sleep cycles based on timer counts
    uint64_t sleepA = (counterA > 2) ? (c64->cycle + counterA - 1) : 0;
    uint64_t sleepB = (counterB > 2) ? (c64->cycle + counterB - 1) : 0;
    
    // CIAs with stopped timers can sleep forever
    if (!(feed & CountA0)) sleepA = UINT64_MAX;
    if (!(feed & CountB0)) sleepB = UINT64_MAX;
    
    setWakeUpCycle(MIN(sleepA, sleepB));
}

void
CIA::wakeUp()
{
    uint64_t idleCycles = idleCounter();
    
    // Make up for missed cycles
    if (idleCycles) {
        if (feed & CountA0) {
            assert(counterA >= idleCycles);
            counterA -= idleCycles;
        }
        if (feed & CountB0) {
            assert(counterB >= idleCycles);
            counterB -= idleCycles;
        }
        resetIdleCounter();
    }
    setWakeUpCycle(0);
}


// -----------------------------------------------------------------------------------------
// Complex Interface Adapter 1
// -----------------------------------------------------------------------------------------

CIA1::CIA1()
{
    setDescription("CIA1");
	debug(3, "  Creating CIA1 at address %p...\n", this);
}

CIA1::~CIA1()
{
	debug(3, "  Releasing CIA1\n");
}

void 
CIA1::dumpState()
{
	msg("CIA 1:\n");
	msg("------\n\n");
	CIA::dumpState();
}

void 
CIA1::pullDownInterruptLine()
{
    c64->cpu.pullDownIrqLine(CPU::CIA);
}

void 
CIA1::releaseInterruptLine()
{
    c64->cpu.releaseIrqLine(CPU::CIA);
}

uint8_t 
CIA1::peekDataPortA()
{
    uint8_t result = PA;
    uint8_t rows = PB & c64->joystickA.bitmask();
    uint8_t columnBits = c64->keyboard.getColumnValues(rows);
    
    // Clear joystick and keyboard bits
    result &= c64->joystickB.bitmask();
    result &= columnBits;
    
    return result;
}

uint8_t
CIA1::peekDataPortB()
{
    uint8_t result = PB;
    uint8_t columns = PA & c64->joystickB.bitmask();
    uint8_t rowBits = c64->keyboard.getRowValues(columns);
    
    // Clear joystick and keyboard bits
    result &= c64->joystickA.bitmask();
    result &= rowBits;
    
    return result;
}

void 
CIA1::pokeDataPortA(uint8_t value)
{
    PALatch = value;
    PA = PALatch | ~DDRA;
}

void
CIA1::pokeDataPortB(uint8_t value)
{
    uint8_t PBold = PB;
    
    PBLatch = value;
    PB = ((PBLatch | ~DDRB) & ~PB67TimerMode) | (PB67TimerOut & PB67TimerMode);
    
    if ((PBold & 0x10) != (PB & 0x10)) { // edge on lightpen bit?
        c64->vic.triggerLightPenInterrupt();
    }
}

void
CIA1::pokeDataPortDirectionA(uint8_t value)
{
    DDRA = value;
    PA = PALatch | ~DDRA;
}

void
CIA1::pokeDataPortDirectionB(uint8_t value)
{
    uint8_t PBold = PB;
    
    DDRB = value;
    PB = ((PBLatch | ~DDRB) & ~PB67TimerMode) | (PB67TimerOut & PB67TimerMode);
    
    if ((PBold & 0x10) != (PB & 0x10)) { // edge on lightpen bit?
        c64->vic.triggerLightPenInterrupt();
    }
}

uint64_t CIA1::wakeUpCycle() { return c64->wakeUpCycleCIA1; }
void CIA1::setWakeUpCycle(uint64_t cycle) { c64->wakeUpCycleCIA1 = cycle; }
uint64_t CIA1::idleCounter() { return c64->idleCounterCIA1; }
void CIA1::resetIdleCounter() { c64->idleCounterCIA1 = 0; }


// -----------------------------------------------------------------------------------------
// Complex Interface Adapter 2
// -----------------------------------------------------------------------------------------

CIA2::CIA2()
{
    setDescription("CIA2");
	debug(3, "  Creating CIA2 at address %p...\n", this);
}

CIA2::~CIA2()
{
	debug(3, "  Releasing CIA2...\n");
}

void 
CIA2::dumpState()
{
	msg("CIA 2:\n");
	msg("------\n\n");
	CIA::dumpState();
}

void 
CIA2::pullDownInterruptLine()
{
    c64->cpu.pullDownNmiLine(CPU::CIA);
}

void 
CIA2::releaseInterruptLine()
{
    c64->cpu.releaseNmiLine(CPU::CIA);
}

uint8_t 
CIA2::peekDataPortA()
{
    uint8_t result = PA;
    
    // The two upper bits are connected to the clock line and the data line
    result &= 0x3F;
    result |= (c64->iec.getClockLine() ? 0x40 : 0x00);
    result |= (c64->iec.getDataLine() ? 0x80 : 0x00);
    
    // The external port lines can pull down any bit, even if it configured as output.
    // Note that bits 0 and 1 are not connected to the bus and determine the memory bank seen by the VIC chip
    // result &= (portLinesB | 0x03);
    
    return result;
}

uint8_t
CIA2::peekDataPortB()
{
    uint8_t result = PB;
    return result;
}

void 
CIA2::pokeDataPortA(uint8_t value)
{
    PALatch = value;
    PA = PALatch | ~DDRA;
    
    // Bits 0 and 1 determine the memory bank seen the VIC
    c64->vic.setMemoryBankAddr((~PA & 0x03) << 14);
    
    // Bits 3 to 5 of PA are connected to the IEC bus
    c64->iec.updateCiaPins(PALatch, DDRA);
}

void
CIA2::pokeDataPortB(uint8_t value)
{
    PBLatch = value;
    PB = ((PBLatch | ~DDRB) & ~PB67TimerMode) | (PB67TimerOut & PB67TimerMode);
    // oldPB = PB;
}

void
CIA2::pokeDataPortDirectionA(uint8_t value)
{
    DDRA = value;
    PA = PALatch | ~DDRA;
    
    // Bits 0 and 1 determine the memory bank seen the VIC
    c64->vic.setMemoryBankAddr((~PA & 0x03) << 14);
    
    // Bits 3 to 5 of PA are connected to the IEC bus
    c64->iec.updateCiaPins(PALatch, DDRA);
}

void
CIA2::pokeDataPortDirectionB(uint8_t value)
{
    DDRB = value;
    PB = ((PBLatch | ~DDRB) & ~PB67TimerMode) | (PB67TimerOut & PB67TimerMode);
    // oldPB = PB;
}

uint64_t CIA2::wakeUpCycle() { return c64->wakeUpCycleCIA2; }
void CIA2::setWakeUpCycle(uint64_t cycle) { c64->wakeUpCycleCIA2 = cycle; }
uint64_t CIA2::idleCounter() { return c64->idleCounterCIA2; }
void CIA2::resetIdleCounter() { c64->idleCounterCIA2 = 0; }
