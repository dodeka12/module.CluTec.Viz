////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      matrix.h
//
// summary:   Declares the matrix class
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

// ------------------------- Matrix -----------------------------
#ifndef _MATRIX_HH_
#define _MATRIX_HH_

#include"mem.h"
#include"makestr.h"
#include"mathelp.h"
#include"CStrMem.h"


#ifdef _GNUCPP3_
#ifndef _TMPL_
#define _TMPL_ <>
#endif
#	ifndef _FRIEND_
#		define _FRIEND_  friend 
#	endif
#else
#ifndef _TMPL_
#define _TMPL_ <>
#endif
#	ifndef _FRIEND_
#		define _FRIEND_  friend 
#	endif
#endif

#ifdef _MAXSVDITS_
#	undef _MAXSVDITS_
#endif
#define _MAXSVDITS_ 30   // Maximum Iterations in SVD algorithm


#ifndef _MAXSTRSIZE_
#define _MAXSTRSIZE_ 1024
#endif


typedef unsigned int uint;


template<class CType> class Matrix;


template<class CType> void PrintMatrix (Matrix<CType> &a, const char *title);


template<class CType> bool EigenValues (Matrix<CType> &xMat, Matrix<CType> &xREVals, Matrix<CType> &xIEVals);
template<class CType> bool Eigen(Matrix<CType> &xMat, Matrix<CType> &xREVals, Matrix<CType> &xIEVals,
	Matrix<CType> &xREVecs, Matrix<CType> &xIEVecs);

template<class CType> bool operator== (const Matrix<CType> &a, const Matrix<CType> &b);
template<class CType> bool operator!= (const Matrix<CType> &a, const Matrix<CType> &b);


template<class CType> CType Det (const Matrix<CType> &a);

// Finds eigenvectors and eigenvalues of a symmetric matrix
template<class CType> void EigenOrder (Matrix<CType> &eval, Matrix<CType> &evec);

// Order w in decending order and adjust COLS of v and u.
template<class CType> void SVDOrder (Matrix<CType> &w, Matrix<CType> &u, Matrix<CType> &v, 
	bool bDescending = true);


template<class CType> Matrix<CType> operator~  (const Matrix<CType> &a); // Transpose
template<class CType> Matrix<CType> operator!  (const Matrix<CType> &a); // Inverse
template<class CType> Matrix<CType> operator-  (const Matrix<CType> &a); // Negate

template<class CType> Matrix<CType> operator+  (const Matrix<CType> &a, 
	const Matrix<CType> &b); // Matrix Add.
template<class CType> Matrix<CType>& operator+=  (Matrix<CType> &a, 
	const Matrix<CType> &b); // Matrix Add.
template<class CType> Matrix<CType> operator+  (const Matrix<CType> &a, 
	const CType &b); // Add scalar to all Matrix comps.
template<class CType> Matrix<CType>& operator+=  (Matrix<CType> &a, 
	const CType &b); // Add scalar to all Matrix comps.

template<class CType> Matrix<CType> operator-  (const Matrix<CType> &a, 
	const Matrix<CType> &b); // Matrix Add.
template<class CType> Matrix<CType>& operator-=  (Matrix<CType> &a, 
	const Matrix<CType> &b); // Matrix Add.
template<class CType> Matrix<CType> operator-  (const Matrix<CType> &a, 
	const CType &b); // Add scalar to all Matrix comps.
template<class CType> Matrix<CType>& operator-=  (Matrix<CType> &a, 
	const CType &b); // Add scalar to all Matrix comps.

template<class CType> Matrix<CType> operator*  (const Matrix<CType> &a, const CType &b);
template<class CType> Matrix<CType> operator*  (CType &b, const Matrix<CType> &a);
template<class CType> Matrix<CType> operator*  (const Matrix<CType> &a, 
	const Matrix<CType> &b); // Matrix Mul.

template<class CType> Matrix<CType>& operator*= (Matrix<CType> &a, 
	const Matrix<CType> &b); // Matrix Mul.


//////////////////////////////////////////////////////////////////

template<class CType>
class Matrix
{
public:
	enum EInverseType
	{
		InverseLU = 0,
		InverseSVD,
	};

public:
	Matrix();
	Matrix(uint nrows, uint ncols, const CType nmat[] = 0);
	Matrix(const Matrix<CType> &a);
	~Matrix();

