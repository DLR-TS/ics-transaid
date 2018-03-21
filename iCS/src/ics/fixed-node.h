/****************************************************************************/
/// @file    fixed-node.h
/// @author  Julen Maneros
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
#ifndef FIXEDNODE_H
#define FIXEDNODE_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <set>

#include "itetris-node.h"
#include "../utils/ics/iCStypes.h"

namespace ics
{

// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class FixedNode
* @brief Represents a fixed non-intelligent traffic element in the simulation.
*/
class FixedNode : public ITetrisNode
{

public:

    /**
    * @brief Constructor
    * @param[in] nodeId Node identifier
    * @param[in] posX The x coordenate of the node's position
    * @param[in] posY The y coordenate of the node's position
    * @param[in] rats The technologies the node with be equipped with
    */
    FixedNode(const ics_types::stationID_t nodeId, float posX, float posY, std::set<std::string> rats);

    /// @brief Destructor
    virtual ~FixedNode();


    /**
    * @brief Provides the x coordinate of the node's position
    * @return The x coordinate corresponding to the position of the node
    */
    float GetPositionX();

    /**
    * @brief Provides the y coordinate of the node's position
    * @return The y coordinate corresponding to the position of the node
    */
    float GetPositionY();

private:

    /// @brief The x coordinate of the node's position
    float m_posX;

    /// @brief The y coordinate of the node's position
    float m_posY;
};

}

#endif