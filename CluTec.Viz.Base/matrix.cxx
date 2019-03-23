////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      matrix.cxx
//
// summary:   matrix class
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

// ------ Matrix Class Library ------
/*
Although the Matrix class is a template class, it may only be used with
types that are data only. That is, they may not create memory block by
themselves, nor are they allowed to have member functions. However,
friend functions have to defined for casting from integer, multiplication
addition, subtraction. comparison, division. Also a function Magnitude has
to be defined.
*/

#ifndef _MATRIX_TEMPLATE_
	#define _MATRIX_TEMPLATE_

#include <string.h>
#include <math.h>
#include <stdio.h>
#include <float.h>

//#include "eispack.h"
#include "matrix.h"
#include "matinst.h"

#include "malloc.h"

//#ifndef _DEBUG
//#	define calloc	dlcalloc
//#	define free		dlfree
//#endif

#ifndef WIN32

    #include <math.h>
		#define _isnan isnan

#endif

	extern "C"
	{
	/* convert matrix to upper Hessenberg form */
	void elmhes(double** a, int n, int lo, int hi, int intch[]);

	/* solve upper Hessenberg matrix for eigenvalues */
	void hqr(int n, int lo, int hi, double** h, double* wr, double* wi, int* ierr);

	/* solve upper Hessenberg matrix for eigenvalues and eigenvectors */
	void hqr2(int n, int lo, int hi, double** h, double* wr, double* wi, double** z, int* ierr);

	/* accumulates stabilized similarity transformations */
	void eltran(int n, int lo, int hi, double** a, int intch[], double** z);

	/* Balance Matrix */
	void balanc(double** a, int n, int* lo, int* hi, double scale[]);

	/* Transform Eigenvectors of Balanced Matrix back to original matrix */
	void balbak(int n, int lo, int hi, double* scale, int m, double** z);

	/* does not normalize eigenvectors */
	//void pnormwr(int n,double **z,double *wr,double *wi);
	}

// ------------------- Additional Functions ----------------

	template<class CType>
	void PrintMatrix(Matrix<CType>& a, const char* title)
	{
		uint r    = a.m_nRows;
		uint len  = uint(strlen(title) + 2);
		char* buf = new char[len + 1];

		memset(buf, ' ', len);
		buf[len] = 0;

		a.FindColWidth();

		for (uint i = 1; i <= r; i++)
		{
			if (i == 1)
			{
				printf("%s: %s\n", title, a.Str(i));
				if (r == 1) { printf("\n"); }
			}
			else if (i == r)
			{
				printf("%s%s\n\n", buf, a.Str(i));
			}
			else
			{
				printf("%s%s\n", buf, a.Str(i));
			}
		}

		delete[] buf;
	}

// ------------------ Class Functions -------------------------

// ////////////////////////////////////////////////////////////////////////////////////////
// Default Constructor
	template<class CType>
	Matrix<CType>::Matrix()
	{
		str      = "";
		colwidth = 1;
		tiny     = CType(1e-14);

		m_nRows = 0;
		m_nCols = 0;

		m_iRowPar = 1;
		m_iColPar = 1;

		m_eInvType = InverseSVD;
	}

// ////////////////////////////////////////////////////////////////////////////////////////
// Constructor
	template<class CType>
	Matrix<CType>::Matrix(uint uRows, uint uCols, const CType pdData[])
	{
		str        = "";
		colwidth   = 1;
		tiny       = CType(1e-14);
		m_iRowPar  = 1;
		m_iColPar  = 1;
		m_eInvType = InverseSVD;

		if (uRows && uCols)
		{
			m_nRows = uRows;
			m_nCols = uCols;

			if (m_mData.Set(m_nRows * m_nCols))
			{
				if (pdData)
				{
					memcpy(m_mData.Data(), pdData, m_mData.Count() * sizeof(CType));
				}
				else
				{
					memset(m_mData.Data(), 0, m_mData.Count() * sizeof(CType));
				}
			}
		}
		else
		{
			m_nRows = 0;
			m_nCols = 0;
			m_mData.Set(0);
		}
	}

// ////////////////////////////////////////////////////////////////////////////////////////
// Copy Constructor
	template<class CType>
	Matrix<CType>::Matrix(const Matrix<CType>& a)
	{
		colwidth = a.colwidth;
		tiny     = a.tiny;

		m_nRows    = a.m_nRows;
		m_nCols    = a.m_nCols;
		m_iRowPar  = a.m_iRowPar;
		m_iColPar  = a.m_iColPar;
		m_eInvType = a.m_eInvType;

		m_mData = a.m_mData;
	}

// ////////////////////////////////////////////////////////////////////////////////////////
// Destructor
	template<class CType>
	Matrix<CType>::~Matrix()
	{
	}

// ////////////////////////////////////////////////////////////////////////////////////////
// Assignment of Matrix
	template<class CType>
	Matrix<CType>& Matrix<CType>::operator=(const Matrix<CType>& a)
	{
		if (a.m_nRows && a.m_nCols)
		{
			str        = "";
			colwidth   = a.colwidth;
			tiny       = a.tiny;
			m_nRows    = a.m_nRows;
			m_nCols    = a.m_nCols;
			m_iRowPar  = a.m_iRowPar;
			m_iColPar  = a.m_iColPar;
			m_eInvType = a.m_eInvType;
			m_mData    = a.m_mData;
		}

		return *this;
	}

// ////////////////////////////////////////////////////////////////////////////////////////
// Assignment of Matrix
	template<class CType>
	Matrix<CType>& Matrix<CType>::operator=(const CType pdData[])
	{
		if (pdData)
		{
			memcpy(m_mData.Data(), pdData, m_mData.Count() * sizeof(CType));
		}
		else
		{
			memset(m_mData.Data(), 0, m_mData.Count() * sizeof(CType));
		}

		return *this;
	}

// ////////////////////////////////////////////////////////////////////////////////////////
// Assignment of value to all comps of Matrix
	template<class CType>
	Matrix<CType>& Matrix<CType>::operator=(const CType dVal)
	{
		CType* pdData;

		if ((pdData = m_mData.Data()))
		{
			uint uIdx, uCnt = m_nRows * m_nCols;

			for (uIdx = 0; uIdx < uCnt; ++uIdx)
			{
				pdData[uIdx] = dVal;
			}
		}

		return *this;
	}

// ////////////////////////////////////////////////////////////////////////////////////////
// Accessing component of Matrix
	template<class CType>
	CType& Matrix<CType>::operator()(uint uRow, uint uCol)
	{
		if (uRow && uCol && (uRow <= m_nRows) && (uCol <= m_nCols))
		{
			return m_mData[(uRow - 1) * m_nCols + uCol - 1];
		}

		throw std::exception("Matrix index out of range");
	}

// ////////////////////////////////////////////////////////////////////////////////////////
// Accessing component of Matrix
	template<class CType>
	CType Matrix<CType>::operator()(uint uRow, uint uCol) const
	{
		if (uRow && uCol && (uRow <= m_nRows) && (uCol <= m_nCols))
		{
			return m_mData[(uRow - 1) * m_nCols + uCol - 1];
		}

		throw std::exception("Matrix index out of range");
	}

// ////////////////////////////////////////////////////////////////////////////////////////
// Comparing Matrices ==
	template<class CType>
	bool operator==(const Matrix<CType>& a, const Matrix<CType>& b)
	{
		if ((a.m_nRows != b.m_nRows) || (a.m_nCols != b.m_nCols))
		{
			return false;
		}

		return memcmp(a.m_mData.Data(), b.m_mData.Data(), a.m_nRows * a.m_nCols * sizeof(CType)) == 0;
	}

// ////////////////////////////////////////////////////////////////////////////////////////
// Comparing Matrices !=
	template<class CType>
	bool operator!=(const Matrix<CType>& a, const Matrix<CType>& b)
	{
		if ((a.m_nRows != b.m_nRows) || (a.m_nCols != b.m_nCols))
		{
			return true;
		}

		return memcmp(a.m_mData.Data(), b.m_mData.Data(), a.m_nRows * a.m_nCols * sizeof(CType)) != 0;
	}

// ////////////////////////////////////////////////////////////////////////////////////////
// Checking Matrix
	template<class CType>
	bool Matrix<CType>::IsOK()
	{
		if (!m_nRows || !m_nCols || !m_mData.Data())
		{
			return false;
		}

		uint uSize = m_nRows * m_nCols;

		if (m_mData.Count() != uSize)
		{
			return false;
		}

		return true;
	}

