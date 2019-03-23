////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLTexture.h
//
// summary:   Declares the ogl texture class
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

#if !defined(AFX_OGLTEXTURE_H__INCLUDED_)
	#define AFX_OGLTEXTURE_H__INCLUDED_

#include "CluTec.Viz.Base/TensorData.h"
#include "CluTec.Viz.Base/TensorIdx.h"

#include "ImageReference.h"
#include "OGLBaseElement.h"
#include "OGLVertex.h"
#include "OGLColor.h"
#include "OGLImage.h"

	class CLUDRAW_API COGLTexture : public COGLBaseElement
	{
	public:

		typedef unsigned char uchar;

		enum class ETextureType
		{
			TEX_NONE,
			TEX_1D,
			TEX_2D,
			TEX_3D,
		};

		enum class EWrapType
		{
			CLAMP = 0,
			REPEAT
		};

		enum class EBlendType
		{
			INTERPOLATE = 0,
			MODULATE,
			ADD,
			MIX,
			INVMOD
		};

/*
        enum TTransMode
        {
                Exact = 0,
                Luminance,
                Font
        };
*/

	public:

		COGLTexture(ETextureType eTexType = ETextureType::TEX_2D);
		COGLTexture(const COGLTexture& oglBitmap);
		virtual ~COGLTexture(void);

		virtual COGLBaseElement* Copy()
		{
			return (COGLBaseElement*) new COGLTexture(*this);
		}

		COGLTexture& operator=(const COGLTexture& oglBitmap);

		bool GenTexture2DExternal(uint uTextureID);
		bool GenTexture(float fPriority, uint uTexUnit = 0);
		void SetTexture(CImageReference& rImage, float fPriority, uint uTexUnit = 0, bool bGenMipMap = true, bool bBorder = false, bool bNorm = true);
		bool SetTexture3d(const CTensorData<double>& xTensor, uint uTexUnit = 0, bool bGenMipMap = false, bool bNorm = true);
		void DeleteTexture();

		void SetPriority(float fPriority);
		void SetWrapType(EWrapType eType)
		{
			m_eWrapType = eType;
		}

		void SetBlendType(EBlendType eColorType, EBlendType eAlphaType)
		{
			m_eBlendTypeColor = eColorType;
			m_eBlendTypeAlpha = eAlphaType;
		}

		bool SetTextureUnit(uint uTexUnit);
		uint GetTextureUnit() { return m_uTexUnit; }

		uint GetTextureUnitCnt() { return m_uTexUnitCnt; }
		uint GetTextureID() { return m_uTexID; }

		// Return this textures target Type (Tex_2D, Tex_3D, etc.)
		ETextureType GetTextureTargetType() { return m_eTexType;  }

		// Returns the pixel and data type of this texture
		bool GetTextureType(int& iPixelType, int& iDataType, bool& bNormalized);

		bool GetTextureSize(int& iWidth, int& iHeight);
		bool GetTextureSize(int& iWidth, int& iHeight, int& iDepth);

		void EnableInterpolate(bool bVal = true)
		{ m_bInterpolate = bVal; }

		void EnableForPicking(bool bVal = true)
		{
			m_bEnableForPicking = bVal;
		}

		bool Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData);
		bool Apply_2D(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData);
		bool Apply_3D(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Converts the given internal format into a string representation.
		/// </summary>
		///
		/// <param name="iInternalFormat"> The internal format. </param>
		///
		/// <returns> The string. </returns>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		static const char* InternalFormatToString(int iInternalFormat);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Converts the given format into a string representation.
		/// </summary>
		///
		/// <param name="iFormat"> The format. </param>
		///
		/// <returns> The string. </returns>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		static const char* FormatToString(int iFormat);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Converts the given type into a string representation.
		/// </summary>
		///
		/// <param name="iType"> The type. </param>
		///
		/// <returns> The string. </returns>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		static const char* TypeToString(int iType);

	protected:

		void _ResetVars();
		void _TellParentContentChanged();
		bool _TryEvalTextureImageFormat(int& iInternalFormat, int& iTexImgType, int iImgType, int iDataType, bool bNorm);
		bool _TryDecomposeTextureImageFormat(int& iImgType, int& iDataType, bool& bNorm, int& iInternalFormat);
		void _SetTextureEnvMode();

	protected:

		// The texture type
		ETextureType m_eTexType;

		// Texture priority
		float m_fPriority;

		// Texture Wrap Type
		EWrapType m_eWrapType;

		// Type Texture is blended with fragments below it
		EBlendType m_eBlendTypeColor, m_eBlendTypeAlpha;

		// Is MipMapping enabled?
		bool m_bMipMap;

		// Does Texture have a border?
		bool m_bBorder;

		// If true, Min/Mag Filter are set to LINEAR, otherwise to NEAREST
		bool m_bInterpolate;

		// Flag whether this is a normalized texture, i.e. components are in range [0,1]
		bool m_bNorm;

		// ID of texture
		uint m_uTexID;

		// Texture unit texture will be displayed in
		uint m_uTexUnit;

		// Number of available texture units
		uint m_uTexUnitCnt;

		int m_iWidth, m_iHeight, m_iDepth;

		// Border Color of Texture
		COGLColor m_BorderColor;

		//The internal Pixel Format (e.g. RGB or RGBA)
		int m_iInternalPixelFormat;

		//The internal Data format (e.g. unsigned char, short)
		int m_iInternalDataFormat;

		// Flag to signal an external textureID.
		// External textures have been created outside of Cluviz, its not allowed to delete such textures
		bool m_bExternalTexture;

		bool m_bEnableForPicking;
	};

#endif
