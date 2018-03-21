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

namespace ics_types
{

class Rectangle: public ConvexPolygon
{
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