// ////////////////////////////////////////////////////////////////////////////////////////
// Set Tiny elements to zero
	template<class CType>
	void Matrix<CType>::TinyToZero(CType fPrec)
	{
		if (!m_nRows || !m_nCols) { return; }

		CType* data = m_mData.Data();
		CType big   = CType(0);
		CType h;
		uint size = m_nRows * m_nCols;
		uint i;

		if (fPrec <= 0)
		{
			fPrec = tiny;
		}

		for (i = 0; i < size; i++)
		{
			if ((h = Mag(data[i])) > big) { big = h; } }

		big *= fPrec;

		for (i = 0; i < size; i++)
		{
			if (Mag(data[i]) <= big) { data[i] = CType(0); } }
	}

/////////////////////////////////////////////////////
/// Round elements of Matrix

	template<class CType>
	Matrix<CType>& Matrix<CType>::Round(CType fPrec)
	{
		if (!m_nRows || !m_nCols) { return *this; }

		CType* data = m_mData.Data();
		CType fTiny;
		uint size = m_nRows * m_nCols;
		uint i;

		if (fPrec == CType(0))
		{
			fPrec = ::Tiny(fTiny);
		}

		for (i = 0; i < size; i++)
		{
			data[i] = ::Round((CType) data[i], (CType) fPrec);
		}

		return *this;
	}

/////////////////////////////////////////////////////
/// Take absolute value of elements of Matrix

	template<class CType>
	bool Matrix<CType>::AbsComps()
	{
		if (!m_nRows || !m_nCols)
		{
			return true;
		}

		CType* data = m_mData.Data();
		uint size   = m_nRows * m_nCols;
		uint i;

		for (i = 0; i < size; i++)
		{
			data[i] = Mag(data[i]);
		}

		return true;
	}

/////////////////////////////////////////////////////
/// Take exponential value of elements of Matrix

	template<class CType>
	bool Matrix<CType>::ExpComps()
	{
		if (!m_nRows || !m_nCols)
		{
			return true;
		}

		CType* data = m_mData.Data();
		uint size   = m_nRows * m_nCols;
		uint i;

		for (i = 0; i < size; i++)
		{
			data[i] = CType(exp(double(data[i])));
		}

		return true;
	}

/////////////////////////////////////////////////////
/// Take logarithmic value of elements of Matrix

	template<class CType>
	bool Matrix<CType>::LogComps()
	{
		if (!m_nRows || !m_nCols)
		{
			return true;
		}

		CType dVal;
		CType* data = m_mData.Data();
		uint size   = m_nRows * m_nCols;
		uint i;

		for (i = 0; i < size; i++)
		{
			dVal = CType(log(double(data[i])));
			if (_isnan(double(dVal)))
			{
				return false;
			}
			data[i] = dVal;
		}

		return true;
	}

/////////////////////////////////////////////////////
/// Take square root of elements of Matrix

	template<class CType>
	bool Matrix<CType>::SqrtComps(CType fPrec)
	{
		if (!m_nRows || !m_nCols)
		{
			return true;
		}

		CType* data = m_mData.Data();
		uint size   = m_nRows * m_nCols;
		uint i;
		double dVal;

		CType fTiny;
		if (fPrec == CType(0))
		{
			fPrec = ::Tiny(fTiny);
		}

		for (i = 0; i < size; i++)
		{
			dVal = double(data[i]);
			if ((dVal < 0.0) && (dVal >= -fPrec))
			{
				data[i] = CType(0);
			}
			else if (dVal >= 0.0)
			{
				data[i] = CType(sqrt(dVal));
			}
			else
			{
				return false;
			}
		}

		return true;
	}

/////////////////////////////////////////////////////
/// Take power value of elements of Matrix

	template<class CType>
	bool Matrix<CType>::PowComps(CType dVal, CType fPrec)
	{
		if (!m_nRows || !m_nCols)
		{
			return true;
		}

		CType fTiny;
		if (fPrec == CType(0))
		{
			fPrec = ::Tiny(fTiny);
		}

		CType dRes;
		CType* data = m_mData.Data();
		uint size   = m_nRows * m_nCols;
		uint i;

		for (i = 0; i < size; i++)
		{
			if (((data[i] > -fPrec) && (data[i] < fPrec)) && (dVal < CType(0)))
			{
				return false;
			}

			dRes = CType(pow(double(data[i]), double(dVal)));
			if (_isnan(double(dRes)))
			{
				return false;
			}

			data[i] = dRes;
		}

		return true;
	}

/////////////////////////////////////////////////////
/// Take sin value of elements of Matrix

	template<class CType>
	bool Matrix<CType>::SinComps()
	{
		if (!m_nRows || !m_nCols) { return true; }

		CType* data = m_mData.Data();
		uint size   = m_nRows * m_nCols;
		uint i;

		for (i = 0; i < size; i++)
		{
			data[i] = CType(sin(double(data[i])));
		}

		return true;
	}

/////////////////////////////////////////////////////
/// Take cos value of elements of Matrix

	template<class CType>
	bool Matrix<CType>::CosComps()
	{
		if (!m_nRows || !m_nCols) { return true; }

		CType* data = m_mData.Data();
		uint size   = m_nRows * m_nCols;
		uint i;

		for (i = 0; i < size; i++)
		{
			data[i] = CType(cos(double(data[i])));
		}

		return true;
	}

/////////////////////////////////////////////////////
/// Take tan value of elements of Matrix

	template<class CType>
	bool Matrix<CType>::TanComps()
	{
		if (!m_nRows || !m_nCols) { return true; }

		CType* data = m_mData.Data();
		uint size   = m_nRows * m_nCols;
		uint i;

		for (i = 0; i < size; i++)
		{
			data[i] = CType(tan(double(data[i])));
		}

		return true;
	}

/////////////////////////////////////////////////////
/// Take asin value of elements of Matrix

	template<class CType>
	bool Matrix<CType>::ArcSinComps(CType fPrec)
	{
		if (!m_nRows || !m_nCols)
		{
			return true;
		}

		CType fTiny;
		if (fPrec == CType(0))
		{
			fPrec = ::Tiny(fTiny);
		}

		CType dVal;
		CType* data = m_mData.Data();
		uint size   = m_nRows * m_nCols;
		uint i;

		for (i = 0; i < size; i++)
		{
			dVal = data[i];

			if ((dVal < CType(-1.0)) && (dVal >= CType(-1.0) - fPrec))
			{
				dVal = CType(-1.0);
			}
			else if (dVal < CType(-1.0))
			{
				return false;
			}

			if ((dVal > CType(1.0)) && (dVal <= CType(1.0) + fPrec))
			{
				dVal = CType(1.0);
			}
			else if (dVal > CType(1.0))
			{
				return false;
			}

			data[i] = CType(asin(double(dVal)));
		}

		return true;
	}

/////////////////////////////////////////////////////
/// Take acos value of elements of Matrix

	template<class CType>
	bool Matrix<CType>::ArcCosComps(CType fPrec)
	{
		if (!m_nRows || !m_nCols)
		{
			return true;
		}

		CType fTiny;
		if (fPrec == CType(0))
		{
			fPrec = ::Tiny(fTiny);
		}

		CType dVal;
		CType* data = m_mData.Data();
		uint size   = m_nRows * m_nCols;
		uint i;

		for (i = 0; i < size; i++)
		{
			dVal = data[i];

			if ((dVal < CType(-1.0)) && (dVal >= CType(-1.0) - fPrec))
			{
				dVal = CType(-1.0);
			}
			else if (dVal < CType(-1.0))
			{
				return false;
			}

			if ((dVal > CType(1.0)) && (dVal <= CType(1.0) + fPrec))
			{
				dVal = CType(1.0);
			}
			else if (dVal > CType(1.0))
			{
				return false;
			}

			data[i] = CType(acos(double(dVal)));
		}

		return true;
	}

