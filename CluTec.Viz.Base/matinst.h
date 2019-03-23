////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      matinst.h
//
// summary:   Declares the matinst class
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

#ifndef _MATINST_HH_
#define _MATINST_HH_

#define Instantiate_Matrix(TYPE)\
\
template class Matrix<TYPE>;\
\
template bool operator==(const Matrix<TYPE> &a, const Matrix<TYPE> &b);\
template bool operator!=(const Matrix<TYPE> &a, const Matrix<TYPE> &b);\
\
template Matrix<TYPE> operator~ (const Matrix<TYPE> &a);\
template Matrix<TYPE> operator! (const Matrix<TYPE> &a);\
template Matrix<TYPE> operator- (const Matrix<TYPE> &a);\
\
template Matrix<TYPE> operator+ (const Matrix<TYPE> &a, const Matrix<TYPE> &b);\
template Matrix<TYPE>& operator+= (Matrix<TYPE> &a, const Matrix<TYPE> &b);\
template Matrix<TYPE> operator+ (const Matrix<TYPE> &a, const TYPE &b);\
template Matrix<TYPE>& operator+= (Matrix<TYPE> &a, const TYPE &b);\
template Matrix<TYPE> operator- (const Matrix<TYPE> &a, const Matrix<TYPE> &b);\
template Matrix<TYPE>& operator-= (Matrix<TYPE> &a, const Matrix<TYPE> &b);\
template Matrix<TYPE> operator- (const Matrix<TYPE> &a, const TYPE &b);\
template Matrix<TYPE>& operator-= (Matrix<TYPE> &a, const TYPE &b);\
\
template Matrix<TYPE> operator* (const Matrix<TYPE> &a, const TYPE &b);\
template Matrix<TYPE> operator* (TYPE &b, const Matrix<TYPE> &a);\
template Matrix<TYPE> operator*(const Matrix<TYPE> &a, const Matrix<TYPE> &b);\
template Matrix<TYPE>& operator*= (Matrix<TYPE> &a, const Matrix<TYPE> &b);\
template TYPE Det(const Matrix<TYPE> &a);\
\
template void EigenOrder(Matrix<TYPE> &eval, Matrix<TYPE> &evec);\
\
template void PrintMatrix(Matrix<TYPE> &a, const char *title);\
\
template void SVDOrder(Matrix<TYPE> &w, Matrix<TYPE> &mU, Matrix<TYPE> &v, bool bDescending);\
\
template bool EigenValues(Matrix<TYPE> &xMat, Matrix<TYPE> &xREVals, Matrix<TYPE> &xIEVals);\
\
template bool Eigen(Matrix<TYPE> &xMat, Matrix<TYPE> &xREVals, Matrix<TYPE> &xIEVals, \
					   Matrix<TYPE> &xREVecs, Matrix<TYPE> &xIEVecs); \
\





#endif
