/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 *                          University Miguel Hernandez,
 *                          EURECOM (www.eurecom.fr), EU FP7 iTETRIS project
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
 * Author:  Michele Rondinone <mrondinone@umh.es>, Jerome Haerri <Jerome.Haerri@eurecom.fr>
 */

/****************************************************************************************
 * Edited by Panagiotis Matzakos <matzakos@eurecom.fr>
 * EURECOM 2015
 * Added IPv6 support
***************************************************************************************/

/****************************************************************************************
 * Edited by Jin Yan (Renault) <jin.yan@renault.com>
 * RENAULT 2017
 * Added LTE support
***************************************************************************************/


#ifndef MW_COMM_CH_SELECTOR_H
#define MW_COMM_CH_SELECTOR_H

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/log.h"
#include "ns3/itetris-types.h"
#include "ns3/iTETRISNodeManager.h"
#include "ns3/mobility-model.h"
#include "ns3/node-container.h"
#include "ns3/umts-net-device.h"

namespace ns3 {


class MWCOMMchSelector : public Object
{

public:

  static TypeId GetTypeId (void);
  MWCOMMchSelector ();
  ~MWCOMMchSelector ();
  void DoDispose (void);

/* \brief retrieve a structure containing a pointer to the Communication Infrastructure Unit to be used to perform a 
 *        message dissemination along with the communication stack and the transmission mode 
 *           
 *        Implementation of the MW techno selector algorithm proposed by M. Wetterwald (EURECOM) in the iTETRIS D7.1.
 *        the algorithms specifies the mechanisms and the interfaces. The intelligence (which infrastructure node(s) 
 *        to select to cover the destination area the best way) is left to designers.
 *
 * \param commProfile the communication profile specified by the application
 * \param destination the circular geographical area to be covered by the application
 * \param technologies list of supported technologies, sorted by preferred technology (preferred comes first)
 * \return the dissemination profile (stack, technology and destination address)
 *
 * TODO select the best communicator (techno) not based on the priority list but on the number of stations reachable
 */
  disseminationProfile GetDisseminationProfile(uint32_t commProfile, CircularGeoAddress destination, TechnologyList technologies/*, STACK stack*/);

  disseminationProfile GetUnicastDisseminationProfile(uint32_t commProfile, uint32_t destinationId, TechnologyList technologies/*, STACK stack*/);

  void SetNode (Ptr<Node> node);
  void NotifyNewAggregate ();

private:

  Ptr<Node> m_node;

};

}; //namespace ns3

#endif /* MW_COMM_CH_SELECTOR_H */