/////////////////////////////////////////////////////
/// Take atan value of elements of Matrix

	template<class CType>
	bool Matrix<CType>::ArcTanComps()
	{
		if (!m_nRows || !m_nCols)
		{
			return true;
		}

		CType* data = m_mData.Data();
		uint size   = m_nRows * m_nCols;
		uint i;

		for (i = 0; i < size; i++)
		{
			data[i] = CType(atan(double(data[i])));
		}

		return true;
	}

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Resize Matrix
	template<class CType>
	bool Matrix<CType>::Resize(uint nRows, uint nCols)
	{
		if (!nRows || !nCols)
		{
			return false;
		}

		uint uCurSize = m_nRows * m_nCols;
		uint uNewSize = nRows * nCols;

		// Just add sufficient memory to the current matrix memory.
		// This can be faster than creating a new matrix, as we use
		// memory allocation with a granularity larger than 1.
		if (!m_mData.Add(uNewSize))
		{
			return false;
		}

		if (m_nRows && m_nCols)
		{
			CType* pdData = m_mData.Data();

			// Memory for new matrix lies behind current matrix
			CType* pdNewData = &pdData[uCurSize];

			// Initialize new matrix memory with zeros
			memset(pdNewData, 0, uNewSize * sizeof(CType));

			uint xr = ((m_nRows > nRows) ? (nRows) : (m_nRows));
			uint xc = ((m_nCols > nCols) ? (nCols) : (m_nCols));
			uint r, c, rp, nrp;

			for (r = 0; r < xr; r++)
			{
				for (c = 0, rp = r * m_nCols, nrp = r * nCols; c < xc; c++)
				{
					pdNewData[nrp + c] = pdData[rp + c];
				}
			}

			// Now move the new matrix memory to the start of the data buffer
			memmove(pdData, pdNewData, uNewSize * sizeof(CType));

			// Resize the memory to the correct size, which may not actually
			// change the amount of allocated memory.
			m_mData.Set(uNewSize);
			m_nCols   = nCols;
			m_nRows   = nRows;
			m_iColPar = 1;
			m_iRowPar = 1;
		}
		else
		{
			// Initialize new matrix memory with zeros
			memset(m_mData.Data(), 0, uNewSize * sizeof(CType));

			m_nCols   = nCols;
			m_nRows   = nRows;
			m_iColPar = 1;
			m_iRowPar = 1;
		}

		return true;
	}

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Reshape Matrix
	template<class CType>
	bool Matrix<CType>::Reshape(uint nRows, uint nCols)
	{
		if (!nRows || !nCols || (nRows * nCols != m_nRows * m_nCols))
		{
			return false;
		}

		m_nRows = nRows;
		m_nCols = nCols;

		return true;
	}

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Get Row of Matrix
	template<class CType>
	void Matrix<CType>::GetRow(uint row, CType nmat[])
	{
		if (row > m_nRows) { return; }

		CType* data = m_mData.Data();

		memcpy(nmat, &data[(row - 1) * m_nCols], m_nCols * sizeof(CType));
	}

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Set Row of Matrix
	template<class CType>
	void Matrix<CType>::SetRow(uint row, CType nmat[])
	{
		if (row > m_nRows) { return; }

		CType* data = m_mData.Data();

		memcpy(&data[(row - 1) * m_nCols], nmat, m_nCols * sizeof(CType));
	}

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Get Column of Matrix
	template<class CType>
	void Matrix<CType>::GetCol(uint col, CType nmat[])
	{
		if (!col || (col > m_nCols)) { return; }

		CType* data = m_mData.Data();

		for (uint i = 0; i < m_nRows; i++)
		{
			nmat[i] = data[i * m_nCols + col - 1];
		}
	}

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Set Column of Matrix
	template<class CType>
	void Matrix<CType>::SetCol(uint nCol, CType nmat[])
	{
		if (!nCol || (nCol > m_nCols)) { return; }

		CType* data = m_mData.Data();

		for (uint i = 0; i < m_nRows; i++)
		{
			data[i * m_nCols + nCol - 1] = nmat[i];
		}
	}

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Swap Rows of Matrix
	template<class CType>
	void Matrix<CType>::SwapRows(uint nRow1, uint nRow2)
	{
		if ((nRow1 == nRow2) || !nRow1 || !nRow2
		    || (nRow1 > m_nRows) || (nRow2 > m_nRows)) { return; }

		uint size   = m_nCols * sizeof(CType);
		char* srow  = new char[size];
		CType* data = m_mData.Data();
		//CType dum;

		nRow1--;
		nRow2--;

		memcpy(srow, &data[nRow1 * m_nCols], size);
		memmove(&data[nRow1 * m_nCols], &data[nRow2 * m_nCols], size);
		memcpy(&data[nRow2 * m_nCols], srow, size);

		m_iRowPar *= 1 - 2 * (Mag(int(nRow1) - int(nRow2)) % 2);
	}

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Swap Columns of Matrix
	template<class CType>
	void Matrix<CType>::SwapCols(uint col1, uint col2)
	{
		if ((col1 == col2) || !col1 || !col2
		    || (col1 > m_nCols) || (col2 > m_nCols)) { return; }

		CType s;
		CType* data = m_mData.Data();

		col1--;
		col2--;

		for (uint i = 0; i < m_nRows; i++)
		{
			s                        = data[i * m_nCols + col1];
			data[i * m_nCols + col1] = data[i * m_nCols + col2];
			data[i * m_nCols + col2] = s;
		}

		m_iColPar *= 1 - 2 * (Mag(int(col1) - int(col2)) % 2);
	}

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Multiply a Row with a constant
	template<class CType>
	void Matrix<CType>::MulRow(uint row, const CType& a)
	{
		if (!row || (row > m_nRows)) { return; }

		CType* data = m_mData.Data();
		uint rp     = (row - 1) * m_nCols;

		for (uint i = 0; i < m_nCols; i++)
		{
			data[rp + i] *= a;
		}
	}

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Multiply a Column with a constant
	template<class CType>
	void Matrix<CType>::MulCol(uint col, const CType& a)
	{
		if (!col || (col > m_nCols)) { return; }

		CType* data = m_mData.Data();

		for (uint i = 0; i < m_nRows; i++)
		{
			data[i * m_nCols + col - 1] *= a;
		}
	}

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Identity Matrix
	template<class CType>
	void Matrix<CType>::Identity()
	{
		if (m_nRows != m_nCols)
		{
			return;
		}

		CType* data = m_mData.Data();
		uint i, j;
		uint size = m_nRows * m_nCols;

		for (i = 0, j = 0; i < size; i++)
		{
			if (i == j)
			{
				data[i] = CType(1);
				j      += m_nCols + 1;
			}
			else
			{
				data[i] = CType(0);
			}
		}
	}

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Transpose Matrix
	template<class CType>
	Matrix<CType> operator~(const Matrix<CType>& a)
	{
		Matrix<CType> b;

		if (!a.m_nRows || !a.m_nCols)
		{
			return b;
		}

		b.Resize(a.m_nCols, a.m_nRows);

		if ((a.m_nRows == 1) && (a.m_nCols == 1))
		{
			b.m_mData[0] = a.m_mData[0];
		}
		else
		{
			CType* adata = a.m_mData.Data();
			CType* bdata = b.m_mData.Data();
			uint uRow, uCol, rp;

			for (uRow = 0; uRow < a.m_nRows; uRow++)
			{
				for (uCol = 0, rp = uRow * a.m_nCols; uCol < a.m_nCols; uCol++)
				{
					bdata[uCol * b.m_nCols + uRow] = adata[rp + uCol];
				}
			}
		}

		return b;
	}

// Left Part of Matrix
	template<class CType>
	Matrix<CType> Matrix<CType>::Left()
	{
		Matrix<CType> a(m_nRows, m_nCols);

		if (!m_nRows || !m_nCols || (m_nRows != m_nCols)) { return a; }

		CType* data  = m_mData.Data();
		CType* adata = a.m_mData.Data();
		uint r, c, rp;

		for (r = 1; r < m_nRows; r++)
		{
			for (c = 0, rp = r * m_nCols; c < r; c++)
			{
				adata[rp + c] = data[rp + c];
			}
		}

		return a;
	}

// Upper Part of Matrix
	template<class CType>
	Matrix<CType> Matrix<CType>::Upper()
	{
		Matrix<CType> a(m_nRows, m_nCols);

		if (!m_nRows || !m_nCols || (m_nRows != m_nCols)) { return a; }

		CType* data  = m_mData.Data();
		CType* adata = a.m_mData.Data();
		uint r, c, rp;

		for (r = 0; r < m_nRows; r++)
		{
			for (c = r, rp = r * m_nCols; c < m_nRows; c++)
			{
				adata[rp + c] = data[rp + c];
			}
		}

		return a;
	}

