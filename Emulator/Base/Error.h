// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "ErrorTypes.h"
#include "Exception.h"

//
// VCError
//

struct VC64Error : public util::Exception
{
    VC64Error(ErrorCode code) : Exception((i64)code) { }
    VC64Error(ErrorCode code, const string &s) : Exception(s, (i64)code) { }

    const char *what() const throw() override;
    string describe() const;
};
