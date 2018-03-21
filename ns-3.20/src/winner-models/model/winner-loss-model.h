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
 * Author: Ramon Bauza <rbauza@umh.es>, Michele Rondinone <mrondinone@umh.es>
 */

#ifndef WINNER_LOSS_MODEL_H
#define WINNER_LOSS_MODEL_H

#include "ns3/propagation-loss-model.h"

namespace ns3 {

class VisibilityModel;
class ShadowingModel;
class FadingModel;

class WinnerLossModel : public PropagationLossModel
{
public:
  static TypeId GetTypeId (void);
  WinnerLossModel ();
  ~WinnerLossModel ();
  void SetLambda (double frequency, double speed);
  void SetLambda (double lambda);
  void SetFrequency (double freq);
  void SetStreetWidth (double streetWidth);
  void SetMinDistance (double minDistance);
  void SetEffEnvironmentHeight (double effEnvironmentHeight);
  void SetVisibilityModel (Ptr<VisibilityModel> visibilityModel);
  void SetFadingModel (Ptr<FadingModel> fadingModel); 
  double GetMinDistance (void) const;
  double GetLambda (void) const;
  double GetStreetWidth (void) const;
  double GetEffEnvironmentHeight (void) const;

protected:
  double GetMin (double num1, double num2) const;
  double GetMax (double num1, double num2) const;
  double m_lambda;
  double m_frequency;
  double m_minDistance;
  double m_streetWidth;
  double m_effEnvironmentHeight;
  Ptr<VisibilityModel> m_visibilityModel;

private:
  WinnerLossModel (const WinnerLossModel &o);
  WinnerLossModel & operator = (const WinnerLossModel &o);
  double DoCalcRxPower (double txPowerDbm,
                                Ptr<MobilityModel> a,
                                Ptr<MobilityModel> b) const;
  double DbmToW (double dbm) const;
  double DbmFromW (double w) const;
  virtual double CalculateLossInLos (Ptr<MobilityModel> a, Ptr<MobilityModel> b, double distance) const = 0; // cambiar
  virtual double CalculateLossInNlos (Ptr<MobilityModel> a, Ptr<MobilityModel> b, double d1, double d2) const = 0; // cambiar

  // added by Andrea
  virtual int64_t DoAssignStreams (int64_t stream);
  Ptr<ShadowingModel> m_shadowingModel;
  Ptr<FadingModel> m_fadingModel; 

  
};

} // namespace ns3

#endif /* WINNER_LOSS_MODEL_H */