// LU Decomposition
	template<class CType>
	bool Matrix<CType>::LUDecomp()
	{
		if (m_nRows != m_nCols) { return false; }

		CType* data    = m_mData.Data();
		CType* impscal = new CType[m_nRows];	// Keeps track of implicit scaling of rows
		CType big;	// Biggest element in row
		CType dum;	// Dummy Variable
		CType sum;	// Used for summation
		uint bestrow = 0;	// Row with largest figure of merit

		uint r, c, rp, i;
		m_iRowPar = 1;
		m_iColPar = 1;

		for (r = 0; r < m_nRows; r++)
		{
			big = CType(0);
			rp  = r * m_nCols;

			for (c = 0; c < m_nCols; c++)
			{
				dum = (CType) Mag(data[rp + c]);
				if (dum > big) { big = dum; }
			}

			if (big == CType(0))
			{
				delete[] impscal;
				return false;
			}

			impscal[r] = CType(1) / big;
		}

		for (c = 0; c < m_nCols; c++)
		{
			// printf("Loop 1:----- \n\n");
			for (r = 0; r < c; r++)
			{
				rp  = r * m_nCols;
				sum = data[rp + c];
				for (i = 0; i < r; i++)
				{
					sum -= data[rp + i] * data[i * m_nCols + c];
				}

				data[rp + c] = sum;
				//      printf("rp: %u, c: %u, sum: %lf\n", rp, c, sum);
			}

			//    printf("Loop 2:----- \n\n");
			big = CType(0);
			for (r = c; r < m_nRows; r++)
			{
				rp  = r * m_nCols;
				sum = data[rp + c];
				for (i = 0; i < c; i++)
				{
					sum -= data[rp + i] * data[i * m_nCols + c];
				}

				data[rp + c] = sum;

				//      printf("\nrp: %u, c: %u, sum: %lf\n", rp, c, sum);

				dum = impscal[r] * Mag(sum);
				if (dum >= big)
				{
					big     = dum;
					bestrow = r;
				}
				//      printf("c: %d, r: %d\n", c, r);
				//      printf("sum: %lf, dum: %lf, big: %lf, bestrow: %u\n\n",
				//	     sum, dum, big, bestrow);
			}

			if (c != bestrow)
			{
				SwapRows(c + 1, bestrow + 1);
				dum              = impscal[bestrow];
				impscal[bestrow] = impscal[c];
				impscal[c]       = dum;
			}

			if (data[c * m_nCols + c] == CType(0))	// May want to set element to
			{
				delete[] impscal;			// a tiny element and carry on
				return false;
			}

			if (c + 1 < m_nCols)
			{
				dum = CType(1) / data[c * m_nCols + c];
				for (r = c + 1; r < m_nRows; r++)
				{
					data[r * m_nCols + c] *= dum;
				}
			}
		}	// Do the same for next column

		delete[] impscal;
		return true;
	}

// Backsubstitution of LU decomposed MAtrix
//template<class CType>
//Matrix<CType> Matrix<CType>::LUBackSub()
//{
//	Matrix<CType> X(m_nRows, 1, m_mAug->Data());  // Resultant Vector
//
//	CType *data = m_mData.Data();
//	CType *Xdata = X.m_mData.Data();
//	CType sum;
//
//	uint r, c, rp;
//	int first = -1;
//
//	X.rowidx[0] = rowidx[0];
//	X.rowpar = rowpar;
//
//	for(r=0;r<m_nRows;r++)
//	{ rp = r*m_nCols;
//	sum = Xdata[r];
//	if (first > -1)
//		for(c=first;c<r;c++)
//			sum -= data[rp+c] * Xdata[c];
//	else if (sum != CType(0))
//		first = r;
//
//	Xdata[r] = sum;
//	}
//
//	for(r=m_nRows-1;r>=0;r--)
//	{ rp = r*m_nCols;
//	sum = Xdata[r];
//	for(c=r+1;c<m_nCols;c++)
//		sum -= data[rp+c] * Xdata[c];
//
//	Xdata[r] = sum / data[rp+r];
//	}
//
//	return X;
//}

// Determinant of Matrix
	template<class CType>
	CType Matrix<CType>::Det()
	{
		Matrix<CType> a(*this);
		CType r = CType(0);

		if (a.LUDecomp())
		{
			CType* data = a.m_mData.Data();
			uint i, j;
			r = CType(a.m_iRowPar);

			for (i = 0, j = 0; i < a.m_nRows; i++, j += a.m_nCols + 1)
			{
				r *= data[j];
			}
		}

		return r;
	}

// Determinant of LU Decomposed Matrix
	template<class CType>
	CType Matrix<CType>::LUDet()
	{
		if (!m_nRows || !m_nCols || (m_nCols != m_nRows))
		{
			return CType(0);
		}

		CType r     = CType(m_iRowPar);
		CType* data = m_mData.Data();
		uint i, j;

		for (i = 0, j = 0; i < m_nRows; i++, j += m_nCols + 1)
		{
			r *= data[j];
		}

		return r;
	}

// Det. of friend Matrix
	template<class CType>
	CType Det(const Matrix<CType>& a)
	{
		Matrix<CType> b(a);

		return b.Det();
	}

// Det. of friend LU Matrix
	template<class CType>
	CType LUDet(const Matrix<CType>& a)
	{
		Matrix<CType> b(a);

		return b.LUDet();
	}

// Inverse of  Matrix
	template<class CType>
	Matrix<CType> operator!(const Matrix<CType>& a)
	{
		Matrix<CType> b(a);
		return b.InvSVD();

		//switch(a.m_eInvType)
		//{
		//case Matrix<CType>::InverseLU:
		//	//if (b.m_nCols == b.m_nRows)
		//	//	return b.InvLU();
		//	break;

		//case Matrix<CType>::InverseSVD:
		//	return b.InvSVD();
		//	break;
		//}

		//return b;
	}

// Negative of Matrix
	template<class CType>
	Matrix<CType> operator-(const Matrix<CType>& a)
	{
		Matrix<CType> b(a);

		CType* data = b.m_mData.Data();
		uint size   = a.m_nRows * a.m_nCols;

		for (uint i = 0; i < size; i++)
		{
			data[i] = -data[i];
		}

		return b;
	}

// Multiplication of Matrix with constant
	template<class CType>
	Matrix<CType> operator*(const Matrix<CType>& a, const CType& b)
	{
		Matrix<CType> c(a);

		CType* data = c.m_mData.Data();
		uint size   = a.m_nRows * a.m_nCols;

		for (uint i = 0; i < size; i++)
		{
			data[i] *= b;
		}

		return c;
	}

// Multiplication of Matrix with constant
	template<class CType>
	Matrix<CType> operator*(CType& b, const Matrix<CType>& a)
	{
		Matrix<CType> c(a);

		CType* data = c.m_mData.Data();
		uint size   = a.m_nRows * a.m_nCols;

		for (uint i = 0; i < size; i++)
		{
			data[i] *= b;
		}

		return c;
	}

// Multiplication of Matrix with Matrix
	template<class CType>
	Matrix<CType> operator*(const Matrix<CType>& a, const Matrix<CType>& b)
	{
		Matrix<CType> c(a.m_nRows, b.m_nCols);

		if (!a.m_nCols || !a.m_nRows || !b.m_nCols || !b.m_nRows || (a.m_nCols != b.m_nRows)) { return c; }

		CType* adata = a.m_mData.Data();
		CType* bdata = b.m_mData.Data();
		CType* cdata = c.m_mData.Data();

		CType dA, dB;
		uint ra, ca, cb;
		uint rpa, rpc, cpos, apos, bpos;

		for (ra = 0; ra < a.m_nRows; ra++)
		{
			rpa = ra * a.m_nCols;
			rpc = ra * b.m_nCols;
			for (cb = 0; cb < b.m_nCols; cb++)
			{
				cpos = rpc + cb;
				for (ca = 0, apos = rpa, bpos = cb; ca < a.m_nCols; ca++, apos++, bpos += b.m_nCols)
				{
					dA = adata[apos];
					dB = bdata[bpos];
					if ((dA != CType(0)) && (dB != CType(0)))
					{
						cdata[cpos] +=  dA * dB;
					}
				}
			}
		}

		return c;
	}

// Multiplication of this Matrix with Matrix
	template<class CType>
	Matrix<CType>& operator*=(Matrix<CType>& a, const Matrix<CType>& b)
	{
		a = a * b;

		return a;
	}

// Addition of Matrix to Matrix
	template<class CType>
	Matrix<CType> operator+(const Matrix<CType>& a, const Matrix<CType>& b)
	{
		Matrix<CType> c(a.m_nRows, a.m_nCols);

		if (!a.m_nCols || !a.m_nRows || !b.m_nCols || !b.m_nRows ||
		    (a.m_nCols != b.m_nCols) || (a.m_nRows != b.m_nRows)) { return c; }

		CType* adata = a.m_mData.Data();
		CType* bdata = b.m_mData.Data();
		CType* cdata = c.m_mData.Data();
		uint size    = a.m_nRows * a.m_nCols;

		for (uint i = 0; i < size; i++)
		{
			cdata[i] = adata[i] + bdata[i];
		}

		return c;
	}

// Addition of Matrix to Matrix
	template<class CType>
	Matrix<CType>& operator+=(Matrix<CType>& a, const Matrix<CType>& b)
	{
		if (!a.m_nCols || !a.m_nRows || !b.m_nCols || !b.m_nRows ||
		    (a.m_nCols != b.m_nCols) || (a.m_nRows != b.m_nRows)) { return a; }

		CType* adata = a.m_mData.Data();
		CType* bdata = b.m_mData.Data();
		uint size    = a.m_nRows * a.m_nCols;

		for (uint i = 0; i < size; i++)
		{
			adata[i] += bdata[i];
		}

		return a;
	}

