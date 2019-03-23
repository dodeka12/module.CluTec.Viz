////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLImage_Exec_EqualDataType.cxx
//
// summary:   ogl image execute equal data type class
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

// Define to simplify code in ExecutePixelOperator_EqualType
#define EXECPIXELOP( PXTYPE1, PXTYPE2 ) Functor<PXTYPE1,PXTYPE1,PXTYPE2>::Execute( \
		(PXTYPE1 *) rTrg.GetDataPtr(), (PXTYPE1 *) rSrc1.GetDataPtr(), (PXTYPE2 *) rSrc2.GetDataPtr(), rPar )

// Expect data type of all images to be the same
// Expect rTrg and rSrc1 to be of the same type
template< template<class TPxL, class TPxR1, class TPxR2> class Functor, class Parameter >
bool COGLImage::ExecutePixelOperator_EqualDataType112( COGLImage &rTrg, const COGLImage &rSrc1, const COGLImage &rSrc2, Parameter& rPar )
{
	int iTrgImgType, iTrgDataType, iTrgBytesPerPixel;
	int iSrc1ImgType, iSrc1DataType, iSrc1BytesPerPixel;
	int iSrc2ImgType, iSrc2DataType, iSrc2BytesPerPixel;
	int iImgType;

	rTrg.GetType( iTrgImgType, iTrgDataType, iTrgBytesPerPixel );
	rSrc1.GetType( iSrc1ImgType, iSrc1DataType, iSrc1BytesPerPixel );
	rSrc2.GetType( iSrc2ImgType, iSrc2DataType, iSrc2BytesPerPixel );

	// All data types have to be the same
	// Target and Src1 image type have to be the same
	if ( iTrgImgType != iSrc1ImgType || 
		 iTrgDataType != iSrc1DataType ||
		 iTrgDataType != iSrc2DataType 
		)
		return false;

	iImgType = iSrc2ImgType;

	if ( iTrgDataType == CLUVIZ_IMG_BYTE )
	{
		if ( iTrgImgType == CLUVIZ_IMG_RGBA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGBA_c, TPixel_RGBA_c);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGBA_c, TPixel_RGB_c);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGBA_c, TPixel_BGRA_c);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGBA_c, TPixel_BGR_c);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGBA_c, TPixel_L_c);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGBA_c, TPixel_LA_c);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_RGB )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGB_c, TPixel_RGBA_c);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGB_c, TPixel_RGB_c);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGB_c, TPixel_BGRA_c);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGB_c, TPixel_BGR_c);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGB_c, TPixel_L_c);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGB_c, TPixel_LA_c);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGRA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGRA_c, TPixel_RGBA_c);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGRA_c, TPixel_RGB_c);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGRA_c, TPixel_BGRA_c);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGRA_c, TPixel_BGR_c);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGRA_c, TPixel_L_c);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGRA_c, TPixel_LA_c);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGR )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGR_c, TPixel_RGBA_c);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGR_c, TPixel_RGB_c);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGR_c, TPixel_BGRA_c);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGR_c, TPixel_BGR_c);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGR_c, TPixel_L_c);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGR_c, TPixel_LA_c);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_L_c, TPixel_RGBA_c);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_L_c, TPixel_RGB_c);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_L_c, TPixel_BGRA_c);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_L_c, TPixel_BGR_c);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_L_c, TPixel_L_c);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_L_c, TPixel_LA_c);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_LA_c, TPixel_RGBA_c);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_LA_c, TPixel_RGB_c);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_LA_c, TPixel_BGRA_c);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_LA_c, TPixel_BGR_c);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_LA_c, TPixel_L_c);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_LA_c, TPixel_LA_c);
			}
		}
	}
	else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_BYTE )
	{
		if ( iTrgImgType == CLUVIZ_IMG_RGBA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGBA_uc, TPixel_RGBA_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGBA_uc, TPixel_RGB_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGBA_uc, TPixel_BGRA_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGBA_uc, TPixel_BGR_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGBA_uc, TPixel_L_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGBA_uc, TPixel_LA_uc);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_RGB )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGB_uc, TPixel_RGBA_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGB_uc, TPixel_RGB_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGB_uc, TPixel_BGRA_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGB_uc, TPixel_BGR_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGB_uc, TPixel_L_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGB_uc, TPixel_LA_uc);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGRA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGRA_uc, TPixel_RGBA_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGRA_uc, TPixel_RGB_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGRA_uc, TPixel_BGRA_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGRA_uc, TPixel_BGR_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGRA_uc, TPixel_L_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGRA_uc, TPixel_LA_uc);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGR )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGR_uc, TPixel_RGBA_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGR_uc, TPixel_RGB_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGR_uc, TPixel_BGRA_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGR_uc, TPixel_BGR_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGR_uc, TPixel_L_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGR_uc, TPixel_LA_uc);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_L_uc, TPixel_RGBA_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_L_uc, TPixel_RGB_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_L_uc, TPixel_BGRA_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_L_uc, TPixel_BGR_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_L_uc, TPixel_L_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_L_uc, TPixel_LA_uc);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_LA_uc, TPixel_RGBA_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_LA_uc, TPixel_RGB_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_LA_uc, TPixel_BGRA_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_LA_uc, TPixel_BGR_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_LA_uc, TPixel_L_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_LA_uc, TPixel_LA_uc);
			}
		}
	}
	else if ( iTrgDataType == CLUVIZ_IMG_SHORT )
	{
		if ( iTrgImgType == CLUVIZ_IMG_RGBA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGBA_s, TPixel_RGBA_s);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGBA_s, TPixel_RGB_s);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGBA_s, TPixel_BGRA_s);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGBA_s, TPixel_BGR_s);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGBA_s, TPixel_L_s);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGBA_s, TPixel_LA_s);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_RGB )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGB_s, TPixel_RGBA_s);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGB_s, TPixel_RGB_s);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGB_s, TPixel_BGRA_s);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGB_s, TPixel_BGR_s);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGB_s, TPixel_L_s);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGB_s, TPixel_LA_s);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGRA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGRA_s, TPixel_RGBA_s);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGRA_s, TPixel_RGB_s);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGRA_s, TPixel_BGRA_s);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGRA_s, TPixel_BGR_s);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGRA_s, TPixel_L_s);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGRA_s, TPixel_LA_s);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGR )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGR_s, TPixel_RGBA_s);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGR_s, TPixel_RGB_s);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGR_s, TPixel_BGRA_s);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGR_s, TPixel_BGR_s);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGR_s, TPixel_L_s);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGR_s, TPixel_LA_s);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_L_s, TPixel_RGBA_s);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_L_s, TPixel_RGB_s);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_L_s, TPixel_BGRA_s);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_L_s, TPixel_BGR_s);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_L_s, TPixel_L_s);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_L_s, TPixel_LA_s);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_LA_s, TPixel_RGBA_s);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_LA_s, TPixel_RGB_s);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_LA_s, TPixel_BGRA_s);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_LA_s, TPixel_BGR_s);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_LA_s, TPixel_L_s);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_LA_s, TPixel_LA_s);
			}
		}
	}
	else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_SHORT )
	{
		if ( iTrgImgType == CLUVIZ_IMG_RGBA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGBA_us, TPixel_RGBA_us);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGBA_us, TPixel_RGB_us);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGBA_us, TPixel_BGRA_us);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGBA_us, TPixel_BGR_us);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGBA_us, TPixel_L_us);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGBA_us, TPixel_LA_us);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_RGB )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGB_us, TPixel_RGBA_us);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGB_us, TPixel_RGB_us);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGB_us, TPixel_BGRA_us);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGB_us, TPixel_BGR_us);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGB_us, TPixel_L_us);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGB_us, TPixel_LA_us);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGRA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGRA_us, TPixel_RGBA_us);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGRA_us, TPixel_RGB_us);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGRA_us, TPixel_BGRA_us);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGRA_us, TPixel_BGR_us);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGRA_us, TPixel_L_us);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGRA_us, TPixel_LA_us);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGR )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGR_us, TPixel_RGBA_us);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGR_us, TPixel_RGB_us);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGR_us, TPixel_BGRA_us);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGR_us, TPixel_BGR_us);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGR_us, TPixel_L_us);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGR_us, TPixel_LA_us);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_L_us, TPixel_RGBA_us);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_L_us, TPixel_RGB_us);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_L_us, TPixel_BGRA_us);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_L_us, TPixel_BGR_us);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_L_us, TPixel_L_us);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_L_us, TPixel_LA_us);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_LA_us, TPixel_RGBA_us);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_LA_us, TPixel_RGB_us);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_LA_us, TPixel_BGRA_us);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_LA_us, TPixel_BGR_us);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_LA_us, TPixel_L_us);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_LA_us, TPixel_LA_us);
			}
		}
	}
	else if ( iTrgDataType == CLUVIZ_IMG_INT )
	{
		if ( iTrgImgType == CLUVIZ_IMG_RGBA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGBA_i, TPixel_RGBA_i);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGBA_i, TPixel_RGB_i);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGBA_i, TPixel_BGRA_i);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGBA_i, TPixel_BGR_i);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGBA_i, TPixel_L_i);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGBA_i, TPixel_LA_i);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_RGB )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGB_i, TPixel_RGBA_i);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGB_i, TPixel_RGB_i);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGB_i, TPixel_BGRA_i);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGB_i, TPixel_BGR_i);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGB_i, TPixel_L_i);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGB_i, TPixel_LA_i);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGRA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGRA_i, TPixel_RGBA_i);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGRA_i, TPixel_RGB_i);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGRA_i, TPixel_BGRA_i);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGRA_i, TPixel_BGR_i);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGRA_i, TPixel_L_i);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGRA_i, TPixel_LA_i);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGR )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGR_i, TPixel_RGBA_i);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGR_i, TPixel_RGB_i);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGR_i, TPixel_BGRA_i);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGR_i, TPixel_BGR_i);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGR_i, TPixel_L_i);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGR_i, TPixel_LA_i);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_L_i, TPixel_RGBA_i);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_L_i, TPixel_RGB_i);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_L_i, TPixel_BGRA_i);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_L_i, TPixel_BGR_i);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_L_i, TPixel_L_i);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_L_i, TPixel_LA_i);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_LA_i, TPixel_RGBA_i);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_LA_i, TPixel_RGB_i);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_LA_i, TPixel_BGRA_i);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_LA_i, TPixel_BGR_i);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_LA_i, TPixel_L_i);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_LA_i, TPixel_LA_i);
			}
		}
	}
	else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_INT )
	{
		if ( iTrgImgType == CLUVIZ_IMG_RGBA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGBA_ui, TPixel_RGBA_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGBA_ui, TPixel_RGB_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGBA_ui, TPixel_BGRA_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGBA_ui, TPixel_BGR_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGBA_ui, TPixel_L_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGBA_ui, TPixel_LA_ui);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_RGB )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGB_ui, TPixel_RGBA_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGB_ui, TPixel_RGB_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGB_ui, TPixel_BGRA_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGB_ui, TPixel_BGR_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGB_ui, TPixel_L_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGB_ui, TPixel_LA_ui);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGRA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGRA_ui, TPixel_RGBA_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGRA_ui, TPixel_RGB_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGRA_ui, TPixel_BGRA_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGRA_ui, TPixel_BGR_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGRA_ui, TPixel_L_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGRA_ui, TPixel_LA_ui);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGR )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGR_ui, TPixel_RGBA_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGR_ui, TPixel_RGB_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGR_ui, TPixel_BGRA_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGR_ui, TPixel_BGR_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGR_ui, TPixel_L_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGR_ui, TPixel_LA_ui);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_L_ui, TPixel_RGBA_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_L_ui, TPixel_RGB_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_L_ui, TPixel_BGRA_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_L_ui, TPixel_BGR_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_L_ui, TPixel_L_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_L_ui, TPixel_LA_ui);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_LA_ui, TPixel_RGBA_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_LA_ui, TPixel_RGB_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_LA_ui, TPixel_BGRA_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_LA_ui, TPixel_BGR_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_LA_ui, TPixel_L_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_LA_ui, TPixel_LA_ui);
			}
		}
	}
	else if ( iTrgDataType == CLUVIZ_IMG_FLOAT )
	{
		if ( iTrgImgType == CLUVIZ_IMG_RGBA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGBA_f, TPixel_RGBA_f);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGBA_f, TPixel_RGB_f);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGBA_f, TPixel_BGRA_f);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGBA_f, TPixel_BGR_f);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGBA_f, TPixel_L_f);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGBA_f, TPixel_LA_f);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_RGB )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGB_f, TPixel_RGBA_f);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGB_f, TPixel_RGB_f);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGB_f, TPixel_BGRA_f);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGB_f, TPixel_BGR_f);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGB_f, TPixel_L_f);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGB_f, TPixel_LA_f);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGRA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGRA_f, TPixel_RGBA_f);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGRA_f, TPixel_RGB_f);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGRA_f, TPixel_BGRA_f);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGRA_f, TPixel_BGR_f);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGRA_f, TPixel_L_f);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGRA_f, TPixel_LA_f);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGR )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGR_f, TPixel_RGBA_f);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGR_f, TPixel_RGB_f);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGR_f, TPixel_BGRA_f);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGR_f, TPixel_BGR_f);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGR_f, TPixel_L_f);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGR_f, TPixel_LA_f);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_L_f, TPixel_RGBA_f);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_L_f, TPixel_RGB_f);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_L_f, TPixel_BGRA_f);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_L_f, TPixel_BGR_f);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_L_f, TPixel_L_f);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_L_f, TPixel_LA_f);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_LA_f, TPixel_RGBA_f);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_LA_f, TPixel_RGB_f);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_LA_f, TPixel_BGRA_f);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_LA_f, TPixel_BGR_f);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_LA_f, TPixel_L_f);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_LA_f, TPixel_LA_f);
			}
		}
	}
	else if ( iTrgDataType == CLUVIZ_IMG_DOUBLE )
	{
		if ( iTrgImgType == CLUVIZ_IMG_RGBA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGBA_d, TPixel_RGBA_d);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGBA_d, TPixel_RGB_d);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGBA_d, TPixel_BGRA_d);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGBA_d, TPixel_BGR_d);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGBA_d, TPixel_L_d);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGBA_d, TPixel_LA_d);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_RGB )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGB_d, TPixel_RGBA_d);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGB_d, TPixel_RGB_d);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGB_d, TPixel_BGRA_d);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGB_d, TPixel_BGR_d);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGB_d, TPixel_L_d);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGB_d, TPixel_LA_d);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGRA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGRA_d, TPixel_RGBA_d);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGRA_d, TPixel_RGB_d);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGRA_d, TPixel_BGRA_d);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGRA_d, TPixel_BGR_d);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGRA_d, TPixel_L_d);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGRA_d, TPixel_LA_d);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGR )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGR_d, TPixel_RGBA_d);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGR_d, TPixel_RGB_d);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGR_d, TPixel_BGRA_d);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGR_d, TPixel_BGR_d);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGR_d, TPixel_L_d);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGR_d, TPixel_LA_d);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_L_d, TPixel_RGBA_d);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_L_d, TPixel_RGB_d);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_L_d, TPixel_BGRA_d);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_L_d, TPixel_BGR_d);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_L_d, TPixel_L_d);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_L_d, TPixel_LA_d);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_LA_d, TPixel_RGBA_d);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_LA_d, TPixel_RGB_d);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_LA_d, TPixel_BGRA_d);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_LA_d, TPixel_BGR_d);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_LA_d, TPixel_L_d);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_LA_d, TPixel_LA_d);
			}
		}
	}



	return true;
}

