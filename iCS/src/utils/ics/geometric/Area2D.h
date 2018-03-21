/****************************************************************************/
/// @file    Area2D.h
/// @author  Pasquale Cataldi (EURECOM)
/// @date    May 19, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

#ifndef AREA2D_H_
#define AREA2D_H_

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
namespace ics_types
{

enum Area2DType               {ROADELEMENT, GEOMETRICSHAPE};

class Area2D
{
public:
    Area2D() {};
    virtual ~Area2D() {};

    virtual Area2DType getArea2DType() const = 0;

protected:
    Area2DType area2DType;
};

}

#endif /* AREA2D_H_ */
