////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.View.Base
// file:      OGLWin_Init.cpp
//
// summary:   Implements the ogl window initialise class
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

#include <string>
using namespace std;

////////////////////////////////////////////////////////
// Call init of CLUDrawBase

bool COGLWin::CLUDrawBaseInit(bool bFirst)
{
	return CCLUDrawBase::Init(bFirst);
}

/////////////////////////////////////////////////////////////////////////
// Create a rendering context with that shares textures with GL Window

bool COGLWin::CreateSharedRC()
{
	// Create a dummy window
	m_hShareGLWnd = ::CreateWindowA("STATIC", "Dummy", WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, 1, 1, NULL, NULL, ::GetModuleHandle(NULL), NULL);

	// Get the device context of this dummy window
	m_hShareGLDC = GetDC(m_hShareGLWnd);

	// Get the pixel format descriptor that matches the combination of our desired pixel format and our dummy window
	PIXELFORMATDESCRIPTOR xPFD;
	DescribePixelFormat(m_hShareGLDC, m_iPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &xPFD);

	// Set the pixel format to our dummy window. The pixel format descriptor is only provided to get it back by calling GetPixelFormat()
	if (SetPixelFormat(m_hShareGLDC, m_iPixelFormat, &xPFD) == FALSE)
	{
		ReleaseDC(m_hShareGLWnd, m_hShareGLDC);
		::DestroyWindow(m_hShareGLWnd);
		return false;
	}

	// Finally create an OpenGL rendering context for this thread
	m_hShareGLRC = wglCreateContext(m_hShareGLDC);

	m_bHasSharedGLWnd = true;
	return true;
}

/////////////////////////////////////////////////////////////////////////
// Create a rendering context with that shares textures with GL Window

bool COGLWin::DestroySharedRC()
{
	if (!m_bHasSharedGLWnd)
	{
		return false;
	}

	wglMakeCurrent(0, 0);
	wglDeleteContext(m_hShareGLRC);
	ReleaseDC(m_hShareGLWnd, m_hShareGLDC);
	::DestroyWindow(m_hShareGLWnd);

	m_hShareGLWnd = 0;
	m_hShareGLDC  = 0;
	m_hShareGLRC  = 0;

	m_bHasSharedGLWnd = false;

	return true;
}

//////////////////////////////////////////////////////////
/// Initialize GL Extensions

bool COGLWin::InitGLExt()
{
	int  iPixelFormat;

	// Standard Pixel Format descriptor
	PIXELFORMATDESCRIPTOR pPFD = {
		sizeof(PIXELFORMATDESCRIPTOR),	//  size of this pfd
		1,			// version number
		PFD_DRAW_TO_WINDOW |	// support window
		PFD_SUPPORT_OPENGL |	// support OpenGL
		PFD_DOUBLEBUFFER,	// double buffered
		PFD_TYPE_RGBA,		// RGBA type
		24,			// 24-bit color depth
		0, 0, 0, 0, 0, 0,	// color bits ignored
		0,			// no alpha buffer
		0,			// shift bit ignored
		0,			// no accumulation buffer
		0, 0, 0, 0,		// accum bits ignored
		0,			// 32-bit z-buffer
		0,			// no stencil buffer
		0,			// no auxiliary buffer
		PFD_MAIN_PLANE,		// main layer
		0,			// reserved
		0, 0, 0			// layer masks ignored
	};

	// Create a dummy window to initialize the OpenGL ARBs
	HWND hWnd = ::CreateWindowA("STATIC", "Dummy", WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, 1, 1, NULL, NULL, ::GetModuleHandle(NULL), NULL);

	//// Get the DC of the GLUT window
	m_hGLDC = GetDC(hWnd);

	// Choose the nearest pixel format
	iPixelFormat = ChoosePixelFormat(m_hGLDC, &pPFD);

	// Set the pixel format
	if (SetPixelFormat(m_hGLDC, iPixelFormat, &pPFD) == FALSE)
	{
		ReleaseDC(hWnd, m_hGLDC);
		::DestroyWindow(hWnd);
		return false;
	}

	// Create a rendering context for this thread
	m_hGLRC = wglCreateContext(m_hGLDC);

	// Make it the current rendering context
	wglMakeCurrent(m_hGLDC, m_hGLRC);

	// Initialize OpenGL Extensions
	if (!Clu::OpenGL::Extensions::IsInitialized())
	{
		Clu::OpenGL::Extensions::Init();
	}

	// Delete rendering context
	wglMakeCurrent(0, 0);
	wglDeleteContext(m_hGLRC);
	ReleaseDC(hWnd, m_hGLDC);
	::DestroyWindow(hWnd);

	m_hGLRC = 0;
	m_hGLDC = 0;

	return true;
}

