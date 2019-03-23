////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLReadBitmap.cpp
//
// summary:   Implements the ogl read bitmap class
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

// OGLColor.cpp: Implementierung der Klasse COGLReadBitmap.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CluTec.Viz.Fltk\Fl_math.h"
#include "OGLReadBitmap.h"
#include "OGLTexture.h"
#include "CluTec.Viz.OpenGL.Extensions\Extensions.h"
#include "CluTec.Viz.OpenGL/Api.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLReadBitmap::COGLReadBitmap()
{
	m_sTypeName = "Capture";

	Reset();
}

COGLReadBitmap::COGLReadBitmap(const COGLReadBitmap& rObj)
{
	m_sTypeName = "Capture";

	*this = rObj;
}

//////////////////////////////////////////////////////////////////////
// Copy Operator

COGLReadBitmap& COGLReadBitmap::operator=(const COGLReadBitmap& rObj)
{
	m_bEnabled = rObj.m_bEnabled;

	m_eTarget    = rObj.m_eTarget;
	m_refTexture = rObj.m_refTexture;
	m_refImage   = rObj.m_refImage;

	m_dX1 = rObj.m_dX1;
	m_dY1 = rObj.m_dY1;
	m_dZ1 = rObj.m_dZ1;

	m_dX2 = rObj.m_dX2;
	m_dY2 = rObj.m_dY2;
	m_dZ2 = rObj.m_dZ2;

	return *this;
}

//////////////////////////////////////////////////////////////////////
// Reset Variables

void COGLReadBitmap::Reset()
{
	m_bEnabled = true;

	m_eTarget = READ_TO_NONE;
	m_refTexture.Clear();

	m_dX1 = 0.0;
	m_dY1 = 0.0;
	m_dZ1 = 0.0;

	m_dX2 = 0.0;
	m_dY2 = 0.0;
	m_dZ2 = 0.0;

	m_iTrgX = 0;
	m_iTrgY = 0;
	m_iTrgW = 0;
	m_iTrgH = 0;
}

//////////////////////////////////////////////////////////////////////
/// Get Window area coordinates from given coordinates

void COGLReadBitmap::GetWinArea(int& iX, int& iY, int& iWidth, int& iHeight)
{
	int piViewport[4];
	double pdModelView[16];
	double pdProjection[16];

	double dWinX1, dWinY1, dWinZ1;
	double dWinX2, dWinY2, dWinZ2;

	glGetIntegerv(GL_VIEWPORT, piViewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, pdModelView);
	glGetDoublev(GL_PROJECTION_MATRIX, pdProjection);

	// Transform position to window coordinates
	gluProject(m_dX1, m_dY1, m_dZ1,
			pdModelView, pdProjection, piViewport,
			&dWinX1, &dWinY1, &dWinZ1);

	gluProject(m_dX2, m_dY2, m_dZ2,
			pdModelView, pdProjection, piViewport,
			&dWinX2, &dWinY2, &dWinZ2);

	iX      = int(floor(dWinX1));
	iY      = int(floor(dWinY1));
	iWidth  = int(abs(floor(dWinX2 - dWinX1)) + 1);
	iHeight = int(abs(floor(dWinY2 - dWinY1)) + 1);

	if (iX < piViewport[0])
	{
		iX = piViewport[0];
	}

	if (iY < piViewport[1])
	{
		iY = piViewport[1];
	}

	if (iX + iWidth - 1 > piViewport[0] + piViewport[2] - 1)
	{
		iWidth = piViewport[0] + piViewport[2] - iX;
	}

	if (iY + iHeight > piViewport[1] + piViewport[3])
	{
		iHeight = piViewport[1] + piViewport[3] - iY;
	}
}

//////////////////////////////////////////////////////////////////////
/// Apply Color

