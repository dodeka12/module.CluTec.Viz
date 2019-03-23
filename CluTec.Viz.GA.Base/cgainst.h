////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.GA.Base
// file:      cgainst.h
//
// summary:   Declares the cgainst class
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

#ifndef __CGAINST_HH
#define __CGAINST_HH

// Use this macro to create a CLUGA_EXT template library of a certain type
// given by TYPE

// Visual C++ doesn't need instantiation of abstract base class!
// GNU C++ seems to want this.

//#ifndef WIN32
	#define Instantiate_Base(TYPE) CLUGA_EXT template  class CLUGA_API Base<TYPE>
//#else
//	#define Instantiate_Base(TYPE) /**/
//#endif


#define Instantiate_CGA(TYPE) \
\
CLUGA_EXT template CLUGA_API int operator==(const Blade<TYPE> &,const Blade<TYPE> &);\
CLUGA_EXT template CLUGA_API Blade<TYPE> operator+(const Blade<TYPE> &,const Blade<TYPE> &);\
CLUGA_EXT template CLUGA_API Blade<TYPE>& operator+=(Blade<TYPE> &,const Blade<TYPE> &);\
CLUGA_EXT template CLUGA_API Blade<TYPE> operator-(const Blade<TYPE> &, const Blade<TYPE> &);\
CLUGA_EXT template CLUGA_API Blade<TYPE>& operator-=(Blade<TYPE> &,const Blade<TYPE> &);\
CLUGA_EXT template CLUGA_API Blade<TYPE> operator^(const Blade<TYPE> &, const TYPE &);\
CLUGA_EXT template CLUGA_API Blade<TYPE> operator^(const TYPE &, const Blade<TYPE> &);\
CLUGA_EXT template CLUGA_API Blade<TYPE>& operator^=(Blade<TYPE> &,const TYPE &a);\
CLUGA_EXT template CLUGA_API Blade<TYPE> operator-(const Blade<TYPE> &);\
CLUGA_EXT template CLUGA_API Blade<TYPE> operator!(const Blade<TYPE> &);\
CLUGA_EXT template CLUGA_API Blade<TYPE> operator~(const Blade<TYPE> &);\
CLUGA_EXT template CLUGA_API Blade<TYPE> operator&(const Blade<TYPE> &,const Blade<TYPE> &);\
CLUGA_EXT template CLUGA_API Blade<TYPE> operator*(const Blade<TYPE> &,const Blade<TYPE> &);\
CLUGA_EXT template CLUGA_API Blade<TYPE> operator^(const Blade<TYPE> &,const Blade<TYPE> &);\
CLUGA_EXT template CLUGA_API Blade<TYPE> cp(const Blade<TYPE> &,const Blade<TYPE> &);\
CLUGA_EXT template CLUGA_API Blade<TYPE> acp(const Blade<TYPE> &,const Blade<TYPE> &);\
CLUGA_EXT template CLUGA_API Blade<TYPE> join(const Blade<TYPE> &, const Blade<TYPE> &);\
CLUGA_EXT template CLUGA_API Blade<TYPE> meet(const Blade<TYPE> &, const Blade<TYPE> &);\
\
\
CLUGA_EXT template CLUGA_API int operator==(const BladeList<TYPE> &,const Blade<TYPE> &);\
\
CLUGA_EXT template CLUGA_API TYPE Scalar(const BladeList<TYPE> &bl);\
\
CLUGA_EXT template CLUGA_API BladeList<TYPE> operator+(const BladeList<TYPE> &, const TYPE &);\
CLUGA_EXT template CLUGA_API BladeList<TYPE> operator+(const TYPE &, const BladeList<TYPE> &);\
\
CLUGA_EXT template CLUGA_API BladeList<TYPE> operator-(const BladeList<TYPE> &, const TYPE &);\
CLUGA_EXT template CLUGA_API BladeList<TYPE> operator-(const TYPE &, const BladeList<TYPE> &);\
\
CLUGA_EXT template CLUGA_API BladeList<TYPE>& operator+=(BladeList<TYPE> &,const TYPE &);\
CLUGA_EXT template CLUGA_API BladeList<TYPE>& operator-=(BladeList<TYPE> &,const TYPE &);\
CLUGA_EXT template CLUGA_API BladeList<TYPE>& operator<<(BladeList<TYPE> &,const TYPE &);\
\
CLUGA_EXT template CLUGA_API BladeList<TYPE> operator+(const BladeList<TYPE> &,\
				    const Blade<TYPE> &);\
