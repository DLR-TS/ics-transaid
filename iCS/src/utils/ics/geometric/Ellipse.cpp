/****************************************************************************/
/// @file    Ellipse.cpp
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

#include "Ellipse.h"
#include <cmath>
using namespace std;

namespace ics_types
{

Ellipse::Ellipse()
{
    shapeType = ELLIPSE;
    area2DType = GEOMETRICSHAPE;
}

Ellipse::Ellipse(Point2D focus1, Point2D focus2, float eccentricity)
{
    shapeType = ELLIPSE;
    area2DType = GEOMETRICSHAPE;
    initEllipse(focus1, focus2, eccentricity);
}

Ellipse::Ellipse(Point2D center, float majorAxis, float minorAxis, float rotationAngleRadians)
{
    shapeType = ELLIPSE;
    area2DType = GEOMETRICSHAPE;
    initEllipse(center, majorAxis, minorAxis, rotationAngleRadians);
}

Ellipse::~Ellipse() { }


void        Ellipse::initEllipse(Point2D focus1, Point2D focus2, float eccentricity)
{
    this->focus1 = focus1;
    this->focus2 = focus2;
    this->eccentricity = eccentricity;

    center.set((focus1.x()+focus2.x())/2.0, (focus1.y()+focus2.y())/2.0);
    majorAxis = 2.0*center.distanceTo(focus1)/eccentricity;
    minorAxis = majorAxis * sqrt(1.0 - eccentricity * eccentricity);
    angle = atan((focus2.x()-focus1.x())/(focus1.y()-focus1.y()));
}

void        Ellipse::initEllipse(Point2D center, float majorAxis, float minorAxis, float rotationAngleRadians)
{
    this->center = center;
    this->majorAxis = majorAxis;
    this->minorAxis = minorAxis;
    eccentricity = sqrt(1.0 - (minorAxis * majorAxis));

    focus1.set(-eccentricity*majorAxis/2.0, 0.0);
    focus1.reshiftRotate(center.x(), center.y(), rotationAngleRadians);
    focus2.set(eccentricity*majorAxis/2.0, 0.0);
    focus2.reshiftRotate(center.x(), center.y(), rotationAngleRadians);
    angle = rotationAngleRadians;
}

Point2D     Ellipse::getCenter() const
{
    return center;
}

float       Ellipse::getOrientationAngle() const
{
    return angle;
}

bool        Ellipse::isInternal(Point2D pos) const
{
    return (pos.distanceTo(focus1) + pos.distanceTo(focus2) <= majorAxis);
}

float       Ellipse::getArea() const
{
    return majorAxis*minorAxis/4.0*M_PI;
}

ShapeType   Ellipse::getShapeType() const
{
    return shapeType;
}

Area2DType  Ellipse::getArea2DType() const
{
    return area2DType;
}

Rectangle   Ellipse::getCircumscribedRectangle()
{
    Point2D pointA(center.x() + majorAxis*cos(angle), center.y() + majorAxis*sin(angle));
    Point2D pointB(center.x() - majorAxis*cos(angle), center.y() - majorAxis*sin(angle));
    Rectangle rect(pointA, pointB, minorAxis);
    return rect;
}

Circle      Ellipse::getCircumscribedCircle()
{
    Circle circle(center, majorAxis);
    return circle;
}

}




