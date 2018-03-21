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

namespace ics_types
{

class Rectangle;

class ConvexPolygon: public GeometricShape
{
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
