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

#include "c2cl4T-socket-factory-impl.h"
#include "c2c-transport.h"
#include "ns3/socketc2c.h"
#include "ns3/assert.h"

namespace ns3 {

c2cl4TSocketFactoryImpl::c2cl4TSocketFactoryImpl ()
  : m_c2cTransport(0)
{}
c2cl4TSocketFactoryImpl::~c2cl4TSocketFactoryImpl ()
{
  NS_ASSERT (m_c2cTransport == 0);
}

void 
c2cl4TSocketFactoryImpl::Setc2cTransport (Ptr<c2cTransport> c2cTransport)
{
  m_c2cTransport = c2cTransport;
}

Ptr<Socketc2c>
c2cl4TSocketFactoryImpl::CreateSocket (void)
{
  return m_c2cTransport->CreateSocket ();
}

void 
c2cl4TSocketFactoryImpl::DoDispose (void)
{
  m_c2cTransport = 0;
  c2cl4TSocketFactory::DoDispose ();
}

} // namespace ns3