CLUGA_EXT template CLUGA_API BladeList<TYPE> operator+(const Blade<TYPE> &,\
				    const BladeList<TYPE> &);\
\
CLUGA_EXT template CLUGA_API BladeList<TYPE> operator-(const BladeList<TYPE> &,\
				    const Blade<TYPE> &);\
CLUGA_EXT template CLUGA_API BladeList<TYPE> operator-(const Blade<TYPE> &,\
				    const BladeList<TYPE> &);\
\
CLUGA_EXT template CLUGA_API BladeList<TYPE>& operator+=(BladeList<TYPE> &,const Blade<TYPE> &);\
CLUGA_EXT template CLUGA_API BladeList<TYPE>& operator-=(BladeList<TYPE> &,const Blade<TYPE> &);\
\
CLUGA_EXT template CLUGA_API BladeList<TYPE>& operator<<(BladeList<TYPE> &,\
				      const Blade<TYPE> &); \
\
CLUGA_EXT template CLUGA_API BladeList<TYPE> operator+(const BladeList<TYPE> &,\
				    const BladeList<TYPE> &);\
CLUGA_EXT template CLUGA_API BladeList<TYPE> operator-(const BladeList<TYPE> &,\
				    const BladeList<TYPE> &);\
CLUGA_EXT template CLUGA_API BladeList<TYPE>& operator+=(BladeList<TYPE> &,\
				      const BladeList<TYPE> &);\
CLUGA_EXT template CLUGA_API BladeList<TYPE>& operator-=(BladeList<TYPE> &,\
				      const BladeList<TYPE> &);\
CLUGA_EXT template CLUGA_API BladeList<TYPE>& operator<<(BladeList<TYPE> &,\
				      const BladeList<TYPE> &);\
\
CLUGA_EXT template CLUGA_API BladeList<TYPE> operator&(const BladeList<TYPE> &,\
				    const Blade<TYPE> &);\
CLUGA_EXT template CLUGA_API BladeList<TYPE> operator&(const Blade<TYPE> &,\
				    const BladeList<TYPE> &);\
CLUGA_EXT template CLUGA_API BladeList<TYPE>& operator&=(BladeList<TYPE> &,\
				      const Blade<TYPE> &);\
\
CLUGA_EXT template CLUGA_API BladeList<TYPE> operator*(const BladeList<TYPE> &,\
				    const Blade<TYPE> &);\
CLUGA_EXT template CLUGA_API BladeList<TYPE> operator*(const Blade<TYPE> &,\
				    const BladeList<TYPE> &);\
CLUGA_EXT template CLUGA_API BladeList<TYPE>& operator*=(BladeList<TYPE> &,\
				      const Blade<TYPE> &);\
\
CLUGA_EXT template CLUGA_API BladeList<TYPE> operator^(const BladeList<TYPE> &,\
				    const TYPE &);\
CLUGA_EXT template CLUGA_API BladeList<TYPE> operator^(const TYPE &,\
				    const BladeList<TYPE> &);\
CLUGA_EXT template CLUGA_API BladeList<TYPE>& operator^=(BladeList<TYPE> &,\
				      const TYPE &);\
\
CLUGA_EXT template CLUGA_API BladeList<TYPE> operator^(const BladeList<TYPE> &,\
				    const Blade<TYPE> &);\
CLUGA_EXT template CLUGA_API BladeList<TYPE> operator^(const Blade<TYPE> &,\
				    const BladeList<TYPE> &);\
CLUGA_EXT template CLUGA_API BladeList<TYPE>& operator^=(BladeList<TYPE> &,\
				      const Blade<TYPE> &);\
\
CLUGA_EXT template CLUGA_API BladeList<TYPE> operator&(const BladeList<TYPE> &,\
				    const BladeList<TYPE> &);\
CLUGA_EXT template CLUGA_API BladeList<TYPE>& operator&=(BladeList<TYPE> &,\
				      const BladeList<TYPE> &);\
