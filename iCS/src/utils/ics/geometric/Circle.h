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

namespace ics_types
{

class Circle: public GeometricShape
{
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
