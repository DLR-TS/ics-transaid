/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
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
 * Author: Michele Rondinone <mrondinone@umh.es>
 */
#ifndef TSTEP_SEQUENCE_NUMBER_TAG_H
#define TSTEP_SEQUENCE_NUMBER_TAG_H

#include "ns3/packet.h"

namespace ns3 {

class Tag;

class TStepSequenceNumberTag : public Tag
{
public:
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  
  TStepSequenceNumberTag ();
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual uint32_t GetSerializedSize () const;
  virtual void Print (std::ostream &os) const;

  uint32_t Get (void) const;
  void Set (uint32_t TstepSN);

private:
  uint32_t m_TStepSequenceNumber;
};

} //namespace ns3

#endif /* TSTEP_SEQUENCE_NUMBER_TAG_H */
