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


#include "blerTable.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("BlerTable");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (BlerTable);

double BlerTable::table[2][8] = {
  { 6.31, 7.94, 10, 15.85, 25.12, 100, 2000, 1000000000},
  { 0.2, 0.15, 0.1, 0.05, 0.02, 0.001, 0.0001, 0.00001}
};



BlerTable::BlerTable ()
{
}

TypeId
BlerTable::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BlerTable")
    .SetParent<Object> ()

  ;
  return tid;
}

double BlerTable::getbler (double ebno)
{
  int i;
  double bler;
  bler = 1e-38;
  for (i = 7; i > -1; i--)
    {
      if (table[0][i] > ebno)
        {
          bler = table[1][i];
        }
      else
        {
          return bler;
        }
    }
  return bler;
}
}
