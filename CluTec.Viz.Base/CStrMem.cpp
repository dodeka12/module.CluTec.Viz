////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      CStrMem.cpp
//
// summary:   Implements the string memory class
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

// Character string Library Definitions

#include "CStrMem.h"

// Constructor
CStrMem::CStrMem(const char* nstr)
{
	str.SetBlockSize(32);
	if (nstr)
	{
		size_t iSize = strlen(nstr) + 1;
		str.Set(iSize);
		strcpy_s(str.Data(), iSize, nstr);
	}
	else
	{
		str    = 1;
		str[0] = 0;
	}

	cfill = ' ';
}

// Copy Constructor
CStrMem::CStrMem(const CStrMem& nstr)
{
	str.SetBlockSize(nstr.str.GetBlockSize());
	str   = nstr.str;
	cfill = nstr.cfill;
}

// Destructor
CStrMem::~CStrMem()
{
}

/// Make Upper Case String
CStrMem& CStrMem::ToUpperCase()
{
	size_t i, len = str.Count();
	char* data = str.Data();

	if (len > 0) { len--; }

	for (i = 0; i < len; i++)
	{
		data[i] = char(toupper(int(data[i])));
	}

	return *this;
}

/// Make Lower Case String
CStrMem& CStrMem::ToLowerCase()
{
	size_t i, len = str.Count();
	char* data = str.Data();

	if (len > 0) { len--; }

	for (i = 0; i < len; i++)
	{
		data[i] = char(tolower(int(data[i])));
	}

	return *this;
}

