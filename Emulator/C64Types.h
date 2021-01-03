// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef C64_TYPES_H
#define C64_TYPES_H

#include "Aliases.h"

#include "CartridgeTypes.h"
#include "CIATypes.h"
#include "PortTypes.h"
#include "CPUTypes.h"
#include "DiskTypes.h"
#include "DriveTypes.h"
#include "FileTypes.h"
#include "FSTypes.h"
#include "MemoryTypes.h"
#include "MessageQueueTypes.h"
#include "MouseTypes.h"
#include "SIDTypes.h"
#include "VICIITypes.h"

//
// Enumerations
//

enum_long(OPT)
{
    // VICII
    OPT_VIC_REVISION,
    OPT_PALETTE,
    OPT_GRAY_DOT_BUG,
    OPT_HIDE_SPRITES,
    OPT_DMA_DEBUG,
    OPT_DMA_CHANNEL_R,
    OPT_DMA_CHANNEL_I,
    OPT_DMA_CHANNEL_C,
    OPT_DMA_CHANNEL_G,
    OPT_DMA_CHANNEL_P,
    OPT_DMA_CHANNEL_S,
    OPT_DMA_COLOR_R,
    OPT_DMA_COLOR_I,
    OPT_DMA_COLOR_C,
    OPT_DMA_COLOR_G,
    OPT_DMA_COLOR_P,
    OPT_DMA_COLOR_S,
    OPT_DMA_DISPLAY_MODE,
    OPT_DMA_OPACITY,
    OPT_CUT_LAYERS,
    OPT_CUT_OPACITY,
    OPT_SS_COLLISIONS,
    OPT_SB_COLLISIONS,

    // Logic board
    OPT_GLUE_LOGIC,

    // CIA
    OPT_CIA_REVISION,
    OPT_TIMER_B_BUG,
    
    // SID
    OPT_SID_ENABLE,
    OPT_SID_ADDRESS,
    OPT_SID_REVISION,
    OPT_SID_FILTER,
    OPT_AUDPAN,
    OPT_AUDVOL,
    OPT_AUDVOLL,
    OPT_AUDVOLR,
    
    // Sound synthesis
    OPT_SID_ENGINE,
    OPT_SID_SAMPLING,
    
    // Memory
    OPT_RAM_PATTERN,
    
    // Drive
    OPT_DRIVE_TYPE,
    OPT_DRIVE_CONNECT,
    OPT_DRIVE_POWER_SWITCH,
    
    // Debugging
    OPT_DEBUGCART
};
typedef OPT Option;

inline bool isOption(long value)
{
    return (unsigned long)value <= OPT_DEBUGCART;
}

inline const char *OptionName(Option value)
{
    switch (value) {
            
        case OPT_VIC_REVISION:        return "VIC_REVISION";
        case OPT_PALETTE:             return "PALETTE";
        case OPT_GRAY_DOT_BUG:        return "GRAY_DOT_BUG";
        case OPT_HIDE_SPRITES:        return "HIDE_SPRITES";
        case OPT_DMA_DEBUG:           return "DMA_DEBUG";
        case OPT_DMA_CHANNEL_R:       return "DMA_CHANNEL_R";
        case OPT_DMA_CHANNEL_I:       return "DMA_CHANNEL_I";
        case OPT_DMA_CHANNEL_C:       return "DMA_CHANNEL_C";
        case OPT_DMA_CHANNEL_G:       return "DMA_CHANNEL_G";
        case OPT_DMA_CHANNEL_P:       return "DMA_CHANNEL_P";
        case OPT_DMA_CHANNEL_S:       return "DMA_CHANNEL_S";
        case OPT_DMA_COLOR_R:         return "DMA_COLOR_R";
        case OPT_DMA_COLOR_I:         return "DMA_COLOR_I";
        case OPT_DMA_COLOR_C:         return "DMA_COLOR_C";
        case OPT_DMA_COLOR_G:         return "DMA_COLOR_G";
        case OPT_DMA_COLOR_P:         return "DMA_COLOR_P";
        case OPT_DMA_COLOR_S:         return "DMA_COLOR_S";
        case OPT_DMA_DISPLAY_MODE:    return "DMA_DISPLAY_MODE";
        case OPT_DMA_OPACITY:         return "DMA_OPACITY";
        case OPT_CUT_LAYERS:          return "CUT_LAYERS";
        case OPT_CUT_OPACITY:         return "CUT_OPACITY";
        case OPT_SS_COLLISIONS:       return "SS_COLLISIONS";
        case OPT_SB_COLLISIONS:       return "SB_COLLISIONS";

        case OPT_GLUE_LOGIC:          return "GLUE_LOGIC";

        case OPT_CIA_REVISION:        return "CIA_REVISION";
        case OPT_TIMER_B_BUG:         return "TIMER_B_BUG";
            
        case OPT_SID_ENABLE:          return "SID_ENABLE";
        case OPT_SID_ADDRESS:         return "SID_ADDRESS";
        case OPT_SID_REVISION:        return "SID_REVISION";
        case OPT_SID_FILTER:          return "SID_FILTER";
        case OPT_AUDPAN:              return "AUDPAN";
        case OPT_AUDVOL:              return "AUDVOL";
        case OPT_AUDVOLL:             return "AUDVOLL";
        case OPT_AUDVOLR:             return "AUDVOLR";
            
        case OPT_SID_ENGINE:          return "SID_ENGINE";
        case OPT_SID_SAMPLING:        return "SID_SAMPLING";
            
        case OPT_RAM_PATTERN:         return "RAM_PATTERN";
            
        case OPT_DRIVE_TYPE:          return "DRIVE_TYPE";
        case OPT_DRIVE_CONNECT:       return "DRIVE_CONNECT";
        case OPT_DRIVE_POWER_SWITCH:  return "DRIVE_POWER_SWITCH";
            
        case OPT_DEBUGCART:           return "DEBUGCART";
    }
    return "???";
}

