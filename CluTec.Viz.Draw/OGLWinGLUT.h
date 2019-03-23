////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLWinGLUT.h
//
// summary:   Declares the ogl window GLUT class
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

// OGLWinGLUT.h: Schnittstelle f�r die Klasse COGLWinGLUT.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLWINGLUT_H__6EB9C282_EE5B_11D5_BA34_00E07D8AFD6A__INCLUDED_)
#define AFX_OGLWINGLUT_H__6EB9C282_EE5B_11D5_BA34_00E07D8AFD6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CvFltk\glut.h"
#include "time.h"
#include "sys/timeb.h"

#include "CLUDrawBase.h"
#include "IOGLWinBase.h"

#include <map>

using std::map;

#ifdef WIN32
	// Macro that is used by Windows and only confuses us here.
	#ifdef CreateWindow
		#undef CreateWindow
	#endif
	typedef struct _timeb TTimeStruct;
	#define GETTIME(S) _ftime(S)
#else
	typedef struct timeb TTimeStruct;
	#define GETTIME(S) ftime(S)
#endif

/*
struct SGLUTWin
{
	char *pcName;
	int iPosX, iPosY;
	int iSizeX, iSizeY;
	unsigned int uMode;

	SGLUTWin() 
	{ 
		pcName = "CLUDraw v1.1 by C.Perwass";
		iPosX = 100; iPosY = 100;
		iSizeX = 500; iSizeY = 500;
		uMode = GLUT_DOUBLE | GLUT_RGB;
	}
};
*/

class COGLWinGLUT : public CCLUDrawBase 
{
public:
	COGLWinGLUT();
	virtual ~COGLWinGLUT();
		
	// Enter Main Loop
	void Run();
	void GetModifiers();
	
	bool IsCreated() { if (m_iHWin != 0) return true; return false; }

	void NotifyDestroy() { m_bNotifyDestroy  = true; }
	bool ShouldDestroy() { return m_bNotifyDestroy; }
	
	virtual bool Create(int *piArg = 0, char **pcArg = 0, 
		char *pcName = "CLUDraw v1.1 by C. Perwass",
		int iPosX = 100, int iPosY = 100,
		int iSizeX = 500, int iSizeY = 500, 
		unsigned int umode = GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	/// Create a GLUT window without calling glutInit.
	/// This can be used if GLUT has already been
	/// initialized by this process.
	virtual bool CreateWindow(
		const char *pcName = "CLUDraw v1.1 by C. Perwass",
		int iPosX = 100, int iPosY = 100,
		int iSizeX = 500, int iSizeY = 500, 
		unsigned int umode = GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	
	/// Detach this window from the GLUT loop.
	virtual bool DestroyWindow();
	
//	virtual bool Create(int *piArg, char **pcArg, MemObj<SGLUTWin>& mWin);

	virtual bool StartAnimation(unsigned int uiTime); // idlefunc is called each fTime milliseconds
	virtual bool StopAnimation();

	// Post Main Window Redisplay to run through all idle loops
	virtual void PostMainWindowUpdate();

	unsigned char CLU_LEFT_BUTTON() {return GLUT_LEFT_BUTTON;};
	unsigned char CLU_RIGHT_BUTTON() {return GLUT_RIGHT_BUTTON;};	
	unsigned char CLU_DOWN() {return GLUT_DOWN;};
	unsigned char CLU_UP() {return GLUT_UP;};

protected:
	virtual bool BeginDraw();
	virtual bool EndDraw();
	virtual void PostWindowUpdate();

	static void GLUTDisplay();
	static void GLUTReshape(int iWidth, int iHeight);
	static void GLUTMouse(int iButton, int iState, int iX, int iY);
	static void GLUTKey(unsigned char cKey, int iX, int iY);
	static void GLUTActiveMouseMove(int iX, int iY);
	static void GLUTIdleFunc();
	static void GLUTTimedFunc(int value);
	static void GLUTMenu(int iIdx);
	
	void calcTimeElapsed();

	int m_iHWin;
	TTimeStruct m_tmStart, m_tmPrev;

	bool m_bNotifyDestroy;

	// Main Window Handle
	static int m_iHMainWin;
	
	// map from window handle to this pointer.
	static map<int,COGLWinGLUT*> m_mapThis; 
};


#endif // !defined(AFX_OGLWINGLUT_H__6EB9C282_EE5B_11D5_BA34_00E07D8AFD6A__INCLUDED_)
