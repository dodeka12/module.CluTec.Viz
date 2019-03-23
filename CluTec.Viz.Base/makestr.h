////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      makestr.h
//
// summary:   Declares the makestr class
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

// Make String helping functions

#ifndef _MAKESTR_HH_
#define _MAKESTR_HH_

#include<string.h>
#include<stdio.h>

#include "CStrMem.h"

// Functions giving type-string of parameter
char* StrType(int a);
char* StrType(unsigned int a);
char* StrType(long int a);
char* StrType(unsigned long int a);
char* StrType(float a);
char* StrType(double a);
char* StrType(long double a);


CStrMem MakeStr(const int &a);
CStrMem MakeStr(const unsigned int &a);
CStrMem MakeStr(const long int &a);
CStrMem MakeStr(const unsigned long int &a);
CStrMem MakeStr(const float &a);
CStrMem MakeStr(const double &a);


#endif