enum_long(C64_MODEL)
{
    C64_MODEL_PAL,
    C64_MODEL_PAL_II,
    C64_MODEL_PAL_OLD,
    C64_MODEL_NTSC,
    C64_MODEL_NTSC_II,
    C64_MODEL_NTSC_OLD,
    C64_MODEL_CUSTOM
};
typedef C64_MODEL C64Model;

inline bool isC64Model(long value) {
    return (unsigned long)value <= C64_MODEL_CUSTOM;
}

inline const char *C64ModelName(C64Model value)
{
    switch (value) {
            
        case C64_MODEL_PAL:       return "PAL";
        case C64_MODEL_PAL_II:    return "PAL_II";
        case C64_MODEL_PAL_OLD:   return "PAL_OLD";
        case C64_MODEL_NTSC:      return "NTSC";
        case C64_MODEL_NTSC_II:   return "NTSC_II";
        case C64_MODEL_NTSC_OLD:  return "NTSC_OLD";
        case C64_MODEL_CUSTOM:    return "CUSTOM";
    }
    return "???";
}

enum_long(ROM_TYPE)
{
    ROM_TYPE_BASIC,
    ROM_TYPE_CHAR,
    ROM_TYPE_KERNAL,
    ROM_TYPE_VC1541
};
typedef ROM_TYPE RomType;

inline bool isRomType(long value) {
    return (unsigned long)value <= ROM_TYPE_VC1541;
}

inline const char *RomTypeName(RomType value)
{
    switch (value) {
            
        case ROM_TYPE_BASIC:   return "BASIC";
        case ROM_TYPE_CHAR:    return "CHAR";
        case ROM_TYPE_KERNAL:  return "KERNAL";
        case ROM_TYPE_VC1541:  return "VC1541";
    }
    return "???";
}

enum_long(EMULATOR_STATE)
{
    EMULATOR_STATE_OFF,
    EMULATOR_STATE_PAUSED,
    EMULATOR_STATE_RUNNING
};
typedef EMULATOR_STATE EmulatorState;

inline bool isEmulatorState(long value) {
    return (unsigned long)value <=  EMULATOR_STATE_RUNNING;
}

inline const char *EmulatorStateName(EmulatorState value)
{
    switch (value) {
            
        case EMULATOR_STATE_OFF:      return "OFF";
        case EMULATOR_STATE_PAUSED:   return "PAUSED";
        case EMULATOR_STATE_RUNNING:  return "RUNNING";
    }
    return "???";
}

enum_long(INSPECTION_TARGET)
{
    INSPECTION_TARGET_NONE,
    INSPECTION_TARGET_CPU,
    INSPECTION_TARGET_MEM,
    INSPECTION_TARGET_CIA,
    INSPECTION_TARGET_VIC,
    INSPECTION_TARGET_SID
};
typedef INSPECTION_TARGET InspectionTarget;

inline bool isInspectionTarget(long value) {
    return (unsigned long)value <= INSPECTION_TARGET_SID;
}

inline const char *InspectionTargetName(InspectionTarget value)
{
    switch (value) {
            
        case INSPECTION_TARGET_NONE:  return "NONE";
        case INSPECTION_TARGET_CPU:   return "CPU";
        case INSPECTION_TARGET_MEM:   return "IMEM";
        case INSPECTION_TARGET_CIA:   return "CIA";
        case INSPECTION_TARGET_VIC:   return "VIC";
        case INSPECTION_TARGET_SID:   return "SID";
    }
    return "???";
}

typedef enum
{
    ERR_OK,
    ERR_ROM_MISSING,
    ERR_ROM_MEGA65_MISMATCH
}
RomErrorCode;

inline bool isRomErrorCode(long value) {
    return value >= ERR_OK && value <= ERR_ROM_MEGA65_MISMATCH;
}

