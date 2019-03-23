////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_IO.cpp
//
// summary:   Implements the function i/o class
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

#include "Func_IO.h"
#include "CluTec.Viz.Parse\Encode.h"

// For the generation of the script dependency paths
#include "CluTec.Viz.Base\Environment.h"

#ifdef _GNUCPP3_
    #include <unistd.h>
	#define _getcwd getcwd
	#define _chdir chdir
#else
    #include <direct.h>
#endif

#undef LoadImage

typedef unsigned char uchar;

//////////////////////////////////////////////////////////////////////
/// Load Bitmap Function

bool  LoadBMPFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	char pcCurPath[500];
	CStrMem csFilename;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	COGLVertex xA;
	bool bExitOnError  = true;
	bool bDoDecode     = false;
	bool bImgTypeGiven = false;
	TCVCounter uKey1   = 0, uKey2 = 0;

	if ((iVarCount < 1) || (iVarCount > 6))
	{
		int piPar[] = { 1, 2, 3, 4, 5, 6 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 6, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().InvalidParType(rVar, 1, iLine, iPos);
		return false;
	}

	csFilename = *mVars(0).GetStringPtr();

	std::string sFilename;
	// Use a standard search pattern to look for resource file in various directories.
	Clu::Viz::CEnvironment::FindResourceFile(sFilename, rCB.GetScriptPath(), rCB.GetScriptName(), csFilename.Str());

	int iImgType = -1, iDataType = -1;

	if ((iVarCount == 2) || (iVarCount == 4) || (iVarCount == 6))
	{
		bExitOnError = false;
	}

	if ((iVarCount >= 4) && (mVars(2).BaseType() == PDT_STRING))
	{
		if (mVars(2).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().GeneralError("Third parameter has to give the image type.", iLine, iPos);
			return false;
		}

		if (mVars(3).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().GeneralError("Fourth parameter has to give the data type.", iLine, iPos);
			return false;
		}

		bImgTypeGiven = true;

		TString csImgType  = *mVars(2).GetStringPtr();
		TString csDataType = *mVars(3).GetStringPtr();

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
			rCB.GetErrorList().GeneralError("Invalid image type", iLine, iPos);
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
			rCB.GetErrorList().GeneralError("Invalid data type", iLine, iPos);
			return false;
		}
	}

	if (((iVarCount == 4) && !bImgTypeGiven) || (iVarCount == 6))
	{
		int iIdx = iVarCount - 2;
		bDoDecode = true;

		if (!mVars(iIdx).CastToCounter(uKey1))
		{
			rCB.GetErrorList().GeneralError("Decode key 1 has to be an integer.", iLine, iPos);
			return false;
		}

		if (!mVars(iIdx + 1).CastToCounter(uKey2))
		{
			rCB.GetErrorList().GeneralError("Decode key 2 has to be an integer.", iLine, iPos);
			return false;
		}
	}