CLUGA_EXT template CLUGA_API BladeList<TYPE> operator*(const BladeList<TYPE> &,\
				    const BladeList<TYPE> &);\
CLUGA_EXT template CLUGA_API BladeList<TYPE>& operator*=(BladeList<TYPE> &,\
				      const BladeList<TYPE> &);\
CLUGA_EXT template CLUGA_API BladeList<TYPE> operator^(const BladeList<TYPE> &,\
				    const BladeList<TYPE> &);\
CLUGA_EXT template CLUGA_API BladeList<TYPE>& operator^=(BladeList<TYPE> &,\
				      const BladeList<TYPE> &);\
\
CLUGA_EXT template CLUGA_API BladeList<TYPE> operator~(const BladeList<TYPE> &);\
CLUGA_EXT template CLUGA_API BladeList<TYPE> operator-(const BladeList<TYPE> &);\
CLUGA_EXT template CLUGA_API BladeList<TYPE> operator!(const BladeList<TYPE> &);\
\
CLUGA_EXT template CLUGA_API BladeList<TYPE> cp(const BladeList<TYPE> &,\
			     const Blade<TYPE> &);\
CLUGA_EXT template CLUGA_API BladeList<TYPE> acp(const BladeList<TYPE> &,\
			      const Blade<TYPE> &);\
CLUGA_EXT template CLUGA_API BladeList<TYPE> cp(const BladeList<TYPE> &,\
			     const BladeList<TYPE> &);\
CLUGA_EXT template CLUGA_API BladeList<TYPE> acp(const BladeList<TYPE> &,\
			      const BladeList<TYPE> &);\
