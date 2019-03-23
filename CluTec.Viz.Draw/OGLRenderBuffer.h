////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLRenderBuffer.h
//
// summary:   Declares the ogl render buffer class
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

//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLRENDERBUFFER_H__INCLUDED_)
	#define AFX_OGLRENDERBUFFER_H__INCLUDED_

#include "ImageReference.h"
#include "OGLBaseElement.h"

#include <vector>
#include <string>

	class CLUDRAW_API COGLRenderBuffer : public COGLBaseElement
	{
	public:

	public:

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Default constructor. Creates an empty render buffer instance.
		/// </summary>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		COGLRenderBuffer(void);

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Move constructor. Cannot copy a render buffer instance. Only one object can hold the render
		/// buffer id. This why we only have a move constructor and no copy constructor.
		/// </summary>
		///
		/// <param name="oglRenderTarget">	[in,out] The ogl render target. </param>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		COGLRenderBuffer(COGLRenderBuffer&& oglRenderTarget);

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Destructor. Destroys the render buffer if one has been associated with this instance.
		/// </summary>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		virtual ~COGLRenderBuffer(void);

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	Assignment operator. As this is not applicable it always throws an exception. </summary>
		///
		/// <param name="oglRenderTarget">	The render target instance. </param>
		///
		/// <returns>	Always throws an exception. </returns>
		////////////////////////////////////////////////////////////////////////////////////////////////////
		COGLRenderBuffer& operator=(const COGLRenderBuffer& oglRenderBuffer);

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// DOES NOT COPY THIS OBJECT, since only one instance may hold the reference to the frame buffer
		/// object.
		/// </summary>
		///
		/// <returns>
		/// Null if it fails, else a render target instance created with the default constructor.
		/// </returns>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		virtual COGLBaseElement* Copy()
		{
			return (COGLBaseElement*) new COGLRenderBuffer();
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Yield the render buffer of this instance to the given instance. When this function returns,
		/// this instance will be invalidated.
		/// </summary>
		///
		/// <param name="oglRenderBuffer">	[in,out] The render buffer object that will contain the
		/// 								render buffer id on return. </param>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		void YieldTo(COGLRenderBuffer& oglRenderBuffer);

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Create a render buffer that will contain an image.
		/// </summary>
		///
		/// <param name="uWidth">	   	The width. </param>
		/// <param name="uHeight">	   	The height. </param>
		/// <param name="uSampleCount">	Number of samples per pixel. </param>
		/// <param name="uPixelType">  	Render buffer pixel type. </param>
		/// <param name="uDataType">   	Render buffer data type. </param>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		void CreateImage(unsigned uWidth, unsigned uHeight, unsigned uSampleCount, unsigned uPixelType, unsigned uDataType);

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Creates a render buffer for depth-stencil use.
		/// </summary>
		///
		/// <param name="uWidth"> 	The width. </param>
		/// <param name="uHeight">	The height. </param>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		void CreateDepthStencil(unsigned uWidth, unsigned uHeight, unsigned uSampleCount);

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Destroys the render buffer associated with this object.
		/// </summary>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		void Destroy();

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Gets the render buffer identifier.
		/// </summary>
		///
		/// <returns>	The render buffer identifier. </returns>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		unsigned GetRenderBufferId() const
		{
			return m_uBufId;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Gets the width.
		/// </summary>
		///
		/// <returns>	The width. </returns>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		unsigned GetWidth() const
		{
			return m_uWidth;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Gets the height.
		/// </summary>
		///
		/// <returns>	The height. </returns>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		unsigned GetHeight() const
		{
			return m_uHeight;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Gets the sample count.
		/// </summary>
		///
		/// <returns>	The sample count. </returns>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		unsigned GetSampleCount() const
		{
			return m_uSampleCount;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Gets an image type.
		/// </summary>
		///
		/// <param name="uPixelType">	[in,out] Type of the pixel. </param>
		/// <param name="uDataType"> 	[in,out] Type of the data. </param>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		void GetImageType(unsigned& uPixelType, unsigned& uDataType)
		{
			uPixelType = m_uPixelType;
			uDataType  = m_uDataType;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// This function does nothing as a render buffer by itself cannot do anything. It needs to be
		/// attached to a frame buffer object.
		/// </summary>
		///
		/// <param name="eMode">	The mode. </param>
		/// <param name="rData">	[in,out] The data. </param>
		///
		/// <returns>	True if it succeeds, false if it fails. </returns>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		bool Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData)
		{
			return true;
		}

	protected:

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Resets this object. Set all elements to zero but does not call any OpenGL functions.
		/// </summary>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		void _Reset();

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Try to evaluate and internal image format from the given pixel and data types.
		/// </summary>
		///
		/// <param name="uInternalFormat">	[in,out] The internal format. </param>
		/// <param name="uPixelType">	  	Type of the pixel. </param>
		/// <param name="uDataType">	  	Type of the data. </param>
		///
		/// <returns>	True if it succeeds, false if it fails. </returns>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		bool _TryEvalInternalImageFormat(unsigned& uInternalFormat, unsigned uPixelType, unsigned uDataType);

	protected:

		/// <summary>	Identifier for the buffer. </summary>
		unsigned m_uBufId;

		unsigned m_uWidth, m_uHeight;
		unsigned m_uSampleCount;

		unsigned m_uPixelType;
		unsigned m_uDataType;
		unsigned m_uInternalFormat;
	};

#endif
