////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLRenderBuffer.cpp
//
// summary:   Implements the ogl render buffer class
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
#include "OGLRenderBuffer.h"
#include "OGLBaseElementList.h"
#include "OGLImageTypeDef.h"

#include "CluTec.Viz.OpenGL.Extensions\Extensions.h"
#include "CluTec.Viz.OpenGL/Api.h"

#include <exception>

// /////////////////////////////////////////////////////////////////////////////////////
COGLRenderBuffer::COGLRenderBuffer(void)
{
	_Reset();
}

// /////////////////////////////////////////////////////////////////////////////////////
COGLRenderBuffer::COGLRenderBuffer(COGLRenderBuffer&& oglRenderBuffer)
{
	oglRenderBuffer.YieldTo(*this);
}

// /////////////////////////////////////////////////////////////////////////////////////
COGLRenderBuffer::~COGLRenderBuffer(void)
{
	Destroy();
}

// /////////////////////////////////////////////////////////////////////////////////////
COGLRenderBuffer& COGLRenderBuffer::operator=(const COGLRenderBuffer& oglRenderBuffer)
{
	throw std::exception("Assignment operator not applicable for COGLRenderBuffer object");
}

// /////////////////////////////////////////////////////////////////////////////////////
void COGLRenderBuffer::_Reset()
{
	m_uBufId       = 0;
	m_uWidth       = 0;
	m_uHeight      = 0;
	m_uSampleCount = 0;

	m_uInternalFormat = 0;
	m_uPixelType      = 0;
	m_uDataType       = 0;
}

// /////////////////////////////////////////////////////////////////////////////////////
void COGLRenderBuffer::YieldTo(COGLRenderBuffer& oglRenderBuffer)
{
	// Copy all data
	oglRenderBuffer.m_uBufId       = m_uBufId;
	oglRenderBuffer.m_uWidth       = m_uWidth;
	oglRenderBuffer.m_uHeight      = m_uHeight;
	oglRenderBuffer.m_uSampleCount = m_uSampleCount;

	oglRenderBuffer.m_uInternalFormat = m_uInternalFormat;
	oglRenderBuffer.m_uPixelType      = m_uPixelType;
	oglRenderBuffer.m_uDataType       = m_uDataType;

	// Now invalidate this instance.
	_Reset();
}

// /////////////////////////////////////////////////////////////////////////////////////
void COGLRenderBuffer::Destroy()
{
	// Only destroy if we have a valid render buffer.
	if (m_uBufId != 0)
	{
		CLU_OGL_CALL(glDeleteRenderbuffers(1, &m_uBufId));
		_Reset();
	}
}

// /////////////////////////////////////////////////////////////////////////////////////
void COGLRenderBuffer::CreateImage(unsigned uWidth, unsigned uHeight, unsigned uSampleCount, unsigned uPixelType, unsigned uDataType)
{
	Destroy();

	if (!_TryEvalInternalImageFormat(m_uInternalFormat, uPixelType, uDataType))
	{
		throw CLU_EXCEPTION("Invalid image type for creating a render buffer");
	}

	m_uPixelType   = uPixelType;
	m_uDataType    = uDataType;
	m_uWidth       = uWidth;
	m_uHeight      = uHeight;
	m_uSampleCount = uSampleCount;

	GLint iMaxSampleCount = 0;
	CLU_OGL_CALL(glGetIntegerv(GL_MAX_SAMPLES, &iMaxSampleCount));

	if (m_uSampleCount > unsigned(iMaxSampleCount))
	{
		m_uSampleCount = unsigned(iMaxSampleCount);
	}

	CLU_OGL_CALL(glGenRenderbuffers(1, &m_uBufId));
	CLU_OGL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, m_uBufId));
	CLU_OGL_CALL(glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_uSampleCount, m_uInternalFormat, m_uWidth, m_uHeight));
	CLU_OGL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, 0));
}

