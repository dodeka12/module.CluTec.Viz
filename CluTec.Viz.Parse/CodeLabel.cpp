////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CodeLabel.cpp
//
// summary:   Implements the code label class
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

#include "StdAfx.h"
#include "CodeLabel.h"
#include "CodeBase.h"
#include "CodeVar.h"
#include "OCIDSymDef.h"

#include "CluTec.Base/Logger.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCodeLabel::CCodeLabel()
{
	m_StdVar.New(PDT_INT, "Unnamed");
	m_StdVar = (int) 0;
}

CCodeLabel::~CCodeLabel()
{

}


//////////////////////////////////////////////////////////////////////
/// Apply
/// If label refers to variable of type PDT_CODEPTR then calls Apply on Code Ptr.
/// otherwise it places the variable on the stack.
/// If label is unknown then variable is created with type PDT_INT.

bool CCodeLabel::Apply(CCodeBase* pCodeBase, SCodeData *pData)
{
	if (!pCodeBase)
		return false;

	int iLine, iPos;
	if (pData)
	{
		iLine = pData->iTextLine;
		iPos = pData->iTextPos;
	}

	const char* pcNamespace;
	CStrMem csVarName;
	if (m_csName[0] == OC_IDSYM_GLOBAL_CHAR)
	{
		csVarName = &(m_csName.Str()[1]);
		pcNamespace = NS_GLOBAL;
	}
	else
	{
		csVarName = m_csName;
		pcNamespace = NS_CURRENT;
	}

	CCodeVar& rVar = pCodeBase->GetVar(csVarName, pcNamespace);

	if (rVar.Type() == PDT_NOTYPE) // variable does not exist
	{
		CCodeVar& rNewVar = pCodeBase->NewVar(csVarName, PDT_NOTYPE, pcNamespace);
		
		//if (rNewVar.Type() == PDT_NOTYPE)
		//{
		//	pCodeBase->m_ErrorList.Internal(iLine, iPos);
		//	return false;
		//}
	
		//// Set Standard Variable
		//rNewVar = m_StdVar;
		pCodeBase->Push(&rNewVar);
	}
/*	else if (rVar.Type() == PDT_CODEPTR) // execute code element
	{
		CCodeElement *pCE = (CCodeElement *) rVar.Val();

		if (!pCE)
		{
			pCodeBase->m_ErrorList.Internal(iLine, iPos);
			return false;
		}

		return pCE->Apply(pCodeBase);
	}
*/	else // Place var on stack
	{
//#if defined(_DEBUG) //&& defined(CLU_TRACE)
//		if ( rVar.Type() == PDT_CODEPTR )
//		{
//			std::string sText;
//			sText = "CLUViz>  Function call: " + rVar.Name();
//
//			CLU_LOG(sText.c_str());
//			//::OutputDebugStringA(sText.Str());
//		}
//#endif

		if (!pCodeBase->Push(&rVar))
		{
			pCodeBase->m_ErrorList.Internal(iLine, iPos);
			return false;
		}
	}

	return true;
}

