////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.GA.Base
// file:      mvs.h
//
// summary:   Declares the mvs class
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

//  Clifford Geometric Algebra Class Definitions

#ifndef _MULTIVSTYLE_H_
	#define _MULTIVSTYLE_H_

#include "base.h"
#include "blade.h"
#include "bladelist.h"

#include "CluTec.Viz.Base\mem.h"

//#define BLADE (uint[])    // Used for casting array initialization

	#define MVS_LIST 0x0001	// Multi-Vector Display Style as [1,..., ]
	#define MVS_SUM  0x0002	// Multi-Vector Display Style as [ 1*g0 + ... + ]

#ifndef _MAXSTRSIZE_
		#define _MAXSTRSIZE_ 1024	// Maximum String Size for String output
#endif

#ifdef _GNUCPP3_
    #ifndef _TMPL_
			#define _TMPL_ <>
    #endif
    #ifndef _FRIEND_
			#define _FRIEND_  friend
    #endif
#else
    #ifndef _TMPL_
			#define _TMPL_
    #endif
    #ifndef _FRIEND_
			#define _FRIEND_  friend
    #endif
#endif

	typedef unsigned uint;
	typedef unsigned long ulong;

	template<class CType> class MultiV;
	template<class CType> class MultiVStyle;

	template<class CType> int operator==(const MultiVStyle<CType>&, const Blade<CType>&);

// Multi-Vector-Style Class Declaration ---------------------

	template<class CType>
	class MultiVStyle
	{
	public:

		MultiVStyle(BladeList<CType>& nstyle);
		MultiVStyle();
		virtual ~MultiVStyle();

		MultiVStyle<CType>& Init(BladeList<CType>& BList,
				short* _grades = 0,
				short* _gpTable = 0,
				short* _ipTable = 0,
				short* _opTable = 0,
				short* _viTable = 0,
				short* _dualTable = 0,
				short* _invoTable =
					0);

		_FRIEND_ int operator== _TMPL_(const MultiVStyle<CType> &, const Blade<CType> &);

		uint GADim() const { return gadim; }
		short* GPTable() const { return gpTable; }
		short* IPTable() const { return ipTable; }
		short* OPTable() const { return opTable; }
		short* ReverseTable() const { return revTable; }
		short* DualTable() const { return dualTable; }
		short* InvoTable() const { return invoTable; }

		short* VecInvTable() const { return viTable; }
		short* Grades() const { return grades; }
		char* BladeName(size_t pos);
		Base<CType>* BasePtr() const { return base; }
		BladeList<CType>* Style() const { return style; }

	protected:

		Base<CType>* base;
		uint dim;
		uint gadim;
		uint tblsize;
		BladeList<CType>* style;
		CStrMem str;

		short* grades;
		short* viTable;	// Vector Inverse Table
		short* gpTable, * ipTable, * opTable, * dualTable, * invoTable, * revTable;

		void makeGPTable();
		void makeIPTable();
		void makeOPTable();
		void makeReverseTable();
		void makeDualTable();
		void makeInvoTable();
		void makeVecInvTable();
		void getGrades();
	};

#endif	// _MULTIVSTYLE_H_