// /////////////////////////////////////////////////////////////////////////////////////
void COGLRenderBuffer::CreateDepthStencil(unsigned uWidth, unsigned uHeight, unsigned uSampleCount)
{
	Destroy();

	CLU_OGL_CALL(glGenRenderbuffers(1, &m_uBufId));
	CLU_OGL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, m_uBufId));

	m_uInternalFormat = GL_DEPTH32F_STENCIL8;
	//m_uInternalFormat = GL_DEPTH24_STENCIL8;
	m_uWidth       = uWidth;
	m_uHeight      = uHeight;
	m_uSampleCount = uSampleCount;

	GLint iMaxSampleCount = 0;
	CLU_OGL_CALL(glGetIntegerv(GL_MAX_SAMPLES, &iMaxSampleCount));

	if (m_uSampleCount > unsigned(iMaxSampleCount))
	{
		m_uSampleCount = unsigned(iMaxSampleCount);
	}

	CLU_OGL_CALL(glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_uSampleCount, m_uInternalFormat, m_uWidth, m_uHeight));
	CLU_OGL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, 0));
}

// /////////////////////////////////////////////////////////////////////////////////////
bool COGLRenderBuffer::_TryEvalInternalImageFormat(unsigned& uInternalFormat, unsigned uImgType, unsigned uDataType)
{
	// May become a parameter at a later stage if rendering to integer buffers is needed and supported.
	const bool bNorm = true;

	if (((uImgType == CLUVIZ_IMG_RGB) || (uImgType == CLUVIZ_IMG_BGR)))
	{
		if (uDataType == CLUVIZ_IMG_BYTE)
		{
			uInternalFormat = (bNorm ? GL_RGB8 : GL_RGB8I);
		}
		else if (uDataType == CLUVIZ_IMG_UNSIGNED_BYTE)
		{
			uInternalFormat = (bNorm ? GL_RGB8 : GL_RGB8UI);
		}
		else if (uDataType == CLUVIZ_IMG_SHORT)
		{
			uInternalFormat = (bNorm ? GL_RGB16 : GL_RGB16I);
		}
		else if (uDataType == CLUVIZ_IMG_UNSIGNED_SHORT)
		{
			uInternalFormat = (bNorm ? GL_RGB16 : GL_RGB16UI);
		}
		else if (uDataType == CLUVIZ_IMG_INT)
		{
			uInternalFormat = (bNorm ? GL_RGB16 : GL_RGB32I);
		}
		else if (uDataType == CLUVIZ_IMG_UNSIGNED_INT)
		{
			uInternalFormat = (bNorm ? GL_RGB16 : GL_RGB32UI);
		}
		else if (uDataType == CLUVIZ_IMG_FLOAT)
		{
			uInternalFormat = (bNorm ? GL_RGB16 : GL_RGB32F);
		}
		else if (uDataType == CLUVIZ_IMG_DOUBLE)
		{
			uInternalFormat = (bNorm ? GL_RGB16 : GL_RGB32F);
		}
		else
		{
			return false;
		}
	}
	else if ((uImgType == CLUVIZ_IMG_RGBA) || (uImgType == CLUVIZ_IMG_BGRA))
	{
		if (uDataType == CLUVIZ_IMG_BYTE)
		{
			uInternalFormat = (bNorm ? GL_RGBA8 : GL_RGBA8I);
		}
		else if (uDataType == CLUVIZ_IMG_UNSIGNED_BYTE)
		{
			uInternalFormat = (bNorm ? GL_RGBA8 : GL_RGBA8UI);
		}
		else if (uDataType == CLUVIZ_IMG_SHORT)
		{
			uInternalFormat = (bNorm ? GL_RGBA16 : GL_RGBA16I);
		}
		else if (uDataType == CLUVIZ_IMG_UNSIGNED_SHORT)
		{
			uInternalFormat = (bNorm ? GL_RGBA16 : GL_RGBA16UI);
		}
		else if (uDataType == CLUVIZ_IMG_INT)
		{
			uInternalFormat = (bNorm ? GL_RGBA16 : GL_RGBA32I);
		}
		else if (uDataType == CLUVIZ_IMG_UNSIGNED_INT)
		{
			uInternalFormat = (bNorm ? GL_RGBA16 : GL_RGBA32UI);
		}
		else if (uDataType == CLUVIZ_IMG_FLOAT)
		{
			uInternalFormat = (bNorm ? GL_RGBA16 : GL_RGBA32F);
		}
		else if (uDataType == CLUVIZ_IMG_DOUBLE)
		{
			uInternalFormat = (bNorm ? GL_RGBA16 : GL_RGBA32F);
		}
		else
		{
			return false;
		}
	}
	else if (uImgType == CLUVIZ_IMG_LUMINANCE)
	{
		if (uDataType == CLUVIZ_IMG_BYTE)
		{
			uInternalFormat = (bNorm ? GL_LUMINANCE8 : GL_R8I);
		}
		else if (uDataType == CLUVIZ_IMG_UNSIGNED_BYTE)
		{
			uInternalFormat = (bNorm ? GL_LUMINANCE8 : GL_R8UI);
		}
		else if (uDataType == CLUVIZ_IMG_SHORT)
		{
			uInternalFormat = (bNorm ? GL_LUMINANCE16 : GL_R16I);
		}
		else if (uDataType == CLUVIZ_IMG_UNSIGNED_SHORT)
		{
			uInternalFormat = (bNorm ? GL_LUMINANCE16 : GL_R16UI);
		}
		else if (uDataType == CLUVIZ_IMG_INT)
		{
			uInternalFormat = (bNorm ? GL_LUMINANCE16 : GL_R32I);
		}
		else if (uDataType == CLUVIZ_IMG_UNSIGNED_INT)
		{
			uInternalFormat = (bNorm ? GL_LUMINANCE16 : GL_R32UI);
		}
		else if (uDataType == CLUVIZ_IMG_FLOAT)
		{
			uInternalFormat = (bNorm ? GL_LUMINANCE16 : GL_R32F);
		}
		else if (uDataType == CLUVIZ_IMG_DOUBLE)
		{
			uInternalFormat = (bNorm ? GL_LUMINANCE16 : GL_R32F);
		}
		else
		{
			return false;
		}
	}
	else if (uImgType == CLUVIZ_IMG_LUMINANCE_ALPHA)
	{
		if (uDataType == CLUVIZ_IMG_BYTE)
		{
			uInternalFormat = (bNorm ? GL_LUMINANCE8_ALPHA8 : GL_RG8I);
		}
		else if (uDataType == CLUVIZ_IMG_UNSIGNED_BYTE)
		{
			uInternalFormat = (bNorm ? GL_LUMINANCE8_ALPHA8 : GL_RG8UI);
		}
		else if (uDataType == CLUVIZ_IMG_SHORT)
		{
			uInternalFormat = (bNorm ? GL_LUMINANCE16_ALPHA16 : GL_RG16I);
		}
		else if (uDataType == CLUVIZ_IMG_UNSIGNED_SHORT)
		{
			uInternalFormat = (bNorm ? GL_LUMINANCE16_ALPHA16 : GL_RG16UI);
		}
		else if (uDataType == CLUVIZ_IMG_INT)
		{
			uInternalFormat = (bNorm ? GL_LUMINANCE16_ALPHA16 : GL_RG32I);
		}
		else if (uDataType == CLUVIZ_IMG_UNSIGNED_INT)
		{
			uInternalFormat = (bNorm ? GL_LUMINANCE16_ALPHA16 : GL_RG32UI);
		}
		else if (uDataType == CLUVIZ_IMG_FLOAT)
		{
			uInternalFormat = (bNorm ? GL_LUMINANCE16_ALPHA16 : GL_RG32F);
		}
		else if (uDataType == CLUVIZ_IMG_DOUBLE)
		{
			uInternalFormat = (bNorm ? GL_LUMINANCE16_ALPHA16 : GL_RG32F);
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

	return true;
}
