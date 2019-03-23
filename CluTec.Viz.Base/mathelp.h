////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      mathelp.h
//
// summary:   Declares the mathelp class
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

// Mathematical helping functions

#ifndef _MATHELP_HH_
#define _MATHELP_HH_

typedef unsigned uint;

int Mag(int a);
float Mag(float a);
double Mag(double a);
long int Mag(long int a);

float Round(float x);
double Round(double x);

int Min(int a, int b);
uint Min(uint a, uint b);
float Min(float a, float b);
double Min(double a, double b);

int Max(int a, int b);
uint Max(uint a, uint b);
float Max(float a, float b);
double Max(double a, double b);

uint Fact(uint n);
uint Comb(uint n, uint k);
uint Vari(uint n, uint k);

// Returns Permutation
int Perm3(uint i, uint &j, uint &k, uint even);

// Returns "tiny" value, i.e. limit of floating point precision.
int Tiny(int& tiny);
float Tiny(float& tiny);
double Tiny(double& tiny);

int Round(int fVal, int fPrec);
float Round(float fVal, float fPrec);
double Round(double fVal, double fPrec);

inline bool IsZero(int iVal, int iPrec)
{ return (iVal >= -iPrec && iVal <= iPrec); }

inline bool IsZero(float fVal, float fPrec)
{ return (fVal >= -fPrec && fVal <= fPrec); }

inline bool IsZero(double dVal, double dPrec)
{ return (dVal >= -dPrec && dVal <= dPrec); }

#endif