	Matrix<CType>& operator= (const Matrix<CType> &a);
	Matrix<CType>& operator= (const CType nmat[]);
	Matrix<CType>& operator= (const CType val); // Sets all comps. to val.
	CType& operator() (uint row, uint col);
	CType operator() (uint row, uint col) const;

	_FRIEND_ bool operator== _TMPL_(const Matrix<CType> &a, const Matrix<CType> &b);
	_FRIEND_ bool operator!= _TMPL_(const Matrix<CType> &a, const Matrix<CType> &b);

	bool AddRows(uint dest, uint src, const Matrix<CType> &a, uint no = 1);
	bool AddCols(uint dest, uint src, const Matrix<CType> &a, uint no = 1);

	Matrix<CType> SubRows(uint row, uint no = 1);
	Matrix<CType> SubCols(uint row, uint no = 1);

	bool Resize(uint nrows, uint ncols);

	// Keep number of elements but change values for rows and cols.
	bool Reshape(uint nrows, uint ncols);

	uint Rows() const { return m_nRows; }
	uint Cols() const { return m_nCols; }
	bool IsOK();


	bool GetSubMatrix(Matrix<CType> &rMat, uint iTRow, uint iLCol, uint iRowNo, uint iColNo);

	void GetRow(uint row, CType nmat[]);
	void SetRow(uint row, CType nmat[]);
	void GetCol(uint col, CType nmat[]);
	void SetCol(uint col, CType nmat[]);

	void SwapRows(uint row1, uint row2); // Also swaps elements in aug
	void SwapCols(uint col1, uint col2);

	void MulRow(uint row, const CType &a); 
	void MulCol(uint col, const CType &a);

	void Identity(); // Make Matrix Identity MAtrix
	void Diagonal(); // Takes Row or Col vector as diagonal of new matrix
	void Trace();    // Makes Row vector from Trace of Matrix

	CType Det(); // Calculates Determinant
	CType LUDet(); // Determinant of LU decomposed Matrix

	// Finds eigenvectors and eigenvalues of a symmetric matrix
	uint Jacobi(Matrix<CType> &eval, Matrix<CType> &evec);

	// Computing Singular Value Decomposition
	int SVD(Matrix<CType> &u, Matrix<CType> &w, Matrix<CType> &v); 


	Matrix<CType> Left();
	Matrix<CType> Upper();
	bool LUDecomp();


	CType Tiny() { return tiny; }
	void Tiny(CType ntiny) { tiny = ntiny; }
	void TinyToZero(CType fPrec = CType(0));
	Matrix<CType>& Round(CType fPrec = 0);
	// Eval absolute value of all components separately
	bool AbsComps();
	// Eval exponential value of all components separately
	bool ExpComps();
	// Eval exponential value of all components separately
	bool LogComps();
	// Eval power value of all components separately
	bool PowComps(CType dPow, CType fPrec = 0);
	// Eval square root of all components separately. Negative values are set to zero.
	bool SqrtComps(CType fPrec = 0);
	// Invert components of matrix. Zero values are set to inf.
	bool InvComps(CType inf = CType(0), CType fPrec = 0); 
	Matrix<CType> InvSVD(CType fPrec = CType(0)); // Inverse of this matrix using SVD

	// Eval sin of matrix components                                      
	bool SinComps();
	// Eval cos of matrix components                                      
	bool CosComps();
	// Eval tan of matrix components                                      
	bool TanComps();

	// Eval asin of matrix components                                      
	bool ArcSinComps(CType fPrec = 0);
	// Eval acos of matrix components                                      
	bool ArcCosComps(CType fPrec = 0);
	// Eval atan of matrix components                                      
	bool ArcTanComps();


	void FindColWidth();
	char* Str(uint row);
	//_FRIEND_ void printMat _TMPL_(Matrix<CType> &a, const char *title, uint augcol);

	CType* Data() { return m_mData.Data(); }
	const CType* Data() const { return m_mData.Data(); }

	//protected:
	CStrMem str;
	uint colwidth;

	Mem<CType> m_mData;   // Matrix components
	uint m_nRows, m_nCols;   // x and y dimensions of Matrix

	int m_iRowPar, m_iColPar;

	CType tiny;

	EInverseType m_eInvType;

	CType pythag(CType a, CType b); // Calc sqrt(a^2 + b^2)

	inline CType sign(CType a, CType b)
	{ return ( (b >= CType(0)) ? Mag(a) : -Mag(a) ); }

	inline CType maxi(CType a, CType b) 
	{ return ( (a >= b) ? a : b ); }


};

#endif
