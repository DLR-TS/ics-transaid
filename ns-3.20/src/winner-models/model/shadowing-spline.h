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

#ifndef SHADOWING_SPLINE_H
#define SHADOWING_SPLINE_H

#include "spline.h"
// #include <rng.h>
#include "ns3/random-variable.h"

namespace ns3 {

class ShadowingSpline : public Spline {
public:
	ShadowingSpline();
	~ShadowingSpline();  

	void Reset(void);
	float Filter(float x, float *shadowingH_filt_);
	void Init(double speed, float *shadowingH_filt_, float current_t, RandomVariable *ranVar);
	void Update(float *shadowingH_filt_, float current_t, RandomVariable *ranVar);
	float GetSample(double std_db, double speed, float *shadowingH_filt_, float current_t, RandomVariable *ranVar);
        void SetDecorrelationDist (float dist);

private:

	bool initSh_;
	float shadowingStepTime_;	// step/speed
	double std_db_;			// constant for each link !!!!

	float *shadowingX_filt_;
	float shadowingTime_;		// time (center) of the current window of samples of the shadowing spline
	float shadowingConst;		// for nodes with speed = 0
        float m_decorrelationDist;

};

} // namespace ns3

#endif