// Addition of scalar to all comps. of Matrix
	template<class CType>
	Matrix<CType> operator+(const Matrix<CType>& a, const CType& b)
	{
		Matrix<CType> c(a.m_nRows, a.m_nCols);

		if (!a.m_nCols || !a.m_nRows) { return c; }

		CType* adata = a.m_mData.Data();
		CType* cdata = c.m_mData.Data();
		uint size    = a.m_nRows * a.m_nCols;

		for (uint i = 0; i < size; i++)
		{
			cdata[i] = adata[i] + b;
		}

		return c;
	}

// Addition of scalar to all comps. of Matrix
	template<class CType>
	Matrix<CType>& operator+=(Matrix<CType>& a, const CType& b)
	{
		if (!a.m_nCols || !a.m_nRows) { return a; }

		CType* adata = a.m_mData.Data();
		uint size    = a.m_nRows * a.m_nCols;

		for (uint i = 0; i < size; i++)
		{
			adata[i] += b;
		}

		return a;
	}

// Subtraction of Matrix from Matrix
	template<class CType>
	Matrix<CType> operator-(const Matrix<CType>& a, const Matrix<CType>& b)
	{
		Matrix<CType> c(a.m_nRows, a.m_nCols);

		if (!a.m_nCols || !a.m_nRows || !b.m_nCols || !b.m_nRows ||
		    (a.m_nCols != b.m_nCols) || (a.m_nRows != b.m_nRows)) { return c; }

		CType* adata = a.m_mData.Data();
		CType* bdata = b.m_mData.Data();
		CType* cdata = c.m_mData.Data();
		uint size    = a.m_nRows * a.m_nCols;

		for (uint i = 0; i < size; i++)
		{
			cdata[i] = adata[i] - bdata[i];
		}

		return c;
	}

// Subtraction of Matrix from this Matrix
	template<class CType>
	Matrix<CType>& operator-=(Matrix<CType>& a, const Matrix<CType>& b)
	{
		if (!a.m_nCols || !a.m_nRows || !b.m_nCols || !b.m_nRows ||
		    (a.m_nCols != b.m_nCols) || (a.m_nRows != b.m_nRows)) { return a; }

		CType* adata = a.m_mData.Data();
		CType* bdata = b.m_mData.Data();
		uint size    = a.m_nRows * a.m_nCols;

		for (uint i = 0; i < size; i++)
		{
			adata[i] -= bdata[i];
		}

		return a;
	}

// Subtraction of scalar to all comps. of Matrix
	template<class CType>
	Matrix<CType> operator-(const Matrix<CType>& a, const CType& b)
	{
		Matrix<CType> c(a.m_nRows, a.m_nCols);

		if (!a.m_nCols || !a.m_nRows) { return c; }

		CType* adata = a.m_mData.Data();
		CType* cdata = c.m_mData.Data();
		uint size    = a.m_nRows * a.m_nCols;

		for (uint i = 0; i < size; i++)
		{
			cdata[i] = adata[i] - b;
		}

		return c;
	}

// Subtraction of scalar to all comps. of Matrix
	template<class CType>
	Matrix<CType>& operator-=(Matrix<CType>& a, const CType& b)
	{
		if (!a.m_nCols || !a.m_nRows) { return a; }

		CType* adata = a.m_mData.Data();
		uint size    = a.m_nRows * a.m_nCols;

		for (uint i = 0; i < size; i++)
		{
			adata[i] -= b;
		}

		return a;
	}

// Add Rows to Matrix
// no rows of matrix a starting from src are inserted into this matrix before row dest.

	template<class CType>
	bool Matrix<CType>::AddRows(uint dest, uint src, const Matrix<CType>& a, uint no)
	{
		if (!m_nRows || !m_nCols || !dest || !src || (src + no - 1 > a.m_nRows)
		    || (m_nCols != a.m_nCols))
		{
			return false;
		}

		uint m;

		if (dest > m_nRows)
		{
			dest = m_nRows + 1;
			m    = 0;
		}
		else
		{
			m = m_nRows - dest + 1;
		}

		if (!Resize(m_nRows + no, m_nCols))
		{
			return false;
		}

		CType* data  = m_mData.Data();
		CType* adata = a.m_mData.Data();

		if (m)
		{
			m_mData.Move((dest + no - 1) * m_nCols, (dest - 1) * m_nCols, m * m_nCols);
		}

		memcpy(&data[(dest - 1) * m_nCols], &adata[(src - 1) * a.m_nCols], no * m_nCols * sizeof(CType));

		return true;
	}

// Add Columns to Matrix
	template<class CType>
	bool Matrix<CType>::AddCols(uint dest, uint src, const Matrix<CType>& a, uint no)
	{
		if (!m_nRows || !m_nCols || !dest || !src || (src + no - 1 > a.m_nCols)
		    || (m_nRows != a.m_nRows))
		{
			return false;
		}

		uint m;

		if (dest > m_nCols)
		{
			dest = m_nCols + 1;
			m    = 0;
		}
		else
		{
			m = m_nCols - dest + 1;
		}

		if (!Resize(m_nRows, m_nCols + no))
		{
			return false;
		}

		CType* data  = m_mData.Data();
		CType* adata = a.m_mData.Data();
		uint rp;

		for (uint r = 0; r < m_nRows; r++)
		{
			rp = r * m_nCols;
			if (m)
			{
				m_mData.Move(rp + dest + no - 1, rp + dest - 1, m);
			}

			for (uint j = 0; j < no; j++)
			{
				data[rp + dest - 1 + j] = adata[r * a.m_nCols + src - 1 + j];
			}
		}

		return true;
	}

// Sub Rows from Matrix
	template<class CType>
	Matrix<CType> Matrix<CType>::SubRows(uint row, uint no)
	{
		Matrix<CType> a(no, m_nCols);

		if (!m_nRows || !m_nCols || (row + no - 1 > m_nRows)) { return a; }

		CType* data  = m_mData.Data();
		CType* adata = a.m_mData.Data();
		uint m;

		if (row >= m_nRows)
		{
			row = m_nRows;
			m   = 0;
		}
		else
		{
			m = m_nRows - row - no + 1;
		}

		memcpy(adata, &data[(row - 1) * m_nCols], no * m_nCols * sizeof(CType));

		if (m)
		{
			m_mData.Move((row - 1) * m_nCols, (row - 1 + no) * m_nCols, m * m_nCols);
		}

		Resize(m_nRows - no, m_nCols);

		return a;
	}

// Sub Columns from Matrix
	template<class CType>
	Matrix<CType> Matrix<CType>::SubCols(uint col, uint no)
	{
		Matrix<CType> a(m_nRows, no);

		if (!m_nRows || !m_nCols || (col + no - 1 > m_nCols))
		{
			return a;
		}

		CType* data  = m_mData.Data();
		CType* adata = a.m_mData.Data();
		uint r, rp, m;

		if (col >= m_nCols)
		{
			col = m_nCols;
			m   = 0;
		}
		else
		{
			m = m_nCols - col - no + 1;
		}

		for (r = 0; r < m_nRows; r++)
		{
			rp = r * m_nCols;
			memcpy(&adata[r * no], &data[rp + col - 1], no * sizeof(CType));

			if (m)
			{
				m_mData.Move(rp + col - 1, rp + col + no - 1, m);
			}
		}

		Resize(m_nRows, m_nCols - no);

		return a;
	}

// Find Column Width
	template<class CType>
	void Matrix<CType>::FindColWidth()
	{
		colwidth = 0;

		if (!m_nRows || !m_nCols) { return; }

		CStrMem hstr;
		CType* data = m_mData.Data();
		uint size   = m_nRows * m_nCols;
		uint i, w;

		for (i = 0; i < size; i++)
		{
			if ((w = uint((hstr = MakeStr(data[i])).Len())) > colwidth)
			{
				colwidth = w;
			}
		}

		colwidth++;
	}

// String Output
	template<class CType>
	char* Matrix<CType>::Str(uint row)
	{
		if (!m_nRows || !m_nCols || !row || (row > m_nRows))
		{
			return str = "Error";
		}

		CStrMem hstr2;

		CType* data = m_mData.Data();
		uint rp     = (row - 1) * m_nCols;
		int sp;

		str = "|";

		for (uint c = 0; c < m_nCols; c++)
		{
			hstr2 = MakeStr(data[rp + c]);
			sp    = colwidth - uint(hstr2.Len());
			if (sp <= 0) { sp = 1; }
			str = (str | sp) + hstr2;
		}

		str += "|";

		return str;
	}

