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

#ifndef UmtsRlcAckHeader_H
#define UmtsRlcAckHeader_H

#include "ns3/header.h"

namespace ns3 {

class UmtsRlcAckHeader : public Header
{
public:
  UmtsRlcAckHeader ();
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;

  void SetDataControl (uint16_t value);
  uint16_t GetDataControl ();

  void SetCpt (uint16_t value);
  uint16_t GetCpt ();

  void SetAckNumber(uint16_t value);
  uint16_t GetAckNumber();

  void SetNackNumber(uint8_t value);
  uint8_t GetNackNumber();
  
    void SetLastFragment(uint8_t value);
  uint8_t GetLastFragment();
  
  void SetFirstFragment(uint8_t value);
  uint16_t GetFirstFragment();

 
private:
  uint16_t m_dataControl;
  uint16_t m_cpt;
  uint16_t m_ackNumber;
  uint16_t m_extension;
  uint8_t m_nackNumber;

  uint8_t m_lastFragment;
  uint8_t m_firstFragment;


};



}

#endif
