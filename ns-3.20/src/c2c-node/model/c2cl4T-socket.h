/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, HITACHI EUROPE SAS, EU FP7 iTETRIS project
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
 * Author: Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 */

#ifndef __C2CL4T_SOCKET_H__
#define __C2CL4T_SOCKET_H__

#include "socketc2c.h"
#include "ns3/traced-callback.h"
#include "ns3/callback.h"
#include "ns3/ptr.h"
#include "ns3/object.h"

namespace ns3 {

class Node;
class Packet;

/**
 * \ingroup socketc2c
 *
 * \brief  iTETRIS [WP600] - (abstract) base class of all c2cl4TSockets
 *
 * This class exists solely for hosting c2cl4TSocket attributes that can
 * be reused across different implementations
 *
 * @author Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 *
 */
class c2cl4TSocket : public Socketc2c
{
public:
 static TypeId GetTypeId (void);
 
 c2cl4TSocket (void);
  virtual ~c2cl4TSocket (void);

private:

  // Indirect the attribute setting and getting through private virtual methods
  virtual void SetRcvBufSize (uint32_t size) = 0;
  virtual uint32_t GetRcvBufSize (void) const = 0;
};

} //namespace ns3

#endif /* c2cl4T_SOCKET_H */