\
\
\
\
CLUGA_EXT template CLUGA_API int operator==(const MultiVStyle<TYPE> &, const Blade<TYPE> &);\
\
\
CLUGA_EXT template CLUGA_API int operator==(const MultiV<TYPE> &, const MultiV<TYPE> &);\
CLUGA_EXT template CLUGA_API int operator!=(const MultiV<TYPE> &, const MultiV<TYPE> &);\
\
CLUGA_EXT template CLUGA_API TYPE Scalar(const MultiV<TYPE> &);\
\
CLUGA_EXT template CLUGA_API MultiV<TYPE> Exp(const MultiV<TYPE>& vA, int iOrder);\
\
CLUGA_EXT template CLUGA_API MultiV<TYPE> Involute(const MultiV<TYPE>& vA);\
\
CLUGA_EXT template CLUGA_API MultiV<TYPE> operator- (const MultiV<TYPE> &);\
CLUGA_EXT template CLUGA_API MultiV<TYPE> operator~ (const MultiV<TYPE> &);\
CLUGA_EXT template CLUGA_API MultiV<TYPE> operator* (const MultiV<TYPE> &);\
CLUGA_EXT template CLUGA_API MultiV<TYPE> operator! (const MultiV<TYPE> &);\
\
CLUGA_EXT template CLUGA_API MultiV<TYPE> operator+ (const MultiV<TYPE>&,const TYPE&);\
CLUGA_EXT template CLUGA_API MultiV<TYPE> operator+ (const TYPE&,const MultiV<TYPE>&);\
CLUGA_EXT template CLUGA_API MultiV<TYPE>& operator+= (MultiV<TYPE>&,const TYPE&);\
CLUGA_EXT template CLUGA_API MultiV<TYPE>& operator<< (MultiV<TYPE>&,const TYPE&);\
\
CLUGA_EXT template CLUGA_API MultiV<TYPE> operator- (const MultiV<TYPE>&,const TYPE&);\
CLUGA_EXT template CLUGA_API MultiV<TYPE> operator- (const TYPE&,const MultiV<TYPE>&);\
CLUGA_EXT template CLUGA_API MultiV<TYPE>& operator-= (MultiV<TYPE>&,const TYPE&);\
\
CLUGA_EXT template CLUGA_API MultiV<TYPE> operator+ (const MultiV<TYPE>&,const Blade<TYPE>&);\
CLUGA_EXT template CLUGA_API MultiV<TYPE> operator+ (const Blade<TYPE>&,const MultiV<TYPE>&);\
CLUGA_EXT template CLUGA_API MultiV<TYPE>& operator+= (MultiV<TYPE>&,const Blade<TYPE>&);\
CLUGA_EXT template CLUGA_API MultiV<TYPE>& operator<< (MultiV<TYPE>&,const Blade<TYPE>&);\
\
CLUGA_EXT template CLUGA_API MultiV<TYPE> operator- (const MultiV<TYPE>&,const Blade<TYPE>&);\
CLUGA_EXT template CLUGA_API MultiV<TYPE> operator- (const Blade<TYPE>&,const MultiV<TYPE>&);\
CLUGA_EXT template CLUGA_API MultiV<TYPE>& operator-= (MultiV<TYPE>&,const Blade<TYPE>&);\
\
CLUGA_EXT template CLUGA_API MultiV<TYPE> operator+ (const MultiV<TYPE>&,const MultiV<TYPE>&);\
CLUGA_EXT template CLUGA_API MultiV<TYPE>& operator+= (MultiV<TYPE>&,const MultiV<TYPE>&);\
CLUGA_EXT template CLUGA_API MultiV<TYPE> operator- (const MultiV<TYPE>&,const MultiV<TYPE>&);\
CLUGA_EXT template CLUGA_API MultiV<TYPE>& operator-= (MultiV<TYPE>&,const MultiV<TYPE>&);\
CLUGA_EXT template CLUGA_API MultiV<TYPE>& operator<< (MultiV<TYPE>&,const MultiV<TYPE>&);\
\
CLUGA_EXT template CLUGA_API MultiV<TYPE> operator& (const MultiV<TYPE>&,const MultiV<TYPE>&);\
CLUGA_EXT template CLUGA_API MultiV<TYPE>& operator&= (MultiV<TYPE>&,const MultiV<TYPE>&);\
CLUGA_EXT template CLUGA_API MultiV<TYPE> operator* (const MultiV<TYPE>&,const MultiV<TYPE>&);\
CLUGA_EXT template CLUGA_API MultiV<TYPE>& operator*= (MultiV<TYPE>&,const MultiV<TYPE>&);\
CLUGA_EXT template CLUGA_API MultiV<TYPE> operator^ (const MultiV<TYPE>&,const MultiV<TYPE>&);\
CLUGA_EXT template CLUGA_API MultiV<TYPE>& operator^= (MultiV<TYPE>&,const MultiV<TYPE>&);\
\
CLUGA_EXT template CLUGA_API MultiV<TYPE> operator^ (const MultiV<TYPE>&,const TYPE &);\
CLUGA_EXT template CLUGA_API MultiV<TYPE> operator^ (const TYPE &,const MultiV<TYPE>&);\
CLUGA_EXT template CLUGA_API MultiV<TYPE>& operator^= (MultiV<TYPE>&,const TYPE &);\
\
CLUGA_EXT template CLUGA_API MultiV<TYPE> operator& (const MultiV<TYPE>&,const Blade<TYPE>&);\
CLUGA_EXT template CLUGA_API MultiV<TYPE> operator& (const Blade<TYPE>&,const MultiV<TYPE>&);\
CLUGA_EXT template CLUGA_API MultiV<TYPE>& operator&= (MultiV<TYPE>&,const Blade<TYPE>&);\
\
CLUGA_EXT template CLUGA_API MultiV<TYPE> operator& (const MultiV<TYPE>&,const TYPE &);\
CLUGA_EXT template CLUGA_API MultiV<TYPE> operator& (const TYPE&,const MultiV<TYPE>&);\
CLUGA_EXT template CLUGA_API MultiV<TYPE>& operator&= (MultiV<TYPE>&, const TYPE &);\
\
CLUGA_EXT template CLUGA_API MultiV<TYPE> operator* (const MultiV<TYPE>&,const Blade<TYPE>&);\
CLUGA_EXT template CLUGA_API MultiV<TYPE> operator* (const Blade<TYPE>&,const MultiV<TYPE>&);\
CLUGA_EXT template CLUGA_API MultiV<TYPE>& operator*= (MultiV<TYPE>&,const Blade<TYPE>&);\
\
CLUGA_EXT template CLUGA_API MultiV<TYPE> operator^ (const MultiV<TYPE>&,const Blade<TYPE>&);\
CLUGA_EXT template CLUGA_API MultiV<TYPE> operator^ (const Blade<TYPE>&,const MultiV<TYPE>&);\
CLUGA_EXT template CLUGA_API MultiV<TYPE>& operator^= (MultiV<TYPE>&,const Blade<TYPE>&);\
\
CLUGA_EXT template CLUGA_API MultiV<TYPE> operator/ (const MultiV<TYPE>&,const TYPE &);\
CLUGA_EXT template CLUGA_API MultiV<TYPE>& operator/= (MultiV<TYPE>&,const TYPE &);\
\
CLUGA_EXT template CLUGA_API BladeList<TYPE> MakeBladeList(const MultiV<TYPE>&);\
\
CLUGA_EXT template CLUGA_API MultiV<TYPE> MakeVector(MultiVStyle<TYPE> &style,uint grade, const TYPE *comps);\
CLUGA_EXT template CLUGA_API MultiV<TYPE> MakeVector(const MultiV<TYPE> &mv,uint grade, const TYPE *comps);\
CLUGA_EXT template CLUGA_API void GetVector(const MultiV<TYPE> &mv, uint grade, TYPE *comps);\
\
CLUGA_EXT template CLUGA_API MultiV<TYPE> MakePS(MultiVStyle<TYPE> &style);\
\
CLUGA_EXT template CLUGA_API MultiV<TYPE> IMeet(const MultiV<TYPE>&,const MultiV<TYPE>&);\
\
CLUGA_EXT template CLUGA_API bool MultiVSVD(const MultiV<TYPE> &mv, MemObj<MultiV<TYPE> > &mvList, Mem<TYPE> &evList, tMVPos bPos);\
\
CLUGA_EXT template CLUGA_API uint MultiVJacobi(const MultiV<TYPE> &mv, MemObj<MultiV<TYPE> > &mvEVecList, Mem<TYPE> &mEValList, tMVPos bPos);\
\
CLUGA_EXT template CLUGA_API bool CalcMultiVGPMat(const MultiV<TYPE> &mvA, Matrix<TYPE> &M, tMVPos bPos, bool bEven);\
\
CLUGA_EXT template CLUGA_API bool CalcMultiVGPMat(const MultiV<TYPE> &mvA, Matrix<TYPE> &M, tMVPos bPos, const Mem<int>& mGrades);\
\
CLUGA_EXT template CLUGA_API bool CalcMultiVGPMatMask(const MultiV<TYPE> &mvA, Matrix<TYPE> &M, tMVPos bPos, const Mem<int>& mMask, EMVOpType eOPType);\
\
CLUGA_EXT template CLUGA_API bool CalcMVListGPMatMask(const MemObj<MultiV<TYPE> > &mvAList, Matrix<TYPE> &M, \
								tMVPos bPos, const Mem<int>& mMask, EMVOpType eOPType, \
								bool bPruneNullRows);\
