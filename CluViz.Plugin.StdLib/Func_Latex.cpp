////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Latex.cpp
//
// summary:   Implements the function latex class
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

#include "stdafx.h"
//#include "CluTec.Viz.OpenGL\CLUImgRep.h"
#include "Func_Latex.h"

//////////////////////////////////////////////////////////////////////
/// The Latex String FUNCTION
/// Converts variables to latex strings

bool  LatexStringFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount == 1)
	{
		CStrMem csVal;

		rCB.CastToLatexString(mVars(0), csVal);
		rVar = csVal;
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Latex Mag Step Function

bool  SetLatexMagStepFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVCounter iVal;

	if (iVarCount == 1)
	{
		if (!mVars(0).CastToCounter(iVal))
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if ((iVal < 5) || (iVal > 100))
		{
			rCB.GetErrorList().InvalidParVal(mVars(0), 1, iLine, iPos);
			return false;
		}

		rCB.GetLatexMagStep() = iVal;
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Set Latex Main Font Function

bool  SetLatexMainFontFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount == 1)
	{
		if ( mVars(0).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().GeneralError("Expect first parameter to be the name of a font.", iLine, iPos);
			return false;
		}

		rCB.GetLatexMainFont() = mVars(0).GetStringPtr()->Str();
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Render Latex Function

bool  SetRenderLatexFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVCounter iVal;

	if (iVarCount == 1)
	{
		if (!mVars(0).CastToCounter(iVal))
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		rCB.SetRenderLatex((iVal ? true : false));
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Latex Function

bool  DrawLatexFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	int iVar;
	CStrMem csText, csFile;
	TVarList& mVars = *rPars.GetVarListPtr();

	COGLColor TextColor = rCB.GetOGLDrawBase()->GetColor();

	int iVarCount = int(mVars.Count());
	float fMagStep;
	TCVScalar dVal[3];

	if ((iVarCount == 4) || (iVarCount == 5))
	{
		for (iVar = 0; iVar < 3; iVar++)
		{
			if (!mVars(iVar).CastToScalar(dVal[iVar], rCB.GetSensitivity()))
			{
				rCB.GetErrorList().InvalidParType(mVars(iVar), iVar + 1, iLine, iPos);
				return false;
			}
		}

		rCB.GetBMPPos().Set(float(dVal[0]), float(dVal[1]), float(dVal[2]));

		CCodeVar& rVar = mVars(3);
		if (rVar.BaseType() == PDT_NOTYPE)
		{
			rCB.GetErrorList().InvalidType(rVar, iLine, iPos);
			return false;
		}
		csText = rVar.ValStr();

		if (iVarCount == 5)
		{
			CCodeVar& rVar = mVars(4);
			if (rVar.BaseType() != PDT_STRING)
			{
				rCB.GetErrorList().InvalidType(rVar, iLine, iPos);
				return false;
			}
			csFile = rVar.ValStr();
		}
	}
	// If a single multivector is given as position vector, cast it to E3
	// and use that position.
	else if ((iVarCount == 2) || (iVarCount == 3))
	{
		if (mVars(0).BaseType() == PDT_MULTIV)
		{
			TMultiV& vA = *mVars(0).GetMultiVPtr();
			TMultiV vB;
			if (!rCB.CastMVtoE3(vA, vB))
			{
				rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
				return false;
			}

			rCB.GetBMPPos().Set((float) vB[E3GA < TCVScalar > ::iE1],
					(float) vB[E3GA < TCVScalar > ::iE2],
					(float) vB[E3GA < TCVScalar > ::iE3]);
		}
		//else if (mVars(0).BaseType() == PDT_VARLIST)
		//{
		//	TVarList &rList = *mVars(0).GetVarListPtr();

		//	if (rList.Count() != 6)
		//	{
		//		rCB.GetErrorList().InvalidParVal(mVars(0), 1, iLine, iPos);
		//		return false;
		//	}

		//	int i;
		//	TCVScalar pdVal[6];
		//	for (i = 0; i < 6; i++)
		//	{
		//		if (!(rList[i].CastToScalar(pdVal[i], rCB.GetSensitivity())))
		//		{
		//			rCB.GetErrorList().InvalidParVal(mVars(0), 1, iLine, iPos);
		//			return false;
		//		}
		//	}

		//	xA.Set( float(pdVal[0]),
		//			float(pdVal[1] + 0.5*(pdVal[1]-pdVal[4])),
		//		    float(pdVal[2]) );
		//}
		else
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		CCodeVar& rVar = mVars(1);

		if (rVar.BaseType() == PDT_NOTYPE)
		{
			rCB.GetErrorList().InvalidType(rVar, iLine, iPos);
			return false;
		}
		csText << rVar.ValStr();

		if (iVarCount == 3)
		{
			CCodeVar& rVar = mVars(2);
			if (rVar.BaseType() != PDT_STRING)
			{
				rCB.GetErrorList().InvalidType(rVar, iLine, iPos);
				return false;
			}
			csFile = rVar.ValStr();
		}
	}
	else
	{
		int piPar[] = { 2, 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (csText.Len() == 0)
	{
		rCB.GetErrorList().GeneralError("Empty string cannot be rendered.", iLine, iPos);
		return false;
	}

	//fMagStep = rCB.GetOGLLatex().GetMagStepScale(rCB.GetLatexMagStep());
	fMagStep = float(rCB.GetLatexMagStep());
	if (!rCB.GetOGLLatex().Add(csText, fMagStep, rCB.GetLatexMainFont().c_str(), TextColor, *((CImageRepository*)::GetImageRepositoryPtr()),
			    ((csFile.Len() > 0) ? csFile.Str() : 0)))
	{
		string sError;
		sError  = "Error: Cannot render Latex text.\n\nLatex Errors:\n";
		sError += rCB.GetOGLLatex().GetRenderErrorText();
		sError += "\n";
		rCB.GetErrorList().AddMsg(sError.c_str(), iLine, iPos, CERR_INTERNAL, CEL_ERROR);
		return false;
	}

	TImage refImg = rCB.GetOGLLatex().GetImageRef(csText.Str());
	rVar = refImg;

	if (refImg.IsValid())
	{
		TScene refScene;

		COGLBitmap* pBMP = new COGLBitmap;
		refScene = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pBMP);

		pBMP->SetPosition(rCB.GetBMPPos());
		pBMP->SetVAlign(rCB.GetVBMPAlign());
		pBMP->SetHAlign(rCB.GetHBMPAlign());
		pBMP->SetScale(rCB.GetCurBitmapScale());
		pBMP->SetImageRef(refImg);

		rCB.GetOGLDrawBase()->DrawBaseElement(refScene);
	}
	else
	{
		rCB.GetErrorList().GeneralError("Latex image could not be generated.", iLine, iPos);
		return false;
	}

	//fMagStep = rCB.GetOGLLatex().GetMagStepScale(rCB.GetLatexMagStep());
	//if (!rCB.GetOGLLatex().Add(csText, xA, fMagStep, m_fVLatexAlign, m_fHLatexAlign,
	//					(csBMPName.Len() > 0 ? csBMPName.Str() : 0) ))
	//{
	//	string sError;
	//	sError = "Error: Cannot render Latex text.\n\nLatex Errors:\n";
	//	sError += rCB.GetOGLLatex().GetRenderErrorText();
	//	sError += "\n";
	//	rCB.GetErrorList().AddMsg(sError.c_str(), iLine, iPos, CERR_INTERNAL, CEL_ERROR);
	//	return false;
	//}

	//rCB.GetOGLLatex().Apply(0, csText.Str());
	//
	//float fL, fR, fB, fT, fZ1, fZ2;
	//if (rCB.GetOGLLatex().GetWorldCoordDims(csText.Str(), fL, fR, fB, fT, fZ1, fZ2))
	//{
	//	TVarList mRet;
	//	mRet.Set(6);
	//	mRet[0] = fL;
	//	mRet[1] = fB;
	//	mRet[2] = fZ1;
	//	mRet[3] = fR;
	//	mRet[4] = fT;
	//	mRet[5] = fZ2;

	//	rVar = mRet;
	//}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Get Latex Image Function

bool  GetLatexImageFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	COGLColor TextColor;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	// Set default text color to current color
	TextColor = rCB.GetOGLDrawBase()->GetColor();
	//TextColor.Set( 1.0f, 1.0f, 1.0f );

	if ((iVarCount != 2) && (iVarCount != 3))
	{
		int piPar[] = { 2, 3 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	// Render Latex and use generated Bitmap as texture
	TString csText, csFile;
	float fMagStep;
	COGLVertex xA;

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("First parameter has to be string containing the Latex text.", iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Second parameter has to be string containing a file name addition where Latex image is stored.", iLine, iPos);
		return false;
	}

	if (iVarCount >= 3)
	{
		if (mVars(2).BaseType() != PDT_COLOR)
		{
			rCB.GetErrorList().GeneralError("Third optional parameter has to be color.", iLine, iPos);
			return false;
		}

		TextColor = *mVars(2).GetOGLColorPtr();
	}

	csText = mVars(0).ValStr();
	csFile = mVars(1).ValStr();

	if (csText.Len() == 0)
	{
		rCB.GetErrorList().GeneralError("Empty string cannot be rendered.", iLine, iPos);
		return false;
	}

	if (csFile.Len() == 0)
	{
		rCB.GetErrorList().GeneralError("Name for LaTeX text has to be given.", iLine, iPos);
		return false;
	}

	//fMagStep = rCB.GetOGLLatex().GetMagStepScale(rCB.GetLatexMagStep());
	fMagStep = float(rCB.GetLatexMagStep());
	if (!rCB.GetOGLLatex().Add(csText, fMagStep, rCB.GetLatexMainFont().c_str(), TextColor, *((CImageRepository*)::GetImageRepositoryPtr()), csFile.Str()))
	{
		string sError;
		sError  = "Error: Cannot render Latex text.\n\nLatex Errors:\n";
		sError += rCB.GetOGLLatex().GetRenderErrorText();
		sError += "\n";
		rCB.GetErrorList().AddMsg(sError.c_str(), iLine, iPos, CERR_INTERNAL, CEL_ERROR);
		return false;
	}

	rVar = rCB.GetOGLLatex().GetImageRef(csText.Str());

	/*!
	COGLBitmap *pImage = rCB.GetOGLLatex().GetBitmap(csText.Str());
	if (!pImage)
	{
	rCB.GetErrorList().GeneralError("Internal Error: Latex bitmap not available.", iLine, iPos);
	return false;
	}

	rVar.New(PDT_IMAGE);
	TImage &rImg = *rVar.GetImagePtr();

	COGLColor rCurCol;
	glGetFloatv(GL_CURRENT_COLOR, rCurCol.Data());
	//pImage->FlushRGB(rCurCol);

	// Copy Image
	*((COGLBitmap *) rImg) = *pImage;
	rImg->FlushRGB(rCurCol);
	!*/

	return true;
}
