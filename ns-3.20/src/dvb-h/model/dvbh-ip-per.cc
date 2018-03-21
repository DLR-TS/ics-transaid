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


#include "dvbh-ip-per.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("DvbhIpPer");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DvbhIpPer);

double DvbhIpPer::QpskTable1_2 [2][11] = {
  { 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20},
  { 0.4, 0.36, 0.3, 0.25, 0.2, 0.15, 0.05, 0.012,0.0015,0.00015,0.0001}
};

double DvbhIpPer::QpskTable3_4 [2][11] = {
  { 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20},
  { 0.4, 0.25, 0.15, 0.07, 0.03, 0.015, 0.004, 0.0015,0.00065,0.00035,0.00016}
};

double DvbhIpPer::QamTable1_2 [2][14] = {
  { 10.7, 11, 12, 13, 14, 15, 16, 17,18,19,20,21,22,23},
  { 0.8, 0.75, 0.6, 0.5, 0.3, 0.2, 0.1, 0.05,0.02,0.006,0.0025,0.001,0.00025,0.0001}
};

double DvbhIpPer::QamTable3_4 [2][15] = {
  { 10, 11, 12, 13, 14, 15, 16, 17,18,19,20,21,22,23,24},
  { 0.8, 0.75, 0.71, 0.65, 0.6, 0.45, 0.3,0.17,0.09,0.05,0.022,0.0065,0.003,0.001,0.0003}
};



DvbhIpPer::DvbhIpPer()
{
}

TypeId
DvbhIpPer::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DvbhIpPer")
    .SetParent<Object> ()

  ;
  return tid;
}

double 
DvbhIpPer::GetIpPer (double ebno,uint8_t tableType)
{
  int i;
  double bler;
//   bler = 1e-38; 
  bler=0.0001;
     
  if(tableType==0)
  {
    for (i = 10; i > -1; i--)
    {
      if (QpskTable1_2[0][i] > ebno)
        {
          bler = QpskTable1_2[1][i];
        }
    }
  }
  else if(tableType==1)
  {
    for (i = 10; i > -1; i--)
    {
      if (QpskTable3_4[0][i] > ebno)
        {
          bler = QpskTable3_4[1][i];
        }
    }
  } 
  else if(tableType==2)
  {
    for (i = 13; i > -1; i--)
    {
      if (QamTable1_2[0][i] > ebno)
        {
          bler = QamTable1_2[1][i];
        }
    }
  } 
  else if(tableType==3)
  {
    for (i = 14; i > -1; i--)
    {
      if (QamTable3_4[0][i] > ebno)
        {
          bler = QamTable3_4[1][i];
        }
    }
  } 
  
 
  return bler;
}
}
