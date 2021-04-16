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
/// @file    tmc.h
/// @author  Julen Maneros
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright � 2008 iTetris Project Consortium - All rights reserved
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
namespace ics {

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TmcNode
 * @brief Configures, executes and closes the iCS
 */
class TmcNode : public ITetrisNode {
public:
    static TmcNode* GetInstance();
    ~TmcNode() {};

private:
    TmcNode();
    static TmcNode* tmcNode_;
};

}

#endif