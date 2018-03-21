/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
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
 * Author: Ramon Bauza <rbauza@umh.es>
 */

#include "losPoint.h"
#include <math.h>

namespace ns3 {

losPoint::losPoint (double _x, double _y)
  : x (_x),
    y (_y)
{}

losPoint::losPoint ()
  : x (0.0),
    y (0.0)
{}

double 
CalculateDistance (const losPoint &a, const losPoint &b)
{
  double dx = b.x - a.x;
  double dy = b.y - a.y;
  double distance = sqrt (dx * dx + dy * dy);
  return distance;
}

std::ostream &operator << (std::ostream &os, const losPoint &losPoint)
{
  os << "(" << losPoint.x << "," << losPoint.y << ")";
  return os;
}

bool 
losPoint::operator== (const losPoint &losPoint)
{
  if ((x == losPoint.x) && (y == losPoint.y))
    {
      return true;
    }
  return false;  
}

} // namespace ns3
