/****************************************************************************/
/// @file    Circle.cpp
/// @author  Pasquale Cataldi (EURECOM)
/// @date    May 6, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "Circle.h"
#include <cmath>
using namespace std;

namespace ics_types
{

Circle::Circle()
{
}

Circle::Circle(Point2D center, float radius)
{
    this->center = center;
    this->radius = radius;
    shapeType = CIRCLE;
    area2DType = GEOMETRICSHAPE;
}

Circle::~Circle() { }

Point2D     Circle::getCenter() const
{
    return center;
}

float       Circle::getRadius() const
{
    return radius;
}

bool        Circle::isInternal(Point2D pos) const
{
  // cerr << "[Geometrical Shape - Circle ] Pos X "<< (float) pos.x() << " Pos Y " << (float) pos.y() << " "  << endl; 
  // cerr << "[Geometrical Shape - Circle ] distance to center "<< (float) pos.distanceTo(center) << " radius " << (float) radius << " "  << endl; 
   return (pos.distanceTo(center) <= radius);
}

float       Circle::getArea() const
{
    return radius*radius*M_PI;
}

ShapeType   Circle::getShapeType() const
{
    return shapeType;
}

Area2DType  Circle::getArea2DType() const
{
    return area2DType;
}

// Area2D* Circle::setArea(float area)
// {
//   return (Area2D*)this;
// }

}
