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
/// @file    fixed-node.cpp
/// @author  Julen Maneros
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "fixed-node.h"

// ===========================================================================
// used namespaces
// ===========================================================================
using namespace ics_types;
using namespace std;

namespace ics {

// ===========================================================================
// member method definitions
// ===========================================================================
FixedNode::FixedNode(const stationID_t nodeId, float posX, float posY, set<string> rats) : ITetrisNode() {
    m_icsId = nodeId;
    m_preAssignedIds.insert(m_icsId);
    m_tsId = "";
    m_posX = posX;
    m_posY = posY;
    m_rats = rats;
    m_type = staType_BASICRSU;
}

FixedNode::~FixedNode() {}

float
FixedNode::GetPositionX() {
    return m_posX;
}

float
FixedNode::GetPositionY() {
    return m_posY;
}

}