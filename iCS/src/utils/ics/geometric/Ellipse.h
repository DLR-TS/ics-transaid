/****************************************************************************/
/// @file    Ellipse.h
/// @author  Pasquale Cataldi (EURECOM)
/// @date    May 6, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

#ifndef ELLIPSE_H_
#define ELLIPSE_H_

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
#include "Rectangle.h"
#include "Circle.h"

namespace ics_types
{

class Ellipse: public GeometricShape
{
public:
    Ellipse();
    Ellipse(Point2D focus1, Point2D focus2, float eccentricity);
    Ellipse(Point2D center, float majorAxis, float minorAxis, float rotationAngleRadians);
    virtual ~Ellipse();

    void        initEllipse(Point2D focus1, Point2D focus2, float eccentricity);
    void        initEllipse(Point2D center, float majorAxis, float minorAxis, float rotationAngleRadians);

    float       getMinorAxis() const;
    float       getMajorAxis() const;
    float       getEccentricity() const;
    Point2D     getFocus1() const;
    Point2D     getFocus2() const;
    Point2D     getCenter() const;
    float       getOrientationAngle() const;
    bool        isInternal(Point2D pos) const;
    float       getArea() const;
    ShapeType   getShapeType() const;
    Area2DType  getArea2DType() const;

    Rectangle   getCircumscribedRectangle();
    Circle      getCircumscribedCircle();

private:
    Point2D     focus1;
    Point2D     focus2;
    Point2D     center;
    float       eccentricity;
    float       majorAxis;
    float       minorAxis;
    float       angle;      // angle in radians from the x axis
};

}

#endif /* ELLIPSE_H_ */

//http://it.wikipedia.org/wiki/Ellisse
