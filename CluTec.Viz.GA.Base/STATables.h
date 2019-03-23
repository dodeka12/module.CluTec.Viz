////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.GA.Base
// file:      STATables.h
//
// summary:   Declares the sta tables class
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

template<class CType> short STA<CType>::piGrades[] = {
	0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4
};

template<class CType> short STA<CType>::piGPTable[] = {
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 2, 1,
	-6, -7, -8, -3, -4, -5, 13, 14, 15, -16, 9, 10, 11, -12, 3, 6,
	-1, -11, 10, -2, -15, 14, 12, -5, 4, -9, 16, -8, 7, -13, 4, 7,
	11, -1, -9, 15, -2, -13, 5, 12, -3, -10, 8, 16, -6, -14, 5, 8,
	-10, 9, -1, -14, 13, -2, -4, 3, 12, -11, -7, 6, 16, -15, 6, 3,
	2, 15, -14, 1, 11, -10, 16, -8, 7, 13, 12, -5, 4, 9, 7, 4, -15,
	2, 13, -11, 1, 9, 8, 16, -6, 14, 5, 12, -3, 10, 8, 5, 14, -13,
	2, 10, -9, 1, -7, 6, 16, 15, -4, 3, 12, 11, 9, 13, 12, -5, 4,
	16, -8, 7, -1, -11, 10, -3, -2, -15, 14, -6, 10, 14, 5, 12, -3,
	8, 16, -6, 11, -1, -9, -4, 15, -2, -13, -7, 11, 15, -4, 3, 12,
	-7, 6, 16, -10, 9, -1, -5, -14, 13, -2, -8, 12, 16, -9, -10,
	-11, -13, -14, -15, -3, -4, -5, 1, -6, -7, -8, 2, 13, 9, -16,
	8, -7, -12, 5, -4, -2, -15, 14, 6, -1, -11, 10, 3, 14, 10, -8,
	-16, 6, -5, -12, 3, 15, -2, -13, 7, 11, -1, -9, 4, 15, 11, 7,
	-6, -16, 4, -3, -12, -14, 13, -2, 8, -10, 9, -1, 5, 16, 12, 13,
	14, 15, 9, 10, 11, -6, -7, -8, -2, -3, -4, -5, -1
};

template<class CType> short STA<CType>::piIPTable[] = {
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
	-3, -4, -5, 0, 0, 0, 0, 9, 10, 11, -12, 0, 0, -1, 0, 0, -2, 0,
	0, 0, -5, 4, -9, 0, -8, 7, -13, 0, 0, 0, -1, 0, 0, -2, 0, 5,
	0, -3, -10, 8, 0, -6, -14, 0, 0, 0, 0, -1, 0, 0, -2, -4, 3, 0,
	-11, -7, 6, 0, -15, 0, 3, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, -5,
	4, 9, 0, 4, 0, 2, 0, 0, 1, 0, 0, 0, 0, 0, 5, 0, -3, 10, 0, 5,
	0, 0, 2, 0, 0, 1, 0, 0, 0, 0, -4, 3, 0, 11, 0, 0, 0, -5, 4, 0,
	0, 0, -1, 0, 0, -3, -2, 0, 0, -6, 0, 0, 5, 0, -3, 0, 0, 0, 0,
	-1, 0, -4, 0, -2, 0, -7, 0, 0, -4, 3, 0, 0, 0, 0, 0, 0, -1, -5,
	0, 0, -2, -8, 0, 0, -9, -10, -11, 0, 0, 0, -3, -4, -5, 1, 0,
	0, 0, 2, 0, 9, 0, 8, -7, 0, 5, -4, -2, 0, 0, 0, -1, 0, 0, 3,
	0, 10, -8, 0, 6, -5, 0, 3, 0, -2, 0, 0, 0, -1, 0, 4, 0, 11, 7,
	-6, 0, 4, -3, 0, 0, 0, -2, 0, 0, 0, -1, 5, 0, 12, 13, 14, 15,
	9, 10, 11, -6, -7, -8, -2, -3, -4, -5, -1
};

template<class CType> short STA<CType>::piOPTable[] = {
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 2, 0,
	-6, -7, -8, 0, 0, 0, 13, 14, 15, -16, 0, 0, 0, 0, 3, 6, 0, -11,
	10, 0, -15, 14, 12, 0, 0, 0, 16, 0, 0, 0, 4, 7, 11, 0, -9, 15,
	0, -13, 0, 12, 0, 0, 0, 16, 0, 0, 5, 8, -10, 9, 0, -14, 13, 0,
	0, 0, 12, 0, 0, 0, 16, 0, 6, 0, 0, 15, -14, 0, 0, 0, 16, 0, 0,
	0, 0, 0, 0, 0, 7, 0, -15, 0, 13, 0, 0, 0, 0, 16, 0, 0, 0, 0,
	0, 0, 8, 0, 14, -13, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 9,
	13, 12, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 14, 0, 12,
	0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 15, 0, 0, 12, 0, 0,
	16, 0, 0, 0, 0, 0, 0, 0, 0, 12, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 13, 0, -16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 14, 0, 0, -16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 0,
	0, 0, -16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

template<class CType> short STA<CType>::piVITable[] = {
	1, 1, -1, -1, -1, -1, -1, -1, 1, 1, 1, -1, 1, 1, 1, -1
};

template<class CType> short STA<CType>::piDualTable[] = {
	-16, 12, 13, 14, 15, -9, -10, -11, 6, 7, 8, -2, -3, -4, -5, 1
};