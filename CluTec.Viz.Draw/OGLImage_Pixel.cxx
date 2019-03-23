////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLImage_Pixel.cxx
//
// summary:   ogl image pixel class
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

// Convert float to pixel value
template<>
void COGLImage::Float2Pixel( char& rPx, float fVal )
{
	fVal = Clamp(fVal);

	if ( fVal > 0.0f )
		fVal *= float(CHAR_MAX);
	else
		fVal *= -float(CHAR_MIN);

	if (fVal > float(CHAR_MAX))
		rPx = (char) CHAR_MAX;
	else if ( fVal < float(CHAR_MIN) )
		rPx = (char) CHAR_MIN;
	else
		rPx = (char) fVal;
}

template<>
void COGLImage::Float2Pixel( unsigned char& rPx, float fVal )
{
	fVal = Clamp(fVal);
	fVal *= float(UCHAR_MAX);

	if (fVal > float(UCHAR_MAX))
		rPx = (unsigned char) UCHAR_MAX;
	else if ( fVal < 0.0f )
		rPx = 0;
	else
		rPx = (unsigned char) fVal;
}

template<>
void COGLImage::Float2Pixel( short& rPx, float fVal )
{
	fVal = Clamp(fVal);
	if ( fVal > 0.0f )
		fVal *= float(SHRT_MAX);
	else
		fVal *= -float(SHRT_MIN);

	if (fVal > float(SHRT_MAX))
		rPx = (short) SHRT_MAX;
	else if ( fVal < float(SHRT_MIN) )
		rPx = (short) SHRT_MIN;
	else
		rPx = (short) fVal;
}

template<>
void COGLImage::Float2Pixel( unsigned short& rPx, float fVal )
{
	fVal = Clamp(fVal);
	fVal *= float(USHRT_MAX);

	if (fVal > float(USHRT_MAX))
		rPx = (unsigned short) USHRT_MAX;
	else if ( fVal < 0.0f )
		rPx = 0;
	else
		rPx = (unsigned short) fVal;
}


template<>
void COGLImage::Float2Pixel( int& rPx, float fVal )
{
	fVal = Clamp(fVal);
	if ( fVal > 0.0f )
		fVal *= float(INT_MAX);
	else
		fVal *= -float(INT_MIN);

	if (fVal > float(INT_MAX))
		rPx = (int) INT_MAX;
	else if ( fVal < float(INT_MIN) )
		rPx = (int) INT_MIN;
	else
		rPx = (int) fVal;
}

template<>
void COGLImage::Float2Pixel( unsigned int& rPx, float fVal )
{
	fVal = Clamp(fVal);
	fVal *= float(UINT_MAX);

	if (fVal > float(UINT_MAX))
		rPx = (unsigned int) UINT_MAX;
	else if ( fVal < 0.0f )
		rPx = 0;
	else
		rPx = (unsigned int) fVal;
}

template<>
void COGLImage::Float2Pixel( float& rPx, float fVal )
{
	rPx = fVal;
}

template<>
void COGLImage::Float2Pixel( double& rPx, float fVal )
{
	rPx = double( fVal );
}




// Convert pixel value to float
template<>
void COGLImage::Pixel2Float( float& fVal, char rPx  )
{
	if ( rPx > char(0) )
		fVal = float(rPx) / float(CHAR_MAX);
	else
		fVal = -float(rPx) / float(CHAR_MIN);
}

template<>
void COGLImage::Pixel2Float( float& fVal, unsigned char rPx  )
{
	fVal = float(rPx) / float(UCHAR_MAX);
}

template<>
void COGLImage::Pixel2Float( float& fVal, short rPx  )
{
	if ( rPx > short(0) )
		fVal = float(rPx) / float(SHRT_MAX);
	else
		fVal = -float(rPx) / float(SHRT_MIN);
}

template<>
void COGLImage::Pixel2Float( float& fVal, unsigned short rPx  )
{
	fVal = float(rPx) / float(USHRT_MAX);
}


template<>
void COGLImage::Pixel2Float( float& fVal, int rPx  )
{
	if ( rPx > int(0) )
		fVal = float(rPx) / float(INT_MAX);
	else
		fVal = -float(rPx) / float(INT_MIN);
}

template<>
void COGLImage::Pixel2Float( float& fVal, unsigned int rPx  )
{
	fVal = float(rPx) / float(UINT_MAX);
}

