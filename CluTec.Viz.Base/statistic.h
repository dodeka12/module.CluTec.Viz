////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      statistic.h
//
// summary:   Declares the statistic class
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

// Class Declaration for statistic operations

#ifndef _STATISTIC_H_
#define _STATISTIC_H_

#include "clu.h"

typedef unsigned int uint;

class Statistic
{
  
public:
	Statistic();
	Statistic(const Statistic& S);
	
	Statistic& operator= (const Statistic& S);
	Statistic& operator= (const Mem<double>& S);
	
	Statistic& operator<< (const float& dVal);
	Statistic& operator<< (const double& dVal);
	Statistic& operator<< (const Mem<double>& mVal);

	bool Reset() { return (m_mData.Set(0)); }
	bool SetSize(uint nSize) { return m_mData.Set(nSize); }

	double& operator[] (uint i) { return m_mData[i]; }
	size_t Count() { return m_mData.Count(); }
	
	double Min() { return m_dMin; }
	double Max() { return m_dMax; }
	
	double Mean() { return m_dMean; }
	double Variance() { return m_dVariance; }
	double MeanVar() { return m_dMeanVar; }
	
	// Analyze Data
	bool Analyze();
	
	bool EvalMean();
	bool EvalVariance();
	bool EvalMeanVar();

protected:
	Mem<double> m_mData;

	double m_dMean, m_dMin, m_dMax, m_dVariance;
	double m_dMeanVar;
};

#endif

