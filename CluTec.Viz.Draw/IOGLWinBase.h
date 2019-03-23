////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      IOGLWinBase.h
//
// summary:   Declares the iogl window base class
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

// IOGLWinBase.h: Interface for defining adapting CLUdraw to different window systems
//				  like GLUT (openGL utilities) or MFC (Microsoft Foundation Classes)
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IOGLWINBASE_H__6EB9C281_EE5B_11D5_BA34_00E07D8AFD6A__INCLUDED_)
#define AFX_IOGLWINBASE_H__6EB9C281_EE5B_11D5_BA34_00E07D8AFD6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLUDRAW_API IOGLWinBase  
{
public:
	 IOGLWinBase() {}
	 ~IOGLWinBase(){};
	// overload these in (maybe all) derived classes to do additional stuff 
	virtual bool Init( bool bFirst ) = 0;
	virtual void Display() = 0;

	// the following virtual functions are defined in the window system dependent derived classes only!
	// see COGLWinGlut or COGLWinMFCstatic for examples	

	// create: this should do all necessary init stuff and create a window (e.g.: in GLUT) 
	//	if not existing (e.g.: with use in MFC)
	virtual bool Create(int *piArg, char **pcArg, 
		char *pcName, int iPosX , int iPosY,
		int iSizeX, int iSizeY, unsigned int uMode) = 0; 
	
	virtual bool BeginDraw() = 0;
	virtual bool EndDraw() =0 ; 
	virtual void GetModifiers() = 0;

	virtual bool StartAnimation(unsigned int uiTime) = 0;
	virtual bool StopAnimation() = 0;

	virtual bool AtAnimStart() = 0;
	virtual bool TimedFunc() = 0;
	virtual bool AtAnimEnd() = 0;

	virtual void SetBGColor(COGLColor &rBGCol) = 0;
	virtual void SetBoxColor(COGLColor &rCol, COGLColor &rXCol, COGLColor &rYCol, COGLColor &rZCol) = 0;

	virtual unsigned char CLU_LEFT_BUTTON() = 0;
	virtual unsigned char CLU_RIGHT_BUTTON() = 0;
	virtual unsigned char CLU_DOWN() = 0;
	virtual unsigned char CLU_UP() = 0;

protected:
	// connect these functions with their window system specific events, like "Mouse Button pressed" etc.
	// don't overload them in the window system specific class, do that in further derived classes
	virtual void Reshape(int iWidth, int iHeight) = 0;
	virtual void Mouse(int iButton, int iState, int iX, int iY) = 0;
	virtual void Key(unsigned char cKey, int iX, int iY) = 0;
	virtual void ActiveMouseMove(int iX, int iY) = 0;
	virtual void IdleFunc() = 0;
};

#endif // !defined(AFX_IOGLWINBASE_H__6EB9C281_EE5B_11D5_BA34_00E07D8AFD6A__INCLUDED_)
