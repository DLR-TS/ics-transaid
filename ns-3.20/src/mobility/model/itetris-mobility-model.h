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

#ifndef ITETRIS_MOBILITY_MODEL_H
#define ITETRIS_MOBILITY_MODEL_H

#include <vector>
#include "mobility-model.h"
#include "ns3/ptr.h"
#include "ns3/itetris-pos-helper.h"

namespace ns3 {

class ItetrisMobilityModel : public MobilityModel
{
public:
  static TypeId GetTypeId (void);
  ItetrisMobilityModel ();
  void SetPositionAndSpeed (const float &latitude, const float &longitude, const float &speed, const float & heading, const std::string &edgeid, const std::string &laneid);
  void SetPositionAndSpeed (const Vector &position, const float &speed, const float & heading, const std::string &edgeid, const std::string &laneid); // Vector position in (x,y,z) coordinates
  void SetPosition (const float &latitude, const float &longitude);
  void SetGeodesicPosition (const std::vector<float> &position); // Set geodesic coordinates (lat, lot, h)

  std::vector<uint32_t> GetGeodesicPosition (void) const; // Get geodesic coordinates (lat, lot, h) in 1/10 micro degrees
  std::vector<float> GetGeodesicPosition0 (void) const; // Get geodesic coordinates (lat, lot, h)
  uint32_t GetLatitude (void) const;  //return latitude in 1/10 micro degrees
  uint32_t GetLongitude (void) const;  //return longitude in 1/10 micro degrees
  uint32_t GetSpeed (void) const;  //return speed in 1/100 m/s
  uint16_t GetHeading (void) const;  //return heading in 1/10 degrees
  float GetLatitudeO (void) const; 
  float GetLongitudeO (void) const; 
  float GetSpeedO (void) const;
  float GetHeadingO (void) const;
  std::string GetEdgeId (void) const;
  std::string GetLaneId (void) const;
 
private:
  virtual void DoSetPosition (const float &latitude, const float &longitude);
  virtual void DoSetPosition (const Vector &position); // Set geocentric coordinates (x,y,z)
  virtual Vector DoGetPosition (void) const; // Get geocentric coordinates (x,y,z)
  virtual Vector DoGetVelocity (void) const;
  itetrisPosHelper m_helper;

};

} // namespace ns3

#endif /* ITETRIS_MOBILITY_MODEL_H */
 
