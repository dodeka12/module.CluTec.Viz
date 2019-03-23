////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      tensinst.h
//
// summary:   Declares the tensinst class
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

// Instantiating Tensor Class

//template Tensor<TYPE>& operator, (Tensor<TYPE>& t, uint pos);
//template Tensor<TYPE>& operator, (Tensor<TYPE>& t, uint* idx);

#define Instantiate_Tensor(TYPE)\
\
template class Tensor<TYPE>;\
\
template Tensor<TYPE> operator- (const Tensor<TYPE>& t);\
\
template Tensor<TYPE> operator* (const Tensor<TYPE>& t1, const Tensor<TYPE>& t2);\
template Tensor<TYPE> operator*(const Tensor<TYPE>& t1, const TYPE a);\
template Tensor<TYPE> operator*(const TYPE a, const Tensor<TYPE>& t1);\
template Tensor<TYPE>& operator*=(Tensor<TYPE>& t1, const Tensor<TYPE>& t2);\
template Tensor<TYPE>& operator*=(Tensor<TYPE>& t1, const TYPE a);\
\
template Tensor<TYPE> operator& (const Tensor<TYPE>& t1, const Tensor<TYPE>& t2);\
template Tensor<TYPE>& operator&=(Tensor<TYPE>& t1, const Tensor<TYPE>& t2);\
\
template Tensor<TYPE> operator/ (const Tensor<TYPE>& t1, const Tensor<TYPE>& t2);\
template Tensor<TYPE> operator/(const Tensor<TYPE>& t1, const TYPE a);\
template Tensor<TYPE>& operator/=(Tensor<TYPE>& t1, const Tensor<TYPE>& t2);\
template Tensor<TYPE>& operator/=(Tensor<TYPE>& t1, const TYPE a);\
\
template Tensor<TYPE> operator% (const Tensor<TYPE>& t1, const Tensor<TYPE>& t2);\
template Tensor<TYPE>& operator%=(Tensor<TYPE>& t1, const Tensor<TYPE>& t2);\
\
template Tensor<TYPE> operator+ (const Tensor<TYPE>& t1, const Tensor<TYPE>& t2);\
template Tensor<TYPE>& operator+= (Tensor<TYPE>& t1, const Tensor<TYPE>& t2);\
template Tensor<TYPE> operator+(const Tensor<TYPE>& t1, const TYPE& a);\
template Tensor<TYPE>& operator+=(Tensor<TYPE>& t1, const TYPE& a);\
\
template Tensor<TYPE> operator- (const Tensor<TYPE>& t1, const Tensor<TYPE>& t2);\
template Tensor<TYPE>& operator-= (Tensor<TYPE>& t1, const Tensor<TYPE>& t2);\
template Tensor<TYPE> operator-(const Tensor<TYPE>& t1, const TYPE& a);\
template Tensor<TYPE>& operator-=(Tensor<TYPE>& t1, const TYPE& a);\
\
template int InitCLoops(Tensor<TYPE>& nt1, Tensor<TYPE>& nt2, uint apos[], Mem<Tensor<TYPE>::cloopVar>& loops, Tensor<TYPE>& nt0);\
\
template int InitMLoops(Tensor<TYPE>& nt1, Tensor<TYPE>& nt2, uint apos[], Mem<Tensor<TYPE>::cloopVar>& loops, Tensor<TYPE>& nt0);\
\