// Make diagonal matrix out of row vector
	template<class CType>
	void Matrix<CType>::Diagonal()
	{
		if (!m_nRows || !m_nCols || ((m_nRows > 1) && (m_nCols > 1)))
		{
			return;
		}

		if (m_nCols == 1)
		{
			m_nCols = m_nRows;
			m_nRows = 1;
		}

		Resize(m_nCols, m_nCols);

		CType* data = Data();
		uint i, j;

		for (i = 1, j = m_nCols + 1; i < m_nCols; i++, j += m_nCols + 1)
		{
			data[j] = data[i];
			data[i] = CType(0);
		}
	}

// Make Row vector from Trace of square matrix
	template<class CType>
	void Matrix<CType>::Trace()
	{
		if (!m_nRows || !m_nCols || (m_nRows != m_nCols))
		{
			return;
		}

		CType* data = Data();
		uint i, j;

		for (i = 1, j = m_nCols + 1; i < m_nCols; i++, j += m_nCols + 1)
		{
			data[i] = data[j];
		}

		Resize(1, m_nCols);
	}

// Jacobi Decomposition to find Eigenvalues and Eigenvectors
	template<class CType>
	uint Matrix<CType>::Jacobi(Matrix<CType>& eval, Matrix<CType>& evec)
	{
		uint rots = 0;
		uint j, iq, ip, i, ipr, iqr, jrp;
		CType tresh, theta, tau, t, sm, s, h, g, c;
		CType* b, * z;

		if (!m_nRows || !m_nCols || (m_nRows != m_nCols)) { return 0; }

		if (!(b = new CType[m_nRows])) { return 0; }

		if (!(z = new CType[m_nRows]))
		{
			delete[] b;
			return 0;
		}

		Matrix<CType> a(this[0]);

		eval.Resize(1, m_nCols);
		evec.Resize(m_nRows, m_nCols);

		CType* deval = eval.Data();
		CType* devec = evec.Data();
		CType* data  = a.Data();

		evec.Identity();

		for (ip = 0; ip < m_nCols; ip++)
		{
			deval[ip] = b[ip] = data[ip * m_nCols + ip];
			z[ip]     = CType(0);
		}

		for (i = 0; i < 50; i++)
		{
			sm = CType(0);
			for (ip = 0; ip < m_nRows; ip++)
			{
				ipr = ip * m_nCols;
				for (iq = 0; iq < m_nRows; iq++)
				{
					sm += Mag(data[ipr + iq]);
				}
			}

			if (sm == CType(0))
			{
				break;		// Exit
			}
			if (i < 3) { tresh = CType(0.2) * sm / CType(m_nRows * m_nRows); }
			else{ tresh = CType(0); }

			for (ip = 0; ip < m_nRows - 1; ip++)
			{
				ipr = ip * m_nCols;
				for (iq = ip + 1; iq < m_nRows; iq++)
				{
					iqr = iq * m_nCols;

					g = CType(100) * Mag(data[ipr + iq]);
					if ((i > 3) && (Mag(deval[ip]) + g == Mag(deval[ip])) &&
					    (Mag(deval[iq]) + g == Mag(deval[iq])))
					{
						data[ipr + iq] = CType(0);
					}
					else if (Mag(data[ipr + iq]) > tresh)
					{
						h = deval[iq] - deval[ip];
						if (Mag(h) + g == Mag(h)) { t = data[ipr + iq] / h; }
						else
						{
							theta = CType(0.5) * h / data[ipr + iq];
							t     = CType(1) / (Mag(theta) + CType(sqrt(double(CType(1) + theta * theta))));
							if (theta < CType(0)) { t = -t; }
						}

						c              = CType(1) / CType(sqrt(double(1 + t * t)));
						s              = t * c;
						tau            = s / (CType(1) + c);
						h              = t * data[ipr + iq];
						z[ip]         -= h;
						z[iq]         += h;
						deval[ip]     -= h;
						deval[iq]     += h;
						data[ipr + iq] = CType(0);

						for (j = 0; j < ip; j++)
						{
							jrp            = j * m_nCols;
							g              = data[jrp + ip];
							h              = data[jrp + iq];
							data[jrp + ip] = g - s * (h + g * tau);
							data[jrp + iq] = h + s * (g - h * tau);
						}

						for (j = ip + 1; j < iq; j++)
						{
							jrp            = j * m_nCols;
							g              = data[ipr + j];
							h              = data[jrp + iq];
							data[ipr + j]  = g - s * (h + g * tau);
							data[jrp + iq] = h + s * (g - h * tau);
						}

						for (j = iq + 1; j < m_nRows; j++)
						{
							g             = data[ipr + j];
							h             = data[iqr + j];
							data[ipr + j] = g - s * (h + g * tau);
							data[iqr + j] = h + s * (g - h * tau);
						}

						for (j = 0; j < m_nRows; j++)
						{
							jrp             = j * m_nCols;
							g               = devec[jrp + ip];
							h               = devec[jrp + iq];
							devec[jrp + ip] = g - s * (h + g * tau);
							devec[jrp + iq] = h + s * (g - h * tau);
						}

						rots++;
					}	// else if (Mag(data[ipr+iq]) > tresh)
				}	// for(iq=ip+1;iq<rows;iq++)
			}	// for(ip=0;ip<rows-1;ip++)

			for (ip = 0; ip < m_nRows; ip++)
			{
				deval[ip] = b[ip] += z[ip];
				z[ip]     = CType(0);
			}
		}	//  for(i=0;i<50;i++)

		delete[] b;
		delete[] z;

		return rots;
	}

// Order Eigenvalues and Eigenvectors
// Expects Eigenvalues in a row vector
	template<class CType>
	void EigenOrder(Matrix<CType>& eval, Matrix<CType>& evec)
	{
		uint k, j, i, n;
		CType p;

		n = eval.m_nCols;

		if (!n) { return; }

		CType* deval = eval.Data();

		for (i = 0; i < n - 1; i++)
		{
			k = i;
			p = deval[i];

			for (j = i + 1; j < n; j++)
			{
				if (Mag(deval[j]) >= Mag(p))
				{
					k = j;
					p = deval[j];
				}
			}

			if (k != i)
			{
				deval[k] = deval[i];
				deval[i] = p;
				evec.SwapCols(i + 1, k + 1);
			}
		}
	}

// Calculate sqrt(a^2 + b^2) without destructive over- or underflow
	template<class CType>
	CType Matrix<CType>::pythag(CType a, CType b)
	{
		CType at, bt, h;

		at = Mag(a);
		bt = Mag(b);

		if (at > bt)
		{
			h = bt / at;
			return at * CType(sqrt(double(CType(1) + h * h)));
		}
		else if (bt == CType(0))
		{
			return CType(0);
		}
		else
		{
			h = at / bt;
			return bt * CType(sqrt(double(CType(1) + h * h)));
		}
	}

