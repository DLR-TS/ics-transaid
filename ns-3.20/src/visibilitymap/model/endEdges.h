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

#ifndef END_EDGES_H
#define END_EDGES_H

#include <map>
#include <string>

using namespace std;

namespace ns3 {

class EndPoints;

class EndEdges
{
public:
  EndEdges ();
  ~EndEdges ();
  void InsertEdge (string edgeId, EndPoints* endPoints);
  EndPoints* GetEndPoints (string edgeId);
  
private:
  typedef map<string,EndPoints*> edges;
  edges m_edges;
};

} // namespace ns3

#endif /* END_EDGES_H */
