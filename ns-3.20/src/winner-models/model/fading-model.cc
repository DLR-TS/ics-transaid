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
 * Author: Miguel Sepulcre <msepulcre@umh.es>, Michele Rondinone <mrondinone@umh.es>
 */

#include "fading-model.h"
#include "ns3/boolean.h"
#include "ns3/double.h"
#include "ns3/string.h"


#include "ns3/enum.h"
#include "ns3/log.h"
#include <math.h>
#include <algorithm>
#include "ns3/pointer.h"

#include <string>
#include <fstream>

NS_LOG_COMPONENT_DEFINE ("FadingModel");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (FadingModel);

TypeId 
FadingModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::FadingModel")
    .SetParent<Object> ()
    .AddConstructor<FadingModel> ()
    .AddAttribute ("FadingFile", "Path to the file defining the ricean fading table.",
                   StringValue (""),
                   MakeStringAccessor (&FadingModel::m_path),
                   MakeStringChecker ())
    .AddAttribute ("MaxVelocity", 
                   "the max velocity in km/h that a node can have in the scenario",
                   DoubleValue (50),
                   MakeDoubleAccessor (&FadingModel::SetmaxVelocity),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Lambda", 
                   "the wavelength relative to this channel",
                   DoubleValue (300000000.0 / 5.90e9),
                   MakeDoubleAccessor (&FadingModel::SetLambda),
                   MakeDoubleChecker<double> ())
    ;
  return tid;
}

FadingModel::FadingModel ()
{
    m_path = ""; // path to the ricean file
    m_lambda=(300000000.0 / 5.90e9);// by default set for 11.p
    fs= 1000.0;
    fm0 = 30.0;
    N = 16384;
    maxVelocity_ = 50;  //// by default set to 50 km/h (urban scenario)
    NLOSK_ = 0.0;
    nn_ = 10000;  // expected number of nodes in the simulation scenario during the entire simulation
    m_startvector_uniform_var = UniformVariable(0.0,8000.0);
}

FadingModel::~FadingModel ()
{
  NS_LOG_FUNCTION_NOARGS ();
  delete [] data1;
  delete [] data2;
  delete [] start_vector;
}

double 
FadingModel::GetLosFading (Ptr<MobilityModel> a, Ptr<MobilityModel> b)
{
   /* Ricean loss in LOS conditions     ricean factor K depends on the distance */
    double K;
  
    double dist= a->GetDistanceFrom(b);         ///////////  do we need a check on the max distance as in shadowing?

    if (dist < 550) {
       K = pow(10.0, (3+0.0142*dist)/10.0); // LOS Winner
    } 
    else {
       K = pow(10.0, (3+0.0142*550)/10.0); // LOS Winner: validity range 500-600m
    }
    double losFading= CalculateFading(a, b, K);
 //   fadingTrace(dist, losFading, 1);
    return losFading;
}

double 
FadingModel::GetNLosFading (Ptr<MobilityModel> a, Ptr<MobilityModel> b)
{
   /* Ricean loss  in NLOS conditions ricean factor K set to 0 */
    double K = NLOSK_;
    double NlosFading= CalculateFading(a, b, K);
  //  double dist= a->GetDistanceFrom(b);         ///////////  do we need a check on the max distance as in shadowing?
  //  fadingTrace(dist, NlosFading, 0);
    return NlosFading;
}

void
FadingModel::SetLambda (double lambda)
{
   m_lambda=lambda;
}

void
FadingModel::SetmaxVelocity (double velocity)
{
   maxVelocity_=velocity;
}

