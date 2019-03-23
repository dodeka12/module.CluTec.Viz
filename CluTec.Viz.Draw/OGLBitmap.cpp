////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLBitmap.cpp
//
// summary:   Implements the ogl bitmap class
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
#include "OGLBitmap.h"
#include "OGLBaseElementList.h"
#include "CluTec.Viz.OpenGL.Extensions\Extensions.h"
#include "CluTec.Viz.OpenGL/Api.h"

COGLBitmap::COGLBitmap(void)
{
	m_sTypeName = "Bitmap";

	ResetVars();
}

COGLBitmap::COGLBitmap(const COGLBitmap& oglBitmap)
{
	m_sTypeName = "Bitmap";

	*this = oglBitmap;
}

COGLBitmap::~COGLBitmap(void)
{
}

void COGLBitmap::ResetVars()
{
	m_fVAlign = 0.0f;
	m_fHAlign = 0.0f;
	m_fScale  = 1.0f;

//	m_bInvert = false;
//	m_bUseTransparency = false;
//  m_bScaleWithCurColor = false;
//	m_iTransMode = Exact;

	m_fX = 0.0f;
	m_fY = 0.0f;
	m_fZ = 0.0f;
}

COGLBitmap& COGLBitmap::operator=(const COGLBitmap& oglBitmap)
{
	m_fVAlign = oglBitmap.m_fVAlign;
	m_fHAlign = oglBitmap.m_fHAlign;
	m_fScale  = oglBitmap.m_fScale;

/*
        m_bInvert = oglBitmap.m_bInvert;
        m_bUseTransparency = oglBitmap.m_bUseTransparency;
        m_bScaleWithCurColor = oglBitmap.m_bScaleWithCurColor;
        m_iTransMode = oglBitmap.m_iTransMode;
        m_rgbaTransPix = oglBitmap.m_rgbaTransPix;
*/

	m_fX = oglBitmap.m_fX;
	m_fY = oglBitmap.m_fY;
	m_fZ = oglBitmap.m_fZ;

	return *this;
}

/////////////////////////////////////////////////////////////////////
/// Set Position

void COGLBitmap::SetPosition(float fX, float fY, float fZ)
{
	m_fX = fX;
	m_fY = fY;
	m_fZ = fZ;

	TellParentContentChanged();
}

/////////////////////////////////////////////////////////////////////
/// Set Position

void COGLBitmap::SetPosition(COGLVertex& rVex)
{
	m_fX = rVex[0];
	m_fY = rVex[1];
	m_fZ = rVex[2];

	TellParentContentChanged();
}

//////////////////////////////////////////////////////////////////////
/// Tell parents that content has changed

void COGLBitmap::TellParentContentChanged()
{
	list<COGLBaseElement*>::iterator itEl;

	for (itEl = m_listParent.begin();
	     itEl != m_listParent.end();
	     ++itEl)
	{
		COGLBaseElementList* pList = dynamic_cast<COGLBaseElementList*>((COGLBaseElement*) (*itEl));
		if (pList)
		{
			pList->SetContentChanged(true, true, false);
		}
	}
}

/////////////////////////////////////////////////////////////////////
/// Apply Text
// iMode alows different types of Apply calls.
// Arbitrary data may be passed by pvData.

