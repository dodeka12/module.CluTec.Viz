////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_File.cpp
//
// summary:   Implements the function file class
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

#include "CluTec.Viz.Fltk\fl_ask.H"
#include "CluTec.Viz.Fltk\Fl_File_Chooser.H"
#include "CluTec.Viz.Xml\XML.h"
#include "CluTec.Viz.Parse\Encode.h"

#include "CluTec.Base/Exception.h"

#ifdef _GNUCPP3_
    #include <unistd.h>
	#define _getcwd getcwd
	#define _chdir chdir
#else
    #include <direct.h>
#endif

#ifdef WIN32
    #include <ShellApi.h>
    #include <commdlg.h>
#endif

#include "CluTec.Viz.Parse\CLUCodeBase.h"
#include "CluTec.Viz.Base\TensorOperators.h"
// For the generation of the script dependency paths
#include "CluTec.Viz.Base\Environment.h"

#include <vector>
#include <algorithm>
#include <functional>		// For greater<int>( )
#include <string.h>
#include "Func_File.h"
#include <time.h>

using namespace std;

//////////////////////////////////////////////////////////////////////
/// Get Directory Listing
///
///	Parameters:
///		1. Search Path and pattern
///
///	Returns:
///		List of files found with complete Path
///

bool GetDirListFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	int iFile, iFileCnt, iDir, iDirCnt;
	string m_sFilename;
	vector<string> m_vecFile, m_vecDir;
	char pcCurPath[500];

	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount != 1)
	{
		int piPar[] = { 1 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("First parameter has to be the search path.", iLine, iPos);
		return false;
	}

	m_sFilename = mVars(0).GetStringPtr()->Str();

	_getcwd(pcCurPath, 499);
	_chdir(rCB.GetScriptPath().c_str());

	#ifdef WIN32
		WIN32_FIND_DATAA FindFileData;
		HANDLE hFind;

		if (m_sFilename.find_first_of(":") == std::string::npos)
		{
			m_sFilename = rCB.GetScriptPath() + m_sFilename;
		}

		hFind = FindFirstFileA(m_sFilename.c_str(), &FindFileData);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					m_vecDir.push_back(string(FindFileData.cFileName));
				}
				else
				{
					m_vecFile.push_back(string(FindFileData.cFileName));
				}
			}
			while (FindNextFileA(hFind, &FindFileData));

			FindClose(hFind);
		}

	#else

		// Add functionality for other OS here

	#endif

	_chdir(pcCurPath);

	iFileCnt = int(m_vecFile.size());
	iDirCnt  = int(m_vecDir.size());

	rVar.New(PDT_VARLIST);
	TVarList& rList = *rVar.GetVarListPtr();
	rList.Set(2);

	rList(0).New(PDT_VARLIST);
	TVarList& rFilePart = *rList(0).GetVarListPtr();
	rFilePart.Set(2);
	rFilePart(0) = "File";

	rFilePart(1).New(PDT_VARLIST);
	TVarList& rFileList = *rFilePart(1).GetVarListPtr();

	rList(1).New(PDT_VARLIST);
	TVarList& rDirPart = *rList(1).GetVarListPtr();
	rDirPart.Set(2);
	rDirPart(0) = "Dir";

	rDirPart(1).New(PDT_VARLIST);
	TVarList& rDirList = *rDirPart(1).GetVarListPtr();

	rFileList.Set(iFileCnt);
	for (iFile = 0; iFile < iFileCnt; ++iFile)
	{
		rFileList[iFile] = m_vecFile[iFile].c_str();
	}

	rDirList.Set(iDirCnt);
	for (iDir = 0; iDir < iDirCnt; ++iDir)
	{
		rDirList[iDir] = m_vecDir[iDir].c_str();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Write Variable Function
///
/// 1. Filename
/// 2. Variable (any type)
/// 3. XML Compression (0-9) (optional, default 0)
///		OR: String giving type of file [ "xml", "bin", "bin2" ]
///	4. (opt) "xml" : (int) xml compression rate;
///			 "bin" : (string) passwort for encoding bin;
///			 "bin2": (varlist) three integers giving [Key1,Key2,compression rate (0-9)], default is [0,0,1]
///
/// Returns string "OK" if data was written, otherwise an error message.

bool WriteVariableFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	enum EFileType { FT_NONE = 0, FT_XML, FT_BIN, FT_BIN2 } eFileType = FT_XML;
	bool bEncode = false;
	CStrMem csFilename;
	TString csPassword;
	TVarList& mVars         = *rPars.GetVarListPtr();
	TCVCounter iCompression = 0;
	TCVCounter iKey1        = 0, iKey2 = 0;

	int iVarCount = int(mVars.Count());

	if ((iVarCount < 2) || (iVarCount > 4))
	{
		int piPar[] = { 2, 3, 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 3, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("First parameter has to be a filename.", iLine, iPos);
		return false;
	}

	if (iVarCount >= 3)
	{
		if (mVars(2).CastToCounter(iCompression))
		{
			if ((iCompression < 0) || (iCompression > 9))
			{
				rCB.GetErrorList().GeneralError(
						"Compression value has to be in range 0 (no compression), "
						"to 9 (max. compression).", iLine, iPos);
				return false;
			}

			eFileType = FT_XML;
		}
		else if (mVars(2).BaseType() == PDT_STRING)
		{
			TString& csType = *mVars(2).GetStringPtr();

			if (csType == "xml")
			{
				eFileType = FT_XML;
			}
			else if (csType == "bin")
			{
				eFileType = FT_BIN;
			}
			else if (csType == "bin2")
			{
				eFileType = FT_BIN2;
			}
			else
			{
				rCB.GetErrorList().GeneralError("Unknown file type.", iLine, iPos);
				return false;
			}
		}
		else
		{
			rCB.GetErrorList().GeneralError("Third parameter has to be compression level or file type.", iLine, iPos);
			return false;
		}
	}

	if (iVarCount >= 4)
	{
		if (eFileType == FT_XML)
		{
			if (mVars(3).CastToCounter(iCompression))
			{
				if ((iCompression < 0) || (iCompression > 9))
				{
					rCB.GetErrorList().GeneralError(
							"Compression value has to be in range 0 (no compression), "
							"to 9 (max. compression).", iLine, iPos);
					return false;
				}
			}
			else
			{
				rCB.GetErrorList().GeneralError("Fourth parameter has to be compression level.", iLine, iPos);
				return false;
			}
		}
		else if (eFileType == FT_BIN)
		{
			if (!mVars(3).BaseType() == PDT_STRING)
			{
				rCB.GetErrorList().GeneralError("Fourth parameter has to be passphrase for encoding.", iLine, iPos);
				return false;
			}

			csPassword = *mVars(3).GetStringPtr();
			bEncode    = true;
		}
		else if (eFileType == FT_BIN2)
		{
			if (mVars(3).BaseType() != PDT_VARLIST)
			{
				rCB.GetErrorList().GeneralError("Fourth parameter has to be list containing [ Key1, Key2, Compression Level ].", iLine, iPos);
				return false;
			}

			bEncode = true;

			TVarList& rList = *mVars(3).GetVarListPtr();

			if (!rList(0).CastToCounter(iKey1))
			{
				rCB.GetErrorList().GeneralError("Fourth parameter has to be list containing [ Key1, Key2, Compression Level ].", iLine, iPos);
				return false;
			}

			if (!rList(1).CastToCounter(iKey2))
			{
				rCB.GetErrorList().GeneralError("Fourth parameter has to be list containing [ Key1, Key2, Compression Level ].", iLine, iPos);
				return false;
			}

			if (!rList(2).CastToCounter(iCompression))
			{
				rCB.GetErrorList().GeneralError("Fourth parameter has to be list containing [ Key1, Key2, Compression Level ].", iLine, iPos);
				return false;
			}
		}
		else
		{
			rCB.GetErrorList().GeneralError("Invalid file type.", iLine, iPos);
			return false;
		}
	}

	csFilename = mVars(0).ValStr();

	if (eFileType == FT_XML)
	{
		char pcCurPath[500];
		CXMLdom DOM;
		CXMLTree xmlTree;
		CXMLElement xmlEl;

		try
		{
			xmlEl.SetName("clu_data");
			xmlEl.AddProp("version", "1.0");

			WriteVariable(rCB, xmlEl.GetSubTree(), mVars(1), 0);

			xmlTree.AddElement(xmlEl);
			DOM.SetElementTree(xmlTree);

			_getcwd(pcCurPath, 499);
			_chdir(rCB.GetScriptPath().c_str());

			DOM.WriteFile(csFilename.Str(), iCompression);

			_chdir(pcCurPath);

			// Return with OK message to script
			rVar = "OK";
		}
		catch (CCluException& rEx)
		{
			rVar = rEx.PrintError().c_str();
		}
	}
	else if (eFileType == FT_BIN)
	{
		using std::ofstream;
		ofstream fsFile;
		char pcCurPath[500];
		Mem<char> mData;

		_getcwd(pcCurPath, 499);
		_chdir(rCB.GetScriptPath().c_str());

		try
		{
			mData.Add(7);
			char* pcData = mData.Data();

			memcpy(pcData, "CLUVAR", 6);
			pcData[6] = 1;

			// Construct buffer containing the data
			WriteVariable(rCB, mData, mVars(1), 0);

			if (bEncode)
			{
				CEncode Encode;
				char* pcPass = 0;

				if (csPassword.Len())
				{
					pcPass = csPassword.Str();
				}

				if (!Encode.WriteFile(csFilename.Str(), "CLUData", mData.Data(), mData.Count(), pcPass))
				{
					rVar = Encode.GetError().c_str();
				}
				else
				{
					// Return with OK message to script
					rVar = "OK";
				}
			}
			else
			{
				fsFile.open(csFilename.Str(), ios_base::out | ios_base::trunc | ios_base::binary);

				if (fsFile.is_open())
				{
					//char pcVersion[] = { 0x01 };
					//fsFile.write( "CLUVAR", 6 );
					//fsFile.write( pcVersion, 1 );

					// Write Data
					fsFile.write(mData.Data(), mData.Count());

					fsFile.close();

					// Return with OK message to script
					rVar = "OK";
				}
				else
				{
					rVar = "Could not open file for writing.";
				}
			}
		}
		catch (CCluException& rEx)
		{
			rVar = rEx.PrintError().c_str();
		}

		_chdir(pcCurPath);
	}
	else if (eFileType == FT_BIN2)
	{
		char pcCurPath[500];
		Mem<char> mData;

		_getcwd(pcCurPath, 499);
		_chdir(rCB.GetScriptPath().c_str());

		try
		{
			CEncode xEncode;

			mData.Add(7);
			char* pcData = mData.Data();

			memcpy(pcData, "CLUVAR", 6);
			pcData[6] = 1;

			// Construct buffer containing the data
			WriteVariable(rCB, mData, mVars(1), 0);

			//ensure we can safely cast to ULONG
			if (mData.Count() > ULONG_MAX)
			{
				return false;
			}
			// Encode data and write to file
			if (!xEncode.WriteFile_v2(csFilename.Str(), "CLUData2", mData.Data(), (unsigned int) (mData.Count()), iCompression, iKey1, iKey2))
			{
				rVar = xEncode.GetError().c_str();
			}
			else
			{
				// Return with OK message to script
				rVar = "OK";
			}
		}
		catch (CCluException& rEx)
		{
			rVar = rEx.PrintError().c_str();
		}

		_chdir(pcCurPath);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Write a single variable
///
/// iPIL (position in list) gives position in varlist.

#define ADD_VAR_START(theSize) \
	mData.Add((uAddSize = theSize) + 1); \
	pcData  = &mData.Last() - uAddSize; \
	*pcData = cVarType; \
	++pcData;

#define ADD_VAR_EXT(theSize) \
	mData.Add(uAddSize = theSize); \
	pcData = &mData.Last() - (uAddSize - 1);

#define ADD_VAL(thePtr, theSize) \
	memcpy((void*) pcData, (void*) thePtr, theSize); \
	pcData += theSize;

void WriteVariable(CCLUCodeBase& rCB, Mem<char>& mData, CCodeVar& rVar, int iPIL)
throw(CCluException)
{
	ECodeDataType eVarType = rVar.BaseType();
	uchar cVarType         = uchar(eVarType);
	uint uAddSize;
	char* pcData;

	if (eVarType == PDT_SCALAR)
	{
		TCVScalar& dVal = *rVar.GetScalarPtr();

		ADD_VAR_START(sizeof(TCVScalar))
		ADD_VAL(&dVal, sizeof(TCVScalar))

		//fsFile.write( (char *) &cVarType, 1 );
		//fsFile.write( (char *) &dVal, sizeof( TCVScalar ) );
	}
	else if (eVarType == PDT_COUNTER)
	{
		TCVCounter& iVal = *rVar.GetCounterPtr();

		ADD_VAR_START(sizeof(TCVCounter))
		ADD_VAL(&iVal, sizeof(TCVCounter))

		//fsFile.write( (char *) &cVarType, 1 );
		//fsFile.write( (char *) &iVal, sizeof( TCVCounter ) );
	}
	else if (eVarType == PDT_STRING)
	{
		TString& sVal = *rVar.GetStringPtr();
		uint uLen     = uint(sVal.Len());

		ADD_VAR_START(sizeof(uint) + uLen)
		ADD_VAL(&uLen, sizeof(uint))
		ADD_VAL(sVal.Str(), uLen)

		//fsFile.write( (char *) &cVarType, 1 );
		//fsFile.write( (char *) &uLen, sizeof( uint ) );
		//fsFile.write( (char *) sVal.Str(), uLen );
	}
	else if (eVarType == PDT_MATRIX)
	{
		uint uRows, uCols, uCount;
		TMatrix& xA = *rVar.GetMatrixPtr();

		uRows  = xA.Rows();
		uCols  = xA.Cols();
		uCount = uRows * uCols;
		TCVScalar* pData = xA.Data();

		ADD_VAR_START(2 * sizeof(uint) + uCount * sizeof(TCVScalar))
		ADD_VAL(&uRows, sizeof(uint))
		ADD_VAL(&uCols, sizeof(uint))
		ADD_VAL(pData, uCount * sizeof(TCVScalar))

		//fsFile.write( (char *) &uRows, sizeof( uint ) );
		//fsFile.write( (char *) &uCols, sizeof( uint ) );
		//fsFile.write( (char *) pData, uCount * sizeof( TCVScalar ) );
	}
	else if ((eVarType == PDT_TENSOR) ||
		 (eVarType == PDT_TENSOR_IDX))
	{
		TTensor* pT, T;

		if (eVarType == PDT_TENSOR_IDX)
		{
			TTensorIdx& rTIdx = *rVar.GetTensorIdxPtr();

			if (!MakeTensor(T, rTIdx))
			{
				throw CCluError("Error creating tensor from index");
			}

			pT = &T;
		}
		else
		{
			pT = rVar.GetTensorPtr();
		}

		TTensor& rT = *pT;

		int iDim, iValence = rT.Valence();
		int iSize = rT.Size();
		int iVal;
		TCVScalar* pData = rT.Data();

		cVarType = uchar(PDT_TENSOR);

		ADD_VAR_START((iValence + 1) * sizeof(int) + uint(iSize) * sizeof(TCVScalar))
		ADD_VAL(&iValence, sizeof(int))

		//fsFile.write( (char *) &cVarType, 1 );
		//fsFile.write( (char *) &iValence, sizeof( int ) );

		for (iDim = 0; iDim < iValence; iDim++)
		{
			iVal = rT.DimSize(iDim);
			ADD_VAL(&iVal, sizeof(int))

			//fsFile.write( (char *) &iVal, sizeof( int ) );
		}

		ADD_VAL(pData, uint(iSize) * sizeof(TCVScalar))
		//fsFile.write( (char *) pData, uint( iSize ) * sizeof( TCVScalar ) );
	}
	else if (eVarType == PDT_MULTIV)
	{
		int iPos, iCount, iBID;
		TMultiV& vA = *rVar.GetMultiVPtr();

		if (vA.BasePtr() == 0)
		{
			throw CCluError("Uninitialized multivector cannot be stored");
		}

		iCount = (int) vA.GetGADim();
		iBID   = vA.BasePtr()->BaseID();

		ADD_VAR_START(2 * sizeof(int) + iCount * sizeof(TCVScalar))
		ADD_VAL(&iBID, sizeof(int))
		ADD_VAL(&iCount, sizeof(int))

		for (iPos = 0; iPos < iCount; iPos++)
		{
			ADD_VAL(&vA[iPos], sizeof(TCVScalar))
		}
	}
	else if (eVarType == PDT_COLOR)
	{
		TOGLColor& rCol = *rVar.GetOGLColorPtr();

		ADD_VAR_START(4 * sizeof(float))
		ADD_VAL(rCol.Data(), 4 * sizeof(float))
	}
	else if (eVarType == PDT_VARLIST)
	{
		TVarList& rList = *rVar.GetVarListPtr();
		int iVar, iCount = int(rList.Count());

		ADD_VAR_START(sizeof(int))
		ADD_VAL(&iCount, sizeof(int))

		for (iVar = 0; iVar < iCount; iVar++)
		{
			WriteVariable(rCB,  mData, rList(iVar), iVar);
		}
	}
	else if (eVarType == PDT_SCENE /* Object */)
	{
		TScene& rScene = *rVar.GetScenePtr();

		if (!rScene.IsValid())
		{
			throw CCluError("Invalid scene");
		}

		if (rScene->GetTypeName() != "Object")
		{
			throw CCluError("Cannot write scene");
		}

		TVexList* pList = dynamic_cast< TVexList* >(((COGLBaseElement*) rScene));

		if (!pList)
		{
			throw CCluError("Given scene does not represent a vertex list");
		}

		uint uPos;
		int piLineStipple[2];
		float pfPOF[2], fAngle, fAlpha, fBackColorScale;
		bool bUsePolygonMode, bUseScaling, bUseTrans, bUseOverrideAlpha;
		bool bUseLineStipple, bUseScaleBackColor;
		GLenum pePolyMode[2];
		COGLVertex vexScale, vexAxis, vexTrans1, vexTrans2;
		TVexList& rList = *pList;

		string sName = rList.GetName();
		uint uSize   = uint(sName.size());
		GLenum eType = rList.GetMode();

		rList.GetPolygonOffsetFill(pfPOF[0], pfPOF[1]);

		bUsePolygonMode = rList.UsePolygonMode();
		rList.GetPolygonMode(pePolyMode[0], pePolyMode[1]);

		bUseScaling = rList.UseScaling();
		rList.GetScaling(vexScale);

		bUseTrans = rList.UseTransform();
		rList.GetRotation(fAngle, vexAxis);
		rList.GetTranslation1(vexTrans1);
		rList.GetTranslation2(vexTrans2);

		bUseOverrideAlpha = rList.UseOverrideAlpha();
		rList.GetOverrideAlpha(fAlpha);

		bUseLineStipple = rList.UseLineStipple();
		rList.GetLineStipple(piLineStipple[0], piLineStipple[1]);

		bUseScaleBackColor = rList.UseScaleBackColor();
		rList.GetBackColorScale(fBackColorScale);

		uint uVexCnt  = rList.GetVexCount();
		uint uNormCnt = rList.GetNormCount();
		uint uTexCnt  = rList.GetTexCount();
		uint uColCnt  = rList.GetColCount();
		uint uIdxCnt  = uint(rList.GetIdxList().Count());

		ADD_VAR_START(uSize + sizeof(uint)	// Name
				+ sizeof(GLenum)				// Vertex List Type
				+ 2 * sizeof(float)			// Polygon Offset Fill
				+ sizeof(bool)					// Is Polygon Mode
				+ 2 * sizeof(GLenum)			// Polygon Mode
				+ sizeof(bool)					// Is Scaling
				+ 3 * sizeof(float)			// Scale
				+ sizeof(bool)					// Use Transform
				+ sizeof(float)					// Rotation angle
				+ 9 * sizeof(float)			// Rotation axis, Translation1, Translation2
				+ sizeof(bool)					// Use Override Alpha
				+ sizeof(float)					// Alpha value
				+ sizeof(bool)					// Use Line Stipple
				+ 2 * sizeof(int)				// Line Stipple Data
				+ sizeof(bool)					// Use Back Color Scale
				+ sizeof(float)					// Back Color Scale
				+ 5 * sizeof(uint)			// List Counts
				+ (uVexCnt + uNormCnt + uTexCnt) * 3 * sizeof(float)
				+ uColCnt * 4 * sizeof(float));

		ADD_VAL(&uSize, sizeof(uint))
		ADD_VAL(sName.c_str(), uSize)
		ADD_VAL(&eType, sizeof(GLenum))

		ADD_VAL(pfPOF, 2 * sizeof(float))

		ADD_VAL(&bUsePolygonMode, sizeof(bool))
		ADD_VAL(pePolyMode, 2 * sizeof(GLenum))

		ADD_VAL(&bUseScaling, sizeof(bool))
		ADD_VAL(((float*) vexScale), 3 * sizeof(float))

		ADD_VAL(&bUseTrans, sizeof(bool))
		ADD_VAL(&fAngle, sizeof(float))
		ADD_VAL(((float*) vexAxis), 3 * sizeof(float))
		ADD_VAL(((float*) vexTrans1), 3 * sizeof(float))
		ADD_VAL(((float*) vexTrans2), 3 * sizeof(float))

		ADD_VAL(&bUseOverrideAlpha, sizeof(bool))
		ADD_VAL(&fAlpha, sizeof(float))

		ADD_VAL(&bUseLineStipple, sizeof(bool))
		ADD_VAL(piLineStipple, 2 * sizeof(int))

		ADD_VAL(&bUseScaleBackColor, sizeof(bool))
		ADD_VAL(&fBackColorScale, sizeof(float))

		ADD_VAL(&uVexCnt, sizeof(uint))
		ADD_VAL(&uNormCnt, sizeof(uint))
		ADD_VAL(&uTexCnt, sizeof(uint))
		ADD_VAL(&uColCnt, sizeof(uint))
		ADD_VAL(&uIdxCnt, sizeof(uint))

		if (uVexCnt > 0)
		{
			for (uPos = 0; uPos < uVexCnt; uPos++)
			{
				ADD_VAL(((float*) rList[uPos].xVex), 3 * sizeof(float))
			}
		}

		if (uNormCnt > 0)
		{
			for (uPos = 0; uPos < uNormCnt; uPos++)
			{
				ADD_VAL(((float*) rList[uPos].xNorm), 3 * sizeof(float))
			}
		}

		if (uTexCnt > 0)
		{
			for (uPos = 0; uPos < uTexCnt; uPos++)
			{
				ADD_VAL(((float*) rList[uPos].xTex), 3 * sizeof(float))
			}
		}

		if (uColCnt > 0)
		{
			for (uPos = 0; uPos < uColCnt; uPos++)
			{
				ADD_VAL(rList[uPos].xCol.Data(), 4 * sizeof(float))
			}
		}

		if (uIdxCnt > 0)
		{
			COGLVertexList::TIdxList& mIdxList = rList.GetIdxList();

			for (uPos = 0; uPos < uIdxCnt; uPos++)
			{
				Mem<unsigned>& mIdx = mIdxList[uPos];
				uint uCnt           = uint(mIdx.Count());

				ADD_VAR_EXT((uCnt + 1) * sizeof(unsigned))
				ADD_VAL(&uCnt, sizeof(unsigned))
				ADD_VAL(mIdx.Data(), uCnt * sizeof(unsigned))
			}
		}
	}
	else if (eVarType == PDT_IMAGE)
	{
		TImage& rImage = *rVar.GetImagePtr();

		if (!rImage.IsValid())
		{
			throw CCluError("Invalid image");
		}

		unsigned uByteCnt;
		int iWidth, iHeight;
		int iImgType, iDataType, iBPP;

		rImage->GetSize(iWidth, iHeight);
		rImage->GetType(iImgType, iDataType, iBPP);

		uByteCnt = unsigned(iWidth) * unsigned(iHeight) * unsigned(iBPP);

		ADD_VAR_START(5 * sizeof(int) + uByteCnt);
		ADD_VAL(&iWidth, sizeof(int))
		ADD_VAL(&iHeight, sizeof(int))
		ADD_VAL(&iImgType, sizeof(int))
		ADD_VAL(&iDataType, sizeof(int))
		ADD_VAL(&iBPP, sizeof(int))
		::LockImageAccess();
		ADD_VAL(rImage->GetDataPtr(), uByteCnt)
		::UnlockImageAccess();
	}
	else
	{
		char pcText[500];

		sprintf_s(pcText, "Writing to file of variable type '%s' is not supported.",
				rVar.TypeStr(rVar.Type()).c_str());

		throw CCluError(pcText);
	}
}

//////////////////////////////////////////////////////////////////////
/// Write a single variable
///
/// iPIL (position in list) gives position in varlist.

void WriteVariable(CCLUCodeBase& rCB, CXMLTree& rTree, CCodeVar& rVar, int iPIL)
throw(CCluException)
{
	char pcText[100];
	string sError;
	CXMLElement rEl;

	rEl.SetName("var");
	if (rVar.BaseType() == PDT_TENSOR_IDX)
	{
		rEl.AddProp("type", rVar.TypeStr(PDT_TENSOR));
	}
	else
	{
		rEl.AddProp("type", rVar.TypeStr(rVar.Type()));
	}
	//rEl.AddProp( "pil", iPIL + 1);

	if (rVar.BaseType() == PDT_SCALAR)
	{
		TCVScalar dVal;
		dVal = *rVar.GetScalarPtr();

		sprintf_s(pcText, "%g", dVal);
		rEl.SetContent(pcText);
	}
	else if (rVar.BaseType() == PDT_COUNTER)
	{
		TCVCounter iVal;
		iVal = *rVar.GetCounterPtr();

		sprintf_s(pcText, "%d", iVal);
		rEl.SetContent(pcText);
	}
	else if (rVar.BaseType() == PDT_STRING)
	{
		rEl.SetContent(rVar.ValStr().Str());
	}
	else if (rVar.BaseType() == PDT_MATRIX)
	{
		int iRows, iCols, iPos, iCount;
		string sData;
		TCVScalar* pData;
		TMatrix& xA = *rVar.GetMatrixPtr();

		iRows  = xA.Rows();
		iCols  = xA.Cols();
		iCount = iRows * iCols;
		pData  = xA.Data();

		rEl.AddProp("rows", iRows);
		rEl.AddProp("cols", iCols);

		for (iPos = 0; iPos < iCount; iPos++)
		{
			sprintf_s(pcText, "%.17lg ", pData[iPos]);
			sData += pcText;
		}

		rEl.SetContent(sData);
	}
	else if ((rVar.BaseType() == PDT_TENSOR) ||
		 (rVar.BaseType() == PDT_TENSOR_IDX))
	{
		string sData;
		TTensor* pT, T;

		if (rVar.BaseType() == PDT_TENSOR_IDX)
		{
			TTensorIdx& rTIdx = *rVar.GetTensorIdxPtr();
			if (!MakeTensor(T, rTIdx))
			{
				throw CCluError("Error creating tensor from index");
			}

			pT = &T;
		}
		else
		{
			pT = rVar.GetTensorPtr();
		}

		TTensor& rT = *pT;

		int iDim, iValence = rT.Valence();
		int iPos, iSize = rT.Size();
		TCVScalar* pData = rT.Data();
		//rEl.AddProp("valence", iValence);

		for (iDim = 0; iDim < iValence; iDim++)
		{
			sprintf_s(pcText, "%d ", rT.DimSize(iDim));
			sData += pcText;
		}

		rEl.AddProp("dim", sData);

		sData = "";
		for (iPos = 0; iPos < iSize; iPos++)
		{
			sprintf_s(pcText, "%.17lg ", pData[iPos]);
			sData += pcText;
		}

		rEl.SetContent(sData);
	}
	else if (rVar.BaseType() == PDT_MULTIV)
	{
		int iPos, iCount, iBID;
		string sData, sBID, sIdx;
		TCVScalar dVal;
		TMultiV& vA = *rVar.GetMultiVPtr();

		if (vA.BasePtr() == 0)
		{
			throw CCluError("Uninitialized multivector cannot be stored");
		}

		iCount = (int) vA.GetGADim();
		iBID   = vA.BasePtr()->BaseID();

		if (iBID == ID_E3GA)
		{
			sBID = "E3";
		}
		#ifdef USE_GA_E8
			else if (iBID == ID_E8GA)
			{
				sBID = "E8";
			}
		#endif
		else if (iBID == ID_PGA)
		{
			sBID = "P3";
		}
		else if (iBID == ID_EL2GA)
		{
			sBID = "C2";
		}
		else if (iBID == ID_CONFGA)
		{
			sBID = "N3";
		}

		rEl.AddProp("bid", sBID);

		for (iPos = 0; iPos < iCount; iPos++)
		{
			dVal = vA[iPos];
			if (!::IsZero(dVal, rCB.GetSensitivity()))
			{
				sprintf_s(pcText, "%d ", iPos);
				sIdx += pcText;

				sprintf_s(pcText, "%.17lg ", dVal);
				sData += pcText;
			}
		}

		rEl.AddProp("idx", sIdx);
		rEl.SetContent(sData);
	}
	else if (rVar.BaseType() == PDT_VARLIST)
	{
		TVarList& rList = *rVar.GetVarListPtr();
		int iVar, iCount = int(rList.Count());

		//rEl.AddProp( "size", iCount);

		for (iVar = 0; iVar < iCount; iVar++)
		{
			WriteVariable(rCB, rEl.GetSubTree(), rList(iVar), iVar);
		}
	}
	else if (rVar.BaseType() == PDT_VEXLIST)
	{
		CXMLTree& rElTree = rEl.GetSubTree();
		TVexList& rList   = *rVar.GetVexListPtr();
		GLenum eType      = rList.GetMode();

		switch (eType)
		{
		case GL_POINTS:
			rEl.AddProp("mode", "points");
			break;

		case GL_LINES:
			rEl.AddProp("mode", "lines");
			break;

		case GL_LINE_STRIP:
			rEl.AddProp("mode", "line_strip");
			break;

		case GL_LINE_LOOP:
			rEl.AddProp("mode", "line_loop");
			break;

		case GL_TRIANGLES:
			rEl.AddProp("mode", "triangles");
			break;

		case GL_TRIANGLE_STRIP:
			rEl.AddProp("mode", "triangle_strip");
			break;

		case GL_TRIANGLE_FAN:
			rEl.AddProp("mode", "triangle_fan");
			break;

		case GL_QUADS:
			rEl.AddProp("mode", "quads");
			break;

		case GL_QUAD_STRIP:
			rEl.AddProp("mode", "quad_strip");
			break;

		case GL_POLYGON:
			rEl.AddProp("mode", "polygon");
			break;

		default:
			sError = "Unknown vertex list type.";

			rEl.SetContent(sError);
			rTree.AddElement(rEl);

			throw CCluError(sError.c_str());
/*
                case GL_:
                        rEl.AddProp("mode", "");
                        break;
*/
		}

		CXMLElement xmlVex, xmlTex, xmlNorm, xmlCol, xmlIdx;
		string sData;
		int iPos, iSize;

		xmlVex.SetName("vertices");
		xmlTex.SetName("texture_coordinates");
		xmlNorm.SetName("normals");
		xmlCol.SetName("colors");
		xmlIdx.SetName("indices");

		iSize = rList.GetVexCount();
		if (iSize > 0)
		{
			xmlVex.AddProp("count", iSize);

			sData = "";
			for (iPos = 0; iPos < iSize; iPos++)
			{
				COGLVertex& xVex = rList[iPos].xVex;

				sprintf_s(pcText, "%.7g %.7g %.7g ", xVex[0], xVex[1], xVex[2]);
				sData += pcText;
			}

			xmlVex.SetContent(sData);
			rElTree.AddElement(xmlVex);
		}

		iSize = rList.GetNormCount();
		if (iSize > 0)
		{
			xmlNorm.AddProp("count", iSize);

			sData = "";
			for (iPos = 0; iPos < iSize; iPos++)
			{
				COGLVertex& xVex = rList[iPos].xNorm;

				sprintf_s(pcText, "%.7g %.7g %.7g ", xVex[0], xVex[1], xVex[2]);
				sData += pcText;
			}

			xmlNorm.SetContent(sData);
			rElTree.AddElement(xmlNorm);
		}

		iSize = rList.GetTexCount();
		if (iSize > 0)
		{
			xmlTex.AddProp("count", iSize);

			sData = "";
			for (iPos = 0; iPos < iSize; iPos++)
			{
				COGLVertex& xVex = rList[iPos].xTex;

				sprintf_s(pcText, "%.7g %.7g %.7g ", xVex[0], xVex[1], xVex[2]);
				sData += pcText;
			}

			xmlTex.SetContent(sData);
			rElTree.AddElement(xmlTex);
		}

		iSize = rList.GetColCount();
		if (iSize > 0)
		{
			xmlCol.AddProp("count", iSize);

			sData = "";
			for (iPos = 0; iPos < iSize; iPos++)
			{
				TColor& rData = rList[iPos].xCol;

				sprintf_s(pcText, "%.4g %.4g %.4g %.4g ", rData[0], rData[1], rData[2], rData[3]);
				sData += pcText;
			}

			xmlCol.SetContent(sData);
			rElTree.AddElement(xmlCol);
		}

		iSize = int(rList.GetIdxList().Count());
		if (iSize > 0)
		{
			xmlIdx.AddProp("count", iSize);

			sData = "";
			for (iPos = 0; iPos < iSize; iPos++)
			{
				sprintf_s(pcText, "%d ", rList.GetIndex(0, iPos));
				sData += pcText;
			}

			xmlIdx.SetContent(sData);
			rElTree.AddElement(xmlIdx);
		}
	}
	else
	{
		sError  = "Writing variables of type '";
		sError += rVar.TypeStr(rVar.Type());
		sError += "' is not implemented, yet.";

		rEl.SetContent(sError);
		rTree.AddElement(rEl);

		throw CCluError(sError.c_str());
	}

	rTree.AddElement(rEl);
}

//////////////////////////////////////////////////////////////////////
/// Read Variable Function
///
/// 1. Filename
/// 2. Variable in which to write result (any type)
/// 3. (opt) File type to read from: [ "xml" | "bin" | "bin2" ]
/// 4. (opt) "bin" : (string)  Encoding password for "bin" type
///			 "bin2": (varlist) List containing keys [ key1, key2 ]
///
/// Returns string "OK" if data was read, otherwise an error message.

bool ReadVariableFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	enum EFileType { FT_NONE = 0, FT_XML, FT_BIN, FT_BIN2 } eFileType = FT_XML;

	CStrMem csFilename, csPass;
	TCVCounter iKey1 = 0, iKey2 = 0;
	TVarList& mVars  = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if ((iVarCount < 2) || (iVarCount > 4))
	{
		int piPar[] = { 2, 3, 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("First parameter has to be filename.", iLine, iPos);
		return false;
	}

	if (mVars(1).IsProtected())
	{
		rCB.GetErrorList().GeneralError("Cannot modify variable given as second parameter.", iLine, iPos);
		return false;
	}

	if (iVarCount >= 3)
	{
		if (mVars(2).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().GeneralError("Expect as third parameter the file type to read from.", iLine, iPos);
			return false;
		}

		TString& csType = *mVars(2).GetStringPtr();

		if (csType == "xml")
		{
			eFileType = FT_XML;
		}
		else if (csType == "bin")
		{
			eFileType = FT_BIN;
		}
		else if (csType == "bin2")
		{
			eFileType = FT_BIN2;
		}
		else
		{
			rCB.GetErrorList().GeneralError("Invalid file type.", iLine, iPos);
			return false;
		}
	}

	if (iVarCount >= 4)
	{
		if (eFileType == FT_BIN)
		{
			if (mVars(3).BaseType() != PDT_STRING)
			{
				rCB.GetErrorList().GeneralError("Expect as fourth parameter the pass-phrase if file is encoded.", iLine, iPos);
				return false;
			}

			csPass = *mVars(3).GetStringPtr();
		}
		else if (eFileType == FT_BIN2)
		{
			if (mVars(3).BaseType() != PDT_VARLIST)
			{
				rCB.GetErrorList().GeneralError("Expect as fourth parameter list of keys [ key1, key2 ].", iLine, iPos);
				return false;
			}

			TVarList& rList = *mVars(3).GetVarListPtr();

			if (!rList(0).CastToCounter(iKey1))
			{
				rCB.GetErrorList().GeneralError("Expect as fourth parameter list of keys [ key1, key2 ].", iLine, iPos);
				return false;
			}

			if (!rList(1).CastToCounter(iKey2))
			{
				rCB.GetErrorList().GeneralError("Expect as fourth parameter list of keys [ key1, key2 ].", iLine, iPos);
				return false;
			}
		}
	}

	csFilename = mVars(0).ValStr();

	std::string sFilename;
	// Use a standard search pattern to look for resource file in various directories.
	Clu::Viz::CEnvironment::FindResourceFile(sFilename, rCB.GetScriptPath(), rCB.GetScriptName(), csFilename.Str());
	csFilename = sFilename.c_str();

	if (eFileType == FT_XML)
	{
		string sError, sVersion;
		char pcCurPath[500];
		CXMLdom xmlDOM;
		CXMLElement xmlEl;

		_getcwd(pcCurPath, 499);
		_chdir(rCB.GetScriptPath().c_str());

		try
		{
			xmlDOM.ParseFile(csFilename.Str());
			xmlDOM.GetElementTree(xmlEl);
			CXMLTree& xmlTree = xmlEl.GetSubTree();

			try
			{
				xmlTree.GetElementList("clu_data");
			}
			catch (...)
			{
				throw CCluError("No CLU data found in file");
			}

			TXMLElementList& xmlRootList = xmlTree.GetElementList("clu_data");
			CXMLElement& xmlEl           = xmlRootList[0];

			sVersion = xmlEl.GetProp("version");
			if (sVersion != "1.0")
			{
				sError = "Cannot read data files of version " + sVersion + ".";
				throw CCluError(sError.c_str());
			}

			TXMLElementList& rElList = xmlRootList[0].GetSubTree().GetElementList("var");
			ReadVariable(rCB, mVars(1), rElList[0]);

			// Return with OK message to script
			rVar = "OK";
		}
		catch (CCluException& rEx)
		{
			rVar = rEx.PrintError().c_str();
		}

		_chdir(pcCurPath);
	}
	else if (eFileType == FT_BIN)
	{
		using std::ifstream;

		CEncode Encode;
		Mem<char> mData, mBuf;
		ifstream fsFile;
		string sFileID;
		char pcCurPath[500];
		const unsigned int cuReadLen = 4096;
		bool bDataRead               = false;

		_getcwd(pcCurPath, 499);
		_chdir(rCB.GetScriptPath().c_str());

		// Check whether file is encoded
		Encode.ReadFileID(csFilename.Str(), sFileID);
		if (sFileID == "CLUData")
		{
			char* pcPass = 0;

			if (csPass.Len())
			{
				pcPass = csPass.Str();
			}

			if (!Encode.ReadFile(csFilename.Str(), sFileID, mData, pcPass))
			{
				rVar = Encode.GetError().c_str();
			}
			else
			{
				bDataRead = true;
			}
		}
		else
		{
			fsFile.open(csFilename.Str(), ios_base::in | ios_base::binary);

			if (fsFile.is_open())
			{
				mBuf.Set(cuReadLen);
				char* pcBuf = mBuf.Data();
				unsigned int uReadCnt, uReadPos = 0;

				do
				{
					fsFile.read(pcBuf, cuReadLen);
					uReadCnt = (unsigned int) fsFile.gcount();

					if (uReadCnt)
					{
						mData.Add(uReadCnt);
						memcpy(&mData[uReadPos], pcBuf, uReadCnt);
						uReadPos += uReadCnt;
					}
				}
				while (!fsFile.eof());

				fsFile.close();

				bDataRead = true;
			}
			else
			{
				rVar = "Could not open file for reading.";
			}
		}

		_chdir(pcCurPath);

		if (bDataRead)
		{
			char pcFileID[7];
			char cVersion;

			pcFileID[6] = 0;
			memcpy(pcFileID, mData.Data(), 6);

			cVersion = mData[6];

			if ((strcmp(pcFileID, "CLUVAR") == 0) &&
			    (cVersion == 0x01))
			{
				try
				{
					char* pcData = &mData[7];
					ReadVariable(rCB, mVars(1), &pcData);

					// Return with OK message to script
					rVar = "OK";
				}
				catch (CCluException& rEx)
				{
					rVar = rEx.PrintError().c_str();
				}
			}
			else
			{
				rVar = "Specified file is does not contain a CLUScript variable.";
			}
		}
	}
	else if (eFileType == FT_BIN2)
	{
		CEncode xEncode;
		string sFileID;
		Mem<char> mData, mBuf;
		char pcCurPath[500];

		_getcwd(pcCurPath, 499);
		_chdir(rCB.GetScriptPath().c_str());

		// Check whether file is encoded
		xEncode.ReadFileID(csFilename.Str(), sFileID);
		if (sFileID == "CLUData2")
		{
			if (!xEncode.ReadFile_v2(csFilename.Str(), sFileID, mData, iKey1, iKey2))
			{
				rVar = xEncode.GetError().c_str();
			}
			else
			{
				char pcFileID[7];
				char cVersion;

				pcFileID[6] = 0;
				memcpy(pcFileID, mData.Data(), 6);

				cVersion = mData[6];

				if ((strcmp(pcFileID, "CLUVAR") == 0) &&
				    (cVersion == 0x01))
				{
					try
					{
						char* pcData = &mData[7];
						ReadVariable(rCB, mVars(1), &pcData);

						// Return with OK message to script
						rVar = "OK";
					}
					catch (CCluException& rEx)
					{
						rVar = rEx.PrintError().c_str();
					}
				}
				else
				{
					rVar = "Specified file is does not contain a CLUScript variable.";
				}
			}
		}
		else
		{
			rVar = "Invalid file type";
		}

		_chdir(pcCurPath);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Read a single binary variable

#define GET_VAL(thePtr, theSize) \
	memcpy((void*) thePtr, (void*) *ppcData, theSize); \
	*ppcData += theSize;

void ReadVariable(CCLUCodeBase& rCB, CCodeVar& rVar, char** ppcData)
throw(CCluException)
{
	ECodeDataType eVarType;
	uchar cVarType;

	GET_VAL(&cVarType, 1)
	//fsFile.read( (char *) &cVarType, 1 );
	eVarType = ECodeDataType(cVarType);

	if (eVarType == PDT_SCALAR)
	{
		rVar.New(PDT_SCALAR, rVar.Name().c_str());
		TCVScalar& dVal = *rVar.GetScalarPtr();

		GET_VAL(&dVal, sizeof(TCVScalar))
		//fsFile.read( (char *) &dVal, sizeof( TCVScalar ) );
	}
	else if (eVarType == PDT_COUNTER)
	{
		rVar.New(PDT_COUNTER, rVar.Name().c_str());
		TCVCounter& iVal = *rVar.GetCounterPtr();

		GET_VAL(&iVal, sizeof(TCVCounter))
		//fsFile.read( (char *) &iVal, sizeof( TCVCounter ) );
	}
	else if (eVarType == PDT_STRING)
	{
		rVar.New(PDT_STRING, rVar.Name().c_str());
		TString& sVal = *rVar.GetStringPtr();
		uint uLen;
		char* pcString;

		GET_VAL(&uLen, sizeof(uint))
		//fsFile.read( (char *) &uLen, sizeof( uint ) );

		if (!(pcString = new char[uLen + 1]))
		{
			throw CCluError("Out of memory while reading string");
		}

		GET_VAL(pcString, uLen)
		//fsFile.read( (char *) pcString, uLen );
		pcString[uLen] = 0;

		sVal = pcString;

		delete[] pcString;
	}
	else if (eVarType == PDT_MATRIX)
	{
		rVar.New(PDT_MATRIX, rVar.Name().c_str());
		TMatrix& xVal = *rVar.GetMatrixPtr();
		uint uRows, uCols;

		GET_VAL(&uRows, sizeof(uint));
		GET_VAL(&uCols, sizeof(uint));
		//fsFile.read( (char *) &uRows, sizeof( uint ) );
		//fsFile.read( (char *) &uCols, sizeof( uint ) );

		xVal.Resize(uRows, uCols);
		TCVScalar* pData = xVal.Data();

		GET_VAL(pData, uRows * uCols * sizeof(TCVScalar))
		//fsFile.read( (char *) pData, uRows * uCols * sizeof( TCVScalar ) );
	}
	else if (eVarType == PDT_TENSOR)
	{
		rVar.New(PDT_TENSOR, rVar.Name().c_str());
		TTensor& tVal = *rVar.GetTensorPtr();

		int iValence, iSize = 1;
		int iDim;

		GET_VAL(&iValence, sizeof(int))
		//fsFile.read( (char *) &iValence, sizeof( int ) );

		if (iValence > 0)
		{
			Mem<int> mDims;
			if (!mDims.Set(iValence))
			{
				throw CCluError("Out of memory while reading tensor");
			}

			for (iDim = 0; iDim < iValence; ++iDim)
			{
				GET_VAL(&mDims[iDim], sizeof(int))
				//fsFile.read( (char *) &mDims[ iDim ], sizeof( int ) );

				if (mDims[iDim] < 0)
				{
					throw CCluError("Negative tensor dimension. File is probably corrupted");
				}

				iSize *= mDims[iDim];
			}

			tVal.Reset(mDims);

			if (iSize > 0)
			{
				GET_VAL(tVal.Data(), iSize * sizeof(TCVScalar))
				//fsFile.read( (char *) tVal.Data(), iSize * sizeof( TCVScalar ) );
			}
		}
		else if (iValence < 0)
		{
			throw CCluError("Valence of tensor is negative. File is probably corrupted");
		}
	}
	else if (eVarType == PDT_MULTIV)
	{
		int iPos, iCount, iBID;

		rVar.New(PDT_MULTIV, rVar.Name().c_str());
		TMultiV& vA = *rVar.GetMultiVPtr();

		GET_VAL(&iBID, sizeof(int))
		GET_VAL(&iCount, sizeof(int))

		if (iBID == ID_E3GA)
		{
			vA = rCB.GetE3GABase().vZero;
		}
		#ifdef USE_GA_E8
			else if (iBID == ID_E8GA)
			{
				vA = rCB.GetE8GABase().vZero;
			}
		#endif
		else if (iBID == ID_PGA)
		{
			vA = rCB.GetP3GABase().vZero;
		}
		else if (iBID == ID_EL2GA)
		{
			vA = rCB.GetEl2GABase().vZero;
		}
		else if (iBID == ID_CONFGA)
		{
			vA = rCB.GetN3GABase().vZero;
		}
		else
		{
			throw CCluError("Unkown multivector type. File probably corrupted");
		}

		for (iPos = 0; iPos < iCount; iPos++)
		{
			GET_VAL(&vA[iPos], sizeof(TCVScalar))
		}
	}
	else if (eVarType == PDT_COLOR)
	{
		rVar.New(PDT_COLOR, rVar.Name().c_str());
		TOGLColor& rCol = *rVar.GetOGLColorPtr();

		GET_VAL(rCol.Data(), 4 * sizeof(float))
	}
	else if (eVarType == PDT_VARLIST)
	{
		rVar.New(PDT_VARLIST, rVar.Name().c_str());
		TVarList& rList = *rVar.GetVarListPtr();

		int iVar, iCount;

		GET_VAL(&iCount, sizeof(int))

		if (iCount < 0)
		{
			throw CCluError("Negative variable list size. File probably corrupted");
		}
		else if (iCount > 0)
		{
			rList.Set(iCount);

			for (iVar = 0; iVar < iCount; iVar++)
			{
				ReadVariable(rCB,  rList(iVar), ppcData);
			}
		}
	}
	else if (eVarType == PDT_SCENE /* Object */)
	{
		uint uPos, uSize;
		uint uVexCnt, uNormCnt, uTexCnt, uColCnt, uIdxCnt;
		int piLineStipple[2];
		GLenum eType, pePolyMode[2];
		bool bUsePolygonMode, bUseScaling, bUseTrans, bUseOverrideAlpha;
		bool bUseLineStipple, bUseScaleBackColor;
		float pfPOF[2], fAngle, fAlpha, fBackColorScale;
		Mem<char> mName;
		COGLVertex vexScale, vexAxis, vexTrans1, vexTrans2;

		if (!rCB.GetOGLDrawBase())
		{
			throw CCluError("DrawBase not defined");
		}

		COGLBERepository* pSceneRep = rCB.GetOGLDrawBase()->GetSceneRepository();
		if (!pSceneRep)
		{
			throw CCluError("Scene repository not defined");
		}

		TVexList* pVexList = new TVexList;
		if (!pVexList)
		{
			throw CCluError("Out of memory while creating vertex list");
		}

		rVar = pSceneRep->New(pVexList);

		TVexList& rList = *pVexList;
		//rVar.New( PDT_SCENE, rVar.Name().c_str() );
		//TVexList &rList = *rVar.GetVexListPtr();

		GET_VAL(&uSize, sizeof(uint))
		mName.Set(uSize + 1);
		GET_VAL(mName.Data(), uSize)
		mName[uSize] = 0;

		GET_VAL(&eType, sizeof(GLenum))

		GET_VAL(pfPOF, 2 * sizeof(float))

		GET_VAL(&bUsePolygonMode, sizeof(bool))
		GET_VAL(pePolyMode, 2 * sizeof(GLenum))

		GET_VAL(&bUseScaling, sizeof(bool))
		GET_VAL(((float*) vexScale), 3 * sizeof(float))

		GET_VAL(&bUseTrans, sizeof(bool))
		GET_VAL(&fAngle, sizeof(float))
		GET_VAL(((float*) vexAxis), 3 * sizeof(float))
		GET_VAL(((float*) vexTrans1), 3 * sizeof(float))
		GET_VAL(((float*) vexTrans2), 3 * sizeof(float))

		GET_VAL(&bUseOverrideAlpha, sizeof(bool))
		GET_VAL(&fAlpha, sizeof(float))

		GET_VAL(&bUseLineStipple, sizeof(bool))
		GET_VAL(piLineStipple, 2 * sizeof(int))

		GET_VAL(&bUseScaleBackColor, sizeof(bool))
		GET_VAL(&fBackColorScale, sizeof(float))

		GET_VAL(&uVexCnt, sizeof(uint))
		GET_VAL(&uNormCnt, sizeof(uint))
		GET_VAL(&uTexCnt, sizeof(uint))
		GET_VAL(&uColCnt, sizeof(uint))
		GET_VAL(&uIdxCnt, sizeof(uint))

		rList.SetMode(eType);
		rList.SetName(mName.Data());
		rList.SetPolygonOffsetFill(pfPOF[0], pfPOF[1]);

		rList.EnablePolygonMode(bUsePolygonMode);
		rList.SetPolygonMode(pePolyMode[0], pePolyMode[1]);

		rList.EnableScaling(bUseScaling);
		rList.SetScaling(vexScale);

		rList.EnableTransform(bUseTrans);
		rList.SetRotation(fAngle, vexAxis);
		rList.SetTranslation1(vexTrans1);
		rList.SetTranslation2(vexTrans2);

		rList.EnableOverrideAlpha(bUseOverrideAlpha);
		rList.SetOverrideAlphaValue(fAlpha);

		rList.EnableLineStipple(bUseLineStipple);
		rList.SetLineStipple(piLineStipple[0], piLineStipple[1]);

		rList.EnableScaleBackColor(bUseScaleBackColor);
		rList.SetBackColorScale(fBackColorScale);

		if (uVexCnt > 0)
		{
			//COGLVertexList::TVexList &mVex = rList.GetVexList();
			rList.Reserve(uVexCnt);

			for (uPos = 0; uPos < uVexCnt; uPos++)
			{
				GET_VAL(((float*) rList[uPos].xVex), 3 * sizeof(float))
			}

			rList.UpdateVexCount();
		}

		if (uNormCnt > 0)
		{
			//COGLVertexList::TVexList &mVex = rList.GetNormList();
			rList.Reserve(uNormCnt);

			for (uPos = 0; uPos < uNormCnt; uPos++)
			{
				GET_VAL(((float*) rList[uPos].xNorm), 3 * sizeof(float))
			}

			rList.UpdateNormCount();
		}

		if (uTexCnt > 0)
		{
			//COGLVertexList::TVexList &mVex = rList.GetTexList();
			rList.Reserve(uTexCnt);

			for (uPos = 0; uPos < uTexCnt; uPos++)
			{
				GET_VAL(((float*) rList[uPos].xTex), 3 * sizeof(float))
			}

			rList.UpdateTexCount();
		}

		if (uColCnt > 0)
		{
			//COGLVertexList::TColList &mCol = rList.GetColList();
			rList.Reserve(uColCnt);

			for (uPos = 0; uPos < uColCnt; uPos++)
			{
				GET_VAL(rList[uPos].xCol.Data(), 4 * sizeof(float))
			}

			rList.UpdateColCount();
		}

		if (uIdxCnt > 0)
		{
			uint uCnt;
			COGLVertexList::TIdxList& mIdxList = rList.GetIdxList();
			mIdxList.Set(uIdxCnt);

			for (uPos = 0; uPos < uIdxCnt; uPos++)
			{
				Mem<unsigned>& mIdx = mIdxList[uPos];

				GET_VAL(&uCnt, sizeof(uint))
				mIdx.Set(uCnt);

				GET_VAL(mIdx.Data(), uCnt * sizeof(unsigned))
			}
		}
	}
	else if (eVarType == PDT_IMAGE)
	{
		unsigned uByteCnt;
		int iWidth, iHeight;
		int iImgType, iDataType, iBPP;

		rVar.New(PDT_IMAGE, rVar.Name().c_str());
		TImage& rImage = *rVar.GetImagePtr();

		GET_VAL(&iWidth, sizeof(int));
		GET_VAL(&iHeight, sizeof(int));
		GET_VAL(&iImgType, sizeof(int));
		GET_VAL(&iDataType, sizeof(int));
		GET_VAL(&iBPP, sizeof(int));

		if ((iWidth < 0) || (iHeight < 0) || (iBPP < 0))
		{
			throw CCluError("File contains invalid image variable");
		}

		rImage->ResizeCanvas(1, 1);
		rImage->ConvertType(iImgType, iDataType);
		rImage->ResizeCanvas(iWidth, iHeight);
		uByteCnt = unsigned(iWidth) * unsigned(iHeight) * unsigned(iBPP);

		::LockImageAccess();
		GET_VAL(rImage->GetDataPtr(), uByteCnt);
		::UnlockImageAccess();
	}
	else
	{
		throw CCluError("File contains unsupported variable type");
	}
}

//////////////////////////////////////////////////////////////////////
/// Read a single variable

void ReadVariable(CCLUCodeBase& rCB, CCodeVar& rVar, CXMLElement& rEl)
throw(CCluException)
{
	char pcText[100];
	string sType, sError, sContent;

	sType    = rEl.GetProp("type");
	sContent = rEl.GetContent();

	if (sType == CCodeVar::TypeStr(PDT_SCALAR))
	{
		TCVScalar dVal;
		sscanf_s(sContent.c_str(), "%lg", &dVal);
		rVar = dVal;
	}
	else if (sType == CCodeVar::TypeStr(PDT_COUNTER))
	{
		TCVCounter iVal;
		sscanf_s(sContent.c_str(), "%d", &iVal);
		rVar = iVal;
	}
	else if (sType == CCodeVar::TypeStr(PDT_STRING))
	{
		rVar = sContent.c_str();
	}
	else if (sType == CCodeVar::TypeStr(PDT_MATRIX))
	{
		stringstream zContent;
		int iRows, iCols, iPos, iCount;
		TCVScalar* pData;
		rVar.New(PDT_MATRIX, rVar.Name().c_str());
		TMatrix& xA = *rVar.GetMatrixPtr();

		iRows = atoi(rEl.GetProp("rows").c_str());
		iCols = atoi(rEl.GetProp("cols").c_str());

		if (!xA.Resize(iRows, iCols))
		{
			sprintf_s(pcText, "Cannot create matrix of dimensions %dx%d.", iRows, iCols);
			throw CCluError(pcText);
		}

		zContent.str(sContent);
		pData  = xA.Data();
		iCount = iRows * iCols;
		for (iPos = 0; iPos < iCount; iPos++)
		{
			pcText[0] = 0;
			zContent.getline(pcText, 99, ' ');
			if (pcText[0] == 0)
			{
				throw CCluError("Not enough data for matrix");
			}

			sscanf_s(pcText, "%lg", &pData[iPos]);
		}
	}
	else if (sType == CCodeVar::TypeStr(PDT_TENSOR))
	{
		stringstream zContent, zDim;
		Mem<int> mDim;
		int iPos, iDim, iValence, iSize;
		TCVScalar* pData;

		rVar.New(PDT_TENSOR);
		TTensor& rT = *rVar.GetTensorPtr();

		zDim.str(rEl.GetProp("dim"));
		iValence = 0;
		while (true)
		{
			pcText[0] = 0;
			zDim.getline(pcText, 99, ' ');
			if (pcText[0] == 0)
			{
				break;
			}

			sscanf_s(pcText, "%d", &iDim);
			iValence++;
			mDim.Add(1);
			mDim.Last() = iDim;
		}

		rT.Reset(mDim);
		pData = rT.Data();
		iSize = rT.Size();

		zContent.str(rEl.GetContent());
		iPos = 0;
		while (iPos < iSize)
		{
			pcText[0] = 0;
			zContent.getline(pcText, 99, ' ');
			if (pcText[0] == 0)
			{
				throw CCluError("Not enough data for tensor");
			}

			sscanf_s(pcText, "%lg", &pData[iPos]);
			iPos++;
		}
	}
	else if (sType == CCodeVar::TypeStr(PDT_MULTIV))
	{
		stringstream zContent, zIdx;
		int iPos;
		string sBID, sIdx;

		rVar.New(PDT_MULTIV, rVar.Name().c_str());
		TMultiV& vA = *rVar.GetMultiVPtr();

		sIdx = rEl.GetProp("idx");
		sBID = rEl.GetProp("bid");

		if (sBID == "E3")
		{
			vA = E3GA<TCVScalar>::vZero;
		}
		#ifdef USE_GA_E8
			else if (sBID == "E8")
			{
				vA = E8GA<TCVScalar>::vZero;
			}
		#endif
		else if (sBID == "P3")
		{
			vA = PGA<TCVScalar>::vZero;
		}
		else if (sBID == "C2")
		{
			vA = El2GA<TCVScalar>::vZero;
		}
		else if (sBID == "N3")
		{
			vA = ConfGA<TCVScalar>::vZero;
		}

		zIdx.str(sIdx);
		zContent.str(sContent);
		while (true)
		{
			pcText[0] = 0;
			zIdx.getline(pcText, 99, ' ');
			if (pcText[0] == 0)
			{
				break;
			}

			sscanf_s(pcText, "%d", &iPos);

			pcText[0] = 0;
			zContent.getline(pcText, 99, ' ');
			if (pcText[0] == 0)
			{
				throw CCluError("Not enough data for multivector");
			}

			sscanf_s(pcText, "%lg", &vA[iPos]);
		}
	}
	else if (sType == CCodeVar::TypeStr(PDT_VARLIST))
	{
		/*
		int iSize;
		string sSize;
		sSize = rEl.GetProp("size");
		iSize = atoi(sSize.c_str());
		if (iSize <= 0)
		throw CCLUError("Invalid variable list size");
		*/
		rVar.New(PDT_VARLIST);
		TVarList& rSubList = *rVar.GetVarListPtr();

		try
		{
			TXMLElementList& rElList = rEl.GetSubTree().GetElementList("var");
			int iEl, iElCount = int(rElList.size());
			rSubList.Set(iElCount);

			for (iEl = 0; iEl < iElCount; iEl++)
			{
				ReadVariable(rCB, rSubList(iEl), rElList[iEl]);
			}
		}
		catch (CXMLTree::ElementNotAvailable& rEx)
		{
			rEx.GetErrorLevel();
			// empty list; no problem
		}
		catch (CCluException& rEx)
		{
			throw rEx;	// throw on
		}
	}
	else if (sType == CCodeVar::TypeStr(PDT_VEXLIST))
	{
		stringstream zContent;
		string sMode;
		int iPos, iCount;

		rVar.New(PDT_VEXLIST);
		TVexList& rVexList = *rVar.GetVexListPtr();

		sMode = rEl.GetProp("mode");
		if (sMode == "points")
		{
			rVexList.SetMode(GL_POINTS);
		}
		else if (sMode == "lines")
		{
			rVexList.SetMode(GL_LINES);
		}
		else if (sMode == "line_strip")
		{
			rVexList.SetMode(GL_LINE_STRIP);
		}
		else if (sMode == "line_loop")
		{
			rVexList.SetMode(GL_LINE_LOOP);
		}
		else if (sMode == "triangles")
		{
			rVexList.SetMode(GL_TRIANGLES);
		}
		else if (sMode == "triangle_strip")
		{
			rVexList.SetMode(GL_TRIANGLE_STRIP);
		}
		else if (sMode == "triangle_loop")
		{
			rVexList.SetMode(GL_TRIANGLE_FAN);
		}
		else if (sMode == "triangle_fan")
		{
			rVexList.SetMode(GL_TRIANGLE_FAN);
		}
		else if (sMode == "quads")
		{
			rVexList.SetMode(GL_QUADS);
		}
		else if (sMode == "quad_strip")
		{
			rVexList.SetMode(GL_QUAD_STRIP);
		}
		else if (sMode == "polygon")
		{
			rVexList.SetMode(GL_POLYGON);
		}

		/////////////////////////////////////////////////////////
		// Read Vertices
		try
		{
			CXMLElement& rSubEl = rEl.GetSubTree().GetElementList("vertices")[0];
			//Mem<COGLVertex> &rValList = rVexList.GetVexList();

			iCount = atoi(rSubEl.GetProp("count").c_str());
			rVexList.Reserve(iCount);

			zContent.str(rSubEl.GetContent());
			for (iPos = 0; iPos < iCount; iPos++)
			{
				float* pData = (float*) rVexList[iPos].xVex;

				for (int i = 0; i < 3; i++)
				{
					pcText[0] = 0;
					zContent.getline(pcText, 99, ' ');
					if (pcText[0] == 0)
					{
						throw CCluError("Not enough data for vertex");
					}

					sscanf_s(pcText, "%g", &pData[i]);
				}
			}

			rVexList.UpdateVexCount();
		}
		catch (CXMLTree::ElementNotAvailable& rEx)
		{
			rEx.GetErrorLevel();
			// empty list; no problem
		}
		catch (CCluException& rEx)
		{
			throw rEx;	// throw on
		}

		/////////////////////////////////////////////////////////
		// Read Normals
		try
		{
			CXMLElement& rSubEl = rEl.GetSubTree().GetElementList("normals")[0];
			//Mem<COGLVertex> &rValList = rVexList.GetNormList();

			iCount = atoi(rSubEl.GetProp("count").c_str());
			rVexList.Reserve(iCount);

			zContent.str(rSubEl.GetContent());
			for (iPos = 0; iPos < iCount; iPos++)
			{
				float* pData = (float*) rVexList[iPos].xNorm;

				for (int i = 0; i < 3; i++)
				{
					pcText[0] = 0;
					zContent.getline(pcText, 99, ' ');
					if (pcText[0] == 0)
					{
						throw CCluError("Not enough data for normal");
					}

					sscanf_s(pcText, "%g", &pData[i]);
				}
			}

			rVexList.UpdateNormCount();
		}
		catch (CXMLTree::ElementNotAvailable& rEx)
		{
			rEx.GetErrorLevel();
			// empty list; no problem
		}
		catch (CCluException& rEx)
		{
			throw rEx;	// throw on
		}

		/////////////////////////////////////////////////////////
		// Read Texture Coordinates
		try
		{
			CXMLElement& rSubEl = rEl.GetSubTree().GetElementList("texture_coordinates")[0];
			//Mem<COGLVertex> &rValList = rVexList.GetTexList();

			iCount = atoi(rSubEl.GetProp("count").c_str());
			rVexList.Reserve(iCount);

			zContent.str(rSubEl.GetContent());
			for (iPos = 0; iPos < iCount; iPos++)
			{
				float* pData = (float*) rVexList[iPos].xTex;

				for (int i = 0; i < 3; i++)
				{
					pcText[0] = 0;
					zContent.getline(pcText, 99, ' ');
					if (pcText[0] == 0)
					{
						throw CCluError("Not enough data for texture coordinate");
					}

					sscanf_s(pcText, "%g", &pData[i]);
				}
			}

			rVexList.UpdateTexCount();
		}
		catch (CXMLTree::ElementNotAvailable& rEx)
		{
			rEx.GetErrorLevel();
			// empty list; no problem
		}
		catch (CCluException& rEx)
		{
			throw rEx;	// throw on
		}

		/////////////////////////////////////////////////////////
		// Read Colors
		try
		{
			CXMLElement& rSubEl = rEl.GetSubTree().GetElementList("colors")[0];
			//Mem<TColor> &rValList = rVexList.GetColList();

			iCount = atoi(rSubEl.GetProp("count").c_str());
			rVexList.Reserve(iCount);

			zContent.str(rSubEl.GetContent());
			for (iPos = 0; iPos < iCount; iPos++)
			{
				TColor& rData = rVexList[iPos].xCol;

				for (int i = 0; i < 4; i++)
				{
					pcText[0] = 0;
					zContent.getline(pcText, 99, ' ');
					if (pcText[0] == 0)
					{
						throw CCluError("Not enough data for color");
					}

					sscanf_s(pcText, "%g", &rData[i]);
				}
			}

			rVexList.UpdateColCount();
		}
		catch (CXMLTree::ElementNotAvailable& rEx)
		{
			rEx.GetErrorLevel();
			// empty list; no problem
		}
		catch (CCluException& rEx)
		{
			throw rEx;	// throw on
		}

		/////////////////////////////////////////////////////////
		// Read Index List
		try
		{
			CXMLElement& rSubEl              = rEl.GetSubTree().GetElementList("indices")[0];
			MemObj<Mem<unsigned> >& rIdxList = rVexList.GetIdxList();
			rIdxList.Set(1);
			Mem<unsigned>& rValList = rIdxList[0];

			iCount = atoi(rSubEl.GetProp("count").c_str());
			rValList.Set(iCount);

			zContent.str(rSubEl.GetContent());
			for (iPos = 0; iPos < iCount; iPos++)
			{
				pcText[0] = 0;
				zContent.getline(pcText, 99, ' ');
				if (pcText[0] == 0)
				{
					throw CCluError("Not enough data for normal");
				}

				sscanf_s(pcText, "%d", &rValList[iPos]);
			}
		}
		catch (CXMLTree::ElementNotAvailable& rEx)
		{
			rEx.GetErrorLevel();
			// empty list; no problem
		}
		catch (CCluException& rEx)
		{
			throw rEx;	// throw on
		}
	}
	else
	{
		sError  = "Reading variables of type '";
		sError += sType;
		sError += "' is not implemented, yet.";

		throw CCluError(sError.c_str());
	}
}

//////////////////////////////////////////////////////////////////////
/// Read Data Function
///
/// 1. Filename
/// 2. Separator Symbol (optional). Standard is space.
///		Return is always end of line. Everything in one line is put in a list.

bool ReadDataFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	char pcSep[2];
	CStrMem csFilename;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if ((iVarCount < 1) || (iVarCount > 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().InvalidType(mVars(0), iLine, iPos);
		return false;
	}

	if (iVarCount >= 2)
	{
		CStrMem csVal;
		if (mVars(1).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().InvalidType(mVars(1), iLine, iPos);
			return false;
		}

		csVal    = mVars(1).ValStr();
		pcSep[0] = csVal[0];
	}
	else
	{
		pcSep[0] = ' ';
	}
	pcSep[1] = 0;

	csFilename = mVars(0).ValStr();

	ifstream zData;
	char pcCurPath[500], pcLine[301], pcVal[2], cVal;
	double dVal;

	rVar.New(PDT_VARLIST);
	TVarList& DList = *rVar.GetVarListPtr();
	vector<TVarList*> vecVarList;
	TVarList* pCurVarList;

	_getcwd(pcCurPath, 499);
	_chdir(rCB.GetScriptPath().c_str());

	zData.open(csFilename.Str());
	if (!zData.is_open())
	{
		_chdir(pcCurPath);

		char pcText[500];
		sprintf_s(pcText, "Data file '%s' could not be opened.", csFilename.Str());

		rCB.GetErrorList().GeneralError(pcText);
		return false;
	}

	vecVarList.push_back(&DList);
	DList.Add(1);
	DList.Last().New(PDT_VARLIST);
	pCurVarList = ((TVarList*) DList.Last().Val());
	vecVarList.push_back(pCurVarList);

	pcVal[1]  = 0;
	pcLine[0] = 0;
	do
	{
		cVal = (char) zData.get();
		//zData.getline(pcLine, 299);
		if (zData.eof() && (pcLine[0] == 0))
		{
			break;
		}
		else if (zData.eof())
		{
			cVal = pcSep[0];
		}

		if (((cVal == pcSep[0]) || (cVal == '\n')) && (pcLine[0] != 0))
		{
			if (sscanf_s(pcLine, "%lg", &dVal) == 1)
			{
				pCurVarList->Add(1);
				pCurVarList->Last() = (TCVScalar) dVal;
			}
			pcLine[0] = 0;
		}
		else if ((cVal != pcSep[0]) && (cVal != '\n'))
		{
			pcVal[0] = cVal;
			//strcat(pcLine, pcVal);
			strcat_s(pcLine, sizeof(pcLine), pcVal);
		}

		if (cVal == '\n')
		{
			vecVarList.pop_back();
			pCurVarList = vecVarList.back();
			pCurVarList->Add(1);
			pCurVarList->Last().New(PDT_VARLIST);
			pCurVarList = ((TVarList*) pCurVarList->Last().Val());
			vecVarList.push_back(pCurVarList);
			pcLine[0] = 0;
		}
	}
	while (!zData.eof());

	zData.close();

	_chdir(pcCurPath);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Write Data Function
///
/// 1. Variable List
/// 2. Filename
/// 3. Separator Symbol (optional). Standard is space.
///		Return is always end of line. Everything in one line is put in a list.

bool WriteDataFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	char pcSep[2];
	int iList, iStr;
	CStrMem csFilename;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if ((iVarCount < 2) || (iVarCount > 3))
	{
		int piPar[] = { 2, 3 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if ((mVars(0).BaseType() == PDT_VARLIST) &&
	    (mVars(1).BaseType() == PDT_STRING))
	{
		iList = 0;
		iStr  = 1;
	}
	else if ((mVars(1).BaseType() == PDT_VARLIST) &&
		 (mVars(0).BaseType() == PDT_STRING))
	{
		iList = 1;
		iStr  = 0;
	}
	else if ((mVars(0).BaseType() != PDT_VARLIST) &&
		 (mVars(0).BaseType() != PDT_STRING))
	{
		rCB.GetErrorList().InvalidType(mVars(0), iLine, iPos);
		return false;
	}
	else if ((mVars(1).BaseType() != PDT_VARLIST) &&
		 (mVars(1).BaseType() != PDT_STRING))
	{
		rCB.GetErrorList().InvalidType(mVars(1), iLine, iPos);
		return false;
	}

	if (iVarCount >= 3)
	{
		CStrMem csVal;
		if (mVars(2).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().InvalidType(mVars(2), iLine, iPos);
			return false;
		}

		csVal    = mVars(2).ValStr();
		pcSep[0] = csVal[0];
	}
	else
	{
		pcSep[0] = ' ';
	}
	pcSep[1] = 0;

	csFilename = mVars(iStr).ValStr();

	ofstream zData;
	char pcCurPath[500], pcLine[301];
	TCVScalar dVal;

	TVarList& DList = *mVars(iList).GetVarListPtr();
	int iDataPos, iDataCount = int(DList.Count());

	_getcwd(pcCurPath, 499);
	_chdir(rCB.GetScriptPath().c_str());

	zData.open(csFilename.Str());
	if (!zData.is_open())
	{
		_chdir(pcCurPath);

		char pcText[500];
		sprintf_s(pcText, "Data file '%s' could not be opened.", csFilename.Str());

		rCB.GetErrorList().GeneralError(pcText);
		return false;
	}

	for (iDataPos = 0; iDataPos < iDataCount; iDataPos++)
	{
		CCodeVar& rDataVar = DList(iDataPos);

		if (rDataVar.CastToScalar(dVal, rCB.GetSensitivity()))
		{
			sprintf_s(pcLine, "%.17lg%s", dVal, pcSep);
			zData << pcLine;
		}
		else if (rDataVar.BaseType() == PDT_VARLIST)
		{
			TVarList& rList = *rDataVar.GetVarListPtr();
			int i, iCount = int(rList.Count());

			zData << "\n";
			for (i = 0; i < iCount; i++)
			{
				if (!rList(i).CastToScalar(dVal, rCB.GetSensitivity()))
				{
					rCB.GetErrorList().GeneralError("Cannot write data.", iLine, iPos);
					zData.close();
					return false;
				}

				sprintf_s(pcLine, "%.17lg%s", dVal, pcSep);
				zData << pcLine;
			}
		}
		else
		{
			rCB.GetErrorList().GeneralError("Cannot write data.", iLine, iPos);
			zData.close();
			return false;
		}
	}

	zData.close();

	_chdir(pcCurPath);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Read Matrix Function
///
/// 1. Filename
/// 2. Separator Symbol (optional). Standard is space.
///		Return is always end of line. Everything in one line is put in a list.

bool ReadMatrixFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	char pcSep[2];
	CStrMem csFilename;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if ((iVarCount < 1) || (iVarCount > 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().InvalidType(mVars(0), iLine, iPos);
		return false;
	}

	if (iVarCount >= 2)
	{
		CStrMem csVal;
		if (mVars(1).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().InvalidType(mVars(1), iLine, iPos);
			return false;
		}

		csVal    = mVars(1).ValStr();
		pcSep[0] = csVal[0];
	}
	else
	{
		pcSep[0] = ' ';
	}
	pcSep[1] = 0;

	csFilename = mVars(0).ValStr();

	ifstream zData;
	int iLineLen, iRow, iCol, iRowCount, iColCount;
	char pcCurPath[500], pcLine[301], pcVal[2], cVal;
	double dVal;

	rVar.New(PDT_MATRIX);
	TMatrix& DMatrix = *rVar.GetMatrixPtr();

	_getcwd(pcCurPath, 499);
	_chdir(rCB.GetScriptPath().c_str());

	zData.open(csFilename.Str());
	if (!zData.is_open())
	{
		_chdir(pcCurPath);

		char pcText[500];
		sprintf_s(pcText, "Matrix data file '%s' could not be opened.", csFilename.Str());

		rCB.GetErrorList().GeneralError(pcText);
		return false;
	}

	iRowCount = 0;
	iColCount = 0;
	iRow      = -1;
	iCol      = -2;
	pcVal[1]  = 0;
	pcLine[0] = 0;
	iLineLen  = 0;
	do
	{
		cVal = (char) zData.get();
		//zData.getline(pcLine, 299);
		if (zData.eof() && (pcLine[0] == 0))
		{
			break;
		}
		else if (zData.eof())
		{
			cVal = pcSep[0];
		}

		if (((cVal == pcSep[0]) || (cVal == '\n')) && (pcLine[0] != 0))
		{
			if (sscanf_s(pcLine, "%lg", &dVal) == 1)
			{
				if (iCol == -2)
				{
					iRowCount = int(dVal);
				}
				else if (iCol == -1)
				{
					iColCount = int(dVal);
					if (iRowCount <= 0)
					{
						rCB.GetErrorList().GeneralError("Invalid row size of matrix.", iLine, iPos);
						return false;
					}
					if (iColCount <= 0)
					{
						rCB.GetErrorList().GeneralError("Invalid column size of matrix.", iLine, iPos);
						return false;
					}
					DMatrix.Resize(iRowCount, iColCount);
				}
				else if ((iRow >= 0) && (iRow < iRowCount) &&
					 (iCol >= 0) && (iCol < iColCount))
				{
					DMatrix(iRow + 1, iCol + 1) = TCVScalar(dVal);
				}
			}
			iLineLen  = 0;
			pcLine[0] = 0;
			iCol++;
		}
		else if ((cVal != pcSep[0]) && (cVal != '\n'))
		{
			pcLine[iLineLen]   = cVal;
			pcLine[++iLineLen] = 0;
		}

		if (cVal == '\n')
		{
			if (iCol < 0)
			{
				rCB.GetErrorList().GeneralError("Matrix dimensions not specified.", iLine, iPos);
				return false;
			}
			iRow++;
			iCol = 0;
		}
	}
	while (!zData.eof());

	zData.close();

	_chdir(pcCurPath);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////
// Show a file with standard program defined by OS

bool ShowFileFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if ((iVarCount < 1) || (iVarCount > 1))
	{
		int piPar[] = { 1 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().InvalidType(mVars(0), iLine, iPos);
		return false;
	}

	#ifdef WIN32
		string sFile;

		sFile  = rCB.GetScriptPath();
		sFile += mVars(0).ValStr();

		HINSTANCE hInstance = ShellExecuteA(NULL,
			"open",
			sFile.c_str(),
			NULL,
			NULL,
			SW_SHOWNORMAL);	//SHOWMINIMIZED);

		if ((int) hInstance < 32)
		{
			rCB.GetErrorList().GeneralError("File cannot be found.", iLine, iPos);
			return false;
		}
	#endif

	return true;
}

/////////////////////////////////////////////////////////////////////////////////
// Show a file with standard program defined by OS
//
// Parameters:
//	1. (opt) Dialog Box Title
//	2. (opt) Search pattern: "Text:*.txt,*.tex;Word:*.doc;"
//  3. (opt) Parameter list:
//		"type":
//		- "open": for open file name dialog box
//		- "save": for save file name dialog box
//
bool FileChooserFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	string sTitle, sPat;
	int iVarCount        = int(mVars.Count());
	std::string sDlgType = "open";

	if (iVarCount > 2)
	{
		int piPar[] = { 0, 1, 2, 3 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 3, iLine, iPos);
		return false;
	}

	if (iVarCount >= 1)
	{
		if (mVars(0).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().InvalidType(mVars(0), iLine, iPos);
			return false;
		}

		sTitle = mVars(0).ValStr().Str();
	}
	else
	{
		sTitle = "Select File";
	}

	if (iVarCount >= 2)
	{
		if (mVars(1).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().InvalidType(mVars(1), iLine, iPos);
			return false;
		}

		sPat = mVars(1).ValStr().Str();
	}
	else
	{
		sPat = "All Files:*.*";
	}

	if (iVarCount >= 3)
	{
		if (mVars(2).BaseType() != PDT_VARLIST)
		{
			rCB.GetErrorList().InvalidType(mVars(1), iLine, iPos);
			return false;
		}

		TVarList& rParList = *mVars(2).GetVarListPtr();
		CCodeVar xVar;

		if (rCB.GetVarListElement(xVar, rParList, TString("type")))
		{
			if (xVar.BaseType() == PDT_STRING)
			{
				sDlgType = *xVar.GetStringPtr();
				if ((sDlgType != "open")
				    && (sDlgType != "save"))
				{
					rCB.GetErrorList().GeneralError("Dialog type has to be 'open' or 'save'.", iLine, iPos);
					return false;
				}
			}
			else
			{
				rCB.GetErrorList().GeneralError("Dialog type has to be 'open' or 'save'.", iLine, iPos);
				return false;
			}
		}
	}

	#ifdef WIN32
		// Parse Pattern list for native Win32 dialog box
		int iIdx, iCnt = int(sPat.length()) + 2;
		char* pcPat = 0, * pcChar = 0;
		char pcFile[1024];

		pcFile[0] = 0;
		pcPat     = new char[iCnt];
		if (!pcPat)
		{
			rCB.GetErrorList().OutOfMemory(iLine, iPos);
			return false;
		}

		//strncpy( pcPat, sPat.c_str(), iCnt-2 );
		strncpy_s(pcPat, iCnt, sPat.c_str(), iCnt - 2);

		for (iIdx = 0, pcChar = pcPat; iIdx < iCnt - 2; ++iIdx, ++pcChar)
		{
			if ((*pcChar == ':') || (*pcChar == ';'))
			{
				*pcChar = 0;
			}
			else if (*pcChar == ',')
			{
				*pcChar = ';';
			}
		}

		pcChar[0] = 0;
		pcChar[1] = 0;

		// Use native Windows dialog box
		OPENFILENAMEA dlgData;
		BOOL bResult = FALSE;

		if (sDlgType == "open")
		{
			dlgData.lStructSize       = sizeof(OPENFILENAMEA);
			dlgData.hwndOwner         = NULL;
			dlgData.hInstance         = NULL;
			dlgData.lpstrFilter       = pcPat;
			dlgData.lpstrCustomFilter = NULL;
			dlgData.nMaxCustFilter    = 0;
			dlgData.nFilterIndex      = 1;
			dlgData.lpstrFile         = pcFile;
			dlgData.nMaxFile          = 1020;
			dlgData.lpstrFileTitle    = NULL;
			dlgData.nMaxFileTitle     = 0;
			dlgData.lpstrInitialDir   = NULL;
			dlgData.lpstrTitle        = sTitle.c_str();
			dlgData.Flags             = OFN_FILEMUSTEXIST;
			dlgData.nFileOffset       = 0;
			dlgData.nFileExtension    = 0;
			dlgData.lpstrDefExt       = NULL;
			dlgData.lCustData         = 0;
			dlgData.lpfnHook          = NULL;
			dlgData.lpfnHook          = NULL;
			dlgData.lpTemplateName    = NULL;
			dlgData.pvReserved        = 0;
			dlgData.dwReserved        = 0;
			dlgData.FlagsEx           = 0;

			bResult = GetOpenFileNameA(&dlgData);
		}
		else if (sDlgType == "save")
		{
			dlgData.lStructSize       = sizeof(OPENFILENAMEA);
			dlgData.hwndOwner         = NULL;
			dlgData.hInstance         = NULL;
			dlgData.lpstrFilter       = pcPat;
			dlgData.lpstrCustomFilter = NULL;
			dlgData.nMaxCustFilter    = 0;
			dlgData.nFilterIndex      = 1;
			dlgData.lpstrFile         = pcFile;
			dlgData.nMaxFile          = 1020;
			dlgData.lpstrFileTitle    = NULL;
			dlgData.nMaxFileTitle     = 0;
			dlgData.lpstrInitialDir   = NULL;
			dlgData.lpstrTitle        = sTitle.c_str();
			dlgData.Flags             = 0;
			dlgData.nFileOffset       = 0;
			dlgData.nFileExtension    = 0;
			dlgData.lpstrDefExt       = NULL;
			dlgData.lCustData         = 0;
			dlgData.lpfnHook          = NULL;
			dlgData.lpfnHook          = NULL;
			dlgData.lpTemplateName    = NULL;
			dlgData.pvReserved        = 0;
			dlgData.dwReserved        = 0;
			dlgData.FlagsEx           = 0;

			bResult = GetSaveFileNameA(&dlgData);
		}

		rVar.New(PDT_STRING);
		if (bResult)
		{
			*rVar.GetStringPtr() = dlgData.lpstrFile;
		}

		delete pcPat;
	#else
	#error "File Chooser not implemented for non Win32 system"

		// Use generic FLTK dialog box
		char pcFilename[2];

		pcFilename[0]         = 0;
		Fl_File_Chooser::sort = fl_casenumericsort;
		char* pcNewFile = fl_file_chooser(sTitle.c_str(), sPat.c_str(), pcFilename);

		rVar.New(PDT_STRING);
		if (pcNewFile)
		{
			*rVar.GetStringPtr() = pcNewFile;
		}
	#endif

	return true;
}

/////////////////////////////////////////////////////////////////////////////////
// Save Screen to BMP file

bool SaveScreenFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	string sFilename;
	int iVarCount = int(mVars.Count());

	if (iVarCount != 1)
	{
		int piPar[] = { 1 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().InvalidType(mVars(0), iLine, iPos);
			return false;
		}

		sFilename = mVars(0).ValStr().Str();
		sFilename = rCB.GetScriptPath() + sFilename;
	}

	if (!rCB.GetCLUDrawBase())
	{
		rCB.GetErrorList().GeneralError("No visualization window present.", iLine, iPos);
		return false;
	}

	if (!rCB.GetCLUDrawBase()->SaveScreen2BMP(sFilename.c_str()))
	{
		rCB.GetErrorList().GeneralError("Could not save image.", iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Read XML File Function
///
/// 1. Filename
/// 2. Variable in which to write result (any type)
///
/// Returns string "OK" if data was read, otherwise an error message.

bool ReadXMLFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
throw(CCluException)
{
	CEncode Encode;
	CStrMem csFilename, csPass;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if ((iVarCount < 2) || (iVarCount > 3))
	{
		int piPar[] = { 2, 3 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (iVarCount >= 3)
	{
		if (mVars(2).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().GeneralError("Third parameter has to be pass phrase.", iLine, iPos);
			return false;
		}

		csPass = mVars(2).ValStr();
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("First parameter has to be filename.", iLine, iPos);
		return false;
	}

	if (mVars(1).IsProtected())
	{
		rCB.GetErrorList().GeneralError("Cannot modify variable given as second parameter.", iLine, iPos);
		return false;
	}

	csFilename = mVars(0).ValStr();

	string sError, sVersion, sRootName, sFileID;
	char pcCurPath[500];
	CXMLdom xmlDOM;
	CXMLElement xmlEl;

	_getcwd(pcCurPath, 499);
	_chdir(rCB.GetScriptPath().c_str());

	try
	{
		Encode.ReadFileID(csFilename.Str(), sFileID);
		if (sFileID == "CLUData")
		{
			Mem<char> mData;
			char* pcPass = 0;

			if (csPass.Len())
			{
				pcPass = csPass.Str();
			}

			Encode.ReadFile(csFilename.Str(), sFileID, mData, pcPass);
			xmlDOM.ParseMemory(mData.Data(), int(mData.Count()));
		}
		else
		{
			xmlDOM.ParseFile(csFilename.Str());
		}

		xmlDOM.GetElementTree(xmlEl);

		mVars(1).New(PDT_VARLIST);
		TVarList& rList = *mVars(1).GetVarListPtr();
/*
                rList.Reserve(2);
                rList(0) = xmlTree.begin()->first.c_str();

                CCodeVar &rVarRoot = rList(1);
                rVarRoot.New(PDT_VARLIST);

                ReadXMLTree(*rVarRoot.GetVarListPtr(),
                                        xmlTree.begin()->second[0].GetSubTree(), iLine, iPos);
*/

		ReadXMLTree(rCB, rList, xmlEl.GetSubTree(), iLine, iPos);

		// Return with OK message to script
		rVar = "OK";
	}
	catch (CCluException& rEx)
	{
		rVar = rEx.PrintError().c_str();
	}

	_chdir(pcCurPath);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Read XML Tree
///

void ReadXMLTree(CCLUCodeBase& rCB, TVarList& rList, CXMLTree& rTree, int iLine, int iPos)
throw(CCluException)
{
	int iTreeSize = int(rTree.size());
	int iElCount, iEl;

	if (iTreeSize == 0)
	{
		return;
	}

	map<string, TXMLElementList>::iterator itElList;

	for (itElList = rTree.begin(); itElList != rTree.end(); ++itElList)
	{
		rList.Add(1);
		CCodeVar& rVar = rList.Last();

		rVar.New(PDT_VARLIST);
		TVarList& rSubList = *rVar.GetVarListPtr();

		rSubList.Set(2);
		rSubList(0) = itElList->first.c_str();

		rSubList(1).New(PDT_VARLIST);
		TVarList& rVarElList = *rSubList(1).GetVarListPtr();

		TXMLElementList& rElList = itElList->second;
		iElCount = int(rElList.size());
		rVarElList.Set(iElCount);

		for (iEl = 0; iEl < iElCount; iEl++)
		{
			CCodeVar& rVarEl               = rVarElList(iEl);
			CXMLElement& rEl               = rElList[iEl];
			map<string, string>& rPropList = rEl.GetPropMap();
			map<string, string>::iterator itProp;
			int iPos, iCount = int(rPropList.size());

			rVarEl.New(PDT_VARLIST);
			TVarList& rVarElSubList = *rVarEl.GetVarListPtr();

			rVarElSubList.Set(iCount + 2);
			// Store Property List
			for (itProp = rPropList.begin(), iPos = 0;
			     itProp != rPropList.end();
			     ++itProp, ++iPos)
			{
				CCodeVar& rVarProp = rVarElSubList(iPos);
				rVarProp.New(PDT_VARLIST);
				TVarList& rVarPropPair = *rVarProp.GetVarListPtr();

				rVarPropPair.Set(2);
				rVarPropPair(0) = itProp->first.c_str();
				rVarPropPair(1) = itProp->second.c_str();
			}

			// Store Content
			{
				CCodeVar& rVarProp = rVarElSubList(iCount);
				rVarProp.New(PDT_VARLIST);
				TVarList& rVarPropPair = *rVarProp.GetVarListPtr();

				rVarPropPair.Set(2);
				rVarPropPair(0) = "_content";
				rVarPropPair(1) = rEl.GetContent().c_str();
			}

			// Store SubTree
			{
				CCodeVar& rVarProp = rVarElSubList(iCount + 1);
				rVarProp.New(PDT_VARLIST);
				TVarList& rVarPropPair = *rVarProp.GetVarListPtr();

				rVarPropPair.Set(2);
				rVarPropPair(0) = "_subtree";

				rVarPropPair(1).New(PDT_VARLIST);

				ReadXMLTree(rCB, *rVarPropPair(1).GetVarListPtr(), rEl.GetSubTree(), iLine, iPos);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////
/// Write XML File Function
///
/// 1. Filename
/// 2. Variable which contains nested list that is to be written.
/// 3. Compression level
/// 4. (true/false) Encode
///
/// Returns string "OK" if data was written, otherwise an error message.

bool WriteXMLFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
throw(CCluException)
{
	int iEncode      = 0;
	int iCompression = 0;
	CStrMem csFilename, csPass;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if ((iVarCount < 2) || (iVarCount > 4))
	{
		int piPar[] = { 2, 3, 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 3, iLine, iPos);
		return false;
	}

	if (iVarCount >= 3)
	{
		if (!mVars(2).CastToCounter(iCompression))
		{
			rCB.GetErrorList().GeneralError("Compression value has to be a counter.", iLine, iPos);
			return false;
		}
	}

	if (iVarCount >= 4)
	{
		#if defined(CLUVIZ_HOME)
			rCB.GetErrorList().GeneralError("Encryption of XML files is only available in professional version.", iLine, iPos);
			return false;
		#else
			if (!mVars(3).CastToCounter(iEncode))
			{
				if (mVars(3).BaseType() != PDT_STRING)
				{
					rCB.GetErrorList().GeneralError("Encode flag has to be 'true' or 'false', or a string.", iLine, iPos);
					return false;
				}

				iEncode = 1;
				csPass  = mVars(3).ValStr();
			}
		#endif
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("First parameter has to be filename.", iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().GeneralError("Second parameter has to be a list.", iLine, iPos);
		return false;
	}

	csFilename = mVars(0).ValStr();

	string sError, sVersion, sRootName;
	char pcCurPath[500];
	CXMLdom xmlDOM;
	CXMLTree xmlTree;
	CXMLElement xmlElRoot;

	_getcwd(pcCurPath, 499);
	_chdir(rCB.GetScriptPath().c_str());

	try
	{
		TVarList& rData = *mVars(1).GetVarListPtr();
		if (rData.Count() != 1)
		{
			throw CCluError("Data list has to have exactly one root element");
		}

		if (rData(0).BaseType() != PDT_VARLIST)
		{
			throw CCluError("Root element is not a list");
		}

		TVarList& rRoot = *rData(0).GetVarListPtr();

		if (rRoot.Count() != 2)
		{
			throw CCluError("Root element is of wrong form");
		}

		if (rRoot(0).BaseType() != PDT_STRING)
		{
			throw CCluError("Name of root element has to be a string");
		}

		xmlElRoot.GetName() = rRoot(0).GetStringPtr()->Str();

		if (rRoot(1).BaseType() != PDT_VARLIST)
		{
			throw CCluError("Second part of root element has to contain a list");
		}

		TVarList& rRootTree = *rRoot(1).GetVarListPtr();

		if (rRootTree.Count() != 1)
		{
			throw CCluError("There has to exist a single root element");
		}

		if (rRootTree(0).BaseType() != PDT_VARLIST)
		{
			throw CCluError("Content part of root element has to be a list");
		}

		TVarList& rRootProp = *rRootTree(0).GetVarListPtr();

		int iProp, iPropCnt = int(rRootProp.Count());
		/*
		if (iPropCnt < 2)
		{
		        throw CCLUError("Root data has incorrect form. Expect '_content' and '_subtree' elements");
		}
		*/

		for (iProp = 0; iProp < iPropCnt; iProp++)
		{
			if (rRootProp(iProp).BaseType() != PDT_VARLIST)
			{
				throw CCluError("Elements of root data have to be lists");
			}

			TVarList& rProp = *rRootProp(iProp).GetVarListPtr();

			if (rProp.Count() != 2)
			{
				throw CCluError("Elements of root data have to consist of value pairs");
			}

			if (rProp(0).BaseType() != PDT_STRING)
			{
				throw CCluError("Name of property has to be a string");
			}

			TString& rElName = *rProp(0).GetStringPtr();
			TString csVal;

			if (rElName == "_content")
			{
				if (!rCB.CastToString(rProp(1), csVal))
				{
					throw CCluError("Cannot cast content value to string");
				}

				xmlElRoot.GetContent() = csVal.Str();
			}
			else if (rElName == "_subtree")
			{
				// Write Subtree of Root Element
				if (rProp(1).BaseType() != PDT_VARLIST)
				{
					throw CCluError("Value of '_subtree' has to be a list");
				}

				WriteXMLTree(rCB, *rProp(1).GetVarListPtr(), xmlElRoot.GetSubTree(), iLine, iPos);
			}
			else
			{
				if (!rCB.CastToString(rProp(1), csVal))
				{
					throw CCluError("Cannot cast property value to string");
				}

				xmlElRoot.AddProp(rElName.Str(), csVal.Str());
			}
		}

		xmlTree.AddElement(xmlElRoot);
		xmlDOM.SetElementTree(xmlTree);

		if (iEncode)
		{
			CEncode Encode;
			vector<char> vecData;
			char* pcPass = 0;

			if (csPass.Len())
			{
				pcPass = csPass.Str();
			}

			xmlDOM.WriteMemory(vecData);
			Encode.WriteFile(csFilename.Str(), "CLUData", &vecData[0], vecData.size(), pcPass);
		}
		else
		{
			xmlDOM.WriteFile(csFilename.Str(), iCompression);
		}

		// Return with OK message to script
		rVar = "OK";
	}
	catch (CCluException& rEx)
	{
		rVar = rEx.PrintError().c_str();
	}

	_chdir(pcCurPath);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Write XML Tree
///

void WriteXMLTree(CCLUCodeBase& rCB, TVarList& rList, CXMLTree& rTree, int iLine, int iPos)
throw(CCluException)
{
	TString csElName;
	int iEl, iElCnt = int(rList.Count());
	CXMLElement xmlEl;

	for (iEl = 0; iEl < iElCnt; iEl++)
	{
		if (rList(iEl).BaseType() != PDT_VARLIST)
		{
			throw CCluError("Subtree has to consist of lists");
		}

		TVarList& rElList = *rList(iEl).GetVarListPtr();

		if (rElList.Count() != 2)
		{
			throw CCluError("Element is of wrong form");
		}

		if (rElList(0).BaseType() != PDT_STRING)
		{
			throw CCluError("Name of element has to be a string");
		}

		csElName = rElList(0).GetStringPtr()->Str();

		if (rElList(1).BaseType() != PDT_VARLIST)
		{
			throw CCluError("Second part of element has to contain a list");
		}

		TVarList& rElTree = *rElList(1).GetVarListPtr();

		// Loop over all elements with same tag
		int iIdx, iCnt = int(rElTree.Count());
		for (iIdx = 0; iIdx < iCnt; iIdx++)
		{
			xmlEl.Reset();
			xmlEl.GetName() = csElName;

			if (rElTree(iIdx).BaseType() != PDT_VARLIST)
			{
				throw CCluError("Content part of element has to be a list");
			}

			TVarList& rElProp = *rElTree(iIdx).GetVarListPtr();

			int iProp, iPropCnt = int(rElProp.Count());
			/*
			if (iPropCnt < 2)
			{
			        throw CCLUError("Element data has incorrect form. Expect '_content' and '_subtree' elements");
			}
			*/

			for (iProp = 0; iProp < iPropCnt; iProp++)
			{
				if (rElProp(iProp).BaseType() != PDT_VARLIST)
				{
					throw CCluError("Parts of element data have to be lists");
				}

				TVarList& rProp = *rElProp(iProp).GetVarListPtr();

				if (rProp.Count() != 2)
				{
					throw CCluError("Properties of element data have to consist of value pairs");
				}

				if (rProp(0).BaseType() != PDT_STRING)
				{
					throw CCluError("Name of property has to be a string");
				}

				TString& rPropName = *rProp(0).GetStringPtr();
				TString csVal;

				if (rPropName == "_content")
				{
					if (!rCB.CastToString(rProp(1), csVal))
					{
						throw CCluError("Cannot cast content value to string");
					}

					xmlEl.GetContent() = csVal.Str();
				}
				else if (rPropName == "_subtree")
				{
					// Write Subtree of Root Element
					if (rProp(1).BaseType() != PDT_VARLIST)
					{
						throw CCluError("Value of '_subtree' has to be a list");
					}

					WriteXMLTree(rCB, *rProp(1).GetVarListPtr(), xmlEl.GetSubTree(), iLine, iPos);
				}
				else
				{
					if (!rCB.CastToString(rProp(1), csVal))
					{
						throw CCluError("Cannot cast property value to string");
					}

					xmlEl.AddProp(rPropName.Str(), csVal.Str());
				}
			}

			rTree.AddElement(xmlEl);
		}
	}
}

//////////////////////////////////////////////////////////////////////
/// Read Text Function
///
/// 1. Filename
/// 2. Variable in which file content is stored as string.
///
///		Returns 'OK' if no error occured, and error string otherwise.

bool ReadTextFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	CStrMem csFilename;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 2)
	{
		int piPar[] = { 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().InvalidType(mVars(0), iLine, iPos);
		return false;
	}

	mVars(1).New(PDT_STRING);
	TString& rText = *mVars(1).GetStringPtr();

	csFilename = mVars(0).ValStr();

	ifstream zData;
	char pcCurPath[500], pcLine[1025];

	_getcwd(pcCurPath, 499);
	_chdir(rCB.GetScriptPath().c_str());

	zData.open(csFilename.Str());
	if (!zData.is_open())
	{
		_chdir(pcCurPath);

		char pcText[500];
		sprintf_s(pcText, "Text file '%s' could not be opened.", csFilename.Str());

		rVar = pcText;
		//rCB.GetErrorList().GeneralError(pcText);
		return true;
	}

	do
	{
		zData.read(pcLine, 1024);
		pcLine[zData.gcount()] = 0;
		rText << pcLine;
	}
	while (!zData.eof());

	zData.close();

	_chdir(pcCurPath);
	rVar = "OK";

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Write Text Function
///
/// 1. Filename
/// 2. String Variable that is to be written.
///
///		Returns 'OK' if no error occured, and error string otherwise.

bool WriteTextFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	CStrMem csFilename;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 2)
	{
		int piPar[] = { 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("No filename given.", iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Expect second parameter to be a string.", iLine, iPos);
		return false;
	}

	TString& rText = *mVars(1).GetStringPtr();

	csFilename = mVars(0).ValStr();

	ofstream zData;
	char pcCurPath[500];

	_getcwd(pcCurPath, 499);
	_chdir(rCB.GetScriptPath().c_str());

	zData.open(csFilename.Str());
	if (!zData.is_open())
	{
		_chdir(pcCurPath);

		char pcText[500];
		sprintf_s(pcText, "Text file '%s' could not be opened for writing.", csFilename.Str());

		rVar = pcText;
		//rCB.GetErrorList().GeneralError(pcText);
		return true;
	}

	zData.write(rText.Str(), rText.Len());
	zData.close();

	_chdir(pcCurPath);
	rVar = "OK";

	return true;
}

