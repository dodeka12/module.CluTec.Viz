////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CLUCodeBase.cpp
//
// summary:   Implements the clu code base class
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

// CLUCodeBase.cpp: Implementierung der Klasse CCLUCodeBase.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
//#include "CvFltk\glut.h"
#include "CluTec.Viz.Fltk\fl_ask.H"
#include "CluTec.Viz.Fltk\Fl_File_Chooser.H"
#include <time.h>
#include <sys/timeb.h>

#ifdef _GNUCPP3_
    #include <unistd.h>
	#define _getcwd getcwd
	#define _chdir chdir
#else
    #include <direct.h>
#endif

#ifdef WIN32
    #include <ShellApi.h>
#endif

#include "CLUCodeBase.h"

#include <float.h>

#include <vector>
#include <algorithm>
#include <functional>		// For greater<int>( )

#include "CluTec.Viz.Base\TensorOperators.h"

using namespace std;

CCLUCodeBase* CCLUCodeBase::sm_pCurCodeBase = 0;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCLUCodeBase::CCLUCodeBase()
{
	m_pFilter      = 0;
	m_pDrawBase    = 0;
	m_pFrameRotor  = 0;
	m_pCLUDrawBase = 0;
	m_pCLUParse    = 0;

	m_fPi        = TCVScalar(2.0 * asin(1.0));
	m_fRadPerDeg = m_fPi / 180.0f;

	m_iCurScript = -1;

	m_bOverrideSetRenderLatex = false;

	m_dCurrentInfoWidth = 0;

	m_eCurSpaceVars = SPACEVARS_NONE;

	time_t TimeVal;
	time(&TimeVal);
	m_Random.seed3((long) TimeVal);

	m_vecVersion.resize(3);
	m_vecVersion[0] = 0;
	m_vecVersion[1] = 0;
	m_vecVersion[2] = 0;

	//SFontData Font;

	//Font.sName = "Times";
	//Font.iFont = FL_TIMES;
	//Font.vecSize.push_back(12);
	//Font.vecSize.push_back(18);
	//Font.vecSize.push_back(24);
	//Font.vecSize.push_back(30);
	//Font.vecSize.push_back(36);
	//Font.vecSize.push_back(48);
	//Font.vecSize.push_back(96);
	//m_vecFont.push_back(Font);

	m_iActTempImageList = 0;
	m_mTempImageList.Set(2);

	sm_pCurCodeBase = this;
	m_pSortVarList  = 0;

	m_bNeedResourceHandleReset = false;

	Reset();
}

CCLUCodeBase::~CCLUCodeBase()
{
}

void CCLUCodeBase::Reset()
{
	m_mapBitmap.clear();
	//CCodeVar::m_ImgRep.Reset();

	m_iPlotMode = PLOT_FILL;

	// Delete all latex bitmaps
	m_oglLatex.ResetInUseFlags();
	m_oglLatex.PruneMap();

	// Reset Gauss Parameters
	m_Random.GaussMean(0);
	m_Random.GaussDev(1);

	// Delete Variables set for a space
	ResetSpaceVars();

	// Reset Variables for Info Window
	ResetInfoVars();

	// Reset environment variables
	ResetEnvVars();

	// Reset Serial IO Ports
	ResetSerialIO();

	// Reset Animation Time Step
	if (m_pCLUDrawBase)
	{
		m_pCLUDrawBase->SetAnimationTimeStep(20);
	}

	GETTIME(&m_tmStart);
}

void CCLUCodeBase::PostResetResourceHandles()
{
	m_bNeedResourceHandleReset = true;
}

void CCLUCodeBase::TryResetResourceHandles()
{
	if (m_bNeedResourceHandleReset)
	{
		m_bNeedResourceHandleReset = false;

		// TODO: Implement Trix reader
		//ResetTrixReaderMap();
	}
}

void CCLUCodeBase::ResetSerialIO()
{
	#ifdef WIN32
		m_mapSerialIO.clear();
	#endif
}

