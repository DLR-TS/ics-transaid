
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

#ifndef SPLINE_H
#define SPLINE_H

namespace ns3 {

class Spline
{

public:
  Spline();
  virtual ~Spline();  

  void InitSpline(float x[], float y[], int N);
  void DeleteSpline();
  
  void SetVectors(float x[] , float y[], int N);
  void Shift(float u,float v);
  float Interpolate(float xi);

  int init_;

private:
  void Get2ndDerivatives();

  float *x_;      // x vector
  float *y_;      // y vector
  int N_;	   // length of x and y
  float *y2_;     // second derivatives of y=f(x) at points x

  int klo_;
  int khi_;

};

} // namespace ns3

#endif
