/****************************************************************************/
/// @file    Polygon.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Jun 2004
/// @version $Id: Polygon.h 13811 2013-05-01 20:31:43Z behrisch $
///
// A 2D- or 3D-polygon
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef Polygon_h
#define Polygon_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/geom/PositionVector.h>
#include <utils/common/Parameterised.h>
#include "Shape.h"

// we need to put this into a namespace to avoid clashing with wingdi.h::Polygon function
namespace SUMO {

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Polygon
 * @brief A 2D- or 3D-polygon
 */
class Polygon : public Shape, public Parameterised {
public:
    /** @brief Constructor
     * @param[in] id The name of the polygon
     * @param[in] type The (abstract) type of the polygon
     * @param[in] color The color of the polygon
     * @param[in] layer The layer of the polygon
     * @param[in] angle The rotation of the polygon
     * @param[in] imgFile The raster image of the polygon
     * @param[in] shape The shape of the polygon
     * @param[in] fill Whether the polygon shall be filled
     */
    Polygon(const std::string& id, const std::string& type,
            const RGBColor& color, const PositionVector& shape, bool fill,
            SUMOReal layer = DEFAULT_LAYER,
            SUMOReal angle = DEFAULT_ANGLE,
            const std::string& imgFile = DEFAULT_IMG_FILE);

    /// @brief Destructor
    virtual ~Polygon();


    /// @name Getter
    /// @{
    /** @brief Returns whether the shape of the polygon
     * @return The shape of the polygon
     */
    inline const PositionVector& getShape() const {
        return myShape;
    }

    /** @brief Returns whether the polygon is filled
     * @return Whether the polygon is filled
     */
    inline bool getFill() const {
        return myFill;
    }
    /// @}



    /// @name Setter
    /// @{
    /** @brief Sets whether the polygon shall be filled
     * @param[in] fill Whether the polygon shall be filled
     */
    inline void setFill(bool fill) {
        myFill = fill;
    }


    /** @brief Sets the shape of the polygon
     * @param[in] shape  The new shape of the polygon
     */
    inline virtual void setShape(const PositionVector& shape) {
        myShape = shape;
    }
    /// @}


protected:
    /// @brief The positions of the polygon
    PositionVector myShape;

    /// @brief Information whether the polygon has to be filled
    bool myFill;

};

}

#endif

/****************************************************************************/

