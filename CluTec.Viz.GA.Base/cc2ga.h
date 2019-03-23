////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.GA.Base
// file:      cc2ga.h
//
// summary:   Declares the Cc 2ga class
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

#ifndef __CC2GA_HH
	#define __CC2GA_HH

#include "cga.h"
#include "CluTec.Viz.Base\xutlib.h"

	#define ID_CC2GA 0x0007

// CC2GA Class Declaration -----------------------
	template<class CType>
	class CC2GA : public Base<CType>
	{
	public:

		CC2GA();

		CType signature(uint i);

		static const MultiVStyle<CType>& GetMVStyle() { return m_MVStyle; }
		static const Mem<uint>& GetBladeIdx(size_t uIdx) { return m_mBladeIdx[uIdx]; }
		static void ResetInit() { m_bInit = true; }

		// Initialise variables. Called by constructor if m_bInit == true;
		void InitVars();

		// Additional Functions -----------------------------

		// Embed a 2d Euclidean vector in this algebra.
		// The vector will be homogenized by adding a 1 in a third dimension.
		// Then the tensor product of the vector with itself is evaluated and
		// the upper triangular part of the resultant symmetric matrix is encoded
		// as a vector in this 6d space.
		static MultiV<CType> Vec(CType dX, CType dY);
		static MultiV<CType> Vec(CType* pdX) { return Vec(pdX[0], pdX[1]); }

		// Embed 3x3 matrix from vector
		static MultiV<CType> Vec(Matrix<CType>& xM);
		// Create 3x3 matrix from vector
		static Matrix<CType> Mat(const MultiV<CType>& vA);

		// Analyse Multivector. Result is returned in rMVI.
/*
        static bool AnalyzeMV(const MultiV<CType>& vA, CMVInfo<CType>& rMVI,
                                                  bool bOPNS = true, CType fPrec = CType(0));
        static bool AnalyzeBlade(const MultiV<CType>& vA, CMVInfo<CType>& rMVI,
                                                         bool bOPNS = true, CType fPrec = CType(0));
        static bool AnalyzeVersor(const MultiV<CType>& vA, CMVInfo<CType>& rMVI,
                                                          CType fPrec = CType(0));

        static bool MakeConicDegenerate(const MultiV<CType>& vA, MultiV<CType>& vB, CType& fDist,
                                                                        bool bOPNS = true, CType fPrec = CType(0));

        static bool FindAllDegConics(const MultiV<CType>& vA, const MultiV<CType>& vB,
                                                                MemObj<MultiV<CType> >& mConicList, CType fPrec = CType(0));

        static bool IntersectConics(const MultiV<CType>& vA, const MultiV<CType>& vB,
                                                                MemObj<CPointND<CType,2> >& mPointList, CType fPrec = CType(0));
*/

	protected:

		// Generates all blades of a particular grade, using recursion.
		bool GenerateBlades(BladeList<CType>& blList, MemObj<Mem<uint> >& mBladeIdx, uint iGrade, size_t iFirstIdx, uint* puIdxList, size_t uIdxPos = 0);

	public:

		static MultiV<CType> vZero;
		static MultiV<CType> vSC;
		static MultiV<CType> vE1, vE2, vE3, vE4, vE5, vE6, vE7, vE8;
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

		static short piGrades[256], piVITable[256], piDualTable[256];
		static short piGPTable[65536], piIPTable[65536], piOPTable[65536];

		// Stores the indices of the different blades
		static MemObj<Mem<uint> > m_mBladeIdx;
	};
#endif
