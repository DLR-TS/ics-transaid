/****************************************************************************/
/// @file    Rectangle.cpp
/// @author  Pasquale Cataldi (EURECOM)
/// @date    May 7, 2010
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

#include "Rectangle.h"

#include <cstdlib>

namespace ics_types
{

Rectangle::Rectangle(std::vector<Point2D> verts) : ConvexPolygon(verts)
{
    center.set((vertices[0].x() + vertices[2].x())/2, (vertices[0].y() + vertices[2].y())/2);
    shapeType = RECTANGLE;
    area2DType = GEOMETRICSHAPE;
}

// A and B are contiguous vertices of the rectangle. center is the center of the rectangle.
Rectangle::Rectangle(Point2D vertA, Point2D vertB, Point2D center) : ConvexPolygon()
{
    float distanceA = center.distanceTo(vertA);
    float distanceB = center.distanceTo(vertB);

    if (((center.x() == (vertA.x()+vertB.x())/2) &&
            (center.y() == (vertA.y()+vertB.y())/2)) ||
            (distanceA != distanceB)) {
        std::cerr << "[facilities] ERROR: the points given do not define a rectangle!" << std::endl;
        abort();
    }

    this->center = center;

    std::vector<Point2D> verts(4);
    verts[0] = vertA;
    verts[1] = vertB;

    float alpha = atan((vertA.y() - center.y()) / (vertA.x() - center.x()));
    verts[2].set(distanceA, 0);
    verts[2].reshiftRotate(center.x(), center.y(), M_PI + alpha);

    float beta = atan((vertB.y() - center.y()) / (vertB.x() - center.x()));
    verts[3].set(distanceB, 0);
    verts[3].reshiftRotate(center.x(), center.y(), M_PI + beta);

    initConvexPolygon(verts);

    shapeType = RECTANGLE;
    area2DType = GEOMETRICSHAPE;
}

// A and B are the medium points on the small edges of the rectangle. height is the rectangle's height.
Rectangle::Rectangle(Point2D pointA, Point2D pointB, float height) : ConvexPolygon()
{
    float alpha = atan((pointB.y()-pointA.y())/(pointB.x()-pointA.x()));

    Point2D vertexQ(height/2,0);
    vertexQ.reshiftRotate(pointA.x(), pointA.y(), alpha + M_PI/2);
    Point2D vertexR(height/2,0);
    vertexR.reshiftRotate(pointA.x(), pointA.y(), alpha - M_PI/2);
    Point2D vertexS(height/2,0);
    vertexS.reshiftRotate(pointB.x(), pointB.y(), alpha - M_PI/2);
    Point2D vertexT(height/2,0);
    vertexT.reshiftRotate(pointB.x(), pointB.y(), alpha + M_PI/2);

    center.set((pointA.x()+pointB.x())/2, (pointA.y()+pointB.y())/2);

    std::vector<Point2D> verts;

    verts.push_back(vertexQ);
    verts.push_back(vertexR);
    verts.push_back(vertexS);
    verts.push_back(vertexT);

    initConvexPolygon(verts);

    shapeType = RECTANGLE;
    area2DType = GEOMETRICSHAPE;
}

Rectangle::~Rectangle()
{

}

float Rectangle::getArea() const
{
    return vertices[1].distanceTo(vertices[0]) * vertices[1].distanceTo(
               vertices[2]);
}

Point2D Rectangle::getCenter() const
{
    return center;
}

Circle  Rectangle::getCircumscribedCircle()
{
    return Circle(center, center.distanceTo(vertices[0]));
}

}
