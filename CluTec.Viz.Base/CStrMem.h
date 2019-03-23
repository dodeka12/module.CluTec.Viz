////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      CStrMem.h
//
// summary:   Declares the string memory class
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

// String Library Declarations

/*
Operators + and << can be used concurrently. 
The former for sums including at least one element at the first or second
position of type cStr.
The latter for streams that do not necessarily include vars of type cStr.
*/

#ifndef _CSTR_HH_
#define _CSTR_HH_

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "mem.h"

class CStrMem;

#include "makestr.h"

#ifndef _MAXSTRSIZE_
#define _MAXSTRSIZE_ 1024
#endif

#define _CSTR_FORMAT_CHARS_ "iouoxXfeEgGcsp"

typedef unsigned uint;
typedef unsigned long ulong;

class CStrMem
{
public:
	CStrMem(const char *nstr = 0);
	CStrMem(const CStrMem& nstr);
	~CStrMem();
	
	//  char& operator[] (uint pos) { return str[pos]; }
	
    CStrMem& ToUpperCase();
	CStrMem& ToLowerCase();

	// Sub String extraction
	CStrMem operator() (size_t fpos, size_t lpos, bool bCut = false);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 	Returns the first iNumChars from this string. If bCut is specified, these characters are deleted from this string.
	/// </summary>
	///
	/// <param name="iNumChars">	(optional) Number of characters to return. </param>
	/// <param name="bCut">			(optional) True, if characters should be removed from string. </param>
	///
	/// <returns>	The first iNumChars characters. </returns>
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CStrMem First(int iNumChars = 1, bool bCut = false);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 	Returns the last iNumChars from this string. If bCut is specified, these characters are deleted from this string.
	/// </summary>
	///
	/// <param name="iNumChars">	(optional) Number of characters to return. </param>
	/// <param name="bCut">			(optional) True, if characters should be removed from string. </param>
	///
	/// <returns>	The last iNumChars characters. </returns>
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CStrMem Last(int iNumChars = 1, bool bCut = false);


	CStrMem& operator= (const CStrMem& nstr);
	CStrMem& operator= (const char *nstr);
	CStrMem& operator= (const char a);
	CStrMem& operator= (const int a);
	CStrMem& operator= (const unsigned int a);
	CStrMem& operator= (const long a);
	CStrMem& operator= (const double a);
	CStrMem& operator= (const long double a);
	CStrMem& operator= (const void* a);
	
	friend int operator== (const CStrMem& a, const CStrMem& b);
	friend int operator!= (const CStrMem& a, const CStrMem& b);
	
	friend int operator== (const CStrMem& a, const char* b);
	friend int operator!= (const CStrMem& a, const char* b);

	friend int operator== (const char* a, const CStrMem& b);
	friend int operator!= (const char* a, const CStrMem& b);

	// Returns position of a in b +1.
	// Returns 0 if a is not in b.
	friend size_t operator> (const char a, const CStrMem& b); // first occurrence
	friend size_t operator< (const char a, const CStrMem& b); // last occurrence
	
	friend CStrMem operator! (const CStrMem& a); // Capitalize
	friend CStrMem operator- (const CStrMem& a); // Change to lower case
	
	friend CStrMem operator~ (const CStrMem& a); // Reverse String
	
	friend CStrMem operator+ (const CStrMem& a, const CStrMem& b);
	friend CStrMem& operator+= (CStrMem& a, const CStrMem& b);
	
	friend CStrMem operator+ (const CStrMem& a, const char *b);
	friend CStrMem operator+ (const char *b, const CStrMem& a);
	friend CStrMem& operator+= (CStrMem& a, const char *b);
	
	friend CStrMem operator+ (const CStrMem& a, const char b);
	friend CStrMem operator+ (const char b, const CStrMem& a);
	friend CStrMem& operator+= (CStrMem& a, const char b);
	
	friend CStrMem operator+ (const CStrMem& a, const uint b);
	friend CStrMem operator+ (const uint b, const CStrMem& a);
	friend CStrMem& operator+= (CStrMem& a, const uint b);
	
	friend CStrMem operator+ (const CStrMem& a, const ulong b);
	friend CStrMem operator+ (const ulong b, const CStrMem& a);
	friend CStrMem& operator+= (CStrMem& a, const ulong b);
	
	friend CStrMem operator+ (const CStrMem& a, const int b);
	friend CStrMem operator+ (const int b, const CStrMem& a);
	friend CStrMem& operator+= (CStrMem& a, const int b);
	
	friend CStrMem operator+ (const CStrMem& a, const long b);
	friend CStrMem operator+ (const long b, const CStrMem& a);
	friend CStrMem& operator+= (CStrMem& a, const long b);
	
	friend CStrMem operator+ (const CStrMem& a, const double b);
	friend CStrMem operator+ (const double b, const CStrMem& a);
	friend CStrMem& operator+= (CStrMem& a, const double b);
	
	friend CStrMem operator+ (const CStrMem& a, const long double b);
	friend CStrMem operator+ (const long double b, const CStrMem& a);
	friend CStrMem& operator+= (CStrMem& a, const long double b);
	
	friend CStrMem& operator<< (CStrMem& a, const CStrMem& b);
	friend CStrMem& operator<< (CStrMem& a, const char *b);
	friend CStrMem& operator<< (CStrMem& a, const char b);
	friend CStrMem& operator<< (CStrMem& a, const uint b);
	friend CStrMem& operator<< (CStrMem& a, const ulong b);
	friend CStrMem& operator<< (CStrMem& a, const int b);
	friend CStrMem& operator<< (CStrMem& a, const long b);
	friend CStrMem& operator<< (CStrMem& a, const double b);
	friend CStrMem& operator<< (CStrMem& a, const long double b);
	
	friend CStrMem operator% (const int a, const CStrMem& b);
	friend CStrMem operator% (const long a, const CStrMem& b);
	friend CStrMem operator% (const double a, const CStrMem& b);
	friend CStrMem operator% (const long double a, const CStrMem& b);
	
	friend CStrMem operator% (const unsigned a, const CStrMem& b);
	friend CStrMem operator% (const unsigned long a, const CStrMem& b);
	
	friend CStrMem operator* (const CStrMem& a, size_t n);
	friend CStrMem operator* (size_t n, const CStrMem& a);
	friend CStrMem& operator*= (CStrMem& a, size_t n);
	
	// Add or sub n characters to string. Char given by cfill
	friend CStrMem operator| (const CStrMem& a, int n);
	friend CStrMem operator| (int n, const CStrMem& a);
	friend CStrMem& operator|= (CStrMem& a, int n);
	
	friend int operator>> (const CStrMem& a, uint& b);
	friend int operator>> (const CStrMem& a, ulong& b);
	friend int operator>> (const CStrMem& a, int& b);
	friend int operator>> (const CStrMem& a, long& b);
	friend int operator>> (const CStrMem& a, float& b);
	friend int operator>> (const CStrMem& a, double& b);
	friend int operator>> (const CStrMem& a, long double& b);
	friend int operator>> (const CStrMem& a, void*& b);
	
	
	
	size_t Len() const { return (str.Count() - 1); }
	char *Str() const { return str.Data(); }
	operator char*() const { return str.Data(); }
	
	char FillChar() const { return cfill; }
	char FillChar(char ncfill) { cfill = ncfill; return cfill; }
	
	// Returns 1 if c is a printf format character
	// i.e. it is in _CSTR_FORMAT_CHARS_.
	// Otherwise zero is returned.
	friend int isFormat(char c);
	
protected:
	Mem<char> str;
	char cfill;
	
	CStrMem& MakeFormatStr(char *ts);
};



#endif
