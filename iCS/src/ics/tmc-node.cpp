/****************************************************************************/
/// @file    tmc.cpp
/// @author  Julen Maneros
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright � 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "tmc-node.h"
#include <stdio.h>

namespace ics
{

// ===========================================================================
// static member definitions
// ===========================================================================
TmcNode* TmcNode::tmcNode_ = NULL;

// ===========================================================================
// static method definitions
// ===========================================================================
TmcNode*
TmcNode::GetInstance()
{
    if (!tmcNode_)
        tmcNode_ = new TmcNode;

    return tmcNode_;
}

// ===========================================================================
// member method definitions
// ===========================================================================
TmcNode::TmcNode()
{
    m_icsId = 0;
    m_nsId = -1;
    m_tsId = "-1";
    m_type = ics_types::staType_SPECIAL;
}

}
