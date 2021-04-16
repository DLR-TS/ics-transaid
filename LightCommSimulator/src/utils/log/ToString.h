/*
 * This file is part of the iTETRIS Control System (https://github.com/DLR-TS/ics-transaid)
 * Copyright (c) 2008-2021 iCS development team and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
/****************************************************************************/
/// @file    ToString.h
/// @author  Christian Roessel
/// @date    Wed, 23 Sep 2002
/// @version $Id: ToString.h 8236 2010-02-10 11:16:41Z behrisch $
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef ToString_h
#define ToString_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include "config.h"
#endif

#include <sstream>
#include <string>
#include <iomanip>

#define OUTPUT_ACCURACY 2

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * Template for conversions from origin format to string representation
 * (when supplied by c++/the stl)
 */
template <class T>
inline std::string toString(const T& t, std::streamsize accuracy = OUTPUT_ACCURACY) {
    std::ostringstream oss;
    oss.setf(std::ios::fixed, std::ios::floatfield);
    oss << std::setprecision(accuracy);
    oss << t;
    return oss.str();
}

#endif

/****************************************************************************/

