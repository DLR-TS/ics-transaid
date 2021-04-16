/*
 * This file is part of the iTETRIS Control System (https://github.com/DLR-TS/ics-transaid)
 * Copyright (c) 2008-2021 iCS development team and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
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

namespace ics {

// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class FixedNode
* @brief Represents a fixed non-intelligent traffic element in the simulation.
*/
class FixedNode : public ITetrisNode {

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