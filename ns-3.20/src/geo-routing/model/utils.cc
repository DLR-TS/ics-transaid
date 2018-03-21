/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, EURECOM, EU FP7 iTETRIS project
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
 * Author: Fatma Hrizi <fatma.hrizi@eurecom.fr>
 */


/*
 *
 * Edited by Ramon Bauza
 *
 */

#include "utils.h"

static const double DEG_TO_RAD = 0.017453292519943295769236907684886;
/// @brief Earth's quatratic mean radius for WGS-84
static const double EARTH_RADIUS_IN_METERS = 6372797.560856;

namespace ns3
{

double CartesianDistance (double lat2, double lon2, double lat1, double lon1)
{
  double distance;
  distance = sqrt(((lat1-lat2)*(lat1-lat2))+((lon1-lon2)*(lon1-lon2)));
  return distance;
}

double ArcInRadians(uint32_t lat2, uint32_t lon2, uint32_t lat1, uint32_t lon1) {
        double latitudeArc  = (lat1 - lat2) * DEG_TO_RAD;
        double longitudeArc = (lon1 - lon2) * DEG_TO_RAD;
        double latitudeH = sin(latitudeArc * 0.5);
        latitudeH *= latitudeH;
        double lontitudeH = sin(longitudeArc * 0.5);
        lontitudeH *= lontitudeH;
        double tmp = cos(lat1*DEG_TO_RAD) * cos(lat2*DEG_TO_RAD);
        return 2.0 * asin(sqrt(latitudeH + tmp*lontitudeH));
}

double DistanceInMeters(uint32_t lat2, uint32_t lon2, uint32_t lat1, uint32_t lon1) {

        return EARTH_RADIUS_IN_METERS*ArcInRadians(lat2, lon2, lat1, lon1);
}

struct c2cCommonHeader::LongPositionVector getMinDistToDest(Ptr <LocationTable> ntable, Ptr<c2cAddress> daddr)
{
    struct c2cCommonHeader::LongPositionVector vector;
    if (ntable->m_Table.size() != 0)
    {
    double min = 1000000;

     for (uint32_t i = 0; i < (ntable->m_Table.size()); i++)
       {
        if (ntable->m_Table[i].is_neigh == true)
        {
        double distance = CartesianDistance (ntable->m_Table[i].Lat, ntable->m_Table[i].Long, daddr->GetGeoAreaPos1 ()->lat, daddr->GetGeoAreaPos1 ()->lon);

        if(distance < min)
        {
          min = distance;
          vector.gnAddr = ntable->m_Table[i].gnAddr;
          vector.Ts = ntable->m_Table[i].Ts;
          vector.Lat = ntable->m_Table[i].Lat;
          vector.Long = ntable->m_Table[i].Long;
          vector.Alt = ntable->m_Table[i].Alt;
          vector.PosAcc = ntable->m_Table[i].PosAcc;
          vector.AltAcc = ntable->m_Table[i].AltAcc;
          vector.Speed = ntable->m_Table[i].Speed;
          vector.Heading = ntable->m_Table[i].Heading;
          vector.SpeedAcc = ntable->m_Table[i].SpeedAcc;
          vector.HeadingAcc = ntable->m_Table[i].HeadingAcc;
        }
        }
       }
    }

  return vector;
}


void store (Ptr<const Packet> p)
{
}

Ptr<c2cAddress> DirectNeighbour (Ptr <LocationTable> ntable, Ptr<c2cAddress> daddr)
{
    bool isdirectneigh = false;
    Ptr<c2cAddress> result;

    if (ntable != 0)
    {
     for (LocationTable::Table::iterator i = ntable->m_Table.begin ();
       i != ntable->m_Table.end (); i++)
       {
         if (daddr->GetId() == i->gnAddr)
         {
            if (i->is_neigh == true)
            {
              isdirectneigh = true;
              result = CreateObject<c2cAddress> ();
              result->Set (i->gnAddr, i->Lat, i->Long);
            }
         }
       }
    }
    if (isdirectneigh == true)
    {
      return result;
    }
    else
    return 0;
}

}
