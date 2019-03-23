////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLImage_Exec_AnyType.cxx
//
// summary:   ogl image execute any type class
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

// Define to simplify code in ExecutePixelOperator_AnyType
#define EXECPIXELOP_112_LEVEL1( PXTYPE ) ExecutePixelOperator_Type112_Level1<Functor, Parameter, PXTYPE, PXTYPE>( (PXTYPE *) rTrg.GetDataPtr(), (PXTYPE *) rSrc1.GetDataPtr(), rSrc2, rPar )
#define EXECPIXELOP_121_LEVEL1( PXTYPE ) ExecutePixelOperator_Type121_Level1<Functor, Parameter, PXTYPE, PXTYPE>( (PXTYPE *) rTrg.GetDataPtr(), rSrc1, (PXTYPE *) rSrc2.GetDataPtr(), rPar )
//#define EXECPIXELOP_LEVEL2( PXTYPE ) ExecutePixelOperator_AnyType_Level2( pTrg, (PXTYPE *) rSrc1.GetDataPtr(), rSrc2, rPar )
//#define EXECPIXELOP_LEVEL3( PXTYPE ) ExecutePixelOperator_AnyType_Level3( pTrg, pSrc1, (PXTYPE *) rSrc2.GetDataPtr(), rPar )

#define EXECPIXELOP_112( PXTYPE1, PXTYPE2, PXTYPE3 ) Functor<PXTYPE1,PXTYPE2,PXTYPE3>::Execute( (PXTYPE1 *) pTrg, (PXTYPE2 *) pSrc1, (PXTYPE3 *) rSrc2.GetDataPtr(), rPar );
#define EXECPIXELOP_121( PXTYPE1, PXTYPE2, PXTYPE3 ) Functor<PXTYPE1,PXTYPE2,PXTYPE3>::Execute( (PXTYPE1 *) pTrg, (PXTYPE2 *) rSrc1.GetDataPtr(), (PXTYPE3 *) pSrc2, rPar );


/// Any Type construct start
template< template<class TPxL, class TPxR1, class TPxR2> class Functor, class Parameter >
bool COGLImage::ExecutePixelOperator_Type112( COGLImage &rTrg, const COGLImage &rSrc1, const COGLImage &rSrc2, Parameter& rPar )
{
	int iTrgImgType, iTrgDataType, iTrgBytesPerPixel;
	int iSrc1ImgType, iSrc1DataType, iSrc1BytesPerPixel;

	rTrg.GetType( iTrgImgType, iTrgDataType, iTrgBytesPerPixel );
	rSrc1.GetType( iSrc1ImgType, iSrc1DataType, iSrc1BytesPerPixel );

	if ( iTrgImgType != iSrc1ImgType || iTrgDataType != iSrc1DataType )
	{
		return false;
	}


	if ( iTrgImgType == CLUVIZ_IMG_RGBA )
	{
		if ( iTrgDataType == CLUVIZ_IMG_BYTE )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_RGBA_c);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_BYTE )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_RGBA_uc);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_SHORT )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_RGBA_s);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_SHORT )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_RGBA_us);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_INT )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_RGBA_i);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_INT )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_RGBA_ui);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_FLOAT )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_RGBA_f);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_DOUBLE )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_RGBA_d);
		}
	}
	else if ( iTrgImgType == CLUVIZ_IMG_RGB )
	{
		if ( iTrgDataType == CLUVIZ_IMG_BYTE )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_RGB_c);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_BYTE )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_RGB_uc);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_SHORT )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_RGB_s);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_SHORT )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_RGB_us);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_INT )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_RGB_i);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_INT )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_RGB_ui);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_FLOAT )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_RGB_f);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_DOUBLE )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_RGB_d);
		}
	}
	else if ( iTrgImgType == CLUVIZ_IMG_BGRA )
	{
		if ( iTrgDataType == CLUVIZ_IMG_BYTE )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_BGRA_c);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_BYTE )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_BGRA_uc);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_SHORT )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_BGRA_s);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_SHORT )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_BGRA_us);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_INT )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_BGRA_i);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_INT )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_BGRA_ui);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_FLOAT )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_BGRA_f);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_DOUBLE )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_BGRA_d);
		}
	}
	else if ( iTrgImgType == CLUVIZ_IMG_BGR )
	{
		if ( iTrgDataType == CLUVIZ_IMG_BYTE )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_BGR_c);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_BYTE )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_BGR_uc);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_SHORT )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_BGR_s);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_SHORT )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_BGR_us);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_INT )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_BGR_i);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_INT )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_BGR_ui);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_FLOAT )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_BGR_f);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_DOUBLE )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_BGR_d);
		}
	}
	else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE )
	{
		if ( iTrgDataType == CLUVIZ_IMG_BYTE )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_L_c);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_BYTE )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_L_uc);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_SHORT )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_L_s);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_SHORT )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_L_us);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_INT )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_L_i);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_INT )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_L_ui);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_FLOAT )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_L_f);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_DOUBLE )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_L_d);
		}
	}
	else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
	{
		if ( iTrgDataType == CLUVIZ_IMG_BYTE )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_LA_c);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_BYTE )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_LA_uc);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_SHORT )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_LA_s);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_SHORT )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_LA_us);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_INT )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_LA_i);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_INT )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_LA_ui);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_FLOAT )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_LA_f);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_DOUBLE )
		{
			return EXECPIXELOP_112_LEVEL1(TPixel_LA_d);
		}
	}

	return true;
}

