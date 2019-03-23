////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CodeLabel.h
//
// summary:   Declares the code label class
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

#if !defined(AFX_CODELABEL_H__C8137DD9_E080_487E_A460_ABB85EFD0EEB__INCLUDED_)
#define AFX_CODELABEL_H__C8137DD9_E080_487E_A460_ABB85EFD0EEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeElement.h"

#include "CodeVar.h"


class CCodeLabel : public CCodeElement  
{
public:
	CCodeLabel();
	virtual ~CCodeLabel();

	bool SetLabel(const char *pcLabel)
	{ if (!pcLabel || *pcLabel == 0) return false; else SetName(pcLabel); return true; }

	virtual bool Apply(CCodeBase* pCodeBase, SCodeData *pData = 0);

	CCodeVar& RefStdVar() { return m_StdVar; }

protected:
	CCodeVar m_StdVar;
};

#endif // !defined(AFX_CODELABEL_H__C8137DD9_E080_487E_A460_ABB85EFD0EEB__INCLUDED_)
