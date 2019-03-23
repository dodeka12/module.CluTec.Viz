////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.GA.Base
// file:      confga.h
//
// summary:   Declares the confga class
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

// 3D Euclidean Geometric Algebra (ConfGA) Class Declaration and Definitions

#ifndef __CONFGA_HH
	#define __CONFGA_HH

#include "cga.h"
#include "CluTec.Viz.Base\xutlib.h"

	#define ID_CONFGA 0x0005

	#define DeclareConfGAVarsLocal(TYPE) \
		const Blade<TYPE> &bSC = ConfGA<TYPE>::bSC; \
		const Blade<TYPE>& bE1    = ConfGA<TYPE>::bE1; \
		const Blade<TYPE>& bE2    = ConfGA<TYPE>::bE2; \
		const Blade<TYPE>& bE3    = ConfGA<TYPE>::bE3; \
		const Blade<TYPE>& bEp    = ConfGA<TYPE>::bEp; \
		const Blade<TYPE>& bEm    = ConfGA<TYPE>::bEm; \
		const Blade<TYPE>& bB1    = ConfGA<TYPE>::bB1; \
		const Blade<TYPE>& bB2    = ConfGA<TYPE>::bB2; \
		const Blade<TYPE>& bB3    = ConfGA<TYPE>::bB3; \
		const Blade<TYPE>& bBp1   = ConfGA<TYPE>::bBp1; \
		const Blade<TYPE>& bBp2   = ConfGA<TYPE>::bBp2; \
		const Blade<TYPE>& bBp3   = ConfGA<TYPE>::bBp3; \
		const Blade<TYPE>& bBm1   = ConfGA<TYPE>::bBm1; \
		const Blade<TYPE>& bBm2   = ConfGA<TYPE>::bBm2; \
		const Blade<TYPE>& bBm3   = ConfGA<TYPE>::bBm3; \
		const Blade<TYPE>& bBpm   = ConfGA<TYPE>::bBpm; \
		const Blade<TYPE>& bTp1   = ConfGA<TYPE>::bTp1; \
		const Blade<TYPE>& bTp2   = ConfGA<TYPE>::bTp2; \
		const Blade<TYPE>& bTp3   = ConfGA<TYPE>::bTp3; \
		const Blade<TYPE>& bTm1   = ConfGA<TYPE>::bTm1; \
		const Blade<TYPE>& bTm2   = ConfGA<TYPE>::bTm2; \
		const Blade<TYPE>& bTm3   = ConfGA<TYPE>::bTm3; \
		const Blade<TYPE>& bTpm1  = ConfGA<TYPE>::bTpm1; \
		const Blade<TYPE>& bTpm2  = ConfGA<TYPE>::bTpm2; \
		const Blade<TYPE>& bTpm3  = ConfGA<TYPE>::bTpm3; \
		const Blade<TYPE>& bT123  = ConfGA<TYPE>::bT123; \
		const Blade<TYPE>& bQp    = ConfGA<TYPE>::bQp; \
		const Blade<TYPE>& bQm    = ConfGA<TYPE>::bQm; \
		const Blade<TYPE>& bQpm1  = ConfGA<TYPE>::bQpm1; \
		const Blade<TYPE>& bQpm2  = ConfGA<TYPE>::bQpm2; \
		const Blade<TYPE>& bQpm3  = ConfGA<TYPE>::bQpm3; \
		const Blade<TYPE>& bI     = ConfGA<TYPE>::bI; \
		const MultiV<TYPE>& vSC   = ConfGA<TYPE>::vSC; \
		const MultiV<TYPE>& vE1   = ConfGA<TYPE>::vE1; \
		const MultiV<TYPE>& vE2   = ConfGA<TYPE>::vE2; \
		const MultiV<TYPE>& vE3   = ConfGA<TYPE>::vE3; \
		const MultiV<TYPE>& vEp   = ConfGA<TYPE>::vEp; \
		const MultiV<TYPE>& vEm   = ConfGA<TYPE>::vEm; \
		const MultiV<TYPE>& vE    = ConfGA<TYPE>::vE; \
		const MultiV<TYPE>& vE0   = ConfGA<TYPE>::vE0; \
		const MultiV<TYPE>& vN    = ConfGA<TYPE>::vN; \
		const MultiV<TYPE>& vNb   = ConfGA<TYPE>::vNb; \
		const MultiV<TYPE>& vB1   = ConfGA<TYPE>::vB1; \
		const MultiV<TYPE>& vB2   = ConfGA<TYPE>::vB2; \
		const MultiV<TYPE>& vB3   = ConfGA<TYPE>::vB3; \
		const MultiV<TYPE>& vBp1  = ConfGA<TYPE>::vBp1; \
		const MultiV<TYPE>& vBp2  = ConfGA<TYPE>::vBp2; \
		const MultiV<TYPE>& vBp3  = ConfGA<TYPE>::vBp3; \
		const MultiV<TYPE>& vBm1  = ConfGA<TYPE>::vBm1; \
		const MultiV<TYPE>& vBm2  = ConfGA<TYPE>::vBm2; \
		const MultiV<TYPE>& vBm3  = ConfGA<TYPE>::vBm3; \
		const MultiV<TYPE>& vBpm  = ConfGA<TYPE>::vBpm; \
		const MultiV<TYPE>& vTp1  = ConfGA<TYPE>::vTp1; \
		const MultiV<TYPE>& vTp2  = ConfGA<TYPE>::vTp2; \
		const MultiV<TYPE>& vTp3  = ConfGA<TYPE>::vTp3; \
		const MultiV<TYPE>& vTm1  = ConfGA<TYPE>::vTm1; \
		const MultiV<TYPE>& vTm2  = ConfGA<TYPE>::vTm2; \
		const MultiV<TYPE>& vTm3  = ConfGA<TYPE>::vTm3; \
		const MultiV<TYPE>& vTpm1 = ConfGA<TYPE>::vTpm1; \
		const MultiV<TYPE>& vTpm2 = ConfGA<TYPE>::vTpm2; \
		const MultiV<TYPE>& vTpm3 = ConfGA<TYPE>::vTpm3; \
		const MultiV<TYPE>& vT123 = ConfGA<TYPE>::vT123; \
		const MultiV<TYPE>& vQp   = ConfGA<TYPE>::vQp; \
		const MultiV<TYPE>& vQm   = ConfGA<TYPE>::vQm; \
		const MultiV<TYPE>& vQpm1 = ConfGA<TYPE>::vQpm1; \
		const MultiV<TYPE>& vQpm2 = ConfGA<TYPE>::vQpm2; \
		const MultiV<TYPE>& vQpm3 = ConfGA<TYPE>::vQpm3; \
		const MultiV<TYPE>& vI    = ConfGA<TYPE>::vI; \
		const MultiV<TYPE>& vZero = ConfGA<TYPE>::vZero; \
		const uint iSC            = ConfGA<TYPE>::iSC; \
		const uint iE1            = ConfGA<TYPE>::iE1; \
		const uint iE2            = ConfGA<TYPE>::iE2; \
		const uint iE3            = ConfGA<TYPE>::iE3; \
		const uint iEp            = ConfGA<TYPE>::iEp; \
		const uint iEm            = ConfGA<TYPE>::iEm; \
		const uint iB1            = ConfGA<TYPE>::iB1; \
		const uint iB2            = ConfGA<TYPE>::iB2; \
		const uint iB3            = ConfGA<TYPE>::iB3; \
		const uint iBp1           = ConfGA<TYPE>::iBp1; \
		const uint iBp2           = ConfGA<TYPE>::iBp2; \
		const uint iBp3           = ConfGA<TYPE>::iBp3; \
		const uint iBm1           = ConfGA<TYPE>::iBm1; \
		const uint iBm2           = ConfGA<TYPE>::iBm2; \
		const uint iBm3           = ConfGA<TYPE>::iBm3; \
		const uint iBpm           = ConfGA<TYPE>::iBpm; \
		const uint iTp1           = ConfGA<TYPE>::iTp1; \
		const uint iTp2           = ConfGA<TYPE>::iTp2; \
		const uint iTp3           = ConfGA<TYPE>::iTp3; \
		const uint iTm1           = ConfGA<TYPE>::iTm1; \
		const uint iTm2           = ConfGA<TYPE>::iTm2; \
		const uint iTm3           = ConfGA<TYPE>::iTm3; \
		const uint iTpm1          = ConfGA<TYPE>::iTpm1; \
		const uint iTpm2          = ConfGA<TYPE>::iTpm2; \
		const uint iTpm3          = ConfGA<TYPE>::iTpm3; \
		const uint iT123          = ConfGA<TYPE>::iT123; \
		const uint iQp            = ConfGA<TYPE>::iQp; \
		const uint iQm            = ConfGA<TYPE>::iQm; \
		const uint iQpm1          = ConfGA<TYPE>::iQpm1; \
		const uint iQpm2          = ConfGA<TYPE>::iQpm2; \
		const uint iQpm3          = ConfGA<TYPE>::iQpm3; \
		const uint iI             = ConfGA<TYPE>::iI; \

