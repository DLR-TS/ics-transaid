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

#ifndef TsHeader_H
#define TsHeader_H

#include "ns3/header.h"

namespace ns3 {

class TsHeader : public Header
{
public:
  TsHeader ();
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;

  void SetErrorIndicator (uint16_t indicator);
  uint8_t GetErrorIndicator ();

  void SetPayloadUnitStart (uint16_t value);
  uint16_t GetPayloadUnitStart ();

  void SetPid (uint16_t pid);
  uint16_t GetPid ();

  void SetCounter (uint8_t count);
  uint8_t GetCounter ();


private:
  uint8_t m_synchronisationByte;
  uint16_t m_error_indicator;
  uint16_t m_payloadUnitStart;
  uint16_t m_priority;
  uint16_t m_pid;
  uint8_t m_scrambling_control;
  uint8_t m_adaptation_field_control;
  uint8_t m_counter;


};



}

#endif