CLUGA_EXT template CLUGA_API bool EvalEPMeanProd (MultiV<TYPE> &vC, EMVOpType eOPType, \
											const MultiV<TYPE> &vA, const MultiV<TYPE> &vB, \
											const Matrix<TYPE> &Cab);\
\
CLUGA_EXT template CLUGA_API bool EvalEPCovMatProd (Matrix<TYPE> &Cuu, EMVOpType eOPType, \
						const MultiV<TYPE> &vX, const MultiV<TYPE> &vY, \
						const Matrix<TYPE> &Cxx, const Matrix<TYPE> &Cyy, \
						const Matrix<TYPE> &Cxy);\
\
CLUGA_EXT template CLUGA_API bool EvalEPCrossCovMatProd (Matrix<TYPE> &Cuz, EMVOpType eOPType, \
		const MultiV<TYPE> &vX, const MultiV<TYPE> &vY, \
		const Matrix<TYPE> &Cxz, const Matrix<TYPE> &Cyz);\
\
CLUGA_EXT template CLUGA_API bool MultiVSolve(const MultiV<TYPE> &mvA, const MultiV<TYPE> &mvB, MultiV<TYPE> &mvX, tMVPos bPos, Mem<TYPE> *pmDiag);\
\
CLUGA_EXT template CLUGA_API bool MultiVGEVSolve(const MultiV<TYPE> &mvA, const MultiV<TYPE> &mvB,\
					 MemObj<MultiV<TYPE> > &mvX, tMVPos bPos1, tMVPos bPos2, bool bEven);\
