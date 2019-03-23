////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLMVFilterBase.cpp
//
// summary:   Implements the oglmv filter base class
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

//// IMPORTANT IMPORTANT IMPORTANT
/*
	Never write this:

		m_VexList.AddVex(m_VexList[0]);

	since internally the memory of m_VexList is enlarged, such that m_VexList[0]
	may not be a valid reference anymore!!!
*/




// OGLFilterBase.cpp: Implementierung der Klasse COGLFilterBase.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLDrawBase.h"
#include "OGLMVFilterBase.h"
//#include "OGLVexListE3.h"
//#include "OGLObjWireSphere.h"
#include "CluTec.Viz.Fltk\Fl_math.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLMVFilterBase::COGLMVFilterBase()
{
	m_pDrawBase = 0;

	m_fPi = 2.0f * (float) asin(1.0);

	m_fSensitivity = 1e-7f;
	
	ResetModes();
}

COGLMVFilterBase::~COGLMVFilterBase()
{
}


//////////////////////////////////////////////////////////////////////
/// Draw List of Blades

bool COGLMVFilterBase::DrawListOfMV(MemObj<MultiV<float> >& mvA)
{
	int i, n = int(mvA.Count());

	for(i=0;i<n;i++)
	{
		DrawMV(mvA[i]);
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Draw List of Blades

bool COGLMVFilterBase::DrawListOfMV(MemObj<MultiV<double> >& mvA)
{
	int i, n = int(mvA.Count());

	for(i=0;i<n;i++)
	{
		DrawMV(mvA[i]);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Cast MV Info from double to float

bool COGLMVFilterBase::CastMVInfoToFloat(CMVInfo<float>& fMVInfo, CMVInfo<double>& dMVInfo)
{
	fMVInfo.m_eType = dMVInfo.m_eType;

	Mem<CPointND<float, 3> >& mfVex = fMVInfo.m_mVex;
	Mem<CPointND<double, 3> >& mdVex = dMVInfo.m_mVex;

	int i, n = int(mdVex.Count());
	
	if (!mfVex.Set(n))
		return false;

	for(i=0;i<n;i++)
	{
		mfVex[i][0] = (float) mdVex[i][0];
		mfVex[i][1] = (float) mdVex[i][1];
		mfVex[i][2] = (float) mdVex[i][2];
	}

	Mem<float>& mfVal = fMVInfo.m_mVal;
	Mem<double>& mdVal = dMVInfo.m_mVal;

	n = int(mdVal.Count());

	if (!mfVal.Set(n))
		return false;

	for(i=0;i<n;i++)
	{
		mfVal[i] = (float) mdVal[i];
	}

	return true;
}

