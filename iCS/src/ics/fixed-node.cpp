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

namespace ics
{

// ===========================================================================
// member method definitions
// ===========================================================================
FixedNode::FixedNode(const stationID_t nodeId, float posX, float posY, set<string> rats) : ITetrisNode()
{
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
FixedNode::GetPositionX()
{
    return m_posX;
}

float
FixedNode::GetPositionY()
{
    return m_posY;
}

}