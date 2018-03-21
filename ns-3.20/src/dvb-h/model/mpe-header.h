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

#ifndef MpeHeader_H
#define MpeHeader_H

#include "ns3/header.h"

namespace ns3 {

class MpeHeader : public Header
{
public:
  MpeHeader ();
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;

  void SetTableId (uint8_t id);
  uint8_t GetTableId ();

  void SetSectionSyntaxIndicator (uint16_t indicator);
  uint16_t GetSectionSyntaxIndicator ();

  void SetSectionLength (uint16_t length);
  uint16_t GetSectionLength ();

  void SetMac6 (uint8_t mac);
  uint8_t GetMac6 ();

  void SetMac5 (uint8_t mac);
  uint8_t GetMac5 ();

  void SetPayloadScramblingControl (uint8_t value);
  uint8_t GetPayloadScramblingControl ();

  void SetAddressScramblingControl (uint8_t value);
  uint8_t GetAddressScramblingControl ();

  void SetSnapFlag (uint8_t flag);
  uint8_t GetSnapFlag ();

  void SetCurrentNextIndicator (uint8_t next);
  uint8_t GetCurrentNextIndicator ();

  void SetSectionNumber (uint8_t number);
  uint8_t GetSectionNumber (void) const;

  void SetLastSectionNumber (uint8_t number);
  uint8_t GetLastSectionNumber (void) const;

  void SetMac4 (uint8_t mac);
  uint8_t GetMac4 ();

  void SetMac3 (uint8_t mac);
  uint8_t GetMac3 ();

  void SetMac2 (uint8_t mac);
  uint8_t GetMac2 ();

  void SetMac1 (uint8_t mac);
  uint8_t GetMac1 ();


private:
  uint8_t m_tableId;
  uint16_t m_section_syntax_indicator;
  uint16_t m_private;
  uint16_t m_reserved;
  uint16_t m_section_length;
  uint8_t m_mac_address6;
  uint8_t m_mac_address5;
  uint8_t m_reserved2;
  uint8_t m_payload_scrambling_control;
  uint8_t m_address_scrambling_control;
  uint8_t m_Llc_Snap_flag;
  uint8_t m_current_next_indicator;
  uint8_t m_section_number;
  uint8_t m_last_section_number;
  uint8_t m_mac_address4;
  uint8_t m_mac_address3;
  uint8_t m_mac_address2;
  uint8_t m_mac_address1;


};



}

#endif