template<>
void COGLImage::Pixel2Float( float& fVal, float rPx  )
{
	fVal = rPx;
}

template<>
void COGLImage::Pixel2Float( float& fVal, double rPx  )
{
	fVal = float(rPx);
}

/////////////////////////////////////////////////////////////////
// Pixel2Float and Float2Pixel for double values

// Convert double to pixel value
template<>
void COGLImage::Float2Pixel( char& rPx, double fVal )
{
	fVal = Clamp(fVal);
	if ( fVal > 0.0f )
		fVal *= double(CHAR_MAX);
	else
		fVal *= -double(CHAR_MIN);

	if (fVal > double(CHAR_MAX))
		rPx = (char) CHAR_MAX;
	else if ( fVal < double(CHAR_MIN) )
		rPx = (char) CHAR_MIN;
	else
		rPx = (char) fVal;
}

template<>
void COGLImage::Float2Pixel( unsigned char& rPx, double fVal )
{
	fVal = Clamp(fVal);
	fVal *= double(UCHAR_MAX);

	if (fVal > double(UCHAR_MAX))
		rPx = (unsigned char) UCHAR_MAX;
	else if ( fVal < 0.0f )
		rPx = 0;
	else
		rPx = (unsigned char) fVal;
}

template<>
void COGLImage::Float2Pixel( short& rPx, double fVal )
{
	fVal = Clamp(fVal);
	if ( fVal > 0.0f )
		fVal *= double(SHRT_MAX);
	else
		fVal *= -double(SHRT_MIN);

	if (fVal > double(SHRT_MAX))
		rPx = (short) SHRT_MAX;
	else if ( fVal < double(SHRT_MIN) )
		rPx = (short) SHRT_MIN;
	else
		rPx = (short) fVal;
}

template<>
void COGLImage::Float2Pixel( unsigned short& rPx, double fVal )
{
	fVal = Clamp(fVal);
	fVal *= double(USHRT_MAX);

	if (fVal > double(USHRT_MAX))
		rPx = (unsigned short) USHRT_MAX;
	else if ( fVal < 0.0f )
		rPx = 0;
	else
		rPx = (unsigned short) fVal;
}


template<>
void COGLImage::Float2Pixel( int& rPx, double fVal )
{
	fVal = Clamp(fVal);
	if ( fVal > 0.0f )
		fVal *= double(INT_MAX);
	else
		fVal *= -double(INT_MIN);

	if (fVal > double(INT_MAX))
		rPx = (int) INT_MAX;
	else if ( fVal < double(INT_MIN) )
		rPx = (int) INT_MIN;
	else
		rPx = (int) fVal;
}

template<>
void COGLImage::Float2Pixel( unsigned int& rPx, double fVal )
{
	fVal = Clamp(fVal);
	fVal *= double(UINT_MAX);

	if (fVal > double(UINT_MAX))
		rPx = (unsigned int) UINT_MAX;
	else if ( fVal < 0.0f )
		rPx = 0;
	else
		rPx = (unsigned int) fVal;
}

template<>
void COGLImage::Float2Pixel( float& rPx, double fVal )
{
	if ( fVal > double(std::numeric_limits<float>::max()) )
		rPx = std::numeric_limits<float>::max();
	else
		rPx = float( fVal );
}

template<>
void COGLImage::Float2Pixel( double& rPx, double fVal )
{
	rPx = double( fVal );
}



// Convert pixel value to double
template<>
void COGLImage::Pixel2Float( double& fVal, char rPx  )
{
	if ( rPx > char(0) )
		fVal = double(rPx) / double(CHAR_MAX);
	else
		fVal = -double(rPx) / double(CHAR_MIN);
}

template<>
void COGLImage::Pixel2Float( double& fVal, unsigned char rPx  )
{
	fVal = double(rPx) / double(UCHAR_MAX);
}

template<>
void COGLImage::Pixel2Float( double& fVal, short rPx  )
{
	if ( rPx > short(0) )
		fVal = double(rPx) / double(SHRT_MAX);
	else
		fVal = -double(rPx) / double(SHRT_MIN);
}

template<>
void COGLImage::Pixel2Float( double& fVal, unsigned short rPx  )
{
	fVal = double(rPx) / double(USHRT_MAX);
}


template<>
void COGLImage::Pixel2Float( double& fVal, int rPx  )
{
	if ( rPx > int(0) )
		fVal = double(rPx) / double(INT_MAX);
	else
		fVal = -double(rPx) / double(INT_MIN);
}

