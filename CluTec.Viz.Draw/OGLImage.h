////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLImage.h
//
// summary:   Declares the ogl image class
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

// OGLImage.h: Schnittstelle f�r die Klasse OGLImage
//
//		This class only deals with the image itself.
//		It does not allow drawing to OGL Window.
//		See class COGLBitmap for this purpose.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLIMAGE_H__INCLUDED_)
	#define AFX_OGLIMAGE_H__INCLUDED_

#include <vector>
#include <limits>

#include "CluTec.Types1\IImage.h"

#include "CluTec.Viz.ImgRepo\CLUImgRep.h"

#include "OGLColor.h"
#include "OGLImageTypeDef.h"

#include "IL\il.h"
#include "IL\ilu.h"
#include "IL\ilut.h"

	using namespace std;

#undef clamp

#ifdef LoadBitmap
    #undef LoadBitmap
#endif

#ifdef LoadImage
    #undef LoadImage
#endif

	class CLUDRAW_API COGLImage
	{
	public:

		typedef unsigned char uchar;
		struct TBGRAPixel;

		struct TRGBAPixel
		{
			unsigned char c[4];

			TRGBAPixel() { c[0] = 0; c[1] = 0; c[2] = 0; c[3] = 0; }
			TRGBAPixel& Set(uchar r, uchar g, uchar b, uchar a)
			{ c[0] = r; c[1] = g; c[2] = b; c[3] = a; return *this; }

			void operator=(TBGRAPixel& rIPt)
			{
				c[0] = rIPt.c[2];
				c[1] = rIPt.c[1];
				c[2] = rIPt.c[0];
				c[3] = rIPt.c[3];
			}

			bool operator==(const TRGBAPixel& rIPt)
			{
				if ((c[0] == rIPt.c[0]) &&
				    (c[1] == rIPt.c[1]) &&
				    (c[2] == rIPt.c[2]))
				{
					return true;
				}

				return false;
			}

			void InvertRGB()
			{
				c[0] = 255 - c[0];
				c[1] = 255 - c[1];
				c[2] = 255 - c[2];
			}

			uchar Luminance()
			{
				double dVal = 0.299 * double(c[0]) + 0.587 * double(c[1]) + 0.144 * double(c[2]);

				if (dVal > 255.0)
				{
					return 255;
				}
				else
				{
					return uchar(dVal);
				}
			}
		};

		struct TBGRAPixel
		{
			unsigned char c[4];

			TBGRAPixel() { c[0] = 0; c[1] = 0; c[2] = 0; c[3] = 0; }
			TBGRAPixel& Set(uchar b, uchar g, uchar r, uchar a)
			{ c[0] = b; c[1] = g; c[2] = r; c[3] = a; return *this; }
		};

		template<class TDataType, class TFloat >
		inline static void Float2Pixel(TDataType& rPx, TFloat fVal);

		template<class TDataType, class TFloat >
		static void Float2Pixel(void* pPx, void* pVal)
		{
			Float2Pixel<TDataType, TFloat>(*((TDataType*) pPx), *((TFloat*) pVal));
		}

		template<class TDataType, class TFloat >
		inline static void Pixel2Float(TFloat& fVal, TDataType rPx);

		template<class TDataType, class TFloat >
		static void Pixel2Float(void* pVal, void* pPx)
		{
			Pixel2Float<TDataType, TFloat>(*((TFloat*) pVal), *((TDataType*) pPx));
		}

		template<class TDataType>
		inline static TDataType PixelBitAND(TDataType xL, TDataType xR);

		template<class TDataType>
		inline static TDataType PixelBitOR(TDataType xL, TDataType xR);

		/// Negate Pixel
		template<class TDataType>
		inline static TDataType PixelNeg(TDataType xR);

		template<class TDataType>
		struct TValueRange
		{
			TValueRange()
			{
				tMin = tMax = TDataType(0);
				dMin = dMax = 0.0;
			}

			TDataType Clamp(const TDataType& tValue)
			{
				if (tValue < tMin) { return tMin; }
				if (tValue > tMax) { return tMax; }
				return tValue;
			}

			TDataType tMin, tMax;
			double dMin, dMax;
		};

		template<class TDataType, unsigned int tuColorCnt, unsigned int tuAlphaCnt, int tiR, int tiG, int tiB, int tiA >
		struct TPixel
		{
			typedef TDataType TType;

			static const unsigned uColorCnt = tuColorCnt;
			static const unsigned uAlphaCnt = tuAlphaCnt;
			static const int iRedIdx        = tiR;
			static const int iGreenIdx      = tiG;
			static const int iBlueIdx       = tiB;
			static const int iAlphaIdx      = tiA;

			static const unsigned uSize      = (tuColorCnt + tuAlphaCnt) * sizeof(TDataType);
			static const unsigned uColorSize = tuColorCnt * sizeof(TDataType);
			static const unsigned uAlphaSize = tuAlphaCnt * sizeof(TDataType);

			TDataType value[tuColorCnt + tuAlphaCnt];

			TDataType& r() { return value[tiR]; }
			TDataType& g() { return value[tiG]; }
			TDataType& b() { return value[tiB]; }
			TDataType& a() { return value[tiA]; }

			const TDataType& r() const { return value[tiR]; }
			const TDataType& g() const { return value[tiG]; }
			const TDataType& b() const { return value[tiB]; }
			const TDataType& a() const { return value[tiA]; }

			//static int iRedIdx  { return tiR; }
			//static int iGreenIdx  { return tiG; }
			//static int iBlueIdx  { return tiB; }
			//static int iAlphaIdx  { return tiA; }

			//static unsigned int uColorCnt  { return tuColorCnt; }
			//static unsigned int uAlphaCnt  { return tuAlphaCnt; }

			//static unsigned int Size()  { return (tuColorCnt + tuAlphaCnt)*sizeof(TDataType); }
			//static unsigned int ColorSize()  { return tuColorCnt*sizeof(TDataType); }
			//static unsigned int AlphaSize()  { return tuAlphaCnt*sizeof(TDataType); }
		};

		typedef TPixel<char, 3, 1, 0, 1, 2, 3> TPixel_RGBA_c;
		typedef TPixel<char, 3, 1, 2, 1, 0, 3> TPixel_BGRA_c;
		typedef TPixel<char, 3, 0, 0, 1, 2, 0> TPixel_RGB_c;
		typedef TPixel<char, 3, 0, 2, 1, 0, 0> TPixel_BGR_c;
		typedef TPixel<char, 1, 0, 0, 0, 0, 0> TPixel_L_c;
		typedef TPixel<char, 1, 1, 0, 0, 0, 1> TPixel_LA_c;

		typedef TPixel<unsigned char, 3, 1, 0, 1, 2, 3> TPixel_RGBA_uc;
		typedef TPixel<unsigned char, 3, 1, 2, 1, 0, 3> TPixel_BGRA_uc;
		typedef TPixel<unsigned char, 3, 0, 0, 1, 2, 0> TPixel_RGB_uc;
		typedef TPixel<unsigned char, 3, 0, 2, 1, 0, 0> TPixel_BGR_uc;
		typedef TPixel<unsigned char, 1, 0, 0, 0, 0, 0> TPixel_L_uc;
		typedef TPixel<unsigned char, 1, 1, 0, 0, 0, 1> TPixel_LA_uc;

		typedef TPixel<__int16, 3, 1, 0, 1, 2, 3> TPixel_RGBA_s;
		typedef TPixel<__int16, 3, 1, 2, 1, 0, 3> TPixel_BGRA_s;
		typedef TPixel<__int16, 3, 0, 0, 1, 2, 0> TPixel_RGB_s;
		typedef TPixel<__int16, 3, 0, 2, 1, 0, 0> TPixel_BGR_s;
		typedef TPixel<__int16, 1, 0, 0, 0, 0, 0> TPixel_L_s;
		typedef TPixel<__int16, 1, 1, 0, 0, 0, 1> TPixel_LA_s;

		typedef TPixel<unsigned __int16, 3, 1, 0, 1, 2, 3> TPixel_RGBA_us;
		typedef TPixel<unsigned __int16, 3, 1, 2, 1, 0, 3> TPixel_BGRA_us;
		typedef TPixel<unsigned __int16, 3, 0, 0, 1, 2, 0> TPixel_RGB_us;
		typedef TPixel<unsigned __int16, 3, 0, 2, 1, 0, 0> TPixel_BGR_us;
		typedef TPixel<unsigned __int16, 1, 0, 0, 0, 0, 0> TPixel_L_us;
		typedef TPixel<unsigned __int16, 1, 1, 0, 0, 0, 1> TPixel_LA_us;

		typedef TPixel<__int32, 3, 1, 0, 1, 2, 3> TPixel_RGBA_i;
		typedef TPixel<__int32, 3, 1, 2, 1, 0, 3> TPixel_BGRA_i;
		typedef TPixel<__int32, 3, 0, 0, 1, 2, 0> TPixel_RGB_i;
		typedef TPixel<__int32, 3, 0, 2, 1, 0, 0> TPixel_BGR_i;
		typedef TPixel<__int32, 1, 0, 0, 0, 0, 0> TPixel_L_i;
		typedef TPixel<__int32, 1, 1, 0, 0, 0, 1> TPixel_LA_i;

		typedef TPixel<unsigned __int32, 3, 1, 0, 1, 2, 3> TPixel_RGBA_ui;
		typedef TPixel<unsigned __int32, 3, 1, 2, 1, 0, 3> TPixel_BGRA_ui;
		typedef TPixel<unsigned __int32, 3, 0, 0, 1, 2, 0> TPixel_RGB_ui;
		typedef TPixel<unsigned __int32, 3, 0, 2, 1, 0, 0> TPixel_BGR_ui;
		typedef TPixel<unsigned __int32, 1, 0, 0, 0, 0, 0> TPixel_L_ui;
		typedef TPixel<unsigned __int32, 1, 1, 0, 0, 0, 1> TPixel_LA_ui;

		typedef TPixel<float, 3, 1, 0, 1, 2, 3> TPixel_RGBA_f;
		typedef TPixel<float, 3, 1, 2, 1, 0, 3> TPixel_BGRA_f;
		typedef TPixel<float, 3, 0, 0, 1, 2, 0> TPixel_RGB_f;
		typedef TPixel<float, 3, 0, 2, 1, 0, 0> TPixel_BGR_f;
		typedef TPixel<float, 1, 0, 0, 0, 0, 0> TPixel_L_f;
		typedef TPixel<float, 1, 1, 0, 0, 0, 1> TPixel_LA_f;

		typedef TPixel<double, 3, 1, 0, 1, 2, 3> TPixel_RGBA_d;
		typedef TPixel<double, 3, 1, 2, 1, 0, 3> TPixel_BGRA_d;
		typedef TPixel<double, 3, 0, 0, 1, 2, 0> TPixel_RGB_d;
		typedef TPixel<double, 3, 0, 2, 1, 0, 0> TPixel_BGR_d;
		typedef TPixel<double, 1, 0, 0, 0, 0, 0> TPixel_L_d;
		typedef TPixel<double, 1, 1, 0, 0, 0, 1> TPixel_LA_d;

		typedef void (* TFuncPtr_Pixel2Float)(void*, void*);
		typedef void (* TFuncPtr_Float2Pixel)(void*, void*);

		// Allows access to image of arbitrary pixel type
		struct TPixelFilter
		{
			TPixelFilter()
			{
				iColorCnt  = iAlphaCnt = 0;
				iR         = iG = iB = iA = 0;
				iTotalSize = iColorSize = iAlphaSize = iElSize = 0;
				pData      = 0;
			}

			template<class TPixelType>
			TPixelFilter(TPixelType* pFirstPixel)
			{
				iColorCnt = TPixelType::uColorCnt;
				iAlphaCnt = TPixelType::uAlphaCnt;

				iR = TPixelType::iRedIdx;
				iG = TPixelType::iGreenIdx;
				iB = TPixelType::iBlueIdx;
				iA = TPixelType::iAlphaIdx;

				iTotalSize = TPixelType::Size();
				iColorSize = TPixelType::ColorSize();
				iAlphaSize = TPixelType::AlphaSize();
				iElSize    = sizeof(TPixelType::TType);

				pData = (void*) pFirstPixel;

				pfPixel2Float = Pixel2Float<TPixelType::TType, double>;
				pfFloat2Pixel = Float2Pixel<TPixelType::TType, double>;
			}

			TFuncPtr_Pixel2Float pfPixel2Float;
			TFuncPtr_Float2Pixel pfFloat2Pixel;

			int iColorCnt, iAlphaCnt;
			int iR, iG, iB, iA;
			int iTotalSize;	// Total size of pixel in bytes
			int iColorSize;	// Total size of color part in bytes
			int iAlphaSize;	// Total size of alpha part in bytes
			int iElSize;	// Size of a single element of pixel in bytes;

			void* pData;
		};

		enum ETransparencyMode
		{
			EXACT = 0,
			LUMINANCE,
			FONT
		};

	public:

		COGLImage(void);
		COGLImage(const COGLImage& OGLImage);
		~COGLImage(void);

		COGLImage& operator=(const COGLImage& OGLImage);

		// Crate image with given size and type
		bool Create(int iWidth, int iHeight, int iImgType, int iDataType);

		// Copy given data to this image, clearing current image
		bool CopyImage(int iWidth, int iHeight, int iImgType, int iDataType, const void* pvData);

		// Clear Image
		bool ClearImage(COGLColor* pcolClear = 0);

		// Convert Image Type
		bool ConvertType(int iImageType, int iDataType);

		// Extract given sub-Image
		bool ExtractSubImage(COGLImage& rSrcImg, int iX, int iY, int iCX, int iCY);

		// Insert sub-Image
		bool InsertSubImage(COGLImage& rSrcImg, int iSrcX, int iSrcY, int iSrcW, int iSrcH, int iTrgX, int iTrgY);

		Clu::CIImage ToIImage();
		void Create(const Clu::CIImage& xImage);

		void GetFilename(CStrMem& csFilename) const
		{ csFilename = m_csFilename; }

		void GetSize(int& iWidth, int& iHeight) const
		{ iWidth = m_iWidth; iHeight = m_iHeight; }

		void GetType(int& iImgType, int& iDataType, int& iBytesPerPixel) const;
		void GetComponentCount(int& iColorCnt, int& iAlphaCnt) const;

		bool GetPixel(int iX, int iY, double& fR, double& fG, double& fB, double& fA);
		bool GetPixelNormalLum(int iX, int iY, double& dX, double& dY, double& dZ);
		bool GetPixelMaxLum(int piX[4], int piY[4], double pdCol[4], double dPart);
		bool GetPixelMinLum(int piX[4], int piY[4], double pdCol[4], double dPart);

		// Multiply image with scalar
		bool OpMultScalar(COGLImage& rSrcImg, double dFactor);
		// Take reciprocal of each pixel and multiply with scalar
		bool OpRecipScalar(COGLImage& rSrcImg, double dFactor);
		// Add scalar to image
		bool OpAddScalar(COGLImage& rSrcImg, double dValue);
		// Negate image and add scalar
		bool OpNegAddScalar(COGLImage& rSrcImg, double dValue);

		// Multiply image with color
		bool OpMultColor(COGLImage& rSrcImg, const COGLColor& colVal);
		// Take reciprocal of each pixel and multiply with scalar
		bool OpRecipColor(COGLImage& rSrcImg, const COGLColor& colVal);
		// Add scalar to image
		bool OpAddColor(COGLImage& rSrcImg, const COGLColor& colVal);
		// Negate image and add scalar
		bool OpNegAddColor(COGLImage& rSrcImg, const COGLColor& colVal);

		// Negate image
		bool OpNeg(COGLImage& rSrcImg);

		// Divide images
		bool OpDivImage(COGLImage& rResult, const COGLImage& rImage);
		// Multiply images
		bool OpMultImage(COGLImage& rResult, const COGLImage& rImage);
		// Add images
		bool OpAddImage(COGLImage& rResult, const COGLImage& rImage);
		// Subtract images
		bool OpSubImage(COGLImage& rResult, const COGLImage& rImage);

		bool SetSize(int iWidth, int iHeight, COGLColor* pcolClear = 0);
		bool ResizeImage(int iWidth, int iHeight);
		bool ResizeCanvas(int iWidth, int iHeight, COGLColor* pcolClear = 0, double dAlignX = 0.5, double dAlignY = 0.5);
		// Add a single pixel border. Color is given by nearest pixel in image.
		bool AddBorder();
		bool RotateImage(float fAngle);
		bool Rotate90Image(int iSteps);
		bool FlipImage(bool bHorizontal);	// If bHorizontal == false, flips vertically.
		bool InsertImage(COGLImage& rSrcImg, int iX, int iY, int iTrgC = -1, int iSrcC = -1);
		bool GetBoundingBox(Mem<int>& mBox, COGLColor& rBGColor);
		bool MakeTransparent(ETransparencyMode eMode, COGLColor& rTransColor);

		bool MaskEqual(COGLImage& rMask, COGLColor& rCol);
		bool MaskNotEqual(COGLImage& rMask, COGLColor& rCol);
		bool MaskEqual(COGLImage& rMask, const COGLImage& rImage);
		bool MaskNotEqual(COGLImage& rMask, const COGLImage& rImage);

		bool MaskLogicAND(COGLImage& rMask, const COGLImage& rImage);
		bool MaskLogicOR(COGLImage& rMask, const COGLImage& rImage);

		bool ImageBitAND(COGLImage& rMask, const COGLImage& rImage);
		bool ImageBitOR(COGLImage& rMask, const COGLImage& rImage);

		void SetFilename(const char* pcText);

		// Set Image Origin
		void SetOrigin(int iX, int iY)
		{
			if ((iX >= 0) && (iX < m_iWidth)) { m_iOrigX = iX; }
			if ((iY >= 0) && (iY < m_iHeight)) { m_iOrigY = iY; }
		}

		// Get Image origin
		void GetOrigin(int& iX, int& iY)
		{ iX = m_iOrigX; iY = m_iOrigY; }

		// Set all rgb values to given color
		void FlushRGB(COGLColor& rCol);

		// Generate OpenGL texture
		uint GenTexture();

		// If bForce == false, then bitmap is only loaded
		// if its filename is different to that of the
		// currently loaded bitmap. Otherwise, it is always loaded.
		// If iImgType and iDataType are set, the loaded image is converted
		// directly to this type.
		bool LoadImage(bool bForce = false, int iImgType = -1, int iDataType = -1);

		// If pcFilename == 0, then internal name is used.
		bool SaveImage(const char* pcFilename = 0);

		// Return a pointer to the data
		uchar* GetDataPtr();
		const uchar* GetDataPtr() const;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Sample an image area.
		/// </summary>
		///
		/// <param name="fPosX">			 The position x coordinate. </param>
		/// <param name="fPosY">			 The position y coordinate. </param>
		/// <param name="fAreaSize">		 Size of the area. </param>
		/// <param name="pucSampleType">	 [in,out] If non-null, the puc sample type. </param>
		/// <param name="pucSampleAreaType"> [in,out] If non-null, the puc sample area type. </param>
		/// <param name="fValueToIgnore">    The value to ignore. </param>
		///
		/// <returns> . </returns>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		std::vector<float> SampleArea(float fPosX, float fPosY, float fAreaSize, char* pucSampleType, char* pucSampleAreaType, float fValueToIgnore);

	protected:

		void ResetVars();
		void ResetPixelIdxLum();
		void ReversePixelIdxLum();

		template<class TFloat>
		static TFloat NumberValueMax() { }

		template<> static double NumberValueMax<double>() { return DBL_MAX; }
		template<> static float NumberValueMax<float>() { return FLT_MAX; }

		template<class TFloat>
		static TFloat Clamp(TFloat dValue) { }

		template<> static double Clamp(double dValue)
		{
			if (dValue < 0.0) { return 0.0; }
			if (dValue > 1.0) { return 1.0; }
			return dValue;
		}

		template<> static float Clamp(float dValue)
		{
			if (dValue < 0.0f) { return 0.0f; }
			if (dValue > 1.0f) { return 1.0f; }
			return dValue;
		}

		struct PGetSubImg
		{
			PGetSubImg()
			{ iSrcWidth = 0; iSrcHeight = 0; iX = 0; iY = 0; iCX = 0; iCY = 0; }

			PGetSubImg(int _iSrcWidth, int _iSrcHeight, int _iX, int _iY, int _iCX, int _iCY)
			{ iSrcWidth = _iSrcWidth; iSrcHeight = _iSrcHeight; iX = _iX; iY = _iY; iCX = _iCX; iCY = _iCY; }

			int iSrcWidth, iSrcHeight;
			int iX, iY;
			int iCX, iCY;
		};

		template< class TPxL, class TPxR >
		class FGetSubImg
		{
		public:

			static bool Execute(TPxL* pTrg, TPxL* pSrc, const PGetSubImg& rPar);
		};

		struct PInsSubImg
		{
			PInsSubImg()
			{
				iSrcWidth  = 0;
				iSrcHeight = 0;
				iTrgWidth  = 0;
				iTrgHeight = 0;
				iSrcX      = 0;
				iSrcY      = 0;
				iTrgX      = 0;
				iTrgY      = 0;
				iSrcW      = 0;
				iSrcH      = 0;
			}

			PInsSubImg(int _iSrcWidth, int _iSrcHeight, int _iTrgWidth, int _iTrgHeight, int _iSrcX, int _iSrcY, int _iSrcW, int _iSrcH, int _iTrgX, int _iTrgY)
			{
				iSrcWidth  = _iSrcWidth;
				iSrcHeight = _iSrcHeight;
				iTrgWidth  = _iTrgWidth;
				iTrgHeight = _iTrgHeight;
				iSrcX      = _iSrcX;
				iSrcY      = _iSrcY;
				iTrgX      = _iTrgX;
				iTrgY      = _iTrgY;
				iSrcW      = _iSrcW;
				iSrcH      = _iSrcH;
			}

			int iSrcWidth, iSrcHeight;
			int iTrgWidth, iTrgHeight;
			int iSrcX, iSrcY;
			int iSrcW, iSrcH;
			int iTrgX, iTrgY;
		};

		template< class TPxL, class TPxR >
		class FInsSubImg
		{
		public:

			static bool Execute(TPxL* pTrg, TPxL* pSrc, const PInsSubImg& rPar);
		};

		struct PInsChanImg
		{
			PInsChanImg()
			{
				iSrcWidth  = 0;
				iSrcHeight = 0;
				iTrgWidth  = 0;
				iTrgHeight = 0;
				iTrgX      = 0;
				iTrgY      = 0;
				iSrcC      = 0;
				iTrgC      = 0;
			}

			PInsChanImg(int _iSrcWidth, int _iSrcHeight, int _iTrgWidth, int _iTrgHeight, int _iTrgX, int _iTrgY, int _iSrcC, int _iTrgC)
			{
				iSrcWidth  = _iSrcWidth;
				iSrcHeight = _iSrcHeight;
				iTrgWidth  = _iTrgWidth;
				iTrgHeight = _iTrgHeight;
				iTrgX      = _iTrgX;
				iTrgY      = _iTrgY;
				iSrcC      = _iSrcC;
				iTrgC      = _iTrgC;
			}

			int iSrcWidth, iSrcHeight;
			int iTrgWidth, iTrgHeight;
			int iTrgX, iTrgY;
			int iSrcC, iTrgC;
		};

		template< class TPxL, class TPxR >
		class FInsChanImg
		{
		public:

			static bool Execute(TPxL* pTrg, TPxL* pSrc, const PInsChanImg& rPar);
		};

		struct PRot90Img
		{
			PRot90Img()
			{
				iSrcWidth  = 0;
				iSrcHeight = 0;
				iTrgWidth  = 0;
				iTrgHeight = 0;
				iSteps     = 0;
			}

			PRot90Img(int _iSrcWidth, int _iSrcHeight, int _iTrgWidth, int _iTrgHeight, int _iSteps)
			{
				iSrcWidth  = _iSrcWidth;
				iSrcHeight = _iSrcHeight;
				iTrgWidth  = _iTrgWidth;
				iTrgHeight = _iTrgHeight;
				iSteps     = _iSteps;
			}

			int iSrcWidth, iSrcHeight;
			int iTrgWidth, iTrgHeight;
			int iSteps;
		};

		template< class TPxL, class TPxR >
		class FRot90Img
		{
		public:

			static bool Execute(TPxL* pTrg, TPxL* pSrc, const PRot90Img& rPar);
		};

		struct PMaskEqualCol
		{
			PMaskEqualCol()
			{ iPixelCount = 0; pfCol[0] = 0.0f; pfCol[1] = 0.0f; pfCol[2] = 0.0f; }

			PMaskEqualCol(int _iPixelCount, COGLColor& rCol)
			{
				iPixelCount = _iPixelCount;
				pfCol[0]    = rCol[0];
				pfCol[1]    = rCol[1];
				pfCol[2]    = rCol[2];
			}

			int iPixelCount;
			float pfCol[3];
		};

		template< class TPxL, class TPxR >
		class FMaskEqualCol
		{
		public:

			static bool Execute(TPxL* pTrg, TPxR* pSrc, const PMaskEqualCol& rPar);
		};

		template< class TPxL, class TPxR >
		class FMaskNotEqualCol
		{
		public:

			static bool Execute(TPxL* pTrg, TPxR* pSrc, const PMaskEqualCol& rPar);
		};

		struct PPixCnt
		{
			PPixCnt()
			{ iPixelCount = 0; }

			PPixCnt(int _iPixelCount)
			{
				iPixelCount = _iPixelCount;
			}

			int iPixelCount;
		};

		template< class TPxL, class TPxR1, class TPxR2 >
		class FMaskEqualImg
		{
		public:

			static bool Execute(TPxL* pTrg, TPxR1* pSrc1, TPxR2* pSrc2, const PPixCnt& rPar);
		};

		template< class TPxL, class TPxR1, class TPxR2 >
		class FMaskNotEqualImg
		{
		public:

			static bool Execute(TPxL* pTrg, TPxR1* pSrc1, TPxR2* pSrc2, const PPixCnt& rPar);
		};

		template< class TPxL, class TPxR1, class TPxR2 >
		class FMaskLogicANDImg
		{
		public:

			static bool Execute(TPxL* pTrg, TPxR1* pSrc1, TPxR2* pSrc2, const PPixCnt& rPar);
		};

		template< class TPxL, class TPxR1, class TPxR2 >
		class FMaskLogicORImg
		{
		public:

			static bool Execute(TPxL* pTrg, TPxR1* pSrc1, TPxR2* pSrc2, const PPixCnt& rPar);
		};

		template< class TPxL, class TPxR1, class TPxR2 >
		class FMaskBitANDImg
		{
		public:

			static bool Execute(TPxL* pTrg, TPxR1* pSrc1, TPxR2* pSrc2, const PPixCnt& rPar);
		};

		template< class TPxL, class TPxR1, class TPxR2 >
		class FMaskBitORImg
		{
		public:

			static bool Execute(TPxL* pTrg, TPxR1* pSrc1, TPxR2* pSrc2, const PPixCnt& rPar);
		};

		template< class TPxL, class TPxR1, class TPxR2 >
		class FOpMultImg
		{
		public:

			static bool Execute(TPxL* pTrg, TPxR1* pSrc1, TPxR2* pSrc2, const PPixCnt& rPar);
		};

		template< class TPxL, class TPxR1, class TPxR2 >
		class FOpDivImg
		{
		public:

			static bool Execute(TPxL* pTrg, TPxR1* pSrc1, TPxR2* pSrc2, const PPixCnt& rPar);
		};

		template< class TPxL, class TPxR1, class TPxR2 >
		class FOpAddImg
		{
		public:

			static bool Execute(TPxL* pTrg, TPxR1* pSrc1, TPxR2* pSrc2, const PPixCnt& rPar);
		};

		template< class TPxL, class TPxR1, class TPxR2 >
		class FOpSubImg
		{
		public:

			static bool Execute(TPxL* pTrg, TPxR1* pSrc1, TPxR2* pSrc2, const PPixCnt& rPar);
		};

		struct PMakeTrans
		{
			PMakeTrans()
			{
				iPixelCount = 0;
				pfCol[0]    = 0.0f;
				pfCol[1]    = 0.0f;
				pfCol[2]    = 0.0f;
				eMode       = COGLImage::EXACT;
			}

			PMakeTrans(int _iPixelCount, COGLColor& rCol, COGLImage::ETransparencyMode _eMode)
			{
				iPixelCount = _iPixelCount;
				pfCol[0]    = rCol[0];
				pfCol[1]    = rCol[1];
				pfCol[2]    = rCol[2];

				eMode = _eMode;
			}

			int iPixelCount;
			float pfCol[3];
			COGLImage::ETransparencyMode eMode;
		};

		template< class TPxL >
		class FMakeTrans
		{
		public:

			static bool Execute(TPxL* pTrg, const PMakeTrans& rPar);
		};

		struct PFlushRGB
		{
			PFlushRGB()
			{ iPixelCount = 0; pfCol[0] = 0.0f; pfCol[1] = 0.0f; pfCol[2] = 0.0f; }

			PFlushRGB(int _iPixelCount, COGLColor& rCol)
			{
				iPixelCount = _iPixelCount;
				pfCol[0]    = rCol[0];
				pfCol[1]    = rCol[1];
				pfCol[2]    = rCol[2];
			}

			int iPixelCount;
			float pfCol[3];
		};

		template< class TPxL >
		class FFlushRGB
		{
		public:

			static bool Execute(TPxL* pTrg, const PFlushRGB& rPar);
		};

		struct PGetBoundBox
		{
			PGetBoundBox()
			{
				iWidth   = 0;
				iHeight  = 0;
				pfCol[0] = 0.0f;
				pfCol[1] = 0.0f;
				pfCol[2] = 0.0f;
				for (int i = 0; i < 4; ++i) { piBox[i] = 0; }
			}

			PGetBoundBox(int _iWidth, int _iHeight, COGLColor& rCol)
			{
				iWidth   = _iWidth;
				iHeight  = _iHeight;
				pfCol[0] = rCol[0];
				pfCol[1] = rCol[1];
				pfCol[2] = rCol[2];
				for (int i = 0; i < 4; ++i) { piBox[i] = 0; }
			}

			int iWidth, iHeight;
			float pfCol[3];
			int piBox[4];
		};

		template< class TPxL >
		class FGetBoundBox
		{
		public:

			static bool Execute(TPxL* pTrg, PGetBoundBox& rPar);
		};

		////////////////////////////////////////////////////////////////
		/// Get Pixel Color
		struct PGetPixelColor
		{
			PGetPixelColor(int _iX, int _iY, int _iWidth, int _iHeight)
			{
				iX      = _iX;
				iY      = _iY;
				iWidth  = _iWidth;
				iHeight = _iHeight;
				dR      = dG = dB = dA = 0.0;
			}

			int iX, iY;
			int iWidth, iHeight;
			double dR, dG, dB, dA;
		};

		template< class TPxL >
		class FGetPixelColor
		{
		public:

			static bool Execute(TPxL* pTrg, PGetPixelColor& rPar);
		};

		////////////////////////////////////////////////////////////////
		/// Get Pixel Normal
		///
		/// Takes pixel luminance as z-values and estimates the local normal

		struct PGetPixelNormalLum
		{
			PGetPixelNormalLum(int _iX, int _iY, int _iWidth, int _iHeight)
			{
				iX      = _iX;
				iY      = _iY;
				iWidth  = _iWidth;
				iHeight = _iHeight;
				dX      = dY = dZ = 0.0;
			}

			int iX, iY;
			int iWidth, iHeight;
			double dX, dY, dZ;
		};

		template< class TPxL >
		class FGetPixelNormalLum
		{
		public:

			static bool Execute(TPxL* pTrg, PGetPixelNormalLum& rPar);
		};

		////////////////////////////////////////////////////////////////
		// Pixel Luminance Index structure for sorting pixel

		struct SPixelLumIdx
		{
			double dLum;
			unsigned uIdx;
		};

		////////////////////////////////////////////////////////////////
		// Pixel Luminance Sort

		template< class TPxL, int iChannel >
		static bool PixelChannelGreater(void* pPix1, void* pPix2)
		{
			return pPix1->value[iChannel] > pPix2->value[iChannel];
		}

		template< class TPxL, int iChannel >
		static bool PixelChannelSmaller(void* pPix1, void* pPix2)
		{
			return pPix1->value[iChannel] < pPix2->value[iChannel];
		}

		template< class TPxL, int iChannel >
		struct SPixelChanGreater
		{
			SPixelChanGreater(TPxL* _pData)
			{ pData = _pData; }

			bool operator()(unsigned uIdx1, unsigned uIdx2)
			{
				return pData[uIdx1].value[iChannel] > pData[uIdx2].value[iChannel];
			}

			TPxL* pData;
		};

		template< class TPxL, int iChannel >
		struct SPixelChanSmaller
		{
			SPixelChanSmaller(TPxL* _pData)
			{ pData = _pData; }

			bool operator()(unsigned uIdx1, unsigned uIdx2)
			{
				return pData[uIdx1].value[iChannel] < pData[uIdx2].value[iChannel];
			}

			TPxL* pData;
		};

		////////////////////////////////////////////////////////////////
		/// Get Pixel with Maximal Luminance
		struct PGetPixelMinMaxLum
		{
			// dPart gives percentage of pixel of which maximum is to be calculated
			// For example, of dPart = 0.99 only the lower 99% off all pixels are
			// used to estimate the maximum.
			PGetPixelMinMaxLum(double _dPart, vector<unsigned>* _pvecIdx, int _iWidth, int _iHeight)
			{
				iWidth  = _iWidth;
				iHeight = _iHeight;
				dPart   = _dPart;

				for (int i = 0; i < 4; ++i)
				{
					pvecIdx[i] = &(_pvecIdx[i]);
					piX[i]     = 0;
					piY[i]     = 0;
					pdCol[i]   = 0;
				}
			}

			int piX[4], piY[4];
			int iWidth, iHeight;
			double dPart;
			double pdCol[4];
			bool bIdxValid;
			vector<unsigned>* pvecIdx[4];
		};

		template< class TPxL >
		class FGetPixelMaxLum
		{
		public:

			static bool Execute(TPxL* pTrg, PGetPixelMinMaxLum& rPar);
		};

		////////////////////////////////////////////////////////////////
		/// Get Pixel with Minimal Luminance

		template< class TPxL >
		class FGetPixelMinLum
		{
		public:

			static bool Execute(TPxL* pTrg, PGetPixelMinMaxLum& rPar);
		};

		////////////////////////////////////////////////////////////////
		/// Scalar Parameter

		struct POpScalar
		{
			POpScalar()
			{
				iWidth  = 0;
				iHeight = 0;
				dValue  = 1.0;
			}

			POpScalar(double _dValue, int _iWidth, int _iHeight)
			{
				dValue  = _dValue;
				iWidth  = _iWidth;
				iHeight = _iHeight;
			}

			int iWidth, iHeight;
			double dValue;
		};

		////////////////////////////////////////////////////////////////
		/// Color Parameter

		struct POpColor
		{
			POpColor()
			{
				iWidth  = 0;
				iHeight = 0;
				colValue.Set(0.0f, 0.0f, 0.0f, 0.0f);
			}

			POpColor(const COGLColor& _colValue, int _iWidth, int _iHeight)
			{
				colValue = _colValue;
				iWidth   = _iWidth;
				iHeight  = _iHeight;
			}

			int iWidth, iHeight;
			COGLColor colValue;
		};

		////////////////////////////////////////////////////////////////
		/// Multiply Image with scalar value
		template< class TPxL, class TPxR >
		class FOpMultScalar
		{
		public:

			static bool Execute(TPxL* pTrg, TPxL* pSrc, const POpScalar& rPar);
		};

		////////////////////////////////////////////////////////////////
		/// Take reciprocal of each pixel value and multiply with factor
		template< class TPxL, class TPxR >
		class FOpRecipScalar
		{
		public:

			static bool Execute(TPxL* pTrg, TPxL* pSrc, const POpScalar& rPar);
		};

		////////////////////////////////////////////////////////////////
		/// Add scalar value to Image
		template< class TPxL, class TPxR >
		class FOpAddScalar
		{
		public:

			static bool Execute(TPxL* pTrg, TPxL* pSrc, const POpScalar& rPar);
		};

		////////////////////////////////////////////////////////////////
		/// Negate image and add scalar value
		template< class TPxL, class TPxR >
		class FOpNegAddScalar
		{
		public:

			static bool Execute(TPxL* pTrg, TPxL* pSrc, const POpScalar& rPar);
		};

		////////////////////////////////////////////////////////////////
		/// Multiply Image with color value
		template< class TPxL, class TPxR >
		class FOpMultColor
		{
		public:

			static bool Execute(TPxL* pTrg, TPxL* pSrc, const POpColor& rPar);
		};

		////////////////////////////////////////////////////////////////
		/// Take reciprocal of each pixel value and multiply with color
		template< class TPxL, class TPxR >
		class FOpRecipColor
		{
		public:

			static bool Execute(TPxL* pTrg, TPxL* pSrc, const POpColor& rPar);
		};

		////////////////////////////////////////////////////////////////
		/// Add color value to Image
		template< class TPxL, class TPxR >
		class FOpAddColor
		{
		public:

			static bool Execute(TPxL* pTrg, TPxL* pSrc, const POpColor& rPar);
		};

		////////////////////////////////////////////////////////////////
		/// Negate image and add color value
		template< class TPxL, class TPxR >
		class FOpNegAddColor
		{
		public:

			static bool Execute(TPxL* pTrg, TPxL* pSrc, const POpColor& rPar);
		};

		////////////////////////////////////////////////////////////////
		/// Negate Image

		template< class TPxL, class TPxR  >
		class FOpNeg
		{
		public:

			static bool Execute(TPxL* pTrg, TPxR* pSrc, const PPixCnt& rPar);
		};

		////////////////////////////////////////////////////////////////
		/// Function execution

		template< template<class TPxL, class TPxR> class Functor, class Parameter >
		bool ExecutePixelOperator_EqualType(COGLImage& rTrg, COGLImage& rSrc, Parameter& rPar);

		template< template<class TPxL, class TPxR1, class TPxR2> class Functor, class Parameter >
		bool ExecutePixelOperator_EqualType(COGLImage& rTrg, COGLImage& rSrc1, const COGLImage& rSrc2, Parameter& rPar);

		// All images must have same data type. rTrg and rSrc1 must have same image type.
		template< template<class TPxL, class TPxR1, class TPxR2> class Functor, class Parameter >
		bool ExecutePixelOperator_EqualDataType112(COGLImage& rTrg, const COGLImage& rSrc1, const COGLImage& rSrc2, Parameter& rPar);

		// All images must have same data type. rTrg and rSrc2 must have same image type.
		template< template<class TPxL, class TPxR1, class TPxR2> class Functor, class Parameter >
		bool ExecutePixelOperator_EqualDataType121(COGLImage& rTrg, const COGLImage& rSrc1, const COGLImage& rSrc2, Parameter& rPar);

		template< template<class TPxL> class Functor, class Parameter >
		bool ExecutePixelOperator(COGLImage& rTrg, Parameter& rPar);

		/// Type 112 construct start. TPxL and TPxR1 have to be the same type
		template< template<class TPxL, class TPxR1, class TPxR2> class Functor, class Parameter >
		bool ExecutePixelOperator_Type112(COGLImage& rTrg, const COGLImage& rSrc1, const COGLImage& rSrc2, Parameter& rPar);

		/// Type 112 Level 1
		template< template<class TPxL, class TPxR1, class TPxR2> class Functor, class Parameter, class TPxL, class TPxR1 >
		bool ExecutePixelOperator_Type112_Level1(TPxL* pTrg, TPxR1* pSrc1, const COGLImage& rSrc2, Parameter& rPar);

		/// Type 121 construct start. TPxL and TPxR2 have to be the same type
		template< template<class TPxL, class TPxR1, class TPxR2> class Functor, class Parameter >
		bool ExecutePixelOperator_Type121(COGLImage& rTrg, const COGLImage& rSrc1, const COGLImage& rSrc2, Parameter& rPar);

		/// Type 121 level 1
		template< template<class TPxL, class TPxR1, class TPxR2> class Functor, class Parameter, class TPxL, class TPxR2 >
		bool ExecutePixelOperator_Type121_Level1(TPxL* pTrg, const COGLImage& rSrc1, TPxR2* pSrc1, Parameter& rPar);

	protected:

		CStrMem m_csFilename;
		CStrMem m_csCurFilename;

		int m_iWidth, m_iHeight;
		int m_iOrigX, m_iOrigY;

		// List of pixel indices which are ordered in ascending
		// order of luminance for each channel.
		// Index lists are always ordered as Red, Green, Blue, Alpha
		vector<unsigned> m_pvecPixIdxLum[4];

		ILuint m_uImgID;
	};

#endif
