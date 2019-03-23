////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Process.cpp
//
// summary:   Implements the function process class
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

#include "Func_Process.h"
#include "Func_Info.h"

typedef unsigned char uchar;

//////////////////////////////////////////////////////////////////////
/// Convert Image to Matrix

bool  BMP2MatrixFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	int iChannel    = 0;

	if ((iVarCount < 1) || (iVarCount > 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (iVarCount == 2)
	{
		if (!mVars(1).CastToCounter(iChannel))
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if ((iChannel < 0) || (iChannel > 4))
		{
			rCB.GetErrorList().InvalidParVal(mVars(1), 2, iLine, iPos);
			return false;
		}
	}

	if (mVars(0).BaseType() != PDT_IMAGE)
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	TImage& rImg = *mVars(0).GetImagePtr();
	if (!rImg.IsValid())
	{
		rCB.GetErrorList().GeneralError("Image is not valid.", iLine, iPos);
		return false;
	}

	COGLImage* pImgNew = 0;
	COGLImage* pImage  = 0, * pImgA = ((COGLImage*) rImg);

	int iImgType, iDataType, iBPP;
	pImgA->GetType(iImgType, iDataType, iBPP);
	if ((iImgType == CLUVIZ_IMG_RGBA) && (iDataType == CLUVIZ_IMG_UNSIGNED_BYTE))
	{
		pImage = pImgA;
	}
	else
	{
		pImgNew = new COGLImage(*pImgA);
		pImgNew->ConvertType(CLUVIZ_IMG_RGBA, CLUVIZ_IMG_UNSIGNED_BYTE);
		pImage = pImgNew;
	}

	COGLImage& oglImage = *pImage;

	::LockImageAccess();

	int iWidth, iHeight, iImgPos, iMatPos, iSize, iLinePos;
	uchar* pImgData = oglImage.GetDataPtr();
	oglImage.GetSize(iWidth, iHeight);

	rVar.New(PDT_MATRIX);

	TMatrix& xM = *rVar.GetMatrixPtr();
	xM.Resize(iHeight, iWidth);
	TCVScalar* pMatData = xM.Data();

	iSize = iWidth * iHeight;

	if (iChannel > 0)
	{
		for (iMatPos = 0, iImgPos = 4 * (iSize - iWidth) + iChannel - 1, iLinePos = 0;
		     iMatPos < iSize;
		     ++iMatPos, iImgPos += 4, ++iLinePos)
		{
			if (iLinePos >= iWidth)
			{
				iLinePos = 0;
				iImgPos -= 8 * iWidth;
			}

			pMatData[iMatPos] = TCVScalar(pImgData[iImgPos]) / TCVScalar(255.0);
		}
	}
	else
	{
		// if no channel given, then average color
		for (iMatPos = 0, iImgPos = 4 * (iSize - iWidth), iLinePos = 0;
		     iMatPos < iSize;
		     ++iMatPos, iImgPos += 4, ++iLinePos)
		{
			if (iLinePos >= iWidth)
			{
				iLinePos = 0;
				iImgPos -= 8 * iWidth;
			}

			pMatData[iMatPos] = (TCVScalar(pImgData[iImgPos])
					     + TCVScalar(pImgData[iImgPos + 1])
					     + TCVScalar(pImgData[iImgPos + 2])) / TCVScalar(3.0 * 255.0);
		}
	}

	::UnlockImageAccess();

	if (pImgNew)
	{
		delete pImgNew;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Matrix to BMP

bool  Matrix2BMPFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	int iChannel    = 0;
	int iWidth, iHeight, iImgPos, iMatPos, iSize, iLinePos;
	int iMatCount = 0, piChannel[4];
	TMatrix* pM[4];
	TCVScalar* pMatData[4];
	TCVScalar dVal;
	uchar ucVal;

	if ((iVarCount < 1) || (iVarCount > 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() != PDT_MATRIX)
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		pM[0]       = mVars(0).GetMatrixPtr();
		pMatData[0] = pM[0]->Data();
		iMatCount   = 1;

		iHeight = pM[0]->Rows();
		iWidth  = pM[0]->Cols();
	}
	else if (iVarCount == 2)
	{
		if ((mVars(0).BaseType() == PDT_VARLIST) && (mVars(1).BaseType() == PDT_VARLIST))
		{
			int iMat;
			TVarList& rMatList = *mVars(0).GetVarListPtr();
			TVarList& rChList  = *mVars(1).GetVarListPtr();
			iMatCount = int(rMatList.Count());

			if (iMatCount != rChList.Count())
			{
				rCB.GetErrorList().GeneralError("Both lists need to be of same length.", iLine, iPos);
				return false;
			}

			for (iMat = 0; iMat < iMatCount; iMat++)
			{
				if (rMatList(iMat).BaseType() != PDT_MATRIX)
				{
					CStrMem csVal;

					csVal << "Element at position " << (iMat + 1) << "in first list is not a matrix.";
					rCB.GetErrorList().GeneralError(csVal.Str(), iLine, iPos);
					return false;
				}

				if (!rChList[iMat].CastToCounter(piChannel[iMat]))
				{
					CStrMem csVal;

					csVal << "Element at position " << (iMat + 1) << "in second list is not a counter.";
					rCB.GetErrorList().GeneralError(csVal.Str(), iLine, iPos);
					return false;
				}

				if ((piChannel[iMat] < 1) || (piChannel[iMat] > 4))
				{
					CStrMem csVal;

					csVal << "Element at position " << (iMat + 1)
					      << "in second list is not in the range of 1 to 4.";
					rCB.GetErrorList().GeneralError(csVal.Str(), iLine, iPos);
					return false;
				}

				pM[iMat] = rMatList(iMat).GetMatrixPtr();
				if (iMat == 0)
				{
					iHeight = pM[0]->Rows();
					iWidth  = pM[0]->Cols();
				}
				else
				{
					if ((iHeight != pM[iMat]->Rows()) ||
					    (iWidth != pM[iMat]->Cols()))
					{
						CStrMem csVal;

						csVal << "Matrix at position " << (iMat + 1)
						      << "in first list is not of the same dimensions as first matrix in list.";
						rCB.GetErrorList().GeneralError(csVal.Str(), iLine, iPos);
						return false;
					}
				}
				pMatData[iMat] = pM[iMat]->Data();
			}
		}
		else
		{
			if (!mVars(1).CastToCounter(iChannel))
			{
				rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
				return false;
			}

			if ((iChannel < 1) || (iChannel > 4))
			{
				rCB.GetErrorList().InvalidParVal(mVars(1), 2, iLine, iPos);
				return false;
			}

			if (mVars(0).BaseType() != PDT_MATRIX)
			{
				rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
				return false;
			}

			pM[0]       = mVars(0).GetMatrixPtr();
			pMatData[0] = pM[0]->Data();
			iMatCount   = 1;

			iHeight = pM[0]->Rows();
			iWidth  = pM[0]->Cols();
		}
	}

	rVar.New(PDT_IMAGE);
	TImage& rImg = *rVar.GetImagePtr();
	if (!rImg.IsValid())
	{
		rCB.GetErrorList().GeneralError("Cannot create image.", iLine, iPos);
		return false;
	}
	COGLImage& oglImage = *((COGLImage*) rImg);

	oglImage.ConvertType(CLUVIZ_IMG_RGBA, CLUVIZ_IMG_UNSIGNED_BYTE);
	if (!oglImage.SetSize(iWidth, iHeight))
	{
		rCB.GetErrorList().GeneralError("Cannot create image.", iLine, iPos);
		return false;
	}

	::LockImageAccess();
	uchar* pImgData = oglImage.GetDataPtr();

	iSize = iWidth * iHeight;
	// Set Image Data to zero everywhere
	memset(pImgData, 0, iSize * 4);

	if (iChannel > 0)
	{
		for (iMatPos = 0, iImgPos = 4 * (iSize - iWidth) + iChannel - 1, iLinePos = 0;
		     iMatPos < iSize;
		     ++iMatPos, iImgPos += 4, ++iLinePos)
		{
			if (iLinePos >= iWidth)
			{
				iLinePos = 0;
				iImgPos -= 8 * iWidth;
			}

			dVal = pMatData[0][iMatPos] * 255.0;
			if (dVal < 0.0)
			{
				pImgData[iImgPos] = uchar(0);
			}
			else if (dVal > 255.0)
			{
				pImgData[iImgPos] = uchar(255);
			}
			else
			{
				pImgData[iImgPos] = uchar(floor(dVal + 0.5));
			}

			if (iChannel < 4)
			{
				pImgData[iImgPos - iChannel + 4] = uchar(255);
			}
		}
	}
	else if (iMatCount > 1)
	{
		for (iMatPos = 0, iImgPos = 4 * (iSize - iWidth), iLinePos = 0;
		     iMatPos < iSize;
		     ++iMatPos, iImgPos += 4, ++iLinePos)
		{
			if (iLinePos >= iWidth)
			{
				iLinePos = 0;
				iImgPos -= 8 * iWidth;
			}

			// Set alpha channel just in case
			pImgData[iImgPos + 3] = uchar(255);

			for (int iMat = 0; iMat < iMatCount; iMat++)
			{
				dVal = pMatData[iMat][iMatPos] * 255.0;
				if (dVal < 0.0)
				{
					pImgData[iImgPos + piChannel[iMat] - 1] = uchar(0);
				}
				else if (dVal > 255.0)
				{
					pImgData[iImgPos + piChannel[iMat] - 1] = uchar(255);
				}
				else
				{
					pImgData[iImgPos + piChannel[iMat] - 1] = uchar(floor(dVal + 0.5));
				}
			}
		}
	}
	else
	{
		// if no channel given, then average color
		TCVScalar dVal;

		for (iMatPos = 0, iImgPos = 4 * (iSize - iWidth), iLinePos = 0;
		     iMatPos < iSize;
		     ++iMatPos, iImgPos += 4, ++iLinePos)
		{
			if (iLinePos >= iWidth)
			{
				iLinePos = 0;
				iImgPos -= 8 * iWidth;
			}

			dVal = pMatData[0][iMatPos] * 255.0;
			if (dVal < 0.0)
			{
				ucVal = uchar(0);
			}
			else if (dVal >= 255.0)
			{
				ucVal = uchar(255);
			}
			else
			{
				ucVal = uchar(floor(dVal + 0.5));
			}

			pImgData[iImgPos]     = pImgData[iImgPos + 1] = pImgData[iImgPos + 2] = ucVal;
			pImgData[iImgPos + 3] = uchar(255);	// Opaque Image
		}
	}

	::UnlockImageAccess();
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Resize Image Function

bool  ResizeBMPFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int i, iVarCount = int(mVars.Count());
	TCVScalar pdVal[2];

	if (iVarCount == 3)
	{
		if (mVars(0).BaseType() != PDT_IMAGE)
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		for (i = 1; i < 3; i++)
		{
			if (!mVars(i).CastToScalar(pdVal[i - 1], rCB.GetSensitivity()))
			{
				rCB.GetErrorList().InvalidParType(mVars(i), i + 1, iLine, iPos);
				return false;
			}

			if (pdVal[i - 1] <= 0)
			{
				char pcText[100];
				sprintf_s(pcText, "New %s of image has to be greater than zero.",
						((i - 1) == 0 ? "width" : "height"));
				rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
				return false;
			}
		}
	}
	else
	{
		int piPar[] = { 3 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	TImage& rImg = *(mVars(0).GetImagePtr());
	if (!rImg->ResizeImage(int(pdVal[0]), int(pdVal[1])))
	{
		rCB.GetErrorList().GeneralError("Error resizing image.", iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Resize Canvas Function

bool  ReshapeBMPFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int i, iVarCount = int(mVars.Count());
	TCVScalar pdVal[2], pdAlign[2];
	COGLColor colClear;

	colClear.Set(0, 0, 0);
	pdAlign[0] = 0.5;
	pdAlign[1] = 0.5;

	if ((iVarCount != 3) && (iVarCount != 4) && (iVarCount != 6))
	{
		int piPar[] = { 3, 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_IMAGE)
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	for (i = 1; i < 3; i++)
	{
		if (!mVars(i).CastToScalar(pdVal[i - 1], rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(mVars(i), i + 1, iLine, iPos);
			return false;
		}

		if (pdVal[i - 1] <= 0)
		{
			char pcText[100];
			sprintf_s(pcText, "New %s of image has to be greater than zero.",
					((i - 1) == 0 ? "width" : "height"));
			rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
			return false;
		}
	}

	if (iVarCount > 3)
	{
		if (mVars(3).BaseType() != PDT_COLOR)
		{
			rCB.GetErrorList().InvalidParType(mVars(3), 4, iLine, iPos);
			return false;
		}

		colClear = *mVars(3).GetOGLColorPtr();
	}

	if (iVarCount > 5)
	{
		for (i = 4; i < 6; i++)
		{
			if (!mVars(i).CastToScalar(pdAlign[i - 4], rCB.GetSensitivity()))
			{
				rCB.GetErrorList().InvalidParType(mVars(i), i + 1, iLine, iPos);
				return false;
			}
		}
	}

	TImage& rImg = *(mVars(0).GetImagePtr());
	if (!rImg->ResizeCanvas(int(pdVal[0]), int(pdVal[1]), &colClear, pdAlign[0], pdAlign[1]))
	{
		rCB.GetErrorList().GeneralError("Error resizing image.", iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Flip Image Horizontally or Vertically
///
/// Parameters:
///		1. (int) If != 0 flip horizontally, otherwise vertically
///

bool  FlipBMPFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	TCVScalar pdVal[1];

	if (iVarCount == 2)
	{
		if (mVars(0).BaseType() != PDT_IMAGE)
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!mVars(1).CastToScalar(pdVal[0], rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}
	}
	else
	{
		int piPar[] = { 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	TImage& rImg = *mVars(0).GetImagePtr();
	if (!rImg.IsValid())
	{
		rCB.GetErrorList().GeneralError("Invalid image.", iLine, iPos);
		return false;
	}

	if (!rImg->FlipImage(((pdVal[0] != 0) ? true : false)))
	{
		rCB.GetErrorList().GeneralError("Error flipping image.", iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Rotate Image Function
///
/// Parameters:
///		1. Angle of rotation in radians
///

//bool  RotateBMPFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
//{
//	TVarList& mVars = *rPars.GetVarListPtr();
//	int iVarCount = mVars.Count();
//	TCVScalar pdVal[2];
//
//	if (iVarCount == 2)
//	{
//		if (mVars(0).BaseType() != PDT_IMAGE)
//		{
//			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
//			return false;
//		}
//
//		if (!mVars(1).CastToScalar(pdVal[0], rCB.GetSensitivity()))
//		{
//			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
//			return false;
//		}
//	}
//	else
//	{
//		int piPar[] = {2};
//
//		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
//		return false;
//	}
//
//	TImage &rImg = *mVars(0).GetImagePtr();
//	if (!rImg.IsValid())
//	{
//		rCB.GetErrorList().GeneralError("Invalid image.", iLine, iPos);
//		return false;
//	}
//
//	if ( !rImg->RotateImage( float(pdVal[0] / rCB.GetRadPerDeg()) ) )
//	{
//		rCB.GetErrorList().GeneralError("Error rotating image.", iLine, iPos);
//		return false;
//	}
//
//	return true;
//}
//

//////////////////////////////////////////////////////////////////////
/// Rotate Image in 90� steps Function
///
/// Parameters:
///		1. Rotation step
///

bool  Rotate90BMPFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	TCVCounter iVal;

	if (iVarCount == 2)
	{
		if (mVars(0).BaseType() != PDT_IMAGE)
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!mVars(1).CastToCounter(iVal))
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}
	}
	else
	{
		int piPar[] = { 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	TImage& rImg = *mVars(0).GetImagePtr();
	if (!rImg.IsValid())
	{
		rCB.GetErrorList().GeneralError("Invalid image.", iLine, iPos);
		return false;
	}

	if (!rImg->Rotate90Image(iVal))
	{
		rCB.GetErrorList().GeneralError("Error rotating image.", iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Copy Image Function
///
/// Parameters:
///		1. Image to be copied
///

bool  CopyBMPFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() != PDT_IMAGE)
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}
	}
	else
	{
		int piPar[] = { 1 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	TImage& rImg = *mVars(0).GetImagePtr();
	if (!rImg.IsValid())
	{
		rCB.GetErrorList().GeneralError("Invalid image.", iLine, iPos);
		return false;
	}

	rVar.New(PDT_IMAGE);
	TImage& rNewImg = *rVar.GetImagePtr();

	if (!rNewImg.IsValid())
	{
		rCB.GetErrorList().GeneralError("Could not create image.", iLine, iPos);
		return false;
	}

	rNewImg->SetFilename("Memory");
	rNewImg->operator=(*((COGLImage*) rImg));

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Insert Image Function
///
/// Origin is bottom left.
///
/// Parameters:
///		1. Target Image
///		2. Source Image
///		3. X-Pos. in Target
///		4. Y-Pos. in Target

bool  InsertBMPFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int i, iVarCount = int(mVars.Count());
	TCVCounter piVal[2], piCh[2] = { -1, -1 };

	if ((iVarCount == 4) || (iVarCount == 6))
	{
		if (mVars(0).BaseType() != PDT_IMAGE)
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (mVars(1).BaseType() != PDT_IMAGE)
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		for (i = 2; i < 4; i++)
		{
			if (!mVars(i).CastToCounter(piVal[i - 2]))
			{
				rCB.GetErrorList().InvalidParType(mVars(i), i + 1, iLine, iPos);
				return false;
			}

			if (piVal[i - 2] < 0)
			{
				if (i == 2)
				{
					rCB.GetErrorList().GeneralError("X-position has to be greater than zero.", iLine, iPos);
				}
				else
				{
					rCB.GetErrorList().GeneralError("Y-position has to be greater than zero.", iLine, iPos);
				}

				return false;
			}
		}

		if (iVarCount == 6)
		{
			for (i = 4; i < 6; i++)
			{
				if (!mVars(i).CastToCounter(piCh[i - 4]))
				{
					rCB.GetErrorList().InvalidParType(mVars(i), i + 1, iLine, iPos);
					return false;
				}

				if ((piCh[i - 4] < 0) || (piCh[i - 4] > 4))
				{
					if (i == 4)
					{
						rCB.GetErrorList().GeneralError("Target channel has to be in range 0 to 4.", iLine, iPos);
					}
					else
					{
						rCB.GetErrorList().GeneralError("Source channel has to be in range 0 to 4.", iLine, iPos);
					}

					return false;
				}

				piCh[i - 4]--;
			}
		}
	}
	else
	{
		int piPar[] = { 4, 6 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	TImage& rTrgImg = *(mVars(0).GetImagePtr());
	TImage& rSrcImg = *(mVars(1).GetImagePtr());

	int iWidth, iHeight;
	rTrgImg->GetSize(iWidth, iHeight);

	if (((COGLImage*) rSrcImg) == ((COGLImage*) rTrgImg))
	{
		rCB.GetErrorList().GeneralError("Cannot insert an image into itself.", iLine, iPos);
		return false;
	}

	if (!rTrgImg->InsertImage(*((COGLImage*) rSrcImg), piVal[0] - 1, piVal[1] - 1, piCh[0], piCh[1]))
	{
		rCB.GetErrorList().GeneralError("Error inserting image.", iLine, iPos);
		return false;
	}

	return true;
}

bool  ConvertBMPTypeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 3)
	{
		int piPar[] = { 3 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_IMAGE)
	{
		rCB.GetErrorList().GeneralError("First parameter has to be an image.", iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Second parameter has to give the image type.", iLine, iPos);
		return false;
	}

	if (mVars(2).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Third parameter has to give the data type.", iLine, iPos);
		return false;
	}

	TImage rImage = *mVars(0).GetImagePtr();

	if (!rImage.IsValid())
	{
		rCB.GetErrorList().GeneralError("Invalid image", iLine, iPos);
		return false;
	}

	int iImgType, iDataType;
	TString csImgType  = *mVars(1).GetStringPtr();
	TString csDataType = *mVars(2).GetStringPtr();

	if (!GetImgTypeID(iImgType, iDataType, csImgType, csDataType))
	{
		rCB.GetErrorList().GeneralError("Invalid image type", iLine, iPos);
		return false;
	}

	if (!rImage->ConvertType(iImgType, iDataType))
	{
		rCB.GetErrorList().GeneralError("Image cannot be converted", iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// New Bitmap Function

bool  NewBMPFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	CStrMem csFilename;
	TVarList& mVars = *rPars.GetVarListPtr();
	TCVCounter iCX, iCY;

	int iVarCount = int(mVars.Count());
	COGLVertex xA;
	COGLColor colClear;
	TString csImgType, csDataType;
	int iImgType = 0, iDataType = 0;

	csImgType  = "rgba";
	csDataType = "u8";
	colClear.Set(0, 0, 0, 1);

	if ((iVarCount != 2) && (iVarCount != 3) && (iVarCount != 5))
	{
		int piPar[] = { 2, 3, 5 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 3, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iCX))
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToCounter(iCY))
	{
		rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	if (iCX <= 0)
	{
		rCB.GetErrorList().GeneralError("Width of image has to be greater than zero.", iLine, iPos);
		return false;
	}

	if (iCY <= 0)
	{
		rCB.GetErrorList().GeneralError("Height of image has to be greater than zero.", iLine, iPos);
		return false;
	}

	if (iVarCount >= 3)
	{
		if (mVars(2).BaseType() != PDT_COLOR)
		{
			rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		colClear = *mVars(2).GetOGLColorPtr();
	}

	if (iVarCount >= 5)
	{
		if (mVars(3).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().GeneralError("Fourth parameter has to give the image type.", iLine, iPos);
			return false;
		}

		if (mVars(4).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().GeneralError("Fifth parameter has to give the data type.", iLine, iPos);
			return false;
		}

		csImgType  = *mVars(3).GetStringPtr();
		csDataType = *mVars(4).GetStringPtr();
	}

	if (!GetImgTypeID(iImgType, iDataType, csImgType, csDataType))
	{
		rCB.GetErrorList().GeneralError("Invalid image type", iLine, iPos);
		return false;
	}

	rVar.New(PDT_IMAGE);

	TImage& oglImage = *rVar.GetImagePtr();
	if (oglImage.IsValid())
	{
		oglImage->SetFilename("Untitled.png");
		oglImage->ConvertType(iImgType, iDataType);
		oglImage->SetSize(iCX, iCY, &colClear);
	}
	else
	{
		rCB.GetErrorList().GeneralError("Cannot create image variable.", iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Clear Bitmap Function

bool  ClearBMPFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	CStrMem csFilename;
	TVarList& mVars = *rPars.GetVarListPtr();
	//TCVCounter iCX, iCY;

	int iVarCount = int(mVars.Count());
	COGLColor colClear;

	colClear.Set(0, 0, 0, 1);

	if ((iVarCount != 1) && (iVarCount != 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (iVarCount >= 2)
	{
		if (mVars(1).BaseType() != PDT_COLOR)
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		colClear = *mVars(1).GetOGLColorPtr();
	}

	TImage rImage = *mVars(0).GetImagePtr();
	if (!rImage.IsValid())
	{
		rCB.GetErrorList().GeneralError("Invalid image variable.", iLine, iPos);
		return false;
	}

	rImage->ClearImage(&colClear);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 	Sample area function.
/// </summary>
///
/// <param name="rCB">   [in,out] The cb. </param>
/// <param name="rVar">  [in,out] The variable. </param>
/// <param name="rPars"> [in,out] The pars. </param>
/// <param name="iLine"> Zero-based index of the line. </param>
/// <param name="iPos">  Zero-based index of the position. </param>
///
///  Function parameter:
///  1. The image to sample
///  2. The position to sample X
///  3. The position to sample X
///  4. The AreaSize (diameter) to sample
///  5. The sample type ("average", "mean")
///  6. The sample area form ("box", "circle")
///
/// <returns> True if it succeeds, false if it fails. </returns>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SampleImgAreaFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 7)
	{
		int piPar[] = { 7 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_IMAGE)
	{
		rCB.GetErrorList().GeneralError("Expect first parameter to be an image.", iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_SCALAR)
	{
		rCB.GetErrorList().GeneralError("Expect second parameter to be  counter/integer (position X to sample).", iLine, iPos);
		return false;
	}

	if (mVars(2).BaseType() != PDT_SCALAR)
	{
		rCB.GetErrorList().GeneralError("Expect third parameter to be  counter/integer  (position Y to sample).", iLine, iPos);
		return false;
	}

	if (mVars(3).BaseType() != PDT_SCALAR)
	{
		rCB.GetErrorList().GeneralError("Expect fourth parameter to be float (sample area size).", iLine, iPos);
		return false;
	}

	if (mVars(4).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Expect fifth parameter to be a string (sample type) .", iLine, iPos);
		return false;
	}

	if (mVars(5).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Expect sixth parameter to a string (sample area type).", iLine, iPos);
		return false;
	}

	if (mVars(6).BaseType() != PDT_SCALAR)
	{
		rCB.GetErrorList().GeneralError("Expect seventh parameter to a float (value to ignore).", iLine, iPos);
		return false;
	}

	TImage rImage = *mVars(0).GetImagePtr();
	if (!rImage.IsValid())
	{
		rCB.GetErrorList().GeneralError("Invalid image variable.", iLine, iPos);
		return false;
	}

	// get the values from the arguments
	TCVScalar fSamplePosX    = *mVars(1).GetScalarPtr();
	TCVScalar fSamplePosY    = *mVars(2).GetScalarPtr();
	TCVScalar fSampleArea    = *mVars(3).GetScalarPtr();
	TString& sSampleType     = *mVars(4).GetStringPtr();
	TString& sSampleAreaType = *mVars(5).GetStringPtr();
	TCVScalar fValueToIgnore = *mVars(6).GetScalarPtr();

	if ((sSampleType != "mean") && (sSampleType != "median") && (sSampleType != "additive"))
	{
		rCB.GetErrorList().GeneralError("Sample type parameter has to be 'mean', 'median' or 'additive'", iLine, iPos);
		return false;
	}

	if ((sSampleAreaType != "box") &&  (sSampleAreaType != "circle"))
	{
		rCB.GetErrorList().GeneralError("Sample Area type parameter has to be 'box' or 'circle'", iLine, iPos);
		return false;
	}

	std::vector<float> lResult = rImage->SampleArea((float) fSamplePosX, (float) fSamplePosY, (float) fSampleArea, sSampleType, sSampleAreaType, (float) fValueToIgnore);

	rVar.New(PDT_VARLIST);
	TVarList& rVarList = *rVar.GetVarListPtr();
	rVarList.Set(lResult.size());

	rVarList(0) = lResult[0];
	rVarList(1) = lResult[1];
	rVarList(2) = lResult[2];
	rVarList(3) = lResult[3];

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Get Image bounding box Function
///
/// Parameter:
///		1. The image
///		2. the border color
///
///	Returns:
///		List of bounding box as [ 'left', 'bottom', 'width', 'height' ]
///

bool  GetBMPBoundingBoxFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount != 2)
	{
		int piPar[] = { 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_IMAGE)
	{
		rCB.GetErrorList().GeneralError("Expect first parameter to be an image.", iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_COLOR)
	{
		rCB.GetErrorList().GeneralError("Expect second parameter to be the background color.", iLine, iPos);
		return false;
	}

	TImage& rImg      = *(mVars(0).GetImagePtr());
	TOGLColor& rColor = *mVars(1).GetOGLColorPtr();
	Mem<int> mBox;

	if (!rImg->GetBoundingBox(mBox, rColor))
	{
		rCB.GetErrorList().GeneralError("Error estimating bounding box.", iLine, iPos);
		return false;
	}

	rVar.New(PDT_VARLIST);
	TVarList& rList = *rVar.GetVarListPtr();

	rList.Set(4);
	rList(0) = mBox[0] + 1;
	rList(1) = mBox[1] + 1;
	rList(2) = mBox[2];
	rList(3) = mBox[3];

	return true;
}