#undef EXECPIXELOP



// Define to simplify code in ExecutePixelOperator_EqualType
#define EXECPIXELOP( PXTYPE1, PXTYPE2 ) Functor<PXTYPE1,PXTYPE2,PXTYPE1>::Execute( \
		(PXTYPE1 *) rTrg.GetDataPtr(), (PXTYPE2 *) rSrc1.GetDataPtr(), (PXTYPE1 *) rSrc2.GetDataPtr(), rPar )

// Expect data type of all images to be the same
// Expect rTrg and rSrc2 to be of the same type
template< template<class TPxL, class TPxR1, class TPxR2> class Functor, class Parameter >
bool COGLImage::ExecutePixelOperator_EqualDataType121( COGLImage &rTrg, const COGLImage &rSrc1, const COGLImage &rSrc2, Parameter& rPar )
{
	int iTrgImgType, iTrgDataType, iTrgBytesPerPixel;
	int iSrc1ImgType, iSrc1DataType, iSrc1BytesPerPixel;
	int iSrc2ImgType, iSrc2DataType, iSrc2BytesPerPixel;
	int iImgType;

	rTrg.GetType( iTrgImgType, iTrgDataType, iTrgBytesPerPixel );
	rSrc1.GetType( iSrc1ImgType, iSrc1DataType, iSrc1BytesPerPixel );
	rSrc2.GetType( iSrc2ImgType, iSrc2DataType, iSrc2BytesPerPixel );

	// All data types have to be the same
	// Target and Src2 image type have to be the same
	if ( iTrgImgType != iSrc2ImgType || 
		 iTrgDataType != iSrc1DataType ||
		 iTrgDataType != iSrc2DataType 
		)
		return false;

	iImgType = iSrc1ImgType;

	if ( iTrgDataType == CLUVIZ_IMG_BYTE )
	{
		if ( iTrgImgType == CLUVIZ_IMG_RGBA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGBA_c, TPixel_RGBA_c);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGBA_c, TPixel_RGB_c);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGBA_c, TPixel_BGRA_c);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGBA_c, TPixel_BGR_c);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGBA_c, TPixel_L_c);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGBA_c, TPixel_LA_c);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_RGB )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGB_c, TPixel_RGBA_c);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGB_c, TPixel_RGB_c);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGB_c, TPixel_BGRA_c);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGB_c, TPixel_BGR_c);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGB_c, TPixel_L_c);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGB_c, TPixel_LA_c);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGRA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGRA_c, TPixel_RGBA_c);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGRA_c, TPixel_RGB_c);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGRA_c, TPixel_BGRA_c);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGRA_c, TPixel_BGR_c);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGRA_c, TPixel_L_c);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGRA_c, TPixel_LA_c);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGR )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGR_c, TPixel_RGBA_c);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGR_c, TPixel_RGB_c);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGR_c, TPixel_BGRA_c);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGR_c, TPixel_BGR_c);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGR_c, TPixel_L_c);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGR_c, TPixel_LA_c);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_L_c, TPixel_RGBA_c);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_L_c, TPixel_RGB_c);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_L_c, TPixel_BGRA_c);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_L_c, TPixel_BGR_c);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_L_c, TPixel_L_c);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_L_c, TPixel_LA_c);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_LA_c, TPixel_RGBA_c);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_LA_c, TPixel_RGB_c);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_LA_c, TPixel_BGRA_c);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_LA_c, TPixel_BGR_c);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_LA_c, TPixel_L_c);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_LA_c, TPixel_LA_c);
			}
		}
	}
	else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_BYTE )
	{
		if ( iTrgImgType == CLUVIZ_IMG_RGBA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGBA_uc, TPixel_RGBA_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGBA_uc, TPixel_RGB_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGBA_uc, TPixel_BGRA_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGBA_uc, TPixel_BGR_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGBA_uc, TPixel_L_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGBA_uc, TPixel_LA_uc);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_RGB )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGB_uc, TPixel_RGBA_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGB_uc, TPixel_RGB_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGB_uc, TPixel_BGRA_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGB_uc, TPixel_BGR_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGB_uc, TPixel_L_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGB_uc, TPixel_LA_uc);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGRA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGRA_uc, TPixel_RGBA_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGRA_uc, TPixel_RGB_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGRA_uc, TPixel_BGRA_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGRA_uc, TPixel_BGR_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGRA_uc, TPixel_L_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGRA_uc, TPixel_LA_uc);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGR )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGR_uc, TPixel_RGBA_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGR_uc, TPixel_RGB_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGR_uc, TPixel_BGRA_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGR_uc, TPixel_BGR_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGR_uc, TPixel_L_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGR_uc, TPixel_LA_uc);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_L_uc, TPixel_RGBA_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_L_uc, TPixel_RGB_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_L_uc, TPixel_BGRA_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_L_uc, TPixel_BGR_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_L_uc, TPixel_L_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_L_uc, TPixel_LA_uc);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_LA_uc, TPixel_RGBA_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_LA_uc, TPixel_RGB_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_LA_uc, TPixel_BGRA_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_LA_uc, TPixel_BGR_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_LA_uc, TPixel_L_uc);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_LA_uc, TPixel_LA_uc);
			}
		}
	}
	else if ( iTrgDataType == CLUVIZ_IMG_SHORT )
	{
		if ( iTrgImgType == CLUVIZ_IMG_RGBA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGBA_s, TPixel_RGBA_s);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGBA_s, TPixel_RGB_s);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGBA_s, TPixel_BGRA_s);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGBA_s, TPixel_BGR_s);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGBA_s, TPixel_L_s);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGBA_s, TPixel_LA_s);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_RGB )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGB_s, TPixel_RGBA_s);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGB_s, TPixel_RGB_s);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGB_s, TPixel_BGRA_s);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGB_s, TPixel_BGR_s);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGB_s, TPixel_L_s);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGB_s, TPixel_LA_s);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGRA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGRA_s, TPixel_RGBA_s);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGRA_s, TPixel_RGB_s);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGRA_s, TPixel_BGRA_s);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGRA_s, TPixel_BGR_s);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGRA_s, TPixel_L_s);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGRA_s, TPixel_LA_s);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGR )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGR_s, TPixel_RGBA_s);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGR_s, TPixel_RGB_s);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGR_s, TPixel_BGRA_s);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGR_s, TPixel_BGR_s);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGR_s, TPixel_L_s);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGR_s, TPixel_LA_s);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_L_s, TPixel_RGBA_s);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_L_s, TPixel_RGB_s);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_L_s, TPixel_BGRA_s);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_L_s, TPixel_BGR_s);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_L_s, TPixel_L_s);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_L_s, TPixel_LA_s);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_LA_s, TPixel_RGBA_s);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_LA_s, TPixel_RGB_s);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_LA_s, TPixel_BGRA_s);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_LA_s, TPixel_BGR_s);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_LA_s, TPixel_L_s);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_LA_s, TPixel_LA_s);
			}
		}
	}
	else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_SHORT )
	{
		if ( iTrgImgType == CLUVIZ_IMG_RGBA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGBA_us, TPixel_RGBA_us);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGBA_us, TPixel_RGB_us);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGBA_us, TPixel_BGRA_us);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGBA_us, TPixel_BGR_us);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGBA_us, TPixel_L_us);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGBA_us, TPixel_LA_us);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_RGB )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGB_us, TPixel_RGBA_us);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGB_us, TPixel_RGB_us);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGB_us, TPixel_BGRA_us);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGB_us, TPixel_BGR_us);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGB_us, TPixel_L_us);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGB_us, TPixel_LA_us);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGRA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGRA_us, TPixel_RGBA_us);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGRA_us, TPixel_RGB_us);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGRA_us, TPixel_BGRA_us);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGRA_us, TPixel_BGR_us);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGRA_us, TPixel_L_us);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGRA_us, TPixel_LA_us);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGR )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGR_us, TPixel_RGBA_us);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGR_us, TPixel_RGB_us);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGR_us, TPixel_BGRA_us);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGR_us, TPixel_BGR_us);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGR_us, TPixel_L_us);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGR_us, TPixel_LA_us);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_L_us, TPixel_RGBA_us);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_L_us, TPixel_RGB_us);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_L_us, TPixel_BGRA_us);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_L_us, TPixel_BGR_us);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_L_us, TPixel_L_us);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_L_us, TPixel_LA_us);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_LA_us, TPixel_RGBA_us);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_LA_us, TPixel_RGB_us);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_LA_us, TPixel_BGRA_us);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_LA_us, TPixel_BGR_us);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_LA_us, TPixel_L_us);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_LA_us, TPixel_LA_us);
			}
		}
	}
	else if ( iTrgDataType == CLUVIZ_IMG_INT )
	{
		if ( iTrgImgType == CLUVIZ_IMG_RGBA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGBA_i, TPixel_RGBA_i);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGBA_i, TPixel_RGB_i);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGBA_i, TPixel_BGRA_i);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGBA_i, TPixel_BGR_i);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGBA_i, TPixel_L_i);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGBA_i, TPixel_LA_i);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_RGB )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGB_i, TPixel_RGBA_i);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGB_i, TPixel_RGB_i);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGB_i, TPixel_BGRA_i);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGB_i, TPixel_BGR_i);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGB_i, TPixel_L_i);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGB_i, TPixel_LA_i);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGRA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGRA_i, TPixel_RGBA_i);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGRA_i, TPixel_RGB_i);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGRA_i, TPixel_BGRA_i);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGRA_i, TPixel_BGR_i);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGRA_i, TPixel_L_i);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGRA_i, TPixel_LA_i);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGR )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGR_i, TPixel_RGBA_i);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGR_i, TPixel_RGB_i);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGR_i, TPixel_BGRA_i);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGR_i, TPixel_BGR_i);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGR_i, TPixel_L_i);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGR_i, TPixel_LA_i);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_L_i, TPixel_RGBA_i);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_L_i, TPixel_RGB_i);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_L_i, TPixel_BGRA_i);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_L_i, TPixel_BGR_i);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_L_i, TPixel_L_i);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_L_i, TPixel_LA_i);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_LA_i, TPixel_RGBA_i);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_LA_i, TPixel_RGB_i);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_LA_i, TPixel_BGRA_i);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_LA_i, TPixel_BGR_i);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_LA_i, TPixel_L_i);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_LA_i, TPixel_LA_i);
			}
		}
	}
	else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_INT )
	{
		if ( iTrgImgType == CLUVIZ_IMG_RGBA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGBA_ui, TPixel_RGBA_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGBA_ui, TPixel_RGB_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGBA_ui, TPixel_BGRA_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGBA_ui, TPixel_BGR_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGBA_ui, TPixel_L_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGBA_ui, TPixel_LA_ui);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_RGB )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGB_ui, TPixel_RGBA_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGB_ui, TPixel_RGB_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGB_ui, TPixel_BGRA_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGB_ui, TPixel_BGR_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGB_ui, TPixel_L_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGB_ui, TPixel_LA_ui);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGRA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGRA_ui, TPixel_RGBA_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGRA_ui, TPixel_RGB_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGRA_ui, TPixel_BGRA_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGRA_ui, TPixel_BGR_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGRA_ui, TPixel_L_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGRA_ui, TPixel_LA_ui);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGR )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGR_ui, TPixel_RGBA_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGR_ui, TPixel_RGB_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGR_ui, TPixel_BGRA_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGR_ui, TPixel_BGR_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGR_ui, TPixel_L_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGR_ui, TPixel_LA_ui);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_L_ui, TPixel_RGBA_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_L_ui, TPixel_RGB_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_L_ui, TPixel_BGRA_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_L_ui, TPixel_BGR_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_L_ui, TPixel_L_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_L_ui, TPixel_LA_ui);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_LA_ui, TPixel_RGBA_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_LA_ui, TPixel_RGB_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_LA_ui, TPixel_BGRA_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_LA_ui, TPixel_BGR_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_LA_ui, TPixel_L_ui);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_LA_ui, TPixel_LA_ui);
			}
		}
	}
	else if ( iTrgDataType == CLUVIZ_IMG_FLOAT )
	{
		if ( iTrgImgType == CLUVIZ_IMG_RGBA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGBA_f, TPixel_RGBA_f);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGBA_f, TPixel_RGB_f);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGBA_f, TPixel_BGRA_f);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGBA_f, TPixel_BGR_f);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGBA_f, TPixel_L_f);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGBA_f, TPixel_LA_f);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_RGB )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGB_f, TPixel_RGBA_f);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGB_f, TPixel_RGB_f);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGB_f, TPixel_BGRA_f);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGB_f, TPixel_BGR_f);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGB_f, TPixel_L_f);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGB_f, TPixel_LA_f);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGRA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGRA_f, TPixel_RGBA_f);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGRA_f, TPixel_RGB_f);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGRA_f, TPixel_BGRA_f);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGRA_f, TPixel_BGR_f);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGRA_f, TPixel_L_f);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGRA_f, TPixel_LA_f);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGR )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGR_f, TPixel_RGBA_f);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGR_f, TPixel_RGB_f);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGR_f, TPixel_BGRA_f);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGR_f, TPixel_BGR_f);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGR_f, TPixel_L_f);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGR_f, TPixel_LA_f);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_L_f, TPixel_RGBA_f);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_L_f, TPixel_RGB_f);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_L_f, TPixel_BGRA_f);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_L_f, TPixel_BGR_f);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_L_f, TPixel_L_f);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_L_f, TPixel_LA_f);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_LA_f, TPixel_RGBA_f);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_LA_f, TPixel_RGB_f);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_LA_f, TPixel_BGRA_f);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_LA_f, TPixel_BGR_f);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_LA_f, TPixel_L_f);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_LA_f, TPixel_LA_f);
			}
		}
	}
	else if ( iTrgDataType == CLUVIZ_IMG_DOUBLE )
	{
		if ( iTrgImgType == CLUVIZ_IMG_RGBA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGBA_d, TPixel_RGBA_d);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGBA_d, TPixel_RGB_d);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGBA_d, TPixel_BGRA_d);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGBA_d, TPixel_BGR_d);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGBA_d, TPixel_L_d);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGBA_d, TPixel_LA_d);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_RGB )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_RGB_d, TPixel_RGBA_d);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_RGB_d, TPixel_RGB_d);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_RGB_d, TPixel_BGRA_d);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_RGB_d, TPixel_BGR_d);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_RGB_d, TPixel_L_d);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_RGB_d, TPixel_LA_d);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGRA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGRA_d, TPixel_RGBA_d);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGRA_d, TPixel_RGB_d);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGRA_d, TPixel_BGRA_d);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGRA_d, TPixel_BGR_d);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGRA_d, TPixel_L_d);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGRA_d, TPixel_LA_d);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_BGR )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_BGR_d, TPixel_RGBA_d);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_BGR_d, TPixel_RGB_d);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_BGR_d, TPixel_BGRA_d);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_BGR_d, TPixel_BGR_d);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_BGR_d, TPixel_L_d);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_BGR_d, TPixel_LA_d);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_L_d, TPixel_RGBA_d);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_L_d, TPixel_RGB_d);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_L_d, TPixel_BGRA_d);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_L_d, TPixel_BGR_d);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_L_d, TPixel_L_d);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_L_d, TPixel_LA_d);
			}
		}
		else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
		{
			if ( iImgType == CLUVIZ_IMG_RGBA )
			{
				return EXECPIXELOP(TPixel_LA_d, TPixel_RGBA_d);
			}
			else if ( iImgType == CLUVIZ_IMG_RGB )
			{
				return EXECPIXELOP(TPixel_LA_d, TPixel_RGB_d);
			}
			else if ( iImgType == CLUVIZ_IMG_BGRA )
			{
				return EXECPIXELOP(TPixel_LA_d, TPixel_BGRA_d);
			}
			else if ( iImgType == CLUVIZ_IMG_BGR )
			{
				return EXECPIXELOP(TPixel_LA_d, TPixel_BGR_d);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE )
			{
				return EXECPIXELOP(TPixel_LA_d, TPixel_L_d);
			}
			else if ( iImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
			{
				return EXECPIXELOP(TPixel_LA_d, TPixel_LA_d);
			}
		}
	}



	return true;
}

#undef EXECPIXELOP

