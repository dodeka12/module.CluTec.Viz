////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      rand.h
//
// summary:   Declares the random class
//
//            Copyright (c) 2019 by Christian Perwass.
//
//            This file is part of the CluTecLib library.
//
//            The CluTecLib library is free software: you can redistribute it and / or modify
//            it under the terms of the GNU Lesser General Public License as published by
//            the Free Software Foundation, either version 3 of the License, or
//            (at your option) any later version.
//
//            The CluTecLib library is distributed in the hope that it will be useful,
//            but WITHOUT ANY WARRANTY; without even the implied warranty of
//            MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//            GNU Lesser General Public License for more details.
//
//            You should have received a copy of the GNU Lesser General Public License
//            along with the CluTecLib library.
//            If not, see <http://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// Class Declaration for random number routines

#ifndef _RAND_HH_
#define _RAND_HH_

typedef long int lint;
typedef unsigned uint;

class Rand
{
  
public:
  Rand(lint nseed = 0);

  Rand& operator= (const Rand& R);

  void seed3(lint nseed);
  double ran3();
  unsigned ran3u();
  inline double pmran3(double x) { return ((ran3()-0.5)*2.0*x); }
  inline double range3(double min, double max)
  { return (min + (max-min)*ran3()); }

  double gausdev() { return Gauss(); }
  void GausDev(double ngasdev) { gasdev = ngasdev; }
  void GausMean(double ngasmean) { gasmean = ngasmean; }

  double Gauss();
  void GaussDev(double ngasdev) { gasdev = ngasdev; }
  void GaussMean(double ngasmean) { gasmean = ngasmean; }

protected:

  // for ran3
  int ran3Inext;
  int ran3Inextp;
  lint ran3Ma[55];  // Do NOT change size

  lint mbig, mseed, mz;
  double fac;

  // for gausdev
  uint gasdevIset;
  double gasdevGset;
  double gasdev, gasmean;
};





#endif



