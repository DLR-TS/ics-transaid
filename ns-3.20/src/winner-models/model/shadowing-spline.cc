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

#include "shadowing-spline.h"
#include "stdlib.h"
#include <iostream>
#include "stdlib.h"
#include "stdio.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("ShadowingSpline");

namespace ns3 {

#define shadowingN_filt_ 7
#define shadowingW_filt_ 11

ShadowingSpline::ShadowingSpline()
{
	initSh_ = false;
	shadowingStepTime_ = -1.0;
	shadowingConst = -1.0;
	m_decorrelationDist = 20.0;
}

void ShadowingSpline::Reset()
{
	initSh_ = false;
	shadowingStepTime_ = -1.0;
	shadowingConst = -1.0;
}

ShadowingSpline::~ShadowingSpline()
{

}

float ShadowingSpline::GetSample(double std_db, double speed, float *shadowingH_filt_, float current_t, RandomVariable *ranVar)
{

	if (initSh_ == false ) {

		// Initialize the link:
		if (shadowingConst == -1.0) shadowingConst = ranVar->GetValue ();
		std_db_ = std_db; // constant for each link !!!
		if (speed > 0.0001) {
			// Initialize only if nodes are moving
                        NS_LOG_DEBUG ("Initiating ShadowingSpline");
			Init(speed, shadowingH_filt_, current_t, ranVar);
 		        initSh_ = true;
		}

	} else {
                NS_LOG_DEBUG ("Getting new sample from ShadowingSpline at t="<<current_t);
		if (speed < 0.0001 && shadowingStepTime_ > 0.0001) {

			// This link was active before. Stop it:
			shadowingConst = Interpolate(current_t); // from now on the shadowing is constant
			delete[] shadowingX_filt_;
			DeleteSpline();
			shadowingStepTime_ = -1.0;

		} else if (speed > 0.0001 && shadowingStepTime_ < 0.0001) {

			// This link was stopped before. Active it:
			Init(speed, shadowingH_filt_, current_t, ranVar);

		} else if (speed > 0.0001 && shadowingStepTime_ > 0.0001) {

			// This link is active. Update it:
			Update( shadowingH_filt_, current_t,ranVar);

		} else if (speed < 0.0001 && shadowingStepTime_ < 0.0001) {
			// This link is stopped. Do nothing.
		}
	}

	// Calculate and return the current shadowing sample:
	if (shadowingStepTime_ == -1.0) return shadowingConst;
	else return Interpolate(current_t);
}

float ShadowingSpline::Filter(float x, float *shadowingH_filt_)
{
   int i;
   for(i=shadowingN_filt_-1; i>0 ; i--)  shadowingX_filt_[i] = shadowingX_filt_[i-1];
   shadowingX_filt_[0]=x;
 
   float y=0.0;
   for(i=0;i<shadowingN_filt_;i++) y = y + shadowingX_filt_[i]*shadowingH_filt_[i];
  
   return y;

}

void ShadowingSpline::SetDecorrelationDist (float dist)
{
  m_decorrelationDist = dist;
}

void ShadowingSpline::Init(double speed, float *shadowingH_filt_, float current_t, RandomVariable *ranVar)
{
	   int i;
	   float TempRandom=0.0;
	   float *x;
	   x = new float[shadowingW_filt_];
	   float *y;
	   y = new float[shadowingW_filt_];	
	
	   shadowingTime_ = current_t;

	   shadowingStepTime_ = m_decorrelationDist/(speed);

	   shadowingX_filt_ = new float[shadowingN_filt_];

	   for(i=0 ; i<shadowingN_filt_ ; i++) shadowingX_filt_[i] = shadowingConst;

	   for(i=0 ; i<shadowingW_filt_ ; i++)
	   	{
  	      	   x[i] = shadowingTime_-shadowingStepTime_*shadowingW_filt_/2.0 + i*shadowingStepTime_;  
	   	   TempRandom = ranVar->GetValue ();
	       	   y[i] = Filter(TempRandom, shadowingH_filt_);   // shadowing filtered sample
	      	}
	
	   InitSpline(x,y,shadowingW_filt_); // this will make init = 1.0 (spline.cc)

}

void ShadowingSpline::Update(float *shadowingH_filt_, float current_t, RandomVariable *ranVar)
{
	float y=0.0, TempRandom;

	if (shadowingStepTime_ > 0.0) {
                NS_LOG_DEBUG ("Updating samples. current_t="<<current_t<<" shadowingTime_="<<shadowingTime_<<" shadowingStepTime_="<<shadowingStepTime_);
		while (current_t > (shadowingTime_ + shadowingStepTime_)) {
			shadowingTime_ = shadowingTime_ + shadowingStepTime_;
		        TempRandom = ranVar->GetValue ();
                        NS_LOG_DEBUG ("TempRandom="<<TempRandom);
		       	y = Filter(TempRandom, shadowingH_filt_);   // shadowing filtered sample
			Shift(shadowingTime_ - shadowingStepTime_*shadowingW_filt_/2.0 + (shadowingW_filt_-1.0)*shadowingStepTime_, y);
		}
	}
}

} // namespace ns3

