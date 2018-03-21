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

#ifndef LOS_POINTS_H
#define LOS_POINTS_H

#include <ostream>

namespace ns3 {

class losPoint
{
public:
  losPoint (double _x, double _y);
  losPoint ();
  double x;
  double y;
  bool operator== (const losPoint &losPoint);
};

double CalculateDistance (const losPoint &a, const losPoint  &b);

std::ostream &operator << (std::ostream &os, const losPoint &losPoint);

} // namespace ns3

#endif /* ROAD_ELEMENTS_POINTS_H */


