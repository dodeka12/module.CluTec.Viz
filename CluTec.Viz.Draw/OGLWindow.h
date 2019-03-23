////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLWindow.h
//
// summary:   Declares the ogl Windows Form
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

// OGLWindow.h: Schnittstelle f�r die Klasse OGLWindow.
// 
// This class does all the initialisation of OpenGL to obtain
// a working context.
// It does not initialize the perspective, etc.
//
//////////////////////////////////////////////////////////////////////



#if !defined(AFX_OGLWINDOW_H__6A0F3602_2E0D_11D2_99B1_845806C10000__INCLUDED_)
#define AFX_OGLWINDOW_H__6A0F3602_2E0D_11D2_99B1_845806C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



#define OGL_STDPFD { \
        sizeof(PIXELFORMATDESCRIPTOR), \
        1,                             \
        PFD_DRAW_TO_WINDOW |           \
        PFD_SUPPORT_OPENGL |           \
        PFD_DOUBLEBUFFER,              \
        PFD_TYPE_RGBA,                 \
        24,                            \
        0, 0, 0, 0, 0, 0,              \
        0,                             \
        0,                             \
        0,                             \
        0, 0, 0, 0,                    \
				16,	                           \
        0,                             \
				0,														 \
				PFD_MAIN_PLANE,								 \
        0,                             \
        0, 0, 0                        \
  };

//        PFD_MAIN_PLANE,                
//        0,                             

class CLUDRAW_API COGLWindow
{
public:
	COGLWindow();
	virtual ~COGLWindow();
	COGLWindow(CWnd& nWnd);

	virtual bool OGLCreate(CWnd& nWnd);
	virtual bool OGLDestroy();

	virtual bool BeginDraw(HDC *pHDC = 0);
	virtual bool EndDraw();

	bool SwapBuffers();

	bool SetOGLContext(HDC *pHDC = 0);

	HGLRC GetRCHandle() { return m_hRC; }

	bool IsOK() { return m_bIsOK; }
	WORD Width() { return m_wWidth; }
	WORD Height() { return m_wHeight; }
	double AspectRatio() { return m_dAspect; }

	void SetOverlayPlanes(char op)
		{ m_PFD.bReserved = (m_PFD.bReserved & 0xF0) | (op & 0x0F); }
	void SetUnderlayPlanes(char up)
		{ m_PFD.bReserved = (m_PFD.bReserved & 0x0F) | ((up & 0x0F) << 4); }

protected:
	bool SetupPixelFormat();


protected:
	bool m_bIsOK;

	CWnd *m_pWnd;
	HGLRC m_hRC;
	CDC *m_pDC;

	WORD m_wWidth, m_wHeight;
	double m_dAspect;

	PIXELFORMATDESCRIPTOR m_PFD, m_ActPFD;
};

#endif // !defined(AFX_OGLWINDOW_H__6A0F3602_2E0D_11D2_99B1_845806C10000__INCLUDED_)
