////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Info.cpp
//
// summary:   Implements the function information class
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

#include "Func_Info.h"

typedef unsigned char uchar;

//////////////////////////////////////////////////////////////////////
/// Get Bitmap Type

bool  GetBMPTypeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 1)
	{
		int piPar[] = { 1 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_IMAGE)
	{
		rCB.GetErrorList().GeneralError("First parameter has to be an image.", iLine, iPos);
		return false;
	}

	int iImgType, iDataType, iBPP;
	TImage rImage = *mVars(0).GetImagePtr();

	if (!rImage.IsValid())
	{
		rCB.GetErrorList().GeneralError("Invalid image", iLine, iPos);
		return false;
	}

	rImage->GetType(iImgType, iDataType, iBPP);

	rVar.New(PDT_VARLIST);
	TVarList& rListName = *rVar.GetVarListPtr();

	rListName.Set(2);

	switch (iImgType)
	{
	case CLUVIZ_IMG_RGB:
		rListName(0) = CStrMem("rgb");
		break;

	case CLUVIZ_IMG_RGBA:
		rListName(0) = CStrMem("rgba");
		break;

	case CLUVIZ_IMG_BGR:
		rListName(0) = CStrMem("bgr");
		break;

	case CLUVIZ_IMG_BGRA:
		rListName(0) = CStrMem("bgra");
		break;

	case CLUVIZ_IMG_LUMINANCE:
		rListName(0) = CStrMem("lum");
		break;

	case CLUVIZ_IMG_LUMINANCE_ALPHA:
		rListName(0) = CStrMem("luma");
		break;

	default:
		rListName(0) = CStrMem("unknown");
		break;
	}

	switch (iDataType)
	{
	case CLUVIZ_IMG_BYTE:
		rListName(1) = CStrMem("8");
		break;

	case CLUVIZ_IMG_UNSIGNED_BYTE:
		rListName(1) = CStrMem("u8");
		break;

	case CLUVIZ_IMG_SHORT:
		rListName(1) = CStrMem("16");
		break;

	case CLUVIZ_IMG_UNSIGNED_SHORT:
		rListName(1) = CStrMem("u16");
		break;

	case CLUVIZ_IMG_INT:
		rListName(1) = CStrMem("32");
		break;

	case CLUVIZ_IMG_UNSIGNED_INT:
		rListName(1) = CStrMem("u32");
		break;

	case CLUVIZ_IMG_FLOAT:
		rListName(1) = CStrMem("float");
		break;

	case CLUVIZ_IMG_DOUBLE:
		rListName(1) = CStrMem("double");
		break;

	default:
		rListName(1) = CStrMem("unknown");
		break;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Convert Bitmap Type

bool  GetImgTypeID(int& iImgType, int& iDataType, const CStrMem& csImgType, const CStrMem& csDataType)
{
	if (csImgType == "rgb")
	{
		iImgType = CLUVIZ_IMG_RGB;
	}
	else if (csImgType == "rgba")
	{
		iImgType = CLUVIZ_IMG_RGBA;
	}
	else if (csImgType == "bgr")
	{
		iImgType = CLUVIZ_IMG_BGR;
	}
	else if (csImgType == "bgra")
	{
		iImgType = CLUVIZ_IMG_BGRA;
	}
	else if (csImgType == "lum")
	{
		iImgType = CLUVIZ_IMG_LUMINANCE;
	}
	else if (csImgType == "luma")
	{
		iImgType = CLUVIZ_IMG_LUMINANCE_ALPHA;
	}
	else
	{
		return false;
	}

	if (csDataType == "8")
	{
		iDataType = CLUVIZ_IMG_BYTE;
	}
	else if (csDataType == "u8")
	{
		iDataType = CLUVIZ_IMG_UNSIGNED_BYTE;
	}
	else if (csDataType == "16")
	{
		iDataType = CLUVIZ_IMG_SHORT;
	}
	else if (csDataType == "u16")
	{
		iDataType = CLUVIZ_IMG_UNSIGNED_SHORT;
	}
	else if (csDataType == "32")
	{
		iDataType = CLUVIZ_IMG_INT;
	}
	else if (csDataType == "u32")
	{
		iDataType = CLUVIZ_IMG_UNSIGNED_INT;
	}
	else if (csDataType == "float")
	{
		iDataType = CLUVIZ_IMG_FLOAT;
	}
	else if (csDataType == "double")
	{
		iDataType = CLUVIZ_IMG_DOUBLE;
	}
	else
	{
		return false;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// The Size FUNCTION
/// Returns the size of various objects

bool  SizeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = int(mVars.Count());

	if (iVarCount == 1)
	{
		ECodeDataType eType = mVars(0).BaseType();

		if (eType == PDT_STRING)
		{
			CStrMem& rStr = *mVars(0).GetStringPtr();

			rVar = (int)rStr.Len();
		}
		else if (eType == PDT_VARLIST)
		{
			TVarList& rList = *mVars(0).GetVarListPtr();

			rVar = (int)rList.Count();
		}
		else if (eType == PDT_VEXLIST)
		{
			TVexList& rList = *mVars(0).GetVexListPtr();

			rVar = (int)rList.Count();
		}
		else if (eType == PDT_INT)
		{
			int iVal = *mVars(0).GetIntPtr();

			if (iVal == 0)
			{
				rVar = (int)0;
			}
			else
			{
				rVar = (int)1;
			}
		}
		else if (eType == PDT_MULTIV)
		{
			TMultiV& vA = *mVars(0).GetMultiVPtr();

			rVar = (TCVCounter)vA.GetGADim();
		}
		else if (eType == PDT_MATRIX)
		{
			TMatrix& xM = *mVars(0).GetMatrixPtr();

			rVar.New(PDT_VARLIST);
			TVarList& rList = *rVar.GetVarListPtr();
			rList.Set(2);
			rList[0] = xM.Rows();
			rList[1] = xM.Cols();
		}
		else if (eType == PDT_TENSOR)
		{
			TTensor& rT = *mVars(0).GetTensorPtr();

			rVar.New(PDT_VARLIST);
			TVarList& rList = *rVar.GetVarListPtr();

			int iDim, iValence = rT.Valence();
			rList.Set(iValence);

			for (iDim = 0; iDim < iValence; iDim++)
			{
				rList[iDim] = rT.DimSize(iDim);
			}
		}
		else if (eType == PDT_TENSOR_IDX)
		{
			TTensorIdx& rTIdx = *mVars(0).GetTensorIdxPtr();

			rVar.New(PDT_VARLIST);
			TVarList& rList = *rVar.GetVarListPtr();

			try
			{
				int iLoop, iLoopCount;
				const TTensorIdx::TFreeIdxMap& mapFreeIdx = rTIdx.GetFreeIdxMap();
				TTensorIdx::TFreeIdxMap::const_iterator it_El, it_End;

				rList.Set(iLoopCount = int(mapFreeIdx.size()));
				iLoop = iLoopCount - 1;
				it_End = mapFreeIdx.end();
				for (it_El = mapFreeIdx.begin();
					it_El != it_End;
					++it_El, --iLoop)
				{
					rList[iLoop] = it_El->second.iMax - it_El->second.iMin + 1;
				}
			}
			catch (...)
			{
				rCB.GetErrorList().GeneralError("Error evaluating size of tensor index.", iLine, iPos);
				return false;
			}
		}
		else if (eType == PDT_IMAGE)
		{
			TImage& Img = *mVars(0).GetImagePtr();
			int iWidth = 0, iHeight = 0;

			Img->GetSize(iWidth, iHeight);

			rVar.New(PDT_VARLIST);
			TVarList& rList = *rVar.GetVarListPtr();
			rList.Set(2);
			rList[0] = iWidth;
			rList[1] = iHeight;
		}
		else if (eType == PDT_SCENE)
		{
			TScene& rScene = *mVars(0).GetScenePtr();
			if (rScene.IsValid())
			{
				// Test whether scene is a texture object
				COGLTexture* pTex = dynamic_cast<COGLTexture*>((COGLBaseElement*)rScene);
				// Test whether scene is a vertex list
				COGLVertexList* pVexList = dynamic_cast<COGLVertexList*>((COGLBaseElement*)rScene);

				if (pTex)
				{
					//int iIdx;
					//int iX, iY;
					//int iOffsetX, iOffsetY;
					int iWidth, iHeight;

					double dTotalWidth, dTotalHeight;

					pTex->GetTextureSize(iWidth, iHeight);

					dTotalWidth = double(iWidth);
					dTotalHeight = double(iHeight);

					rVar.New(PDT_VARLIST);
					TVarList& rBase = *rVar.GetVarListPtr();
					rBase.Set(1);

					// Size of total texture
					rBase(0).New(PDT_VARLIST);
					TVarList& rSizePart = *rBase(0).GetVarListPtr();
					rSizePart.Set(2);
					rSizePart(0) = CStrMem("size");

					rSizePart(1).New(PDT_VARLIST);
					TVarList& rSize = *rSizePart(1).GetVarListPtr();
					rSize.Set(2);
					rSize(0) = iWidth;
					rSize(1) = iHeight;
				}
				else if (pVexList)
				{
					if (!rCB.VexListGetSize(*pVexList, rVar, iLine, iPos))
					{
						return false;
					}
				}
			}
		}
		else
		{
			rVar = (int)1;
			//rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			//return false;
		}
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// The Type FUNCTION
/// Returns type of variable as string

bool TypeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = int(mVars.Count());

	if (iVarCount == 1)
	{
		CCodeVar* pPar;

		if (mVars(0).BaseType() == PDT_PTR_VAR)
		{
			pPar = &(mVars(0).DereferenceVarPtr());
		}
		else
		{
			pPar = &(mVars(0));
		}

		if (pPar)
		{
			rVar = pPar->TypeStr().c_str();
		}
	}
	else if (iVarCount == 0)
	{
		rVar = "Empty";
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	return true;
}
