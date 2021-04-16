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
/// @file    Area2D.h
/// @author  Pasquale Cataldi (EURECOM)
/// @date    May 19, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

#ifndef AREA2D_H_
#define AREA2D_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

/*
 *
 */
namespace ics_types {

enum Area2DType               {ROADELEMENT, GEOMETRICSHAPE};

class Area2D {
public:
    Area2D() {};
    virtual ~Area2D() {};

    virtual Area2DType getArea2DType() const = 0;

protected:
    Area2DType area2DType;
};

}

#endif /* AREA2D_H_ */
