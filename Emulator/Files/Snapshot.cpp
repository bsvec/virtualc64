// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

const u8 Snapshot::magicBytes[] = { 'V', 'C', '6', '4' };

bool
Snapshot::isCompatibleName(const std::string &name)
{
    return true; // name == "VC64"; 
}

bool
Snapshot::isCompatibleStream(std::istream &stream)
{
    if (streamLength(stream) < 0x15) return false; 
    return matchingStreamHeader(stream, magicBytes, sizeof(magicBytes));
}

Snapshot::Snapshot(size_t capacity)
{
    size = capacity + sizeof(SnapshotHeader);
    data = new u8[size];
    
    SnapshotHeader *header = (SnapshotHeader *)data;
    header->magicBytes[0] = magicBytes[0];
    header->magicBytes[1] = magicBytes[1];
    header->magicBytes[2] = magicBytes[2];
    header->magicBytes[3] = magicBytes[3];
    header->major = V_MAJOR;
    header->minor = V_MINOR;
    header->subminor = V_SUBMINOR;
    header->timestamp = time(NULL);
}

Snapshot *
Snapshot::makeWithC64(C64 *c64)
{
    Snapshot *snapshot;
    
    snapshot = new Snapshot(c64->size());
    
    snapshot->takeScreenshot(c64);

    if (SNP_DEBUG) c64->dump();
    c64->save(snapshot->getData());
    
    return snapshot;
}

void
Snapshot::takeScreenshot(C64 *c64)
{
    SnapshotHeader *header = (SnapshotHeader *)data;
    
    unsigned xStart = FIRST_VISIBLE_PIXEL;
    unsigned yStart = FIRST_VISIBLE_LINE;
    header->screenshot.width = VISIBLE_PIXELS;
    header->screenshot.height = c64->vic.numVisibleRasterlines();
    
    u32 *source = (u32 *)c64->vic.stableEmuTexture();
    u32 *target = header->screenshot.screen;
    source += xStart + yStart * TEX_WIDTH;
    for (unsigned i = 0; i < header->screenshot.height; i++) {
        memcpy(target, source, header->screenshot.width * 4);
        target += header->screenshot.width;
        source += TEX_WIDTH;
    }
}

bool
Snapshot::isTooOld()
{    
    if (data[4] < V_MAJOR) return true; else if (data[4] > V_MAJOR) return false;
    if (data[5] < V_MINOR) return true; else if (data[5] > V_MINOR) return false;
    return data[6] < V_SUBMINOR;
}

bool
Snapshot::isTooNew()
{
    if (data[4] > V_MAJOR) return true; else if (data[4] < V_MAJOR) return false;
    if (data[5] > V_MINOR) return true; else if (data[5] < V_MINOR) return false;
    return data[6] > V_SUBMINOR;
}