void CCLUCodeBase::ResetEnvVars()
{
	//m_iCurBitmapFont = 0; //FL_TIMES; //OGL_BMPFONT_Times_Std_12;
	//m_iCurBitmapFontSize = 0; //24;

	//// Current Bitmap Scale
	m_fCurBitmapScale = 1.0f;

	// Reset Latex Mag Step
	m_iLatexMagStep  = 12;
	m_sLatexMainFont = "Times New Roman";

	// Align Latex Bitmaps at bottom
	m_fVLatexAlign = 0.0f;
	m_fHLatexAlign = 0.0f;

	// Align Bitmaps at bottom
	m_fVBMPAlign = 0.0f;
	m_fHBMPAlign = 0.0f;

	// Position of Bitmaps
	m_xBMPPos.Set(0.0f, 0.0f, 0.0f);

	// Switch off transparency in Bitmap
	m_bUseBMPTrans = false;

	// Text precision
	m_iTextPrec = 3;

	// Evaluation precision
	m_fSensitivity = 1e-12;

	// Visualization precision
	if (m_pFilter)
	{
		m_pFilter->SetSensitivity(1e-12);
	}

	// Switch off Latex rendering.
	// The user can easily override this with shift+ctrl+p.
	SetRenderLatex(false);

	// Set Line Width
	//SetLineWidth(2.0f);

	// Disable smooth lines
	//SetSmoothLine(false);

	// Set Point Size
	//SetPointSize(5.0f);

	// Disable smooth points
	//SetSmoothPoint(false);

	// Disable smooth polygons
	//SetSmoothPolygon(false);

	// Reset Texture
	//glBindTexture(GL_TEXTURE_2D, 0);

	if (m_pDrawBase)
	{
		m_pDrawBase->EnableAbsTexCoords(false);
		m_pDrawBase->SetTextureRepeatFactor(0.0f);
	}

	// Reset Arrow Shape
	if (m_pDrawBase)
	{
		m_pDrawBase->SetArrowShape(0.15f, 15.0f / 180.0f * float(m_fPi), 0.01f);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCLUCodeBase::ResetInfoVars()
{
	m_dCurrentInfoWidth = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCLUCodeBase::ResetOutputObjectList()
{
	m_vecOutputObject.resize(0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCLUCodeBase::CollectGarbage()
{
	// Delete all latex bitmaps that were not used the last time round.
	m_oglLatex.PruneMap();

	// Mark all latex bitmaps as not being used currently.
	m_oglLatex.ResetInUseFlags();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCLUCodeBase::CleanUp()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCLUCodeBase::SetVersion(int iMajor, int iMinor, int iRevision)
{
	m_vecVersion[0] = iMajor;
	m_vecVersion[1] = iMinor;
	m_vecVersion[2] = iRevision;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCLUCodeBase::GetVersion(int& iMajor, int& iMinor, int& iRevision)
{
	iMajor    = m_vecVersion[0];
	iMinor    = m_vecVersion[1];
	iRevision = m_vecVersion[2];
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUCodeBase::IsInRange(TCVScalar& dVal, TCVScalar dMin, TCVScalar dMax, int iLine, int iPos)
{
	if ((dVal < dMin) && (dVal >= dMin - m_fSensitivity))
	{
		dVal = dMin;
	}
	else if (dVal < dMin)
	{
		m_ErrorList.OutOfRange(dVal, dMin, dMax, iLine, iPos);
		return false;
	}

	if ((dVal > dMax) && (dVal <= dMax + m_fSensitivity))
	{
		dVal = dMax;
	}
	else if (dVal > dMax)
	{
		m_ErrorList.OutOfRange(dVal, dMin, dMax, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Delete Variables specific to the vector spaces

void CCLUCodeBase::ResetSpaceVars()
{
	switch (m_eCurSpaceVars)
	{
	case SPACEVARS_E3:
		DeleteConstVar("id");
		DeleteConstVar("e1");
		DeleteConstVar("e2");
		DeleteConstVar("e3");
		DeleteConstVar("I");
		break;

	case SPACEVARS_P3:
		DeleteConstVar("id");
		DeleteConstVar("e1");
		DeleteConstVar("e2");
		DeleteConstVar("e3");
		DeleteConstVar("e4");
		DeleteConstVar("I");
		break;

	case SPACEVARS_N3:
		DeleteConstVar("id");
		DeleteConstVar("e1");
		DeleteConstVar("e2");
		DeleteConstVar("e3");
		DeleteConstVar("ep");
		DeleteConstVar("em");
		DeleteConstVar("e");
		DeleteConstVar("einf");
		DeleteConstVar("e0");
		DeleteConstVar("ni");
		DeleteConstVar("no");
		DeleteConstVar("n");
		DeleteConstVar("nb");
		DeleteConstVar("I");
		break;

	case SPACEVARS_C2:
		DeleteConstVar("id");
		DeleteConstVar("e1");
		DeleteConstVar("e2");
		DeleteConstVar("e3");
		DeleteConstVar("e4");
		DeleteConstVar("e5");
		DeleteConstVar("e6");
		DeleteConstVar("I");
		break;

	case SPACEVARS_NC2:
		DeleteConstVar("id");
		DeleteConstVar("e1");
		DeleteConstVar("e2");
		DeleteConstVar("e3");
		DeleteConstVar("e4");
		DeleteConstVar("e5");
		DeleteConstVar("e6");
		DeleteConstVar("e7");
		DeleteConstVar("e8");
		DeleteConstVar("I");
		break;

	case SPACEVARS_E8:
		DeleteConstVar("id");
		DeleteConstVar("e1");
		DeleteConstVar("e2");
		DeleteConstVar("e3");
		DeleteConstVar("e4");
		DeleteConstVar("e5");
		DeleteConstVar("e6");
		DeleteConstVar("e7");
		DeleteConstVar("e8");
		DeleteConstVar("I");
		break;
	}

	m_eCurSpaceVars = SPACEVARS_NONE;
}

//////////////////////////////////////////////////////////////////////
/// Cast To Float
bool CCLUCodeBase::CastToFloat(CCodeVar& rVar, float& rfVal)
{
	switch (rVar.BaseType())
	{
	case PDT_INT:
		rfVal = ((float) *rVar.GetIntPtr());
		break;

	case PDT_UINT:
		rfVal = ((float) *rVar.GetUIntPtr());
		break;

	case PDT_LONG:
		rfVal = (float) *rVar.GetLongPtr();
		break;

	case PDT_FLOAT:
		rfVal = (float) *rVar.GetFloatPtr();
		break;

	case PDT_DOUBLE:
		rfVal = (float) *rVar.GetDoublePtr();
		break;

	default:
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Cast To Double

bool CCLUCodeBase::CastToDouble(CCodeVar& rVar, double& rdVal)
{
	switch (rVar.BaseType())
	{
	case PDT_INT:
		rdVal = ((double) *rVar.GetIntPtr());
		break;

	case PDT_UINT:
		rdVal = ((double) *rVar.GetUIntPtr());
		break;

	case PDT_LONG:
		rdVal = ((double) *rVar.GetLongPtr());
		break;

	case PDT_FLOAT:
		rdVal = ((double) *rVar.GetFloatPtr());
		break;

	case PDT_DOUBLE:
		rdVal = ((double) *rVar.GetDoublePtr());
		break;

	default:
		return false;
	}

	return true;
}

bool CCLUCodeBase::SetPlotMode(int iMode)
{
	switch (iMode)
	{
	case PLOT_POINTS:
		m_iPlotMode = PLOT_POINTS;
		break;

	case PLOT_LINES:
		m_iPlotMode = PLOT_LINES;
		break;

	case PLOT_FILL:
		m_iPlotMode = PLOT_FILL;
		break;
	default:
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////
/// Execute User Defined Function

bool CCLUCodeBase::ExecUserFunc(CCodeVar& rResVar, TCodePtr pCodePtr, TVarList& rParList, int iCodeLine, int iCodePos)
{
	// function call

	// Create a local var list which is added to
	// the local var list stack.
	// GetVar searches first in this stack,
	// then in ConstVarList and then in VarList.
	CCodeVarList mLocalVarList;

	// Parameters to function can be accessed via
	// the variable _P.

	if (!mLocalVarList.New("_P", PDT_PTR_VARLIST))
	{
		m_ErrorList.Internal(iCodeLine, iCodePos);
		return false;
	}

	mLocalVarList["_P"] = &rParList;

	PushLocal(&mLocalVarList);
	SetCurrentNamespace(NS_LOCAL);

	// Dummy Var for popping LocalVarList pointer later.
	CCodeVarList* pDummy;

	// Execute code
	CCodeElementList* pCode = dynamic_cast<CCodeElementList*>(((CCodeElement*) pCodePtr));
	if (pCode)
	{
		int iTempVarCount, iCurTempVarCount;
		int iCL, iCount = pCode->Count();
		CCodeVar* pDVar;

		LockStack();

		for (iCL = 0; iCL < iCount; iCL++)
		{
			// Empty stack up to lock
			while (Pop(pDVar))
				;

			iTempVarCount = int(m_mTempVarList.Count());
			if (!(*pCode)[iCL]->Apply(this))
			{
				if ((m_ErrorList.Last().iLevel == CEL_INTERNAL) &&
				    (m_ErrorList.Last().iNo == CERR_BREAK))
				{
					m_ErrorList.Reset();
					break;
				}

				// Pop Local Var List
				PopLocal(pDummy);

				if (m_mLocalVarStack.Count() == 0)
				{
					SetCurrentNamespace(NS_GLOBAL);
				}

				// Clear Stack up to lock
				// Otherwise stack may return references to variables
				// which only exist locally within the function.
				while (Pop(pDVar))
					;

				return false;
			}

			// Do not delete temp. vars. created in last line of function,
			// since this may be return value.
			if (iCL + 1 < iCount)
			{
				iCurTempVarCount = int(m_mTempVarList.Count());
				DeleteTempVar(iTempVarCount, iCurTempVarCount - iTempVarCount);
				//m_mTempVarList.Del(iTempVarCount, iCurTempVarCount-iTempVarCount);
			}
		}

		pDVar = 0;

		// Allow only single return value
		while (Pop(pDVar))
			;

		// If there is a return value, then dereference it if its a pointer
		if (pDVar)
		{
			rResVar = pDVar->DereferenceVarPtr(true);
			rResVar.DereferencePtr();

			/*
			if (rResVar.Type() == PDT_VARLIST)
			{
			TVarList &rResList = *rResVar.GetVarListPtr();
			int i, iCount = rResList.Count();

			for (i = 0; i < iCount; i++)
			{

			((TVarList *) rResVar.Val())->DereferencePtr();
			}
			*/
		}

		UnlockStack();
	}

	// Pop Local Var List
	PopLocal(pDummy);
	if (m_mLocalVarStack.Count() == 0)
	{
		SetCurrentNamespace(NS_GLOBAL);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Render Latex
void CCLUCodeBase::SetRenderLatex(bool bVal)
{
	if (m_bOverrideSetRenderLatex)
	{
		return;
	}

	if (bVal)
	{
		m_oglLatex.EnableRenderLatex(true);
	}
	else
	{
		m_oglLatex.EnableRenderLatex(false);
	}
}

////////////////////////////////////////////////////////////////////////////////////////
///// Math

//////////////////////////////////////////////////////////////////////
/// Cast to Matrix

bool CCLUCodeBase::CastToMatrix(TMatrix& rMat, CCodeVar& rVar, int iLine, int iPos)
{
	if (rVar.BaseType() == PDT_VARLIST)
	{
		int i, j;
		int iRows = 0, iCols = 0;
		TCVScalar fVal;
		TVarList& rRowList = *rVar.GetVarListPtr();

		if (rRowList[0].BaseType() == PDT_VARLIST)
		{
			// Nested List defines Matrix
			iRows = int(rRowList.Count());

			for (i = 0; i < iRows; i++)
			{
				if (rRowList[i].BaseType() != PDT_VARLIST)
				{
					GetErrorList().InvalidType(rRowList[i], iLine, iPos);
					return false;
				}

				TVarList& rRow = *((TVarList*) rRowList[i].GetVarListPtr());

				if ((i > 0) && ((int) rRow.Count() != iCols))
				{
					GetErrorList().InconsistentRowLength(i + 1, iLine, iPos);
					return false;
				}
				else if (i == 0)
				{
					// Initialize Matrix Size
					iCols = int(rRow.Count());
					rMat.Resize(iRows, iCols);
				}

				// Walk along row
				for (j = 0; j < iCols; j++)
				{
					if (!rRow[j].CastToScalar(fVal, GetSensitivity()))
					{
						GetErrorList().InvalidType(rRow[j], iLine, iPos);
						return false;
					}

					rMat(i + 1, j + 1) = fVal;
				}	// for j
			}	// for i
		}
		else if (rRowList[0].CastToScalar(fVal, GetSensitivity()))
		{
			// Single List gives column vector
			iRows = int(rRowList.Count());
			rMat.Resize(iRows, 1);

			rMat(1, 1) = fVal;

			for (i = 1; i < iRows; i++)
			{
				if (!rRowList[i].CastToScalar(fVal, GetSensitivity()))
				{
					GetErrorList().InvalidType(rRowList[i], iLine, iPos);
					return false;
				}

				rMat(i + 1, 1) = fVal;
			}
		}
		else
		{
			GetErrorList().InvalidParType(rRowList[0], 1, iLine, iPos);
			return false;
		}
	}	// type == PDT_VARLIST
	else if (rVar.BaseType() == PDT_MULTIV)
	{
		TMultiV& vA = *rVar.GetMultiVPtr();

		if (vA.BasePtr()->BaseID() == ID_EL2GA)
		{
			// Embed conic vector as symmetric 3x3 matrix.
			rMat = GetEl2GABase().Mat(vA);
		}
		#ifdef USE_GA_CC2
			else if (vA.BasePtr()->BaseID() == ID_CC2GA)
			{
				// Embed conic vector as symmetric 3x3 matrix.
				rMat = m_CC2GABase.Mat(vA);
			}
		#endif
		else
		{
			GetErrorList().GeneralError("Multivector is not in a conic space.", iLine, iPos);
			return false;
		}
	}	// type == PDT_MULTIV
	else if ((rVar.BaseType() == PDT_TENSOR_IDX) ||
		 (rVar.BaseType() == PDT_TENSOR))
	{
		TTensor T, * pT;

		if (rVar.BaseType() == PDT_TENSOR_IDX)
		{
			TTensorIdx& rTIdx = *rVar.GetTensorIdxPtr();

			if (!MakeTensor(T, rTIdx))
			{
				GetErrorList().GeneralError("Error creating tensor from index.", iLine, iPos);
				return false;
			}

			pT = &T;
		}
		else
		{
			pT = rVar.GetTensorPtr();
		}

		TTensor& rT = *pT;

		if (rT.Valence() == 1)
		{
			rMat.Resize(rT.DimSize(0), 1);

			memcpy(rMat.Data(), rT.Data(), rT.Size() * sizeof(TCVScalar));
		}
		else if (rT.Valence() == 2)
		{
			rMat.Resize(rT.DimSize(0), rT.DimSize(1));

			memcpy(rMat.Data(), rT.Data(), rT.Size() * sizeof(TCVScalar));
		}
		else
		{
			GetErrorList().GeneralError("Tensor needs to be of valence 1 or 2.", iLine, iPos);
			return false;
		}
	}
	else
	{
		GetErrorList().InvalidParType(rVar, 1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Cast Multivector to E3

bool CCLUCodeBase::CastMVtoE3(const TMultiV& vA, TMultiV& vR)
{
	TMultiV vB;
	vR.SetStyle(m_E3GABase.GetMVStyle());
	int iID = (int) vA.GetBase().BaseID();

	if (iID == ID_E3GA)
	{
		vR = vA;
	}
	#ifdef USE_GA_E8
		else if (iID == ID_E8GA)
		{
			vR = m_E8GABase.Vec(vA[1], vA[2], vA[3], 0, 0, 0, 0, 0);
		}
	#endif
	else if (iID == ID_PGA)
	{
		vB    = PGA<TCVScalar>::Vec(vA);
		vR[1] = vB[1];
		vR[2] = vB[2];
		vR[3] = vB[3];
	}
	else if (iID == ID_CONFGA)
	{
		CMVInfo<float> MVInfo;

		// Only Analyze Multivector
		m_pFilter->DrawMV(vA, true);
		MVInfo = m_pFilter->GetMVInfo();

		if (MVInfo.m_eType == GA_POINT)
		{
			vR = m_E3GABase.Vec(double(MVInfo.m_mVex[0][0]),
					double(MVInfo.m_mVex[0][1]),
					double(MVInfo.m_mVex[0][2]));
		}
		else{ vR = 0; }
/*
                if ( !::IsZero(Scalar(vA * m_N3GABase.vE), m_pFilter->GetDoubleSens()) )
                {
                        vB = ConfGA<TCVScalar>::MakeEuclidVec(vA);
                        vR[1] = vB[1];
                        vR[2] = vB[2];
                        vR[3] = vB[3];
                }
                else vR = 0;
*/
	}
	else if (iID == ID_EL2GA)
	{
		CMVInfo<float> Info;

		// Analyze Multivector
		m_pFilter->DrawMV(vA, true);
		Info = m_pFilter->GetMVInfo();
		if (Info.m_eType == GA_POINT)
		{
			vR = m_E3GABase.Vec(TCVScalar(Info.m_mVex[0][0]),
					TCVScalar(Info.m_mVex[0][1]),
					TCVScalar(Info.m_mVex[0][2]));
		}
		else{ vR = 0; }
	}
	else{ return false; }

	return true;
}

//////////////////////////////////////////////////////////////////////
// Cast Multivector to P3

bool CCLUCodeBase::CastMVtoP3(const TMultiV& vA, TMultiV& vR)
{
	TMultiV vB;
	int iID = (int) vA.GetBase().BaseID();

	if (iID == ID_E3GA)
	{
		vR = PGA<TCVScalar>::hVec(vA[1], vA[2], vA[3], TCVScalar(1));
	}
	#ifdef USE_GA_E8
		else if (iID == ID_E8GA)
		{
			vR = PGA<TCVScalar>::hVec(vA[1], vA[2], vA[3], TCVScalar(1));
		}
	#endif
	else if (iID == ID_PGA)
	{
		vR = vA;
	}
	else if (iID == ID_CONFGA)
	{
		if (!::IsZero(Scalar(vA * m_N3GABase.vE), m_pFilter->GetDoubleSens()))
		{
			vB = ConfGA<TCVScalar>::MakeEuclidVec(vA);
			vR = PGA<TCVScalar>::hVec(vB[1], vB[2], vB[3], TCVScalar(1));
		}
		else
		{
			vB = ConfGA<TCVScalar>::MakeEuclidVec(vA);
			vR = PGA<TCVScalar>::hVec(vB[1], vB[2], vB[3], TCVScalar(0));
		}
	}
	else if (iID == ID_EL2GA)
	{
		CMVInfo<float> Info;

		// Analyze Multivector
		m_pFilter->DrawMV(vA, true);
		Info = m_pFilter->GetMVInfo();
		if (Info.m_eType == GA_POINT)
		{
			vR = m_P3GABase.hVec(TCVScalar(Info.m_mVex[0][0]),
					TCVScalar(Info.m_mVex[0][1]),
					TCVScalar(Info.m_mVex[0][2]),
					TCVScalar(1));
		}
		else
		{
			vR = m_P3GABase.vZero;
		}
	}
	else
	{
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Cast Multivector to El2

bool CCLUCodeBase::CastMVtoEl2(TMultiV& vA, TMultiV& vR)
{
	vR.SetStyle(m_El2GABase.GetMVStyle());
	int iID = (int) vA.GetBase().BaseID();

	if (iID == ID_E3GA)
	{
		vR = m_El2GABase.Vec(vA[1], vA[2]);
	}
	#ifdef USE_GA_E8
		else if (iID == ID_E8GA)
		{
			vR = m_El2GABase.Vec(vA[1], vA[2]);
		}
	#endif
	else if (iID == ID_PGA)
	{
		vA = PGA<TCVScalar>::Vec(vA);
		vR = m_El2GABase.Vec(vA[1], vA[2]);
	}
	else if (iID == ID_CONFGA)
	{
		if (!::IsZero(Scalar(vA * m_N3GABase.vE), m_pFilter->GetDoubleSens()))
		{
			vA = ConfGA<TCVScalar>::MakeEuclidVec(vA);
			vR = m_El2GABase.Vec(vA[1], vA[2]);
		}
		else{ vR = 0; }
	}
	else if (iID == ID_EL2GA)
	{
		vR = vA;
	}
	else{ return false; }

	return true;
}

//////////////////////////////////////////////////////////////////////
// Cast Multivector to E8
#ifdef USE_GA_E8

	bool CCLUCodeBase::CastMVtoE8(TMultiV& vA, TMultiV& vR)
	{
		vR.SetStyle(m_E8GABase.GetMVStyle());
		int iID = (int) vA.GetBase().BaseID();

		if (iID == ID_E3GA)
		{
			vR = m_E8GABase.Vec(vA[1], vA[2], vA[3], 0, 0, 0, 0, 0);
		}
		else if (iID == ID_PGA)
		{
			vA = PGA<TCVScalar>::Vec(vA);
			vR = m_E8GABase.Vec(vA[1], vA[2], vA[3], 0, 0, 0, 0, 0);
		}
		else if (iID == ID_CONFGA)
		{
			if (!::IsZero(Scalar(vA * m_N3GABase.vE), m_pFilter->GetDoubleSens()))
			{
				vA = ConfGA<TCVScalar>::MakeEuclidVec(vA);
				vR = m_E8GABase.Vec(vA[1], vA[2], vA[3], 0, 0, 0, 0, 0);
			}
			else{ vR = 0; }
		}
		else if (iID == ID_E8GA)
		{
			vR = vA;
		}
		else{ return false; }

		return true;
	}

#endif

//////////////////////////////////////////////////////////////////////
/// Static Version of List Sort Comparison

bool CCLUCodeBase::StaticListSortCompare(int iLIdx, int iRIdx)
{
	if (sm_pCurCodeBase)
	{
		return sm_pCurCodeBase->ListSortCompare(iLIdx, iRIdx);
	}

	return false;
}

//////////////////////////////////////////////////////////////////////
/// List Sort Comparison

bool CCLUCodeBase::ListSortCompare(int iLIdx, int iRIdx)
{
	if (!m_pSortVarList)
	{
		return false;
	}

	bool bRes       = false;
	CCodeVar& rLVar = (*m_pSortVarList)[iLIdx];
	CCodeVar& rRVar = (*m_pSortVarList)[iRIdx];

	if (m_pSortCode == 0)
	{
		bool bLisS, bRisS, bLisStr, bRisStr;
		bool bLisEl = false, bRisEl = false;
		TCVScalar dLVal, dRVal;
		char* pcLVal = nullptr, * pcRVal = nullptr;

		bLisS = rLVar.CastToScalar(dLVal);
		bRisS = rRVar.CastToScalar(dRVal);

		bLisStr = (rLVar.BaseType() == PDT_STRING);
		bRisStr = (rRVar.BaseType() == PDT_STRING);

		if (rLVar.BaseType() == PDT_VARLIST)
		{
			TVarList& rSubList = *rLVar.GetVarListPtr();
			if (rSubList(0).BaseType() == PDT_STRING)
			{
				bLisEl = true;
				pcLVal = rSubList(0).GetStringPtr()->Str();
			}
		}

		if (rRVar.BaseType() == PDT_VARLIST)
		{
			TVarList& rSubList = *rRVar.GetVarListPtr();
			if (rSubList(0).BaseType() == PDT_STRING)
			{
				bRisEl = true;
				pcRVal = rSubList(0).GetStringPtr()->Str();
			}
		}

		if (bLisS && bRisS)
		{
			if (m_bSortAscend)
			{
				bRes = (dLVal < dRVal);
			}
			else
			{
				bRes = (dLVal > dRVal);
			}
		}
		else if (bLisStr && bRisStr)
		{
			pcLVal = (*rLVar.GetStringPtr()).Str();
			pcRVal = (*rRVar.GetStringPtr()).Str();

			if (m_bSortAscend)
			{
				bRes = (_stricmp(pcLVal, pcRVal) < 0);
			}
			else
			{
				bRes = (_stricmp(pcLVal, pcRVal) > 0);
			}
		}
		else if (bLisEl && bRisEl)
		{
			if (m_bSortAscend)
			{
				bRes = (_stricmp(pcLVal, pcRVal) < 0);
			}
			else
			{
				bRes = (_stricmp(pcLVal, pcRVal) > 0);
			}
		}
		else if (bLisS && bRisStr)
		{
			bRes = (m_bSortAscend ? true : false);
		}
		else if (bRisS && bLisStr)
		{
			bRes = (m_bSortAscend ? false : true);
		}
		else if ((bLisS || bLisStr) && !(bRisS || bRisStr))
		{
			bRes = (m_bSortAscend ? true : false);
		}
		else if ((bRisS || bRisStr) && !(bLisS || bLisStr))
		{
			bRes = (m_bSortAscend ? false : true);
		}
	}
	else
	{
		TCVScalar dVal;
		CCodeVar Res;
		TVarList List;

		List.Set(2);
		List(0) = &rLVar;
		List(1) = &rRVar;

		if (!ExecUserFunc(Res, m_pSortCode, List, m_iSortLine, m_iSortPos))
		{
			throw SortError();
		}

		if (!Res.CastToScalar(dVal, m_fSensitivity))
		{
			m_ErrorList.GeneralError("Invalid return value from comparison function.",
					m_iSortLine, m_iSortPos);

			throw SortError();
		}

		if (dVal > 0)
		{
			bRes = true;
		}
		else
		{
			bRes = false;
		}
	}

	return bRes;
}