bool COGLBitmap::Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData)
{
	//float pfCol[4];
	GLboolean bValid;
	int iImgType, iDataType, iBytesPerPixel;
	int iGLImgType, iGLDataType;
	int iWidth, iHeight, iOrigX, iOrigY;
	int iOffX, iOffY;
	int piViewport[4];
	double pdModelView[16];
	double pdProjection[16];
	double dWinX, dWinY, dWinZ;
	double dOffX, dOffY, dHeight, dWidth;
	double dX, dY, dZ;

	float fScaleX, fScaleY;
	double dScaleX, dScaleY;// = double( m_fScale );

	if (!m_Image.IsValid())
	{
		return false;
	}

	m_Image->GetSize(iWidth, iHeight);
	if ((iWidth == 0) || (iHeight == 0))
	{
		return true;
	}

	m_Image->GetType(iImgType, iDataType, iBytesPerPixel);

	glGetIntegerv(GL_VIEWPORT, piViewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, pdModelView);
	glGetDoublev(GL_PROJECTION_MATRIX, pdProjection);

	glGetFloatv(GL_ZOOM_X, &fScaleX);
	glGetFloatv(GL_ZOOM_Y, &fScaleY);
	dScaleX = double(fScaleX);
	dScaleY = double(fScaleY);

	// Transform raster position to window coordinates
	gluProject(double(m_fX), double(m_fY), double(m_fZ),
			pdModelView, pdProjection, piViewport,
			&dWinX, &dWinY, &dWinZ);

	// Use glBitmap only to adapt raster position.
	m_Image->GetOrigin(iOrigX, iOrigY);

	if ((m_fVAlign != 1.0f) || (m_fHAlign != 0.0f) || (iOrigX != 0) || (iOrigY != 0))
	{
		dWinY += (double(iHeight - iOrigY) * double(1.0f - m_fVAlign) /*+ double(iOrigY)*/) * dScaleY;
		dWinX += -(double(iWidth - iOrigX)  * double(m_fHAlign)	/*+ double(iOrigX)*/) * dScaleX;
//		glBitmap(0, 0, 0, 0, fXMove, fYMove, 0);
	}

	dHeight = double(iHeight) * Mag(dScaleY);
	dWidth  = double(iWidth)  * Mag(dScaleX);

	iOffX  = iOffY = 0;
	bValid = true;

	if ((dOffX = dWinX - double(piViewport[0])) < 0)
	{
		if (dWidth >= -dOffX)
		{
			iOffX = int(floor(-dOffX / dScaleX + 0.5));
			dWinX = double(piViewport[0]) + 1;
		}
		else
		{
			bValid = false;
		}
	}
	else if (dWinX > double(piViewport[0] + piViewport[2] - 1))
	{
		bValid = false;
	}

	//if ( (dOffY = dWinY - double(piViewport[1])) < 0 )
	if ((dOffY = double(piViewport[1] + piViewport[3] - 1) - dWinY) < 0)
	{
		if (dHeight >= -dOffY)
		{
			iOffY = int(floor(-dOffY / dScaleY + 0.5));
			//dWinY = double( piViewport[1] ) + 1;
			dWinY = double(piViewport[1] + piViewport[3] - 2);
		}
		else
		{
			bValid = false;
		}
	}
	//else if ( dWinY > double(piViewport[1] + piViewport[3] - 1) )
	else if (dWinY < double(piViewport[1]))
	{
		bValid = false;
	}

	// Check wether raster position can be seen
	if (!bValid)
	{
		// If not, don't draw bitmap.
		return true;
	}

	//if ( glWindowPos3d )
	//{
	//	// There does not seem to be any clipping if dWinZ is outside the range [0,1].
	//	// Enforce this here.
	//	if ( dWinZ > 1 || dWinZ < 0 )
	//		return true;

	//	glWindowPos3d( floor(dWinX), floor(dWinY) /*+ double(iHeight - iOffY)*/, dWinZ );
	//}
	//else
	//{
	gluUnProject(floor(dWinX + 0.5), floor(dWinY + 0.5) /*+ double(iHeight - iOffY)*/, dWinZ,
			pdModelView, pdProjection, piViewport,
			&dX, &dY, &dZ);

	glRasterPos3d(dX, dY, dZ);
	//}

	glPixelStorei(GL_UNPACK_ROW_LENGTH, iWidth);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, iOffY);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, iOffX);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// GL and IL defines are identical
	iGLImgType  = iImgType;
	iGLDataType = iDataType;
/*
        switch( iImgType )
        {
        case IL_RGB:
                iGLImgType = GL_RGB; break;
        case IL_RGBA:
                iGLImgType = GL_RGBA; break;
        }

        switch( iDataType )
        {
        case IL_BYTE:
                iGLDataType = GL_BYTE; break;
        case IL_UNSIGNED_BYTE:
                iGLDataType = GL_UNSIGNED_BYTE; break;
        }
*/

	if (eMode == COGLBaseElement::PICK)
	{
		//GLint piBox[4];
		uint uPickColor = 0;

		// Push Object ID onto name stack
		rData.PushPickName(GetUID());

		// Now store current pick name stack in list
		uPickColor = rData.ConvertNameToColor(rData.StorePickNameStack(), 0);

		float fR, fG, fB, fA;

		fR = float(uPickColor & 0x000000FF) / 255.0f;
		fG = float((uPickColor >> 8) & 0x000000FF) / 255.0f;
		fB = float((uPickColor >> 16) & 0x000000FF) / 255.0f;
		fA = float((uPickColor >> 24) & 0x000000FF) / 255.0f;

		// Do not draw actual image but our Pick Id Color
		glPixelTransferf(GL_RED_SCALE, 0.0f);
		glPixelTransferf(GL_GREEN_SCALE, 0.0f);
		glPixelTransferf(GL_BLUE_SCALE, 0.0f);
		glPixelTransferf(GL_ALPHA_SCALE, 0.0f);

		glPixelTransferf(GL_RED_BIAS, fR);
		glPixelTransferf(GL_GREEN_BIAS, fG);
		glPixelTransferf(GL_BLUE_BIAS, fB);
		glPixelTransferf(GL_ALPHA_BIAS, fA);
	}

	if ((iImgType > 0) && (iDataType > 0))
	{
		// Since images are stored with origin at top left,
		// need to flip image here
		glPixelZoom(GLfloat(dScaleX), -GLfloat(dScaleY));

		::LockImageAccess();
		glDrawPixels(iWidth - iOffX, iHeight - iOffY,
				iGLImgType, iGLDataType,
				(GLvoid*) m_Image->GetDataPtr());
		::UnlockImageAccess();

		glPixelZoom(GLfloat(dScaleX), GLfloat(dScaleY));
	}

	if (eMode == COGLBaseElement::PICK)
	{
		rData.PopPickName();

		glPixelTransferf(GL_RED_SCALE, 1.0f);
		glPixelTransferf(GL_GREEN_SCALE, 1.0f);
		glPixelTransferf(GL_BLUE_SCALE, 1.0f);
		glPixelTransferf(GL_ALPHA_SCALE, 1.0f);

		glPixelTransferf(GL_RED_BIAS, 0.0f);
		glPixelTransferf(GL_GREEN_BIAS, 0.0f);
		glPixelTransferf(GL_BLUE_BIAS, 0.0f);
		glPixelTransferf(GL_ALPHA_BIAS, 0.0f);
	}

	//glPixelZoom(1, 1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

	return true;
}

