////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLFrameStack.cpp
//
// summary:   Implements the ogl frame stack class
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

// OGLFrameStack.cpp: Implementierung der Klasse COGLFrameStack.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLFrameStack.h"
#include "OGLBaseElementList.h"

#include "CluTec.Viz.Fltk\Fl_math.h"
#include "CluTec.Viz.OpenGL/Api.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLFrameStack::COGLFrameStack()
{
	m_sTypeName = "FrameStack";

	Reset();
}

COGLFrameStack::COGLFrameStack(EFrameMode eFrameMode, bool bDoPush)
{
	m_sTypeName = "FrameStack";

	m_bDoPush    = bDoPush;
	m_eFrameMode = eFrameMode;
}

COGLFrameStack::COGLFrameStack(const COGLFrameStack& rTrans)
{
	m_sTypeName = "Frame";

	*this = rTrans;
}

COGLFrameStack& COGLFrameStack::operator=(const COGLFrameStack& rTrans)
{
	COGLBaseElement::operator=(rTrans);

	m_eFrameMode = rTrans.m_eFrameMode;
	m_bDoPush    = rTrans.m_bDoPush;

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Reset

void COGLFrameStack::Reset()
{
	m_eFrameMode = MODELVIEW;
	m_bDoPush    = true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COGLFrameStack::Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData)
{
	try
	{
		// Store current matrix mode
		int iMatrixMode;
		CLU_OGL_CALL(glGetIntegerv(GL_MATRIX_MODE, &iMatrixMode));

		if (m_bDoPush)
		{
			switch (m_eFrameMode)
			{
			case COGLFrameStack::ALL:
				rData.xMatrixStack.PushAll();
				rData.PushModelview();
				rData.PushProjection();
				rData.PushTexture();
				break;
			case COGLFrameStack::MODELVIEW:
				rData.xMatrixStack.Push(Clu::CMatrixStack::ModelView);
				rData.PushModelview();
				break;
			case COGLFrameStack::PROJECTION:
				rData.xMatrixStack.Push(Clu::CMatrixStack::Projection);
				rData.PushProjection();
				break;
			case COGLFrameStack::TEXTURE:
				rData.xMatrixStack.Push(Clu::CMatrixStack::Texture);
				rData.PushTexture();
				break;
			default:
				break;
			}
		}
		else
		{
			switch (m_eFrameMode)
			{
			case COGLFrameStack::ALL:
				rData.xMatrixStack.PopAll();
				rData.PopModelview();
				rData.PopProjection();
				rData.PopTexture();
				break;
			case COGLFrameStack::MODELVIEW:
				rData.xMatrixStack.Pop(Clu::CMatrixStack::ModelView);
				rData.PopModelview();
				break;
			case COGLFrameStack::PROJECTION:
				rData.xMatrixStack.Pop(Clu::CMatrixStack::Projection);
				rData.PopProjection();
				break;
			case COGLFrameStack::TEXTURE:
				rData.xMatrixStack.Pop(Clu::CMatrixStack::Texture);
				rData.PopTexture();
				break;
			default:
				break;
			}
		}

		// Restore former matrix mode
		CLU_OGL_CALL(glMatrixMode(iMatrixMode));
		return true;
	}
	catch (Clu::CIException& ex)
	{
		throw CLU_EXCEPTION_NEST("Error applying frame stack", std::move(ex));
	}
}
