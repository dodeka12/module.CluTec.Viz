////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.GA.Base
// file:      blade.cxx
//
// summary:   blade class
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

//------------------- Blade Class Library -----------------------

#include "blade.h"

#include "CluTec.Viz.Base\makestr.h"
#include "CluTec.Viz.Base\mathelp.h"

////////////////////////////////////////////////////////////////////////////////////
// Default Constructor

template<class CType>
Blade<CType>::Blade()
{
	m_pxBase = 0;
	m_uGrade = 0;
	m_xFact  = 0;
	m_sStr   = "";
	m_xComps = 0;
	m_sName  = "";
}

////////////////////////////////////////////////////////////////////////////////////
// Constructor

template<class CType>
Blade<CType>::Blade(Base<CType>& nbase, CType nfact, uint ngrade, uint ncomps[], char* nname)
{
	m_pxBase = &nbase;
	uint dim = m_pxBase->VSDim();

	if (dim < ngrade) { ngrade = dim; }

	m_uGrade = ngrade;
	m_xFact  = nfact;
	m_sStr   = "";

	if ((m_xComps = dim) && ncomps)
	{
		memcpy(m_xComps.Data(), ncomps, m_uGrade * sizeof(uint));
	}

	if (nname) { m_sName = nname; }
	else{ m_sName = ""; }

	check();
}

////////////////////////////////////////////////////////////////////////////////////
// Copy Constructor

template<class CType>
Blade<CType>::Blade(const Blade<CType>& a)
{
	m_pxBase = a.m_pxBase;
	uint dim = m_pxBase->VSDim();

	m_uGrade = a.m_uGrade;
	if (dim < m_uGrade) { m_uGrade = dim; }

	m_xFact  = a.m_xFact;
	m_sStr   = "";
	m_xComps = a.m_xComps;
	m_sName  = a.m_sName;

	check();
}

////////////////////////////////////////////////////////////////////////////////////
// Destructor

template<class CType>
Blade<CType>::~Blade()
{
}

////////////////////////////////////////////////////////////////////////////////////
// Reset Blade

template<class CType>
void Blade<CType>::Reset(Base<CType>& nbase, CType nfact, uint ngrade, uint ncomps[], char* nname)
{
	m_pxBase = &nbase;
	uint dim = m_pxBase->VSDim();

	if (dim < ngrade) { ngrade = dim; }

	m_uGrade = ngrade;
	m_xFact  = nfact;
	m_sStr   = "";

	if ((m_xComps = dim) && ncomps)
	{
		memcpy(m_xComps.Data(), ncomps, m_uGrade * sizeof(uint));
	}

	if (nname) { m_sName = nname; }
	else{ m_sName = ""; }

	check();
}

////////////////////////////////////////////////////////////////////////////////////
// Take Out element

