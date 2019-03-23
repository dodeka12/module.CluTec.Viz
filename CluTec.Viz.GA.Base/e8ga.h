////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.GA.Base
// file:      e8ga.h
//
// summary:   Declares the 8ga class
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

// Conformal Conic GA. Encodes upper triangular part of 3x3 matrix, i.e. 6 dimensions.
// Similar to El2GA, but uses Minkowski basis.

#ifndef __E8GA_HH
	#define __E8GA_HH

#include "cga.h"
#include "CluTec.Viz.Base\xutlib.h"

	#define ID_E8GA 0x0008

// CC2GA Class Declaration -----------------------
	template<class CType>
	class E8GA : public Base<CType>
	{
	public:

		E8GA();

		CType signature(uint i);

		static const MultiVStyle<CType>& GetMVStyle() { return m_MVStyle; }
		static const Mem<uint>& GetBladeIdx(uint uIdx) { return m_mBladeIdx[uIdx]; }
		static void ResetInit() { m_bInit = true; }

		// Initialise variables. Called by constructor if m_bInit == true;
		void InitVars();

		// Additional Functions -----------------------------

		static MultiV<CType> Vec(CType dE1, CType dE2, CType dE3, CType dE4, CType dE5, CType dE6, CType dE7, CType dE8);

		static MultiV<CType> Vec(CType* pdX) { return Vec(pdX[0], pdX[1], pdX[2], pdX[3], pdX[4], pdX[5], pdX[6], pdX[7]); }

		static MultiV<CType> Rotor(const MultiV<CType>& vA, const CType& rad);

		// Analyse Multivector. Result is returned in rMVI.
/*
        static bool AnalyzeMV(const MultiV<CType>& vA, CMVInfo<CType>& rMVI,
                                                  bool bOPNS = true, CType fPrec = CType(0));
        static bool AnalyzeBlade(const MultiV<CType>& vA, CMVInfo<CType>& rMVI,
                                                         bool bOPNS = true, CType fPrec = CType(0));
        static bool AnalyzeVersor(const MultiV<CType>& vA, CMVInfo<CType>& rMVI,
                                                          CType fPrec = CType(0));

*/

	protected:

		// Generates all blades of a particular grade, using recursion.
		bool GenerateBlades(BladeList<CType>& blList, MemObj<Mem<uint> >& mBladeIdx, uint uGrade, uint iFirstIdx, uint* puIdxList, uint uIdxPos = 0);

	public:

		static MultiV<CType> vZero;
		static MultiV<CType> vSC;
		static MultiV<CType> vE1;
		static MultiV<CType> vE2;
		static MultiV<CType> vE3;
		static MultiV<CType> vE4;
		static MultiV<CType> vE5;
		static MultiV<CType> vE6;
		static MultiV<CType> vE7;
		static MultiV<CType> vE8;
		static MultiV<CType> vI;

		static const uint iSC;
		static const uint iE1;
		static const uint iE2;
		static const uint iE3;
		static const uint iE4;
		static const uint iE5;
		static const uint iE6;
		static const uint iE7;
		static const uint iE8;
		static const uint iI;

	protected:

		static MultiVStyle<CType> m_MVStyle;

	private:

		static bool m_bInit;
		static const CType m_dSqrt2;

		static short piGrades[256];
		static short piVITable[256];
		static short piDualTable[256];
		static short piGPTable[65536];
		static short piIPTable[65536];
		static short piOPTable[65536];

		// Stores the indices of the different blades
		static MemObj<Mem<uint> > m_mBladeIdx;
	};
#endif
