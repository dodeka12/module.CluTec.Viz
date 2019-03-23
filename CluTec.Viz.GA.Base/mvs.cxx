////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.GA.Base
// file:      mvs.cxx
//
// summary:   mvs class
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

//--------------- Multi-Vector-Style Library ---------------------

#include "mvs.h"

#include "CluTec.Viz.Base\makestr.h"
#include "CluTec.Viz.Base\mathelp.h"

////////////////////////////////////////////////////////////////////////////////////
// Constructor

template<class CType>
MultiVStyle<CType>::MultiVStyle(BladeList<CType>& nstyle)
{
	base    = nstyle.BasePtr();
	dim     = base->VSDim();
	gadim   = base->GADim();
	tblsize = gadim * gadim;

	if (nstyle.Count() == gadim)
	{
		style     = new BladeList<CType>(nstyle);
		gpTable   = new short[tblsize];
		ipTable   = new short[tblsize];
		opTable   = new short[tblsize];
		revTable  = new short[gadim];
		dualTable = new short[gadim];
		invoTable = new short[gadim];
		viTable   = new short[gadim];
		grades    = new short[gadim];

		makeGPTable();
		makeIPTable();
		makeOPTable();
		makeDualTable();
		makeInvoTable();
		makeVecInvTable();
		getGrades();
	}
	else
	{
		style     = 0;
		gpTable   = 0;
		ipTable   = 0;
		opTable   = 0;
		dualTable = 0;
		invoTable = 0;
		viTable   = 0;
		grades    = 0;
	}
}

////////////////////////////////////////////////////////////////////////////////////
// Default Constructor

template<class CType>
MultiVStyle<CType>::MultiVStyle()
{
	base    = 0;
	dim     = 0;
	gadim   = 0;
	tblsize = 0;

	style     = 0;
	gpTable   = 0;
	ipTable   = 0;
	opTable   = 0;
	dualTable = 0;
	invoTable = 0;
	viTable   = 0;
	grades    = 0;
}

////////////////////////////////////////////////////////////////////////////////////
// Destructor

template<class CType>
MultiVStyle<CType>::~MultiVStyle()
{
	if (style) { delete style; }
	if (gpTable) { delete[] gpTable; }
	if (ipTable) { delete[] ipTable; }
	if (opTable) { delete[] opTable; }
	if (revTable) { delete[] revTable; }
	if (dualTable) { delete[] dualTable; }
	if (invoTable) { delete[] invoTable; }
	if (viTable) { delete[] viTable; }
	if (grades) { delete[] grades; }
}

////////////////////////////////////////////////////////////////////////////////////
// Assignment Operator
template<class CType>
MultiVStyle<CType>& MultiVStyle<CType>::Init(BladeList<CType>& nstyle, short* _grades, short* _gpTable, short* _ipTable, short* _opTable, short* _viTable, short* _dualTable, short* _invoTable)
{
	base    = nstyle.BasePtr();
	dim     = base->VSDim();
	gadim   = base->GADim();
	tblsize = gadim * gadim;

	if (nstyle.Count() == gadim)
	{
		if (style) { delete style; }
		if (gpTable) { delete[] gpTable; }
		if (ipTable) { delete[] ipTable; }
		if (opTable) { delete[] opTable; }
		if (revTable) { delete[] revTable; }
		if (dualTable) { delete[] dualTable; }
		if (invoTable) { delete[] invoTable; }
		if (viTable) { delete[] viTable; }
		if (grades) { delete[] grades; }

		style     = new BladeList<CType>(nstyle);
		gpTable   = new short[tblsize];
		ipTable   = new short[tblsize];
		opTable   = new short[tblsize];
		revTable  = new short[gadim];
		dualTable = new short[gadim];
		invoTable = new short[gadim];
		viTable   = new short[gadim];
		grades    = new short[gadim];

		if (_gpTable) { memcpy(gpTable, _gpTable, tblsize * sizeof(short)); }
		else{ makeGPTable(); }

		if (_ipTable) { memcpy(ipTable, _ipTable, tblsize * sizeof(short)); }
		else{ makeIPTable(); }

		if (_opTable) { memcpy(opTable, _opTable, tblsize * sizeof(short)); }
		else{ makeOPTable(); }

		if (_dualTable) { memcpy(dualTable, _dualTable, gadim * sizeof(short)); }
		else{ makeDualTable(); }

		if (_invoTable) { memcpy(invoTable, _invoTable, gadim * sizeof(short)); }
		else{ makeInvoTable(); }

		if (_viTable) { memcpy(viTable, _viTable, gadim * sizeof(short)); }
		else{ makeVecInvTable(); }

		if (_grades) { memcpy(grades, _grades, gadim * sizeof(short)); }
		else{ getGrades(); }

		makeReverseTable();
	}
	else
	{
		if (style) { delete style; }
		if (gpTable) { delete[] gpTable; }
		if (ipTable) { delete[] ipTable; }
		if (opTable) { delete[] opTable; }
		if (revTable) { delete[] revTable; }
		if (dualTable) { delete[] dualTable; }
		if (invoTable) { delete[] invoTable; }
		if (viTable) { delete[] viTable; }
		if (grades) { delete[] grades; }

		style     = 0;
		gpTable   = 0;
		ipTable   = 0;
		opTable   = 0;
		revTable  = 0;
		dualTable = 0;
		invoTable = 0;
		viTable   = 0;
		grades    = 0;
	}

	return this[0];
}