/////////////////////////////////////////////////////////////////////
/// Apply as Texture
/*
bool COGLBitmap::ApplyAsTexture(bool bGenMipMaps)
{
        if (m_bIsTexture)
        {
                glBindTexture(GL_TEXTURE_2D, m_uTexID);

                if (bGenMipMaps)
                {
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                }
                else
                {
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                }
        }
        else
        {
                float fPriority = 1.0f;

                ilBindImage(m_uImgID);

//		m_uTexID = ilutGLBindMipmaps();

                if (bGenMipMaps)
                {
                        m_uTexID = ilutGLBindMipmaps();
                        glPrioritizeTextures(1, &m_uTexID, &fPriority);
                }
                else
                {
*/
/*
                        GLuint	TexID = 0;

                        //	glPushAttrib(GL_ALL_ATTRIB_BITS);

                        glGenTextures(1, &TexID);
                        glBindTexture(GL_TEXTURE_2D, TexID);
*/
/*
                        m_uTexID = ilutGLBindMipmaps();
                        glPrioritizeTextures(1, &m_uTexID, &fPriority);

                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_LINEAR_MIPMAP_LINEAR);

//			if (!ilutGLBuildMipmaps())
//			if (!ilutGLTexImage(0)) //(!ilutGLBuildMipmaps())
//			{
//				glDeleteTextures(1, &TexID);
//				m_uTexID = 0;
//			}
                }

                if (m_uTexID == 0)
                {
                        m_bIsTexture = false;
                        return false;
                }

                m_bIsTexture = true;
        }

        return true;
}
*/

/////////////////////////////////////////////////////////////////////
/// If image is also texture, then delete the texture
/*
bool COGLBitmap::DeleteTexture()
{
        if (m_bIsTexture)
        {
                glDeleteTextures(1, &m_uTexID);
                m_bIsTexture = false;
                m_uTexID = 0;
        }

        return true;
}
*/

/////////////////////////////////////////////////////////////////////
/// Get World Coordinate Dimensions
/*
bool COGLBitmap::GetWorldCoordDims(float &fLeft, float &fRight, float &fBottom, float &fTop,
                                                                   float &fZ1, float &fZ2)
{

        double pdPos[4], pdObj[3];
        double pdMV[16], pdPM[16];
        int piVP[4];

        glGetDoublev(GL_MODELVIEW_MATRIX, pdMV);
        glGetDoublev(GL_PROJECTION_MATRIX, pdPM);
        glGetIntegerv(GL_VIEWPORT, piVP);

        glRasterPos3f(m_fX, m_fY, m_fZ);

        if (m_fVAlign != 0.0f || m_fHAlign != 0.0f)
        {
                float fYMove = -float(m_iHeight) * m_fVAlign * m_fScale;
                float fXMove = -float(m_iWidth) * m_fHAlign * m_fScale;
                glBitmap(0, 0, 0, 0, fXMove, fYMove, 0);
                glGetDoublev(GL_CURRENT_RASTER_POSITION, pdPos);

                gluUnProject(pdPos[0], pdPos[1], pdPos[2], pdMV, pdPM, piVP,
                        &pdObj[0], &pdObj[1], &pdObj[2]);

                fLeft = float(pdObj[0]);
                fBottom = float(pdObj[1]);
                fZ1 = float(pdObj[2]);
        }
        else
        {
                fLeft = m_fX;
                fBottom = m_fY;
                fZ1 = m_fZ;
        }

        glBitmap(0, 0, 0, 0, float(m_iWidth)*m_fScale, float(m_iHeight)*m_fScale, 0);
        glGetDoublev(GL_CURRENT_RASTER_POSITION, pdPos);

        gluUnProject(pdPos[0], pdPos[1], pdPos[2], pdMV, pdPM, piVP,
                                 &pdObj[0], &pdObj[1], &pdObj[2]);

        fRight = float(pdObj[0]);
        fTop = float(pdObj[1]);
        fZ2 = float(pdObj[2]);

        return true;
}
*/