bool COGLReadBitmap::Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData)
{
	if ((eMode == COGLBaseElement::PICK) ||
	    (m_eTarget == READ_TO_NONE) ||
	    !m_bEnabled ||
	    !rData.bInLastRenderPass		// Only capture in last render pass
	    )
	{
		return true;
	}

	int iX, iY, iWidth, iHeight;
	int iImgType, iDataType, iBytesPerPixel;
	int iGLImgType, iGLDataType;

	if ((m_eTarget == READ_TO_IMAGE) &&
	    m_refImage.IsValid())
	{
		GetWinArea(iX, iY, iWidth, iHeight);
		COGLImage* pImg = 0;

		int iTotalW, iTotalH;
		int iCurW, iCurH;
		m_refImage->GetSize(iCurW, iCurH);
		m_refImage->GetType(iImgType, iDataType, iBytesPerPixel);

		iTotalW = m_iTrgX + iWidth;
		iTotalH = m_iTrgY + iHeight;

		// If a target origin is set, or if target image is larger
		// than region to grab, then create a temporary
		// image into which to grab from the screen buffer.
		// Then flip this image and insert it into the
		// final target image m_refImage
		if ((m_iTrgX != 0) || (m_iTrgY != 0) || (iCurW > iTotalW) || (iCurH > iTotalH))
		{
			pImg = new COGLImage();
		}

		if (pImg)
		{
			if (iTotalW < iCurW)
			{
				iTotalW = iCurW;
			}

			if (iTotalH < iCurH)
			{
				iTotalH = iCurH;
			}

			pImg->ConvertType(iImgType, iDataType);
			pImg->ResizeCanvas(iWidth, iHeight);

			if ((iCurW < iTotalW) || (iCurH < iTotalH))
			{
				m_refImage->ResizeCanvas(iTotalW, iTotalH, 0, 0.0, 0.0);
			}
		}
		else
		{
			m_refImage->ResizeCanvas(iWidth, iHeight, 0, 0.0, 0.0);
		}

		// GL Types are equal to image types
		iGLImgType  = iImgType;
		iGLDataType = iDataType;

		bool bCaptureLuminance = (iGLImgType == GL_LUMINANCE);	//|| iGLImgType == GL_LUMINANCE_ALPHA );

		// Set Pixel color scales if luminance is desired
		if (bCaptureLuminance)
		{
			//glPixelTransferf( GL_RED_SCALE, 1.0f );
			//glPixelTransferf( GL_GREEN_SCALE, 0.0f );
			//glPixelTransferf( GL_BLUE_SCALE, 0.0f );

			//glPixelTransferf( GL_RED_BIAS, 0.0f );
			//glPixelTransferf( GL_GREEN_BIAS, 0.0f );
			//glPixelTransferf( GL_BLUE_BIAS, 0.0f );

			iGLImgType = GL_RED;
		}

		glReadBuffer(GL_BACK);

		glPixelStorei(GL_PACK_ROW_LENGTH, 0);
		glPixelStorei(GL_PACK_SKIP_ROWS, 0);
		glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);

		::LockImageAccess();
		if (pImg)
		{
			glReadPixels(iX, iY, iWidth, iHeight, iGLImgType, iGLDataType, pImg->GetDataPtr());
		}
		else
		{
			glReadPixels(iX, iY, iWidth, iHeight, iGLImgType, iGLDataType, m_refImage->GetDataPtr());
		}
		::UnlockImageAccess();

		//if ( bCaptureLuminance )
		//{
		//	//glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
		//	//glPixelTransferf( GL_RED_SCALE, 1.0f );
		//	//glPixelTransferf( GL_GREEN_SCALE, 1.0f );
		//	//glPixelTransferf( GL_BLUE_SCALE, 1.0f );
		//}

		if (pImg)
		{
			pImg->FlipImage(0);
			m_refImage->InsertImage(*pImg, m_iTrgX, m_iTrgY);
			delete pImg;
		}
		else
		{
			// Flip image vertically since origin of images is top left.
			m_refImage->FlipImage(0);
		}
	}
	else if ((m_eTarget == READ_TO_TEXTURE) &&
		 m_refTexture.IsValid())
	{
		COGLTexture* pTexture = dynamic_cast< COGLTexture* >((COGLBaseElement*) m_refTexture);
		if (!pTexture)
		{
			return false;
		}

		uint uTexID = pTexture->GetTextureID();
		if (uTexID == 0)
		{
			return true;
		}

		int iInternalFormat;

		uint uTexUnit;
		uint uOldTexUnit, uOldTexID;

		if (glActiveTexture)
		{
			uTexUnit = pTexture->GetTextureUnit();
			glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*) &uOldTexUnit);
			glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*) &uOldTexID);

			glActiveTexture(GL_TEXTURE0 + uTexUnit);
		}
		else
		{
			glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*) &uOldTexID);
		}

		glBindTexture(GL_TEXTURE_2D, uTexID);

		GetWinArea(iX, iY, iWidth, iHeight);

		glReadBuffer(GL_BACK);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &iInternalFormat);

		glCopyTexImage2D(GL_TEXTURE_2D, 0, iInternalFormat, iX, iY, iWidth, iHeight, 0);

		if (glActiveTexture)
		{
			glActiveTexture(uOldTexUnit);
		}

		glBindTexture(GL_TEXTURE_2D, uOldTexID);
	}
	else
	{
		return false;
	}

	return true;
}