// Calculate Singular Value Decomposition
	template<class CType>
	int Matrix<CType>::SVD(Matrix<CType>& u, Matrix<CType>& w, Matrix<CType>& v)
	{
		uint urp, urpi, urpj;
		uint vrp;	//, vrpi, vrpj;
		uint cr, l, k, j, jj, its, i, rcmin, hi, hj;
		int retval = 0;
		CType x, y, z, scale, s, h, g, f, c, anorm;
		CType* rv1;

		if (!m_nCols || !m_nRows) { return 0; }

		if (!(rv1 = new CType[m_nCols])) { return 0; }

		u = this[0];
		v.Resize(m_nCols, m_nCols);
		w.Resize(1, m_nCols);

		CType* udata = u.Data();
		CType* vdata = v.Data();
		CType* wdata = w.Data();

		/* Use the following for debugging array overshoots
		Mem<CType> &udata = *u.mat;
		Mem<CType> &vdata = *v.mat;
		Mem<CType> &wdata = *w.mat;
		*/

		g     = CType(0);
		scale = CType(0);
		anorm = CType(0);

		// Householder reduction to bidiagonal form
		for (i = 0, l = 1; i < m_nCols; i++, l++)
		{
			urpi = i * m_nCols;

			rv1[i] = scale * g;
			g      = CType(0);
			s      = CType(0);
			scale  = CType(0);

			if (i < m_nRows)
			{
				for (k = i; k < m_nRows; k++)
				{
					scale += Mag(udata[k * m_nCols + i]);
				}

				if (scale != CType(0))
				{
					for (k = i; k < m_nRows; k++)
					{
						urp         = k * m_nCols + i;
						udata[urp] /= scale;
						s          += udata[urp] * udata[urp];
					}

					f               = udata[urpi + i];
					g               = -sign(CType(sqrt(double(s))), f);
					h               = f * g - s;
					udata[urpi + i] = f - g;

					for (j = l; j < m_nCols; j++)
					{
						s = CType(0);
						for (k = i; k < m_nRows; k++)
						{
							urp = k * m_nCols;
							s  += udata[urp + i] * udata[urp + j];
						}

						f = s / h;
						for (k = i; k < m_nRows; k++)
						{
							urp             = k * m_nCols;
							udata[urp + j] += f * udata[urp + i];
						}
					}	// for(j=l;j<cols;j++)

					for (k = i; k < m_nRows; k++)
					{
						udata[k * m_nCols + i] *= scale;
					}
				}	// if (scale != CType(0))
			}	// if (i < rows)

			wdata[i] = scale * g;
			g        = CType(0);
			s        = CType(0);
			scale    = CType(0);

			if ((i < m_nRows) && (i != m_nCols))
			{
				for (k = l; k < m_nCols; k++)
				{
					scale += Mag(udata[urpi + k]);
				}

				if (scale != CType(0))
				{
					for (k = l; k < m_nCols; k++)
					{
						urp         = urpi + k;
						udata[urp] /= scale;
						s          += udata[urp] * udata[urp];
					}

					f               = udata[urpi + l];
					g               = -sign(CType(sqrt(double(s))), f);
					h               = f * g - s;
					udata[urpi + l] = f - g;

					for (k = l; k < m_nCols; k++)
					{
						rv1[k] = udata[urpi + k] / h;
					}

					for (j = l; j < m_nRows; j++)
					{
						urpj = j * m_nCols;

						s = CType(0);
						for (k = l; k < m_nCols; k++)
						{
							s += udata[urpj + k] * udata[urpi + k];
						}

						for (k = l; k < m_nCols; k++)
						{
							udata[urpj + k] += s * rv1[k];
						}
					}	// for(j=l;j<rows;j++)

					for (k = l; k < m_nCols; k++)
					{
						udata[urpi + k] *= scale;
					}
				}	// if (scale != CType(0))
			}	// if (i < rows && i != cols)

			anorm = maxi(anorm, (Mag(wdata[i]) + Mag(rv1[i])));
		}	// for(i=0,l=1;i<cols;i++,l++)

		// Accumulation of right-hand transformations

		for (i = m_nCols - 1, l = m_nCols; l > 0; i--, l--)
		{
			urpi = i * m_nCols;

			if (i < m_nCols - 1)
			{
				if (g != CType(0))
				{
					for (j = l; j < m_nCols; j++)
					{
						vdata[j * m_nCols + i] = (udata[urpi + j] / udata[urpi + l]) / g;
					}

					for (j = l; j < m_nCols; j++)
					{
						s = CType(0);
						for (k = l; k < m_nCols; k++)
						{
							s += udata[urpi + k] * vdata[k * m_nCols + j];
						}

						for (k = l; k < m_nCols; k++)
						{
							vrp             = k * m_nCols;
							vdata[vrp + j] += s * vdata[vrp + i];
						}
					}	// for(j=l;j<cols;j++)
				}	// if (g != CType(0))

				for (j = l; j < m_nCols; j++)
				{
					vdata[urpi + j]        = CType(0);
					vdata[j * m_nCols + i] = CType(0);
				}
			}	// if (i < cols-1)

			vdata[urpi + i] = CType(1);
			g               = rv1[i];
		}	// for(i=cols-1,l=cols;l>0;i--,l--)

		// Accumulation of left-hand transformations

		if (m_nRows < m_nCols)
		{
			rcmin = m_nRows;
		}
		else
		{
			rcmin = m_nCols;
		}

		for (i = rcmin - 1, l = rcmin; l > 0; i--, l--)
		{
			urpi = i * m_nCols;

			g = wdata[i];

			for (j = l; j < m_nCols; j++)
			{
				udata[urpi + j] = CType(0);
			}

			if (g != CType(0))
			{
				g = CType(1) / g;

				for (j = l; j < m_nCols; j++)
				{
					s = CType(0);
					for (k = l; k < m_nRows; k++)
					{
						urp = k * m_nCols;
						s  += udata[urp + i] * udata[urp + j];
					}

					f = (s / udata[urpi + i]) * g;

					for (k = i; k < m_nRows; k++)
					{
						urp             = k * m_nCols;
						udata[urp + j] += f * udata[urp + i];
					}
				}	// for(j=l;j<cols;j++)

				for (j = i; j < m_nRows; j++)
				{
					udata[j * m_nCols + i] *= g;
				}
			}	// if (g != CType(0))
			else
			{
				for (j = i; j < m_nRows; j++)
				{
					udata[j * m_nCols + i] = CType(0);
				}
			}
			udata[urpi + i] += CType(1);
		}	// for(i=rcmin-1,l=rcmin;l>0;i--,l--)

		// Diagonalization of the bidiagonal form

		int status = 0;

		for (k = m_nCols - 1, hi = m_nCols; hi > 0; k--, hi--)	// Loop over singular values
		{
			for (its = 0; its < _MAXSVDITS_; its++)	// Loop over allowed iteration
			{
				status = 0;

				for (l = k, cr = k - 1, hj = k + 1; hj > 0; l--, cr--, hj--)	// Test for splitting
				{
					if ((Mag(rv1[l]) + anorm) == anorm)
					{
						status = 1;
						break;
					}
					if (cr >= 0)
					{
						if ((Mag(wdata[cr]) + anorm) == anorm)
						{
							break;
						}
					}
				}	// Test for splitting

				if (!status)
				{
					c = CType(0);
					s = CType(1);

					for (i = l; i <= k; i++)
					{
						f       = s * rv1[i];
						rv1[i] *= c;

						if ((Mag(f) + anorm) == anorm)
						{
							break;
						}

						g        = wdata[i];
						h        = pythag(f, g);
						wdata[i] = h;
						h        = CType(1) / h;
						c        = g * h;
						s        = -(f * h);

						for (j = 0; j < m_nRows; j++)
						{
							urpj = j * m_nCols;

							y                = udata[urpj + cr];
							z                = udata[urpj + i];
							udata[urpj + cr] = (y * c) + (z * s);
							udata[urpj + i]  = -(y * s) + (z * c);
						}	// for(j=0;j<rows;j++)
					}	// for(i=l;i<=k;i++)
				}	// if (!status)

				z = wdata[k];
				if (l == k)			// Convergence
				{
					if (z < CType(0))
					{
						wdata[k] = -z;
						for (j = 0; j < m_nCols; j++)
						{
							vrp        = j * m_nCols + k;
							vdata[vrp] = -vdata[vrp];
						}
					}	// if (z < CType(0))
					retval = 1;
					break;				// Exit
				}	// if (l == k)

				if (its == _MAXSVDITS_)
				{
					retval = -1;
				}

				x  = wdata[l];
				cr = k - 1;
				y  = wdata[cr];
				g  = rv1[cr];
				h  = rv1[k];

				f = ((y - z) * (y + z) + (g - h) * (g + h)) / (CType(2) * h * y);
				g = pythag(f, CType(1));
				f = ((x - z) * (x + z) + h * ((y / (f + sign(g, f))) - h)) / x;

				// Next QR transformation

				c = CType(1);
				s = CType(1);
				for (j = l, i = l + 1; j <= cr; j++, i++)
				{
					g      = rv1[i];
					y      = wdata[i];
					h      = s * g;
					g     *= c;
					z      = pythag(f, h);
					rv1[j] = z;

					c  = f / z;
					s  = h / z;
					f  = (x * c) + (g * s);
					g  = -(x * s) + (g * c);
					h  = y * s;
					y *= c;

					for (jj = 0; jj < m_nCols; jj++)
					{
						vrp            = jj * m_nCols;
						x              = vdata[vrp + j];
						z              = vdata[vrp + i];
						vdata[vrp + j] = (x * c) + (z * s);
						vdata[vrp + i] = -(x * s) + (z * c);
					}

					z        = pythag(f, h);
					wdata[j] = z;

					if (z != CType(0))
					{
						z = CType(1) / z;
						c = f * z;
						s = h * z;
					}

					f = (c * g) + (s * y);
					x = -(s * g) + (c * y);

					for (jj = 0; jj < m_nRows; jj++)
					{
						urp            = jj * m_nCols;
						y              = udata[urp + j];
						z              = udata[urp + i];
						udata[urp + j] = (y * c) + (z * s);
						udata[urp + i] = -(y * s) + (z * c);
					}
				}	// for(j=l,i=l+1;j<cr;j++,i++)

				rv1[l]   = CType(0);
				rv1[k]   = f;
				wdata[k] = x;
			}	// for(its=0;its<_MAXSVDITS_;its++)
		}	// for(k=cols-1;k>=0;k--)     // Loop over singular values

		delete[] rv1;

		return retval;
	}

