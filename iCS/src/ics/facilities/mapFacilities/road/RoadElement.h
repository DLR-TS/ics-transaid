/****************************************************************************/
/// @file    RoadElement.h
/// @author  Pasquale Cataldi (EURECOM)
/// @date    Apr 12, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

#ifndef ROADELEMENT_H_
#define ROADELEMENT_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "../../../../utils/ics/geometric/Area2D.h"
#include "../../../../utils/ics/iCStypes.h"
using namespace ics_types;

namespace ics_facilities {
// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RoadElement
 * @brief Represents an abstract class that can represent any road element.
*/
class RoadElement: public ics_types::Area2D {
public:

    /**
    * @brief Constructor.
    */
    RoadElement() {};

    /**
    * @brief Destructor.
    */
    virtual ~RoadElement() {};

    /**
    * @brief Returns the ID of the road element.
    */
    virtual roadElementID_t getID() const = 0;

    /**
    * @brief Returns the road element type.
    */
    virtual roadElementType getRoadElementType() const = 0;

    /**
    * @brief Returns ROADELEMENT.
    */
    virtual Area2DType getArea2DType() const = 0;

protected:

    /// @brief ID of the road element.
    roadElementID_t ID;

    /// @brief Type of the road element (LANE, EDGE, JUNCTION, TRAFFICLIGHT).
    roadElementType elementType;

    /// @brief Area type. For classes that inherits form RoadElement this variable must be ROADELEMENT.
    Area2DType area2DType;
};

} //namespace

#endif /* ROADELEMENT_H_ */
