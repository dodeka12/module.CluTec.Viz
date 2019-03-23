////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLFilterN3.h
//
// summary:   Declares the ogl filter n 3 class
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

// OGLFilterN3.h: Schnittstelle f�r die Klasse COGLFilterN3.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLFILTERN3_H__9301AEE9_2C49_4AFC_8E87_DE0B12AAE99A__INCLUDED_)
#define AFX_OGLFILTERN3_H__9301AEE9_2C49_4AFC_8E87_DE0B12AAE99A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLMVFilterBase.h"
#include "OGLDrawBase.h"
#include "OGLFilterP3.h"


#define N3M_AUTO	(ID_CONFGA << 16) + 0x0001
#define N3M_BLADE	(ID_CONFGA << 16) + 0x0002
#define N3M_VERSOR	(ID_CONFGA << 16) + 0x0003
#define N3M_OPNS	(ID_CONFGA << 16) + 0x0004
#define N3M_IPNS	(ID_CONFGA << 16) + 0x0005
#define N3M_SOLID	(ID_CONFGA << 16) + 0x0006
#define N3M_WIRE	(ID_CONFGA << 16) + 0x0007
#define N3M_SHOW_IMAG	(ID_CONFGA << 16) + 0x0008
#define N3M_HIDE_IMAG	(ID_CONFGA << 16) + 0x0009


class CLUDRAW_API COGLFilterN3 : public COGLMVFilterBase  
{

public:
	COGLFilterN3();
	virtual ~COGLFilterN3();

	virtual bool DrawMV(const MultiV<double>& vA, bool bAnalyzeOnly = false);
	virtual bool DrawMV(const MultiV<float>& vA, bool bAnalyzeOnly = false);
	virtual bool SetMode(int iMode);
	virtual bool ResetModes();

	bool DrawMVInfo(CMVInfo<float>& rInfo);

	void EnableSolidObjects(bool bSolid = true) { m_bSolidObjects = bSolid; }
	void EnableOPNSMode(bool bOPNS = true) { m_bDrawOPNS = bOPNS; }

protected:
	COGLFilterP3 m_P3Filter;
	
	bool m_bSolidObjects;
	bool m_bDrawOPNS;		// Draw Outer Product Nullspace of blades
	bool m_bMVIsVersor;		// Interpret Multivectors as versors
	bool m_bMVIsBlade;		// Interpret Multivectors as blade
};

#endif // !defined(AFX_OGLFILTERN3_H__9301AEE9_2C49_4AFC_8E87_DE0B12AAE99A__INCLUDED_)