// Order w in decending or ascending order and adjust COLS of v and u.
	template<class CType>
	void SVDOrder(Matrix<CType>& w, Matrix<CType>& u, Matrix<CType>& v, bool bDescending)
	{
		uint k, j, i, n;
		CType p;

		n = w.m_nCols;

		if (!n) { return; }

		CType* dw = w.Data();

		for (i = 0; i < n - 1; i++)
		{
			k = i;
			p = dw[i];
			for (j = i + 1; j < n; j++)
			{
				if (((Mag(dw[j]) >= Mag(p)) && bDescending) ||
				    ((Mag(dw[j]) <= Mag(p)) && !bDescending))
				{
					k = j;
					p = dw[j];
				}
			}

			if (k != i)
			{
				dw[k] = dw[i];
				dw[i] = p;
				v.SwapCols(i + 1, k + 1);
				u.SwapCols(i + 1, k + 1);
			}
		}
	}

// Inverse of this matrix using SVD
	template<class CType>
	Matrix<CType> Matrix<CType>::InvSVD(CType fPrec)
	{
		Matrix<CType> Inv(m_nRows, m_nCols);

		if (!m_nRows || !m_nCols) { return Inv; }

		Matrix<CType> u, v, w;

		if (SVD(u, w, v) <= 0) { return Inv; }

		w.TinyToZero(fPrec);
		w.InvComps();
		w.Diagonal();

		Inv            = v * w * (~u);
		Inv.m_eInvType = m_eInvType;

		return Inv;
	}

// Invert Components of matrix. Zero values are set to inf.
	template<class CType>
	bool Matrix<CType>::InvComps(CType inf, CType fPrec)
	{
		if (!m_nCols || !m_nRows)
		{
			return true;
		}

		CType fTiny;
		if (fPrec == CType(0))
		{
			fPrec = ::Tiny(fTiny);
		}

		CType* data = Data();
		uint size   = m_nRows * m_nCols;

		for (uint i = 0; i < size; i++)
		{
			if (!::IsZero(data[i], fPrec))
			{
				data[i] = CType(1) / data[i];
			}
			else
			{
				data[i] = inf;
			}
		}

		return true;
	}

/// Extract Submatrix from this matrix
	template<class CType>
	bool Matrix<CType>::GetSubMatrix(Matrix<CType>& rMat,            uint iTRow, uint iLCol, uint iRowNo, uint iColNo)
	{
		if (!m_nRows || !m_nCols ||
		    (iLCol < 1) || (iLCol > int(m_nCols)) ||
		    (iTRow < 1) || (iTRow > int(m_nRows)) ||
		    (iColNo <= 0) || (iRowNo <= 0) ||
		    (iLCol + iColNo - 1 > int(m_nCols)) ||
		    (iTRow + iRowNo - 1 > int(m_nRows)))
		{
			return false;
		}

		rMat.Resize(iRowNo, iColNo);
		CType* pfMatData = rMat.Data();

		if (!pfMatData)
		{
			return false;
		}

		CType* pfData = Data();

		uint iRow, iCol, iMatPos, iRowPos;

		for (iRow = 0, iMatPos = 0; iRow < iRowNo; iRow++)
		{
			//TODO existiert ein fall s.d. iRowPos < 0 ?
			iRowPos = (iTRow + iRow - 1) * m_nCols + iLCol - 1;
			for (iCol = 0; iCol < iColNo; iCol++, iMatPos++, iRowPos++)
			{
				pfMatData[iMatPos] = pfData[iRowPos];
			}
		}

		return true;
	}

////////////////////////////////////////////////////////////////////////
// Evaluate Eigenvalues of arbitrary matrix.
// Returns row vectors of real and imaginary part of eigenvalues.

	template<class CType>
	bool EigenValues(Matrix<CType>& xMat, Matrix<CType>& xREVals, Matrix<CType>& xIEVals)
	{
		double** a, * wr, * wi;
		int* inter;
		int i, j, n, ierr;

		n = int(xMat.Rows());
		if (n != int(xMat.Cols()))
		{
			return false;
		}

		a = (double**) calloc(n, sizeof(double*));
		for (i = 0; i < n; i++)
		{
			a[i] = (double*) calloc(n, sizeof(double));
		}

		wr    = (double*) calloc(n, sizeof(double));
		wi    = (double*) calloc(n, sizeof(double));
		inter = (int*) calloc(n, sizeof(int));

		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
			{
				a[i][j] = xMat(i + 1, j + 1);
			}
		}

		elmhes(a, n, 0, n - 1, inter);
		hqr(n, 0, n - 1, a, wr, wi, &ierr);

		xREVals.Resize(1, n);
		xIEVals.Resize(1, n);

		for (i = 0; i < n; i++)
		{
			xREVals(1, i + 1) = CType(wr[i]);
			xIEVals(1, i + 1) = CType(wi[i]);
		}

		for (i = 0; i < n; i++)
		{
			free(a[i]);
		}

		free(a);
		free(inter);
		free(wr);
		free(wi);

		return true;
	}

////////////////////////////////////////////////////////////////////////
// Evaluate Eigenvalues and Eigenvectors of arbitrary matrix.
// Returns row vectors of real and imaginary part of eigenvalues and
// matrices of real and imaginary part of eigenvectors.

	template<class CType>
	bool Eigen(Matrix<CType>& xMat, Matrix<CType>& xREVals, Matrix<CType>& xIEVals,	Matrix<CType>& xREVecs, Matrix<CType>& xIEVecs)
	{
		double** a, ** v, * wr, * wi, ** z, * scale;
		double dSum, dVal;
		int* inter;
		int i, j, n, ierr, low, high;

		n = int(xMat.Rows());
		if (n != int(xMat.Cols()))
		{
			return false;
		}

		a = (double**) calloc(n, sizeof(double*));
		v = (double**) calloc(n, sizeof(double*));
		z = (double**) calloc(n, sizeof(double*));
		for (i = 0; i < n; i++)
		{
			a[i] = (double*) calloc(n, sizeof(double));
			v[i] = (double*) calloc(n, sizeof(double));
			z[i] = (double*) calloc(2 * n, sizeof(double));
		}

		wr    = (double*) calloc(n, sizeof(double));
		wi    = (double*) calloc(n, sizeof(double));
		scale = (double*) calloc(n, sizeof(double));
		inter = (int*) calloc(n, sizeof(int));

		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
			{
				a[i][j] = xMat(i + 1, j + 1);
			}
		}

		balanc(a, n, &low, &high, scale);

		elmhes(a, n, low, high, inter);
		eltran(n, low, high, a, inter, z);
		hqr2(n, low, high, a, wr, wi, z, &ierr);
		//pnormwr(n, z, wr, wi); // does not normalize

		balbak(n, low, high, scale, n, z);

		xREVals.Resize(1, n);
		xIEVals.Resize(1, n);
		xREVecs.Resize(n, n);
		xIEVecs.Resize(n, n);

		for (i = 0; i < n; i++)
		{
			xREVals(1, i + 1) = CType(wr[i]);
			xIEVals(1, i + 1) = CType(wi[i]);

			if (wi[i] == 0.0)
			{
				dSum = 0;
				for (j = 0; j < n; j++)
				{
					dVal  = z[j][i];
					dSum += dVal * dVal;
				}

				dVal = sqrt(dSum);

				if (dVal != 0.0)
				{
					for (j = 0; j < n; j++)
					{
						xREVecs(j + 1, i + 1) = CType(z[j][i] / dVal);
					}
				}
				else
				{
					for (j = 0; j < n; j++)
					{
						xREVecs(j + 1, i + 1) = CType(0);
					}
				}
			}
			else if (wi[i] > 0.0)
			{
				// complex eigenvector
				for (j = 0; j < n; j++)
				{
					xREVecs(j + 1, i + 1) = CType(z[j][i]);
					xIEVecs(j + 1, i + 1) = CType(z[j][i + 1]);
				}
			}
			else	// wi[i] < 0.0
			{
				// conjugate complex eigenvector
				for (j = 0; j < n; j++)
				{
					xREVecs(j + 1, i + 1) = CType(z[j][i - 1]);
					xIEVecs(j + 1, i + 1) = -CType(z[j][i]);
				}
			}
		}

		for (i = 0; i < n; i++)
		{
			free(a[i]);
			free(v[i]);
			free(z[i]);
		}

		free(a);
		free(v);
		free(z);
		free(inter);
		free(wr);
		free(wi);
		free(scale);

		return true;
	}

#endif
