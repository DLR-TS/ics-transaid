/****************************************************************************/
/// @file    tmc.h
/// @author  Julen Maneros
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
#ifndef TMC_NODE_H
#define TMC_NODE_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "itetris-node.h"
namespace ics
{

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TmcNode
 * @brief Configures, executes and closes the iCS
 */
class TmcNode : public ITetrisNode
{
public:
    static TmcNode* GetInstance();
    ~TmcNode() {};

private:
    TmcNode();
    static TmcNode* tmcMode_;
};

}

#endif