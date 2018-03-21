/****************************************************************************/
/// @file    GeometricShape.h
/// @author  Pasquale Cataldi (EURECOM)
/// @date    May 6, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

#ifndef GEOMETRICSHAPE_H_
#define GEOMETRICSHAPE_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "../geometric/Area2D.h"
#include "../iCStypes.h"

/*
 *
 */
namespace ics_types
{

class GeometricShape: public Area2D
{
public:
    GeometricShape() {};
    virtual ~GeometricShape() {};

    virtual bool        isInternal(Point2D Position) const = 0;
    virtual float       getArea() const = 0;
    virtual ShapeType   getShapeType() const = 0;
    virtual Area2DType  getArea2DType() const = 0;

protected:
    ShapeType           shapeType;
    Area2DType          area2DType;
};

}

#endif /* GEOMETRICSHAPE_H_ */
