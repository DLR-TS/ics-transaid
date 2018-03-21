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

#ifndef SHADOWING_MODEL_H
#define SHADOWING_MODEL_H

#include "ns3/simulator.h"
#include "ns3/object.h"
#include "ns3/random-variable.h"
#include "ns3/mobility-model.h"
#include "shadowing-spline.h"
#include <map>

namespace ns3 {

class ShadowingModel : public Object
{
public:
  static TypeId GetTypeId (void);
  ShadowingModel ();
  ~ShadowingModel ();
  double GetLosShadowing (Ptr<MobilityModel> a, Ptr<MobilityModel> b);
  double GetNLosShadowing (Ptr<MobilityModel> a, Ptr<MobilityModel> b);
  void SetLosStd (double losStd);
  void SetNLosStd (double nLosStd);
  void SetIsCorrelatedShadowing (bool correlatedShadowing);
  bool IsCorrelatedShadowing (void);

protected:
  double GetMin (double num1, double num2) const;
  double GetMax (double num1, double num2) const;

private:

  typedef std::vector<ShadowingSpline*> ShadowingLinks;
  std::vector<ShadowingLinks*> m_shadowingSpline;

  ShadowingModel (const ShadowingModel &o);
  ShadowingModel & operator = (const ShadowingModel &o);

  double DbmToW (double dbm) const;
  double DbmFromW (double w) const;
  double ConvertToDb (double shadow) const;
  double GetLosCorrelatedShadowing (Ptr<MobilityModel> a, Ptr<MobilityModel> b);
  double GetNLosCorrelatedShadowing (Ptr<MobilityModel> a, Ptr<MobilityModel> b);
  void UpdateShadowingSpline (uint32_t maxId, uint32_t minId);
  void UpdateShadowingLinks (ShadowingLinks* shadowingLinks, uint32_t maxId, uint32_t minId);
  double CalculateSpeed (Vector speed);

  bool m_correlatedShadowing;
  double m_losStd;
  double m_nLosStd;
  float *m_shadowingHFilt; // Filter coefficients

  RandomVariable* m_lossShadowing;
  RandomVariable* m_nLossShadowing;
  
};

} // namespace ns3

#endif /* SHADOWING_MODEL_H */