template<class CType>
void Blade<CType>::takeOut(size_t pos)
{
	if ((pos >= 0) && (pos < m_uGrade))
	{
		m_uGrade--;
		for (size_t i = pos; i < m_uGrade; i++)
		{
			m_xComps[i] = m_xComps[i + 1];
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////
// Put In element

template<class CType>
void Blade<CType>::putIn(uint e, size_t pos)
{
	size_t dim = m_pxBase->VSDim();

	if ((pos >= 0) && (pos < dim) && (m_uGrade < dim))
	{
		if (pos >= m_uGrade) { pos = m_uGrade; }
		for (size_t i = m_uGrade; i > pos; i--)
		{
			m_xComps[i] = m_xComps[i - 1];
		}

		m_xComps[pos] = e;
		m_uGrade++;
	}

	check();
}

////////////////////////////////////////////////////////////////////////////////////
// Make Blade to Pseudo Scalar

template<class CType>
void Blade<CType>::makePS()
{
	uint dim = m_pxBase->VSDim();
	uint j   = m_pxBase->FirstIdx();

	for (uint i = 0; i < dim; i++, j++)
	{
		m_xComps[i] = j;
	}

	m_uGrade = dim;
	m_xFact  = CType(1);
}

////////////////////////////////////////////////////////////////////////////////////
// Assignment of Scalar

template<class CType>
Blade<CType>& Blade<CType>::operator=(const CType f)
{
	m_xFact  = f;
	m_uGrade = 0;
	check();

	return *this;
}

////////////////////////////////////////////////////////////////////////////////////
// Assignment of a Blade part; first element in ncomps[] is grade

template<class CType>
Blade<CType>& Blade<CType>::operator=(const uint ncomps[])
{
	if (ncomps[0] > m_pxBase->VSDim()) { return *this; }

	for (uint i = 0; i < ncomps[0]; i++)
	{
		m_xComps[i] = ncomps[i + 1];
	}

	check();
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////
// Assignment of Blade

template<class CType>
Blade<CType>& Blade<CType>::operator=(const Blade<CType>& a)
{
	m_pxBase = a.m_pxBase;
	m_xComps = a.m_xComps;

	m_uGrade = a.m_uGrade;
	m_xFact  = a.m_xFact;

	m_sName = a.m_sName;

	check();
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////
// Comparing Blades but ignoring prefactor
// Gives: 0 if blades are not equal
//        1 if blades are even permutation of each other
//       -1 if blades are odd permutation of each other

template<class CType>
int /*Blade<CType>::*/ operator==(const Blade<CType>& c, const Blade<CType>& d)
{
	int f = 1;
	int j;
	Blade<CType> a(c.m_pxBase[0]), b(d.m_pxBase[0]);
	a = c;
	b = d;

	if (a.m_uGrade != b.m_uGrade) { return 0; }

	j = 0;
	do
	{
		if (a.m_xComps[0] == b.m_xComps[j])
		{
			f *= 1 - 2 * (j % 2);
			a.takeOut(0);
			b.takeOut(j);
			j = -1;
		}
		j++;
	}
	while (a.m_uGrade && b.m_uGrade && uint(j) < b.m_uGrade);

	if (a.m_uGrade || b.m_uGrade) { return 0; }

	return f;
}

////////////////////////////////////////////////////////////////////////////////////
// Check Blade
// Gives: 1 if blade was changed
//        0 if blade was not changed

template<class CType>
int Blade<CType>::check()
{
	if ((m_xFact == CType(0)) && (m_uGrade != 0))
	{
		m_uGrade = 0;
		return 1;
	}

	size_t i, j;
	size_t f = m_pxBase->FirstIdx();
	size_t l = f + m_pxBase->VSDim() - 1;

	for (i = 0; i < m_uGrade; i++)
	{
		if ((m_xComps[i] < f) || (m_xComps[i] > l))
		{
			m_uGrade = 0;
			m_xFact  = 0;
			return 1;
		}
	}

	for (i = 0; i < m_uGrade; i++)
	{
		for (j = i + 1; j < m_uGrade; j++)
		{
			if (m_xComps[i] == m_xComps[j])
			{
				m_uGrade = 0;
				m_xFact  = 0;
				return 1;
			}
		}
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////
// Addition of two Blades +. Gives own Blade if Blades cannot be added

template<class CType>
Blade<CType> /*Blade<CType>::*/ operator+(const Blade<CType>& a, const Blade<CType>& b)
{
	int r;
	Blade<CType> c(a.m_pxBase[0]);
	c = a;

	if ((r = (a == b)))
	{
		c.m_xFact += CType(r) * b.m_xFact;
	}

	c.check();
	return c;
}

////////////////////////////////////////////////////////////////////////////////////
// Addition of two Blades +=. Gives own Blade if Blades cannot be added

template<class CType>
Blade<CType>& /*Blade<CType>::*/ operator+=(Blade<CType>& a, const Blade<CType>& b)
{
	Blade<CType> c(b);
	int r;

	if ((r = (a == b)))
	{
		a.m_xFact += CType(r) * b.m_xFact;
	}

	a.check();

	return a;
}

////////////////////////////////////////////////////////////////////////////////////
// Subtraction of two Blades -. Gives own Blade if Blades cannot be added

template<class CType>
Blade<CType> /*Blade<CType>::*/ operator-(const Blade<CType>& a, const Blade<CType>& b)
{
	int r;
	Blade<CType> c(a.m_pxBase[0]);
	c = a;

	if ((r = (a == b)))
	{
		c.m_xFact -= CType(r) * b.m_xFact;
	}

	c.check();
	return c;
}

////////////////////////////////////////////////////////////////////////////////////
// Subtraction of two Blades -=. Gives own Blade if Blades cannot be added

template<class CType>
Blade<CType>& /*Blade<CType>::*/ operator-=(Blade<CType>& a, const Blade<CType>& b)
{
	int r;

	if ((r = (a == b)))
	{
		a.m_xFact -= CType(r) * b.m_xFact;
	}

	a.check();
	return a;
}

////////////////////////////////////////////////////////////////////////////////////
// Multiplication with Prefactor

template<class CType>
Blade<CType> /*Blade<CType>::*/ operator^(const Blade<CType>& a, const CType& b)
{
	Blade<CType> c(a.m_pxBase[0]);
	c = a;

	c.m_xFact *= b;

	return c;
}

////////////////////////////////////////////////////////////////////////////////////
// Multiplication of Prefactor with Blade

template<class CType>
Blade<CType> /*Blade<CType>::*/ operator^(const CType& b, const Blade<CType>& a)
{
	Blade<CType> c(a.m_pxBase[0]);
	c = a;

	c.m_xFact *= b;

	return c;
}

////////////////////////////////////////////////////////////////////////////////////
// Multiplication of this Blade with Prefactor

template<class CType>
Blade<CType>& /*Blade<CType>::*/ operator^=(Blade<CType>& a, const CType& b)
{
	a.m_xFact *= b;

	return a;
}

////////////////////////////////////////////////////////////////////////////////////
// Negation of Blade

template<class CType>
Blade<CType> /*Blade<CType>::*/ operator-(const Blade<CType>& a)
{
	Blade<CType> b(a.m_pxBase[0]);
	b = a;

	b.m_xFact *= CType(-1);

	return b;
}

////////////////////////////////////////////////////////////////////////////////////
// Reverse of Blade

template<class CType>
Blade<CType> /*Blade<CType>::*/ operator~(const Blade<CType>& a)
{
	Blade<CType> b(a.m_pxBase[0]);
	b = a;

	for (uint i = 0; i < b.m_uGrade; i++)
	{
		b.m_xComps[i] = a.m_xComps[b.m_uGrade - i - 1];
	}

	b.m_sName = "";

	return b;
}

////////////////////////////////////////////////////////////////////////////////////
// Inverse of Blade

template<class CType>
Blade<CType> /*Blade<CType>::*/ operator!(const Blade<CType>& a)
{
	CType f = CType(1);
	Blade<CType> b(a.m_pxBase[0]);
	b = a;

	for (uint i = 0; i < b.m_uGrade; i++)
	{
		b.m_xComps[i] = a.m_xComps[b.m_uGrade - i - 1];
		f            *= a.m_pxBase->signature(b.m_xComps[i]);
	}

	b.m_xFact = f / a.m_xFact;

	b.m_sName = "";

	return b;
}

////////////////////////////////////////////////////////////////////////////////////
// Geometric Product

template<class CType>
Blade<CType> /*Blade<CType>::*/ operator&(const Blade<CType>& c, const Blade<CType>& d)
{
	//  printf("\n\nGeometric Prod. of two Blades -----------------\n\n");

	size_t dim = c.m_pxBase->VSDim();
	size_t i, j, r;

	Blade<CType> e(c.m_pxBase[0]);
	Blade<CType> a(c.m_pxBase[0]), b(d.m_pxBase[0]);

	a = c;
	b = d;

	e.m_xFact = a.m_xFact * b.m_xFact;

	//  printf("FIRST>> a: %s, b: %s, e: %s\n\n", a.Str(), b.Str(), e.Str());

	for (i = 0; i < int(a.m_uGrade); i++)
	{
		for (j = 0; j < int(b.m_uGrade); j++)
		{
			if (a.m_xComps[i] == b.m_xComps[j])
			{
				r          = 1 - 2 * ((a.m_uGrade - i - 1 + j) % 2);
				e.m_xFact *= CType(r) * (a.m_pxBase->signature(a.m_xComps[i]));
				a.takeOut(i);
				b.takeOut(j);
				/*
				printf("a: %s, b: %s, e: %s, r: %d\n",
				a.Str(BS_EXPLICIT), b.Str(BS_EXPLICIT),
				e.Str(BS_EXPLICIT), r);
				*/
				i = -1;
				break;
			}
		}
	}

	for (i = 0; i < int(a.m_uGrade); i++)
	{
		e.m_xComps[i] = a.m_xComps[i];
	}

	for (i = 0, j = a.m_uGrade; i < int(b.m_uGrade) && j < dim; i++, j++)
	{
		e.m_xComps[j] = b.m_xComps[i];
	}

	e.m_uGrade = a.m_uGrade + b.m_uGrade;

	//  printf("\nLAST>> a: %s, b: %s, e: %s\n", a.Str(), b.Str(), e.Str());

	return e;
}

////////////////////////////////////////////////////////////////////////////////////
// Inner Product

template<class CType>
Blade<CType> /*Blade<CType>::*/ operator*(const Blade<CType>& c, const Blade<CType>& d)
{
	int p = 1;
	uint i, j, r;
	Blade<CType> e(c.m_pxBase[0]);
	Blade<CType> a(c.m_pxBase[0]), b(d.m_pxBase[0]);
	a = c;
	b = d;

	if (!a.m_uGrade && !b.m_uGrade)
	{
		e.m_xFact = CType(0);
		return e;
	}
	else if (!a.m_uGrade || !b.m_uGrade)
	{
		return e;
	}

	for (i = 0; i < int(a.m_uGrade); i++)
	{
		for (j = 0; j < int(b.m_uGrade); j++)
		{
			if (a.m_xComps[i] == b.m_xComps[j])
			{
				r  = (1 - 2 * ((a.m_uGrade - i - 1 + j) % 2));
				p *= r * int(a.m_pxBase->signature(a.m_xComps[i]));
				a.takeOut(i);
				b.takeOut(j);
				i = -1;
				break;
			}
		}
	}

	if (!a.m_uGrade || !b.m_uGrade)
	{
		e.m_xFact = a.m_xFact * b.m_xFact * p;

		if (a.m_uGrade)
		{
			for (i = 0; i < int(a.m_uGrade); i++)
			{
				e.m_xComps[i] = a.m_xComps[i];
			}

			e.m_uGrade = a.m_uGrade;
		}
		else
		{
			for (i = 0; i < int(b.m_uGrade); i++)
			{
				e.m_xComps[i] = b.m_xComps[i];
			}

			e.m_uGrade = b.m_uGrade;
		}
	}

	return e;
}

////////////////////////////////////////////////////////////////////////////////////
// Outer Product

template<class CType>
Blade<CType> /*Blade<CType>::*/ operator^(const Blade<CType>& a, const Blade<CType>& b)
{
	size_t dim = a.m_pxBase->VSDim();
	size_t i, j;
	Blade<CType> c(a.m_pxBase[0]);

	c.m_xFact = a.m_xFact * b.m_xFact;

	for (i = 0; i < int(a.m_uGrade); i++)
	{
		for (j = 0; j < int(b.m_uGrade); j++)
		{
			if (a.m_xComps[i] == b.m_xComps[j])
			{
				c.m_xFact = 0;
				i         = a.m_uGrade;
				break;
			}
		}
	}

	if (c.m_xFact)
	{
		for (i = 0; i < int(a.m_uGrade); i++)
		{
			c.m_xComps[i] = a.m_xComps[i];
		}

		for (i = 0, j = a.m_uGrade; i < int(b.m_uGrade) && j < dim; i++, j++)
		{
			c.m_xComps[j] = b.m_xComps[i];
		}

		c.m_uGrade = a.m_uGrade + b.m_uGrade;
	}

	return c;
}

////////////////////////////////////////////////////////////////////////////////////
// The JOIN of two Blades

template<class CType>
Blade<CType> /*Blade<CType>::*/ join(const Blade<CType>& a,		 const Blade<CType>& b)
{
	uint i, j;
	uint dim = a.m_pxBase->VSDim();
	Blade<CType> c(a.m_pxBase[0]), d(a.m_pxBase[0]);
	c = b;

	for (i = 0; i < a.m_uGrade && i < dim; i++)
	{
		for (j = 0; j < c.m_uGrade; j++)
		{
			if (a.m_xComps[i] == c.m_xComps[j])
			{
				c.takeOut(j);
				break;
			}
		}

		d.m_xComps[i] = a.m_xComps[i];
	}

	for (j = 0; j < c.m_uGrade && i < dim; j++, i++)
	{
		d.m_xComps[i] = c.m_xComps[j];
	}

	d.m_uGrade = i;
	if (i)
	{
		d.m_xFact = CType(1);	// "a.fact * b.fact;" is WRONG for
	}
	// projective geometry calculations, when the join is needed
	// to create the dual. However, the above, gives the right results.

	d.check();
	return d;
}

////////////////////////////////////////////////////////////////////////////////////
// The MEET of two Blades

template<class CType>
Blade<CType> /*Blade<CType>::*/ meet(const Blade<CType>& a,		 const Blade<CType>& b)
{
	Blade<CType> c(a.m_pxBase[0]), d(a.m_pxBase[0]), e(a.m_pxBase[0]);
	Blade<CType> I(a.m_pxBase[0]), Ii(a.m_pxBase[0]);

	I  = join(a, b);// Join
	Ii = !I;// Inverse of Join

	c = ((a & Ii) ^ (b & Ii)) & I;	// `deMorgan' Rule

	if (c.m_uGrade == 0) { c.m_xFact = CType(0); }

	return c;
}

////////////////////////////////////////////////////////////////////////////////////
// Commutator Product

template<class CType>
Blade<CType> cp(const Blade<CType>& a, const Blade<CType>& b)
{
	return (a & b) - (b & a);
}

////////////////////////////////////////////////////////////////////////////////////
// Anti-Commutator Product

template<class CType>
Blade<CType> acp(const Blade<CType>& a, const Blade<CType>& b)
{
	return (a & b) + (b & a);
}

////////////////////////////////////////////////////////////////////////////////////
// String output of blade

template<class CType>
char* Blade<CType>::Str(uint strtype)
{
	size_t len;
	CStrMem vname = m_pxBase->VName();

	m_sStr = MakeStr(CType(m_xFact));

	if (!m_sName.Len() || (strtype == BS_EXPLICIT))
	{
		m_sStr << "(";

		for (uint i = 0; i < m_uGrade; i++)
		{
			m_sStr << vname << m_xComps[i] << "^";
		}

		len = m_sStr.Len();
		if (m_sStr[len - 1] == '^') { m_sStr[len - 1] = ')'; }
		else{ m_sStr << ")"; }
	}
	else{ m_sStr << "^" << m_sName; }

	return m_sStr;
}