template< template<class TPxL, class TPxR1, class TPxR2> class Functor, class Parameter, class TPxL, class TPxR1 >
bool COGLImage::ExecutePixelOperator_Type112_Level1( TPxL *pTrg, TPxR1 *pSrc1, const COGLImage &rSrc2, Parameter& rPar )
{
	int iSrc2ImgType, iSrc2DataType, iSrc2BytesPerPixel;
	rSrc2.GetType( iSrc2ImgType, iSrc2DataType, iSrc2BytesPerPixel );

	if ( iSrc2ImgType == CLUVIZ_IMG_RGBA )
	{
		if ( iSrc2DataType == CLUVIZ_IMG_BYTE )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_RGBA_c);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_UNSIGNED_BYTE )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_RGBA_uc);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_SHORT )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_RGBA_s);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_UNSIGNED_SHORT )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_RGBA_us);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_INT )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_RGBA_i);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_UNSIGNED_INT )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_RGBA_ui);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_FLOAT )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_RGBA_f);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_DOUBLE )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_RGBA_d);
		}
	}
	else if ( iSrc2ImgType == CLUVIZ_IMG_RGB )
	{
		if ( iSrc2DataType == CLUVIZ_IMG_BYTE )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_RGB_c);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_UNSIGNED_BYTE )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_RGB_uc);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_SHORT )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_RGB_s);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_UNSIGNED_SHORT )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_RGB_us);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_INT )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_RGB_i);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_UNSIGNED_INT )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_RGB_ui);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_FLOAT )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_RGB_f);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_DOUBLE )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_RGB_d);
		}
	}
	else if ( iSrc2ImgType == CLUVIZ_IMG_BGRA )
	{
		if ( iSrc2DataType == CLUVIZ_IMG_BYTE )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_BGRA_c);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_UNSIGNED_BYTE )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_BGRA_uc);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_SHORT )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_BGRA_s);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_UNSIGNED_SHORT )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_BGRA_us);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_INT )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_BGRA_i);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_UNSIGNED_INT )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_BGRA_ui);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_FLOAT )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_BGRA_f);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_DOUBLE )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_BGRA_d);
		}
	}
	else if ( iSrc2ImgType == CLUVIZ_IMG_BGR )
	{
		if ( iSrc2DataType == CLUVIZ_IMG_BYTE )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_BGR_c);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_UNSIGNED_BYTE )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_BGR_uc);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_SHORT )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_BGR_s);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_UNSIGNED_SHORT )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_BGR_us);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_INT )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_BGR_i);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_UNSIGNED_INT )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_BGR_ui);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_FLOAT )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_BGR_f);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_DOUBLE )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_BGR_d);
		}
	}
	else if ( iSrc2ImgType == CLUVIZ_IMG_LUMINANCE )
	{
		if ( iSrc2DataType == CLUVIZ_IMG_BYTE )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_L_c);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_UNSIGNED_BYTE )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_L_uc);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_SHORT )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_L_s);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_UNSIGNED_SHORT )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_L_us);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_INT )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_L_i);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_UNSIGNED_INT )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_L_ui);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_FLOAT )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_L_f);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_DOUBLE )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_L_d);
		}
	}
	else if ( iSrc2ImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
	{
		if ( iSrc2DataType == CLUVIZ_IMG_BYTE )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_LA_c);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_UNSIGNED_BYTE )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_LA_uc);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_SHORT )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_LA_s);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_UNSIGNED_SHORT )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_LA_us);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_INT )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_LA_i);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_UNSIGNED_INT )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_LA_ui);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_FLOAT )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_LA_f);
		}
		else if ( iSrc2DataType == CLUVIZ_IMG_DOUBLE )
		{
			return EXECPIXELOP_112(TPxL, TPxR1, TPixel_LA_d);
		}
	}

	return true;
}


