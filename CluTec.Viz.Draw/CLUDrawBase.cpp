////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      CLUDrawBase.cpp
//
// summary:   Implements the clu draw base class
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
#include <iostream>
#include "CLUDrawBase.h"
#include "stdlib.h"
#include "CluTec.Viz.Fltk\Fl_math.h"
#include "CluTec.Math\Static.Matrix.Math.h"

#ifdef _GNUCPP3_
	#include <unistd.h>
	#define _getcwd getcwd
	#define _chdir chdir
#else
	#include <direct.h>
	#include <io.h>
#endif

#include "CluTec.Viz.OpenGL.Extensions\Extensions.h"
#include "CluTec.Viz.OpenGL\Api.h"

#include "IL\il.h"
#include "IL\ilu.h"
#include "IL\ilut.h"

#include <fstream>
using namespace std;

//extern "C"
//{
//int ConvertJPEGtoPS(const char* pcSrcFilename, const char* pcTrgFilename);
//}

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCLUDrawBase::CCLUDrawBase()
{
	m_bMouseLDown = false;
	m_bMouseRDown = false;
	m_bShiftDown  = false;
	m_bCtrlDown   = false;
	m_bAltDown    = false;

	m_bNumLock    = false;
	m_bScrollLock = false;
	m_bCapsLock   = false;

	m_b2DViewEnabled = false;

	m_bPickMouseOver    = false;
	m_bPickMouseClick   = false;
	m_bOGLMouse         = false;
	m_bDrawColorStereo  = false;
	m_bDrawTransparency = true;

	m_bSendControlKeyEvents  = false;	// Send control key events to script
	m_bSendFunctionKeyEvents = false;	// Send function key events to script

	m_pbColorMaskLeft[0] = true;
	m_pbColorMaskLeft[1] = false;
	m_pbColorMaskLeft[2] = false;
	m_pbColorMaskLeft[3] = true;

	m_pbColorMaskRight[0] = false;
	m_pbColorMaskRight[1] = true;
	m_pbColorMaskRight[2] = true;
	m_pbColorMaskRight[3] = true;

	m_fPi        = float(asin(1.0) * 2.0);
	m_fRadPerDeg = m_fPi / 180.0f;

	m_fColorStereoSep      = 0.04f;
	m_fColorStereoDegAngle = 0.48f * m_fRadPerDeg;

	m_bUseLighting = false;

	m_bUseLocalRot = false;

	m_bIsAnimated  = false;
	m_iAnimRotType = 0;

	// Standard number of depth levels
	m_iDepthLevelCount = 21;
	m_dDepthLevelWidth = 1.0 / double(m_iDepthLevelCount);
	m_dDepthLevelGap   = 0.05 * m_dDepthLevelWidth;
	// Near depth value of central depth level
	m_dDepthLevelOrig = double(m_iDepthLevelCount >> 1) * m_dDepthLevelWidth;

	m_iPosX = 0;
	m_iPosY = 0;

	m_vRMain     = m_E3Base.vSC;
	m_vRViewMain = m_E3Base.vSC;
	m_vU         = m_E3Base.vSC;
	m_vAxis      = m_E3Base.vSC;

	m_vU    = 0;
	m_vAxis = 0;

	m_xPers.Set(70.0f, 1.0f, 0.1f, 100.0f);
	m_xOrtho.Set(-5, 5, -5, 5, -1, 10);
	m_fOrthoFac     = 0.01f;
	m_fOrthoZoomFac = 1e-4f;

	m_iAnimPause   = 33;
	m_iVisTimeStep = 33;

	m_iRTMode     = 0;
	m_iStdRTMode  = 0;
	m_fFrameAngle = 0;

	m_uMaxTransforms = 11;
	m_vxTransform.resize(m_uMaxTransforms);

	unsigned i = 0;
	for (i = 0; i < m_uMaxTransforms; i++)
	{
		m_vxTransform[i].Reset();
	}

	m_xCameraTransform.Reset();

	Red.Set(1.0f, 0.0f, 0.0f);
	Green.Set(0.0f, 1.0f, 0.0f);
	Blue.Set(0.0f, 0.0f, 1.0f);

	Cyan.Set(0.0f, 1.0f, 1.0f);
	Yellow.Set(1.0f, 1.0f, 0.0f);
	Magenta.Set(1.0f, 0.0f, 1.0f);

	Orange.Set(1.0f, 0.7f, 0.0f);
	White.Set(1.0f, 1.0f, 1.0f);
	Black.Set(0.0f, 0.0f, 0.0f);

	DRed = 0.3f * Red;
	MRed = 0.6f * Red;

	DBlue = 0.3f * Blue;
	MBlue = 0.6f * Blue;

	DGreen = 0.3f * Green;
	MGreen = 0.6f * Green;

	DCyan = 0.3f * Cyan;
	MCyan = 0.6f * Cyan;

	DYellow = 0.3f * Yellow;
	MYellow = 0.6f * Yellow;

	DMagenta = 0.3f * Magenta;
	MMagenta = 0.6f * Magenta;

	DOrange = 0.3f * Orange;
	MOrange = 0.6f * Orange;

	DGray = 0.3f * White;
	MGray = 0.6f * White;

	m_BGCol.Set(0.0f, 0.0f, 0.0f, 1.0f);
	m_BoxCol.Set(0.5f, 0.5f, 0.5f, 1.0f);
	m_BoxXCol.Set(0.6f, 0.0f, 0.0f, 1.0f);
	m_BoxYCol.Set(0.0f, 0.6f, 0.0f, 1.0f);
	m_BoxZCol.Set(0.0f, 0.0f, 0.6f, 1.0f);

	m_fRotFac   = 2.0f;
	m_fTransFac = 0.05f;

	m_iSaveScreenNo    = 0;
	m_fTimeStep        = 0;
	m_fTotalTime       = 0;
	m_uAnimateTimeStep = 0;

	for (i = 0; i < MRHLEN; i++)
	{
		m_pvMouseRotHist[i] = m_E3Base.vZero;
	}

	m_iMRHPos     = 0;
	m_bMRHChanged = false;

	ResetStdLight();

	m_iGLVMinor   = 0;
	m_iGLVMajor   = 0;
	m_iGLSLVMinor = 0;
	m_iGLSLVMajor = 0;

	m_bGLHas_MultiTexture = false;
	m_bGLHas_Shader       = false;
	m_bGLCan_Multisample  = false;

	InitTime();
}

CCLUDrawBase::~CCLUDrawBase()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Initialize time

