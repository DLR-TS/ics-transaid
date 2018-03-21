/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 *                          University Miguel Hernandez, EU FP7 iTETRIS project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Ramon Bauza <rbauza@umh.es>
 */

#ifndef COMM_MODULE_INSTALLER_H
#define COMM_MODULE_INSTALLER_H

#include "ns3/simulator.h"
#include "ns3/node-container.h"
#include "ns3/object.h"
#include "ns3/itetris-types.h"


namespace ns3
{

/**
 * @class CommModuleInstaller
 * @brief The base class CommModuleInstaller can be derived to implement a specific installer which allows attach a communication module to a node.
 */
class CommModuleInstaller : public Object
{
  public:
    static TypeId GetTypeId (void);
    virtual void Install (NodeContainer container) = 0;
    virtual void Install (NodeContainer container, STACK stack){}; 
    virtual void Configure (std::string Filename) {};
    virtual void RelateInstaller (Ptr<CommModuleInstaller> installer) {};
    virtual ~CommModuleInstaller();
};

}

#endif





