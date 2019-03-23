////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.GA.Base
// file:      e3ga.h
//
// summary:   Declares the 3ga class
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

// 3D Euclidean Geometric Algebra (E3GA) Class Declaration and Definitions

#ifndef __E3GA_HH
	#define __E3GA_HH

#include "cga.h"
#include "CluTec.Viz.Base\xutlib.h"
#include "MVInfo.h"

	#define ID_E3GA 0x0003

	#define DeclareE3GAVarsLocal(TYPE) \
		const Blade<TYPE> &bSC = E3GA<TYPE>::bSC; \
		const Blade<TYPE>& bE1    = E3GA<TYPE>::bE1; \
		const Blade<TYPE>& bE2    = E3GA<TYPE>::bE2; \
		const Blade<TYPE>& bE3    = E3GA<TYPE>::bE3; \
		const Blade<TYPE>& bL1    = E3GA<TYPE>::bL1; \
		const Blade<TYPE>& bL2    = E3GA<TYPE>::bL2; \
		const Blade<TYPE>& bL3    = E3GA<TYPE>::bL3; \
		const Blade<TYPE>& bI     = E3GA<TYPE>::bI; \
		const MultiV<TYPE>& vSC   = E3GA<TYPE>::vSC; \
		const MultiV<TYPE>& vE1   = E3GA<TYPE>::vE1; \
		const MultiV<TYPE>& vE2   = E3GA<TYPE>::vE2; \
		const MultiV<TYPE>& vE3   = E3GA<TYPE>::vE3; \
		const MultiV<TYPE>& vL1   = E3GA<TYPE>::vL1; \
		const MultiV<TYPE>& vL2   = E3GA<TYPE>::vL2; \
		const MultiV<TYPE>& vL3   = E3GA<TYPE>::vL3; \
		const MultiV<TYPE>& vI    = E3GA<TYPE>::vI; \
		const MultiV<TYPE>& vZero = E3GA<TYPE>::vZero; \
		const uint iSC            = E3GA<TYPE>::iSC; \
		const uint iE1            = E3GA<TYPE>::iE1; \
		const uint iE2            = E3GA<TYPE>::iE2; \
		const uint iE3            = E3GA<TYPE>::iE3; \
		const uint iL1            = E3GA<TYPE>::iL1; \
		const uint iL2            = E3GA<TYPE>::iL2; \
		const uint iL3            = E3GA<TYPE>::iL3; \
		const uint iI             = E3GA<TYPE>::iI;

// E3GA Class Declaration -----------------------
	template<class CType>
	class E3GA : public Base<CType>
	{
	public:

		E3GA();

		CType signature(uint i);

		static const MultiVStyle<CType>& GetMVStyle() { return m_MVStyle; }
		static void ResetInit() { m_bInit = true; }

		// Initialise variables. Called by constructor if m_bInit == true;
		void InitVars();

		// Additional Functions -----------------------------

		// Analyse Multivector. Result is returned in rMVI.
		static bool AnalyzeMV(const MultiV<CType>& vA, CMVInfo<CType>& rMVI, bool bOPNS, CType fPrec);
		static bool AnalyzeBlade(const MultiV<CType>& vA, CMVInfo<CType>& rMVI, bool bOPNS, CType fPrec);
		static bool AnalyzeRotor(const MultiV<CType>& vA, CMVInfo<CType>& rMVI, CType fPrec);

		// Make 3-D Vector
		static MultiV<CType> Vec(const CType& a1, const CType& a2, const CType& a3);
		static MultiV<CType> Vec(const CType* pA);

		// Make 3-D Vector from spherical coords
		static MultiV<CType> SVec(const CType& r, const CType& theta, const CType& phi);
		static MultiV<CType> SVec(const CType* pA);

		// Make Bivector
		static MultiV<CType> Bivec(const CType& a1, const CType& a2, const CType& a3);
		static MultiV<CType> Bivec(const CType* pA);

		// Make rotation operator
		static MultiV<CType> Rotor(const CType& a1, const CType& a2, const CType& a3, const CType& rad);
		static MultiV<CType> Rotor(const MultiV<CType>& vA, const CType& rad, bool bAxis = true);

	public:

		static Blade<CType> bE1;
		static Blade<CType> bSC;
		static Blade<CType> bE2;
		static Blade<CType> bE3;
		static Blade<CType> bL1;
		static Blade<CType> bL2;
		static Blade<CType> bL3;
		static Blade<CType> bI;

		static MultiV<CType> vSC;
		static MultiV<CType> vE1;
		static MultiV<CType> vE2;
		static MultiV<CType> vE3;
		static MultiV<CType> vL1;
		static MultiV<CType> vL2;
		static MultiV<CType> vL3;
		static MultiV<CType> vI;

		static MultiV<CType> vZero;
		static RingBuf<uint> BLADE;

		static const uint iSC;
		static const uint iE1;
		static const uint iE2;
		static const uint iE3;
		static const uint iL1;
		static const uint iL2;
		static const uint iL3;
		static const uint iI;

	protected:

		static MultiVStyle<CType> m_MVStyle;

	private:

		static bool m_bInit;

		static short piGrades[8];
		static short piVITable[8];
		static short piDualTable[8];
		static short piGPTable[64];
		static short piIPTable[64];
		static short piOPTable[64];
	};
#endif
