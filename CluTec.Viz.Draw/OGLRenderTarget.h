////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLRenderTarget.h
//
// summary:   Declares the ogl render target class
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

#if !defined(AFX_OGLRENDERTARGET_H__INCLUDED_)
	#define AFX_OGLRENDERTARGET_H__INCLUDED_

#include <vector>
#include <string>

#include "ImageReference.h"
#include "OGLBaseElement.h"
#include "OGLBERepository.h"
#include "OGLRenderBuffer.h"

#include "CluTec.Viz.OpenGL.Extensions\Extensions.h"
#include "CluTec.Viz.OpenGL/Api.h"

	class CLUDRAW_API COGLRenderTarget : public COGLBaseElement
	{
	public:

		enum EAttachmentType
		{
			Attach_None = 0,
			Attach_RenderBuffer,
			Attach_Texture
		};

		enum EAttachmentPoint
		{
			AttachPnt_None         = 0,
			AttachPnt_Color0       = GL_COLOR_ATTACHMENT0,
			AttachPnt_Color1       = GL_COLOR_ATTACHMENT1,
			AttachPnt_Color2       = GL_COLOR_ATTACHMENT2,
			AttachPnt_Color3       = GL_COLOR_ATTACHMENT3,
			AttachPnt_Depth        = GL_DEPTH_ATTACHMENT,
			AttachPnt_Stencil      = GL_STENCIL_ATTACHMENT,
			AttachPnt_DepthStencil = GL_DEPTH_STENCIL_ATTACHMENT,
		};

		struct SAttachment
		{
			EAttachmentType eType;
			COGLBEReference refTarget;
		};

	public:

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Default constructor. Creates an empty render target.
		/// </summary>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		COGLRenderTarget();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Move constructor. Not allowed.
		/// </summary>
		///
		/// <param name="oglRenderTarget"> [in,out] The render target instance. </param>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		COGLRenderTarget(COGLRenderTarget&& oglRenderTarget) = delete;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Destructor.
		///
		/// 	Destroys the frame buffer object if one is associated with this object. Also destroys the associated render buffer.
		/// </summary>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual ~COGLRenderTarget();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	DOES NOT COPY THIS OBJECT, since only one instance may hold the reference to the frame buffer object.
		/// </summary>
		///
		/// <returns> Null if it fails, else a render target instance created with the default constructor. </returns>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual COGLBaseElement* Copy()
		{
			return (COGLBaseElement*) new COGLRenderTarget();
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Assignment operator. As this is not applicable it's deleted.
		/// </summary>
		///
		/// <param name="oglRenderTarget"> The render target instance. </param>
		///
		/// <returns> Not implemented. </returns>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		COGLRenderTarget& operator=(const COGLRenderTarget& oglRenderTarget) = delete;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Enables/disables this render target in a call to Apply().
		/// </summary>
		///
		/// <param name="bEnable"> (Optional) True to enable, false to disable. </param>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void Enable(bool bEnable = true)
		{
			m_bEnabled = bEnable;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Query if this object is enabled.
		/// </summary>
		///
		/// <returns> True if enabled, false if not. </returns>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool IsEnabled()
		{
			return m_bEnabled;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Enables a single rendering to the file with the given filename. The render target will automatically write the rendered
		/// 	image to the given file when the scene graph is rendered.
		/// </summary>
		///
		/// <param name="refSnapshotScene"> The reference snapshot scene. </param>
		/// <param name="pcFilename">	    Filename of the image file. </param>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void EnableSingleRenderToFile(COGLBEReference refSnapshotScene, const char* pcFilename);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Query if this object has successfully rendered to a file.
		/// </summary>
		///
		/// <returns> True if render to file successful, false if not. </returns>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool IsRenderToFileSuccessful();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Create a default render target. This creates a frame buffer object and attaches two render buffers to it: one RGBA8
		/// 	multi-sample image and a depth-stencil buffer.
		/// </summary>
		///
		/// <param name="uWidth">	    The width. </param>
		/// <param name="uHeight">	    The height. </param>
		/// <param name="uSampleCount"> Number of samples. </param>
		/// <param name="pBERepo">	    [in,out] If non-null, the be repo. </param>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void Create(unsigned uWidth, unsigned uHeight, unsigned uSampleCount, COGLBERepository* pBERepo);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Destroys the associated frame buffer object and any associated render buffers.
		/// </summary>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void Destroy();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Gets the image from the framebuffer object.
		/// </summary>
		///
		/// <param name="xImage"> [in,out] The image. </param>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void GetImage(COGLImage& xImage);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Makes this render target the current render target.
		/// </summary>
		///
		/// <param name="eMode"> The mode. </param>
		/// <param name="rData"> [in,out] The data. </param>
		///
		/// <returns> True if it succeeds, false if it fails. </returns>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Finalizes rendering. Has to be called when render target is changed.
		/// </summary>
		///
		/// <param name="bLastRenderPass"> true to last render pass. </param>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void Finalize(bool bLastRenderPass);

	protected:

		void _Reset();

	protected:

		/// <summary> True to enable, false to disable. </summary>
		bool m_bEnabled;

		/// <summary> Framebuffer Object Id. </summary>
		unsigned m_uFboId;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	When rendering to a multisample framebuffer object we need to resolve the multisampling to a
		/// 	non-multisampled framebuffer before we can read the pixels. This is the id of this normal
		/// 	framebuffer object.
		/// </summary>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		unsigned m_uResolveFboId;

		unsigned m_uWidth;
		unsigned m_uHeight;
		unsigned m_uSampleCount;

		GLint m_piViewport[4];
		GLint m_piScissorBox[4];
		GLboolean m_bUseScissor;
		GLdouble m_pdProjMat[16];

		/// <summary> Number of times this render target object is applied during the first render pass. </summary>
		unsigned m_uApplyCount;

		bool m_bEnableSingleRenderToFile;
		bool m_bRenderToFileSuccess;
		std::string m_sRenderToFilename;

		COGLBEReference m_refRenderScene;
	};

#endif
