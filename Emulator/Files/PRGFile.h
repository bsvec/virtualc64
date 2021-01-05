// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "AnyCollection.h"

class PRGFile : public AnyCollection {

public:

    //
    // Class methods
    //
    
    static bool isCompatibleName(const std::string &name);
    static bool isCompatibleStream(std::istream &stream);
    static bool isCompatibleBuffer(const u8 *buf, size_t len);
    static bool isCompatibleFile(const char *path);
    
    
    //
    // Constructing
    //
    
    static PRGFile *makeWithFileSystem(class FSDevice *fs, int item = 0);

    
    //
    // Initializing
    //
    
    PRGFile() : AnyCollection() { }
    PRGFile(size_t capacity) : AnyCollection(capacity) { }
    
    const char *getDescription() override { return "PRGFile"; }
    
    
    //
    // Methods from AnyFile
    //

    FileType type() override { return FILETYPE_PRG; }
    bool matchingBuffer(const u8 *buf, size_t len) override;
    bool matchingFile(const char *path) override;
    
    
    //
    // Methods from AnyCollection
    //

    PETName<16> collectionName() override;
    u64 collectionCount() override;
    PETName<16> itemName(unsigned nr) override;
    u64 itemSize(unsigned nr) override;
    u8 readByte(unsigned nr, u64 pos) override;
};
