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

#ifndef ITETRIS_HELPER_H
#define ITETRIS_HELPER_H

#include "ns3/nstime.h"
#include "ns3/vector.h"
#include <string.h>

namespace ns3 {

class Rectangle;

class itetrisPosHelper
{

 public:
  itetrisPosHelper ();
  itetrisPosHelper (const float &latitude, const float &longitude);
  itetrisPosHelper (const float &latitude, const float &longitude, const float &speed);

  void InitializePosition (const float &latitude, const float &longitude); 
  void InitializePosition (const Vector &position); // Set geocentric position (x,y,z)

  void Reset (const float &latitude, const float &longitude, const float &speed, const float & heading, const std::string &edgeid, const std::string &laneid);
  void Reset (const Vector &position, const float &speed, const float & heading, const std::string &edgeid, const std::string &laneid);

  Vector GetGeocentricPosition (void) const; // Get geocentric position (x,y,z)
  std::vector<uint32_t> GetGeodesicPosition (void) const; // Get geodesic position (lat, lon, h) in 1/10 micro degrees
  std::vector<float> GetGeodesicPositionO (void) const; // Get geodesic position (lat, lon, h)

  void SetLatitude (uint32_t latitude);
  uint32_t GetLatitude (void) const;

  void SetLongitude (uint32_t longitude);
  uint32_t GetLongitude (void) const;

  void SetSpeed (uint16_t speed);
  uint16_t GetSpeed (void) const;

  void SetHeading (uint16_t heading);
  uint16_t GetHeading (void) const;

  void SetLatitudeO (float latitude); 
  float GetLatitudeO (void) const;

  void SetLongitudeO (float longitude);
  float GetLongitudeO (void) const;

  void SetSpeedO (float speed);
  float GetSpeedO (void) const;

  void SetHeadingO (float heading);
  float GetHeadingO (void) const;

  std::string GetEdgeId (void) const;
  std::string GetLaneId (void) const;

 private:

  void UpdateGeodecentricPosition (void);
  void UpdateGeodedesicPosition (void);

  mutable Time m_lastUpdate;
  mutable Vector m_position; // Geocentric position (x,y,z)

  /* attibutes to be used for headers building following ETSI specs
   * Latitude and Longitude expressed in 1/10 micro degrees
   * Speed expressed in 1/100 m/s
   * Heading in 1/10 degrees
   */
  uint32_t m_latitude; // Latitude to be used in headers building
  uint32_t m_longitude;  // Longitude to be used in headers building
  uint16_t m_speed;  // Speed to be used in headers building
  uint16_t m_heading; // Heading to be used in headers building

  float m_latitudeO; // Original latitude retrieved from iCS 
  float m_longitudeO; // Original longitude retrieved from iCS
  float m_speedO;  // Original speed retrieved from iCS
  float m_headingO;  // Original heading retrieved from iCS

  std::string m_edgeId;
  std::string m_laneId;

};

} // namespace ns3

#endif /* ITETRIS_HELPER_H */