////////////////////////////////////////////////////////////////////////////////////
// Make Reverse Table

template<class CType>
void MultiVStyle<CType>::makeReverseTable()
{
	if (!revTable) { return; }

	int iGrade;
	for (uint i = 0; i < gadim; i++)
	{
		iGrade = grades[i];
		if ((((iGrade * (iGrade - 1)) >> 1) % 2))
		{
			revTable[i] = -short(i + 1);
		}
		else
		{
			revTable[i] = short(i + 1);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////
// Make Involution Table

template<class CType>
void MultiVStyle<CType>::makeInvoTable()
{
	if (!invoTable) { return; }

	Blade<CType> a(base[0]);
	uint i;
	int r;

	//  printf("Make Dual Table ------------------------");
	//  printf("\nb: %s\n\n", b.Str());

	for (i = 0; i < gadim; i++)
	{
		a            = style[0][i] & (~style[0][i]);
		r            = (a.Factor() < 0);
		invoTable[i] = short(r);
		//        printf("\na: %s, s: %s, b: %s, r: %d, dt: %d\n",a.Str(),
		//               style[0][i].Str(), b.Str(), r, dualTable[i]);
	}
}

////////////////////////////////////////////////////////////////////////////////////
// Make Dual Table

template<class CType>
void MultiVStyle<CType>::makeDualTable()
{
	if (!dualTable) { return; }

	Blade<CType> a(base[0]);
	Blade<CType> b(base[0]);
	size_t i;
	int r;

	b.makePS();
	b = !b;	// Inverse of Pseudo Scalar

	//  printf("Make Dual Table ------------------------");
	//  printf("\nb: %s\n\n", b.Str());

	for (i = 0; i < gadim; i++)
	{
		a            = style[0][i] & b;
		r            = (style[0] == a);
		dualTable[i] = short(r * int(style[0][((r < 0) ? (-r) : (r)) - 1].Factor()) * int(a.Factor()));
		//        printf("\na: %s, s: %s, b: %s, r: %d, dt: %d\n",a.Str(),
		//               style[0][i].Str(), b.Str(), r, dualTable[i]);
	}
}

////////////////////////////////////////////////////////////////////////////////////
// Make GP Table

template<class CType>
void MultiVStyle<CType>::makeGPTable()
{
	if (!gpTable) { return; }

	Blade<CType> a(base[0]);
	uint i, j, k;
	int r;

	for (i = 0, k = 0; i < gadim; i++)
	{
		for (j = 0; j < gadim; j++, k++)
		{
			a          = style[0][i] & style[0][j];
			r          = (style[0] == a);
			gpTable[k] = short(r * int(style[0][((r < 0) ? (-r) : (r)) - 1].Factor())
					   * int(a.Factor()));
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////
// Make IP Table

template<class CType>
void MultiVStyle<CType>::makeIPTable()
{
	if (!ipTable) { return; }

	Blade<CType> a(base[0]);
	uint i, j, k;
	int r;

	for (i = 0, k = 0; i < gadim; i++)
	{
		for (j = 0; j < gadim; j++, k++)
		{
			a = style[0][i] * style[0][j];
			if (a.Factor() == CType(0)) { ipTable[k] = 0; }
			else
			{
				r          = (style[0] == a);
				ipTable[k] = short(r * int(style[0][((r < 0) ? (-r) : (r)) - 1].Factor())
						   * int(a.Factor()));
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////
// Make OP Table

template<class CType>
void MultiVStyle<CType>::makeOPTable()
{
	if (!opTable) { return; }

	Blade<CType> a(base[0]);
	uint i, j, k;
	int r;

	for (i = 0, k = 0; i < gadim; i++)
	{
		for (j = 0; j < gadim; j++, k++)
		{
			a = style[0][i] ^ style[0][j];
			if (a.Factor() == CType(0)) { opTable[k] = 0; }
			else
			{
				r          = (style[0] == a);
				opTable[k] = short(r * int(style[0][((r < 0) ? (-r) : (r)) - 1].Factor())
						   * int(a.Factor()));
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////
// Make Vector Inverse Table

template<class CType>
void MultiVStyle<CType>::makeVecInvTable()
{
	if (!viTable) { return; }

	Blade<CType> a(base[0]);

	for (uint i = 0; i < gadim; i++)
	{
		a          = (style[0][i] * (~style[0][i]));
		viTable[i] = (short) a.Factor();
	}
}

////////////////////////////////////////////////////////////////////////////////////
// Make Grade List

template<class CType>
void MultiVStyle<CType>::getGrades()
{
	if (!grades) { return; }

	for (uint i = 0; i < gadim; i++)
	{
		grades[i] = (short) style[0][i].Grade();
	}
}

////////////////////////////////////////////////////////////////////////////////////
// Name of Blade at pos

template<class CType>
char* MultiVStyle<CType>::BladeName(size_t pos)
{
	if (pos < gadim)
	{
		return style[0][pos].Str();
	}
	else
	{
		return str = "(ERROR)";
	}
}

////////////////////////////////////////////////////////////////////////////////////
// Finding Blade in MVStyle

template<class CType>
int operator==(const MultiVStyle<CType>& amvs, const Blade<CType>& b)
{
	return amvs.style[0] == b;
}
