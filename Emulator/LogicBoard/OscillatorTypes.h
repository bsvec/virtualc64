// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Aliases.h"
#include "Reflection.h"

//
// Enumerations
//

enum_long(GRID)
{
    GRID_STABLE_50HZ,
    GRID_UNSTABLE_50HZ,
    GRID_STABLE_60HZ,
    GRID_UNSTABLE_60HZ,
    GRID_COUNT
};
typedef GRID PowerGrid;

#ifdef __cplusplus
struct PowerGridEnum : util::Reflection<PowerGridEnum, PowerGrid> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < GRID_COUNT;
    }
    
    static const char *prefix() { return "GRID"; }
    static const char *key(PowerGrid value)
    {
        switch (value) {
                
            case GRID_STABLE_50HZ:    return "STABLE_50HZ";
            case GRID_UNSTABLE_50HZ:  return "UNSTABLE_50HZ";
            case GRID_STABLE_60HZ:    return "STABLE_60HZ";
            case GRID_UNSTABLE_60HZ:  return "UNSTABLE_60HZ";
            case GRID_COUNT:          return "???";
        }
        return "???";
    }
};
#endif


//
// Structures
//

typedef struct
{
    PowerGrid powerGrid;
}
OscillatorConfig;
