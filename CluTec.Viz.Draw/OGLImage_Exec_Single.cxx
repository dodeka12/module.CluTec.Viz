////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLImage_Exec_Single.cxx
//
// summary:   ogl image execute single class
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

// Define to simplify code in ExecutePixelOperator
#define EXECPIXELOP( PXTYPE ) Functor<PXTYPE>::Execute( (PXTYPE *) rTrg.GetDataPtr(), rPar )

template< template<class TPxL> class Functor, class Parameter >
bool COGLImage::ExecutePixelOperator( COGLImage &rTrg, Parameter& rPar )
{
	int iTrgImgType, iTrgDataType, iTrgBytesPerPixel;

	rTrg.GetType( iTrgImgType, iTrgDataType, iTrgBytesPerPixel );

	if ( iTrgImgType == CLUVIZ_IMG_RGBA )
	{
		if ( iTrgDataType == CLUVIZ_IMG_BYTE )
		{
			return EXECPIXELOP(TPixel_RGBA_c);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_BYTE )
		{
			return EXECPIXELOP(TPixel_RGBA_uc);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_SHORT )
		{
			return EXECPIXELOP(TPixel_RGBA_s);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_SHORT )
		{
			return EXECPIXELOP(TPixel_RGBA_us);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_INT )
		{
			return EXECPIXELOP(TPixel_RGBA_i);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_INT )
		{
			return EXECPIXELOP(TPixel_RGBA_ui);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_FLOAT )
		{
			return EXECPIXELOP(TPixel_RGBA_f);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_DOUBLE )
		{
			return EXECPIXELOP(TPixel_RGBA_d);
		}
	}
	else if ( iTrgImgType == CLUVIZ_IMG_RGB )
	{
		if ( iTrgDataType == CLUVIZ_IMG_BYTE )
		{
			return EXECPIXELOP(TPixel_RGB_c);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_BYTE )
		{
			return EXECPIXELOP(TPixel_RGB_uc);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_SHORT )
		{
			return EXECPIXELOP(TPixel_RGB_s);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_SHORT )
		{
			return EXECPIXELOP(TPixel_RGB_us);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_INT )
		{
			return EXECPIXELOP(TPixel_RGB_i);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_INT )
		{
			return EXECPIXELOP(TPixel_RGB_ui);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_FLOAT )
		{
			return EXECPIXELOP(TPixel_RGB_f);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_DOUBLE )
		{
			return EXECPIXELOP(TPixel_RGB_d);
		}
	}
	else if ( iTrgImgType == CLUVIZ_IMG_BGRA )
	{
		if ( iTrgDataType == CLUVIZ_IMG_BYTE )
		{
			return EXECPIXELOP(TPixel_BGRA_c);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_BYTE )
		{
			return EXECPIXELOP(TPixel_BGRA_uc);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_SHORT )
		{
			return EXECPIXELOP(TPixel_BGRA_s);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_SHORT )
		{
			return EXECPIXELOP(TPixel_BGRA_us);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_INT )
		{
			return EXECPIXELOP(TPixel_BGRA_i);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_INT )
		{
			return EXECPIXELOP(TPixel_BGRA_ui);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_FLOAT )
		{
			return EXECPIXELOP(TPixel_BGRA_f);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_DOUBLE )
		{
			return EXECPIXELOP(TPixel_BGRA_d);
		}
	}
	else if ( iTrgImgType == CLUVIZ_IMG_BGR )
	{
		if ( iTrgDataType == CLUVIZ_IMG_BYTE )
		{
			return EXECPIXELOP(TPixel_BGR_c);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_BYTE )
		{
			return EXECPIXELOP(TPixel_BGR_uc);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_SHORT )
		{
			return EXECPIXELOP(TPixel_BGR_s);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_SHORT )
		{
			return EXECPIXELOP(TPixel_BGR_us);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_INT )
		{
			return EXECPIXELOP(TPixel_BGR_i);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_INT )
		{
			return EXECPIXELOP(TPixel_BGR_ui);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_FLOAT )
		{
			return EXECPIXELOP(TPixel_BGR_f);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_DOUBLE )
		{
			return EXECPIXELOP(TPixel_BGR_d);
		}
	}
	else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE )
	{
		if ( iTrgDataType == CLUVIZ_IMG_BYTE )
		{
			return EXECPIXELOP(TPixel_L_c);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_BYTE )
		{
			return EXECPIXELOP(TPixel_L_uc);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_SHORT )
		{
			return EXECPIXELOP(TPixel_L_s);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_SHORT )
		{
			return EXECPIXELOP(TPixel_L_us);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_INT )
		{
			return EXECPIXELOP(TPixel_L_i);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_INT )
		{
			return EXECPIXELOP(TPixel_L_ui);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_FLOAT )
		{
			return EXECPIXELOP(TPixel_L_f);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_DOUBLE )
		{
			return EXECPIXELOP(TPixel_L_d);
		}
	}
	else if ( iTrgImgType == CLUVIZ_IMG_LUMINANCE_ALPHA )
	{
		if ( iTrgDataType == CLUVIZ_IMG_BYTE )
		{
			return EXECPIXELOP(TPixel_LA_c);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_BYTE )
		{
			return EXECPIXELOP(TPixel_LA_uc);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_SHORT )
		{
			return EXECPIXELOP(TPixel_LA_s);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_SHORT )
		{
			return EXECPIXELOP(TPixel_LA_us);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_INT )
		{
			return EXECPIXELOP(TPixel_LA_i);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_UNSIGNED_INT )
		{
			return EXECPIXELOP(TPixel_LA_ui);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_FLOAT )
		{
			return EXECPIXELOP(TPixel_LA_f);
		}
		else if ( iTrgDataType == CLUVIZ_IMG_DOUBLE )
		{
			return EXECPIXELOP(TPixel_LA_d);
		}
	}

	return true;
}

#undef EXECPIXELOP