// SubString Extraction
CStrMem CStrMem::operator()(size_t fpos, size_t lpos, bool bCut)
{
	CStrMem s;
	char* data = str.Data();
	char* sdata;
	size_t iLen;

	if (fpos > lpos) { return s; }

	s    |= int(iLen = lpos - fpos + 1);
	sdata = s.Str();

	strncpy_s(sdata, s.Len() + 1, &data[fpos], iLen);

	// Remove characters from original string
	if (bCut)
	{
		str.Del(fpos, lpos - fpos + 1);
	}

	return s;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CStrMem CStrMem::Last(int iNumChars, bool bCut)
{
	CStrMem sNewString;
	char* pcData   = str.Data();
	size_t nLength = Len();

	if (iNumChars > (int) nLength)
	{
		return sNewString;
	}

	sNewString |= (int) iNumChars;
	char* pcNewStringData = sNewString.Str();

	for (int i = (int) nLength - 1, j = iNumChars - 1; j >= 0; i--, j--)
	{
		pcNewStringData[j] = pcData[i];
	}

	// Remove characters from original string
	if (bCut)
	{
		str.Del(nLength - iNumChars, iNumChars);
	}

	return sNewString;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CStrMem CStrMem::First(int iNumChars, bool bCut)
{
	CStrMem sNewString;
	char* pcData   = str.Data();
	size_t nLength = Len();

	if (iNumChars > (int) nLength)
	{
		return sNewString;
	}

	sNewString |= (int) iNumChars;
	char* pcNewStringData = sNewString.Str();

	for (int i = 0; i < iNumChars; i++)
	{
		pcNewStringData[i] = pcData[i];
	}

	// Remove characters from original string
	if (bCut)
	{
		str.Del(0, iNumChars);
	}

	return sNewString;
}

// Assignment operator
CStrMem& CStrMem::operator=(const CStrMem& nstr)
{
	str   = nstr.str;
	cfill = nstr.cfill;

	return *this;
}

// Assignment operator
CStrMem& CStrMem::operator=(const char* nstr)
{
	if (nstr)
	{
		if ((str = strlen(nstr) + 1))
		{
			strcpy_s(str.Data(), str.Count(), nstr);
		}
	}
	else
	{
		str    = 1;
		str[0] = 0;
	}

	return *this;
}

// Assignment operator
CStrMem& CStrMem::operator=(const char a)
{
	str    = 2;
	str[0] = a;
	str[1] = 0;

	return *this;
}

// Assignment operator
CStrMem& CStrMem::operator=(const long a)
{
	char hstr[_MAXSTRSIZE_];

	sprintf_s(hstr, _MAXSTRSIZE_, "%ld", a);
	*this = hstr;

	return *this;
}

// Assignment operator
CStrMem& CStrMem::operator=(const int a)
{
	char hstr[_MAXSTRSIZE_];

	sprintf_s(hstr, _MAXSTRSIZE_, "%d", a);
	*this = hstr;

	return *this;
}

// Assignment operator
CStrMem& CStrMem::operator=(const unsigned int a)
{
	char hstr[_MAXSTRSIZE_];

	sprintf_s(hstr, _MAXSTRSIZE_, "%u", a);
	*this = hstr;

	return *this;
}

// Assignment operator
CStrMem& CStrMem::operator=(const double a)
{
	char hstr[_MAXSTRSIZE_];

	sprintf_s(hstr, _MAXSTRSIZE_, "%lg", a);
	*this = hstr;

	return *this;
}

// Assignment operator
CStrMem& CStrMem::operator=(const long double a)
{
	char hstr[_MAXSTRSIZE_];

	sprintf_s(hstr, _MAXSTRSIZE_, "%Lg", a);
	*this = hstr;

	return *this;
}

// Assignment operator
CStrMem& CStrMem::operator=(const void* a)
{
	char hstr[_MAXSTRSIZE_];

	sprintf_s(hstr, _MAXSTRSIZE_, "%p", a);
	*this = hstr;

	return *this;
}

// Test whether char is a format char
int isFormat(char c)
{
	if (!strchr(_CSTR_FORMAT_CHARS_, c)) { return 0; }
	else{ return 1; }
}

// Comparison Operator
int operator==(const CStrMem& a, const CStrMem& b)
{
	return !strcmp(a.str.Data(), b.str.Data());
}

// NOT Comparison operator. Same as strcmp
int operator!=(const CStrMem& a, const CStrMem& b)
{
	return strcmp(a.str.Data(), b.str.Data());
}

// Comparison Operator
int operator==(const CStrMem& a, const char* b)
{
	return !strcmp(a.str.Data(), b);
}

// NOT Comparison operator. Same as strcmp
int operator!=(const CStrMem& a, const char* b)
{
	return strcmp(a.str.Data(), b);
}

// Comparison Operator
int operator==(const char* b, const CStrMem& a)
{
	return !strcmp(a.str.Data(), b);
}

// NOT Comparison operator. Same as strcmp
int operator!=(const char* b, const CStrMem& a)
{
	return strcmp(a.str.Data(), b);
}

// Searches for a in b from left to right
size_t operator>(const char a, const CStrMem& b)
{
	char* pos;
	CStrMem str;

	str = b;

	pos = strchr(str, a);

	if (!pos) { return 0; }
	else{ return ((char*) pos - (char*) str.Str()) + 1; }
}

// Searches for a in b from right to left
size_t operator<(const char a, const CStrMem& b)
{
	char* pos;
	CStrMem str;

	str = b;

	pos = strrchr(str, a);

	if (!pos) { return 0; }
	else{ return ((char*) pos - (char*) str.Str()) + 1; }
}

// Capitalize string
CStrMem operator!(const CStrMem& a)
{
	CStrMem b;
	char* s;
	int i = 0;

	b = a;
	s = b.str.Data();

	while (s[i] != 0)
	{
		s[i] = (char) toupper(s[i]);
		i++;
	}

	return b;
}

// Lower case string
CStrMem operator-(const CStrMem& a)
{
	CStrMem b;
	char* s;
	int i = 0;

	b = a;
	s = b.str.Data();

	while (s[i] != 0)
	{
		s[i] = (char) tolower(s[i]);
		i++;
	}

	return b;
}

// Reverse String
CStrMem operator~(const CStrMem& a)
{
	CStrMem b;
	char* s, c;
	size_t l, n, i, j;

	b = a;
	l = b.Len();
	if (l <= 1) { return b; }

	n = l >> 1;
	s = b.str.Data();

	for (i = 0, j = l - 1; i < n; i++, j--)
	{
		c    = s[i];
		s[i] = s[j];
		s[j] = c;
	}

	return b;
}

// Concat Strings
CStrMem operator+(const CStrMem& a, const CStrMem& b)
{
	CStrMem c, d;
	c = a;
	d = b;

	if ((c.str += strlen(d.str.Data())))
	{
		strcat_s(c.str.Data(), c.str.Count(), d.str.Data());
	}

	return c;
}

// Concat to this String
CStrMem& operator+=(CStrMem& a, const CStrMem& b)
{
	CStrMem c;
	c = b;

	if ((a.str += strlen(c.str.Data())))
	{
		strcat_s(a.str.Data(), a.str.Count(), c.str.Data());
	}

	return a;
}

// Concat Strings
CStrMem operator+(const CStrMem& a, const char* b)
{
	CStrMem c;
	c = a;

	if ((c.str += strlen(b)))
	{
		strcat_s(c.str.Data(), c.str.Count(), b);
	}

	return c;
}

// Concat Strings
CStrMem operator+(const char* b, const CStrMem& a)
{
	CStrMem c, d;
	c = b;
	d = a;

	if ((c.str += strlen(d.str.Data())))
	{
		strcat_s(c.str.Data(), c.str.Count(), d.str.Data());
	}

	return c;
}

// Concat to this String
CStrMem& operator+=(CStrMem& a, const char* b)
{
	if ((a.str += strlen(b)))
	{
		strcat_s(a.str.Data(), a.str.Count(), b);
	}

	return a;
}

// add char
CStrMem operator+(const CStrMem& a, const char b)
{
	CStrMem c;
	c = a;

	size_t no = c.str.Count();

	c.str        += 1;
	c.str[no - 1] = b;
	c.str[no]     = 0;

	return c;
}

// add char
CStrMem operator+(const char b, const CStrMem& a)
{
	CStrMem c;

	c.str   += 1;
	c.str[0] = b;
	c.str[1] = 0;

	c += a;

	return c;
}

// Add char to this String
CStrMem& operator+=(CStrMem& a, const char b)
{
	size_t no = a.str.Count();

	a.str        += 1;
	a.str[no - 1] = b;
	a.str[no]     = 0;

	return a;
}

// Concat uint to string
CStrMem operator+(const CStrMem& a, const uint b)
{
	CStrMem c;
	char hstr[_MAXSTRSIZE_];

	c = a;

	sprintf_s(hstr, _MAXSTRSIZE_, "%u", b);
	c += hstr;

	return c;
}

// Concat string to uint
CStrMem operator+(const uint b, const CStrMem& a)
{
	CStrMem c;
	char hstr[_MAXSTRSIZE_];

	c = a;

	sprintf_s(hstr, _MAXSTRSIZE_, "%u", b);
	c = hstr + c;

	return c;
}

// Concat int to this string
CStrMem& operator+=(CStrMem& a, const uint b)
{
	char hstr[_MAXSTRSIZE_];

	sprintf_s(hstr, _MAXSTRSIZE_, "%u", b);
	a += hstr;

	return a;
}

// Concat unsigned long to string
CStrMem operator+(const CStrMem& a, const unsigned long b)
{
	CStrMem c;
	char hstr[_MAXSTRSIZE_];

	c = a;

	sprintf_s(hstr, _MAXSTRSIZE_, "%lu", b);
	c += hstr;

	return c;
}

// Concat string to unsigned long
CStrMem operator+(const unsigned long b, const CStrMem& a)
{
	CStrMem c;
	char hstr[_MAXSTRSIZE_];

	c = a;

	sprintf_s(hstr, _MAXSTRSIZE_, "%lu", b);
	c = hstr + c;

	return c;
}

// Concat unsigned long to this string
CStrMem& operator+=(CStrMem& a, const unsigned long b)
{
	char hstr[_MAXSTRSIZE_];

	sprintf_s(hstr, _MAXSTRSIZE_, "%lu", b);
	a += hstr;

	return a;
}

// Concat int to string
CStrMem operator+(const CStrMem& a, const int b)
{
	CStrMem c;
	char hstr[_MAXSTRSIZE_];

	c = a;

	sprintf_s(hstr, _MAXSTRSIZE_, "%d", b);
	c += hstr;

	return c;
}

// Concat string to int
CStrMem operator+(const int b, const CStrMem& a)
{
	CStrMem c;
	char hstr[_MAXSTRSIZE_];

	c = a;

	sprintf_s(hstr, _MAXSTRSIZE_, "%d", b);
	c = hstr + c;

	return c;
}

// Concat int to this string
CStrMem& operator+=(CStrMem& a, const int b)
{
	char hstr[_MAXSTRSIZE_];

	sprintf_s(hstr, _MAXSTRSIZE_, "%d", b);
	a += hstr;

	return a;
}

// Concat int to string
CStrMem operator+(const CStrMem& a, const long b)
{
	CStrMem c;
	char hstr[_MAXSTRSIZE_];

	c = a;

	sprintf_s(hstr, _MAXSTRSIZE_, "%ld", b);
	c += hstr;

	return c;
}

// Concat string to int
CStrMem operator+(const long b, const CStrMem& a)
{
	CStrMem c;
	char hstr[_MAXSTRSIZE_];

	c = a;

	sprintf_s(hstr, _MAXSTRSIZE_, "%ld", b);
	c = hstr + c;

	return c;
}

// Concat int to this string
CStrMem& operator+=(CStrMem& a, const long b)
{
	char hstr[_MAXSTRSIZE_];

	sprintf_s(hstr, _MAXSTRSIZE_, "%ld", b);
	a += hstr;

	return a;
}

// Concat double to string
CStrMem operator+(const CStrMem& a, const double b)
{
	CStrMem c;
	char hstr[_MAXSTRSIZE_];

	c = a;

	sprintf_s(hstr, _MAXSTRSIZE_,  "%lg", b);
	c += hstr;

	return c;
}

// Concat string to double
CStrMem operator+(const double b, const CStrMem& a)
{
	CStrMem c;
	char hstr[_MAXSTRSIZE_];

	c = a;

	sprintf_s(hstr, _MAXSTRSIZE_, "%lg", b);
	c = hstr + c;

	return c;
}

// Concat double to this string
CStrMem& operator+=(CStrMem& a, const double b)
{
	char hstr[_MAXSTRSIZE_];

	sprintf_s(hstr, _MAXSTRSIZE_, "%lg", b);
	a += hstr;

	return a;
}

// Concat long double to string
CStrMem operator+(const CStrMem& a, const long double b)
{
	CStrMem c;
	char hstr[_MAXSTRSIZE_];

	c = a;

	sprintf_s(hstr, _MAXSTRSIZE_, "%Lg", b);
	c += hstr;

	return c;
}

// Concat string to long double
CStrMem operator+(const long double b, const CStrMem& a)
{
	CStrMem c;
	char hstr[_MAXSTRSIZE_];

	c = a;

	sprintf_s(hstr, _MAXSTRSIZE_, "%Lg", b);
	c = hstr + c;

	return c;
}

// Concat double to this string
CStrMem& operator+=(CStrMem& a, const long double b)
{
	char hstr[_MAXSTRSIZE_];

	sprintf_s(hstr, _MAXSTRSIZE_, "%Lg", b);
	a += hstr;

	return a;
}

// Stream cStr
CStrMem& operator<<(CStrMem& a, const CStrMem& b)
{
	a += b;

	return a;
}

// Stream char*
CStrMem& operator<<(CStrMem& a, const char* b)
{
	a += b;

	return a;
}

// Stream char
CStrMem& operator<<(CStrMem& a, const char b)
{
	a += b;

	return a;
}

// Stream uint
CStrMem& operator<<(CStrMem& a, const uint b)
{
	a += b;

	return a;
}

// Stream ulong
CStrMem& operator<<(CStrMem& a, const unsigned long b)
{
	a += b;

	return a;
}

// Stream int
CStrMem& operator<<(CStrMem& a, const int b)
{
	a += b;

	return a;
}

// Stream long
CStrMem& operator<<(CStrMem& a, const long b)
{
	a += b;

	return a;
}

// Stream double
CStrMem& operator<<(CStrMem& a, const double b)
{
	a += b;

	return a;
}

// Stream long double
CStrMem& operator<<(CStrMem& a, const long double b)
{
	a += b;

	return a;
}

//********************************************************************
// % operator ********************************************************
//********************************************************************

CStrMem& CStrMem::MakeFormatStr(char* ts)
{
	size_t l = Len();

	if (!l) { (*this = "%") + ts; }
	else if (!isFormat(str[l - 1]))
	{
		*this = "%" + *this + ts;
	}
	else{ *this = "%" + *this; }

	return *this;
}

// Format variable
CStrMem operator%(const int a, const CStrMem& b)
{
	CStrMem str(b);
	char hstr[_MAXSTRSIZE_];

	sprintf_s(hstr, _MAXSTRSIZE_, (char*) str.MakeFormatStr(StrType(a)), a);

	return str = hstr;
}

// Format variable
CStrMem operator%(const long a, const CStrMem& b)
{
	CStrMem str(b);
	char hstr[_MAXSTRSIZE_];

	sprintf_s(hstr, _MAXSTRSIZE_, (char*) str.MakeFormatStr(StrType(a)), a);

	return str = hstr;
}

// Format double
CStrMem operator%(const double a, const CStrMem& b)
{
	CStrMem str(b);
	char hstr[_MAXSTRSIZE_];

	sprintf_s(hstr, _MAXSTRSIZE_, (char*) str.MakeFormatStr(StrType(a)), a);

	return str = hstr;
}

// Format unsigned int
CStrMem operator%(const long double a, const CStrMem& b)
{
	CStrMem str(b);
	char hstr[_MAXSTRSIZE_];

	sprintf_s(hstr, _MAXSTRSIZE_, (char*) str.MakeFormatStr(StrType(a)), a);

	return str = hstr;
}

// Format variable
CStrMem operator%(const uint a, const CStrMem& b)
{
	CStrMem str(b);
	char hstr[_MAXSTRSIZE_];

	sprintf_s(hstr, _MAXSTRSIZE_, (char*) str.MakeFormatStr(StrType(a)), a);

	return str = hstr;
}

// Format variable
CStrMem operator%(const unsigned long a, const CStrMem& b)
{
	CStrMem str(b);
	char hstr[_MAXSTRSIZE_];

	sprintf_s(hstr, _MAXSTRSIZE_, (char*) str.MakeFormatStr(StrType(a)), a);

	return str = hstr;
}

//********************************************************************
// * operator ********************************************************
//********************************************************************

// Multiply string n times
CStrMem operator*(const CStrMem& a, size_t n)
{
	CStrMem f;

	if (!n)
	{
		return f;
	}

	for (size_t i = 0; i < n; i++)
	{
		f += a;
	}

	return f;
}

// Multiply string n times
CStrMem operator*(size_t n, const CStrMem& a)
{
	CStrMem f;

	if (!n)
	{
		return f;
	}

	for (size_t i = 0; i < n; i++)
	{
		f += a;
	}

	return f;
}

// Multiply this string n times
CStrMem& operator*=(CStrMem& a, size_t n)
{
	CStrMem f;

	if (!n)
	{
		return a;
	}

	for (size_t i = 0; i < n; i++)
	{
		f += a;
	}

	a = f;
	return a;
}

// Add or sub n fill chars to right of string
CStrMem operator|(const CStrMem& a, int n)
{
	CStrMem b;
	char* s;
	size_t f;

	b = a;
	f = b.Len();

	if (!n) { return b; }

	if (n > 0)
	{
		if ((b.str += n))
		{
			s = &b.str.Data()[f];
			memset(s, b.cfill, n);
			s[n] = 0;
		}
	}
	else if (n = -n, (n < (int) f))
	{
		b.str.Del(f - n, n);
	}
	else
	{
		b = "";
	}

	return b;
}

// Add or sub n fill chars to left of string
CStrMem operator|(int n, const CStrMem& a)
{
	CStrMem b;
	char* s;
	size_t f;

	b = a;
	f = b.Len();

	if (!n) { return b; }

	if (n > 0)
	{
		if ((b.str.Insert(0, n)))
		{
			s = b.str.Data();
			memset(s, b.cfill, n);
		}
	}
	else if (n = -n, (n < (int) f))
	{
		b.str.Del(0, n);
	}
	else
	{
		b = "";
	}

	return b;
}

// Add n fill chars to right of this string
CStrMem& operator|=(CStrMem& a, int n)
{
	char* s;
	size_t f;

	f = a.Len();

	if (!n) { return a; }

	if (n > 0)
	{
		if ((a.str += n))
		{
			s = &a.str.Data()[f];
			memset(s, a.cfill, n);
			s[n] = 0;
		}
	}
	else if (n = -n, (n < (int) f))
	{
		a.str.Del(f - n, n);
	}
	else
	{
		a = "";
	}

	return a;
}

// Conversion of string to uint
int operator>>(const CStrMem& a, uint& n)
{
	CStrMem b;

	b = a;
	return sscanf_s(b.Str(), "%u", &n);
}

// Conversion of string to ulong
int operator>>(const CStrMem& a, ulong& n)
{
	CStrMem b;

	b = a;
	return sscanf_s(b.Str(), "%lu", &n);
}

// Conversion of string to int
int operator>>(const CStrMem& a, int& n)
{
	CStrMem b;

	b = a;
	return sscanf_s(b.Str(), "%d", &n);
}

// Conversion of string to long
int operator>>(const CStrMem& a, long& n)
{
	CStrMem b;

	b = a;
	return sscanf_s(b.Str(), "%ld", &n);
}

// Conversion of string to float
int operator>>(const CStrMem& a, float& n)
{
	CStrMem b;

	b = a;
	return sscanf_s(b.Str(), "%g", &n);
}

// Conversion of string to double
int operator>>(const CStrMem& a, double& n)
{
	CStrMem b;

	b = a;
	return sscanf_s(b.Str(), "%lg", &n);
}

// Conversion of string to long double
int operator>>(const CStrMem& a, long double& n)
{
	CStrMem b;

	b = a;
	return sscanf_s(b.Str(), "%Lg", &n);
}

// Conversion of string to void pointer
int operator>>(const CStrMem& a, void*& n)
{
	CStrMem b;

	b = a;
	return sscanf_s(b.Str(), "%p", &n);
}
