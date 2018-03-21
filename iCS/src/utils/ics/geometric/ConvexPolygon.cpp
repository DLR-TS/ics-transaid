/****************************************************************************/
/// @file    ConvexPolygon.cpp
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

#include "ConvexPolygon.h"
#include "Rectangle.h"

#include <algorithm>
#include <ostream>
#include <vector>
#include <cfloat>

namespace ics_types
{

typedef std::pair<float, Point2D> convexPolygon_pair;

bool    sort_pred(const convexPolygon_pair& left, const convexPolygon_pair& right);
float   computeTriangleArea(Point2D p0, Point2D p1, Point2D p2);

ConvexPolygon::ConvexPolygon()
{
    shapeType = CONVEXPOLIGON;
    area2DType = GEOMETRICSHAPE;
}

ConvexPolygon::ConvexPolygon(std::vector<Point2D> verts)
{
    shapeType = CONVEXPOLIGON;
    area2DType = GEOMETRICSHAPE;

    if (!initConvexPolygon(verts)) {
        std::cerr << "[facilities] WARNING: the polygon [ " ;
        for (unsigned int i=0; i < verts.size(); i++)
            std::cerr << "(" << verts[i] << ") ";
        std::cerr << " ] is not convex!" << std::endl;
        abort();
    }
    shapeType = CONVEXPOLIGON;
}

ConvexPolygon::~ConvexPolygon()
{

}

bool    ConvexPolygon::initConvexPolygon(std::vector<Point2D> verts)
{
    bool convexityFlag = true;

    if (verts.empty()) {
        std::cerr << "[facilities] ERROR: the vertices of the convex polygon are not set." << std::endl;
        abort();
    }

    if (!vertices.empty())
        vertices.clear();

#ifdef _CHECK_CONVEXITY
    // If the polygon is not convex
    // convexityFlag = false;
#endif

    std::vector<Point2D>::iterator itP;
    std::vector<convexPolygon_pair>::iterator it;
    std::vector<convexPolygon_pair> angles;

    for (itP=verts.begin(); itP!=verts.end(); ++itP)
        angles.push_back(convexPolygon_pair(itP->x(), *itP));

#ifdef _CHECK_CONVEXITY
    std::cout << "[facilities] Unordered vertices:" << std::endl;
    for (itP=verts.begin(); itP!=verts.end(); ++itP)
        std::cout << "(" << *itP << ")" << std::endl;
    std::cout << std::endl;
#endif

    std::sort(angles.begin(), angles.end(), sort_pred);


    for (it=angles.begin(); it!=angles.end(); ++it)
        it->first = (it->second).shiftedAtan((angles.begin())->second);

    std::sort(angles.begin()+1, angles.end(), sort_pred);

    for (it=angles.begin(); it!=angles.end(); ++it)
        vertices.push_back(it->second);

#ifdef _CHECK_CONVEXITY
    std::cout << "[facilities] Counterclockwise ordered vertices:" << std::endl;
    for (itP=vertices.begin(); itP!=vertices.end(); ++itP)
        std::cout << "(" << *itP << ")" << std::endl;
    std::cout << std::endl;
#endif

    angles.clear();

    return convexityFlag;
}

bool    ConvexPolygon::isInternal(Point2D pos) const
{
    bool internalFlag = true;
    std::vector<Point2D>::const_iterator it;
    std::vector<Point2D>::const_iterator itp1;
    for (it = vertices.begin(); it < vertices.end(); it++) {
        itp1 = it + 1;
        if (itp1 == vertices.end())
            itp1 = vertices.begin();

        internalFlag &= (computeTriangleArea(*it, *itp1 ,pos) >= 0);
    }
    return internalFlag;
}

// $ A = \frac{1}{2} \sum_{i = 0}^{n - 1}( x_i y_{i + 1} - x_{i + 1} y_i) $
float   ConvexPolygon::getArea() const
{
    float area = 0.0;
    std::vector<Point2D>::const_iterator it;
    std::vector<Point2D>::const_iterator itp1;
    for (it = vertices.begin(); it < vertices.end(); it++) {
        itp1 = it + 1;
        if (itp1 == vertices.end())
            itp1 = vertices.begin();

        area += (it->x() * itp1->y() - itp1->x() * it->y());
    }
    return (area/2.0);
}

ShapeType   ConvexPolygon::getShapeType() const
{
    return shapeType;
}

Area2DType  ConvexPolygon::getArea2DType() const
{
    return area2DType;
}

unsigned int ConvexPolygon::getNumberOfVertices() const
{
    return vertices.size();
}

Rectangle   ConvexPolygon::getCircumscribedRectangle()
{
    float minX = FLT_MAX;
    float minY = FLT_MAX;
    float maxX = FLT_MIN;
    float maxY = FLT_MIN;

    for (unsigned int j = 0; j < vertices.size(); j++) {
        if (vertices[j].x() < minX) minX = vertices[j].x();
        if (vertices[j].y() < minY) minY = vertices[j].y();
        if (vertices[j].x() > maxX) maxX = vertices[j].x();
        if (vertices[j].x() > maxY) maxY = vertices[j].y();
    }

    std::vector<Point2D> rectVerts;
    rectVerts.push_back(Point2D(minX,minY));
    rectVerts.push_back(Point2D(maxX,minY));
    rectVerts.push_back(Point2D(maxX,maxY));
    rectVerts.push_back(Point2D(minX,maxY));
    Rectangle rect(rectVerts);
    return rect;
}

Circle      ConvexPolygon::getCircumscribedCircle()
{
    Rectangle rect = getCircumscribedRectangle();
    std::vector<Point2D> rectVerts = rect.getVertices();
    Point2D rectCenter = rect.getCenter();
    float maxDistance = 0;
    for (unsigned int i = 0; i < vertices.size(); i++) {
        float currVert2rectCenterDistance = rectCenter.distanceTo(vertices[i]);
        if (currVert2rectCenterDistance > maxDistance)
            maxDistance = currVert2rectCenterDistance;
    }
    Circle circle(rectCenter, maxDistance);
    return circle;
}

void        ConvexPolygon::printVertices()
{
    for (unsigned int i=0; i<vertices.size(); i++)
        std::cout << "(" << vertices[i] << ") ";
}

const std::vector<Point2D> ConvexPolygon::getVertices() const
{
    return vertices;
}

// ================== Internal functions ========================

bool sort_pred(const convexPolygon_pair& left, const convexPolygon_pair& right)
{
    return left.first < right.first;
}

float computeTriangleArea(Point2D p0, Point2D p1, Point2D p2)
{
    return (p1.x() * p2.y() -
            p1.y() * p2.x() -
            p0.x() * p2.y() +
            p0.y() * p2.x() +
            p0.x() * p1.y() -
            p0.y() * p1.x());
}

}
