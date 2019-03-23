////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.View.Base
// file:      OGLWin_Display.cpp
//
// summary:   Implements the ogl window display class
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
#include "OGLWin.h"
#include <stdlib.h>
#include "CLUOutput.h"
#include "CluTec.Viz.Base\Environment.h"
#include "CluTec.Viz.OpenGL.Extensions\Extensions.h"
#include "CluTec.Viz.OpenGL\Api.h"
#include "CluTec.Viz.Draw\OGLRenderTarget.h"
#include "CluTec.Viz.Draw\MatrixStack.h"

#include "CluTec.Base/Logger.h"

// TODO: Implement Set Thread Name
//#include "CluTec.Base\SetThreadNameNative.h"


#include <string>
using namespace std;

#ifdef _DEBUG
	#define TIMER_START(theVar) theVar = GetTime()
	#define TIMER_END(theVar) theVar   = GetTime() - theVar
#else
	#define TIMER_START(theVar) __noop
	#define TIMER_END(theVar) __noop
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COGLWin::FlushDraw()
{
	// Read special reserved script variables
	_GetScriptVisualizationSettings();

	DisplaySceneGraph();
	glFlush();
	SwapBuffers(m_hGLDC);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COGLWin::Display()
{
	PostRedisplay();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COGLWin::_GetScriptVisualizationSettings()
{
	CCodeVar xVar;
	TCVScalar iVal;

	/************************************************************************/
	/* _BGColor                                                             */
	/************************************************************************/
	if (m_xParse.GetVar("_BGColor", xVar))
	{
		if (xVar.Type() == PDT_COLOR)
		{
			COGLColor colBG = *((COGLColor*) xVar.Val());
			SetBGColor(colBG);
		}
	}

	/************************************************************************/
	/* _ViewAngle                                                           */
	/************************************************************************/
	if (m_xParse.GetVar("_ViewAngle", xVar))
	{
		xVar.CastToScalar(m_dViewAngle);
	}

	/************************************************************************/
	/* _2dView                                                              */
	/************************************************************************/

	// Variable has to be existent, has to be a scalar and has to be different than zero
	bool bIs2DVariableSet = (m_xParse.GetVar("_2dView", xVar) && xVar.CastToScalar(iVal) && (iVal != 0));

	// Flag if 2D view variable has been changed since last execution
	m_b2DViewVariableChanged = (!m_b2DViewEnabled && bIs2DVariableSet) || (m_b2DViewEnabled && !bIs2DVariableSet);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COGLWin::PostRedisplay(bool bWait, bool bForce, bool bNow)
{
	//CLU_LOG("Enter PostRedisplay...\n");
	// TODO: Exit on timeout!!
	WaitForSingleObject(m_hMutexVis, 1000);
	//CLU_LOG("...Got mutex\n");
	// Reset visualization event
	ResetEvent(m_hEventVis);
	m_bDoRedisplay = true;

	// If bForce is true, then run visualization even if m_bVisActive is false.
	// This can ensure that ExecScript() calls are executed even if the window is disabled.
	if (bForce)
	{
		SetEvent(m_hEventForceVis);
	}

	if (bNow)
	{
		SetEvent(m_hEventVisNow);
	}

	ReleaseMutex(m_hMutexVis);

	if (bWait)
	{
		//CLU_LOG("...waiting for processed\n");
		WaitForSingleObject(m_hEventVis, 5000);
		//CLU_LOG("...processed!\n\n");
		ResetEvent(m_hEventVis);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COGLWin::DoDisplay()
{
	try
	{
		// Now we display, so we can allow idle loop to check script.
		m_bWaitWithIdleForNextDisplay = false;

		// Save Frame Transform
		bool bRedraw   = false;
		int iActRTMode = GetActRTMode();

		m_b2DViewVariableChanged = false;

		if (iActRTMode && m_vxTransform[iActRTMode].iFlags)
		{
			AddExecModeVar(EXEC_MODE_USER);
		}

		if (m_bCodeChanged || (m_bAnimCode && m_bDoAnimCodeStep))
		{
			bRedraw             = true;
			m_bCodeChanged      = false;
			m_bCheckForAnimate  = true;
			m_bProcessPickHits  = true;						// Allow function PickProcessHits() to generate Notify messages again.
			m_bProcessStartDrag = false;					// Allow active mouse move to issue 'drag' messages

			if (m_bCodeChanged)
			{
				this->ShowWindow();
			}
		}
		else
		{
			for (int i = 1; i <= 9; i++)
			{
				if (m_vxTransform[i].iFlags)
				{
					bRedraw = true;
					break;
				}
			}

			if (m_iExecMode & EXEC_MODE_TOOL ||
				m_iExecMode & EXEC_MODE_LINK ||
				m_iExecMode & EXEC_MODE_KEYUP ||
				m_iExecMode & EXEC_MODE_KEYDOWN ||
				m_iExecMode & EXEC_MODE_PICKOVER ||
				m_iExecMode & EXEC_MODE_PICKSEL ||
				m_iExecMode & EXEC_MODE_PICKDRAG ||
				m_iExecMode & EXEC_MODE_QUIT ||
				m_iExecMode & EXEC_MODE_USER)
			{
				bRedraw             = true;
				m_bCheckForAnimate  = true;
				m_bProcessPickHits  = true;						// Allow function PickProcessHits() to generate Notify messages again.
				m_bProcessStartDrag = false;					// Allow active mouse move to issue 'drag' messages
			}
		}

		// Check whether only space has been transformed
		if (bRedraw)
		{
			//CLU_LOG(">>>> Drawing script");
			m_xParse.TryResetResourceHandles();

			// Reset Background Color
			m_BGCol.Set(0.0f, 0.0f, 0.0f, 1.0f);

			// Execute script to generate scene graph
			Draw();

			if (!HasError())
			{
				// Read special reserved script variables
				_GetScriptVisualizationSettings();
				m_iExecMode = EXEC_MODE_NONE;
			}
			//CLU_LOG(">>>> Drawing finished");

		}

		// Do the actual drawing
		if (m_bDoDisplaySceneGraph)
		{
			//CLU_LOG(">>>> Displaying Scene Graph");
			DisplaySceneGraph();
			//CLU_LOG(">>>> Displaying Scene Graph finished");
		}

		// If a mouse event callback asked for a script execution then this has been done now
		m_bWaitForMouseEventExecScript = false;
		m_bInDisplay                   = false;
	}
	catch (Clu::CIException& ex)
	{
		throw CLU_EXCEPTION_NEST("Error doing display", std::move(ex));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COGLWin::_CreateErrorOutput()
{
	// Get error location
	CStrMem csErrFile;
	int iErrLine, iErrPos, iErrInPos;
	m_xParse.GetCodeErrorPos(iErrLine, iErrPos, iErrInPos, csErrFile);

	// Get error string
	CStrMem csErrText = m_xParse.PrintCodeErrors();

	if (m_bThrowExceptionOnScriptError)
	{
		csErrText << " : " << csErrFile << " - " << __FUNCTION__ << " [" << iErrLine << "]";
		throw CLU_EXCEPTION(csErrText.Str());
	}

	CCLUCodeBase::SOutputObject xOutObj;

	xOutObj.sName          = "Output up to Error";
	xOutObj.bCanChangeSize = false;
	xOutObj.bCanHideValue  = false;
	xOutObj.bBRAfterValue  = true;
	xOutObj.bBRBeforeValue = true;
	xOutObj.bShowInfo      = false;
	xOutObj.bShowValue     = true;
	xOutObj.sRelSym        = ":";
	xOutObj.sType          = " ";
	xOutObj.iSize          = 0;
	xOutObj.sValue         = " ";
	m_xParse.InsertOutputObject(xOutObj);

	xOutObj.sName          = "Stack";
	xOutObj.bCanChangeSize = true;
	xOutObj.bCanHideValue  = true;
	xOutObj.bBRAfterValue  = true;
	xOutObj.bBRBeforeValue = true;
	xOutObj.bShowInfo      = false;
	xOutObj.bShowValue     = false;
	xOutObj.sRelSym        = ":";
	xOutObj.sType          = "stack";
	xOutObj.iSize          = 0;
	xOutObj.sValue         = m_xParse.PrintStack().Str();
	m_xParse.InsertOutputObject(xOutObj);

	xOutObj.sName          = "Runtime Error";
	xOutObj.bCanChangeSize = false;
	xOutObj.bCanHideValue  = true;
	xOutObj.bBRAfterValue  = true;
	xOutObj.bBRBeforeValue = true;
	xOutObj.bShowInfo      = true;
	xOutObj.bShowValue     = true;
	xOutObj.sRelSym        = " ";
	xOutObj.sType          = "error message";
	xOutObj.iSize          = 0;

	m_bHasOutput = true;

	if (iErrLine >= 0)
	{
		xOutObj.sInfo  = CStrMem() << "Line " << iErrLine << ", Pos " << iErrPos;
		xOutObj.sValue = CStrMem() << "<font size=\"10\">File: <i>" << csErrFile << "</i></font><p>" << csErrText;

		m_xParse.InsertOutputObject(xOutObj);

		if (m_pEditor && !m_bVizOnlyMode)
		{
			m_bShowErrorInEditor = true;
		}
	}
	else
	{
		xOutObj.sInfo  = "";
		xOutObj.sValue = csErrText;

		m_xParse.InsertOutputObject(xOutObj);
		m_xParse.ResetErrors();
	}

	m_xParse.OverrideSetRenderLatex(false);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COGLWin::Draw()
{
	try
	{
		if (!m_xParse.IsCodeOK())
		{
			m_bResetTools = true;

			if (m_pOutput)
			{
				m_pOutput->SetText(" ");
			}

			// Standard reason for re-evaluation of script.
			SetExecModeVar(EXEC_MODE_NONE);
			return;
		}

		// Set standard color
		if (m_bUseLighting)
		{
			m_DrawBase.SetColor(MGray);
		}
		else
		{
			m_DrawBase.SetColor(MGray);
		}

		if (m_pToolBox)
		{
			m_pToolBox->ResetToolInUseFlags();
			m_pToolBar->ResetToolInUseFlags();
		}

		m_xParse.ToggleTempImageList();
		m_xParse.ResetTempImageList();

		// Reset Filter Modes
		m_xFilter.ResetModes();
		m_DrawBase.ResetModes();

		//CLU_LOG(">>>> >> Setting font...");
		// Reset Text Font
		m_xParse.GetCodeBase().SetFont("times", "rm", 12);
		m_xParse.GetCodeBase().GetDirectWrite().SetDefaultFontParameter();
		//CLU_LOG(">>>> >> ... setting font finished.");

		m_bFrameTransChanged = false;
		MultiV<float> vR(m_vRMain);

		m_vxTransform[0].ResetTransChanged();

		ClearOutput();
		ClearError();

		if (m_pOutput && m_bVizOnlyMode && m_pOutput->shown())
		{
			m_pOutput->hide();
		}

		try
		{
			m_pMainScene->Reset();
		}
		catch (Clu::CIException& ex)
		{
			throw CLU_EXCEPTION_NEST(__FUNCTION__, std::move(ex));
		}

		//CLU_LOG(">>>> >> Running code...");

		/************************************************************************/
		/* RUN CODE                                                             */
		/************************************************************************/
		if (m_xParse.RunCode())
		{
			m_bHasOutput = (m_xParse.GetOutputObjectList().size() > 0);

			if (m_pOutput && !m_bHasOutput)
			{
				m_pOutput->Clear();
			}

			if (m_vxTransform[0].HasTransChanged(0) || m_vxTransform[0].HasTransChanged(1) || m_vxTransform[0].HasTransChanged(2))
			{
				m_vxTransform[0].iFlags = 0;
				m_bFrameTransChanged    = true;
			}

			// Reset transform flags after redraw
			for (int i = 1; i <= 9; i++)
			{
				m_vxTransform[i].ResetChanged();
			}

			if (vR != m_vRMain)
			{
				m_bFrameTransChanged = true;
			}

			m_xParse.OverrideSetRenderLatex(false);
		}
		else
		{
			m_bHasError = true;
			m_bAnimCode = false;
			_CreateErrorOutput();
		}

		//CLU_LOG(">>>> >> ... running code finished");

		// Keep fixed tools but do not redraw here
		m_bPruneTools = true;

		// Reset Tool Name Variable
		if (m_iExecMode &
			(EXEC_MODE_TOOL |
			 EXEC_MODE_USER |
			 EXEC_MODE_LINK |
			 EXEC_MODE_KEYUP |
			 EXEC_MODE_KEYDOWN |
			 EXEC_MODE_ANIM |
			 EXEC_MODE_PICKOVER |
			 EXEC_MODE_PICKSEL |
			 EXEC_MODE_PICKDRAG |
			 EXEC_MODE_QUIT))
		{
			SetToolNameVar("None");
			SetLinkDataVar("None");
		}

		// Standard reason for re-evaluation of script.
		SetExecModeVar(EXEC_MODE_NONE);
	}
	catch (Clu::CIException& ex)
	{
		throw CLU_EXCEPTION_NEST(__FUNCTION__, std::move(ex));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COGLWin::DisplaySceneGraph()
{
	try
	{
		#ifdef _DEBUG
			double dT0, dT1, dT2;
		#endif

		TIMER_START(dT0);

		// Reset Perspective Angle to standard
		SetPerspectiveAngle(GLfloat(m_dViewAngle));

		// Toggle 2D view if 2D variable has been changed
		if (m_b2DViewVariableChanged)
		{
			Enable2dView(!m_b2DViewEnabled);
		}

		try
		{
			BeginDraw();
		}
		catch (Clu::CIException& ex)
		{
			throw CLU_EXCEPTION_NEST("Error beginning to draw", std::move(ex));
		}

		m_SceneApplyData.bInPickState = false;

		if (!m_bIsEmbedded)
		{
			GetModifiers();
		}

		m_SceneApplyData.bShiftDown = m_bShiftDown;
		m_SceneApplyData.bCtrlDown  = m_bCtrlDown;
		m_SceneApplyData.bAltDown   = m_bAltDown;

		// Set depth level data
		m_SceneApplyData.iDepthLevelCount = m_iDepthLevelCount;
		// The total width of a depth level including the gap to the next one
		m_SceneApplyData.dDepthLevelWidth = m_dDepthLevelWidth;
		// The gap between consecutive depth levels.
		m_SceneApplyData.dDepthLevelGap = m_dDepthLevelGap;
		// The near value of the central depth level
		m_SceneApplyData.dDepthLevelOrig = m_dDepthLevelOrig;

		// Disable point sprites and shader support for points
		glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
		glDisable(GL_POINT_SPRITE);
		glTexEnvf(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_FALSE);

		// In 2D view switch off any point attenuation by default
		if (m_b2DViewVariableChanged && m_b2DViewEnabled && glPointParameterfv)
		{
			float pfPntAtt[] = { 1.0f, 0.0f, 0.0f };
			glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, pfPntAtt);
		}

		int iDispCnt;
		double pdWorldProjection[16];
		if (m_bDrawColorStereo)
		{
			iDispCnt = 2;
			glGetDoublev(GL_PROJECTION_MATRIX, pdWorldProjection);
			m_SceneApplyData.bUseColorStereo = true;
		}
		else
		{
			iDispCnt                         = 1;
			m_SceneApplyData.bUseColorStereo = false;
		}

		m_SceneApplyData.bNeedAnimate = false;

		double pdWorldTransform[16];
		glGetDoublev(GL_MODELVIEW_MATRIX, pdWorldTransform);
		glEnable(GL_ALPHA_TEST);

		for (int iDisp = 0; iDisp < iDispCnt; ++iDisp)
		{
			// The current depth level index
			m_SceneApplyData.dDepthLevelIdx = 0.0;

			if (m_bDrawColorStereo)
			{
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glMultMatrixd(pdWorldProjection);

				if (iDisp == 1)
				{
					m_SceneApplyData.fColorStereoSep      = m_fColorStereoSep;
					m_SceneApplyData.fColorStereoDegAngle = m_fColorStereoDegAngle;

					glTranslatef(m_fColorStereoSep, 0, 0);
					glRotatef(m_fColorStereoDegAngle, 0, 1, 0);

					glMatrixMode(GL_MODELVIEW);
					glColorMask(m_pbColorMaskLeft[0], m_pbColorMaskLeft[1], m_pbColorMaskLeft[2], m_pbColorMaskLeft[3]);
				}
				else
				{
					m_SceneApplyData.fColorStereoSep      = -m_fColorStereoSep;
					m_SceneApplyData.fColorStereoDegAngle = -m_fColorStereoDegAngle;

					glTranslatef(-m_fColorStereoSep, 0, 0);
					glRotatef(-m_fColorStereoDegAngle, 0, 1, 0);

					glMatrixMode(GL_MODELVIEW);
					glColorMask(m_pbColorMaskRight[0], m_pbColorMaskRight[1], m_pbColorMaskRight[2], m_pbColorMaskRight[3]);
				}

				glClear(GL_DEPTH_BUFFER_BIT);
			}

			glAlphaFunc(GL_GREATER, 0.99f);

			// Draw Mouse Pointer
			/*
			glPushMatrix();

			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();

			glOrtho(0, m_iSizeX,
			m_iSizeY, 0,
			-10.0, 10.0);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glTranslatef( float( m_iMousePosX ), float( m_iMousePosY ), 0.0f );
			m_pMouseScene->Apply( COGLBaseElement::DRAW, m_SceneApplyData );

			glMatrixMode(GL_PROJECTION);
			glPopMatrix();

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			*/

			// /////////////////////////////////////////////////////////////////////////////////////////
			// /////////////////////////////////////////////////////////////////////////////////////////
			// Use the following block to draw the scene in picking mode
			//glMatrixMode(GL_PROJECTION);
			//glGetDoublev(GL_PROJECTION_MATRIX, m_SceneApplyData.pdGlobalProjMat);
			//glMatrixMode(GL_MODELVIEW);

			//m_SceneApplyData.InitPickNames();
			//m_pMainScene->Apply(COGLBaseElement::PICK, m_SceneApplyData);
			// /////////////////////////////////////////////////////////////////////////////////////////
			// /////////////////////////////////////////////////////////////////////////////////////////

			// Ensure that alpha part of pick scenes is drawn
			m_SceneApplyData.ResetPickDrawFlags();

			// First pass draws all opaque objects
			m_SceneApplyData.bInTransparencyPass = false;
			m_SceneApplyData.bInFirstRenderPass  = true;

			// This is or is not the last render pass
			m_SceneApplyData.bInLastRenderPass = !m_bDrawTransparency;
			m_SceneApplyData.pCurShader        = nullptr;
			m_SceneApplyData.pCurRenderTarget  = nullptr;

			if (glUseProgram)
			{
				glUseProgram(0);
			}

			/************************************************************************/
			/* APPLY #1                                                             */
			/************************************************************************/
			try
			{
				TIMER_START(dT1);
				m_pMainScene->Apply(COGLBaseElement::DRAW, m_SceneApplyData);
				CleanFrameStack();
				TIMER_END(dT1);
			}
			catch (Clu::CIException& ex)
			{
				throw CLU_EXCEPTION_NEST("Error applying first stage of main scene", std::move(ex));
			}

			/************************************************************************/
			/* Redraw Scene to enable transparency                                  */
			/************************************************************************/
			if (m_bDrawTransparency)
			{
				try
				{
					m_SceneApplyData.xMatrixStack.Pop(Clu::CMatrixStack::Texture);
					m_SceneApplyData.xMatrixStack.Push(Clu::CMatrixStack::Texture);

					m_SceneApplyData.xMatrixStack.Pop(Clu::CMatrixStack::ModelView);
					m_SceneApplyData.xMatrixStack.Push(Clu::CMatrixStack::ModelView);

					// Apply World Transform
					glMultMatrixd(pdWorldTransform);

					glEnable(GL_BLEND);
					glAlphaFunc(GL_LESS, 1.0f);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					glDepthMask(GL_FALSE);

					// Ensure that alpha part of pick scenes is drawn
					m_SceneApplyData.ResetPickDrawFlags();

					// Second pass draws all transparent objects
					m_SceneApplyData.bInTransparencyPass = true;
					m_SceneApplyData.bInFirstRenderPass  = false;
					// This is the last render pass
					m_SceneApplyData.bInLastRenderPass = true;
					m_SceneApplyData.pCurShader        = nullptr;
					m_SceneApplyData.pCurRenderTarget  = nullptr;

					if (glUseProgram)
					{
						glUseProgram(0);
					}
				}
				catch (Clu::CIException& ex)
				{
					throw CLU_EXCEPTION_NEST("Error preparing transparency stage of main scene", std::move(ex));
				}

				/************************************************************************/
				/* APPLY #2                                                             */
				/************************************************************************/
				try
				{
					TIMER_START(dT2);
					m_pMainScene->Apply(COGLBaseElement::DRAW, m_SceneApplyData);
					CleanFrameStack();
					TIMER_END(dT2);
				}
				catch (Clu::CIException& ex)
				{
					throw CLU_EXCEPTION_NEST("Error applying transparency stage of main scene", std::move(ex));
				}

				glDepthMask(GL_TRUE);
				glDisable(GL_BLEND);
			}

			if (m_bDrawColorStereo)
			{
				m_SceneApplyData.xMatrixStack.Pop(Clu::CMatrixStack::Texture);
				m_SceneApplyData.xMatrixStack.Push(Clu::CMatrixStack::Texture);

				m_SceneApplyData.xMatrixStack.Pop(Clu::CMatrixStack::ModelView);
				m_SceneApplyData.xMatrixStack.Push(Clu::CMatrixStack::ModelView);

				// Apply World Transform
				glMultMatrixd(pdWorldTransform);
			}
		}

		if (m_bDrawColorStereo)
		{
			glColorMask(true, true, true, true);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glMultMatrixd(pdWorldProjection);
			glMatrixMode(GL_MODELVIEW);
		}

		glDisable(GL_ALPHA_TEST);

		if (m_SceneApplyData.pCurRenderTarget != nullptr)
		{
			// Clean-up rendering to render target
			COGLRenderTarget* pRT = dynamic_cast<COGLRenderTarget*>(m_SceneApplyData.pCurRenderTarget);
			if (pRT != nullptr)
			{
				pRT->Finalize(true);
			}

			m_SceneApplyData.pCurRenderTarget = nullptr;
		}

		// Disable all textures on all Texture units !
		if (glActiveTexture)
		{
			for (int iTexUnit = 0; iTexUnit < OGL_MAX_TEX_UNITS; ++iTexUnit)
			{
				glActiveTexture(GL_TEXTURE0 + iTexUnit);
				glBindTexture(GL_TEXTURE_2D, 0);
				glDisable(GL_TEXTURE_2D);
			}

			glActiveTexture(GL_TEXTURE0);
		}

		// Always call this function after drawing
		EndDraw();

		TIMER_END(dT0);
	}
	catch (Clu::CIException& ex)
	{
		throw CLU_EXCEPTION_NEST("Error displaying scene graph", std::move(ex));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COGLWin::CleanFrameStack()
{
	if (!m_SceneApplyData.IsStackEmptyModelview())
	{
		do
		{
			m_SceneApplyData.xMatrixStack.Pop(Clu::CMatrixStack::ModelView);
		}
		while (m_SceneApplyData.PopModelview());
	}

	if (!m_SceneApplyData.IsStackEmptyProjection())
	{
		do
		{
			m_SceneApplyData.xMatrixStack.Pop(Clu::CMatrixStack::Projection);
		}
		while (m_SceneApplyData.PopProjection());
	}

	if (!m_SceneApplyData.IsStackEmptyTexture())
	{
		do
		{
			m_SceneApplyData.xMatrixStack.Pop(Clu::CMatrixStack::Texture);
		}
		while (m_SceneApplyData.PopTexture());
	}
}

//////////////////////////////////////////////////////////////////////
// At Animation Start

bool COGLWin::AtAnimEnd()
{
	LockVis(INFINITE);

	m_SceneApplyData.bAnimate = false;

	m_SceneApplyData.dTime     = 0.0;
	m_SceneApplyData.dTimeStep = 0.0;
	m_SceneApplyData.dPickTime = 0.0;

	UnlockVis();
	return true;
}

//////////////////////////////////////////////////////////////////////
// At Animation Start

bool COGLWin::AtAnimStart()
{
/*
		CCodeVar Var;
		TCVScalar dVal;

		if (m_xParse.GetVar("_DoAnimate", Var))
		{
				if (Var.CastToScalar(dVal) && dVal != 0)
				{
						m_bAnimCode = true;
						////SetAnimCode(true);
				}
				else
				{
						m_bAnimCode = false;
						////SetAnimCode(false);
				}
		}
		else
		{
				//m_xParse.NewVar("_DoAnimate") = 1;
				////SetAnimCode(true);
				m_bAnimCode = false;
		}
*/
	LockVis(INFINITE);

	m_SceneApplyData.bAnimate = true;

	UnlockVis();
//	COGLWinFLTK::AtAnimStart();

	return true;
}

/////////////////////////////////////////////////////////////////////
/// Set Animation Time Step

bool COGLWin::SetAnimationTimeStep(int iTimeStep)
{
	LockVis(INFINITE);

	if (!CCLUDrawBase::SetAnimationTimeStep(iTimeStep))
	{
		UnlockVis();
		return false;
	}

	m_bAnimPauseChanged = true;
	UnlockVis();
	return true;
}

////////////////////////////////////////////////////////
/// Main Loop of visualization thread

void COGLWin::RunVisLoop(void* pData)
{
	#ifdef _DEBUG
		// TODO: Implement thread name setting
		//Clu::Threading::SetCurrentThreadName("CluViz Visualization Loop");
	#endif

	HANDLE phWait[2];
	//TTimeStruct Time1, Time2;
	//double dTime1, dTime2;
	int iAnimCodeWait = 1;

	COGLWin* pThis = (COGLWin*) pData;
	bool bAnimCode = pThis->m_bAnimCode;

	HANDLE hThread = ::GetCurrentThread();
	::SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);

	pThis->m_fAnimTimeStep   = 0.0f;
	pThis->m_bDoAnimCodeStep = false;
	//GETTIME( &Time1 );
	//dTime1 = pThis->GetTime();

	// Create main rendering context
	pThis->CreateMainRC();

	if (pThis->m_bInitFirst)
	{
		// Initialize OpenGL in function modules
		CCLUParse::InitVizModules();
	}

	// Initialize OpenGL settings.
	// This also reads the GL and GLSL version of the current rendering context
	pThis->CLUDrawBaseInit(pThis->m_bInitFirst);

	int iMaxClipPlanes = 0;
	glGetIntegerv(GL_MAX_CLIP_PLANES, &iMaxClipPlanes);
	pThis->m_SceneApplyData.mClipPlanes.Set(iMaxClipPlanes * 4);

	// Set version variables
	pThis->m_xParse.DefineVersionGL(pThis->m_iGLVMajor, pThis->m_iGLVMinor);
	pThis->m_xParse.DefineVersionGLSL(pThis->m_iGLSLVMajor, pThis->m_iGLSLVMinor);

	// Standard depth range
	// Additional scenes with view ports can select a separate depth range. Each depth range has a width of m_dDepthLevelWidth.
	glDepthRange(pThis->m_dDepthLevelOrig, pThis->m_dDepthLevelOrig + pThis->m_dDepthLevelWidth - pThis->m_dDepthLevelGap);

	// Initialize time variables for initialization
	pThis->m_dTimeStart = pThis->GetTime();
	pThis->m_dTimePrev  = pThis->m_dTimeStart;

	//GETTIME(&pThis->m_tmStart);
	//memcpy(&pThis->m_tmPrev, &pThis->m_tmStart, sizeof(TTimeStruct));

	pThis->m_fTotalTime = 0;
	pThis->m_fTimeStep  = 0;

	// Indicate that thread is running
	WaitForSingleObject(pThis->m_hMutexVisRun, INFINITE);

	// If this variable is set to false from external thread this thread ends.
	pThis->m_bRunVisLoop = true;

	// Create mutex for visualization thread access
	pThis->m_hMutexVis = CreateMutex(NULL, TRUE, NULL);

	phWait[0] = pThis->m_hEventVisTimer;
	phWait[1] = pThis->m_hEventVisNow;

	// Tell other thread that initialization is finished
	SetEvent(pThis->m_hEventVis);
	SwitchToThread();

	try
	{
		// Start main visualization loop
		while (pThis->m_bRunVisLoop)
		{
			// Release visualization thread mutex
			ReleaseMutex(pThis->m_hMutexVis);
			SwitchToThread();

			//CLU_LOG(">> Waiting for timer or now event...");
			// Wait for timer to signal next visualization step OR signal for forced visualization.
			WaitForMultipleObjects(2, phWait, FALSE, INFINITE);	// TODO: Timeout?
			//CLU_LOG(">> ... got it!");

			// Now wait for mutex
			WaitForSingleObject(pThis->m_hMutexVis, INFINITE);	// TODO: Timeout?
			//CLU_LOG(">> ... Got visualization mutex");

			ResetEvent(pThis->m_hEventVisNow);

			if (pThis->m_bVisActive || (WaitForSingleObject(pThis->m_hEventForceVis, 0) == WAIT_OBJECT_0))
			{
				//CLU_LOG(">> ... start visualize");
				ResetEvent(pThis->m_hEventForceVis);

				if (!bAnimCode && pThis->m_bAnimCode)
				{
					iAnimCodeWait = pThis->m_iAnimPause / pThis->m_iVisTimeStep;
					if (iAnimCodeWait < 1)
					{
						iAnimCodeWait = 1;
					}
				}

				bAnimCode = pThis->m_bAnimCode;

				// Ensure that code is only animated in set time steps.
				if (bAnimCode)
				{
					if (--iAnimCodeWait)
					{
						pThis->m_bDoAnimCodeStep = false;
					}
					else
					{
						pThis->m_bDoAnimCodeStep = true;

						iAnimCodeWait = pThis->m_iAnimPause / pThis->m_iVisTimeStep;
						if (iAnimCodeWait < 1)
						{
							iAnimCodeWait = 1;
						}

						pThis->m_fAnimTimeStep = 1e-3f * float(iAnimCodeWait * pThis->m_iVisTimeStep);
					}
				}

				//CLU_LOG(">> Entering timed func...");
				pThis->TimedFunc();
				//CLU_LOG(">> ...finished!");
			}

			//CLU_LOG(">> ... set event vis");
			// Tell other threads that visualization has finished a loop
			SetEvent(pThis->m_hEventVis);

			//CLU_LOG(">> ... end loop.");
		}
	}
	catch (Clu::CIException& ex)
	{
		#ifndef CLU_RTM
			printf("\n\nCLU Exception:\n\n%s\n\n", ex.ToString().ToCString());
		#endif

		if (pThis->m_pErrorEventCallbackFunc)
		{
			pThis->m_pErrorEventCallbackFunc(ex, pThis->m_pErrorEventCallbackContext);
		}
	}

	// End timer thread
	pThis->m_bRunVisTimerLoop = false;
	::Sleep(2 * pThis->m_iVisTimeStep);
	WaitForSingleObject(pThis->m_hEventVisTimer, INFINITE);
	ResetEvent(pThis->m_hEventVisTimer);

	CloseHandle(pThis->m_hMutexVis);
	pThis->m_hMutexVis = 0;

	// Delete rendering context
	wglMakeCurrent(0, 0);
	wglDeleteContext(pThis->m_hGLRC);
	pThis->m_hGLRC = 0;

	::ReleaseDC(pThis->GetGLWindowHandle(), pThis->m_hGLDC);
	pThis->m_hGLDC = 0;

	pThis->DestroySharedRC();
	pThis->m_hShareExtGLRC = 0;

	// Indicate that thread stopped
	ReleaseMutex(pThis->m_hMutexVisRun);
}

////////////////////////////////////////////////////////
/// Main Loop of visualization timer thread

void COGLWin::RunVisTimerLoop(void* pData)
{
	#ifdef _DEBUG
		//Clu::Threading::SetCurrentThreadName("CluViz Timer Loop");
	#endif

	COGLWin* pThis = (COGLWin*) pData;

	__int64 i64Time1 = pThis->GetTime64();

	HANDLE hThread = ::GetCurrentThread();
	::SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);

	// If this variable is set to false from external thread this thread ends.
	pThis->m_bRunVisTimerLoop = true;

	while (pThis->m_bRunVisTimerLoop)
	{
		::Sleep(1);

		__int64 i64Time2 = pThis->GetTime64();
		if (int(i64Time2 - i64Time1) >= pThis->m_iVisTimeStep)
		{
			i64Time1 = i64Time2;
			SetEvent(pThis->m_hEventVisTimer);
		}
	}

	SetEvent(pThis->m_hEventVisTimer);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COGLWin::TimedFunc()
{
	try
	{

		// Try to take rendering context
		if (wglMakeCurrent(m_hGLDC, m_hGLRC) == FALSE)
		{
			throw CLU_EXCEPTION("Error obtaining rendering context");
		}

		// Check for starting error
		GLenum uGLError = glGetError();
		if (uGLError != GL_NO_ERROR)
		{
			throw CLU_EXCEPTION(Clu::OpenGL::GetErrorText(uGLError, "TimedFunc"));
		}

		//double dT1, dT2, dT3, dT4, dT0;
		//bool bDoTimer = false;

		//if ( m_bDoRedisplay )
		//{
		//	bDoTimer = true;
		//	dT0 = GetTime();
		//}

		// Timed function runs now and can process events from Mouse Click
		m_bMouseClickEventNeedsProcessingByTimedFunc = false;

		if (m_bDoReshape)
		{
			m_bDoReshape = false;
			CCLUDrawBase::Reshape(m_iReshapeW, m_iReshapeH);
			AddExecModeVar(EXEC_MODE_RESHAPE);
		}

		// If needed evaluate pick state before visualization. This is for example needed if the user releases a mouse button
		if (m_bPickBeforeDrawOnce)
		{
			Picking(MOUSEPICK_OVER, m_iPickX, m_iPickY, m_SceneApplyData.iPickW, m_SceneApplyData.iPickH);

			m_bDoPicking          = false;
			m_bPickBeforeDrawOnce = false;
		}

		if (m_bDoAnimDisplay || (m_bAnimCode && m_bDoAnimCodeStep) || m_iAnimRotType || m_bDoRedisplay)
		{
			m_bDoRedisplay = false;

			CCodeVar Var;
			TCVScalar dVal;

			// Check whether scene is animated
			if (m_bDoAnimDisplay || (m_bAnimCode && m_bDoAnimCodeStep) || m_iAnimRotType)
			{
				// Reset animation time if this is the first animation step
				if (m_bResetAnimTime)
				{
					m_bResetAnimTime = false;

					m_SceneApplyData.dPickTime = 0.0;

					// Initialize time variables for initialization
					//GETTIME(&m_tmStart);
					//memcpy(&m_tmPrev, &m_tmStart, sizeof(TTimeStruct));
					m_dTimeStart = m_dTimePrev = GetTime();
				}

				// Record time step each time scene is drawn in animation mode
				CalcTimeElapsed();

				m_SceneApplyData.dTime      = (double) m_fTotalTime;
				m_SceneApplyData.dTimeStep  = (double) m_fTimeStep;
				m_SceneApplyData.dPickTime += (double) m_fTimeStep;
			}

			if (m_bAnimCode)
			{
				CCodeVar& rVarDTime = m_xParse.GetVar("dTime");
				if (rVarDTime.Type() == PDT_DOUBLE)
				{
					*((double*) rVarDTime.Val()) = (double) m_fAnimTimeStep;
				}

				CCodeVar& rVarTime = m_xParse.GetVar("Time");
				if (rVarTime.Type() == PDT_DOUBLE)
				{
					*((double*) rVarTime.Val()) = (double) m_fTotalTime;
				}
			}

			if (m_iAnimRotType == ANIMROT_MOUSE)
			{
				if (HasMouseRotHistChanged())
				{
					m_vRot = GetMouseRotHistSum();
					//ResetMouseRotHist();
				}

				float fVal   = 2e-2f * float(sqrt(m_vRot.Mag2()));
				float fAngle = m_fTimeStep * fVal;

				if ((fAngle != 0.0f) && (m_vRot != m_E3Base.vZero))
				{
					if (!m_bUseLocalRot)
					{
						m_vRMain = m_E3Base.Rotor(m_vRot, fAngle, false) & m_vRMain;
					}
					else
					{
						m_vRMain = m_vRMain & m_E3Base.Rotor(m_vRot, fAngle, false);
					}

					m_vxTransform[0].SetRotChanged(0);
					m_vxTransform[0].SetRotChanged(1);
					m_vxTransform[0].SetRotChanged(2);
				}
			}

			if (m_bCodeChanged)
			{
				SetExecModeVar(EXEC_MODE_CHANGE);
			}

			if (m_bAnimCode && m_bDoAnimCodeStep)
			{
				AddExecModeVar(EXEC_MODE_ANIM);
			}

			m_SceneApplyData.bAnimate = m_bDoAnimDisplay;

			//dT1 = GetTime();
			DoDisplay();
			//dT1 = GetTime() - dT1;

			if (m_bDoDisplaySceneGraph)
			{
				//dT2 = GetTime();
				glFlush();
				//dT2 = GetTime() - dT2;

				if (m_bDoSaveScreen)
				{
					m_bDoSaveScreen = false;

					CCLUDrawBase::SaveScreen2BMP(m_sSaveScreenFilename.Str());
				}

				//dT3 = GetTime();
				SwapBuffers(m_hGLDC);
				//dT3 = GetTime() - dT3;

				m_bDoAnimDisplay = m_SceneApplyData.bNeedAnimate;

				if (!HasError() && m_xParse.GetVar("_DoAnimate", Var))
				{
					if (Var.CastToScalar(dVal) && (dVal != 0))
					{
						if (m_bAnimPauseChanged)
						{
							m_bAnimPauseChanged = false;						// Set by SetAnimationTimeStep
						}
						m_bAnimCode = true;
					}
					else
					{
						m_bAnimCode = false;
					}
				}
				else
				{
					m_bAnimCode = false;
				}

				// Flag that animation time should be reset when animation starts the next time
				if (!(m_bDoAnimDisplay || m_bAnimCode || m_iAnimRotType))
				{
					//// Initialize time variables
					//GETTIME(&m_tmStart);
					//memcpy(&m_tmPrev, &m_tmStart, sizeof(TTimeStruct));
					m_dTimeStart               = m_dTimePrev = GetTime();
					m_fTotalTime               = 0.0f;
					m_fTimeStep                = 0.0f;
					m_fAnimTimeStep            = 0.0f;
					m_SceneApplyData.dPickTime = 0.0;
					m_bResetAnimTime           = true;
				}
			}
		}

		if (m_bDoDisplaySceneGraph && (m_bDoPicking || m_bPickAfterDrawOnce))
		{
			//dT4 = GetTime();
			Picking(MOUSEPICK_OVER, m_iPickX, m_iPickY, m_SceneApplyData.iPickW, m_SceneApplyData.iPickH);
			//dT4 = GetTime() - dT4;

			m_bPickAfterDrawOnce = false;
			m_bDoPicking         = false;
		}

		//if ( bDoTimer )
		//{
		//	dT0 = GetTime() - dT0;
		//	return true;
		//}
		
		// Give up rendering context so that other threads may draw into it.
		wglMakeCurrent(0, 0);

		return true;
	}
	catch (Clu::CIException& ex)
	{
		// Give up rendering context so that other threads may draw into it.
		wglMakeCurrent(0, 0);
		throw CLU_EXCEPTION_NEST("Error processing timed function", std::move(ex));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COGLWin::LockVis(int iWait)
{
	return WaitForSingleObject(m_hMutexVis, iWait) != WAIT_TIMEOUT;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COGLWin::UnlockVis()
{
	ReleaseMutex(m_hMutexVis);
}
