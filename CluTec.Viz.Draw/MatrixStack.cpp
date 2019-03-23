////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      MatrixStack.cpp
//
// summary:   Implements the matrix stack class
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

#include "stdafx.h"
#include "MatrixStack.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// namespace: Clu
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Clu
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void CMatrixStack::Push(EMatrixMode eMode)
	{
		try
		{
			// Activate the given matrix mode. This is required since some callers rely on this behavior
			CLU_OGL_CALL(glMatrixMode((unsigned)eMode));

			if (eMode == EMatrixMode::ModelView)
			{
				// Get matrix and push it
				m_xStackModelView.push(SMatrix(GL_MODELVIEW_MATRIX));
			}
			else if (eMode == EMatrixMode::Projection)
			{
				// Get matrix and push it
				m_xStackProjection.push(SMatrix(GL_PROJECTION_MATRIX));
			}
			else
			{
				// Save active texture unit
				int iActiveTexUnit;
				CLU_OGL_CALL(glGetIntegerv(GL_ACTIVE_TEXTURE, &iActiveTexUnit));

				// Push the matrix for each texture unit
				for (int iTexUnit = 0; iTexUnit < OGL_MAX_TEX_UNITS; iTexUnit++)
				{
					// Activate current texture unit
					CLU_OGL_CALL(glActiveTexture(GL_TEXTURE0 + iTexUnit));

					// Get matrix and push it
					m_xStackTextures[iTexUnit].push(SMatrix(GL_TEXTURE_MATRIX));
				}

				// Restore active texture unit
				CLU_OGL_CALL(glActiveTexture(iActiveTexUnit));
			}
		}
		catch (Clu::CIException& ex)
		{
			throw CLU_EXCEPTION_NEST(CLU_S "Error pushing matrix of mode '" << _GetModeName(eMode).c_str() << "'", std::move(ex));
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void CMatrixStack::Pop(EMatrixMode eMode)
	{
		try
		{
			// Activate the given matrix mode. This is required for setting the desired matrix
			CLU_OGL_CALL(glMatrixMode((unsigned)eMode));

			if (eMode == EMatrixMode::ModelView)
			{
				// Pop top of the stack and apply matrix
				_Pop(m_xStackModelView);
			}
			else if (eMode == EMatrixMode::Projection)
			{
				// Pop top of the stack and apply matrix
				_Pop(m_xStackProjection);
			}
			else
			{
				// Save active texture unit
				int iActiveTexUnit;
				CLU_OGL_CALL(glGetIntegerv(GL_ACTIVE_TEXTURE, &iActiveTexUnit));

				// Push the matrix for each texture unit
				for (int iTexUnit = 0; iTexUnit < OGL_MAX_TEX_UNITS; iTexUnit++)
				{
					// Activate current texture unit
					CLU_OGL_CALL(glActiveTexture(GL_TEXTURE0 + iTexUnit));

					// Pop top of the stack and apply matrix
					_Pop(m_xStackTextures[iTexUnit]);
				}

				// Restore active texture unit
				CLU_OGL_CALL(glActiveTexture(iActiveTexUnit));
			}
		}
		catch (Clu::CIException& ex)
		{
			throw CLU_EXCEPTION_NEST(CLU_S "Error popping top of the stack of mode '" << _GetModeName(eMode).c_str() << "'", std::move(ex));
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void CMatrixStack::_Pop(std::stack<SMatrix>& xStack)
	{
		if (xStack.empty())
		{
			throw CLU_EXCEPTION("The desired stack is empty");
		}

		// Get top of the stack and apply matrix
		CLU_OGL_CALL(glLoadMatrixf(xStack.top().pfData));

		// Remove the top of the stack
		xStack.pop();
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool CMatrixStack::IsEmpty() const
	{
		bool bIsEmpty = true;
		for (int iTexUnit = 0; iTexUnit < OGL_MAX_TEX_UNITS; iTexUnit++)
		{
			bIsEmpty &= m_xStackTextures[iTexUnit].empty();
		}

		bIsEmpty &= m_xStackProjection.empty();
		bIsEmpty &= m_xStackModelView.empty();

		return bIsEmpty;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool CMatrixStack::IsEmpty(EMatrixMode eMode) const
	{
		bool bIsEmpty = true;
		switch (eMode)
		{
		case Clu::CMatrixStack::ModelView:
			bIsEmpty &= m_xStackModelView.empty();
			break;

		case Clu::CMatrixStack::Projection:
			bIsEmpty &= m_xStackProjection.empty();
			break;

		case Clu::CMatrixStack::Texture:
			for (int iTexUnit = 0; iTexUnit < OGL_MAX_TEX_UNITS; iTexUnit++)
			{
				bIsEmpty &= m_xStackTextures[iTexUnit].empty();
			}

			break;
		default:
			break;
		}

		return bIsEmpty;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void CMatrixStack::PushAll()
	{
		try
		{
			Push(EMatrixMode::Texture);
			Push(EMatrixMode::Projection);
			Push(EMatrixMode::ModelView);
		}
		catch (Clu::CIException& ex)
		{
			throw CLU_EXCEPTION_NEST("Error pushing all matrices", std::move(ex));
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void CMatrixStack::PopAll()
	{
		try
		{
			Pop(EMatrixMode::Texture);
			Pop(EMatrixMode::Projection);
			Pop(EMatrixMode::ModelView);
		}
		catch (Clu::CIException& ex)
		{
			throw CLU_EXCEPTION_NEST("Error popping all matrices", std::move(ex));
		}
	}
}