double 
FadingModel::CalculateFading (Ptr<MobilityModel> a, Ptr<MobilityModel> b, double K)
{
    double time_index;
    double tmp_x1, tmp_x2, x1_interp, x2_interp;
    double fm = maxVelocity_ / m_lambda;   /* Max doppler freq in scenario */

    uint32_t maxId = GetMax (a->GetNode()->GetId(),b->GetNode()->GetId());
    uint32_t minId = GetMin (a->GetNode()->GetId(),b->GetNode()->GetId());
    uint32_t link = maxId + nn_*minId - (minId+1)*(minId+2)/2;
    if (link >= (nn_*nn_ - nn_*(nn_+1)/2))
    {
      NS_FATAL_ERROR (" FADING-MODEL: link number is too high compared to the start_vector capacity, please increase the nn_ variable in the constructor of this class");
    }
    
    NS_LOG_DEBUG  ("     FADING: transmission from node "<<a->GetNode()->GetId()<<" to node "<< b->GetNode()->GetId()<< "     link n  "<<link);

    time_index = ( Simulator::Now ().GetSeconds () * fs * fm / fm0) + start_vector[link] ;

    time_index = time_index - double(N)*floor(time_index/double(N)); 

		/* Do envelope interpolation using Legendre polynomials */
		double X0, X1, X2, X3;
		int ind0, ind1, ind2, ind3;
		
		ind1 = int(floor(time_index));
		ind0 = (ind1-1+N) % N;
		ind2 = (ind1+1) % N;
		ind3 = (ind1+2) % N;
		
		X1 = time_index - ind1;
		X0 = X1+1.0;
		X2 = X1-1.0;
		X3 = X1-2.0;

		x1_interp = data1[ind0]*X1*X2*X3/(-6.0) +
			data1[ind1]*X0*X2*X3*(0.5) +
			data1[ind2]*X0*X1*X3*(-0.5) +
			data1[ind3]*X0*X1*X2/6.0;
			
		x2_interp = data2[ind0]*X1*X2*X3/(-6.0) +
			data2[ind1]*X0*X2*X3*(0.5) +
			data2[ind2]*X0*X1*X3*(-0.5) +
			data2[ind3]*X0*X1*X2/6.0;     

		/* Find the envelope multiplicative factor */
		tmp_x1 = x1_interp + sqrt(2.0 * K);
		tmp_x2 = x2_interp;

   double fading = (tmp_x1*tmp_x1 + tmp_x2*tmp_x2) / (2.0 * (K+1.0));  // in W, I have to convert it in dB

   double fadingdB = ConvertToDb (fading);

   NS_LOG_DEBUG  ("     FADING:           fading variable in W = "<<fading<<"         in dB = "<< fadingdB); 

   return fadingdB;	// in dB
}


double 
FadingModel::ConvertToDb (double shadow) const
{
  double fadingDb = log10 (shadow) * 10.0;
  return fadingDb;
}

double
FadingModel::GetMax (double num1, double num2) const
{
  return (num1 < num2 ? num2 : num1);
}

double
FadingModel::GetMin (double num1, double num2) const
{
  return (num1 < num2 ? num1 : num2);
}

void 
FadingModel::Initialize (void)
{
   NS_LOG_DEBUG  ("     FADING:           ricean file= "<<m_path<<"     max speed = "<< maxVelocity_); 
   ReadRiceanTable ();
   InitializeStartVector();
   
}

void 
FadingModel::ReadRiceanTable (void)
{

   FILE *fdstream;
   float *tmp1, *tmp2;
   if(m_path == "")
   {
      NS_FATAL_ERROR (" FADING-MODEL: No Ricean File to open");
   }
   else{
     fdstream = fopen(m_path.c_str (), "r");
   }

   data1 = new float[N];
   data2 = new float[N];

   tmp1 = data1;
   tmp2 = data2;

   for(int k=0; k<N; k++) {
     int ret_val = fscanf(fdstream, "%f %f", tmp1++, tmp2++);
     if(ret_val != 2) {
         NS_FATAL_ERROR (" FADING-MODEL: Ricean File is not been reading properly");
     }
   }

//    for(int k=0; k<N; k++) {
//      NS_LOG_DEBUG  ("     FADING: index= "<<k<<"        DATA1 = "<<data1[k]<<"     DATA2 = "<< data1[k]);
//      }
}

void
FadingModel::InitializeStartVector(void)
{
   start_vector = new float[nn_*nn_ - nn_*(nn_+1)/2];
   for (uint i=0; i<(nn_*nn_ - nn_*(nn_+1)/2);i++) {   // total number of prospected links
     start_vector[i] = m_startvector_uniform_var.GetValue();
    // NS_LOG_DEBUG  ("     FADING: startvariable "<<i<<" = "<< start_vector[i]);
   }
}


// void 
// FadingModel::fadingTrace(double dist, double fading, uint isLOS)
// {
//   if(isLOS==1)
//   {
//   FILE *fp = fopen("/home/michele/ns-3/fadingLOS.txt", "a+");
// 
//     fprintf(fp, "%.9f %f %f\n",(Simulator::Now()).GetSeconds(), dist, fading);
//     fclose(fp);
//   }
//   else
//   {
//    FILE *fp = fopen("/home/michele/ns-3/FadingNLOS.txt", "a+");
// 
//     fprintf(fp, "%.9f %f %f\n",(Simulator::Now()).GetSeconds(), dist, fading);
//     fclose(fp);
//   }
// }


} // namespace ns3