template<>
void COGLImage::Pixel2Float( double& fVal, unsigned int rPx  )
{
	fVal = double(rPx) / double(UINT_MAX);
}

template<>
void COGLImage::Pixel2Float( double& fVal, float rPx  )
{
	fVal = double( rPx );
}

template<>
void COGLImage::Pixel2Float( double& fVal, double rPx  )
{
	fVal = double(rPx);
}



///////////////////////////////////////////////////////////////////
// Pixel Bit AND
template <class TDataType>
TDataType COGLImage::PixelBitAND( TDataType xL, TDataType xR )
{
	return (xL & xR);
}

template<>
float COGLImage::PixelBitAND( float xL, float xR )
{
	float fA;
	int iL, iR;

	Float2Pixel( iL, xL );
	Float2Pixel( iR, xR );

	Pixel2Float( fA, iL & iR );

	return fA;
}

template<>
double COGLImage::PixelBitAND( double xL, double xR )
{
	double fA;
	int iL, iR;

	Float2Pixel( iL, xL );
	Float2Pixel( iR, xR );

	Pixel2Float( fA, iL & iR );

	return fA;
}


// Pixel Bit OR
template <class TDataType>
TDataType COGLImage::PixelBitOR( TDataType xL, TDataType xR )
{
	return (xL | xR);
}

template<>
float COGLImage::PixelBitOR( float xL, float xR )
{
	float fA;
	int iL, iR;

	Float2Pixel( iL, xL );
	Float2Pixel( iR, xR );

	Pixel2Float( fA, iL | iR );

	return fA;
}

template<>
double COGLImage::PixelBitOR( double xL, double xR )
{
	double fA;
	int iL, iR;

	Float2Pixel( iL, xL );
	Float2Pixel( iR, xR );

	Pixel2Float( fA, iL | iR );

	return fA;
}

////////////////////////////////////////////////////
//// Negate Pixel value

template<>
char COGLImage::PixelNeg( char xR )
{
	return -xR;
}

template<>
unsigned char COGLImage::PixelNeg( unsigned char xR )
{
	return 0;
}

template<>
short COGLImage::PixelNeg( short xR )
{
	return -xR;
}

template<>
unsigned short COGLImage::PixelNeg( unsigned short xR )
{
	return 0;
}

template<>
int COGLImage::PixelNeg( int xR )
{
	return -xR;
}

template<>
unsigned int COGLImage::PixelNeg( unsigned int xR )
{
	return 0;
}

template<>
float COGLImage::PixelNeg( float xR )
{
	return -xR;
}

template<>
double COGLImage::PixelNeg( double xR )
{
	return -xR;
}

//////////////////////////////////////////////////////////////////
//// Value Range Specializations

template<>
COGLImage::TValueRange<char>::TValueRange()
{
	tMin = SCHAR_MIN;
	tMax = SCHAR_MAX;
	dMin = double( tMin );
	dMax = double( tMax );
}

template<>
COGLImage::TValueRange<unsigned char>::TValueRange()
{
	tMin = (unsigned char) 0;
	tMax = UCHAR_MAX;
	dMin = double( tMin );
	dMax = double( tMax );
}

template<>
COGLImage::TValueRange<__int16>::TValueRange()
{
	tMin = SHRT_MIN;
	tMax = SHRT_MAX;
	dMin = double( tMin );
	dMax = double( tMax );
}

template<>
COGLImage::TValueRange<unsigned __int16>::TValueRange()
{
	tMin = 0;
	tMax = USHRT_MAX;
	dMin = double( tMin );
	dMax = double( tMax );
}

template<>
COGLImage::TValueRange<__int32>::TValueRange()
{
	tMin = INT_MIN;
	tMax = INT_MAX;
	dMin = double( tMin );
	dMax = double( tMax );
}

template<>
COGLImage::TValueRange<unsigned __int32>::TValueRange()
{
	tMin = 0;
	tMax = UINT_MAX;
	dMin = double( tMin );
	dMax = double( tMax );
}

template<>
COGLImage::TValueRange<float>::TValueRange()
{
	tMin = 0.0f;
	tMax = 1.0f;
	dMin = double( tMin );
	dMax = double( tMax );
}

template<>
COGLImage::TValueRange<double>::TValueRange()
{
	tMin = 0.0;
	tMax = 1.0;
	dMin = double( tMin );
	dMax = double( tMax );
}

