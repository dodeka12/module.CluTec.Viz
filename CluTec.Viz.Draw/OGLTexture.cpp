////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLTexture.cpp
//
// summary:   Implements the ogl texture class
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

#include "CluTec.Base/Exception.h"
#include "CluTec.Base/Conversion.h"

#include "CluTec.Viz.OpenGL.Extensions\Extensions.h"
#include "CluTec.Viz.OpenGL/Api.h"


#include "OGLTexture.h"
#include "OGLBaseElementList.h"



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
COGLTexture::COGLTexture(ETextureType eTexType)
{
	m_sTypeName = "Texture";
	m_eTexType  = eTexType;

	CLU_OGL_CALL(glGetIntegerv(GL_MAX_TEXTURE_UNITS, (GLint*) &m_uTexUnitCnt));
	if (m_uTexUnitCnt > OGL_MAX_TEX_UNITS)
	{
		m_uTexUnitCnt = OGL_MAX_TEX_UNITS;
	}

	_ResetVars();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
COGLTexture::COGLTexture(const COGLTexture& oglTexture)
{
	m_sTypeName = "Texture";

	*this = oglTexture;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
COGLTexture::~COGLTexture()
{
	DeleteTexture();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COGLTexture::_ResetVars()
{
	m_fPriority       = 1.0f;
	m_eWrapType       = EWrapType::CLAMP;
	m_eBlendTypeColor = EBlendType::MODULATE;
	m_eBlendTypeAlpha = EBlendType::ADD;
	m_bMipMap         = false;
	m_uTexID          = 0;
	m_uTexUnit        = 0;

	m_bNorm            = true;
	m_bInterpolate     = true;
	m_bExternalTexture = false;

	m_iInternalPixelFormat = CLUVIZ_IMG_RGB;
	m_iInternalDataFormat  = CLUVIZ_IMG_UNSIGNED_BYTE;

	m_BorderColor.Set(0.0f, 0.0f, 0.0f, 0.5f);

	m_iWidth  = 0;
	m_iHeight = 0;
	m_iDepth  = 0;

	m_bEnableForPicking = false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
COGLTexture& COGLTexture::operator=(const COGLTexture& rTexture)
{
	m_eTexType        = rTexture.m_eTexType;
	m_eWrapType       = rTexture.m_eWrapType;
	m_eBlendTypeColor = rTexture.m_eBlendTypeColor;
	m_eBlendTypeAlpha = rTexture.m_eBlendTypeAlpha;

	// Dangerous!!!
	// Should have an ID repository
	m_uTexID      = rTexture.m_uTexID;
	m_uTexUnit    = rTexture.m_uTexUnit;
	m_uTexUnitCnt = rTexture.m_uTexUnitCnt;

	m_bInterpolate = rTexture.m_bInterpolate;

	_TellParentContentChanged();

	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COGLTexture::SetPriority(float fPriority)
{
	m_fPriority = fPriority;
	CLU_OGL_CALL(glPrioritizeTextures(1, &m_uTexID, &m_fPriority));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COGLTexture::SetTextureUnit(uint uTexUnit)
{
	// Should check what texture units are available
	if (uTexUnit >= m_uTexUnitCnt)
	{
		return false;
	}

	m_uTexUnit = uTexUnit;
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COGLTexture::GenTexture(float fPriority, uint uTexUnit)
{
	m_bMipMap  = false;
	m_uTexUnit = uTexUnit;

	if (glActiveTexture)
	{
		CLU_OGL_CALL(glActiveTexture(GL_TEXTURE0 + m_uTexUnit));
	}

	CLU_OGL_CALL(glGenTextures(1, &m_uTexID));

	if (m_uTexID)
	{
		SetPriority(fPriority);
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COGLTexture::DeleteTexture()
{
	if (m_uTexID == 0)
	{
		return;
	}

	if (!m_bExternalTexture)
	{
		if (glActiveTexture)
		{
			glActiveTexture(GL_TEXTURE0 + m_uTexUnit);
		}

		if (glIsTexture(m_uTexID) == GL_TRUE)
		{
			Clu::OpenGL::DeleteTextures(1, &m_uTexID);
		}
	}

	m_iWidth               = 0;
	m_iHeight              = 0;
	m_iDepth               = 0;
	m_bInterpolate         = true;
	m_bMipMap              = false;
	m_iInternalPixelFormat = CLUVIZ_IMG_RGB;
	m_iInternalDataFormat  = CLUVIZ_IMG_UNSIGNED_BYTE;
	m_bNorm                = true;
	m_bExternalTexture     = false;
	m_uTexID               = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COGLTexture::GetTextureSize(int& iWidth, int& iHeight)
{
	if (m_eTexType != ETextureType::TEX_2D)
	{
		return false;
	}

	iWidth  = m_iWidth;
	iHeight = m_iHeight;
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COGLTexture::GetTextureSize(int& iWidth, int& iHeight, int& iDepth)
{
	if (m_eTexType != ETextureType::TEX_3D)
	{
		return false;
	}

	iWidth  = m_iWidth;
	iHeight = m_iHeight;
	iDepth  = m_iDepth;
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COGLTexture::_TryDecomposeTextureImageFormat(int& iImgType, int& iDataType, bool& bNorm, int& iInternalFormat)
{
	switch (iInternalFormat)
	{
	//////////////////////////////////////////////////////////
	// Base Types with no BitDepth information
	// We Assume unsigned char bitdepth for these types
	// All base types are normalized and unsigned (https://www.opengl.org/wiki/Image_Format)
	//////////////////////////////////////////////////////////

	case GL_RGB:
		iImgType  = CLUVIZ_IMG_RGB;
		iDataType = CLUVIZ_IMG_UNSIGNED_BYTE;
		bNorm     = true;
		break;

	case GL_RGBA:
		iImgType  = CLUVIZ_IMG_RGBA;
		iDataType = CLUVIZ_IMG_UNSIGNED_BYTE;
		bNorm     = true;
		break;

	case GL_RG:
		iImgType  = CLUVIZ_IMG_LUMINANCE_ALPHA;
		iDataType = CLUVIZ_IMG_UNSIGNED_BYTE;
		bNorm     = true;
		break;

	case GL_R:
		iImgType  = CLUVIZ_IMG_LUMINANCE;
		iDataType = CLUVIZ_IMG_UNSIGNED_BYTE;
		bNorm     = true;
		break;

	case GL_LUMINANCE:
		iImgType  = CLUVIZ_IMG_LUMINANCE;
		iDataType = CLUVIZ_IMG_UNSIGNED_BYTE;
		bNorm     = true;
		break;

	//////////////////////////////////////////////////////////
	// RGB
	//////////////////////////////////////////////////////////

	case GL_RGB8:
		iImgType  = CLUVIZ_IMG_RGB;
		iDataType = CLUVIZ_IMG_BYTE;
		bNorm     = true;
		break;

	case GL_RGB8I:
		iImgType  = CLUVIZ_IMG_RGB;
		iDataType = CLUVIZ_IMG_BYTE;
		bNorm     = false;
		break;

	case GL_RGB8UI:
		iImgType  = CLUVIZ_IMG_RGB;
		iDataType = CLUVIZ_IMG_UNSIGNED_BYTE;
		bNorm     = false;
		break;

	case GL_RGB16:
		iImgType  = CLUVIZ_IMG_RGB;
		iDataType = CLUVIZ_IMG_SHORT;
		bNorm     = true;
		break;

	case GL_RGB16I:
		iImgType  = CLUVIZ_IMG_RGB;
		iDataType = CLUVIZ_IMG_SHORT;
		bNorm     = false;
		break;

	case GL_RGB16UI:
		iImgType  = CLUVIZ_IMG_RGB;
		iDataType = CLUVIZ_IMG_UNSIGNED_SHORT;
		bNorm     = false;
		break;

	case GL_RGB32I:
		iImgType  = CLUVIZ_IMG_RGB;
		iDataType = CLUVIZ_IMG_INT;
		bNorm     = false;
		break;

	case GL_RGB32UI:
		iImgType  = CLUVIZ_IMG_RGB;
		iDataType = CLUVIZ_IMG_UNSIGNED_INT;
		bNorm     = false;
		break;

	case GL_RGB32F:
		iImgType  = CLUVIZ_IMG_RGB;
		iDataType = CLUVIZ_IMG_FLOAT;
		bNorm     = false;
		break;

	//////////////////////////////////////////////////////////
	// RGBA
	//////////////////////////////////////////////////////////

	case GL_RGBA8:
		iImgType  = CLUVIZ_IMG_RGBA;
		iDataType = CLUVIZ_IMG_BYTE;
		bNorm     = true;
		break;

	case GL_RGBA8I:
		iImgType  = CLUVIZ_IMG_RGBA;
		iDataType = CLUVIZ_IMG_BYTE;
		bNorm     = false;
		break;

	case GL_RGBA8UI:
		iImgType  = CLUVIZ_IMG_RGBA;
		iDataType = CLUVIZ_IMG_UNSIGNED_BYTE;
		bNorm     = false;
		break;

	case GL_RGBA16:
		iImgType  = CLUVIZ_IMG_RGBA;
		iDataType = CLUVIZ_IMG_SHORT;
		bNorm     = true;
		break;

	case GL_RGBA16I:
		iImgType  = CLUVIZ_IMG_RGBA;
		iDataType = CLUVIZ_IMG_SHORT;
		bNorm     = false;
		break;

	case GL_RGBA16UI:
		iImgType  = CLUVIZ_IMG_RGBA;
		iDataType = CLUVIZ_IMG_UNSIGNED_SHORT;
		bNorm     = false;
		break;

	case GL_RGBA32I:
		iImgType  = CLUVIZ_IMG_RGBA;
		iDataType = CLUVIZ_IMG_INT;
		bNorm     = false;
		break;

	case GL_RGBA32UI:
		iImgType  = CLUVIZ_IMG_RGBA;
		iDataType = CLUVIZ_IMG_UNSIGNED_INT;
		bNorm     = false;
		break;

	case GL_RGBA32F:
		iImgType  = CLUVIZ_IMG_RGBA;
		iDataType = CLUVIZ_IMG_FLOAT;
		bNorm     = false;
		break;

	//////////////////////////////////////////////////////////
	// Luminance
	//////////////////////////////////////////////////////////

	case GL_LUMINANCE8:
		iImgType  = CLUVIZ_IMG_LUMINANCE;
		iDataType = CLUVIZ_IMG_BYTE;
		bNorm     = true;
		break;

	case GL_R8I:
		iImgType  = CLUVIZ_IMG_LUMINANCE;
		iDataType = CLUVIZ_IMG_BYTE;
		bNorm     = false;
		break;

	case GL_R8UI:
		iImgType  = CLUVIZ_IMG_LUMINANCE;
		iDataType = CLUVIZ_IMG_UNSIGNED_BYTE;
		bNorm     = false;
		break;

	case GL_LUMINANCE16:
		iImgType  = CLUVIZ_IMG_LUMINANCE;
		iDataType = CLUVIZ_IMG_SHORT;
		bNorm     = true;
		break;

	case GL_R16I:
		iImgType  = CLUVIZ_IMG_LUMINANCE;
		iDataType = CLUVIZ_IMG_SHORT;
		bNorm     = false;
		break;

	case GL_R16UI:
		iImgType  = CLUVIZ_IMG_LUMINANCE;
		iDataType = CLUVIZ_IMG_UNSIGNED_SHORT;
		bNorm     = false;
		break;

	case GL_R32I:
		iImgType  = CLUVIZ_IMG_LUMINANCE;
		iDataType = CLUVIZ_IMG_INT;
		bNorm     = false;
		break;

	case GL_R32UI:
		iImgType  = CLUVIZ_IMG_LUMINANCE;
		iDataType = CLUVIZ_IMG_UNSIGNED_INT;
		bNorm     = false;
		break;

	case GL_R32F:
		iImgType  = CLUVIZ_IMG_LUMINANCE;
		iDataType = CLUVIZ_IMG_FLOAT;
		bNorm     = false;
		break;

	//////////////////////////////////////////////////////////
	// Luminance Alpha
	//////////////////////////////////////////////////////////

	case GL_LUMINANCE8_ALPHA8:
		iImgType  = CLUVIZ_IMG_LUMINANCE_ALPHA;
		iDataType = CLUVIZ_IMG_BYTE;
		bNorm     = true;
		break;

	case GL_RG8I:
		iImgType  = CLUVIZ_IMG_LUMINANCE_ALPHA;
		iDataType = CLUVIZ_IMG_BYTE;
		bNorm     = false;
		break;

	case GL_RG8UI:
		iImgType  = CLUVIZ_IMG_LUMINANCE_ALPHA;
		iDataType = CLUVIZ_IMG_UNSIGNED_BYTE;
		bNorm     = false;
		break;

	case GL_LUMINANCE16_ALPHA16:
		iImgType  = CLUVIZ_IMG_LUMINANCE_ALPHA;
		iDataType = CLUVIZ_IMG_SHORT;
		bNorm     = true;
		break;

	case GL_RG16I:
		iImgType  = CLUVIZ_IMG_LUMINANCE_ALPHA;
		iDataType = CLUVIZ_IMG_SHORT;
		bNorm     = false;
		break;

	case GL_RG16UI:
		iImgType  = CLUVIZ_IMG_LUMINANCE_ALPHA;
		iDataType = CLUVIZ_IMG_UNSIGNED_SHORT;
		bNorm     = false;
		break;

	case GL_RG32I:
		iImgType  = CLUVIZ_IMG_LUMINANCE_ALPHA;
		iDataType = CLUVIZ_IMG_INT;
		bNorm     = false;
		break;

	case GL_RG32UI:
		iImgType  = CLUVIZ_IMG_LUMINANCE_ALPHA;
		iDataType = CLUVIZ_IMG_UNSIGNED_INT;
		bNorm     = false;
		break;

	case GL_RG32F:
		iImgType  = CLUVIZ_IMG_LUMINANCE_ALPHA;
		iDataType = CLUVIZ_IMG_FLOAT;
		bNorm     = false;
		break;

	default:
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* COGLTexture::InternalFormatToString(int iInternalFormat)
{
	switch (iInternalFormat)
	{
	/************************************************************************/
	/* Table 1. Unsized Internal Formats                                    */
	/************************************************************************/
	case GL_RGB: return "GL_RGB";
	case GL_RGBA: return "GL_RGBA";
	case GL_LUMINANCE_ALPHA: return "GL_LUMINANCE_ALPHA";
	case GL_LUMINANCE: return "GL_LUMINANCE";
	case GL_ALPHA: return "GL_ALPHA";

	/************************************************************************/
	/* Table 2. Sized Internal Formats                                      */
	/************************************************************************/
	case GL_R8: return "GL_R8";
	case GL_R8_SNORM: return "GL_R8_SNORM";
	case GL_R16F: return "GL_R16F";
	case GL_R32F: return "GL_R32F";
	case GL_R8UI: return "GL_R8UI";
	case GL_R8I: return "GL_R8I";
	case GL_R16UI: return "GL_R16UI";
	case GL_R16I: return "GL_R16I";
	case GL_R32UI: return "GL_R32UI";
	case GL_R32I: return "GL_R32I";
	case GL_RG8: return "GL_RG8";
	case GL_RG8_SNORM: return "GL_RG8_SNORM";
	case GL_RG16F: return "GL_RG16F";
	case GL_RG32F: return "GL_RG32F";
	case GL_RG8UI: return "GL_RG8UI";
	case GL_RG8I: return "GL_RG8I";
	case GL_RG16UI: return "GL_RG16UI";
	case GL_RG16I: return "GL_RG16I";
	case GL_RG32UI: return "GL_RG32UI";
	case GL_RG32I: return "GL_RG32I";
	case GL_RGB8: return "GL_RGB8";
	case GL_SRGB8: return "GL_SRGB8";
	case GL_RGB565: return "GL_RGB565";
	case GL_RGB8_SNORM: return "GL_RGB8_SNORM";
	case GL_R11F_G11F_B10F: return "GL_R11F_G11F_B10F";
	case GL_RGB9_E5: return "GL_RGB9_E5";
	case GL_RGB16F: return "GL_RGB16F";
	case GL_RGB32F: return "GL_RGB32F";
	case GL_RGB8UI: return "GL_RGB8UI";
	case GL_RGB8I: return "GL_RGB8I";
	case GL_RGB16UI: return "GL_RGB16UI";
	case GL_RGB16I: return "GL_RGB16I";
	case GL_RGB32UI: return "GL_RGB32UI";
	case GL_RGB32I: return "GL_RGB32I";
	case GL_RGBA8: return "GL_RGBA8";
	case GL_SRGB8_ALPHA8: return "GL_SRGB8_ALPHA8";
	case GL_RGBA8_SNORM: return "GL_RGBA8_SNORM";
	case GL_RGB5_A1: return "GL_RGB5_A1";
	case GL_RGBA4: return "GL_RGBA4";
	case GL_RGB10_A2: return "GL_RGB10_A2";
	case GL_RGBA16F: return "GL_RGBA16F";
	case GL_RGBA32F: return "GL_RGBA32F";
	case GL_RGBA8UI: return "GL_RGBA8UI";
	case GL_RGBA8I: return "GL_RGBA8I";
	case GL_RGB10_A2UI: return "GL_RGB10_A2UI";
	case GL_RGBA16UI: return "GL_RGBA16UI";
	case GL_RGBA16I: return "GL_RGBA16I";
	case GL_RGBA32I: return "GL_RGBA32I";
	case GL_RGBA32UI: return "GL_RGBA32UI";
	case GL_DEPTH_COMPONENT16: return "GL_DEPTH_COMPONENT16";
	case GL_DEPTH_COMPONENT24: return "GL_DEPTH_COMPONENT24";
	case GL_DEPTH_COMPONENT32F: return "GL_DEPTH_COMPONENT32F";
	case GL_DEPTH24_STENCIL8: return "GL_DEPTH24_STENCIL8";
	case GL_DEPTH32F_STENCIL8: return "GL_DEPTH32F_STENCIL8";
	default: return "NOT SUPPORTED";
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* COGLTexture::FormatToString(int iFormat)
{
	switch (iFormat)
	{
	case GL_RED: return "GL_RED";
	case GL_RED_INTEGER: return "GL_RED_INTEGER";
	case GL_RG: return "GL_RG";
	case GL_RG_INTEGER: return "GL_RG_INTEGER";
	case GL_RGB: return "GL_RGB";
	case GL_RGB_INTEGER: return "GL_RGB_INTEGER";
	case GL_RGBA: return "GL_RGBA";
	case GL_RGBA_INTEGER: return "GL_RGBA_INTEGER";
	case GL_DEPTH_COMPONENT: return "GL_DEPTH_COMPONENT";
	case GL_DEPTH_STENCIL: return "GL_DEPTH_STENCIL";
	case GL_LUMINANCE_ALPHA: return "GL_LUMINANCE_ALPHA";
	case GL_LUMINANCE: return "GL_LUMINANCE";
	case GL_ALPHA: return "GL_ALPHA";
	default: return "NOT SUPPORTED";
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* COGLTexture::TypeToString(int iType)
{
	switch (iType)
	{
	case GL_UNSIGNED_BYTE: return "GL_UNSIGNED_BYTE";
	case GL_BYTE: return "GL_BYTE";
	case GL_UNSIGNED_SHORT: return "GL_UNSIGNED_SHORT";
	case GL_SHORT: return "GL_SHORT";
	case GL_UNSIGNED_INT: return "GL_UNSIGNED_INT";
	case GL_INT: return "GL_INT";
	case GL_HALF_FLOAT: return "GL_HALF_FLOAT";
	case GL_FLOAT: return "GL_FLOAT";
	case GL_UNSIGNED_SHORT_5_6_5: return "GL_UNSIGNED_SHORT_5_6_5";
	case GL_UNSIGNED_SHORT_4_4_4_4: return "GL_UNSIGNED_SHORT_4_4_4_4";
	case GL_UNSIGNED_SHORT_5_5_5_1: return "GL_UNSIGNED_SHORT_5_5_5_1";
	case GL_UNSIGNED_INT_2_10_10_10_REV: return "GL_UNSIGNED_INT_2_10_10_10_REV";
	case GL_UNSIGNED_INT_10F_11F_11F_REV: return "GL_UNSIGNED_INT_10F_11F_11F_REV";
	case GL_UNSIGNED_INT_5_9_9_9_REV: return "GL_UNSIGNED_INT_5_9_9_9_REV";
	case GL_UNSIGNED_INT_24_8: return "GL_UNSIGNED_INT_24_8";
	case GL_FLOAT_32_UNSIGNED_INT_24_8_REV: return "GL_FLOAT_32_UNSIGNED_INT_24_8_REV";
	default: return "NOT SUPPORTED";
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COGLTexture::_TryEvalTextureImageFormat(int& iInternalFormat, int& iTexImgType, int iImgType, int iDataType, bool bNorm)
{
	if (((iImgType == CLUVIZ_IMG_RGB) || (iImgType == CLUVIZ_IMG_BGR)))
	{
		if (iDataType == CLUVIZ_IMG_BYTE)
		{
			iInternalFormat = (bNorm ? GL_RGB8 : GL_RGB8I);
		}
		else if (iDataType == CLUVIZ_IMG_UNSIGNED_BYTE)
		{
			iInternalFormat = (bNorm ? GL_RGB8 : GL_RGB8UI);
		}
		else if (iDataType == CLUVIZ_IMG_SHORT)
		{
			iInternalFormat = (bNorm ? GL_RGB16 : GL_RGB16I);
		}
		else if (iDataType == CLUVIZ_IMG_UNSIGNED_SHORT)
		{
			iInternalFormat = (bNorm ? GL_RGB16 : GL_RGB16UI);
		}
		else if (iDataType == CLUVIZ_IMG_INT)
		{
			iInternalFormat = (bNorm ? GL_RGB16 : GL_RGB32I);
		}
		else if (iDataType == CLUVIZ_IMG_UNSIGNED_INT)
		{
			iInternalFormat = (bNorm ? GL_RGB16 : GL_RGB32UI);
		}
		else if (iDataType == CLUVIZ_IMG_FLOAT)
		{
			iInternalFormat = (bNorm ? GL_RGB16 : GL_RGB32F);
		}
		else if (iDataType == CLUVIZ_IMG_DOUBLE)
		{
			iInternalFormat = (bNorm ? GL_RGB16 : GL_RGB32F);
		}
		else
		{
			return false;
		}
	}
	else if ((iImgType == CLUVIZ_IMG_RGBA) || (iImgType == CLUVIZ_IMG_BGRA))
	{
		if (iDataType == CLUVIZ_IMG_BYTE)
		{
			iInternalFormat = (bNorm ? GL_RGBA8 : GL_RGBA8I);
		}
		else if (iDataType == CLUVIZ_IMG_UNSIGNED_BYTE)
		{
			iInternalFormat = (bNorm ? GL_RGBA8 : GL_RGBA8UI);
		}
		else if (iDataType == CLUVIZ_IMG_SHORT)
		{
			iInternalFormat = (bNorm ? GL_RGBA16 : GL_RGBA16I);
		}
		else if (iDataType == CLUVIZ_IMG_UNSIGNED_SHORT)
		{
			iInternalFormat = (bNorm ? GL_RGBA16 : GL_RGBA16UI);
		}
		else if (iDataType == CLUVIZ_IMG_INT)
		{
			iInternalFormat = (bNorm ? GL_RGBA16 : GL_RGBA32I);
		}
		else if (iDataType == CLUVIZ_IMG_UNSIGNED_INT)
		{
			iInternalFormat = (bNorm ? GL_RGBA16 : GL_RGBA32UI);
		}
		else if (iDataType == CLUVIZ_IMG_FLOAT)
		{
			iInternalFormat = (bNorm ? GL_RGBA16 : GL_RGBA32F);
		}
		else if (iDataType == CLUVIZ_IMG_DOUBLE)
		{
			iInternalFormat = (bNorm ? GL_RGBA16 : GL_RGBA32F);
		}
		else
		{
			return false;
		}
	}
	else if (iImgType == CLUVIZ_IMG_LUMINANCE)
	{
		if (iDataType == CLUVIZ_IMG_BYTE)
		{
			iInternalFormat = (bNorm ? GL_LUMINANCE8 : GL_R8I);
		}
		else if (iDataType == CLUVIZ_IMG_UNSIGNED_BYTE)
		{
			iInternalFormat = (bNorm ? GL_LUMINANCE8 : GL_R8UI);
		}
		else if (iDataType == CLUVIZ_IMG_SHORT)
		{
			iInternalFormat = (bNorm ? GL_LUMINANCE16 : GL_R16I);
		}
		else if (iDataType == CLUVIZ_IMG_UNSIGNED_SHORT)
		{
			iInternalFormat = (bNorm ? GL_LUMINANCE16 : GL_R16UI);
		}
		else if (iDataType == CLUVIZ_IMG_INT)
		{
			iInternalFormat = (bNorm ? GL_LUMINANCE16 : GL_R32I);
		}
		else if (iDataType == CLUVIZ_IMG_UNSIGNED_INT)
		{
			iInternalFormat = (bNorm ? GL_LUMINANCE16 : GL_R32UI);
		}
		else if (iDataType == CLUVIZ_IMG_FLOAT)
		{
			iInternalFormat = (bNorm ? GL_LUMINANCE16 : GL_R32F);
		}
		else if (iDataType == CLUVIZ_IMG_DOUBLE)
		{
			iInternalFormat = (bNorm ? GL_LUMINANCE16 : GL_R32F);
		}
		else
		{
			return false;
		}
	}
	else if (iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA)
	{
		if (iDataType == CLUVIZ_IMG_BYTE)
		{
			iInternalFormat = (bNorm ? GL_LUMINANCE8_ALPHA8 : GL_RG8I);
		}
		else if (iDataType == CLUVIZ_IMG_UNSIGNED_BYTE)
		{
			iInternalFormat = (bNorm ? GL_LUMINANCE8_ALPHA8 : GL_RG8UI);
		}
		else if (iDataType == CLUVIZ_IMG_SHORT)
		{
			iInternalFormat = (bNorm ? GL_LUMINANCE16_ALPHA16 : GL_RG16I);
		}
		else if (iDataType == CLUVIZ_IMG_UNSIGNED_SHORT)
		{
			iInternalFormat = (bNorm ? GL_LUMINANCE16_ALPHA16 : GL_RG16UI);
		}
		else if (iDataType == CLUVIZ_IMG_INT)
		{
			iInternalFormat = (bNorm ? GL_LUMINANCE16_ALPHA16 : GL_RG32I);
		}
		else if (iDataType == CLUVIZ_IMG_UNSIGNED_INT)
		{
			iInternalFormat = (bNorm ? GL_LUMINANCE16_ALPHA16 : GL_RG32UI);
		}
		else if (iDataType == CLUVIZ_IMG_FLOAT)
		{
			iInternalFormat = (bNorm ? GL_LUMINANCE16_ALPHA16 : GL_RG32F);
		}
		else if (iDataType == CLUVIZ_IMG_DOUBLE)
		{
			iInternalFormat = (bNorm ? GL_LUMINANCE16_ALPHA16 : GL_RG32F);
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	iTexImgType = iImgType;

	if (!bNorm)
	{
		if ((iDataType != CLUVIZ_IMG_FLOAT) && (iDataType != CLUVIZ_IMG_DOUBLE))
		{
			if (iImgType == CLUVIZ_IMG_RGB)
			{
				iTexImgType = GL_RGB_INTEGER;
			}
			else if (iImgType == CLUVIZ_IMG_BGR)
			{
				iTexImgType = GL_BGR_INTEGER;
			}
			else if (iImgType == CLUVIZ_IMG_RGBA)
			{
				iTexImgType = GL_RGBA_INTEGER;
			}
			else if (iImgType == CLUVIZ_IMG_BGRA)
			{
				iTexImgType = GL_BGRA_INTEGER;
			}
			else if (iImgType == CLUVIZ_IMG_LUMINANCE)
			{
				iTexImgType = GL_RED_INTEGER;
			}
		}
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COGLTexture::SetTexture3d(const CTensorData<double>& xTensor, uint uTexUnit, bool bGenMipMap, bool bNorm)
{
	int iTensorValence = xTensor.Valence();
	if (((iTensorValence != 3) && (iTensorValence != 4)) || (uTexUnit >= m_uTexUnitCnt))
	{
		return false;
	}

	int iInternalFormat;
	int iImgType, iDataType;
	int iWidth, iHeight, iDepth;

	m_eTexType = ETextureType::TEX_3D;
	m_uTexUnit = uTexUnit;
	m_bBorder  = false;

	if (glActiveTexture)
	{
		glActiveTexture(GL_TEXTURE0 + m_uTexUnit);
	}

	DeleteTexture();

	iDepth  = xTensor.DimSize(0);
	iHeight = xTensor.DimSize(1);
	iWidth  = xTensor.DimSize(2);

	iDataType = CLUVIZ_IMG_FLOAT;
	if (iTensorValence == 3)
	{
		iImgType = CLUVIZ_IMG_LUMINANCE;
	}
	else
	{
		int iColChanCnt = xTensor.DimSize(3);
		switch (iColChanCnt)
		{
		case 1:
			iImgType = CLUVIZ_IMG_LUMINANCE;
			break;

		case 2:
			iImgType = CLUVIZ_IMG_LUMINANCE_ALPHA;
			break;

		case 3:
			iImgType = CLUVIZ_IMG_RGB;
			break;

		case 4:
			iImgType = CLUVIZ_IMG_RGBA;
			break;

		default:
			return false;
		}
	}

	if (!_TryEvalTextureImageFormat(iInternalFormat, iImgType, iImgType, iDataType, bNorm))
	{
		return false;
	}

	// Check whether the 3d texture format is supported by OpenGL by creating a proxy texture.
	glTexImage3D(GL_PROXY_TEXTURE_3D, 0, iInternalFormat, iWidth, iHeight, iDepth, 0, iImgType, iDataType, nullptr);

	int iTestTexWidth = 0;
	// Now read the texture width. If it is zero, then the 3d texture cannot be created.
	glGetTexLevelParameteriv(GL_PROXY_TEXTURE_3D, 0, GL_TEXTURE_WIDTH, &iTestTexWidth);
	if (iTestTexWidth == 0)
	{
		return false;
	}

	// Now generate a new texture.
	if (!GenTexture(m_fPriority, m_uTexUnit))
	{
		return false;
	}

	// Convert the tensor to the texture data format.
	Mem<float> mData(xTensor.Size());
	unsigned uCnt     = (unsigned) mData.Count();
	float* pfTrgData  = mData.Data();
	double* pdSrcData = xTensor.Data();

	for (unsigned uIdx = 0; uIdx < uCnt; ++uIdx, ++pfTrgData, ++pdSrcData)
	{
		*pfTrgData = float(*pdSrcData);
	}

	// Now copy the data to the OpenGL device
	CLU_OGL_CALL(glBindTexture(GL_TEXTURE_3D, m_uTexID));
	CLU_OGL_CALL(glTexImage3D(GL_TEXTURE_3D, 0, iInternalFormat, iWidth, iHeight, iDepth, 0, iImgType, iDataType, mData.Data()));

	if (bGenMipMap)
	{
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		CLU_OGL_CALL(glGenerateMipmap(GL_TEXTURE_3D));
		m_bMipMap = true;
	}
	else
	{
		m_bMipMap = false;
	}

	_TellParentContentChanged();
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COGLTexture::SetTexture(CImageReference& rImage, float fPriority, uint uTexUnit, bool bGenMipMap, bool bBorder, bool bNorm)
{
	try
	{
		if (!rImage.IsValid())
		{
			throw CLU_EXCEPTION("Image is invalid");
		}

		if (uTexUnit >= m_uTexUnitCnt)
		{
			throw CLU_EXCEPTION(CLU_S "Texture unit '" << uTexUnit << "' is higher than '" << m_uTexUnitCnt << "'");
		}

		m_eTexType = ETextureType::TEX_2D;
		m_uTexUnit = uTexUnit;
		m_bBorder  = bBorder;

		if (glActiveTexture)
		{
			CLU_OGL_CALL(glActiveTexture(GL_TEXTURE0 + m_uTexUnit));
		}

		// Copy image and then flip it vertically, since texture have origin at bottom left
		// and images at top left.
		CImageReference xImage = rImage.GetImageRepositoryPtr()->Copy(rImage);
		xImage->FlipImage(0);

		int iImgType, iDataType, iBytesPerPixel;
		xImage->GetType(iImgType, iDataType, iBytesPerPixel);

		// Get the size of the image
		xImage->GetSize(m_iWidth, m_iHeight);

		int iInternalFormat;
		if (!_TryEvalTextureImageFormat(iInternalFormat, iImgType, iImgType, iDataType, bNorm))
		{
			throw CLU_EXCEPTION("Error evaluating texture image format");
		}

		m_iInternalDataFormat  = iDataType;
		m_iInternalPixelFormat = iImgType;

		m_bNorm = bNorm;

		// Get initial size reduction
		int iMaxTexSize = 0;
		CLU_OGL_CALL(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &iMaxTexSize));

		// Is image bigger than the maximum texture size?
		if ((m_iWidth > iMaxTexSize) || (m_iHeight > iMaxTexSize))
		{
			// Calculate scale to resize image to fit maximum texture size
			double dScale = double(iMaxTexSize) / double(m_iWidth > m_iHeight ? m_iWidth : m_iHeight);

			// Calculate new width and height
			m_iWidth  = int(floor(dScale * double(m_iWidth) + 0.5));
			m_iHeight = int(floor(dScale * double(m_iHeight) + 0.5));

			// Resize image
			xImage->ResizeImage(m_iWidth, m_iHeight);
		}

		// Create texture ID for the first time
		if (m_uTexID == 0)
		{
			if (GenTexture(m_fPriority, m_uTexUnit) == false)
			{
				throw CLU_EXCEPTION("Error generating texture");
			}
		}

		// Bind the texture
		CLU_OGL_CALL(glBindTexture(GL_TEXTURE_2D, m_uTexID));

		// Create texture and copy image into texture
		::LockImageAccess();
		Clu::OpenGL::TexImage2D(GL_TEXTURE_2D, 0, iInternalFormat, m_iWidth, m_iHeight, 0, iImgType, iDataType, xImage->GetDataPtr());

		#ifdef DEBUG
		GLenum eGlError;
		if (!Clu::OpenGL::IsOK(eGlError))
		{ 
			std::string sInfo = Clu::ToStdString(CLU_S
				InternalFormatToString(iInternalFormat)
				<< ", Format: " << FormatToString(iImgType)
				<< ", Type: " << TypeToString(iDataType)
				<< ", Width: " << m_iWidth << ", Height: " << m_iHeight
				<< ", Pointer: " << (size_t)xImage->GetDataPtr());

			throw CLU_EXCEPTION(Clu::OpenGL::GetErrorText(eGlError, sInfo));
		}
		#endif

		/************************************************************************/
		/* MipMaps?                                                             */
		/************************************************************************/
		if (bGenMipMap)
		{
			// In order to use gluScaleImage proper, we need to set the pixel pack/unpack alignment to 1
			// Otherwise the scaling of images with odd height or width does not work
			CLU_OGL_CALL(glPixelStorei(GL_PACK_ALIGNMENT, 1));
			CLU_OGL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

			unsigned iBytes = unsigned(m_iHeight) * unsigned(m_iWidth) * unsigned(iBytesPerPixel);
			uchar* pImgSrc  = new uchar[iBytes];
			uchar* pImgTrg  = new uchar[iBytes];

			memcpy(pImgSrc, xImage->GetDataPtr(), iBytes);
			::UnlockImageAccess();

			// Create MipMaps
			int iSrcW  = m_iWidth, iSrcH = m_iHeight;
			int iLevel = 1;

			while (iSrcW > 1 || iSrcH > 1)
			{
				// Calculate MipMap level size
				int iTrgW = std::max(iSrcW >> 1, 1);
				int iTrgH = std::max(iSrcH >> 1, 1);

				// Scale image to the new smaller size
				gluScaleImage(iImgType, iSrcW, iSrcH, iDataType, pImgSrc, iTrgW, iTrgH, iDataType, pImgTrg);

				// Create MipMap level texture
				Clu::OpenGL::TexImage2D(GL_TEXTURE_2D, iLevel, iInternalFormat, iTrgW, iTrgH, 0, iImgType, iDataType, pImgTrg);

				++iLevel;
				iSrcW = iTrgW;
				iSrcH = iTrgH;

				// Swap pointer
				std::swap(pImgSrc, pImgTrg);
			}

			delete[] pImgSrc;
			delete[] pImgTrg;

			m_bMipMap = true;
		}
		else
		{
			::UnlockImageAccess();
			m_bMipMap = false;
		}

		xImage->GetSize(m_iWidth, m_iHeight);

		_TellParentContentChanged();

		// Unbind the texture
		CLU_OGL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
	}
	catch (Clu::CIException& xEx)
	{
		throw CLU_EXCEPTION_NEST("Error setting texture", std::move(xEx));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COGLTexture::_TellParentContentChanged()
{
	for (COGLBaseElement* pxBaseElement : m_listParent)
	{
		COGLBaseElementList* pxBaseElementList = dynamic_cast<COGLBaseElementList*>(pxBaseElement);
		if (pxBaseElementList != nullptr)
		{
			pxBaseElementList->SetContentChanged(true, true, false);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COGLTexture::Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData)
{
	// Cannot use texturing in picking mode, as we use the object color to identify picked objects
	if ((eMode == COGLBaseElement::PICK) && !m_bEnableForPicking)
	{
		return true;
	}

	switch (m_eTexType)
	{
	case ETextureType::TEX_2D:
		return Apply_2D(eMode, rData);

	case ETextureType::TEX_3D:
		return Apply_3D(eMode, rData);

	default:
		throw CLU_EXCEPTION("Error applying texture. Texture is of type 'None'");
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COGLTexture::Apply_3D(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData)
{
	if (glActiveTexture)
	{
		glActiveTexture(GL_TEXTURE0 + m_uTexUnit);
	}

	// Check whether texture is only reset
	if (glIsTexture(m_uTexID) != GL_TRUE)
	{
		glBindTexture(GL_TEXTURE_3D, 0);
		glDisable(GL_TEXTURE_3D);

		rData.pbActTexUnit[m_uTexUnit] = false;
		rData.bMultiTexture            = rData.IsMultiTextured();
		return true;
	}

	glBindTexture(GL_TEXTURE_3D, m_uTexID);
	glEnable(GL_TEXTURE_3D);

	rData.pbActTexUnit[m_uTexUnit] = true;
	rData.bMultiTexture            = rData.IsMultiTextured();

	if (m_eWrapType == EWrapType::REPEAT)
	{
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	if (m_bInterpolate)
	{
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, (m_bMipMap) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, (m_bMipMap) ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
	}

	_SetTextureEnvMode();
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COGLTexture::Apply_2D(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData)
{
	if (glActiveTexture)
	{
		CLU_OGL_CALL(glActiveTexture(GL_TEXTURE0 + m_uTexUnit));
	}

	// Check whether texture is only reset
	if (glIsTexture(m_uTexID) != GL_TRUE)
	{
		CLU_OGL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
		CLU_OGL_CALL(glDisable(GL_TEXTURE_2D));

		rData.pbActTexUnit[m_uTexUnit] = false;
		rData.bMultiTexture            = rData.IsMultiTextured();
		return true;
	}

	uint uTexID = m_uTexID;

	CLU_OGL_CALL(glBindTexture(GL_TEXTURE_2D, uTexID));
	CLU_OGL_CALL(glEnable(GL_TEXTURE_2D));

	rData.pbActTexUnit[m_uTexUnit] = true;
	rData.bMultiTexture            = rData.IsMultiTextured();

	if (m_eWrapType == EWrapType::REPEAT)
	{
		CLU_OGL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		CLU_OGL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	}
	else
	{
		CLU_OGL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (m_bBorder) ? GL_CLAMP_TO_BORDER : GL_CLAMP_TO_EDGE));
		CLU_OGL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (m_bBorder) ? GL_CLAMP_TO_BORDER : GL_CLAMP_TO_EDGE));
	}

	if (m_bInterpolate)
	{
		CLU_OGL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		CLU_OGL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (m_bMipMap) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR));
	}
	else
	{
		CLU_OGL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		CLU_OGL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (m_bMipMap) ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST));
	}

	_SetTextureEnvMode();
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COGLTexture::_SetTextureEnvMode()
{
	if (!m_bNorm)
	{
		return;
	}

	// Define Combine Function
	CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE));

	if (m_eBlendTypeColor == EBlendType::INTERPOLATE)
	{
		// Estimate RGB Value as layers with transparency given by alpha
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_PREVIOUS));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_TEXTURE));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_SRC2_RGB, GL_TEXTURE));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_ALPHA));
	}
	else if (m_eBlendTypeColor == EBlendType::MODULATE)
	{
		// Estimate RGB Value as layers with transparency given by alpha
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_PREVIOUS));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_TEXTURE));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR));
	}
	else if (m_eBlendTypeColor == EBlendType::ADD)
	{
		// Estimate RGB Value as layers with transparency given by alpha
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_ADD));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_PREVIOUS));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_TEXTURE));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR));
	}
	else if (m_eBlendTypeColor == EBlendType::MIX)
	{
		// Estimate RGB Value as layers with transparency given by alpha
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_PREVIOUS));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_TEXTURE));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_ONE_MINUS_SRC_ALPHA));
	}
	else if (m_eBlendTypeColor == EBlendType::INVMOD)
	{
		// Estimate RGB Value as layers with transparency given by alpha
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_PREVIOUS));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_TEXTURE));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_ONE_MINUS_SRC_COLOR));
	}

	if (m_eBlendTypeAlpha == EBlendType::INTERPOLATE)
	{
		// Combine alpha values
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_INTERPOLATE));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_SRC1_ALPHA, GL_PREVIOUS));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_TEXTURE));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_SRC2_ALPHA, GL_TEXTURE));
	}
	else if (m_eBlendTypeAlpha == EBlendType::MODULATE)
	{
		// Combine alpha values
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_PREVIOUS));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_SRC1_ALPHA, GL_TEXTURE));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA));
	}
	else if (m_eBlendTypeAlpha == EBlendType::ADD)
	{
		// Combine alpha values
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_ADD));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_PREVIOUS));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_SRC1_ALPHA, GL_TEXTURE));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA));
	}
	else if (m_eBlendTypeAlpha == EBlendType::MIX)
	{
		// Combine alpha values
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_PREVIOUS));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_SRC1_ALPHA, GL_TEXTURE));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_ONE_MINUS_SRC_COLOR));
	}
	else if (m_eBlendTypeAlpha == EBlendType::INVMOD)
	{
		// Combine alpha values
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_PREVIOUS));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_SRC1_ALPHA, GL_TEXTURE));
		CLU_OGL_CALL(glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COGLTexture::GenTexture2DExternal(uint uTextureID)
{
	DeleteTexture();

	// Only delete texture if given texture ID is 0
	if (uTextureID == 0)
	{
		return true;
	}

	m_bExternalTexture = true;
	m_uTexID           = uTextureID;

	CLU_OGL_CALL(glActiveTexture(GL_TEXTURE0));
	CLU_OGL_CALL(glBindTexture(GL_TEXTURE_2D, m_uTexID));

	// Get width
	int iParam;
	CLU_OGL_CALL(glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &iParam));
	m_iWidth = iParam;

	// Get height
	CLU_OGL_CALL(glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &iParam));
	m_iHeight = iParam;

	//Get Depth
	CLU_OGL_CALL(glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_DEPTH, &iParam));
	m_iDepth = iParam;

	//Set Texture Type
	m_eTexType = ETextureType::TEX_2D;

	// Minify Filter
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &iParam);
	switch (iParam)
	{
	case GL_NEAREST:
		m_bInterpolate = false;
		m_bMipMap      = false;
		break;

	case GL_LINEAR:
		m_bInterpolate = true;
		m_bMipMap      = false;
		break;

	case GL_NEAREST_MIPMAP_NEAREST:
		m_bInterpolate = true;
		m_bMipMap      = true;
		break;

	case GL_LINEAR_MIPMAP_NEAREST:
		m_bInterpolate = true;
		m_bMipMap      = true;
		break;

	case GL_NEAREST_MIPMAP_LINEAR:
		m_bInterpolate = true;
		m_bMipMap      = true;
		break;

	case GL_LINEAR_MIPMAP_LINEAR:
		m_bInterpolate = true;
		m_bMipMap      = true;
		break;
	default:
		break;
	}

	// Magnify Filter
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &iParam);
	switch (iParam)
	{
	case GL_NEAREST:
		m_bInterpolate = false;
		break;

	case GL_LINEAR:
		m_bInterpolate = true;
		break;

	default:
		break;
	}

	int iInternalFormat;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &iInternalFormat);

	int iPixelType;
	int iDataType;
	bool bNorm;
	_TryDecomposeTextureImageFormat(iPixelType, iDataType, bNorm, iInternalFormat);

	m_iInternalPixelFormat = iPixelType;
	m_iInternalDataFormat  = iDataType;
	m_bNorm                = bNorm;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COGLTexture::GetTextureType(int& iPixelType, int& iDataType, bool& bNormalized)
{
	iPixelType  = m_iInternalPixelFormat;
	iDataType   = m_iInternalDataFormat;
	bNormalized = m_bNorm;

	return true;
}
