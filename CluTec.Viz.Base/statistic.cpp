////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      statistic.cpp
//
// summary:   Implements the statistic class
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

// Class Library Definitions for Random number generators

#include <math.h>
#include <string.h>

#include "statistic.h"


////////////////////////////////////////////////////////////////////////////////////
/// Constructor

Statistic::Statistic()
{
	m_dMin = m_dMax = m_dMean = m_dVariance = 0;
	m_dMeanVar = 0;
}


////////////////////////////////////////////////////////////////////////////////////
/// Copy Constructor

Statistic::Statistic(const Statistic& S)
{
	*this = S;
}

////////////////////////////////////////////////////////////////////////////////////
/// Operator =

Statistic& Statistic::operator =(const Statistic& S)
{
	m_mData = S.m_mData;

	m_dMin = S.m_dMin;
	m_dMax = S.m_dMax;

	m_dMean = S.m_dMean;
	m_dVariance = S.m_dVariance;

	m_dMeanVar = S.m_dMeanVar;

	return *this;
}


////////////////////////////////////////////////////////////////////////////////////
/// Operator = Mem<double>

Statistic& Statistic::operator =(const Mem<double>& mVal)
{
	m_mData = mVal;

	m_dMin = m_dMax = m_dMean = m_dVariance = 0;
	m_dMeanVar = 0;

	return *this;
}


////////////////////////////////////////////////////////////////////////////////////
/// Operator << float

Statistic& Statistic::operator <<(const float& dVal)
{
	if (m_mData++) m_mData[uint(m_mData.Count())-1] = double(dVal);

	return *this;
}

////////////////////////////////////////////////////////////////////////////////////
/// Operator << double

Statistic& Statistic::operator <<(const double& dVal)
{
	if (m_mData++) m_mData[uint(m_mData.Count())-1] = dVal;

	return *this;
}


////////////////////////////////////////////////////////////////////////////////////
/// Operator << Mem<double>

Statistic& Statistic::operator <<(const Mem<double>& mVal)
{
	uint i = uint(m_mData.Count()), n = uint(mVal.Count());

	if (m_mData += n) 
	{
		memcpy(&m_mData[i], &mVal[0], n*sizeof(double));
	}

	return *this;
}


////////////////////////////////////////////////////////////////////////////////////
/// Analyze

bool Statistic::Analyze()
{
	EvalMean();
	EvalVariance();
	EvalMeanVar();

	return true;
}


////////////////////////////////////////////////////////////////////////////////////
/// Evaluate Mean

bool Statistic::EvalMean()
{
	uint i, n = uint(m_mData.Count());
	double dVal;

	if (!n) return false;

	m_dMin = m_dMax = m_mData[0];
	m_dMean = 0.0;
	for(i=0;i<n;i++)
	{
		m_dMean += (dVal = m_mData[i]);

		if (dVal < m_dMin) m_dMin = dVal;
		else if (dVal > m_dMax) m_dMax = dVal;
	}
	m_dMean /= double(n);

	return true;
}


////////////////////////////////////////////////////////////////////////////////////
/// Evaluate Variance

bool Statistic::EvalVariance()
{
	uint i, n = uint(m_mData.Count());

	if (!n) return false;

	m_dVariance = 0.0;
	for(i=0;i<n;i++)
	{
		m_dVariance += pow(m_mData[i] - m_dMean, 2.0);
	}
	m_dVariance /= double(n);

	return true;
}


////////////////////////////////////////////////////////////////////////////////////
/// Evaluate Variance of Mean

bool Statistic::EvalMeanVar()
{
	uint i, n = uint(m_mData.Count());

	if (!n) return false;

	m_dMeanVar = 0.0;
	for(i=0;i<n;i++)
	{
		m_dMeanVar += pow(m_mData[i] - m_dMean, 2.0);
	}
	m_dMeanVar = sqrt(m_dMeanVar / double(n-1));

	return true;
}