//////////////////////////////////////////////////////////
// Prune Pixel Format list

bool COGLWin::PrunePixelFormatList(std::vector<int>& vecPrune, const std::vector<int>& vecFormat, int iAttribute, int iExpValue)
{
	int iValue;
	bool bFound = false;
	unsigned uFormat;

	vecPrune.clear();

	for (uFormat = 0; uFormat < vecFormat.size(); ++uFormat)
	{
		if ((wglGetPixelFormatAttribivARB(m_hGLDC, vecFormat[uFormat], 0, 1, &iAttribute, &iValue) == TRUE)
		    && (iValue == iExpValue))
		{
			vecPrune.push_back(vecFormat[uFormat]);
			bFound = true;
		}
	}

	return bFound;
}

//////////////////////////////////////////////////////////
/// Find appropriate pixel format

bool COGLWin::InitPixelFormat()
{
	// Standard Pixel Format descriptor
	PIXELFORMATDESCRIPTOR pPFD;
	PIXELFORMATDESCRIPTOR pPFD_Std = {
		sizeof(PIXELFORMATDESCRIPTOR),	//  size of this pfd
		1,			// version number
		PFD_DRAW_TO_WINDOW |	// support window
		PFD_SUPPORT_OPENGL |	// support OpenGL
		PFD_DOUBLEBUFFER,	// double buffered
		PFD_TYPE_RGBA,		// RGBA type
		32,			// 32-bit color depth
		0, 0, 0, 0, 0, 0,	// color bits ignored
		8,			// 8 bit alpha buffer
		0,			// shift bit ignored
		0,			// no accumulation buffer
		0, 0, 0, 0,		// accum bits ignored
		16,			// 16-bit z-buffer
		4,			// 4-bit stencil buffer
		0,			// no auxiliary buffer
		PFD_MAIN_PLANE,		// main layer
		0,			// reserved
		0, 0, 0			// layer masks ignored
	};

	int iPixelFormat = -1;
	int piNewFormat[100];
	unsigned int uFormatCount = 0;

	// Get the DC of the GLUT window
	m_hGLDC = GetDC(GetGLWindowHandle());

	// If the wglChoosePixelFormat extension exists, try to get a pixel format
	// with 2x multisampling.
	if (wglChoosePixelFormatARB)
	{
		GLint piAttribIList1[] =
		{
			WGL_DRAW_TO_WINDOW_ARB,         GL_TRUE,
			//WGL_SWAP_METHOD_ARB,		WGL_SWAP_EXCHANGE_ARB,
			WGL_SUPPORT_OPENGL_ARB,         GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB,          GL_TRUE,
			WGL_STEREO_ARB,                         GL_FALSE,
			WGL_PIXEL_TYPE_ARB,                     WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB,                     32,
			WGL_ALPHA_BITS_ARB,                     8,
			WGL_DEPTH_BITS_ARB,                     24,
			WGL_STENCIL_BITS_ARB,           8,
			WGL_SAMPLE_BUFFERS_ARB,         GL_TRUE,
			WGL_SAMPLES_ARB,                        2,
			0, 0
		};

		GLint piAttribIList2[] =
		{
			WGL_DRAW_TO_WINDOW_ARB,         GL_TRUE,
			//WGL_SWAP_METHOD_ARB,		WGL_SWAP_EXCHANGE_ARB,
			WGL_SUPPORT_OPENGL_ARB,         GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB,          GL_TRUE,
			WGL_STEREO_ARB,                         GL_FALSE,
			WGL_PIXEL_TYPE_ARB,                     WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB,                     32,
			WGL_ALPHA_BITS_ARB,                     8,
			WGL_DEPTH_BITS_ARB,                     16,
			WGL_STENCIL_BITS_ARB,           4,
			WGL_SAMPLE_BUFFERS_ARB,         GL_TRUE,
			WGL_SAMPLES_ARB,                        2,
			0, 0
		};

		GLint piAttribIList3[] =
		{
			WGL_DRAW_TO_WINDOW_ARB,         GL_TRUE,
			//WGL_SWAP_METHOD_ARB,		WGL_SWAP_EXCHANGE_ARB,
			WGL_SUPPORT_OPENGL_ARB,         GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB,          GL_TRUE,
			WGL_STEREO_ARB,                         GL_FALSE,
			WGL_PIXEL_TYPE_ARB,                     WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB,                     32,
			WGL_ALPHA_BITS_ARB,                     8,
			WGL_DEPTH_BITS_ARB,                     32,
			WGL_STENCIL_BITS_ARB,           4,
			//WGL_SAMPLE_BUFFERS_ARB,		GL_TRUE,
			//WGL_SAMPLES_ARB,			2,
			0, 0
		};

		GLint piAttribIList4[] =
		{
			WGL_DRAW_TO_WINDOW_ARB,         GL_TRUE,
			//WGL_SWAP_METHOD_ARB,		WGL_SWAP_EXCHANGE_ARB,
			WGL_SUPPORT_OPENGL_ARB,         GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB,          GL_TRUE,
			WGL_STEREO_ARB,                         GL_FALSE,
			WGL_PIXEL_TYPE_ARB,                     WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB,                     32,
			WGL_ALPHA_BITS_ARB,                     8,
			WGL_DEPTH_BITS_ARB,                     16,
			WGL_STENCIL_BITS_ARB,           4,
			//WGL_SAMPLE_BUFFERS_ARB,		GL_TRUE,
			//WGL_SAMPLES_ARB,			2,
			0, 0
		};

		GLint* piAttrib[] =
		{
			piAttribIList1,
			piAttribIList2,
			piAttribIList3,
			piAttribIList4
		};

		GLint iAttribList, iAttribListCount = 4;

		GLfloat pfAttribFList[] = { 0.0f, 0.0f };

		for (iAttribList = 0; iAttribList < iAttribListCount; ++iAttribList)
		{
			if ((wglChoosePixelFormatARB(m_hGLDC, piAttrib[iAttribList], pfAttribFList, 100, piNewFormat, &uFormatCount) == TRUE)
			    && (uFormatCount > 0))
			{
				unsigned uFormat;
				std::vector<int> vecFormat, vecPrune;

				vecFormat.resize(uFormatCount);

				for (uFormat = 0; uFormat < uFormatCount; ++uFormat)
				{
					vecFormat[uFormat] = piNewFormat[uFormat];
				}

				if (!PrunePixelFormatList(vecPrune, vecFormat, WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB))
				{
					if (PrunePixelFormatList(vecPrune, vecFormat, WGL_ACCELERATION_ARB, WGL_GENERIC_ACCELERATION_ARB))
					{
						vecFormat = vecPrune;
					}
				}
				else
				{
					vecFormat = vecPrune;
				}

				if (PrunePixelFormatList(vecPrune, vecFormat, WGL_NUMBER_OVERLAYS_ARB, 0))
				{
					vecFormat = vecPrune;
				}

				if (PrunePixelFormatList(vecPrune, vecFormat, WGL_NUMBER_UNDERLAYS_ARB, 0))
				{
					vecFormat = vecPrune;
				}

				if (!PrunePixelFormatList(vecPrune, vecFormat, WGL_SWAP_METHOD_ARB, WGL_SWAP_EXCHANGE_ARB))
				{
					if (PrunePixelFormatList(vecPrune, vecFormat, WGL_SWAP_METHOD_ARB, WGL_SWAP_COPY_ARB))
					{
						vecFormat = vecPrune;
					}
				}
				else
				{
					vecFormat = vecPrune;
				}

				if (vecFormat.size())
				{
					int iSample = m_iMultisampleTarget;

					while (iSample > 0 && !PrunePixelFormatList(vecPrune, vecFormat, WGL_SAMPLES_ARB, iSample))
					{
						iSample >>= 1;
					}

					vecFormat = vecPrune;
				}

				if (vecFormat.size())
				{
					for (uFormat = 0; uFormat < vecFormat.size(); ++uFormat)
					{
						iPixelFormat = vecFormat[uFormat];
						// Get the PFD struct for the new pixel format
						DescribePixelFormat(m_hGLDC, iPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pPFD);

						if (SetPixelFormat(m_hGLDC, iPixelFormat, &pPFD) == FALSE)
						{
							iPixelFormat = 0;
						}
						else
						{
							break;
						}
					}
				}
				else
				{
					// No pixel format found meeting our desired criteria
					// Use a minimal pixel format
					// Choose the nearest pixel format
					iPixelFormat = ChoosePixelFormat(m_hGLDC, &pPFD_Std);

					// Set the pixel format
					if (SetPixelFormat(m_hGLDC, iPixelFormat, &pPFD_Std) == FALSE)
					{
						return false;
					}
				}
				break;
			}
		}

		if (iPixelFormat < 0)
		{
			// No pixel format could be found
			m_hGLDC        = 0;
			m_iPixelFormat = -1;
			return false;
		}
	}
	else
	{
		// Choose the nearest pixel format
		iPixelFormat = ChoosePixelFormat(m_hGLDC, &pPFD_Std);

		// Set the pixel format
		if (SetPixelFormat(m_hGLDC, iPixelFormat, &pPFD_Std) == FALSE)
		{
			return false;
		}
	}

	m_iPixelFormat = iPixelFormat;

	::ReleaseDC(GetGLWindowHandle(), m_hGLDC);
	m_hGLDC = 0;

	return true;
}

