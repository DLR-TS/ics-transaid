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

#include "spline.h"
#include <stdlib.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

namespace ns3 {

Spline::Spline()
{
	init_ = 0;
}

void Spline::InitSpline(float x[], float y[], int N)
{

  //x_ = (float *)malloc(N*sizeof(float));
  //if (x_ == NULL) perror("Spline: Memory allocation error");
  x_ = new float[N];

  //y_ = (float *)malloc(N*sizeof(float));
  //if (y_ == NULL) perror("Spline: Memory allocation error");
  y_ = new float[N];

  //y2_ = (float *)malloc(N*sizeof(float));
  //if (y2_ == NULL) perror("Spline: Memory allocation error");
  y2_ = new float[N];

  for (int k=0; k<N;k++){
	x_[k]=x[k];
	y_[k]=y[k];
	y2_[k]=y[k];// just for initialization
  }
  N_ = N;

  // Limits for bisection algorithm in interpolation
  klo_=0;
  khi_=1;

  init_ = 1;

  Get2ndDerivatives();

}


Spline::~Spline()
{

	/*free(x_);
	free(y_);
	free(y2_);*/

}

void Spline::DeleteSpline()
{
	delete[] x_;
	delete[] y_;
	delete[] y2_;
}



void Spline::SetVectors(float x[], float y[], int N)
{

  for (int k=0; k<N;k++){
	x_[k]=x[k];
	y_[k]=y[k];
	y2_[k]=y[k];// just for initialization
  }

  N_ = N;

  // Limits for bisection algorithm in interpolation
  klo_=0;
  khi_=1;

  init_ = 1;

  Get2ndDerivatives();

}


void Spline::Shift(float u,float v)
{
  // Remove x_[0] and y_[0], and add u and v at the end of the vectors:

  for(int i=1; i<N_ ; i++)
    {
      x_[i-1]=x_[i];
      y_[i-1]=y_[i];
    }
  x_[N_-1]=u;
  y_[N_-1]=v;

  // Limits for bisection algorithm in interpolation
  klo_=0;
  khi_=1;

  Get2ndDerivatives();

  // cout << x << endl << y << endl;
}


void Spline::Get2ndDerivatives()
{

  float u[N_];

  // Set second derivative of first and last point to zero
  y2_[0]=0;
  y2_[N_-1]=0;
  u[0]=0;
  u[N_-1]=0;

  // Decomposition loop of tridiagonal algorithm
  for (int i=1;i<N_-1;i++)
    {
      float sig=(x_[i]-x_[i-1])/(x_[i+1]-x_[i-1]);
      float p=sig*y2_[i-1]+2.0;
      y2_[i]=(sig-1.0)/p;
      u[i]=(y_[i+1]-y_[i])/(x_[i+1]-x_[i])-(y_[i]-y_[i-1])/(x_[i]-x_[i-1]);
      u[i]=(6.0*u[i]/(x_[i+1]-x_[i-1])-sig*u[i-1])/p;
    }

  // Backsubstitution
  for (int k=N_-2;k>=0;k--)
    {
      y2_[k]=y2_[k]*y2_[k+1]+u[k];
    }

}


float Spline::Interpolate(float xi)
{
  // Test if limits are still aproppriate
  if ( (x_[klo_] > xi) || (x_[khi_] < xi) )
  {
      // Need to find new limits
      klo_=0;
      khi_=N_-1;
      
      while (khi_-klo_ > 1)
	  {
		  int k=(khi_+klo_) >> 1;
		  if (x_[k] > xi) 
			  khi_=k;
		  else
			  klo_=k;
	  } 
  }

  //  cout << klo_ << '\t' << xi << '\t' << khi_ << endl;
    
  float h=x_[khi_]-x_[klo_];
  float a=(x_[khi_]-xi)/h;
  float b=(xi-x_[klo_])/h;

  // obtain a cubic spline interpolation
  float yi=a*y_[klo_]+b*y_[khi_]+((a*a*a-a)*y2_[klo_]+(b*b*b-b)*y2_[khi_])*h*h/6.0;

  return yi;
}

/****************************
main()
{
  CNArrayfloat X(5);
  CNArrayfloat Y(5);

  X[0]=0;
  X[1]=1;
  X[2]=2;
  X[3]=3;
  X[4]=4;


  Y[0]=2;
  Y[1]=0.1;
  Y[2]=0.5;
  Y[3]=2;
  Y[4]=1.0;

  Spline S(X,Y);

  for (int i=0;i<=20;i++)
    {
      cout << 4.0/20.0*i << '\t' << S.Interpolate(4.0/20.0*i) 
	   << '\t' << Y[int(4.0/20.0*i)] << endl;
    }

}

***************************************/

} // namespace ns3

