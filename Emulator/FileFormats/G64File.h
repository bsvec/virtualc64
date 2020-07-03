// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _G64FILE_INC
#define _G64FILE_INC

#include "AnyDisk.h"
#include "Disk.h"

/*! @class   G64File
 *  @brief   The G64File class declares the programmatic interface for a file
 *           in G64 format.
 */
class G64File : public AnyDisk {

private:

    //! @brief    Header signature
    static const u8 magicBytes[];
    
    /*! @brief    Number of the currently selected halftrack
     *  @details  0, if no halftrack is selected
     */
    Halftrack selectedHalftrack  = 0;
    
public:
    
    //
    //! @functiongroup Class methods
    //
    
    //! @brief    Returns true iff buffer contains a G64 file
    static bool isG64Buffer(const u8 *buffer, size_t length);
    
    //! @brief    Returns true iff the specified file is a G64 file
    static bool isG64File(const char *filename);
    
    
    //
    //! @functiongroup Creating and destructing G64 archives
    //
    
    //! @brief    Standard constructor
    G64File();

    //! @brief    Creates an empty G64 file with the specified capacity
    G64File(size_t capacity);

    //! @brief    Factory method
    static G64File *makeWithBuffer(const u8 *buffer, size_t length);
    
    //! @brief    Factory method
    static G64File *makeWithFile(const char *path);
    
    //! @brief    Factory method
    static G64File *makeWithDisk(Disk *disk);
    
    
    //
    //! @functiongroup Methods from AnyC64File
    //
    
    C64FileType type() { return G64_FILE; }
    const char *typeAsString() { return "G64"; }
    bool hasSameType(const char *filename) { return G64File::isG64File(filename); }
    
    
    //
    //! @functiongroup Methods from AnyArchive (API not supported by G64 format)
    //
    
    int numberOfItems() { assert(false); return 0; };
    size_t getSizeOfItem() { assert(false); return 0; }
    const char *getNameOfItem() { assert(false); return ""; }
    const char *getTypeOfItem() { assert(false); return ""; }
    u16 getDestinationAddrOfItem() { assert(false); return 0; }
    void selectItem(unsigned n) { assert(false); }
    u32 getStartOfItem(unsigned n) { assert(false); return 0; }

    
    //
    //! @functiongroup Methods from AnyDisk
    //
    
    int numberOfHalftracks() { return 84; }
    void selectHalftrack(Halftrack ht);
    size_t getSizeOfHalftrack();
    void seekHalftrack(long offset);
    
private:
    
    long getStartOfHalftrack(Halftrack ht);
};

#endif