void CCLUDrawBase::InitTime()
{
	if (::QueryPerformanceFrequency((LARGE_INTEGER*) &m_liTimeFreq) == FALSE)
	//{
	//	m_liTimeFreq /= __int64(1000);
	//}
	//else
	{
		m_liTimeFreq = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Get time in seconds as double

double CCLUDrawBase::GetTime()
{
	__int64 liCount;
	double dTime;

	if (::QueryPerformanceCounter((LARGE_INTEGER*) &liCount) && (m_liTimeFreq > 0))
	{
		dTime = 1000.0 * double(liCount) / double(m_liTimeFreq);
	}
	else
	{
		dTime = 0.0;
	}

	return dTime;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Get time in seconds as Int64

__int64 CCLUDrawBase::GetTime64()
{
	__int64 liCount;
	__int64 liTime;

	if (::QueryPerformanceCounter((LARGE_INTEGER*) &liCount) && (m_liTimeFreq > 0))
	{
		liTime = (1000 * liCount) / m_liTimeFreq;
	}
	else
	{
		liTime = 0;
	}

	return liTime;
}

//////////////////////////////////////////////////////////////////////
// Flush everything that has been drawn up to now to the screen

bool CCLUDrawBase::FlushDraw()
{
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Color Stereo Mask

void CCLUDrawBase::SetColorStereoMask(int piMaskLeft[4], int piMaskRight[4])
{
	int i;

	for (i = 0; i < 4; ++i)
	{
		m_pbColorMaskLeft[i]  = (piMaskLeft[i] != 0);
		m_pbColorMaskRight[i] = (piMaskRight[i] != 0);
	}
}

//////////////////////////////////////////////////////////////////////
/// Adjust Rotation Angle

inline void CCLUDrawBase::AdjustRotAngle(float& fDeg)
{
	if (fDeg >= 360.0f) { fDeg -= 360.0f; }
	else if (fDeg < 0)
	{
		fDeg = 360.0f + fDeg;
	}
}

//////////////////////////////////////////////////////////////////////
/// Set Animation Time Step

bool CCLUDrawBase::SetAnimationTimeStep(int iTimeStep)
{
	if (iTimeStep <= 0)
	{
		return false;
	}

	m_iAnimPause = iTimeStep;
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Visualization Time Step

bool CCLUDrawBase::SetVisualizationTimeStep(int iTimeStep)
{
	if ((iTimeStep < 5) || (iTimeStep > 1000))
	{
		return false;
	}

	m_iVisTimeStep = iTimeStep;
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUDrawBase::Init(bool bFirst)
{
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	GLint iMSBufCnt, iMSSampleCnt;
	glGetIntegerv(GL_SAMPLE_BUFFERS, &iMSBufCnt);
	glGetIntegerv(GL_SAMPLES, &iMSSampleCnt);

	if ((iMSBufCnt > 0) && (iMSSampleCnt > 1))
	{
		m_bGLCan_Multisample = true;
	}
	else
	{
		m_bGLCan_Multisample = false;
	}

	glShadeModel(GL_SMOOTH);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (m_bUseLighting)
	{
		m_StdMaterial.Face(GL_FRONT_AND_BACK);
		m_StdMaterial.Ambient(1.0f, 1.0f, 1.0f, 1.0f);
		m_StdMaterial.Diffuse(1.0f, 1.0f, 1.0f, 1.0f);
		m_StdMaterial.Specular(0.8f, 0.8f, 0.8f, 1.0f);
		m_StdMaterial.Shininess(20.0f);

		ResetStdLight();

		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

		glEnable(GL_LIGHTING);

		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
		glEnable(GL_COLOR_MATERIAL);
	}

	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_UPPER_LEFT);

	// Get GL Version
	Clu::OpenGL::Extensions::GetGLVersion(&m_iGLVMajor, &m_iGLVMinor);

	// Get GLSL Version
	if (Clu::OpenGL::Extensions::GetGLSLVersion(&m_iGLSLVMajor, &m_iGLSLVMinor))
	{
		m_bGLHas_Shader = true;
	}

	if ((m_iGLVMajor == 1) && (m_iGLVMinor >= 3))
	{
		m_bGLHas_MultiTexture = true;
	}

	glDisable(GL_TEXTURE_2D);
	return true;
}

bool CCLUDrawBase::ResetStdLight()
{
	m_StdLight.LightNo(0);
	m_StdLight.Ambient(0.5f, 0.5f, 0.5f, 1.0f);
	m_StdLight.Diffuse(0.5f, 0.5f, 0.5f, 1.0f);
	m_StdLight.Specular(0.2f, 0.2f, 0.2f, 1.0f);
	m_StdLight.Position(5.0f, 5.0f, 0.0f, 1.0f);
	m_StdLight.ConstAtt(0.0f);
	m_StdLight.LinAtt(0.2f);
	m_StdLight.QuadAtt(0.0f);
	m_StdLight.Enable();

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
COGLColor CCLUDrawBase::GetBGColor()
{
	return m_BGCol;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCLUDrawBase::SetBGColor(COGLColor& rBGCol)
{
	m_BGCol = rBGCol;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCLUDrawBase::SetBoxColor(COGLColor& rCol, COGLColor& rXCol, COGLColor& rYCol, COGLColor& rZCol)
{
	m_BoxCol  = rCol;
	m_BoxXCol = rXCol;
	m_BoxYCol = rYCol;
	m_BoxZCol = rZCol;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUDrawBase::SetMouseMode(int iMode)
{
	return SetRTMode(iMode);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUDrawBase::GetMouseMode(int& iMode)
{
	iMode = GetActRTMode();
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUDrawBase::SetOGLSize(double dWidth, double dHeight)
{
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUDrawBase::GetOGLSize(double& dWidth, double& dHeight)
{
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUDrawBase::BeginDraw()
{
	try
	{
		float* pfCol = m_BGCol.Data();

		// Unbind any framebuffer that may be bound.
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(pfCol[0], pfCol[1], pfCol[2], 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/************************************************************************/
		/* Begin Reset Matrices                                                 */
		/************************************************************************/
		if (m_bGLHas_MultiTexture)
		{
			int iTexUnitCnt;
			glGetIntegerv(GL_MAX_TEXTURE_UNITS, (GLint*) &iTexUnitCnt);

			glMatrixMode(GL_TEXTURE);
			for (int iTexUnit = 0; iTexUnit < iTexUnitCnt; iTexUnit++)
			{
				glActiveTexture(GL_TEXTURE0 + iTexUnit);
				glLoadIdentity();
			}

			glActiveTexture(GL_TEXTURE0);
		}
		else
		{
			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
		}

		m_SceneApplyData.xMatrixStack.PushAll();

		/************************************************************************/
		/* End Reset Matrices                                                   */
		/************************************************************************/
		glPixelZoom(1.0f, 1.0f);

		if (m_bUseLighting)
		{
			float pfAmbient[] = { 0.8f, 0.8f, 0.8f, 1.0f };
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, pfAmbient);

			ResetStdLight();

			m_StdLight.Apply();
			m_StdMaterial.Apply();
		}

		// Reset Texture
		glBindTexture(GL_TEXTURE_2D, 0);

		// Reset Color
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		// Reset Line width
		glLineWidth(2.0f);

		// Point Parameters
		if (glPointParameterf)
		{
			glPointSize(160.0f);
			glPointParameterf(GL_POINT_SIZE_MIN, 1.0f);
			glPointParameterf(GL_POINT_SIZE_MAX, 240.0f);
			glPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE, 1.0f);

			float pfPntAtt[] = { 1.0f, 2.0f, 4.0f };
			glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, pfPntAtt);
		}
		else
		{
			glPointSize(6.0f);
		}

		// Disable Multisample by default
		glDisable(GL_MULTISAMPLE);

		// Switch lighting on by default
		glEnable(GL_LIGHTING);

		// Do world transformation
		WorldTransform();
		return true;
	}
	catch (Clu::CIException& ex)
	{
		throw CLU_EXCEPTION_NEST(__FUNCTION__, std::move(ex));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUDrawBase::EndDraw()
{
	m_SceneApplyData.xMatrixStack.PopAll();
	return true;
}

//////////////////////////////////////////////////////////////////////
// Transformation of the world called in BeginDraw

void CCLUDrawBase::WorldTransform()
{
	// Do object transformation
	STransform& rTrans = m_vxTransform[0];

	TVec3 vfModelTranslate = TVec3(rTrans.pfTrans[0], rTrans.pfTrans[1], rTrans.pfTrans[2]);
	TVec3 vfViewTranslate  = m_xCameraTransform.GetCameraPosition() * -1.0f;

	float afViewRotation[16];
	m_xCameraTransform.RotationMatToArray(afViewRotation);

	// CAMERA Movement
	// apply camera transformation
	glMultMatrixf(afViewRotation);

	// and translate the camera
	glTranslatef(vfViewTranslate.x(), vfViewTranslate.y(), vfViewTranslate.z());

	glTranslatef(vfModelTranslate.x(), vfModelTranslate.y(), vfModelTranslate.z());

	if (m_bUseLocalRot)		// && !(m_bIsAnimated && m_iAnimRotType))
	{
		glRotatef(rTrans.pfRot[2], 1, 0, 0);
		glRotatef(rTrans.pfRot[0], 0, 1, 0);
		glRotatef(rTrans.pfRot[1], 0, 0, 1);
	}
	else
	{
		m_fFrameAngle = float(2.0 * _acos(double(Scalar(m_vRMain)))) / m_fRadPerDeg;
		//m_vAxis = (((~m_vRViewMain) & m_vRMain(2)) & (m_vRViewMain)) & m_E3Base.vI;
		m_vAxis = (m_vRMain(2)) & m_E3Base.vI;
		//printf("t, A : %f, %s\n", fAngle, m_vAxis.Str());

		rTrans.pfRot[0] = m_vAxis[1];
		rTrans.pfRot[1] = m_vAxis[2];
		rTrans.pfRot[2] = m_vAxis[3];

		glRotatef(m_fFrameAngle, m_vAxis[1], m_vAxis[2], m_vAxis[3]);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 	Resets the 3D view.
/// </summary>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCLUDrawBase::Reset3dView()
{
	// Reset mouse sensitivity to default
	SetTransFac(0.02f);

	// Reset all camera and model transformation
	m_vxTransform[0].Reset();
	m_xCameraTransform.Reset();

	m_vRMain     = m_E3Base.vSC;
	m_vRViewMain = m_E3Base.vSC;
	m_vU         = m_E3Base.vSC;
	m_vAxis      = m_E3Base.vSC;

	m_vU    = 0;
	m_vAxis = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCLUDrawBase::Enable2dView(bool bEnable)
{
	try
	{
		m_b2DViewEnabled = bEnable;

		if (bEnable)
		{
			// Reset original Ortho factor and update Ortho vars.
			m_fOrthoFac      = 0.01f;
			m_xOrtho.fLeft   = -float(m_iSizeX) * 0.5f * m_fOrthoFac;
			m_xOrtho.fRight  = float(m_iSizeX) * 0.5f * m_fOrthoFac;
			m_xOrtho.fBottom = -float(m_iSizeY) * 0.5f * m_fOrthoFac;
			m_xOrtho.fTop    = float(m_iSizeY) * 0.5f * m_fOrthoFac;
			m_xOrtho.fNear   = -100.0f;
			m_xOrtho.fFar    = 100.0f;
		}

		UpdateMatrices();

		m_vxTransform[0].Reset();

		m_xCameraTransform.Reset();

		m_vRMain     = m_E3Base.vSC;
		m_vRViewMain = m_E3Base.vSC;
		m_vU         = m_E3Base.vSC;
		m_vAxis      = m_E3Base.vSC;

		m_vU    = 0;
		m_vAxis = 0;
	}
	catch (Clu::CIException& ex)
	{
		throw CLU_EXCEPTION_NEST(__FUNCTION__, std::move(ex));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCLUDrawBase::UpdateMatrices()
{
	try
	{
		CLU_OGL_CALL(glMatrixMode(GL_PROJECTION));
		CLU_OGL_CALL(glLoadIdentity());

		if (m_b2DViewEnabled)
		{
			CLU_OGL_CALL(glOrtho(m_xOrtho.fLeft, m_xOrtho.fRight, m_xOrtho.fBottom, m_xOrtho.fTop, m_xOrtho.fNear, m_xOrtho.fFar));
		}
		else
		{
			CLU_OGL_CALL(gluPerspective(m_xPers.fAngle, m_xPers.fAspect, m_xPers.fNear, m_xPers.fFar));
		}

		CLU_OGL_CALL(glMatrixMode(GL_MODELVIEW));
		CLU_OGL_CALL(glLoadIdentity());
	}
	catch (Clu::CIException& ex)
	{
		throw CLU_EXCEPTION_NEST(__FUNCTION__, std::move(ex));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCLUDrawBase::SetPerspectiveAngle(GLfloat fAngle, bool bUpdateMatrices)
{
	try
	{
		m_xPers.Set(fAngle, float(m_iSizeX) / float(m_iSizeY), m_xPers.fNear, m_xPers.fFar);

		if (bUpdateMatrices)
		{
			UpdateMatrices();
		}
	}
	catch (Clu::CIException& ex)
	{
		throw CLU_EXCEPTION_NEST(__FUNCTION__, std::move(ex));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCLUDrawBase::SetPerspective(GLfloat fAngle, GLfloat fNear, GLfloat fFar)
{
	try
	{
		m_xPers.Set(fAngle, float(m_iSizeX) / float(m_iSizeY), fNear, fFar);
		UpdateMatrices();

		Display();
	}
	catch (Clu::CIException& ex)
	{
		throw CLU_EXCEPTION_NEST(__FUNCTION__, std::move(ex));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCLUDrawBase::SetOrtho(GLfloat fLeft, GLfloat fRight, GLfloat fBottom, GLfloat fTop, GLfloat fNear, GLfloat fFar)
{
	try
	{
		m_xOrtho.Set(fLeft, fRight, fBottom, fTop, fNear, fFar);
		UpdateMatrices();

		Display();
	}
	catch (Clu::CIException& ex)
	{
		throw CLU_EXCEPTION_NEST(__FUNCTION__, std::move(ex));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCLUDrawBase::DrawBox(float fSize)
{
	float fS = fSize / 2;

	glBegin(GL_LINES);	// front side

	if (m_bUseLighting)
	{
		m_StdMaterial.Diffuse(1.0, 1.0, 1.0, 1.0);
		m_StdMaterial.Ambient(m_BoxXCol);
		glNormal3f(-1, 0, 0);
		glVertex3f(-fS, -fS, -fS);
		glNormal3f(1, 0, 0);
		glVertex3f(fS, -fS, -fS);

		m_StdMaterial.Diffuse(1.0, 1.0, 1.0, 1.0);
		m_StdMaterial.Ambient(m_BoxCol);
		glNormal3f(0, -1, 0);
		glVertex3f(fS, -fS, -fS);
		glNormal3f(0, 1, 0);
		glVertex3f(fS, fS, -fS);

		glNormal3f(1, 0, 0);
		glVertex3f(fS, fS, -fS);
		glNormal3f(-1, 0, 0);
		glVertex3f(-fS, fS, -fS);

		m_StdMaterial.Diffuse(1.0, 1.0, 1.0, 1.0);
		m_StdMaterial.Ambient(m_BoxYCol);
		glNormal3f(0, 1, 0);
		glVertex3f(-fS, fS, -fS);
		glNormal3f(0, -1, 0);
		glVertex3f(-fS, -fS, -fS);
	}
	else
	{
		glVertex3f(-fS, -fS, -fS);
		glVertex3f(fS, -fS, -fS);

		glVertex3f(fS, -fS, -fS);
		glVertex3f(fS, fS, -fS);

		glVertex3f(fS, fS, -fS);
		glVertex3f(-fS, fS, -fS);

		glVertex3f(-fS, fS, -fS);
		glVertex3f(-fS, -fS, -fS);
	}

	// Note that glGetError returns GL_INVALID_OPERATION if it is called between a call to glBegin and its corresponding call to glEnd.
	CLU_OGL_CALL(glEnd());

	m_StdMaterial.Diffuse(1.0, 1.0, 1.0, 1.0);
	m_StdMaterial.Ambient(m_BoxCol);

	glBegin(GL_LINES);	// back side

	if (m_bUseLighting)
	{
		glNormal3f(-1, 0, 0);
		glVertex3f(-fS, -fS, fS);
		glNormal3f(1, 0, 0);
		glVertex3f(fS, -fS, fS);

		glNormal3f(0, -1, 0);
		glVertex3f(fS, -fS, fS);
		glNormal3f(0, 1, 0);
		glVertex3f(fS, fS, fS);

		glNormal3f(1, 0, 0);
		glVertex3f(fS, fS, fS);
		glNormal3f(-1, 0, 0);
		glVertex3f(-fS, fS, fS);

		glNormal3f(0, 1, 0);
		glVertex3f(-fS, fS, fS);
		glNormal3f(0, -1, 0);
		glVertex3f(-fS, -fS, fS);
	}
	else
	{
		glVertex3f(-fS, -fS, fS);
		glVertex3f(fS, -fS, fS);

		glVertex3f(fS, -fS, fS);
		glVertex3f(fS, fS, fS);

		glVertex3f(fS, fS, fS);
		glVertex3f(-fS, fS, fS);

		glVertex3f(-fS, fS, fS);
		glVertex3f(-fS, -fS, fS);
	}

	// Note that glGetError returns GL_INVALID_OPERATION if it is called between a call to glBegin and its corresponding call to glEnd.
	CLU_OGL_CALL(glEnd());

	glBegin(GL_LINES);	// connecting lines

	if (m_bUseLighting)
	{
		m_StdMaterial.Diffuse(1.0, 1.0, 1.0, 1.0);
		m_StdMaterial.Ambient(m_BoxZCol);

		glNormal3f(0, 0, -1);
		glVertex3f(-fS, -fS, -fS);
		glNormal3f(0, 0, 1);
		glVertex3f(-fS, -fS, fS);

		m_StdMaterial.Diffuse(1.0, 1.0, 1.0, 1.0);
		m_StdMaterial.Ambient(m_BoxCol);
		glNormal3f(0, 0, -1);
		glVertex3f(fS, -fS, -fS);
		glNormal3f(0, 0, 1);
		glVertex3f(fS, -fS, fS);

		glNormal3f(0, 0, -1);
		glVertex3f(fS, fS, -fS);
		glNormal3f(0, 0, 1);
		glVertex3f(fS, fS, fS);

		glNormal3f(0, 0, -1);
		glVertex3f(-fS, fS, -fS);
		glNormal3f(0, 0, 1);
		glVertex3f(-fS, fS, fS);
	}
	else
	{
		glVertex3f(-fS, -fS, -fS);
		glVertex3f(-fS, -fS, fS);

		glVertex3f(fS, -fS, -fS);
		glVertex3f(fS, -fS, fS);

		glVertex3f(fS, fS, -fS);
		glVertex3f(fS, fS, fS);

		glVertex3f(-fS, fS, -fS);
		glVertex3f(-fS, fS, fS);
	}

	// Note that glGetError returns GL_INVALID_OPERATION if it is called between a call to glBegin and its corresponding call to glEnd.
	CLU_OGL_CALL(glEnd());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCLUDrawBase::DrawBase()
{
	if (m_bUseLighting)
	{
		glBegin(GL_LINES);

		glColor3f(1.0, 0.0, 0.0);

		glNormal3f(-1.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);

		glNormal3f(1.0, 0.0, 0.0);
		glVertex3f(1.0, 0.0, 0.0);

		glColor3f(0.0, 1.0, 0.0);

		glNormal3f(0.0, -1.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);

		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f(0.0, 1.0, 0.0);

		glColor3f(0.0, 0.0, 1.0);

		glNormal3f(0.0, 0.0, -1.0);
		glVertex3f(0.0, 0.0, 0.0);

		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(0.0, 0.0, 1.0);

		// Note that glGetError returns GL_INVALID_OPERATION if it is called between a call to glBegin and its corresponding call to glEnd.
		CLU_OGL_CALL(glEnd());
	}
	else
	{
		glBegin(GL_LINES);
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(1.0, 0.0, 0.0);

		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 1.0, 0.0);

		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 1.0);

		// Note that glGetError returns GL_INVALID_OPERATION if it is called between a call to glBegin and its corresponding call to glEnd.
		CLU_OGL_CALL(glEnd());
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCLUDrawBase::Display()
{
	// Need to overwrite this function
	BeginDraw();
	DrawBase();
	EndDraw();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCLUDrawBase::Reshape(int iWidth, int iHeight)
{
	SetViewport(0, 0, iWidth, iHeight);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCLUDrawBase::GetViewport(int& iX, int& iY, int& iW, int& iH)
{
	iX = m_iPosX;
	iY = m_iPosY;
	iW = m_iSizeX;
	iH = m_iSizeY;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUDrawBase::SetViewport(int iX, int iY, int iW, int iH)
{
	try
	{
		if ((iW <= 0) || (iH <= 0))
		{
			return false;
		}

		glViewport(m_iPosX = iX, m_iPosY = iY, m_iSizeX = iW, m_iSizeY = iH);

		m_xPers.fAspect = float(m_iSizeX) / float(m_iSizeY);

		m_xOrtho.fLeft   = -float(m_iSizeX) * 0.5f * m_fOrthoFac;
		m_xOrtho.fRight  = float(m_iSizeX) * 0.5f * m_fOrthoFac;
		m_xOrtho.fBottom = -float(m_iSizeY) * 0.5f * m_fOrthoFac;
		m_xOrtho.fTop    = float(m_iSizeY) * 0.5f * m_fOrthoFac;

		UpdateMatrices();
		return true;
	}
	catch (Clu::CIException& ex)
	{
		throw CLU_EXCEPTION_NEST(__FUNCTION__, std::move(ex));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCLUDrawBase::Key(unsigned char cKey, int iX, int iY)
{
	/// Need to implement this function
	if ((cKey == 'q') || (cKey == 'Q'))
	{
		exit(0);
	}

	if ((cKey == 's') || (cKey == 'S'))
	{
		char pcName[30];
		sprintf_s(pcName, 30, "OGLScreen%03d.bmp", m_iSaveScreenNo++);
		SaveScreen2BMP(pcName);
	}

	if ((cKey == 'a') || (cKey == 'A'))
	{
		if (!m_bIsAnimated)
		{
			StartAnimation(m_iAnimPause);
		}
		else{ StopAnimation(); }
	}

	if ((cKey == 'm') || (cKey == 'M'))
	{
		if (m_iAnimRotType != 4)
		{
			m_iAnimRotType = 4;	// ANIMROT_MOUSE
		}
		else{ m_iAnimRotType = 0; }
	}

	// Select Rotation mode
	if ((cKey >= '0') && (cKey <= '9'))
	{
		SetRTMode(cKey - '0');
	}
}

//////////////////////////////////////////////////////////////////////
/// Key

bool CCLUDrawBase::SetRTMode(int iMode)
{
	if ((iMode < 0) || (iMode > 9))
	{
		return false;
	}

	m_vxTransform[m_iRTMode].iFlags = 0;
	m_iRTMode                       = iMode;
	m_vxTransform[m_iRTMode].iFlags = 0;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Mouse

void CCLUDrawBase::Mouse(int iButton, int iState, int iX, int iY)
{
	GetModifiers();

	if (iButton == CLU_LEFT_BUTTON())
	{
		if (iState == CLU_DOWN())
		{
			m_bMouseLDown  = true;
			m_sMousePos.iX = iX;
			m_sMousePos.iY = iY;

			if (!(m_bCtrlDown ? 0 : m_iRTMode))
			{
				ResetMouseRotHist();
			}
		}
		else if (iState == CLU_UP())
		{
			m_bMouseLDown                   = false;
			m_vxTransform[m_iRTMode].iFlags = 0;
		}
	}
	else if (iButton == CLU_RIGHT_BUTTON())
	{
		if (iState == CLU_DOWN())
		{
			m_bMouseRDown  = true;
			m_sMousePos.iX = iX;
			m_sMousePos.iY = iY;
		}
		else if (iState == CLU_UP())
		{
			m_bMouseRDown                   = false;
			m_vxTransform[m_iRTMode].iFlags = 0;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCLUDrawBase::PassiveMouseMove(int iX, int iY)
{
	if (m_bPickMouseOver)
	{
		Picking(MOUSEPICK_OVER, iX, iY, 2, 2);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUDrawBase::Picking(EMousePickType ePickType, int iX, int iY, int iPickW, int iPickH)
{
	COGLVertex xA0, xA1, xA2, xA3;
	COGLVertex xB0, xB2;

	int piViewport[4];
	glGetIntegerv(GL_VIEWPORT, piViewport);

	m_SceneApplyData.xMatrixStack.Push(Clu::CMatrixStack::Projection);
	CLU_OGL_CALL(glLoadIdentity());

	if (m_b2DViewEnabled)
	{
		CLU_OGL_CALL(glOrtho(m_xOrtho.fLeft, m_xOrtho.fRight, m_xOrtho.fBottom, m_xOrtho.fTop, m_xOrtho.fNear, m_xOrtho.fFar));
	}
	else
	{
		CLU_OGL_CALL(gluPerspective(m_xPers.fAngle, m_xPers.fAspect, m_xPers.fNear, m_xPers.fFar));
	}

	m_SceneApplyData.xMatrixStack.Push(Clu::CMatrixStack::ModelView);
	CLU_OGL_CALL(glLoadIdentity());

	WorldTransform();

	// Instead of using gluPickMatrix use additional clipping planes for this purpose need to find coordinates of corners of picking region in world coordinates.
	double pdProj[16], pdFrame[16];
	glGetDoublev(GL_PROJECTION_MATRIX, pdProj);
	glGetDoublev(GL_MODELVIEW_MATRIX, pdFrame);

	double dXL = double(iX - iPickW);
	double dXR = double(iX + iPickW);
	double dYB = double(piViewport[3] - iY - 1 - iPickH);
	double dYT = double(piViewport[3] - iY - 1 + iPickH);

	double dX, dY, dZ;
	// Left-Bottom-Front
	gluUnProject(dXL, dYB, 0.2, pdFrame, pdProj, piViewport, &dX, &dY, &dZ);
	xA0.Set(float(dX), float(dY), float(dZ));

	// Right-Bottom-Front
	gluUnProject(dXR, dYB, 0.2, pdFrame, pdProj, piViewport, &dX, &dY, &dZ);
	xA1.Set(float(dX), float(dY), float(dZ));

	// Right-Top-Front
	gluUnProject(dXR, dYT, 0.2, pdFrame, pdProj, piViewport, &dX, &dY, &dZ);
	xA2.Set(float(dX), float(dY), float(dZ));

	// Left-Top-Front
	gluUnProject(dXL, dYT, 0.2, pdFrame, pdProj, piViewport, &dX, &dY, &dZ);
	xA3.Set(float(dX), float(dY), float(dZ));

	// Left-Bottom-Back
	gluUnProject(dXL, dYB, 0.8, pdFrame, pdProj, piViewport, &dX, &dY, &dZ);
	xB0.Set(float(dX), float(dY), float(dZ));

	// Right-Top-Back
	gluUnProject(dXR, dYT, 0.8, pdFrame, pdProj, piViewport, &dX, &dY, &dZ);
	xB2.Set(float(dX), float(dY), float(dZ));

	// Clip plane: left
	COGLVertex xDir = ((xB0 - xA0) ^ (xA3 - xA0));
	xDir.Norm();

	double pdClipPlane[4][4];
	pdClipPlane[0][0] = double(((float*) xDir)[0]);
	pdClipPlane[0][1] = double(((float*) xDir)[1]);
	pdClipPlane[0][2] = double(((float*) xDir)[2]);
	pdClipPlane[0][3] = -double(xDir * xA0);

	// Clip plane: right
	xDir = ((xB2 - xA2) ^ (xA1 - xA2));
	xDir.Norm();
	pdClipPlane[1][0] = double(((float*) xDir)[0]);
	pdClipPlane[1][1] = double(((float*) xDir)[1]);
	pdClipPlane[1][2] = double(((float*) xDir)[2]);
	pdClipPlane[1][3] = -double(xDir * xA2);

	// Clip plane: bottom
	xDir = ((xA1 - xA0) ^ (xB0 - xA0));
	xDir.Norm();
	pdClipPlane[2][0] = double(((float*) xDir)[0]);
	pdClipPlane[2][1] = double(((float*) xDir)[1]);
	pdClipPlane[2][2] = double(((float*) xDir)[2]);
	pdClipPlane[2][3] = -double(xDir * xA0);

	// Clip plane: top
	xDir = ((xA3 - xA2) ^ (xB2 - xA2));
	xDir.Norm();
	pdClipPlane[3][0] = double(((float*) xDir)[0]);
	pdClipPlane[3][1] = double(((float*) xDir)[1]);
	pdClipPlane[3][2] = double(((float*) xDir)[2]);
	pdClipPlane[3][3] = -double(xDir * xA2);

	glClipPlane(GL_CLIP_PLANE0, (GLdouble*) &pdClipPlane[0][0]);
	glClipPlane(GL_CLIP_PLANE1, (GLdouble*) &pdClipPlane[1][0]);
	glClipPlane(GL_CLIP_PLANE2, (GLdouble*) &pdClipPlane[2][0]);
	glClipPlane(GL_CLIP_PLANE3, (GLdouble*) &pdClipPlane[3][0]);

	glEnable(GL_CLIP_PLANE0);
	glEnable(GL_CLIP_PLANE1);
	glEnable(GL_CLIP_PLANE2);
	glEnable(GL_CLIP_PLANE3);

	// //////////////////////////////////////////////////////////////////////////////////////////
	//
	// Prepare pick drawing In this mode objects are drawn without lighting or texture mapping or
	// shaders. Instead, their color is an Idx into a list of pick name stacks that contain the
	// object IDs of objects and scenes drawn. Therefore, we need to switch off all fancy features
	// of OpenGL to just get the geometry drawn with flat colors. Clear viewport color and depth
	// buffers to black.
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Reset Pixel Zoom
	glPixelZoom(1.0f, 1.0f);

	// Reset Line width
	glLineWidth(2.0f);

	// Point Parameters
	if (glPointParameterf)
	{
		glPointSize(160.0f);
		glPointParameterf(GL_POINT_SIZE_MIN, 1.0f);
		glPointParameterf(GL_POINT_SIZE_MAX, 240.0f);
		glPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE, 1.0f);

		float pfPntAtt[] = { 1.0f, 2.0f, 4.0f };
		glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, pfPntAtt);
	}
	else
	{
		glPointSize(6.0f);
	}

	// Disable Multisample
	glDisable(GL_MULTISAMPLE);

	// Disable Lighting
	glDisable(GL_LIGHTING);

	// Draw into back buffer
	glDrawBuffer(GL_BACK);

	// Draw for picking
	PickDraw();

	// Flush all drawing operations
	glFlush();

	// Read from back framebuffer to see which object was drawn in viewport
	glReadBuffer(GL_BACK);

	// Calculate patch size and patch position
	const int ciRPatchHW  = 2;
	const int ciRPatchHH  = 2;
	const int ciRW        = 2 * ciRPatchHW + 1;
	const int ciRH        = 2 * ciRPatchHH + 1;
	const int ciPatchSize = ciRW * ciRH;
	int iRX               = iX - ciRPatchHW;
	int iRY               = piViewport[3] - iY - 1 - ciRPatchHH;
	unsigned int puPixData[ciPatchSize];
	float pfDepthData[ciPatchSize];

	if (iRX < 0)
	{
		iRX = 0;
	}
	else if (iRX >= piViewport[2] - ciRW)
	{
		iRX = piViewport[2] - ciRW - 1;
	}

	if (iRY < 0)
	{
		iRY = 0;
	}
	else if (iRY >= piViewport[3] - ciRH)
	{
		iRY = piViewport[3] - ciRH - 1;
	}

	// Read color pixels
	glReadPixels(iRX, iRY, ciRW, ciRH, GL_RGBA, GL_UNSIGNED_BYTE, puPixData);

	// Read depth pixels
	glReadPixels(iRX, iRY, ciRW, ciRH, GL_DEPTH_COMPONENT, GL_FLOAT, pfDepthData);

	// Analyze depth pixels
	int iMinIdx         = 0;
	float fPickMinDepth = 2.0f;
	for (int iIdx = 0; iIdx < ciPatchSize; ++iIdx)
	{
		if (pfDepthData[iIdx] < fPickMinDepth)
		{
			fPickMinDepth = pfDepthData[iIdx];
			iMinIdx       = iIdx;
		}
	}

	// Take closest object as the selected one
	uint uPickColor = puPixData[iMinIdx];

	glDisable(GL_CLIP_PLANE0);
	glDisable(GL_CLIP_PLANE1);
	glDisable(GL_CLIP_PLANE2);
	glDisable(GL_CLIP_PLANE3);

	m_SceneApplyData.xMatrixStack.Pop(Clu::CMatrixStack::Projection);
	m_SceneApplyData.xMatrixStack.Pop(Clu::CMatrixStack::ModelView);

	// Process the hits
	return PickProcessHits(ePickType, uPickColor, fPickMinDepth);	//, iHitCnt, mSelBuf );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCLUDrawBase::ActiveMouseMove(int iX, int iY)
{
	try
	{
		GetModifiers();

		int iStepX = iX - m_sMousePos.iX;
		int iStepY = iY - m_sMousePos.iY;

		m_vxTransform[m_iStdRTMode].iFlags = 0;

		if (!iStepX && !iStepY)
		{
			return;
		}

		int iActMode;

		// If CTRL is pressed always rotate whole space
		iActMode = ((m_bCtrlDown || m_iRTMode == 0) ? m_iStdRTMode : m_iRTMode);

		STransform& rTrans = m_vxTransform[iActMode];
		rTrans.iFlags = 0;

		if (m_bMouseLDown)
		{
			if (iActMode == m_iStdRTMode)
			{
				float fX, fY;

				if (!m_b2DViewEnabled)
				{
					fX = float(iStepX);
					fY = float(iStepY);

					if (!m_bShiftDown)
					{
						// Set the rotation Axis with respect to the camera orientation
						TVec3 vfRotationAxis = m_xCameraTransform.GetUpVector() * fX;
						vfRotationAxis += m_xCameraTransform.GetRightVector() * fY;

						m_vU[4] = vfRotationAxis.x();
						m_vU[5] = vfRotationAxis.y();
						m_vU[6] = vfRotationAxis.z();
					}
					else
					{
						TVec3 vfRotationAxis = m_xCameraTransform.GetRightVector() *  fX;
						vfRotationAxis += m_xCameraTransform.GetViewVector() *  fY;

						m_vU[4] = vfRotationAxis.x();
						m_vU[5] = vfRotationAxis.y();
						m_vU[6] = vfRotationAxis.z();
					}
				}
				else
				{
					m_vU[4] = 0.0;
					m_vU[5] = (fY = 0.0f);	// iL2
					m_vU[6] = (fX = -float(iStepX));//iL3
				}

				if ((fX != 0.0f) || (fY != 0.0f))
				{
					if (m_bUseLocalRot)
					{
						rTrans.pfRot[0] += fX * m_fRotFac;

						if (m_bShiftDown)
						{
							rTrans.pfRot[1] += fY * m_fRotFac;
						}
						else
						{
							rTrans.pfRot[2] += fY * m_fRotFac;
						}
					}
					else
					{
						if (iActMode == 0)
						{
							m_pvMouseRotHist[m_iMRHPos] = m_vU;
							m_iMRHPos++;

							if (m_iMRHPos >= MRHLEN)
							{
								m_iMRHPos = 0;
							}

							m_bMRHChanged = true;

							float fAngle = sqrt(fX * fX + fY * fY) * m_fRotFac * m_fRadPerDeg;

							m_vRMain = m_E3Base.Rotor(m_vU, fAngle, false) & m_vRMain;
						}
						else if (iActMode == 10)
						{
							m_xCameraTransform.RotateAroundY(fX * 0.2f * m_fRotFac * m_fRadPerDeg);
							m_xCameraTransform.RotateAroundX(fY * 0.2f * m_fRotFac * m_fRadPerDeg);
						}
						else
						{
							// manipulate the RVView main with
							float fAngle = sqrt(fX * fX + fY * fY) * m_fRotFac * m_fRadPerDeg;
							m_vRViewMain = m_E3Base.Rotor(m_vU, fAngle, false) & m_vRViewMain;
						}
					}

					rTrans.SetRotChanged(0);
					rTrans.SetRotChanged(1);
					rTrans.SetRotChanged(2);
				}
			}
			else
			{
				// Old Mouse Control Mechanism
				if (iStepX)
				{
					rTrans.pfRot[0] += GLfloat(iStepX) * m_fRotFac;
					rTrans.SetRotChanged(0);
				}

				if (iStepY)
				{
					if (m_bShiftDown)
					{
						rTrans.pfRot[1] += GLfloat(iStepY) * m_fRotFac;
						rTrans.SetRotChanged(1);
					}
					else
					{
						rTrans.pfRot[2] += GLfloat(iStepY) * m_fRotFac;
						rTrans.SetRotChanged(2);
					}
				}
			}

			AdjustRotAngle(rTrans.pfRot[0]);
			AdjustRotAngle(rTrans.pfRot[1]);
			AdjustRotAngle(rTrans.pfRot[2]);

			m_sMousePos.iX = iX;
			m_sMousePos.iY = iY;

			Display();
		}
		else if (m_bMouseRDown)
		{
			if (iActMode == 10)
			{
				// If we are in ViewRTMode, perform a translation relative to the view
				m_xCameraTransform.TranslateInViewDirection((float) (-iStepY) / 50.f);
				m_xCameraTransform.TranslateStrafeToViewDirection((float) iStepX / 50.f);
			}
			else
			{
				if (iStepX)
				{
					// In StdMode, we translate the model according to the current camera orientation
					TVec3 vfTranslation = m_xCameraTransform.GetRightVector() * GLfloat(iStepX) * m_fTransFac;

					rTrans.pfTrans[0] += vfTranslation.x();
					rTrans.pfTrans[1] += vfTranslation.y();
					rTrans.pfTrans[2] += vfTranslation.z();
					rTrans.SetAllTransChanged();
				}

				if (iStepY)
				{
					if (!m_b2DViewEnabled)
					{
						if (m_bShiftDown)
						{
							// Shift pressed ? translate the model up/down (relative to the camera)
							// We get window coordinates with origin on TopLeft, OpenGL is bottom left orientated.
							// We need to Flip the y-Step to get a consistent movement.
							TVec3 vfTranslation = m_xCameraTransform.GetUpVector() * GLfloat(-iStepY) * m_fTransFac;

							rTrans.pfTrans[0] += vfTranslation.x();
							rTrans.pfTrans[1] += vfTranslation.y();
							rTrans.pfTrans[2] += vfTranslation.z();
						}
						else
						{
							// Shift not pressed ? move in the cameras view direction
							TVec3 vfTranslation = m_xCameraTransform.GetViewVector() * GLfloat(iStepY) * m_fTransFac;

							rTrans.pfTrans[0] += vfTranslation.x();
							rTrans.pfTrans[1] += vfTranslation.y();
							rTrans.pfTrans[2] += vfTranslation.z();
						}
						rTrans.SetAllTransChanged();
					}
					else
					{
						if (iActMode == 0)
						{
							// Zoom Ortho
							if (m_bShiftDown)
							{
								m_fOrthoFac -= GLfloat(iStepY) * m_fOrthoZoomFac;
								if (m_fOrthoFac < 0.001f)
								{
									m_fOrthoFac = 0.001f;
								}

								m_xOrtho.fLeft   = -float(m_iSizeX) * 0.5f * m_fOrthoFac;
								m_xOrtho.fRight  = float(m_iSizeX) * 0.5f * m_fOrthoFac;
								m_xOrtho.fBottom = -float(m_iSizeY) * 0.5f * m_fOrthoFac;
								m_xOrtho.fTop    = float(m_iSizeY) * 0.5f * m_fOrthoFac;
								UpdateMatrices();

								rTrans.SetTransChanged(2);
							}
							else
							{
								rTrans.pfTrans[1] -= GLfloat(iStepY) * m_fTransFac;
								rTrans.SetTransChanged(1);
							}
						}
						else
						{
							if (!m_bShiftDown)
							{
								rTrans.pfTrans[1] -= GLfloat(iStepY) * m_fTransFac;
								rTrans.SetTransChanged(1);
							}
							else
							{
								rTrans.pfTrans[2] += GLfloat(iStepY) * m_fTransFac;
								rTrans.SetTransChanged(2);
							}
						}
					}
				}
			}

			m_sMousePos.iX = iX;
			m_sMousePos.iY = iY;

			Display();
		}
	}
	catch (Clu::CIException& ex)
	{
		throw CLU_EXCEPTION_NEST(__FUNCTION__, std::move(ex));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCLUDrawBase::Mouse3D_KeyDown(int iKeyCode)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCLUDrawBase::Mouse3D_KeyUp(int iKeyCode)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCLUDrawBase::Mouse3D_Move(double dTX, double dTY, double dTZ, double dLength, double dRX, double dRY, double dRZ, double dAngle)
{
	try
	{
		// Transform whole space
		STransform& rTrans = m_vxTransform[m_iStdRTMode];
		rTrans.iFlags = 0;

		if (dLength != 0.0)
		{
			rTrans.pfTrans[0] += GLfloat(dTX);	// * m_fTransFac;
			rTrans.SetTransChanged(0);

			if (!m_b2DViewEnabled)
			{
				rTrans.pfTrans[1] += GLfloat(dTY);	// * m_fTransFac;
				rTrans.SetTransChanged(1);

				rTrans.pfTrans[2] += GLfloat(dTZ);	// * m_fTransFac;
				rTrans.SetTransChanged(2);
			}
			else
			{
				// Zoom Ortho
				m_fOrthoFac -= 10.0f * float(dTZ) * m_fOrthoZoomFac;
				if (m_fOrthoFac < 0.001f)
				{
					m_fOrthoFac = 0.001f;
				}

				m_xOrtho.fLeft   = -float(m_iSizeX) * 0.5f * m_fOrthoFac;
				m_xOrtho.fRight  = float(m_iSizeX)  * 0.5f * m_fOrthoFac;
				m_xOrtho.fBottom = -float(m_iSizeY)  * 0.5f * m_fOrthoFac;
				m_xOrtho.fTop    = float(m_iSizeY) * 0.5f * m_fOrthoFac;
				UpdateMatrices();

				rTrans.SetTransChanged(2);

				rTrans.pfTrans[0] += GLfloat(dTX);	// * m_fTransFac;
				rTrans.pfTrans[1] += GLfloat(dTY);	// * m_fTransFac;
				rTrans.SetTransChanged(0);
				rTrans.SetTransChanged(1);
			}
		}

		if (dAngle != 0.0)
		{
			if (!m_b2DViewEnabled)
			{
				m_vU[5] = float(dRY);
				m_vU[4] = float(dRX);
				m_vU[6] = float(dRZ);

				if (m_bUseLocalRot)
				{
					rTrans.pfRot[0] += float(dRY * dAngle);	// * m_fRotFac;
					rTrans.pfRot[1] += float(dRX * dAngle);	// * m_fRotFac;
					rTrans.pfRot[2] += float(dRZ * dAngle);	// * m_fRotFac;
				}
				else
				{
					float fAngle;

					m_pvMouseRotHist[m_iMRHPos] = m_vU;
					m_iMRHPos++;

					if (m_iMRHPos >= MRHLEN)
					{
						m_iMRHPos = 0;
					}

					m_bMRHChanged = true;

					fAngle = float(dAngle) * m_fRotFac;	// * m_fRadPerDeg;

					m_vRMain = m_E3Base.Rotor(m_vU, fAngle, false) & m_vRMain;
				}
			}
			else
			{
				m_vU[4] = 0.0f;
				m_vU[5] = 0.0f;	// iL2
				m_vU[6] = 1.0f;	//iL3

				if (m_bUseLocalRot)
				{
					rTrans.pfRot[0] = 0.0f;
					rTrans.pfRot[1] = 0.0f;
					rTrans.pfRot[2] = float(dRZ * dAngle);
				}
				else
				{
					float fAngle;

					m_pvMouseRotHist[m_iMRHPos] = m_vU;
					m_iMRHPos++;

					if (m_iMRHPos >= MRHLEN)
					{
						m_iMRHPos = 0;
					}

					m_bMRHChanged = true;

					fAngle = float(dRZ * dAngle) * m_fRotFac;	// * m_fRadPerDeg;

					m_vRMain = m_E3Base.Rotor(m_vU, fAngle, false) & m_vRMain;
				}
			}

			rTrans.SetRotChanged(0);
			rTrans.SetRotChanged(1);
			rTrans.SetRotChanged(2);
		}

		Display();
	}
	catch (Clu::CIException& ex)
	{
		throw CLU_EXCEPTION_NEST(__FUNCTION__, std::move(ex));
	}
}

//////////////////////////////////////////////////////////////////////
///	Menu

void CCLUDrawBase::Menu(int iIdx)
{
}

//////////////////////////////////////////////////////////////////////
/// Timed Function

bool CCLUDrawBase::TimedFunc()
{
	float fAngle = m_fTimeStep * 20.0f * m_fRadPerDeg;

	m_vU[4] = 0.0f;
	m_vU[5] = 1.0f;
	m_vU[6] = 0.0f;

	m_vRMain = m_E3Base.Rotor(m_vU, fAngle, false) & m_vRMain;

	m_vxTransform[0].SetRotChanged(0);
	m_vxTransform[0].SetRotChanged(1);
	m_vxTransform[0].SetRotChanged(2);

	Display();

	return true;
}

/////////////////////////////////////////////////////////////////////
// Save Image as PGM-file
//
/// Function written by Bodo Rosenhahn. 3.12.2001

bool CCLUDrawBase::SaveScreen2PGM(const char* pcFilename)
{
	int iWidth, iHeight;
	FILE* pFile;
	unsigned char* pImage;
	unsigned char* pImages;
	int i, j;

	iWidth  = m_iSizeX;
	iHeight =  m_iSizeY;
	// open Image / Get Memory
	fopen_s(&pFile, pcFilename, "w");
	if (!pFile)
	{
		return false;
	}
	if ((pImage = (unsigned char*) malloc(3 * iWidth * iHeight * sizeof(unsigned char))) == NULL)
	{
		fclose(pFile);
		return false;
	}
	if ((pImages = (unsigned char*) malloc(3 * iWidth * iHeight * sizeof(unsigned char))) == NULL)
	{
		fclose(pFile);
		free(pImage);
		return false;
	}
	// Get Pixels
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadBuffer(GL_BACK_LEFT);
	glReadPixels(0, 0, iWidth, iHeight, GL_RGB, GL_UNSIGNED_BYTE, pImage);
	// Header for PGM-File
	fwrite("P6 \n", 4, 1, pFile);
	//fwrite ("500 500 \n", 9, 1, pFile );
	fprintf(pFile, "%d %d \n", iWidth, iHeight);
	fwrite("255 \n", 5, 1, pFile);
	// Reorder Bytes
	for (i = 0; i < iHeight; i++)
	{
		for (j = 0; j < iWidth * 3; j = j + 3)
		{
			pImages[iWidth * 3 * (iHeight - (i + 1)) + (j + 1)] = pImage[iWidth * 3 * i + j];
			pImages[iWidth * 3 * (iHeight - (i + 1)) + (j + 2)] = pImage[iWidth * 3 * i + (j + 1)];
			pImages[iWidth * 3 * (iHeight - (i + 1)) + (j + 0)] = pImage[iWidth * 3 * i + (j + 2)];
		}
	}

	fwrite(pImages, iWidth * iHeight * 3, 1, pFile);
	fclose(pFile);
//	printf("Image saved under '%s'.\n", cFilename);

	return true;
}

/////////////////////////////////////////////////////////////////////
// Save Image in file (any format supported by DevIL)
//

bool CCLUDrawBase::SaveScreen2BMP(const char* pcFilename)
{
	ILuint uImgID;
	int iWidth, iHeight;
	unsigned char* pImage;
	CStrMem csFilename, csBaseName, csDummyName, csExt;
	size_t nExtPos;
	//bool bEPSFile = false;
	//bool bJPS     = false;
	bool bBB      = false;	// Save Bounding Box in additional file

	csBaseName = csFilename = pcFilename;

	nExtPos     = ('.' < csBaseName);
	csExt       = csBaseName.Last(int(csFilename.Len() - nExtPos), 1);
	csBaseName |= -1;

	//if (csExt == "eps")
	//{
	//	csFilename = csBaseName + ".tmp";
	//	bEPSFile   = true;
	//}
	//else if (csExt == "jps")
	//{
	//	csFilename = csBaseName + ".jpg";
	//	bEPSFile   = true;
	//	bJPS       = true;
	//}
	//else 
	if (csExt == "bpng")
	{
		csFilename = csBaseName + ".png";
		bBB        = true;
	}

	iWidth  = m_iSizeX;
	iHeight =  m_iSizeY;

	LockVis();
	::LockImageAccess();

	ilGenImages(1, &uImgID);
	ilBindImage(uImgID);
	if (iluEnlargeCanvas(iWidth, iHeight, 1) == IL_FALSE)
	{
		ilDeleteImages(1, &uImgID);
		::UnlockImageAccess();
		UnlockVis();
		return false;
	}

	ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
	pImage = (unsigned char*) ilGetData();

	// Get Pixels
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadBuffer(GL_BACK_LEFT);
	glReadPixels(0, 0, iWidth, iHeight, GL_RGB, GL_UNSIGNED_BYTE, pImage);

	remove(csFilename.Str());

	//if (bEPSFile)
	//{
	//	if (ilSave(IL_JPG, (ILstring) csFilename.Str()) == IL_FALSE)
	//	{
	//		ilDeleteImages(1, &uImgID);
	//		::UnlockImageAccess();
	//		UnlockVis();
	//		return false;
	//	}
	//}
	//else
	//{
		if (ilSaveImage((ILstring) csFilename.Str()) == IL_FALSE)
		{
			ilDeleteImages(1, &uImgID);
			::UnlockImageAccess();
			UnlockVis();
			return false;
		}
	//}

	ilDeleteImages(1, &uImgID);

	//if (bEPSFile)
	//{
	//	csDummyName = csBaseName + ".eps";

	//	if (!ConvertJPEGtoPS(csFilename.Str(), csDummyName.Str()))
	//	{
	//		::UnlockImageAccess();
	//		UnlockVis();
	//		return false;
	//	}

	//	if (!bJPS)
	//	{
	//		remove(csFilename.Str());
	//	}
	//}

	if (bBB)
	{
		csDummyName = csBaseName + ".bb";
		ofstream zData;

		zData.open(csDummyName.Str());
		if (!zData.is_open())
		{
			::UnlockImageAccess();
			UnlockVis();
			return false;
		}

		char pcText[1000];
		//int iMaxX, iMaxY;

		//iMaxX = int(ceil( float(iWidth)*0.72f ));
		//iMaxY = int(ceil( float(iHeight)*0.72f ));

		sprintf_s(pcText, 1000,
				"%%%%Creator: CLUCalc\n"
				"%%%%BoundingBox: 0 0 %d %d\n",
				iWidth, iHeight);

		zData.write(pcText, strlen(pcText));
		zData.close();
	}

	::UnlockImageAccess();
	UnlockVis();
	return true;
}

/*
bool CCLUDrawBase::SaveScreen2BMP(const char *pcFilename)
{
		int iWidth, iHeight;
		FILE *pFile;
		BITMAPFILEHEADER FileHead;
		BITMAPINFOHEADER InfoHead;
		unsigned char *pImage;

		iWidth = m_iSizeX;
		iHeight =  m_iSizeY;

		// open Image / Get Memory
		if (!(pFile = fopen(pcFilename, "wb")))
		{
				return false;
		}

		if ((pImage = (unsigned char *)malloc(3*iWidth*iHeight*sizeof(unsigned char))) == NULL)
		{
				fclose(pFile);
				return false;
		}

		// Get Pixels
		glPixelStorei(GL_PACK_ALIGNMENT,1);
		glReadBuffer(GL_FRONT_LEFT);
		glReadPixels( 0, 0, iWidth, iHeight, GL_RGB, GL_UNSIGNED_BYTE, pImage );

		//////////////////////////////////////////////////////////////////////////
		// Write BMP File Header
		FileHead.bfType = BITMAP_TYPE;
		FileHead.bfSize = 3 * iWidth * iHeight;	// 24-bit format
		FileHead.bfReserved1 = 0;
		FileHead.bfReserved2 = 0;
		FileHead.bfOffBits = 54;//sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

		FileHead.bfSize += FileHead.bfOffBits;

		if (fwrite(&FileHead.bfType, sizeof(WORD), 1, pFile) != 1) return false;
		if (fwrite(&FileHead.bfSize, sizeof(DWORD), 1, pFile) != 1) return false;
		if (fwrite(&FileHead.bfReserved1, sizeof(WORD), 1, pFile) != 1) return false;
		if (fwrite(&FileHead.bfReserved2, sizeof(WORD), 1, pFile) != 1) return false;
		if (fwrite(&FileHead.bfOffBits, sizeof(DWORD), 1, pFile) != 1) return false;

		//////////////////////////////////////////////////////////////////////////
		// Write BMP Info Header
		InfoHead.biSize = sizeof(BITMAPINFOHEADER);
		InfoHead.biWidth = iWidth;
		InfoHead.biHeight = iHeight;
		InfoHead.biPlanes = 1;
		InfoHead.biBitCount = 24;
		InfoHead.biCompression = BI_RGB;
		InfoHead.biSizeImage = 3 * iWidth * iHeight;
		InfoHead.biXPelsPerMeter = 2880;
		InfoHead.biYPelsPerMeter = 2880;
		InfoHead.biClrUsed = 0;
		InfoHead.biClrImportant = 0;

		if (fwrite(&InfoHead.biSize, sizeof(DWORD), 1, pFile) != 1) return false;
		if (fwrite(&InfoHead.biWidth, sizeof(LONG), 2, pFile) != 2) return false;
		if (fwrite(&InfoHead.biPlanes, sizeof(WORD), 2, pFile) != 2) return false;
		if (fwrite(&InfoHead.biCompression, sizeof(DWORD), 2, pFile) != 2) return false;
		if (fwrite(&InfoHead.biXPelsPerMeter, sizeof(LONG), 2, pFile) != 2) return false;
		if (fwrite(&InfoHead.biClrUsed, sizeof(DWORD), 2, pFile) != 2) return false;

		//////////////////////////////////////////////////////////////////////////
		// Write BMP Data
		unsigned char *pBMPData;

		int x = 0, y = 0, fx = 0, fy = 0, nPixels;
		int uLineLength, uOffset;
		int uAbsOffset;
		int uPixType = InfoHead.biBitCount;

		nPixels = iWidth * iHeight;
		uLineLength = uPixType * iWidth;
		if ((uOffset = uLineLength % 32) != 0)
				uLineLength += 32 - uOffset;
		uLineLength /= 16;

		if (!(pBMPData = new unsigned char[uLineLength<<1]))
		{
				fclose(pFile);
				return false;
		}

		for(fy=0,y=0;fy<iHeight;fy++,y++)
		{
				uOffset = y * iWidth * 3;

				for(x=0,fx=0,uAbsOffset=uOffset;x<iWidth;x++,fx+=3)
				{
						pBMPData[fx+2] = pImage[uAbsOffset++];
						pBMPData[fx+1] = pImage[uAbsOffset++];
						pBMPData[fx]   = pImage[uAbsOffset++];
				}

				// Write Line
				if (int(fwrite(pBMPData, 2, uLineLength, pFile)) != uLineLength) break;

		} // for
		delete pBMPData;
		free(pImage);

		// Close File
		fclose ( pFile );

		return true;
}
*/

//////////////////////////////////////////////////////////////////////////
// Tool Functions

bool CCLUDrawBase::GetToolValue(const char* pcName, double& rVal)
{
	return false;
}

bool CCLUDrawBase::GetToolValue(const char* pcName, string& rVal)
{
	return false;
}

bool CCLUDrawBase::AddToolSlider(const char* pcName, double dMin, double dMax, double dStep, double dVal, bool bFixed, const char* pcDesc)
{
	return false;
}

bool CCLUDrawBase::AddToolCounter(const char* pcName, double dMin, double dMax, double dStep, double dLStep, double dVal, bool bFixed, const char* pcDesc)
{
	return false;
}

bool CCLUDrawBase::AddToolInput(const char* pcName, double dMin, double dMax, double dVal, bool bFixed, const char* pcDesc)
{
	return false;
}

bool CCLUDrawBase::AddToolInputText(const char* pcName, bool bImmediateCallback, const char* pcInitText, bool bFixed, const char* pcDesc, bool bDoUpdate)
{
	return false;
}

bool CCLUDrawBase::AddToolCheckBox(const char* pcName, bool bSet, bool bFixed, const char* pcDesc)
{
	return false;
}

bool CCLUDrawBase::AddToolButton(const char* pcName,            bool bFixed, const char* pcDesc)
{
	return false;
}

bool CCLUDrawBase::AddToolImageButton(const char* pcName, COGLImage* pImgAct, COGLImage* pImgDeact, bool bFixed, const char* pcDesc)
{
	return false;
}

bool CCLUDrawBase::AddToolChoice(const char* pcName, vector<string>& vecData, const char* pcCoice, bool bFixed, const char* pcDesc, bool bDoUpdate)
{
	return false;
}

bool CCLUDrawBase::AddToolbarImageButton(const char* pcName, COGLImage* pImgAct, COGLImage* pImgDeact, bool bFixed, const char* pcDesc)
{
	return false;
}

bool CCLUDrawBase::AddToolbarCounter(const char* pcName,
		const char* pcUnit,
		double dMin,
		double dMax,
		double dStep,
		double dLStep,
		double dVal,
		bool bActive,
		bool bFixed,
		const char* pcDesc)
{
	return false;
}

bool CCLUDrawBase::AddToolbarLabel(const char* pcName, bool bFixed, const char* pcDesc)
{
	return false;
}

bool CCLUDrawBase::EnableMenu(bool bVal)
{
	return false;
}

bool CCLUDrawBase::EnableStdCtrl(bool bVal)
{
	return false;
}

bool CCLUDrawBase::SetMenu(vector<SMenuItem>& vecMenuItem)
{
	return false;
}

// Info Window Functions
bool CCLUDrawBase::SetInfoText(const char* pcText)
{
	return false;
}

// Set Width of Info window in percent
bool CCLUDrawBase::SetInfoWidth(double dWidth)
{
	return false;
}