\
CLUGA_EXT template CLUGA_API bool MultiVGEVSolveMask(const MultiV<TYPE> &vA, const MultiV<TYPE> &vB, \
							MemObj<MultiV<TYPE> > &mvXList, Mem<TYPE>& mDiag, \
							tMVPos bPos1, tMVPos bPos2, const Mem<int>& mMask);\
\
CLUGA_EXT template CLUGA_API bool MultiVGEVSolveMask(const MemObj<MultiV<TYPE> >&vA, const MemObj<MultiV<TYPE> >&vB, \
							MemObj<MultiV<TYPE> > &mvXList, Mem<TYPE>& mDiag, \
							tMVPos bPos1, tMVPos bPos2, const Mem<int>& mMask);\
\
CLUGA_EXT template CLUGA_API bool EvalVersor(const MemObj<MultiV<TYPE> >&vA, const MemObj<MultiV<TYPE> >&vB, \
							MemObj<MultiV<TYPE> > &mvXList, Mem<TYPE>& mDiag, \
							const Mem<int>& mMask);\
\
CLUGA_EXT template CLUGA_API bool MultiVGEVSolve(const MultiV<TYPE> &mvA, const MultiV<TYPE> &mvB, const MultiV<TYPE> &mvC,\
					 MultiV<TYPE> &vX, tMVPos bPos1, tMVPos bPos2, const Mem<int>& mGrades);\
\
CLUGA_EXT template CLUGA_API bool MultiVGEVSolveMask(const MultiV<TYPE> &mvA, const MultiV<TYPE> &mvB, const MultiV<TYPE> &mvC,\
					 MultiV<TYPE> &vX, tMVPos bPos1, tMVPos bPos2, const Mem<int>& mMask);\
\
CLUGA_EXT template CLUGA_API MultiV<TYPE> Project(const MultiV<TYPE> &mvA, const MultiV<TYPE> &mvX);\
CLUGA_EXT template CLUGA_API MultiV<TYPE> Reject(const MultiV<TYPE> &mvA, const MultiV<TYPE> &mvX);\
\
CLUGA_EXT template CLUGA_API bool GradeList(const MultiV<TYPE> &mvA, Mem<uint> &mList, TYPE dPrec);\
CLUGA_EXT template CLUGA_API bool FactorBlade(const MultiV<TYPE> &vA, MemObj<MultiV<TYPE> > &mvList, \
						  bool bNorm, MultiV<TYPE> *pvB, TYPE *pFac, TYPE dPrec);\
CLUGA_EXT template CLUGA_API bool FactorBlade(const MultiV<TYPE> &vA, const MultiV<TYPE>& vNegDim, \
						  MemObj<MultiV<TYPE> > &mvList, bool bNorm, \
						  MultiV<TYPE> *pvB, TYPE *pFac, TYPE dPrec);\
CLUGA_EXT template CLUGA_API bool FactorBlade(const MultiV<TYPE> &vA, MemObj<MultiV<TYPE> > &mvList, \
						  MemObj<MultiV<TYPE> > &mvBasis, bool bNorm, \
						  int *piOrder, TYPE *pFac, TYPE dPrec);\
\
CLUGA_EXT template CLUGA_API MultiV<TYPE> Join(const MultiV<TYPE>& vA,const MultiV<TYPE>& vB, \
							bool bNorm, TYPE dPrec);\
CLUGA_EXT template CLUGA_API MultiV<TYPE> Join(const MultiV<TYPE>& vA,const MultiV<TYPE>& vB, \
							const MultiV<TYPE>& vNegDim, bool bNorm, TYPE dPrec);\
CLUGA_EXT template CLUGA_API MultiV<TYPE> Meet(const MultiV<TYPE>& mvA,const MultiV<TYPE>& mvB, TYPE dPrec);\
CLUGA_EXT template CLUGA_API MultiV<TYPE> Meet(const MultiV<TYPE>& mvA,const MultiV<TYPE>& mvB, \
							const MultiV<TYPE>& vNegDim, TYPE dPrec);\
\

#endif
