////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CodeVar.h
//
// summary:   Declares the code variable class
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

#pragma once

#include <set>
#include "CodeElement.h"
#include "VarList.h"

#include "CluTec.Viz.Base\matrix.h"
#include "CluTec.Viz.Base\TensorData.h"
#include "CluTec.Viz.Base\TensorIdx.h"
#include "CluTec.Viz.GA.Base\multiv.h"

#include "CluTec.Viz.Draw\OGLVertexList.h"
#include "CluTec.Viz.Draw\ImageRepository.h"

enum ECodeDataType
{
	PDT_NOTYPE,
	PDT_STRING,
	PDT_INT,
	PDT_UINT,
	PDT_LONG,
	PDT_FLOAT,
	PDT_DOUBLE,
	PDT_MULTIV,
	PDT_MATRIX,
	PDT_TENSOR,
	PDT_TENSOR_IDX,
	PDT_COLOR,
	PDT_CODEPTR,
	PDT_VARLIST,
	PDT_VEXLIST,
	PDT_IMAGE,
	PDT_SCENE,
	PDT_PTR_STRING,
	PDT_PTR_INT,
	PDT_PTR_UINT,
	PDT_PTR_LONG,
	PDT_PTR_FLOAT,
	PDT_PTR_DOUBLE,
	PDT_PTR_MULTIV,
	PDT_PTR_MATRIX,
	PDT_PTR_TENSOR,
	PDT_PTR_TENSOR_IDX,
	PDT_PTR_COLOR,
	PDT_PTR_CODEPTR,
	PDT_PTR_VARLIST,
	PDT_PTR_VEXLIST,
	PDT_PTR_IMAGE,
	PDT_PTR_SCENE,
	PDT_PTR_VAR,
	PDT_MAX_TYPE,
	PDT_SCALAR      = PDT_DOUBLE,
	PDT_COUNTER     = PDT_INT,
	PDT_PTR_SCALAR  = PDT_PTR_DOUBLE,
	PDT_PTR_COUNTER = PDT_PTR_INT
};

typedef int TCVCounter;
typedef double TCVScalar;
typedef Matrix<TCVScalar> TMatrix;
typedef CTensorData<TCVScalar> TTensor;
typedef CTensorIdx<TCVScalar> TTensorIdx;
typedef MultiV<TCVScalar> TMultiV;
typedef CCodeElement* TCodePtr;
typedef CVarList* TVarListPtr;
typedef CVarList TVarList;
typedef COGLVertexList TVexList;
typedef COGLColor TOGLColor;
typedef CStrMem TString;
typedef CImageReference TImage;
typedef COGLBEReference TScene;

class CCodeVar
{
public:

	typedef TString*        TStringPtr;
	typedef int*            TIntPtr;
	typedef uint*           TUIntPtr;
	typedef long*           TLongPtr;
	typedef float*          TFloatPtr;
	typedef double*         TDoublePtr;
	typedef TMatrix*        TMatrixPtr;
	typedef TTensor*        TTensorPtr;
	typedef TTensorIdx*     TTensorIdxPtr;
	typedef TMultiV*        TMultiVPtr;
	typedef TOGLColor*      TOGLColorPtr;
	typedef TCodePtr*       TCodePtrPtr;
	typedef TVarList*       TVarListPtr;
	typedef TVexList*       TVexListPtr;
	typedef TImage*         TImagePtr;
	typedef TScene*         TScenePtr;
	typedef CCodeVar*       TVarPtr;

public:

	CCodeVar();
	CCodeVar(const CCodeVar& rVar);
	virtual ~CCodeVar();

	bool New(ECodeDataType _nType, const char* pcName = 0);
	bool Delete(bool bForce = false);

	// Reset all references to this variable and delete variable content.
	void Destroy();

	void EnableProtect(bool bVal = true) { m_bProtected = bVal; }
	bool IsProtected() { return m_bProtected; }

	// Also copies Name of variable!
	CCodeVar& CopyInstance(const CCodeVar& rVar);

	// Set Pointer Content
	// Sets content is this variable is pointer.
	// rVar has to be of same type as pointer content.
	// If bAllowCast = true then it is attempted to cast rVar appropriately.
	bool SetPtrContent(const CCodeVar& rVar, bool bAllowCast = true);

	// Takes content from rVar and sets it as content of this var without
	// copying anything. If the process is successful, rVar is of type
	// PDT_NOTYPE on return.
	bool SetContent(CCodeVar& rVar);

	// Only copies value of variable
	CCodeVar& operator=(const CCodeVar& rVar);
	CCodeVar& operator=(const TVarPtr& rVar);

