////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.GA.Base
// file:      E3GATables.h
//
// summary:   Declares the 3G the tables class
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

template<class CType> short E3GA<CType>::piGrades[] = {
	0, 1, 1, 1, 2, 2, 2, 3
};

template<class CType> short E3GA<CType>::piGPTable[] = {
	1, 2, 3, 4, 5, 6, 7, 8, 2, 1, 7, -6, 8, -4, 3, 5, 3, -7, 1, 5,
	4, 8, -2, 6, 4, 6, -5, 1, -3, 2, 8, 7, 5, 8, -4, 3, -1, -7, 6,
	-2, 6, 4, 8, -2, 7, -1, -5, -3, 7, -3, 2, 8, -6, 5, -1, -4, 8,
	5, 6, 7, -2, -3, -4, -1
};

template<class CType> short E3GA<CType>::piIPTable[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 1, 0, 0, 0, -4, 3, 5,
	0, 0, 1, 0, 4, 0, -2, 6,
	0, 0, 0, 1, -3, 2, 0, 7,
	0, 0, -4, 3, -1, 0, 0, -2,
	0, 4, 0, -2, 0, -1, 0, -3,
	0, -3, 2, 0, 0, 0, -1, -4,
	0, 5, 6, 7, -2, -3, -4, -1
};

template<class CType> short E3GA<CType>::piOPTable[] = {
	1, 2, 3, 4, 5, 6, 7, 8, 2, 0, 7, -6, 8, 0, 0, 0, 3, -7, 0, 5,
	0, 8, 0, 0, 4, 6, -5, 0, 0, 0, 8, 0, 5, 8, 0, 0, 0, 0, 0, 0,
	6, 0, 8, 0, 0, 0, 0, 0, 7, 0, 0, 8, 0, 0, 0, 0, 8, 0, 0, 0, 0,
	0, 0, 0
};

template<class CType> short E3GA<CType>::piVITable[] = {
	1, 1, 1, 1, 1, 1, 1, 1
};

template<class CType> short E3GA<CType>::piDualTable[] = {
	-8, -5, -6, -7, 2, 3, 4, 1
};
