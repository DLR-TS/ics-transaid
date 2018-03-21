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

#include "readVisibilityMap.h"
#include <sstream>
#include <iostream>
#include "roadElements.h"
#include "visibilityMap.h"
#include "roadElementPoints.h"
#include "initPoints.h"
#include "endEdges.h"
#include "endPoints.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("ReadVisibilityMap");

namespace ns3 {

ReadVisibilityMap::ReadVisibilityMap (string filename)
  : m_filename (filename),
    m_roadElements (0),
    m_visibilityMap (0)
{}

ReadVisibilityMap::~ReadVisibilityMap ()
{}

RoadElements* 
ReadVisibilityMap::GetRoadElements (void)
{
  return m_roadElements;
}

VisibilityMap*
ReadVisibilityMap::GetVisibilityMap (void)
{
  return m_visibilityMap;
}

bool
ReadVisibilityMap::ReadXmlFile (void)
{
  int num_lines = 0;
  int num_losPoints = 0;
  int num_roadElements = 0;
 //std::cout << "Path: "<< m_filename << std::endl;
  std::ifstream file (m_filename.c_str (), std::ios::in);

  if (file.is_open())
    {

      m_roadElements = new RoadElements ();
      m_visibilityMap = new VisibilityMap ();
      std::string edgeId = "";

      while (!file.eof() )
        {
          std::string line;
          getline (file, line);
          std::string::size_type startTag = line.find_first_of ("<");
          std::string::size_type endTag = line.find_first_of (">");

          if (startTag == std::string::npos ||
              endTag == std::string::npos)
            {
              continue;
            }

          std::string::size_type spacePos = line.find_first_of (" ", startTag + 1);
	  std::string::size_type slashPos = line.find_first_of ("/", startTag + 1);
          
          std::string::size_type elementIdEnd = spacePos;
          bool closingTag = false;

          if (spacePos == std::string::npos)
            {
              elementIdEnd = slashPos;
              closingTag = true;
            }

          std::string element = line.substr (startTag + 1, elementIdEnd - startTag - 1);

          if ( (element == "edge") && (closingTag == false) )
            {
              edgeId = line.substr (elementIdEnd + 5, endTag - 1 - elementIdEnd - 5);
            }

          else if (element == "point")
                 {
                   if (edgeId != "")
                     {
                       RoadElementPoints* points = InsertPoints (file,line,num_losPoints);
                       m_roadElements->InsertEdge(edgeId,points);	 
                       edgeId = "";
                       num_roadElements = num_roadElements + 1;
                     }
                   else
                     {
                       NS_LOG_DEBUG ("*** Error in edgeId ***");
                       return false;
                     }
                     
                 }   
          else if ( (element == "initEdge") && (closingTag == false) )
            {
              edgeId = line.substr (elementIdEnd + 5, endTag - 1 - elementIdEnd - 5);
            }
	  else if ( (element == "initPoint") && (closingTag == false) )
            {
              NS_LOG_DEBUG ("initEdge " << edgeId);
              InitPoints* initPoints = InsertInitPoints (file,line,edgeId);
              m_visibilityMap->InsertEdge (edgeId, initPoints);
            }
	  else if (element == "endEdge")
            {
            }
	  else if (element == "endPoint")
            {
            }
    
          num_lines += 1;
         }
      file.close();

    }
  NS_LOG_DEBUG ("Num lines read " << num_lines);
  NS_LOG_DEBUG ("Num road elements " << num_roadElements);
  NS_LOG_DEBUG ("Num LOS points " << num_losPoints);
  return true;
}

RoadElementPoints*
ReadVisibilityMap::InsertPoints (std::ifstream &file, std::string &line, int &num_losPoints)
{
  RoadElementPoints* points = new RoadElementPoints ();
  std::string::size_type end = 0;

  while (end != std::string::npos)
    {
      std::string::size_type  start = line.find_first_of ("=");
      end = line.find_first_of ("\"", start + 2);
      std::string pointId = line.substr (start + 2, end - start -2);
      start = line.find_first_of ("=",end + 1);
      end = line.find_first_of ("\"", start + 2);
      double x = ReadDouble (line.substr (start + 2, end - start -2));
      start = line.find_first_of ("=",end + 1);
      end = line.find_first_of ("\"", start + 2);
      double y = ReadDouble (line.substr (start + 2, end - start -2));
      losPoint* location = new losPoint (x,y);
      points->InsertLosPoint(pointId,location);

      getline (file, line);
      start = line.find_first_of ("<");
      end = line.find_first_of (" ", start + 1);
      num_losPoints = num_losPoints + 1;
    }

   return points;
}

InitPoints* 
ReadVisibilityMap::InsertInitPoints (std::ifstream &file, std::string &line, std::string initEdgeId)
{
  InitPoints* initPoints = new InitPoints ();
  bool iterate = true;

  while (iterate)
    {
      std::string::size_type start = line.find_first_of ("=");
      std::string::size_type end = line.find_first_of ("\"", start + 2);
      std::string initPointId = line.substr (start + 2, end - start -2);
      NS_LOG_DEBUG ( "  initPointId "<< initPointId);
      EndEdges* endEdges = InsertEndEdges(file,line); // InsertEndEdges comes back when it finds <initPoint/>
      losPoint* location = FindLocationForPointId (initPointId,initEdgeId);
      initPoints->InsertPoint(location,endEdges);

      getline (file, line);
      start = line.find_first_of ("<");
      std::string::size_type spacePos = line.find_first_of (" ", start + 1);
      std::string::size_type slashPos = line.find_first_of ("/", start + 1);

      std::string::size_type elementIdEnd = spacePos;
      bool closingTag = false;

      if (spacePos == std::string::npos)
        {
          elementIdEnd = slashPos;
          closingTag = true;
        }

      std::string tag = line.substr (start + 1, elementIdEnd - start - 1);

      if ( (tag == "initPoint") && (closingTag == false) )
        {    
          iterate = true;
        }
      else if ( (tag == "initEdge") && (closingTag == true) )
        {
          iterate = false;
        }
      else 
        {
          NS_LOG_DEBUG ("Error in ReadVisibilityMap::InsertInitPoints");
          iterate = false;
        }

    }

   return initPoints;
}

EndEdges* 
ReadVisibilityMap::InsertEndEdges (std::ifstream &file, std::string &line)
{
  EndEdges* endEdges = new EndEdges ();
  bool iterate = true;
  getline (file, line);

  while (iterate)
    {
      std::string::size_type start = line.find_first_of ("=");
      std::string::size_type end = line.find_first_of ("\"", start + 2);
      std::string endEdgeId = line.substr (start + 2, end - start -2);
      NS_LOG_DEBUG ("    endEdgeId "<< endEdgeId);
      EndPoints* endPoints = InsertEndPoints (file,line,endEdgeId); // InsertEndPoints comes back when it finds <endEdge/>
      endEdges->InsertEdge(endEdgeId,endPoints);

      getline (file, line);
      start = line.find_first_of ("<");
      std::string::size_type spacePos = line.find_first_of (" ", start + 1);
      std::string::size_type slashPos = line.find_first_of ("/", start + 1);

      std::string::size_type elementIdEnd = spacePos;
      bool closingTag = false;

      if (spacePos == std::string::npos)
        {
          elementIdEnd = slashPos;
          closingTag = true;
        }

      std::string tag = line.substr (start + 1, elementIdEnd - start - 1);

      if ( (tag == "endEdge") && (closingTag == false) )
        {    
          iterate = true;
        }
      else if ( (tag == "initPoint") && (closingTag == true) )
        {
          iterate = false;
        }
      else 
        {
         //std::cout << "tag " << tag << std::endl;
          NS_LOG_DEBUG ("Error in ReadVisibilityMap::InsertEndEdges - tag " << tag << " - closingTag " << closingTag);
          iterate = false;
        }

    }

   return endEdges;
}

EndPoints* 
ReadVisibilityMap::InsertEndPoints (std::ifstream &file, std::string &line, std::string endEdgeId)
{
  EndPoints* endPoints = new EndPoints ();
  std::string::size_type end = 0;
  getline (file, line);

  while (end != std::string::npos)
    {
      std::string::size_type  start = line.find_first_of ("=");
      end = line.find_first_of ("\"", start + 2);
      std::string endPointId = line.substr (start + 2, end - start -2);
      start = line.find_first_of ("=",end + 1);
      end = line.find_first_of ("\"", start + 2);
      bool vis = ReadBool (line.substr (start + 2, end - start -2));
      NS_LOG_DEBUG ("      endPointId " << endPointId << " vis=" << vis);
      losPoint* location = FindLocationForPointId (endPointId,endEdgeId);
      endPoints->InsertPoint(location,vis);
      getline (file, line);
      start = line.find_first_of ("<");
      end = line.find_first_of (" ", start + 1);
    }

   return endPoints;
}

losPoint* 
ReadVisibilityMap::FindLocationForPointId (std::string pointId, std::string edgeId)
{
  losPoint* point = NULL;
  RoadElementPoints* roadElementsPoints = m_roadElements->GetEdge (edgeId);
  if ( roadElementsPoints !=NULL )
    {
      point = roadElementsPoints->GetLosPoint (pointId);
    }
  if ( point == NULL )
    {
      NS_LOG_DEBUG ("Error in ReadVisibilityMap::FindLocationForPointId");
    }
  return point;
}


double
ReadVisibilityMap::ReadDouble (std::string valueString) const
{
  std::istringstream iss;
  iss.str (valueString);
  double value;
  iss >> value;
  return value;
}

bool
ReadVisibilityMap::ReadBool (std::string valueString) const
{
  std::istringstream iss;
  iss.str (valueString);
  bool value;
  iss >> value;
  return value;
}


} // namespace ns3

