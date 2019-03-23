////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLRenderTarget.cpp
//
// summary:   Implements the ogl render target class
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
#include "OGLRenderTarget.h"
#include "OGLBaseElementList.h"
#include "OGLScene.h"

#include "CluTec.Viz.OpenGL.Extensions\Extensions.h"
#include "CluTec.Viz.OpenGL/Api.h"

#include <algorithm>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
COGLRenderTarget::COGLRenderTarget()
{
	_Reset();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
COGLRenderTarget::~COGLRenderTarget()
{
	Destroy();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COGLRenderTarget::_Reset()
{
	m_uFboId        = 0;
	m_uResolveFboId = 0;

	m_uWidth       = 0;
	m_uHeight      = 0;
	m_uSampleCount = 0;

	memset(m_piViewport, 0, 4 * sizeof(GLint));
	memset(m_piScissorBox, 0, 4 * sizeof(GLint));
	memset(m_pdProjMat, 0, 16 * sizeof(GLdouble));
	m_bUseScissor = false;

	m_uApplyCount = 0;

	m_bEnabled                  = true;
	m_bEnableSingleRenderToFile = false;
	m_bRenderToFileSuccess      = false;
	m_sRenderToFilename         = "";
	m_refRenderScene.Clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COGLRenderTarget::Destroy()
{
	// Once a framebuffer object is deleted, its name is again unused and it has no attachments.
	// If a framebuffer that is currently bound is deleted, the default frame buffer is bound.

	if (m_uFboId != 0)
	{
		CLU_OGL_CALL(glDeleteFramebuffers(1, &m_uFboId));
	}

	if (m_uResolveFboId != 0)
	{
		CLU_OGL_CALL(glDeleteFramebuffers(1, &m_uResolveFboId));
	}

	if ((m_uFboId != 0) || (m_uResolveFboId != 0))
	{
		_Reset();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COGLRenderTarget::Create(unsigned uWidth, unsigned uHeight, unsigned uSampleCount, COGLBERepository* pxBERepo)
{
	try
	{
		Destroy();

		GLint iMaxSampleCount = 0;
		CLU_OGL_CALL(glGetIntegerv(GL_MAX_SAMPLES, &iMaxSampleCount));

		m_uSampleCount = std::min(uSampleCount, (unsigned) iMaxSampleCount);
		m_uWidth       = uWidth;
		m_uHeight      = uHeight;

		SAttachment xAttach;
		/************************************************************************/
		/* GL_READ_FRAMEBUFFER                                                  */
		/************************************************************************/
		if (m_uSampleCount > 0)
		{
			// If we have multisample FBOs, then we need another FBO to resolve the multisampling before reading the actual image.
			COGLRenderBuffer* pRBuffer      = new COGLRenderBuffer();
			COGLBEReference refResolveImage = pxBERepo->New(pRBuffer);

			pRBuffer->CreateImage(m_uWidth, m_uHeight, 0, CLUVIZ_IMG_RGBA, CLUVIZ_IMG_UNSIGNED_BYTE);

			CLU_OGL_CALL(glGenFramebuffers(1, &m_uResolveFboId));
			CLU_OGL_CALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_uResolveFboId));

			/************************************************************************/
			/* Resolve Color Attachment 0                                           */
			/************************************************************************/
			xAttach.eType     = Attach_RenderBuffer;
			xAttach.refTarget = refResolveImage;

			CLU_OGL_CALL(glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, AttachPnt_Color0, GL_RENDERBUFFER, pRBuffer->GetRenderBufferId()));
			CLU_OGL_CALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
		}

		/************************************************************************/
		/* GL_FRAMEBUFFER (GL_DRAW_FRAMEBUFFER)                                 */
		/************************************************************************/
		COGLRenderBuffer* pImage        = new COGLRenderBuffer();
		COGLRenderBuffer* pDepthStencil = new COGLRenderBuffer();

		COGLBEReference refImage        = pxBERepo->New(pImage);
		COGLBEReference refDepthStencil = pxBERepo->New(pDepthStencil);

		CLU_OGL_CALL(glGenFramebuffers(1, &m_uFboId));
		CLU_OGL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_uFboId));

		pImage->CreateImage(m_uWidth, m_uHeight, m_uSampleCount, CLUVIZ_IMG_RGBA, CLUVIZ_IMG_UNSIGNED_BYTE);
		pDepthStencil->CreateDepthStencil(m_uWidth, m_uHeight, m_uSampleCount);

		// Color Attachment 0
		xAttach.eType     = Attach_RenderBuffer;
		xAttach.refTarget = refImage;

		CLU_OGL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, AttachPnt_Color0, GL_RENDERBUFFER, pImage->GetRenderBufferId()));

		// Depth Stencil Attachment
		xAttach.eType     = Attach_RenderBuffer;
		xAttach.refTarget = refDepthStencil;

		CLU_OGL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, AttachPnt_DepthStencil, GL_RENDERBUFFER, pDepthStencil->GetRenderBufferId()));
		CLU_OGL_CALL(glCheckFramebufferStatus(GL_FRAMEBUFFER));
		CLU_OGL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}
	catch (Clu::CIException& ex)
	{
		throw CLU_EXCEPTION_NEST(__FUNCTION__, std::move(ex));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COGLRenderTarget::EnableSingleRenderToFile(COGLBEReference refRenderScene, const char* pcFilename)
{
	m_bEnableSingleRenderToFile = true;
	m_bRenderToFileSuccess      = false;
	m_sRenderToFilename         = pcFilename;
	m_refRenderScene            = refRenderScene;

	COGLScene* pRenderScene = dynamic_cast<COGLScene*>((COGLBaseElement*) m_refRenderScene);
	if (pRenderScene != nullptr)
	{
		pRenderScene->EnableDrawScene(true);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COGLRenderTarget::IsRenderToFileSuccessful()
{
	return m_bRenderToFileSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COGLRenderTarget::GetImage(COGLImage& xImage)
{
	if (m_uFboId == 0)
	{
		throw std::exception("Render target does not contain image data");
	}

	if ((m_uSampleCount > 0) && (m_uResolveFboId == 0))
	{
		throw std::exception("Render target does not contain a buffer to resolve multisampling");
	}

	if (!xImage.Create(m_uWidth, m_uHeight, CLUVIZ_IMG_RGBA, CLUVIZ_IMG_UNSIGNED_BYTE))
	{
		throw std::exception("Out of memory while creating image to copy framebuffer to");
	}

	if (m_uSampleCount > 0)
	{
		// Bind read and draw buffer
		CLU_OGL_CALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_uFboId));
		CLU_OGL_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uResolveFboId));

		// Copy a block of pixels from the read framebuffer to the draw framebuffer
		CLU_OGL_CALL(glBlitFramebuffer(0, 0, m_uWidth, m_uHeight, 0, 0, m_uWidth, m_uHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST));

		// Unbind draw buffer and bind read buffer
		CLU_OGL_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
		CLU_OGL_CALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_uResolveFboId));
	}
	else
	{
		// Bind read buffer
		CLU_OGL_CALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_uFboId));
	}

	// Prepare pixel read
	CLU_OGL_CALL(glReadBuffer(GL_COLOR_ATTACHMENT0));
	CLU_OGL_CALL(glPixelStorei(GL_PACK_ROW_LENGTH, 0));
	CLU_OGL_CALL(glPixelStorei(GL_PACK_SKIP_ROWS, 0));
	CLU_OGL_CALL(glPixelStorei(GL_PACK_SKIP_PIXELS, 0));
	CLU_OGL_CALL(glPixelStorei(GL_PACK_ALIGNMENT, 1));

	// Read image
	::LockImageAccess();
	glReadPixels(0, 0, m_uWidth, m_uHeight, CLUVIZ_IMG_RGBA, CLUVIZ_IMG_UNSIGNED_BYTE, xImage.GetDataPtr());
	::UnlockImageAccess();

	if (glGetError() != GL_NO_ERROR)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		throw std::exception("Error reading pixels from frame buffer");
	}

	xImage.FlipImage(false);

	CLU_OGL_CALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COGLRenderTarget::Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData)
{
	if ((eMode == COGLBaseElement::PICK) || (m_bEnabled == false))
	{
		return true;
	}

	// If some render target is already active, then finalize it.
	if (rData.pCurRenderTarget != nullptr)
	{
		// Clean-up rendering to render target
		COGLRenderTarget* pRT = dynamic_cast<COGLRenderTarget*>(rData.pCurRenderTarget);
		if (pRT != nullptr)
		{
			pRT->Finalize(rData.bInLastRenderPass);
		}

		rData.pCurRenderTarget = nullptr;
	}

	// Apply the default frame buffer. The redirection to the attachments is now disabled
	if (m_uFboId == 0)
	{
		CLU_OGL_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
		return true;
	}

	// Bind this render target as the frame buffer. All rendering operations will store their result in the attachments
	CLU_OGL_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uFboId));

	rData.pCurRenderTarget = this;

	// Store current viewport
	CLU_OGL_CALL(glGetIntegerv(GL_VIEWPORT, m_piViewport));

	// Store current scissor box and disable scissor test.
	CLU_OGL_CALL(glGetIntegerv(GL_SCISSOR_BOX, m_piScissorBox));
	CLU_OGL_CALL(glGetBooleanv(GL_SCISSOR_TEST, &m_bUseScissor));
	CLU_OGL_CALL(glDisable(GL_SCISSOR_TEST));

	// Store current projection matrix
	CLU_OGL_CALL(glGetDoublev(GL_PROJECTION_MATRIX, m_pdProjMat));

	CLU_OGL_CALL(glViewport(0, 0, m_uWidth, m_uHeight));

	CLU_OGL_CALL(glMatrixMode(GL_PROJECTION));
	float fRatioX = float(m_piViewport[2]) / float(m_uWidth);
	float fRatioY = float(m_piViewport[3]) / float(m_uHeight);
	if (fRatioX > fRatioY)
	{
		CLU_OGL_CALL(glScalef(1.0f, fRatioY / fRatioX, 1.0f));
	}
	else
	{
		CLU_OGL_CALL(glScalef(fRatioX / fRatioY, 1.0f, 1.0f));
	}

	CLU_OGL_CALL(glMatrixMode(GL_MODELVIEW));

	if (rData.bInFirstRenderPass)
	{
		if (m_uApplyCount == 0)
		{
			CLU_OGL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
		}

		++m_uApplyCount;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COGLRenderTarget::Finalize(bool bInLastRenderPass)
{
	CLU_OGL_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));

	if (bInLastRenderPass)
	{
		if (m_uApplyCount > 0)
		{
			--m_uApplyCount;
		}

		// Test whether this is the last finalize that will be called for render target object
		// in last render pass.
		if (m_uApplyCount == 0)
		{
			if (m_bEnableSingleRenderToFile && m_bEnabled)
			{
				m_bEnableSingleRenderToFile = false;

				COGLScene* pRenderScene = dynamic_cast<COGLScene*>((COGLBaseElement*) m_refRenderScene);
				if (pRenderScene != nullptr)
				{
					pRenderScene->EnableDrawScene(false);
				}

				COGLImage xImage;
				GetImage(xImage);
				m_bRenderToFileSuccess = xImage.SaveImage(m_sRenderToFilename.c_str());

				// Clear the scene reference
				m_refRenderScene.Clear();
			}
		}
	}

	glViewport(m_piViewport[0], m_piViewport[1], m_piViewport[2], m_piViewport[3]);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(m_pdProjMat);
	glMatrixMode(GL_MODELVIEW);

	if (m_bUseScissor)
	{
		glScissor(m_piScissorBox[0], m_piScissorBox[1], m_piScissorBox[2], m_piScissorBox[3]);
		glEnable(GL_SCISSOR_TEST);
	}
}
