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


#ifndef OVSF_TREES_H
#define OVSF_TREES_H

#include <stdint.h>
#include "ns3/packet.h"
#include "ns3/object.h"
#include "ns3/ipv4-address.h"

namespace ns3 {

struct node
{
  uint32_t source_;
  int sf_;
  int k_;
  int deep_;
};

class OvsfTree : public Object
{
public:
  node root;

  OvsfTree ();
  OvsfTree (int maxsf);
  void Dispose ();
  int check (void);
  int InsertSf (uint32_t source, int sf);
  int RemoveSf (uint32_t source, int sf);
  void BuildNodes (int sf, int k, int deep, int maxsf);
  void BuildTree (int maxsf);

private:
  int Insert (uint32_t source, int sf, int temp);
  node Search (uint32_t source, int sf, int temp);

  Ptr<OvsfTree> m_left;
  Ptr<OvsfTree> m_right;
  Ptr<OvsfTree> m_next;
  Ptr<OvsfTree> m_up;
};

}
#endif
