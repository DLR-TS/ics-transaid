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

#ifndef READ_VISIBILITY_MAP_H
#define READ_VISIBILITY_MAP_H

#include <string>
#include <fstream>

using namespace std;

namespace ns3 {

class RoadElementPoints;
class InitPoints;
class EndEdges;
class EndPoints;
class RoadElements;
class losPoint;
class VisibilityMap;

/**
 * @class ReadVisibilityMap
 * @brief Read from a file the visibility map that will be used by the winner-models/VisibilityMapModel
 */
class ReadVisibilityMap
{
public:
  ReadVisibilityMap (string filename);
  ~ReadVisibilityMap ();
  bool ReadXmlFile (void);
  VisibilityMap* GetVisibilityMap (void);
  RoadElements* GetRoadElements (void); 


private:
  RoadElementPoints* InsertPoints (std::ifstream &file, std::string &line,int &num_losPoints);
  InitPoints* InsertInitPoints (std::ifstream &file, std::string &line, std::string initEdgeId);
  EndEdges* InsertEndEdges (std::ifstream &file, std::string &line);
  EndPoints* InsertEndPoints (std::ifstream &file, std::string &line, std::string endEdgeId);
  losPoint* FindLocationForPointId (std::string pointId,std::string edgeId);
  double ReadDouble (std::string valueString) const;
  bool ReadBool (std::string valueString) const;

  string m_filename;
  RoadElements* m_roadElements;
  VisibilityMap* m_visibilityMap;
};

} // namespace ns3

#endif /* READ_VISIBILITY_MAP_H */
