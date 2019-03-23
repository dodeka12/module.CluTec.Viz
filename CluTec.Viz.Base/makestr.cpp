////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      makestr.cpp
//
// summary:   Implements the makestr class
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

#include"makestr.h"
#include "CStrMem.h"

char* StrType(int a) { return "d"; }
char* StrType(unsigned int a) { return "u"; }
char* StrType(long int a) { return "ld"; }
char* StrType(unsigned long int a) { return "lu"; }
char* StrType(float a) { return "g"; }
char* StrType(double a) { return "lg"; }
char* StrType(long double a) { return "Lg"; }


CStrMem MakeStr(const int &a) 
{ 
	CStrMem str,fs("d");
	str = (a%fs);
	return str; 
}

CStrMem MakeStr(const unsigned int &a) 
{ 
	CStrMem str, fs("u");
	str = (a%fs);
  return str; 
}

CStrMem MakeStr(const long &a) 
{ 
	CStrMem str, fs("ld");
	str = (a%fs);
  return str; 
}

CStrMem MakeStr(const unsigned long &a) 
{ 
	CStrMem str, fs("lu");
	str = (a%fs);
  return str; 
}

CStrMem MakeStr(const float& a)
{
	int iLen, i,j;
	float b;
	CStrMem str, exp, fs1 = "e", fs2 = "f";


	b = ((a < 0.0) ? (-a) : (a));
	if ((b < 1e-3 && a != 0.0) || b > 1e3)
	{
		str = (a%fs1);
		iLen = int(str.Len());
		exp = &str[iLen-4];
		str |= -4;

		for (i=iLen-5,j=0; i>=0; i--,j++)
			if (str[i] != '0' && str[i] != '.') break;

		if (str[i] == '.') j++;

		str |= -j;
		str << exp;
	}
	else
	{
		str = (a%fs2);
		iLen = int(str.Len());

		for (i=iLen-1,j=0; i>=0; i--,j++)
			if (str[i] != '0') break;

		if (str[i] == '.') j++;
		str |= -j;
	}

	return str;
}

CStrMem MakeStr(const double &a) 
{ 
  int len, i,j;
  double b;
	CStrMem str, exp, fs1 = "le", fs2 = "lf";

  b = ((a < 0.0) ? (-a) : (a));
  if((b < 1e-3 && a != 0.0) || b > 1e3) 
  { 
	str = (a%fs1);
    len = int(str.Len());
    exp = &str[len-4];
    str |= -4;

    for(i=len-5,j=0;i>=0;i--,j++)
      if(str[i] != '0' && str[i] != '.') break;
 
    if (str[i] == '.') j++;
		str |= -j;
    str << exp;    
  }
  else
  { 
	str = (a%fs2);
    len = int(str.Len());

    for(i=len-1,j=0;i>=0;i--,j++)
      if (str[i] != '0') break;

		if (str[i] == '.') j++;
		str |= -j;
  }

  return str; 
}

