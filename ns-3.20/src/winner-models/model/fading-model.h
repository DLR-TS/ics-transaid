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
 * Author:  Miguel Sepulcre <msepulcre@umh.es>, Michele Rondinone <mrondinone@umh.es>
 */

#ifndef FADING_MODEL_H
#define FADING_MODEL_H

#include "ns3/simulator.h"
#include "ns3/object.h"
#include "ns3/mobility-model.h"
#include "ns3/random-variable.h" 

namespace ns3 {

class FadingModel : public Object
{
public:
  static TypeId GetTypeId (void);
  FadingModel ();
  ~FadingModel ();
  double GetLosFading (Ptr<MobilityModel> a, Ptr<MobilityModel> b);        
  double GetNLosFading (Ptr<MobilityModel> a, Ptr<MobilityModel> b);      

  void Initialize (void);


private:

  double CalculateFading (Ptr<MobilityModel> a, Ptr<MobilityModel> b, double K);

  double ConvertToDb (double fading) const;
  double GetMin (double num1, double num2) const;
  double GetMax (double num1, double num2) const;

  void SetLambda (double lambda);
  void SetmaxVelocity (double velocity);

  void ReadRiceanTable (void);  
  void InitializeStartVector(void);  

 // void fadingTrace(double dist, double fading, uint isLOS);

  double m_lambda;    ///////////////

  int N;                  /* Num points in table */
  double fm0;              /* Max doppler freq in table */
  double fs;               /* Sampling rate */
  double  maxVelocity_;   /* Maximum velocity of vehicle/objects in environment (km/h).  Used for computing doppler */
  double NLOSK_;          /* Ricean K factor */
  float *data1;           /* Data values for inphase and quad phase */
  float *data2;
  float *start_vector;    /* vector containing uniformly distributed variables used to calculate the index to get values from the ricean table */
  uint nn_;                /* max number of nodes estimated to be present in the scenario over the entire simulation*/

  RandomVariable  m_startvector_uniform_var; 

  std::string m_path;          /*file containing the ricean table*/

  
};

} // namespace ns3

#endif /* FADING_MODEL_H */