/// Any Type construct start
template< template<class TPxL, class TPxR1, class TPxR2> class Functor, class Parameter >
bool COGLImage::ExecutePixelOperator_Type121( COGLImage &rTrg, const COGLImage &rSrc1, const COGLImage &rSrc2, Parameter& rPar )
{
	int iTrgImgType, iTrgDataType, iTrgBytesPerPixel;
	int iSrc2ImgType, iSrc2DataType, iSrc2BytesPerPixel;

	rTrg.GetType( iTrgImgType, iTrgDataType, iTrgBytesPerPixel );
	rSrc2.GetType( iSrc2ImgType, iSrc2DataType, iSrc2BytesPerPixel );

	if ( iTrgImgType != iSrc2ImgType || iTrgDataType != iSrc2DataType )
	{
		return false;
	}


	if ( iTrgImgType == CLUVIZ_IMG_RGBA )
	{
		if ( iTrgDataType == CLUVIZ_IMG_BYTE )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_RGBA_c);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_BYTE )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_RGBA_uc);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_SHORT )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_RGBA_s);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_SHORT )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_RGBA_us);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_INT )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_RGBA_i);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_INT )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_RGBA_ui);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_FLOAT )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_RGBA_f);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_DOUBLE )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_RGBA_d);
		}
	}
	else if ( iTrgImgType == CLUVIZ_IMG_RGB )
	{
		if ( iTrgDataType == CLUVIZ_IMG_BYTE )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_RGB_c);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_BYTE )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_RGB_uc);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_SHORT )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_RGB_s);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_SHORT )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_RGB_us);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_INT )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_RGB_i);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_INT )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_RGB_ui);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_FLOAT )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_RGB_f);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_DOUBLE )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_RGB_d);
		}
	}
	else if ( iTrgImgType == CLUVIZ_IMG_BGRA )
	{
		if ( iTrgDataType == CLUVIZ_IMG_BYTE )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_BGRA_c);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_BYTE )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_BGRA_uc);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_SHORT )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_BGRA_s);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_SHORT )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_BGRA_us);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_INT )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_BGRA_i);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_INT )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_BGRA_ui);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_FLOAT )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_BGRA_f);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_DOUBLE )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_BGRA_d);
		}
	}
	else if ( iTrgImgType == CLUVIZ_IMG_BGR )
	{
		if ( iTrgDataType == CLUVIZ_IMG_BYTE )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_BGR_c);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_BYTE )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_BGR_uc);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_SHORT )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_BGR_s);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_SHORT )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_BGR_us);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_INT )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_BGR_i);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_INT )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_BGR_ui);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_FLOAT )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_BGR_f);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_DOUBLE )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_BGR_d);
		}
	}
	else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE )
	{
		if ( iTrgDataType == CLUVIZ_IMG_BYTE )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_L_c);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_BYTE )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_L_uc);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_SHORT )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_L_s);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_SHORT )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_L_us);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_INT )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_L_i);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_INT )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_L_ui);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_FLOAT )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_L_f);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_DOUBLE )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_L_d);
		}
	}
	else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
	{
		if ( iTrgDataType == CLUVIZ_IMG_BYTE )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_LA_c);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_BYTE )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_LA_uc);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_SHORT )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_LA_s);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_SHORT )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_LA_us);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_INT )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_LA_i);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_INT )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_LA_ui);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_FLOAT )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_LA_f);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_DOUBLE )
		{
			return EXECPIXELOP_121_LEVEL1(TPixel_LA_d);
		}
	}

	return true;
}