/*
        if (csFilename[1] != ':')
        {
                // Insert Path to currently loaded script before filename.
                sFilename = rCB.GetScriptPath() + sFilename;
        }
*/

	TImage* pImg = 0;

	if (bExitOnError)
	{
		rVar.New(PDT_IMAGE);
		pImg = rVar.GetImagePtr();
	}
	else
	{
		// Do not give variable new name, since it then looses its identity.
		mVars(1).New(PDT_IMAGE);
		pImg = mVars(1).GetImagePtr();
	}

	TImage& oglImage = *pImg;

	if (oglImage.IsValid())
	{
		oglImage->SetFilename(sFilename.c_str());

		_getcwd(pcCurPath, 499);
		_chdir(rCB.GetScriptPath().c_str());

		if (bDoDecode)
		{
			CEncode xEncode;
			std::string sFileID;

			if (!xEncode.ReadImage(sFilename, sFileID, *((COGLImage*) oglImage), uKey1, uKey2))
			{
				if (bExitOnError)
				{
					rCB.GetErrorList().AddMsg("Error: Cannot load image.", iLine, iPos, CERR_INTERNAL, CEL_ERROR);
					_chdir(pcCurPath);
					return false;
				}
				else
				{
					rVar = CStrMem("Error: Cannot load image");
				}
			}
			else if (!bExitOnError)
			{
				rVar = CStrMem("OK");
			}

			if ((iImgType >= 0) && (iDataType >= 0))
			{
				oglImage->ConvertType(iImgType, iDataType);
			}
		}
		else
		{
			if (!oglImage->LoadImage(false, iImgType, iDataType))
			{
				if (bExitOnError)
				{
					rCB.GetErrorList().AddMsg("Error: Cannot load image.", iLine, iPos, CERR_INTERNAL, CEL_ERROR);
					_chdir(pcCurPath);
					return false;
				}
				else
				{
					rVar = CStrMem("Error: Cannot load image");
				}
			}
			else if (!bExitOnError)
			{
				rVar = CStrMem("OK");
			}
		}

		_chdir(pcCurPath);
	}
	else
	{
		rCB.GetErrorList().AddMsg("Error: Cannot create image variable.", iLine, iPos, CERR_INTERNAL, CEL_FATAL);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Save Bitmap Function

bool  SaveBMPFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	char pcCurPath[500];
	CStrMem csFilename;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount             = int(mVars.Count());
	TCVCounter uKey1          = 0, uKey2 = 0;
	TCVCounter uCompressLevel = 1;
	COGLVertex xA;
	bool bDoEncode = false;

	if ((iVarCount != 2) && (iVarCount != 4) && (iVarCount != 5))
	{
		int piPar[] = { 2, 4, 5 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 3, iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_IMAGE)
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	if (iVarCount >= 4)
	{
		if (!mVars(2).CastToCounter(uKey1))
		{
			rCB.GetErrorList().GeneralError("Expect third parameter to be an integer.", iLine, iPos);
			return false;
		}

		if (!mVars(3).CastToCounter(uKey2))
		{
			rCB.GetErrorList().GeneralError("Expect fourth parameter to be an integer.", iLine, iPos);
			return false;
		}

		bDoEncode = true;
	}

	if (iVarCount >= 5)
	{
		if (!mVars(4).CastToCounter(uCompressLevel))
		{
			rCB.GetErrorList().GeneralError("Expect fifth parameter to give compression level.", iLine, iPos);
			return false;
		}
	}

	TImage& Img = *mVars(1).GetImagePtr();
	csFilename = mVars(0).ValStr();

	string sFilename = csFilename.Str();

/*
        if (csFilename[1] != ':')
        {
                // Insert Path to currently loaded script before filename.
                sFilename = rCB.GetScriptPath() + sFilename;
        }
*/

	if (!Img.IsValid())
	{
		rCB.GetErrorList().GeneralError("Invalid Image.", iLine, iPos);
		return false;
	}

	_getcwd(pcCurPath, 499);
	_chdir(rCB.GetScriptPath().c_str());

	if (bDoEncode)
	{
		CEncode xEncode;
		string sFileID = "CLUImageRaw";

		if (!xEncode.WriteImage(sFilename, sFileID, *((COGLImage*) Img), uCompressLevel, uKey1, uKey2))
		{
			rVar = CStrMem("Error: Cannot save image");
			//rCB.GetErrorList().GeneralError("Error saving image.", iLine, iPos);
			//_chdir(pcCurPath);
			//return false;
		}
		else
		{
			rVar = CStrMem("OK");
		}
	}
	else
	{
		if (!Img->SaveImage(sFilename.c_str()))
		{
			rVar = CStrMem("Error: Cannot save image");
			//rCB.GetErrorList().GeneralError("Error saving image.", iLine, iPos);
			//_chdir(pcCurPath);
			//return false;
		}
		else
		{
			rVar = CStrMem("OK");
		}
	}

	_chdir(pcCurPath);

	return true;
}
