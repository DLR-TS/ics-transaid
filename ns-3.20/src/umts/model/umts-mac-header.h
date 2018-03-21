/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, CBT, EU FP7 iTETRIS project
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
 * Author: Sendoa Vaz <svaz@cbt.es>
 */

#ifndef UmtsMacHeader_H
#define UmtsMacHeader_H

#include "ns3/header.h"

namespace ns3 {

enum ChannelCode{
  RCCCH=0x00,
  RDTCH=0x0100,
  RDCCH=0x0100,
  FCCCH=0x01000000,
  FDTCH=0x11,
  FDCCH=0x11
};


class UmtsMacHeader : public Header
{
public:
  UmtsMacHeader ();
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;

  void SetTctf (uint8_t value);
  uint8_t GetTctf ();
  

private:
  uint8_t m_tctf;
  uint8_t m_ct;
  uint8_t m_ueId;


};



}

#endif
