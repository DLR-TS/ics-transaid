/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, EURECOM, Uwicore Laboratory UMH, EU FP7 iTETRIS project
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
 * Author: Fatma Hrizi <fatma.hrizi@eurecom.fr>, Ramon Bauza <rbauza@umh.es>
 */

#include "ns3/itetris-mobility-model.h"

#include "ns3/node-list.h"
#include "ns3/node.h"


using namespace std;
namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (ItetrisMobilityModel);

TypeId
ItetrisMobilityModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ItetrisMobilityModel")
    .SetParent<MobilityModel> ()
    .SetGroupName ("Mobility")
    .AddConstructor<ItetrisMobilityModel> ()
  ;
  return tid;
}

ItetrisMobilityModel::ItetrisMobilityModel ()
{
}

void
ItetrisMobilityModel::SetPositionAndSpeed (const float &latitude, const float &longitude, const float &speed,
                                           const float & heading, const std::string &edgeid, const std::string &laneid)
{
  m_helper.Reset (latitude, longitude, speed, heading, edgeid, laneid);
  NotifyCourseChange ();
}

void ItetrisMobilityModel::SetPositionAndSpeed (const Vector &position, const float &speed, const float & heading, const std::string &edgeid, const std::string &laneid)
{
  m_helper.Reset (position, speed, heading, edgeid, laneid);
  NotifyCourseChange ();
//   cout << "Position Updated." << position << " " << speed <<" "<< heading << " " << edgeid << " " << laneid << endl;
//   string temp;
//   cin >> temp;
}

void
ItetrisMobilityModel::SetPosition (const float &latitude, const float &longitude)
{
  DoSetPosition (latitude, longitude);
}

uint32_t 
ItetrisMobilityModel::GetLongitude (void) const
{
  return m_helper.GetLongitude ();
}

uint32_t 
ItetrisMobilityModel::GetLatitude (void) const
{
  return m_helper.GetLatitude ();
}

uint32_t 
ItetrisMobilityModel::GetSpeed (void) const
{
  return m_helper.GetSpeed ();
}

uint16_t
ItetrisMobilityModel::GetHeading (void) const
{
  return m_helper.GetHeading ();
}

float 
ItetrisMobilityModel::GetLongitudeO (void) const
{
  return m_helper.GetLongitudeO ();
}

float 
ItetrisMobilityModel::GetLatitudeO (void) const
{
  return m_helper.GetLatitudeO ();
}

float 
ItetrisMobilityModel::GetSpeedO (void) const
{
  return m_helper.GetSpeedO ();
}

float 
ItetrisMobilityModel::GetHeadingO (void) const
{
  return m_helper.GetHeadingO ();
}

void 
ItetrisMobilityModel::DoSetPosition (const float &latitude, const float &longitude)
{
  m_helper.InitializePosition (latitude, longitude);
}

Vector
ItetrisMobilityModel::DoGetPosition (void) const
{
  //std::cout<<"ITETRIS MOBILITY MODEL!!!"<<std::endl;
  return (m_helper.GetGeocentricPosition ());
}

void 
ItetrisMobilityModel::DoSetPosition (const Vector &position)
{
  m_helper.InitializePosition (position); 
}

Vector
ItetrisMobilityModel::DoGetVelocity (void) const
{
  //std::cout<<"ITETRIS MOBILITY MODEL!!!"<<std::endl;
  return Vector(m_helper.GetSpeedO (),0,0);
}

std::vector<uint32_t> 
ItetrisMobilityModel::GetGeodesicPosition (void) const
{
  return m_helper.GetGeodesicPosition ();
}

std::vector<float> 
ItetrisMobilityModel::GetGeodesicPosition0 (void) const
{
  return m_helper.GetGeodesicPositionO  ();
}

std::string 
ItetrisMobilityModel::GetEdgeId (void) const
{
  return m_helper.GetEdgeId ();
}

std::string 
ItetrisMobilityModel::GetLaneId (void) const
{
  return m_helper.GetLaneId ();
}

} // namespace ns3
 