////////////////////////////////////////////////////////
// Create Rendering Context for main window

bool COGLWin::CreateMainRC()
{
	// Get the windows context of our main window
	m_hGLDC = GetDC(GetGLWindowHandle());

	// Get the pixel format descriptor that matches the combination of our desired pixel format and our main window
	PIXELFORMATDESCRIPTOR xPFD;
	DescribePixelFormat(m_hGLDC, m_iPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &xPFD);

	// Set the pixel format to our main window. The pixel format descriptor is only provided to get it back by calling GetPixelFormat()
	if (SetPixelFormat(m_hGLDC, m_iPixelFormat, &xPFD) == FALSE)
	{
		return false;
	}

	// Finally create an OpenGL rendering context for this thread
	m_hGLRC = wglCreateContext(m_hGLDC);

	if (m_hShareGLRC)
	{
		// Share Textures and Display lists
		wglShareLists(m_hGLRC, m_hShareGLRC);
	}
	else if (m_hShareExtGLRC)
	{
		// Share Textures and Display lists
		BOOL bRet = wglShareLists(m_hShareExtGLRC, m_hGLRC);
		if (bRet != TRUE)
		{
			// TODO: Add error handling
			//#define __LOCATION__ __FILE__ " (" __LINE__ ")" " - " __FUNCTION__  ": "

#pragma message(__FILE__ ": "  "Add error handling here")
			DWORD dwErr = GetLastError();
		}
	}

	// Make it the current rendering context
	wglMakeCurrent(m_hGLDC, m_hGLRC);

	if (wglSwapIntervalEXT)
	{
		// Synchronize with vsync
		wglSwapIntervalEXT(1);
	}

	return true;
}
