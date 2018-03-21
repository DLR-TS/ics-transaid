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

#ifndef UmtsRlcUmHeader_H
#define UmtsRlcUmHeader_H

#include "ns3/header.h"

namespace ns3 {

class UmtsRlcUmHeader : public Header
{
public:
  UmtsRlcUmHeader ();
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;

  void SetLastNumber (uint16_t value);
  uint16_t GetLastNumber();
 
  void SetResegmentation (uint16_t value);
  uint16_t GetResegmentation ();

  void SetSequenceNumber(uint16_t value);
  uint16_t GetSequenceNumber();

  void SetLengthIndicator(uint16_t value);
  uint16_t GetLengthIndicator();

    void SetLastFragment(uint8_t value);
  uint8_t GetLastFragment();
  
  void SetFirstFragment(uint8_t value);
  uint16_t GetFirstFragment();
  
private:
  
  uint16_t m_reserved;
  uint16_t m_lastNumber;
  uint16_t m_resegmentation;
  uint16_t m_sequenceNumber;
  uint8_t m_lengthIndicator;
  uint8_t m_extension2;
  
  uint8_t m_lastFragment;
  uint8_t m_firstFragment;
  


};



}

#endif