template< template<class TPxL, class TPxR1, class TPxR2> class Functor, class Parameter, class TPxL, class TPxR2 >
bool COGLImage::ExecutePixelOperator_Type121_Level1( TPxL *pTrg, const COGLImage &rSrc1, TPxR2 *pSrc2, Parameter& rPar )
{
	int iSrc1ImgType, iSrc1DataType, iSrc1BytesPerPixel;
	rSrc1.GetType( iSrc1ImgType, iSrc1DataType, iSrc1BytesPerPixel );

	if ( iSrc1ImgType == CLUVIZ_IMG_RGBA )
	{
		if ( iSrc1DataType == CLUVIZ_IMG_BYTE )
		{
			return EXECPIXELOP_121(TPxL, TPixel_RGBA_c, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_UNSIGNED_BYTE )
		{
			return EXECPIXELOP_121(TPxL, TPixel_RGBA_uc, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_SHORT )
		{
			return EXECPIXELOP_121(TPxL, TPixel_RGBA_s, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_UNSIGNED_SHORT )
		{
			return EXECPIXELOP_121(TPxL, TPixel_RGBA_us, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_INT )
		{
			return EXECPIXELOP_121(TPxL, TPixel_RGBA_i, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_UNSIGNED_INT )
		{
			return EXECPIXELOP_121(TPxL, TPixel_RGBA_ui, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_FLOAT )
		{
			return EXECPIXELOP_121(TPxL, TPixel_RGBA_f, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_DOUBLE )
		{
			return EXECPIXELOP_121(TPxL, TPixel_RGBA_d, TPxR2);
		}
	}
	else if ( iSrc1ImgType == CLUVIZ_IMG_RGB )
	{
		if ( iSrc1DataType == CLUVIZ_IMG_BYTE )
		{
			return EXECPIXELOP_121(TPxL, TPixel_RGB_c, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_UNSIGNED_BYTE )
		{
			return EXECPIXELOP_121(TPxL, TPixel_RGB_uc, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_SHORT )
		{
			return EXECPIXELOP_121(TPxL, TPixel_RGB_s, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_UNSIGNED_SHORT )
		{
			return EXECPIXELOP_121(TPxL, TPixel_RGB_us, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_INT )
		{
			return EXECPIXELOP_121(TPxL, TPixel_RGB_i, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_UNSIGNED_INT )
		{
			return EXECPIXELOP_121(TPxL, TPixel_RGB_ui, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_FLOAT )
		{
			return EXECPIXELOP_121(TPxL, TPixel_RGB_f, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_DOUBLE )
		{
			return EXECPIXELOP_121(TPxL, TPixel_RGB_d, TPxR2);
		}
	}
	else if ( iSrc1ImgType == CLUVIZ_IMG_BGRA )
	{
		if ( iSrc1DataType == CLUVIZ_IMG_BYTE )
		{
			return EXECPIXELOP_121(TPxL, TPixel_BGRA_c, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_UNSIGNED_BYTE )
		{
			return EXECPIXELOP_121(TPxL, TPixel_BGRA_uc, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_SHORT )
		{
			return EXECPIXELOP_121(TPxL, TPixel_BGRA_s, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_UNSIGNED_SHORT )
		{
			return EXECPIXELOP_121(TPxL, TPixel_BGRA_us, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_INT )
		{
			return EXECPIXELOP_121(TPxL, TPixel_BGRA_i, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_UNSIGNED_INT )
		{
			return EXECPIXELOP_121(TPxL, TPixel_BGRA_ui, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_FLOAT )
		{
			return EXECPIXELOP_121(TPxL, TPixel_BGRA_f, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_DOUBLE )
		{
			return EXECPIXELOP_121(TPxL, TPixel_BGRA_d, TPxR2);
		}
	}
	else if ( iSrc1ImgType == CLUVIZ_IMG_BGR )
	{
		if ( iSrc1DataType == CLUVIZ_IMG_BYTE )
		{
			return EXECPIXELOP_121(TPxL, TPixel_BGR_c, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_UNSIGNED_BYTE )
		{
			return EXECPIXELOP_121(TPxL, TPixel_BGR_uc, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_SHORT )
		{
			return EXECPIXELOP_121(TPxL, TPixel_BGR_s, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_UNSIGNED_SHORT )
		{
			return EXECPIXELOP_121(TPxL, TPixel_BGR_us, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_INT )
		{
			return EXECPIXELOP_121(TPxL, TPixel_BGR_i, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_UNSIGNED_INT )
		{
			return EXECPIXELOP_121(TPxL, TPixel_BGR_ui, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_FLOAT )
		{
			return EXECPIXELOP_121(TPxL, TPixel_BGR_f, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_DOUBLE )
		{
			return EXECPIXELOP_121(TPxL, TPixel_BGR_d, TPxR2);
		}
	}
	else if ( iSrc1ImgType == CLUVIZ_IMG_LUMINANCE )
	{
		if ( iSrc1DataType == CLUVIZ_IMG_BYTE )
		{
			return EXECPIXELOP_121(TPxL, TPixel_L_c, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_UNSIGNED_BYTE )
		{
			return EXECPIXELOP_121(TPxL, TPixel_L_uc, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_SHORT )
		{
			return EXECPIXELOP_121(TPxL, TPixel_L_s, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_UNSIGNED_SHORT )
		{
			return EXECPIXELOP_121(TPxL, TPixel_L_us, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_INT )
		{
			return EXECPIXELOP_121(TPxL, TPixel_L_i, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_UNSIGNED_INT )
		{
			return EXECPIXELOP_121(TPxL, TPixel_L_ui, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_FLOAT )
		{
			return EXECPIXELOP_121(TPxL, TPixel_L_f, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_DOUBLE )
		{
			return EXECPIXELOP_121(TPxL, TPixel_L_d, TPxR2);
		}
	}
	else if ( iSrc1ImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
	{
		if ( iSrc1DataType == CLUVIZ_IMG_BYTE )
		{
			return EXECPIXELOP_121(TPxL, TPixel_LA_c, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_UNSIGNED_BYTE )
		{
			return EXECPIXELOP_121(TPxL, TPixel_LA_uc, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_SHORT )
		{
			return EXECPIXELOP_121(TPxL, TPixel_LA_s, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_UNSIGNED_SHORT )
		{
			return EXECPIXELOP_121(TPxL, TPixel_LA_us, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_INT )
		{
			return EXECPIXELOP_121(TPxL, TPixel_LA_i, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_UNSIGNED_INT )
		{
			return EXECPIXELOP_121(TPxL, TPixel_LA_ui, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_FLOAT )
		{
			return EXECPIXELOP_121(TPxL, TPixel_LA_f, TPxR2);
		}
		else if ( iSrc1DataType == CLUVIZ_IMG_DOUBLE )
		{
			return EXECPIXELOP_121(TPxL, TPixel_LA_d, TPxR2);
		}
	}

	return true;
}






#undef EXECPIXELOP_112
#undef EXECPIXELOP_112_LEVEL1
#undef EXECPIXELOP_121
#undef EXECPIXELOP_121_LEVEL1

