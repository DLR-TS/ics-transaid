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
/// @file    Circle.h
/// @author  Pasquale Cataldi (EURECOM)
/// @date    May 6, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/


#ifndef CIRCLE_H_
#define CIRCLE_H_

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

namespace ics_types {

class Circle: public GeometricShape {
public:
    Circle(); //Arantza
    Circle(Point2D center, float radius);
    virtual ~Circle();

    Point2D     getCenter() const;
    float       getRadius() const;
    bool        isInternal(Point2D pos) const;
    float       getArea() const;
    ShapeType   getShapeType() const;
    Area2DType  getArea2DType() const;
// Area2D* 	setArea(float area);  //Arantza

private:
    float   radius;
    Point2D center;
};

}

#endif /* CIRCLE_H_ */
