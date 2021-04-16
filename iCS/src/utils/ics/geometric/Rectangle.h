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
/// @file    Rectangle.h
/// @author  Pasquale Cataldi (EURECOM)
/// @date    May 7, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

#ifndef RECTANGLE_H_
#define RECTANGLE_H_

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
#include "Circle.h"
#include "ConvexPolygon.h"

namespace ics_types {

class Rectangle: public ConvexPolygon {
public:
    Rectangle(std::vector<Point2D> verts);
    Rectangle(Point2D vertA, Point2D vertB, Point2D center);
    Rectangle(Point2D pointA, Point2D pointB, float height);
    virtual ~Rectangle();

    float   getArea() const;

    Point2D getCenter() const;

    Circle  getCircumscribedCircle();

private:
    Point2D center;
};

}

#endif /* RECTANGLE_H_ */
