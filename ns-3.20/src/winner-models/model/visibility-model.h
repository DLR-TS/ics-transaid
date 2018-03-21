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

#ifndef VISIBILITY_MODEL_H
#define VISIBILITY_MODEL_H

#include "ns3/mobility-model.h"
#include "ns3/object.h"

namespace ns3 {

/**
 * @class VisibilityModel
 * @brief Base class that can be derived to implement a VisibilityModel that provides the visibility conditions between transmiter and receiver based on the position of a set of obstacles
 */

class VisibilityModel : public Object
{
public:
  static TypeId GetTypeId (void);
  VisibilityModel (void);
  ~VisibilityModel ();
  VisibilityModel (std::string path);
  virtual bool GetVisibility (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const = 0;
  virtual void GetNlosDistances(Ptr<MobilityModel> a, Ptr<MobilityModel> b, double &dist1, double &dist2) const = 0;
  virtual void InitializeVisibilityModel (void) {};

protected:
  std::string m_path;

};

} // namespace ns3

#endif /* VISIBILITY_MODEL_H */
