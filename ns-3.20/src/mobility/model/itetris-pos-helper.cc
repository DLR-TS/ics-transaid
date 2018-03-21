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

#include "ns3/simulator.h"
#include "ns3/rectangle.h"
#include "itetris-pos-helper.h"
#include "ns3/geo-utils.h"

namespace ns3 {

itetrisPosHelper::itetrisPosHelper ()
{}

itetrisPosHelper::itetrisPosHelper (const float &latitude, const float &longitude)
{
  m_latitudeO = latitude;
  m_longitudeO = longitude;
  m_speedO = 0.0;
  m_headingO = 0.0;

  m_latitude = (uint32_t) (m_latitudeO * 1000000 * 10);
  m_longitude = (uint32_t) (m_longitudeO * 1000000 * 10);
  m_speed = (uint16_t) (m_speedO * 100);
  m_heading = (uint16_t) (m_headingO * 10);

  m_edgeId = "";
  m_laneId = "";

  UpdateGeodecentricPosition ();
}

itetrisPosHelper::itetrisPosHelper (const float &latitude, const float &longitude,
		                    const float &speed)
{
  m_latitudeO = latitude;
  m_longitudeO = longitude;
  m_speedO = speed;
  m_headingO = 0.0;

  m_latitude = (uint32_t) (m_latitudeO * 1000000 * 10);
  m_longitude = (uint32_t) (m_longitudeO * 1000000 * 10);
  m_speed = (uint16_t) (m_speedO * 100);
  m_heading = (uint16_t) (m_headingO * 10);

  m_edgeId = "";
  m_laneId = "";

  UpdateGeodecentricPosition ();
}

void 
itetrisPosHelper::InitializePosition (const float &latitude, const float &longitude)
{
  m_latitudeO = latitude;
  m_longitudeO = longitude;
  m_latitude = (uint32_t) (m_latitudeO * 1000000 * 10);
  m_longitude = (uint32_t) (m_longitudeO * 1000000 * 10);
  UpdateGeodecentricPosition ();
}

void
itetrisPosHelper::InitializePosition (const Vector &position)
{
  m_position = position;
  UpdateGeodedesicPosition ();
}

void  
itetrisPosHelper::Reset (const float &latitude, const float &longitude, const float &speed, const float &heading, const std::string &edgeid, const std::string &laneid)
{
  m_latitudeO = latitude;
  m_longitudeO = longitude;
  m_speedO = speed;
  m_headingO = heading;
  m_edgeId = edgeid;
  m_laneId = laneid;
  
  m_latitude = (uint32_t) (m_latitudeO * 1000000 * 10);
  m_longitude = (uint32_t) (m_longitudeO * 1000000 * 10);
  m_speed = (uint16_t) (m_speedO * 100);
  m_heading = (uint16_t) (m_headingO * 10);

  UpdateGeodecentricPosition ();
}

void 
itetrisPosHelper::Reset (const Vector &position, const float &speed, const float & heading, const std::string &edgeid, const std::string &laneid)
{
  m_position = position;
  m_speedO = speed;
  m_headingO = heading;
  m_edgeId = edgeid;
  m_laneId = laneid;

  UpdateGeodedesicPosition ();

  m_speed = (uint16_t) (m_speedO * 100);
  m_heading = (uint16_t) (m_headingO * 10);
}

void 
itetrisPosHelper::UpdateGeodecentricPosition (void)
{
  std::vector <double> geocentricPos = GeoToLocConvert (m_latitudeO, m_longitudeO, 0.0);
  m_position.x = geocentricPos[0];
  m_position.y = geocentricPos[1];
  m_position.z = geocentricPos[2];
}

void itetrisPosHelper::UpdateGeodedesicPosition (void)
{
  std::vector <double> geodesicPos = LocToGeoConvert (m_position.x, m_position.y, m_position.z);
  m_latitudeO = geodesicPos.at(0);
  m_longitudeO = geodesicPos.at(1);
  m_latitude = (uint32_t) (m_latitudeO * 1000000 * 10);
  m_longitude = (uint32_t) (m_longitudeO * 1000000 * 10);
}

void
itetrisPosHelper::SetLatitude (uint32_t latitude)
{
  m_latitude = latitude;
  m_latitudeO = (float) m_latitude / 10000000 / 10;
  UpdateGeodecentricPosition ();
}

uint32_t
itetrisPosHelper::GetLatitude (void) const
{
  return m_latitude;
}

void 
itetrisPosHelper::SetLongitude (uint32_t longitude)
{
  m_longitude = longitude;
  m_longitudeO = (float) m_longitude / 10000000 / 10;
  UpdateGeodecentricPosition ();
}

uint32_t
itetrisPosHelper::GetLongitude (void) const
{
  return m_longitude;
}

void
itetrisPosHelper::SetSpeed (const uint16_t speed)
{
  m_speed = speed;
  m_speedO = (float) m_speed / 100;
}

uint16_t 
itetrisPosHelper::GetSpeed (void) const
{
  return m_speed;
}

void
itetrisPosHelper::SetHeading (uint16_t heading)
{
  m_heading = heading;
  m_headingO = (float) m_heading / 10;
}

uint16_t 
itetrisPosHelper::GetHeading (void) const
{
  return m_heading;
}

void

itetrisPosHelper::SetLatitudeO (float latitude)
{
  m_latitudeO = latitude;
  m_latitude = (uint32_t) (m_latitudeO * 1000000 * 10);
  UpdateGeodecentricPosition ();
}


float
itetrisPosHelper::GetLatitudeO (void) const
{
  return m_latitudeO;
}

void 

itetrisPosHelper::SetLongitudeO (float longitude)
{
  m_longitudeO = longitude;
  m_longitude = (uint32_t) (m_longitudeO * 1000000 * 10);
  UpdateGeodecentricPosition ();
}


float
itetrisPosHelper::GetLongitudeO (void) const
{
  return m_longitudeO;
}

void 
itetrisPosHelper::SetSpeedO (float speed)
{
  m_speedO = speed;
  m_speed = (uint16_t) (m_speedO * 100);
}

float
itetrisPosHelper::GetSpeedO (void) const
{
  return m_speedO;
}

void 
itetrisPosHelper::SetHeadingO (float heading)
{
  m_headingO = heading;
  m_heading = (uint16_t) (m_headingO * 10);
}

float
itetrisPosHelper::GetHeadingO (void) const
{
  return m_headingO;
}

Vector 
itetrisPosHelper::GetGeocentricPosition (void) const
{
  return m_position;
}

std::vector<float> 
itetrisPosHelper::GetGeodesicPositionO (void) const
{
  std::vector<float> geodesicPos;
  geodesicPos.push_back(m_latitudeO);
  geodesicPos.push_back(m_longitudeO);
  return geodesicPos;
}

std::vector<uint32_t> 
itetrisPosHelper::GetGeodesicPosition (void) const
{
  std::vector<uint32_t> geodesicPos;
  geodesicPos.push_back(m_latitude);
  geodesicPos.push_back(m_longitude);
 return geodesicPos;
}

std::string 
itetrisPosHelper::GetEdgeId (void) const
{
  return m_edgeId;
}

std::string 
itetrisPosHelper::GetLaneId (void) const
{
  return m_laneId;
}

} // namespace ns3