	CCodeVar& operator=(const TString& rVar);
	CCodeVar& operator=(const char* pcVar);
	CCodeVar& operator=(const int& rVar);
	CCodeVar& operator=(const uint& rVar);
	CCodeVar& operator=(const long& rVar);
	CCodeVar& operator=(const float& rVar);
	CCodeVar& operator=(const double& rVar);
	CCodeVar& operator=(const TMultiV& rVar);
	CCodeVar& operator=(const TMatrix& rVar);
	CCodeVar& operator=(const TTensor& rVar);
	CCodeVar& operator=(const TTensorIdx& rVar);
	CCodeVar& operator=(const TOGLColor& rVar);
	CCodeVar& operator=(const TCodePtr& pVar);
	CCodeVar& operator=(const TVarList& pVar);
	CCodeVar& operator=(const TVexList& pVar);
	CCodeVar& operator=(const TImage& pVar);
	CCodeVar& operator=(const TScene& pVar);

	CCodeVar& operator=(const TStringPtr& rVar);
	CCodeVar& operator=(const TIntPtr& rVar);
	CCodeVar& operator=(const TUIntPtr& rVar);
	CCodeVar& operator=(const TLongPtr& rVar);
	CCodeVar& operator=(const TFloatPtr& rVar);
	CCodeVar& operator=(const TDoublePtr& rVar);
	CCodeVar& operator=(const TMultiVPtr& rVar);
	CCodeVar& operator=(const TMatrixPtr& rVar);
	CCodeVar& operator=(const TTensorPtr& rVar);
	CCodeVar& operator=(const TTensorIdxPtr& rVar);
	CCodeVar& operator=(const TOGLColorPtr& rVar);
	CCodeVar& operator=(const TCodePtrPtr& rVar);
	CCodeVar& operator=(const TVarListPtr& rVar);
	CCodeVar& operator=(const TVexListPtr& rVar);
	CCodeVar& operator=(const TImagePtr& rVar);
	CCodeVar& operator=(const TScenePtr& rVar);

	int* GetIntPtr();
	uint* GetUIntPtr();
	long* GetLongPtr();
	float* GetFloatPtr();
	double* GetDoublePtr();
	TCVScalar* GetScalarPtr();
	TCVCounter* GetCounterPtr();
	TString* GetStringPtr();
	TMultiV* GetMultiVPtr();
	TMatrix* GetMatrixPtr();
	TTensor* GetTensorPtr();
	TTensorIdx* GetTensorIdxPtr();
	TOGLColor* GetOGLColorPtr();
	TCodePtr* GetCodePtrPtr();
	TVarList* GetVarListPtr();
	TVexList* GetVexListPtr();
	TImage* GetImagePtr();
	TScene* GetScenePtr();
	const TScene* GetScenePtr() const;

	bool CastToScalar(TCVScalar& Val, TCVScalar fPrec = 0) const;

	// If bExact == true then float and double are not
	// converted to counter. Instead false is returned.
	bool CastToCounter(TCVCounter& Val, bool bExact = true) const;

	// Convert Single Element List to Single Element
	CCodeVar& ConvertSELtoSE();

	// Convert Single Element to Single Element List
	CCodeVar& ConvertSEtoSEL();

	// Dereference Pointer
	CCodeVar& DereferencePtr();

	// Dereference Variable Pointer
	// If bComplete == true, then variable is dereferenced until it is
	// not a pointer to a variable anymore.
	CCodeVar& DereferenceVarPtr(bool bComplete = false);

	// Register a variable that references this one
	void RegisterRefVar(CCodeVar* pVar);

	// DeRegister a variable that references this one
	void DeRegisterRefVar(CCodeVar* pVar);

	// Tell this variable that its reference is no longer valid
	void InvalidateRefVar();

	// Tell all variables that reference this one that their reference is invalid
	void InvalidateReferences();

	// Check whether variable is pointer
	bool IsPtr() { return m_bIsPtr; }

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 	Gets the name.
	/// </summary>
	///
	/// <returns>	The name. </returns>
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::string& Name()
	{
		return m_sName;
	}

	ECodeDataType Type() { return m_nType; }
	ECodeDataType BaseType();

	std::string TypeStr() const;
	static std::string TypeStr(ECodeDataType eType);

	bool IsTypeValid()
	{
		if ((m_nType < PDT_NOTYPE) || (m_nType >= PDT_MAX_TYPE)) { return false; }
		else{ return true; }
	}

	void* Val() { return m_pData; }

	// Returns value as string
	TString ValStr();

public:

	// The image repository
	// Image variables only contain references to images in this repository.
	// Repository is held in CLUGL.dll
	CImageRepository* m_pImgRep;

protected:

	bool SetVar(ECodeDataType _nType, void* _pData, const char* pcName = 0);

	union UData
	{
		int Int;
		uint UInt;
		long Long;
		float Float;
		double Double;
		void* Ptr;
	} m_xData;

	ECodeDataType   m_nType;
	void* m_pData;		// Data of type nType

	std::string m_sName;

	// If true variable cannot be overwritten, or deleted.
	bool m_bProtected;

	// True if stored variable is pointer
	bool m_bIsPtr;

	// List of CodeVars that reference this CodeVar
	std::set<CCodeVar*> m_setRefBy;
};