enum_long(ERRORCode)
{
    ERROR_OK,
    ERROR_UNKNOWN,

    // Memory errors
    ERROR_OUT_OF_MEMORY,

    // File errors
    ERROR_FILE_NOT_FOUND,
    ERROR_INVALID_TYPE,
    ERROR_CANT_READ,
    ERROR_CANT_WRITE,

    // Snapshots
    ERROR_UNSUPPORTED_SNAPSHOT,

    // Cartridges
    ERROR_UNSUPPORTED_CRT,
    
    // File system errors
    ERROR_UNSUPPORTED,
    ERROR_WRONG_CAPACITY,
    ERROR_HAS_CYCLES,
    ERROR_CORRUPTED,
    ERROR_IMPORT_ERROR,

    // Block errros
    ERROR_EXPECTED,
    ERROR_EXPECTED_MIN,
    ERROR_EXPECTED_MAX,

    // Export errors
    ERROR_DIRECTORY_NOT_EMPTY,
    ERROR_CANNOT_CREATE_DIR,
    ERROR_CANNOT_CREATE_FILE
};
typedef ERRORCode ErrorCode;

inline bool isErrorCode(long value)
{
    return (unsigned long)value <= ERROR_CANNOT_CREATE_FILE;
}

inline const char *ErrorCodeName(ErrorCode value)
{
    switch (value) {
            
        case ERROR_OK:                    return "OK";
        case ERROR_UNKNOWN:               return "UNKNOWN";

        case ERROR_OUT_OF_MEMORY:         return "OUT_OF_MEMORY";

        case ERROR_FILE_NOT_FOUND:        return "FILE_NOT_FOUND";
        case ERROR_INVALID_TYPE:          return "INVALID_TYPE";
        case ERROR_CANT_READ:             return "CANT_READ";
        case ERROR_CANT_WRITE:            return "CANT_WRITE";

        case ERROR_UNSUPPORTED_SNAPSHOT:  return "UNSUPPORTED_SNAPSHOT";
            
        case ERROR_UNSUPPORTED_CRT:       return "UNSUPPORTED_CRT";

        case ERROR_UNSUPPORTED:           return "UNSUPPORTED";
        case ERROR_WRONG_CAPACITY:        return "WRONG_CAPACITY";
        case ERROR_HAS_CYCLES:            return "HAS_CYCLES";
        case ERROR_CORRUPTED:             return "CORRUPTED";
        case ERROR_IMPORT_ERROR:          return "IMPORT_ERROR";

        case ERROR_DIRECTORY_NOT_EMPTY:   return "DIRECTORY_NOT_EMPTY";
        case ERROR_CANNOT_CREATE_DIR:     return "CANNOT_CREATE_DIR";
        case ERROR_CANNOT_CREATE_FILE:    return "CANNOT_CREATE_FILE";

        case ERROR_EXPECTED:              return "EXPECTED";
        case ERROR_EXPECTED_MIN:          return "EXPECTED_MIN";
        case ERROR_EXPECTED_MAX:          return "EXPECTED_MAX";
    }
    return "???";
}

//
// Structures
//

typedef struct
{
    VICConfig vic;
    CIAConfig cia1;
    CIAConfig cia2;
    SIDConfig sid;
    MemConfig mem;
}
C64Configuration;

typedef struct
{
    VICRevision vic;
    bool grayDotBug;
    CIARevision cia;
    bool timerBBug;
    SIDRevision sid;
    bool sidFilter;
    GlueLogic glue;
    RamPattern pattern;
}
C64ConfigurationDeprecated;

// Configurations of standard C64 models
static const C64ConfigurationDeprecated configurations[] = {
    
    // C64 PAL
    { VICREV_PAL_6569_R3, false, MOS_6526, true, MOS_6581, true, GLUE_LOGIC_DISCRETE, RAM_PATTERN_C64 },
    
    // C64_II_PAL
    { VICREV_PAL_8565, true, MOS_8521, false, MOS_8580, true, GLUE_LOGIC_IC, RAM_PATTERN_C64C },
    
    // C64_OLD_PAL
    { VICREV_PAL_6569_R1, false, MOS_6526, true, MOS_6581, true, GLUE_LOGIC_DISCRETE, RAM_PATTERN_C64 },

    // C64_NTSC
    { VICREV_NTSC_6567, false, MOS_6526, false, MOS_6581, true, GLUE_LOGIC_DISCRETE, RAM_PATTERN_C64 },

    // C64_II_NTSC
    { VICREV_NTSC_8562, true, MOS_8521, true, MOS_8580, true, GLUE_LOGIC_IC, RAM_PATTERN_C64C },
    
    // C64_OLD_NTSC
    { VICREV_NTSC_6567_R56A, false, MOS_6526, false, MOS_6581, true, GLUE_LOGIC_DISCRETE, RAM_PATTERN_C64 }
};

#endif
