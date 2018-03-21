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
#ifndef END_POINTS_H
#define END_POINTS_H

#include <vector>

using namespace std;

namespace ns3 {

class losPoint;

struct visibilityPoint {
  losPoint* _location;
  bool _visibility;
};

class EndPoints
{
public:
  EndPoints ();
  ~EndPoints ();
  void InsertPoint (losPoint* location, bool visibility);
  losPoint* GetClosestPoint (double x, double y);
  bool GetVisibility (double x, double y);
  
private:
  typedef std::vector<visibilityPoint*> visibilityPoints;
  visibilityPoints m_visibilityPoints;
  visibilityPoints::const_iterator GetVisibilityPoint (double x, double y);
  
};

} // namespace ns3

#endif /* END_POINTS_H */

