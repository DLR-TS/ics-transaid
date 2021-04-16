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
/// @file    ConvexPolygon.h
/// @author  Pasquale Cataldi (EURECOM)
/// @date    May 7, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

#ifndef CONVEXPOLYGON_H_
#define CONVEXPOLYGON_H_

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
#include "GeometricShape.h"
#include "Circle.h"

#include <vector>

namespace ics_types {

class Rectangle;

class ConvexPolygon: public GeometricShape {
public:
    ConvexPolygon();
    ConvexPolygon(std::vector<Point2D> verts);
    virtual ~ConvexPolygon();

    bool        initConvexPolygon(std::vector<Point2D> verts);
    bool        isInternal(Point2D Position) const;
    float       getArea() const;
    ShapeType   getShapeType() const;
    Area2DType  getArea2DType() const;

    const std::vector<Point2D> getVertices() const;
    unsigned int getNumberOfVertices() const;

    Rectangle   getCircumscribedRectangle();
    Circle      getCircumscribedCircle();

    void    printVertices();

protected:
    std::vector<Point2D>    vertices;
    ShapeType               shapeType;
    Area2DType              area2DType;

};

}

#endif /* CONVEXPOLYGON_H_ */