// ConfGA Class Declaration -----------------------
	template<class CType>
	class ConfGA : public Base<CType>
	{
	public:

		ConfGA();

		CType signature(uint i);

		static const MultiVStyle<CType>& GetMVStyle() { return m_MVStyle; }
		static void ResetInit() { m_bInit = true; }

		// Initialise variables. Called by constructor if m_bInit == true;
		void InitVars();

		// Additional Functions -----------------------------

		// Analyse Multivector. Result is returned in rMVI.
		static bool AnalyzeMV(const MultiV<CType>& vA, CMVInfo<CType>& rMVI, bool bOPNS = true, CType fPrec = CType(0));
		static bool AnalyzeBlade(const MultiV<CType>& vA, CMVInfo<CType>& rMVI, bool bOPNS = true, CType fPrec = CType(0));
		static bool AnalyzeVersor(const MultiV<CType>& vA, CMVInfo<CType>& rMVI, CType fPrec = CType(0));

		// Make Null Vector
		static MultiV<CType> NullVec(const CType& a1, const CType& a2, const CType& a3);
		static MultiV<CType> NullVec(const CType* pA);

		// Make Null Vector from spherical coordinates
		static MultiV<CType> NullSVec(const CType& r, const CType& theta, const CType& phi);

		// Make Direction Vector
		static MultiV<CType> DirVec(const CType& a1, const CType& a2, const CType& a3);

		// Make Direction Vector from spherical coordinates
		static MultiV<CType> DirSVec(const CType& r, const CType& theta, const CType& phi);

		// Renormalize Null Vector
		static MultiV<CType> RenormNullVec(const MultiV<CType>& vA, CType fPrec = CType(0));

		// Make Euclidean Vector from Null Vector
		static MultiV<CType> MakeEuclidVec(const MultiV<CType>& vA, CType fPrec = CType(0));

		// Make Sphere
		static MultiV<CType> Sphere(const CType& a1, const CType& a2, const CType& a3, const CType& rad);
		static MultiV<CType> Sphere(const CType* pA, const CType& rad);

		// Make translation operator
		static MultiV<CType> Translator(const CType& a1, const CType& a2, const CType& a3);
		static MultiV<CType> Translator(const MultiV<CType>& vA);

		// Make rotation operator
		static MultiV<CType> Rotor(const CType& a1, const CType& a2, const CType& a3, const CType& rad, CType fPrec = CType(0));
		static MultiV<CType> Rotor(const MultiV<CType>& vA, const CType& rad, bool bAxis = true, CType fPrec = CType(0));

		// Make dilation operator
		static MultiV<CType> Dilator(const CType& rho);

		// Print 1-Vector in terms of e1,e2,e3,e,e0
		static CStrMem MakeStrVector(const MultiV<CType>& vA);

		// Make Bivector
//	static MultiV<CType> Bivec(const CType &a1, const CType &a2, const CType &a3);

	public:

		static Blade<CType> bSC;
		static Blade<CType> bE1;
		static Blade<CType> bE2;
		static Blade<CType> bE3;
		static Blade<CType> bEp;
		static Blade<CType> bEm;
		static Blade<CType> bB1;
		static Blade<CType> bB2;
		static Blade<CType> bB3;
		static Blade<CType> bBp1;
		static Blade<CType> bBp2;
		static Blade<CType> bBp3;
		static Blade<CType> bBm1;
		static Blade<CType> bBm2;
		static Blade<CType> bBm3;
		static Blade<CType> bBpm;
		static Blade<CType> bTp1;
		static Blade<CType> bTp2;
		static Blade<CType> bTp3;
		static Blade<CType> bTm1;
		static Blade<CType> bTm2;
		static Blade<CType> bTm3;
		static Blade<CType> bTpm1;
		static Blade<CType> bTpm2;
		static Blade<CType> bTpm3;
		static Blade<CType> bT123;
		static Blade<CType> bQp;
		static Blade<CType> bQm;
		static Blade<CType> bQpm1;
		static Blade<CType> bQpm2;
		static Blade<CType> bQpm3;
		static Blade<CType> bI;

		static MultiV<CType> vSC;
		static MultiV<CType> vE1;
		static MultiV<CType> vE2;
		static MultiV<CType> vE3;
		static MultiV<CType> vEp;
		static MultiV<CType> vEm;
		static MultiV<CType> vE;
		static MultiV<CType> vE0;
		static MultiV<CType> vN;
		static MultiV<CType> vNb;
		static MultiV<CType> vB1;
		static MultiV<CType> vB2;
		static MultiV<CType> vB3;
		static MultiV<CType> vBp1;
		static MultiV<CType> vBp2;
		static MultiV<CType> vBp3;
		static MultiV<CType> vBm1;
		static MultiV<CType> vBm2;
		static MultiV<CType> vBm3;
		static MultiV<CType> vBpm;
		static MultiV<CType> vTp1;
		static MultiV<CType> vTp2;
		static MultiV<CType> vTp3;
		static MultiV<CType> vTm1;
		static MultiV<CType> vTm2;
		static MultiV<CType> vTm3;
		static MultiV<CType> vTpm1;
		static MultiV<CType> vTpm2;
		static MultiV<CType> vTpm3;
		static MultiV<CType> vT123;
		static MultiV<CType> vQp;
		static MultiV<CType> vQm;
		static MultiV<CType> vQpm1;
		static MultiV<CType> vQpm2;
		static MultiV<CType> vQpm3;
		static MultiV<CType> vI;
		static MultiV<CType> vZero;
		static RingBuf<uint> BLADE;

		static const uint iSC;
		static const uint iE1;
		static const uint iE2;
		static const uint iE3;
		static const uint iEp;
		static const uint iEm;
		static const uint iB1;
		static const uint iB2;
		static const uint iB3;
		static const uint iBp1;
		static const uint iBp2;
		static const uint iBp3;
		static const uint iBm1;
		static const uint iBm2;
		static const uint iBm3;
		static const uint iBpm;
		static const uint iTp1;
		static const uint iTp2;
		static const uint iTp3;
		static const uint iTm1;
		static const uint iTm2;
		static const uint iTm3;
		static const uint iTpm1;
		static const uint iTpm2;
		static const uint iTpm3;
		static const uint iT123;
		static const uint iQp;
		static const uint iQm;
		static const uint iQpm1;
		static const uint iQpm2;
		static const uint iQpm3;
		static const uint iI;

	protected:

		static MultiVStyle<CType> m_MVStyle;

	private:

		static bool m_bInit;

		static short piGrades[32];
		static short piVITable[32];
		static short piDualTable[32];
		static short piGPTable[1024];
		static short piIPTable[1024];
		static short piOPTable[1024];
	};
#endif
