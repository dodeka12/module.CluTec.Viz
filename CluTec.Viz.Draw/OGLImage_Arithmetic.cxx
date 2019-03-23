////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLImage_Arithmetic.cxx
//
// summary:   ogl image arithmetic class
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

#define CLU_DBL_MAX (std::numeric_limits<double>::max())
#define CLU_DBL_MIN (std::numeric_limits<double>::min())

/// Divide Images
template < class TPxL, class TPxR1, class TPxR2 >
bool COGLImage::FOpDivImg<TPxL,TPxR1,TPxR2>::Execute( TPxL *pTrg, TPxR1 *pSrc1, TPxR2 *pSrc2, const PPixCnt& rPar )
{
	int iPix;
	double dPix1, dPix2;


	if ( TPxL::uColorCnt == 3 )
	{
		if ( TPxR1::uColorCnt == 3 && TPxR2::uColorCnt == 3 )
		{
			for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
			{
				Pixel2Float( dPix1, pSrc1[iPix].r() );
				Pixel2Float( dPix2, pSrc2[iPix].r() );
				if ( dPix2 == 0.0 )
					Float2Pixel( pTrg[iPix].r(), CLU_DBL_MAX );
				else
					Float2Pixel( pTrg[iPix].r(), dPix1 / dPix2 );

				Pixel2Float( dPix1, pSrc1[iPix].g() );
				Pixel2Float( dPix2, pSrc2[iPix].g() );
				if ( dPix2 == 0.0 )
					Float2Pixel( pTrg[iPix].g(), CLU_DBL_MAX );
				else
					Float2Pixel( pTrg[iPix].g(), dPix1 / dPix2 );

				Pixel2Float( dPix1, pSrc1[iPix].b() );
				Pixel2Float( dPix2, pSrc2[iPix].b() );
				if ( dPix2 == 0.0 )
					Float2Pixel( pTrg[iPix].b(), CLU_DBL_MAX );
				else
					Float2Pixel( pTrg[iPix].b(), dPix1 / dPix2 );

				if ( TPxL::uAlphaCnt )
					Float2Pixel( pTrg[iPix].a(), 1.0 );
			}
		}
		else if ( TPxR1::uColorCnt == 3 && TPxR2::uColorCnt == 1 )
		{
			for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
			{
				Pixel2Float( dPix2, pSrc2[iPix].r() );
				if ( dPix2 == 0.0 )
				{
					Float2Pixel( pTrg[iPix].r(), CLU_DBL_MAX );
					Float2Pixel( pTrg[iPix].g(), CLU_DBL_MAX );
					Float2Pixel( pTrg[iPix].b(), CLU_DBL_MAX );
				}
				else
				{
					Pixel2Float( dPix1, pSrc1[iPix].r() );
					Float2Pixel( pTrg[iPix].r(), dPix1 / dPix2 );

					Pixel2Float( dPix1, pSrc1[iPix].g() );
					Float2Pixel( pTrg[iPix].g(), dPix1 / dPix2 );

					Pixel2Float( dPix1, pSrc1[iPix].b() );
					Float2Pixel( pTrg[iPix].b(), dPix1 / dPix2 );
				}

				if ( TPxL::uAlphaCnt )
					Float2Pixel( pTrg[iPix].a(), 1.0 );
			}
		}
		else if ( TPxR1::uColorCnt == 1 && TPxR2::uColorCnt == 3 )
		{
			for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
			{
				Pixel2Float( dPix1, pSrc1[iPix].r() );

				Pixel2Float( dPix2, pSrc2[iPix].r() );
				if ( dPix2 == 0.0 )
					Float2Pixel( pTrg[iPix].r(), CLU_DBL_MAX );
				else
					Float2Pixel( pTrg[iPix].r(), dPix1 / dPix2 );

				Pixel2Float( dPix2, pSrc2[iPix].g() );
				if ( dPix2 == 0.0 )
					Float2Pixel( pTrg[iPix].g(), CLU_DBL_MAX );
				else
					Float2Pixel( pTrg[iPix].g(), dPix1 / dPix2 );

				Pixel2Float( dPix2, pSrc2[iPix].b() );
				if ( dPix2 == 0.0 )
					Float2Pixel( pTrg[iPix].b(), CLU_DBL_MAX );
				else
					Float2Pixel( pTrg[iPix].b(), dPix1 / dPix2 );

				if ( TPxL::uAlphaCnt )
					Float2Pixel( pTrg[iPix].a(), 1.0 );
			}
		}
		else if ( TPxR1::uColorCnt == 1 && TPxR2::uColorCnt == 1 )
		{
			for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
			{
				Pixel2Float( dPix1, pSrc1[iPix].r() );
				Pixel2Float( dPix2, pSrc2[iPix].r() );
				if ( dPix2 == 0.0 )
					dPix1 = CLU_DBL_MAX;
				else
					dPix1 /= dPix2;


				Float2Pixel( pTrg[iPix].r(), dPix1 );
				Float2Pixel( pTrg[iPix].g(), dPix1 );
				Float2Pixel( pTrg[iPix].b(), dPix1 );

				if ( TPxL::uAlphaCnt )
					Float2Pixel( pTrg[iPix].a(), 1.0 );
			}
		}
	}
	else if ( TPxL::uColorCnt == 1 )
	{
		for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
		{
			Pixel2Float( dPix1, pSrc1[iPix].r() );
			Pixel2Float( dPix2, pSrc2[iPix].r() );
			if ( dPix2 == 0.0 )
				dPix1 = CLU_DBL_MAX;
			else
				dPix1 /= dPix2;

			Float2Pixel( pTrg[iPix].r(), dPix1 );

			if ( TPxL::uAlphaCnt )
				Float2Pixel( pTrg[iPix].a(), 1.0 );
		}
	}

	return true;
}


/// Multiply Images
template < class TPxL, class TPxR1, class TPxR2 >
bool COGLImage::FOpMultImg<TPxL,TPxR1,TPxR2>::Execute( TPxL *pTrg, TPxR1 *pSrc1, TPxR2 *pSrc2, const PPixCnt& rPar )
{
	int iPix;
	double dPix1, dPix2;

	if ( TPxL::uColorCnt == 3 )
	{
		if ( TPxR1::uColorCnt == 3 && TPxR2::uColorCnt == 3 )
		{
			for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
			{
				Pixel2Float( dPix1, pSrc1[iPix].r() );
				Pixel2Float( dPix2, pSrc2[iPix].r() );
				Float2Pixel( pTrg[iPix].r(), dPix1 * dPix2 );

				Pixel2Float( dPix1, pSrc1[iPix].g() );
				Pixel2Float( dPix2, pSrc2[iPix].g() );
				Float2Pixel( pTrg[iPix].g(), dPix1 * dPix2 );

				Pixel2Float( dPix1, pSrc1[iPix].b() );
				Pixel2Float( dPix2, pSrc2[iPix].b() );
				Float2Pixel( pTrg[iPix].b(), dPix1 * dPix2 );

				if ( TPxL::uAlphaCnt )
					Float2Pixel( pTrg[iPix].a(), 1.0 );
			}
		}
		else if ( TPxR1::uColorCnt == 3 && TPxR2::uColorCnt == 1 )
		{
			for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
			{
				Pixel2Float( dPix2, pSrc2[iPix].r() );

				Pixel2Float( dPix1, pSrc1[iPix].r() );
				Float2Pixel( pTrg[iPix].r(), dPix1 * dPix2 );

				Pixel2Float( dPix1, pSrc1[iPix].g() );
				Float2Pixel( pTrg[iPix].g(), dPix1 * dPix2 );

				Pixel2Float( dPix1, pSrc1[iPix].b() );
				Float2Pixel( pTrg[iPix].b(), dPix1 * dPix2 );

				if ( TPxL::uAlphaCnt )
					Float2Pixel( pTrg[iPix].a(), 1.0 );
			}
		}
		else if ( TPxR1::uColorCnt == 1 && TPxR2::uColorCnt == 3 )
		{
			for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
			{
				Pixel2Float( dPix1, pSrc1[iPix].r() );

				Pixel2Float( dPix2, pSrc2[iPix].r() );
				Float2Pixel( pTrg[iPix].r(), dPix1 * dPix2 );

				Pixel2Float( dPix2, pSrc2[iPix].g() );
				Float2Pixel( pTrg[iPix].g(), dPix1 * dPix2 );

				Pixel2Float( dPix2, pSrc2[iPix].b() );
				Float2Pixel( pTrg[iPix].b(), dPix1 * dPix2 );

				if ( TPxL::uAlphaCnt )
					Float2Pixel( pTrg[iPix].a(), 1.0 );
			}
		}
		else if ( TPxR1::uColorCnt == 1 && TPxR2::uColorCnt == 1 )
		{
			for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
			{
				Pixel2Float( dPix1, pSrc1[iPix].r() );
				Pixel2Float( dPix2, pSrc2[iPix].r() );
				dPix1 *= dPix2;
				
				Float2Pixel( pTrg[iPix].r(), dPix1 );
				Float2Pixel( pTrg[iPix].g(), dPix1 );
				Float2Pixel( pTrg[iPix].b(), dPix1 );

				if ( TPxL::uAlphaCnt )
					Float2Pixel( pTrg[iPix].a(), 1.0 );
			}
		}
	}
	else if ( TPxL::uColorCnt == 1 )
	{
		for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
		{
			Pixel2Float( dPix1, pSrc1[iPix].r() );
			Pixel2Float( dPix2, pSrc2[iPix].r() );
			Float2Pixel( pTrg[iPix].r(), dPix1 * dPix2 );

			if ( TPxL::uAlphaCnt )
				Float2Pixel( pTrg[iPix].a(), 1.0 );
		}
	}


	return true;
}

/// Add Images
template < class TPxL, class TPxR1, class TPxR2 >
bool COGLImage::FOpAddImg<TPxL,TPxR1,TPxR2>::Execute( TPxL *pTrg, TPxR1 *pSrc1, TPxR2 *pSrc2, const PPixCnt& rPar )
{
	int iPix;
	double dPix1, dPix2;

	if ( TPxL::uColorCnt == 3 )
	{
		if ( TPxR1::uColorCnt == 3 && TPxR2::uColorCnt == 3 )
		{
			for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
			{
				Pixel2Float( dPix1, pSrc1[iPix].r() );
				Pixel2Float( dPix2, pSrc2[iPix].r() );
				Float2Pixel( pTrg[iPix].r(), dPix1 + dPix2 );

				Pixel2Float( dPix1, pSrc1[iPix].g() );
				Pixel2Float( dPix2, pSrc2[iPix].g() );
				Float2Pixel( pTrg[iPix].g(), dPix1 + dPix2 );

				Pixel2Float( dPix1, pSrc1[iPix].b() );
				Pixel2Float( dPix2, pSrc2[iPix].b() );
				Float2Pixel( pTrg[iPix].b(), dPix1 + dPix2 );

				if ( TPxL::uAlphaCnt )
					Float2Pixel( pTrg[iPix].a(), 1.0 );
			}
		}
		else if ( TPxR1::uColorCnt == 3 && TPxR2::uColorCnt == 1 )
		{
			for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
			{
				Pixel2Float( dPix2, pSrc2[iPix].r() );

				Pixel2Float( dPix1, pSrc1[iPix].r() );
				Float2Pixel( pTrg[iPix].r(), dPix1 + dPix2 );

				Pixel2Float( dPix1, pSrc1[iPix].g() );
				Float2Pixel( pTrg[iPix].g(), dPix1 + dPix2 );

				Pixel2Float( dPix1, pSrc1[iPix].b() );
				Float2Pixel( pTrg[iPix].b(), dPix1 + dPix2 );

				if ( TPxL::uAlphaCnt )
					Float2Pixel( pTrg[iPix].a(), 1.0 );
			}
		}
		else if ( TPxR1::uColorCnt == 1 && TPxR2::uColorCnt == 3 )
		{
			for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
			{
				Pixel2Float( dPix1, pSrc1[iPix].r() );

				Pixel2Float( dPix2, pSrc2[iPix].r() );
				Float2Pixel( pTrg[iPix].r(), dPix1 + dPix2 );

				Pixel2Float( dPix2, pSrc2[iPix].g() );
				Float2Pixel( pTrg[iPix].g(), dPix1 + dPix2 );

				Pixel2Float( dPix2, pSrc2[iPix].b() );
				Float2Pixel( pTrg[iPix].b(), dPix1 + dPix2 );

				if ( TPxL::uAlphaCnt )
					Float2Pixel( pTrg[iPix].a(), 1.0 );
			}
		}
		else if ( TPxR1::uColorCnt == 1 && TPxR2::uColorCnt == 1 )
		{
			for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
			{
				Pixel2Float( dPix1, pSrc1[iPix].r() );
				Pixel2Float( dPix2, pSrc2[iPix].r() );
				dPix1 += dPix2;
				
				Float2Pixel( pTrg[iPix].r(), dPix1 );
				Float2Pixel( pTrg[iPix].g(), dPix1 );
				Float2Pixel( pTrg[iPix].b(), dPix1 );

				if ( TPxL::uAlphaCnt )
					Float2Pixel( pTrg[iPix].a(), 1.0 );
			}
		}
	}
	else if ( TPxL::uColorCnt == 1 )
	{
		for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
		{
			Pixel2Float( dPix1, pSrc1[iPix].r() );
			Pixel2Float( dPix2, pSrc2[iPix].r() );
			Float2Pixel( pTrg[iPix].r(), dPix1 + dPix2 );

			if ( TPxL::uAlphaCnt )
				Float2Pixel( pTrg[iPix].a(), 1.0 );
		}
	}


	return true;
}

/// Sub Images
template < class TPxL, class TPxR1, class TPxR2 >
bool COGLImage::FOpSubImg<TPxL,TPxR1,TPxR2>::Execute( TPxL *pTrg, TPxR1 *pSrc1, TPxR2 *pSrc2, const PPixCnt& rPar )
{
	int iPix;
	double dPix1, dPix2;

	if ( TPxL::uColorCnt == 3 )
	{
		if ( TPxR1::uColorCnt == 3 && TPxR2::uColorCnt == 3 )
		{
			for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
			{
				Pixel2Float( dPix1, pSrc1[iPix].r() );
				Pixel2Float( dPix2, pSrc2[iPix].r() );
				Float2Pixel( pTrg[iPix].r(), dPix1 - dPix2 );

				Pixel2Float( dPix1, pSrc1[iPix].g() );
				Pixel2Float( dPix2, pSrc2[iPix].g() );
				Float2Pixel( pTrg[iPix].g(), dPix1 - dPix2 );

				Pixel2Float( dPix1, pSrc1[iPix].b() );
				Pixel2Float( dPix2, pSrc2[iPix].b() );
				Float2Pixel( pTrg[iPix].b(), dPix1 - dPix2 );

				if ( TPxL::uAlphaCnt )
					Float2Pixel( pTrg[iPix].a(), 1.0 );
			}
		}
		else if ( TPxR1::uColorCnt == 3 && TPxR2::uColorCnt == 1 )
		{
			for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
			{
				Pixel2Float( dPix2, pSrc2[iPix].r() );

				Pixel2Float( dPix1, pSrc1[iPix].r() );
				Float2Pixel( pTrg[iPix].r(), dPix1 - dPix2 );

				Pixel2Float( dPix1, pSrc1[iPix].g() );
				Float2Pixel( pTrg[iPix].g(), dPix1 - dPix2 );

				Pixel2Float( dPix1, pSrc1[iPix].b() );
				Float2Pixel( pTrg[iPix].b(), dPix1 - dPix2 );

				if ( TPxL::uAlphaCnt )
					Float2Pixel( pTrg[iPix].a(), 1.0 );
			}
		}
		else if ( TPxR1::uColorCnt == 1 && TPxR2::uColorCnt == 3 )
		{
			for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
			{
				Pixel2Float( dPix1, pSrc1[iPix].r() );

				Pixel2Float( dPix2, pSrc2[iPix].r() );
				Float2Pixel( pTrg[iPix].r(), dPix1 - dPix2 );

				Pixel2Float( dPix2, pSrc2[iPix].g() );
				Float2Pixel( pTrg[iPix].g(), dPix1 - dPix2 );

				Pixel2Float( dPix2, pSrc2[iPix].b() );
				Float2Pixel( pTrg[iPix].b(), dPix1 - dPix2 );

				if ( TPxL::uAlphaCnt )
					Float2Pixel( pTrg[iPix].a(), 1.0 );
			}
		}
		else if ( TPxR1::uColorCnt == 1 && TPxR2::uColorCnt == 1 )
		{
			for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
			{
				Pixel2Float( dPix1, pSrc1[iPix].r() );
				Pixel2Float( dPix2, pSrc2[iPix].r() );
				dPix1 -= dPix2;
				
				Float2Pixel( pTrg[iPix].r(), dPix1 );
				Float2Pixel( pTrg[iPix].g(), dPix1 );
				Float2Pixel( pTrg[iPix].b(), dPix1 );

				if ( TPxL::uAlphaCnt )
					Float2Pixel( pTrg[iPix].a(), 1.0 );
			}
		}
	}
	else if ( TPxL::uColorCnt == 1 )
	{
		for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
		{
			Pixel2Float( dPix1, pSrc1[iPix].r() );
			Pixel2Float( dPix2, pSrc2[iPix].r() );
			Float2Pixel( pTrg[iPix].r(), dPix1 - dPix2 );

			if ( TPxL::uAlphaCnt )
				Float2Pixel( pTrg[iPix].a(), 1.0 );
		}
	}


	return true;
}




/// Multiply Image with scalar value
template < class TPxL, class TPxR >
bool COGLImage::FOpMultScalar<TPxL, TPxR>::Execute( TPxL *pTrg, TPxL *pSrc, const POpScalar& rPar )
{
	TValueRange<TPxL::TType> xRange;
	int iIdx, iCnt = rPar.iWidth * rPar.iHeight;
	double dPix;

	if ( pSrc[0].uColorCnt == 3 )
	{
		for(iIdx = 0; iIdx < iCnt; ++iIdx)
		{
			TPxL& rTrgPix = pTrg[iIdx];
			TPxR& rSrcPix = pSrc[iIdx];

			Pixel2Float( dPix, rSrcPix.r() );
			Float2Pixel( rTrgPix.r(), dPix * rPar.dValue );

			Pixel2Float( dPix, rSrcPix.g() );
			Float2Pixel( rTrgPix.g(), dPix * rPar.dValue );

			Pixel2Float( dPix, rSrcPix.b() );
			Float2Pixel( rTrgPix.b(), dPix * rPar.dValue );

			//dPix = double( rSrcPix.r() ) * rPar.dValue;
			//if ( dPix > xRange.dMax )
			//	rTrgPix.r() = xRange.tMax;
			//else if ( dPix < xRange.dMin )
			//	rTrgPix.r() = xRange.tMin;
			//else
			//	rTrgPix.r() = TPxL::TType( dPix );

			//dPix = double( rSrcPix.g() ) * rPar.dValue;
			//if ( dPix > xRange.dMax )
			//	rTrgPix.g() = xRange.tMax;
			//else if ( dPix < xRange.dMin )
			//	rTrgPix.g() = xRange.tMin;
			//else
			//	rTrgPix.g() = TPxL::TType( dPix );

			//dPix = double( rSrcPix.b() ) * rPar.dValue;
			//if ( dPix > xRange.dMax )
			//	rTrgPix.b() = xRange.tMax;
			//else if ( dPix < xRange.dMin )
			//	rTrgPix.b() = xRange.tMin;
			//else
			//	rTrgPix.b() = TPxL::TType( dPix );

			if ( rTrgPix.uAlphaCnt )
			{
				if ( rSrcPix.uAlphaCnt )
				{
					rTrgPix.a() = rSrcPix.a();
				}
				else
				{
					rTrgPix.a() = xRange.tMax;
				}
			}
		}
	}
	else if ( pSrc[0].uColorCnt == 1 )
	{
		for(iIdx = 0; iIdx < iCnt; ++iIdx)
		{
			TPxL& rTrgPix = pTrg[iIdx];
			TPxR& rSrcPix = pSrc[iIdx];

			Pixel2Float( dPix, rSrcPix.r() );
			Float2Pixel( rTrgPix.r(), dPix * rPar.dValue );

			//dPix = double( rSrcPix.r() ) * rPar.dValue;
			//if ( dPix > xRange.dMax )
			//	rTrgPix.r() = xRange.tMax;
			//else if ( dPix < xRange.dMin )
			//	rTrgPix.r() = xRange.tMin;
			//else
			//	rTrgPix.r() = TPxL::TType( dPix );

			if ( rTrgPix.uAlphaCnt )
			{
				if ( rSrcPix.uAlphaCnt )
				{
					rTrgPix.a() = rSrcPix.a();
				}
				else
				{
					rTrgPix.a() = xRange.tMax;
				}
			}
		}
	}

	return true;
}


/// Take reciprocal of each pixel and multiply with scalar
template < class TPxL, class TPxR >
bool COGLImage::FOpRecipScalar<TPxL, TPxR>::Execute( TPxL *pTrg, TPxL *pSrc, const POpScalar& rPar )
{
	TValueRange<TPxL::TType> xRange;
	int iIdx, iCnt = rPar.iWidth * rPar.iHeight;
	double dPix;

	if ( pSrc[0].uColorCnt == 3 )
	{
		for(iIdx = 0; iIdx < iCnt; ++iIdx)
		{
			TPxL& rTrgPix = pTrg[iIdx];
			TPxR& rSrcPix = pSrc[iIdx];

			Pixel2Float( dPix, rSrcPix.r() );
			if ( dPix >= 0 && dPix <= DBL_EPSILON )
				Float2Pixel( rTrgPix.r(), CLU_DBL_MAX );
			else if ( dPix >= -DBL_EPSILON && dPix <= 0 )
				Float2Pixel( rTrgPix.r(), CLU_DBL_MIN );
			else
				Float2Pixel( rTrgPix.r(), rPar.dValue / dPix );

			Pixel2Float( dPix, rSrcPix.g() );
			if ( dPix >= 0 && dPix <= DBL_EPSILON )
				Float2Pixel( rTrgPix.g(), CLU_DBL_MAX );
			else if ( dPix >= -DBL_EPSILON && dPix <= 0 )
				Float2Pixel( rTrgPix.g(), CLU_DBL_MIN );
			else
				Float2Pixel( rTrgPix.g(), rPar.dValue / dPix );

			Pixel2Float( dPix, rSrcPix.b() );
			if ( dPix >= 0 && dPix <= DBL_EPSILON )
				Float2Pixel( rTrgPix.b(), CLU_DBL_MAX );
			else if ( dPix >= -DBL_EPSILON && dPix <= 0 )
				Float2Pixel( rTrgPix.b(), CLU_DBL_MIN );
			else
				Float2Pixel( rTrgPix.b(), rPar.dValue / dPix );



			//dPix = dFactor / double( rSrcPix.r() );
			//if ( dPix > xRange.dMax )
			//	rTrgPix.r() = xRange.tMax;
			//else if ( dPix < xRange.dMin )
			//	rTrgPix.r() = xRange.tMin;
			//else
			//	rTrgPix.r() = TPxL::TType( dPix );

			//dPix = dFactor / double( rSrcPix.g() );
			//if ( dPix > xRange.dMax )
			//	rTrgPix.g() = xRange.tMax;
			//else if ( dPix < xRange.dMin )
			//	rTrgPix.g() = xRange.tMin;
			//else
			//	rTrgPix.g() = TPxL::TType( dPix );

			//dPix = dFactor / double( rSrcPix.b() );
			//if ( dPix > xRange.dMax )
			//	rTrgPix.b() = xRange.tMax;
			//else if ( dPix < xRange.dMin )
			//	rTrgPix.b() = xRange.tMin;
			//else
			//	rTrgPix.b() = TPxL::TType( dPix );

			if ( rTrgPix.uAlphaCnt )
			{
				if ( rSrcPix.uAlphaCnt )
				{
					rTrgPix.a() = rSrcPix.a();
				}
				else
				{
					rTrgPix.a() = xRange.tMax;
				}
			}
		}
	}
	else if ( pSrc[0].uColorCnt == 1 )
	{
		for(iIdx = 0; iIdx < iCnt; ++iIdx)
		{
			TPxL& rTrgPix = pTrg[iIdx];
			TPxR& rSrcPix = pSrc[iIdx];

			Pixel2Float( dPix, rSrcPix.r() );
			if ( dPix >= 0 && dPix <= DBL_EPSILON )
				Float2Pixel( rTrgPix.r(), CLU_DBL_MAX );
			else if ( dPix >= -DBL_EPSILON && dPix <= 0 )
				Float2Pixel( rTrgPix.r(), CLU_DBL_MIN );
			else
				Float2Pixel( rTrgPix.r(), rPar.dValue / dPix );

			if ( rTrgPix.uAlphaCnt )
			{
				if ( rSrcPix.uAlphaCnt )
				{
					rTrgPix.a() = rSrcPix.a();
				}
				else
				{
					rTrgPix.a() = xRange.tMax;
				}
			}
		}
	}

	return true;
}

/// Add scalar value to Image
template < class TPxL, class TPxR >
bool COGLImage::FOpAddScalar<TPxL, TPxR>::Execute( TPxL *pTrg, TPxL *pSrc, const POpScalar& rPar )
{
	TValueRange<TPxL::TType> xRange;
	int iIdx, iCnt = rPar.iWidth * rPar.iHeight;
	double dPix;

	if ( pSrc[0].uColorCnt == 3 )
	{
		for(iIdx = 0; iIdx < iCnt; ++iIdx)
		{
			TPxL& rTrgPix = pTrg[iIdx];
			TPxR& rSrcPix = pSrc[iIdx];

			Pixel2Float( dPix, rSrcPix.r() );
			Float2Pixel( rTrgPix.r(), dPix + rPar.dValue );

			Pixel2Float( dPix, rSrcPix.g() );
			Float2Pixel( rTrgPix.g(), dPix + rPar.dValue );

			Pixel2Float( dPix, rSrcPix.b() );
			Float2Pixel( rTrgPix.b(), dPix + rPar.dValue );

			if ( rTrgPix.uAlphaCnt )
			{
				if ( rSrcPix.uAlphaCnt )
				{
					rTrgPix.a() = rSrcPix.a();
				}
				else
				{
					rTrgPix.a() = xRange.tMax;
				}
			}
		}
	}
	else if ( pSrc[0].uColorCnt == 1 )
	{
		for(iIdx = 0; iIdx < iCnt; ++iIdx)
		{
			TPxL& rTrgPix = pTrg[iIdx];
			TPxR& rSrcPix = pSrc[iIdx];

			Pixel2Float( dPix, rSrcPix.r() );
			Float2Pixel( rTrgPix.r(), dPix + rPar.dValue );

			if ( rTrgPix.uAlphaCnt )
			{
				if ( rSrcPix.uAlphaCnt )
				{
					rTrgPix.a() = rSrcPix.a();
				}
				else
				{
					rTrgPix.a() = xRange.tMax;
				}
			}
		}
	}

	return true;
}

/// Negate image and add scalar value
template < class TPxL, class TPxR >
bool COGLImage::FOpNegAddScalar<TPxL, TPxR>::Execute( TPxL *pTrg, TPxL *pSrc, const POpScalar& rPar )
{
	TValueRange<TPxL::TType> xRange;
	int iIdx, iCnt = rPar.iWidth * rPar.iHeight;
	double dPix;

	if ( pSrc[0].uColorCnt == 3 )
	{
		for(iIdx = 0; iIdx < iCnt; ++iIdx)
		{
			TPxL& rTrgPix = pTrg[iIdx];
			TPxR& rSrcPix = pSrc[iIdx];

			Pixel2Float( dPix, rSrcPix.r() );
			Float2Pixel( rTrgPix.r(), rPar.dValue - dPix );

			Pixel2Float( dPix, rSrcPix.g() );
			Float2Pixel( rTrgPix.g(), rPar.dValue - dPix );

			Pixel2Float( dPix, rSrcPix.b() );
			Float2Pixel( rTrgPix.b(), rPar.dValue - dPix );

			if ( rTrgPix.uAlphaCnt )
			{
				if ( rSrcPix.uAlphaCnt )
				{
					rTrgPix.a() = rSrcPix.a();
				}
				else
				{
					rTrgPix.a() = xRange.tMax;
				}
			}
		}
	}
	else if ( pSrc[0].uColorCnt == 1 )
	{
		for(iIdx = 0; iIdx < iCnt; ++iIdx)
		{
			TPxL& rTrgPix = pTrg[iIdx];
			TPxR& rSrcPix = pSrc[iIdx];

			Pixel2Float( dPix, rSrcPix.r() );
			Float2Pixel( rTrgPix.r(), rPar.dValue - dPix );

			if ( rTrgPix.uAlphaCnt )
			{
				if ( rSrcPix.uAlphaCnt )
				{
					rTrgPix.a() = rSrcPix.a();
				}
				else
				{
					rTrgPix.a() = xRange.tMax;
				}
			}
		}
	}

	return true;
}



/// Multiply Image with color value
template < class TPxL, class TPxR >
bool COGLImage::FOpMultColor<TPxL, TPxR>::Execute( TPxL *pTrg, TPxL *pSrc, const POpColor& rPar )
{
	TValueRange<TPxL::TType> xRange;
	int iIdx, iCnt = rPar.iWidth * rPar.iHeight;
	double dPix;

	if ( pSrc[0].uColorCnt == 3 )
	{
		for(iIdx = 0; iIdx < iCnt; ++iIdx)
		{
			TPxL& rTrgPix = pTrg[iIdx];
			TPxR& rSrcPix = pSrc[iIdx];

			Pixel2Float( dPix, rSrcPix.r() );
			Float2Pixel( rTrgPix.r(), dPix * rPar.colValue[0] );

			Pixel2Float( dPix, rSrcPix.g() );
			Float2Pixel( rTrgPix.g(), dPix * rPar.colValue[1] );

			Pixel2Float( dPix, rSrcPix.b() );
			Float2Pixel( rTrgPix.b(), dPix * rPar.colValue[2] );

			if ( rTrgPix.uAlphaCnt )
			{
				if ( rSrcPix.uAlphaCnt )
				{
					Pixel2Float( dPix, rSrcPix.a() );
					Float2Pixel( rTrgPix.a(), dPix * rPar.colValue[3] );
				}
				else
				{
					Float2Pixel( rTrgPix.a(), rPar.colValue[3] );
				}
			}
		}
	}
	else if ( pSrc[0].uColorCnt == 1 )
	{
		for(iIdx = 0; iIdx < iCnt; ++iIdx)
		{
			TPxL& rTrgPix = pTrg[iIdx];
			TPxR& rSrcPix = pSrc[iIdx];

			Pixel2Float( dPix, rSrcPix.r() );
			Float2Pixel( rTrgPix.r(), dPix * rPar.colValue[0] );

			if ( rTrgPix.uAlphaCnt )
			{
				if ( rSrcPix.uAlphaCnt )
				{
					Pixel2Float( dPix, rSrcPix.a() );
					Float2Pixel( rTrgPix.a(), dPix * rPar.colValue[3] );
				}
				else
				{
					Float2Pixel( rTrgPix.a(), rPar.colValue[3] );
				}
			}
		}
	}

	return true;
}


/// Take reciprocal of each pixel and multiply with color
template < class TPxL, class TPxR >
bool COGLImage::FOpRecipColor<TPxL, TPxR>::Execute( TPxL *pTrg, TPxL *pSrc, const POpColor& rPar )
{
	TValueRange<TPxL::TType> xRange;
	int iIdx, iCnt = rPar.iWidth * rPar.iHeight;
	double dPix;

	if ( pSrc[0].uColorCnt == 3 )
	{
		for(iIdx = 0; iIdx < iCnt; ++iIdx)
		{
			TPxL& rTrgPix = pTrg[iIdx];
			TPxR& rSrcPix = pSrc[iIdx];

			Pixel2Float( dPix, rSrcPix.r() );
			if ( dPix >= 0 && dPix <= DBL_EPSILON )
				Float2Pixel( rTrgPix.r(), CLU_DBL_MAX );
			else if ( dPix >= -DBL_EPSILON && dPix <= 0 )
				Float2Pixel( rTrgPix.r(), CLU_DBL_MIN );
			else
				Float2Pixel( rTrgPix.r(), rPar.colValue[0] / dPix );

			Pixel2Float( dPix, rSrcPix.g() );
			if ( dPix >= 0 && dPix <= DBL_EPSILON )
				Float2Pixel( rTrgPix.g(), CLU_DBL_MAX );
			else if ( dPix >= -DBL_EPSILON && dPix <= 0 )
				Float2Pixel( rTrgPix.g(), CLU_DBL_MIN );
			else
				Float2Pixel( rTrgPix.g(), rPar.colValue[1] / dPix );

			Pixel2Float( dPix, rSrcPix.b() );
			if ( dPix >= 0 && dPix <= DBL_EPSILON )
				Float2Pixel( rTrgPix.b(), CLU_DBL_MAX );
			else if ( dPix >= -DBL_EPSILON && dPix <= 0 )
				Float2Pixel( rTrgPix.b(), CLU_DBL_MIN );
			else
				Float2Pixel( rTrgPix.b(), rPar.colValue[2] / dPix );

			if ( rTrgPix.uAlphaCnt )
			{
				if ( rSrcPix.uAlphaCnt )
				{
					Pixel2Float( dPix, rSrcPix.a() );
					Float2Pixel( rTrgPix.a(), rPar.colValue[3] / dPix );
				}
				else
				{
					Float2Pixel( rTrgPix.a(), rPar.colValue[3] );
				}
			}
		}
	}
	else if ( pSrc[0].uColorCnt == 1 )
	{
		for(iIdx = 0; iIdx < iCnt; ++iIdx)
		{
			TPxL& rTrgPix = pTrg[iIdx];
			TPxR& rSrcPix = pSrc[iIdx];

			Pixel2Float( dPix, rSrcPix.r() );
			if ( dPix >= 0 && dPix <= DBL_EPSILON )
				Float2Pixel( rTrgPix.r(), CLU_DBL_MAX );
			else if ( dPix >= -DBL_EPSILON && dPix <= 0 )
				Float2Pixel( rTrgPix.r(), CLU_DBL_MIN );
			else
				Float2Pixel( rTrgPix.r(), rPar.colValue[0] / dPix );

			if ( rTrgPix.uAlphaCnt )
			{
				if ( rSrcPix.uAlphaCnt )
				{
					Pixel2Float( dPix, rSrcPix.a() );
					Float2Pixel( rTrgPix.a(), rPar.colValue[3] / dPix );
				}
				else
				{
					Float2Pixel( rTrgPix.a(), rPar.colValue[3] );
				}
			}
		}
	}

	return true;
}

/// Add color value to Image
template < class TPxL, class TPxR >
bool COGLImage::FOpAddColor<TPxL, TPxR>::Execute( TPxL *pTrg, TPxL *pSrc, const POpColor& rPar )
{
	TValueRange<TPxL::TType> xRange;
	int iIdx, iCnt = rPar.iWidth * rPar.iHeight;
	double dPix;

	if ( pSrc[0].uColorCnt == 3 )
	{
		for(iIdx = 0; iIdx < iCnt; ++iIdx)
		{
			TPxL& rTrgPix = pTrg[iIdx];
			TPxR& rSrcPix = pSrc[iIdx];

			Pixel2Float( dPix, rSrcPix.r() );
			Float2Pixel( rTrgPix.r(), dPix + rPar.colValue[0] );

			Pixel2Float( dPix, rSrcPix.g() );
			Float2Pixel( rTrgPix.g(), dPix + rPar.colValue[1] );

			Pixel2Float( dPix, rSrcPix.b() );
			Float2Pixel( rTrgPix.b(), dPix + rPar.colValue[2] );

			if ( rTrgPix.uAlphaCnt )
			{
				if ( rSrcPix.uAlphaCnt )
				{
					Pixel2Float( dPix, rSrcPix.a() );
					Float2Pixel( rTrgPix.a(), rPar.colValue[3] + dPix );
				}
				else
				{
					Float2Pixel( rTrgPix.a(), rPar.colValue[3] );
				}
			}
		}
	}
	else if ( pSrc[0].uColorCnt == 1 )
	{
		for(iIdx = 0; iIdx < iCnt; ++iIdx)
		{
			TPxL& rTrgPix = pTrg[iIdx];
			TPxR& rSrcPix = pSrc[iIdx];

			Pixel2Float( dPix, rSrcPix.r() );
			Float2Pixel( rTrgPix.r(), dPix + rPar.colValue[0] );

			if ( rTrgPix.uAlphaCnt )
			{
				if ( rSrcPix.uAlphaCnt )
				{
					Pixel2Float( dPix, rSrcPix.a() );
					Float2Pixel( rTrgPix.a(), rPar.colValue[3] + dPix );
				}
				else
				{
					Float2Pixel( rTrgPix.a(), rPar.colValue[3] );
				}
			}
		}
	}

	return true;
}

/// Negate image and add scalar value
template < class TPxL, class TPxR >
bool COGLImage::FOpNegAddColor<TPxL, TPxR>::Execute( TPxL *pTrg, TPxL *pSrc, const POpColor& rPar )
{
	TValueRange<TPxL::TType> xRange;
	int iIdx, iCnt = rPar.iWidth * rPar.iHeight;
	double dPix;

	if ( pSrc[0].uColorCnt == 3 )
	{
		for(iIdx = 0; iIdx < iCnt; ++iIdx)
		{
			TPxL& rTrgPix = pTrg[iIdx];
			TPxR& rSrcPix = pSrc[iIdx];

			Pixel2Float( dPix, rSrcPix.r() );
			Float2Pixel( rTrgPix.r(), rPar.colValue[0] - dPix );

			Pixel2Float( dPix, rSrcPix.g() );
			Float2Pixel( rTrgPix.g(), rPar.colValue[1] - dPix );

			Pixel2Float( dPix, rSrcPix.b() );
			Float2Pixel( rTrgPix.b(), rPar.colValue[2] - dPix );

			if ( rTrgPix.uAlphaCnt )
			{
				if ( rSrcPix.uAlphaCnt )
				{
					Pixel2Float( dPix, rSrcPix.a() );
					Float2Pixel( rTrgPix.a(), rPar.colValue[3] - dPix );
				}
				else
				{
					Float2Pixel( rTrgPix.a(), rPar.colValue[3] );
				}
			}
		}
	}
	else if ( pSrc[0].uColorCnt == 1 )
	{
		for(iIdx = 0; iIdx < iCnt; ++iIdx)
		{
			TPxL& rTrgPix = pTrg[iIdx];
			TPxR& rSrcPix = pSrc[iIdx];

			Pixel2Float( dPix, rSrcPix.r() );
			Float2Pixel( rTrgPix.r(), rPar.colValue[0] - dPix );

			if ( rTrgPix.uAlphaCnt )
			{
				if ( rSrcPix.uAlphaCnt )
				{
					Pixel2Float( dPix, rSrcPix.a() );
					Float2Pixel( rTrgPix.a(), rPar.colValue[3] - dPix );
				}
				else
				{
					Float2Pixel( rTrgPix.a(), rPar.colValue[3] );
				}
			}
		}
	}

	return true;
}






/// Negate image 
template < class TPxL, class TPxR >
bool COGLImage::FOpNeg<TPxL, TPxR>::Execute( TPxL *pTrg, TPxR *pSrc, const PPixCnt& rPar )
{
	TValueRange<TPxL::TType> xRange;
	int iIdx, iCnt = rPar.iPixelCount;

	if ( pSrc[0].uColorCnt == 3 )
	{
		for(iIdx = 0; iIdx < iCnt; ++iIdx)
		{
			TPxL& rTrgPix = pTrg[iIdx];
			TPxR& rSrcPix = pSrc[iIdx];

			rTrgPix.r() = PixelNeg( rSrcPix.r() );
			rTrgPix.g() = PixelNeg( rSrcPix.g() );
			rTrgPix.b() = PixelNeg( rSrcPix.b() );

			if ( rTrgPix.uAlphaCnt )
			{
				if ( rSrcPix.uAlphaCnt )
				{
					rTrgPix.a() = rSrcPix.a();
				}
				else
				{
					rTrgPix.a() = xRange.tMax;
				}
			}
		}
	}
	else if ( pSrc[0].uColorCnt == 1 )
	{
		for(iIdx = 0; iIdx < iCnt; ++iIdx)
		{
			TPxL& rTrgPix = pTrg[iIdx];
			TPxR& rSrcPix = pSrc[iIdx];

			rTrgPix.r() = PixelNeg( rSrcPix.r() );

			if ( rTrgPix.uAlphaCnt )
			{
				if ( rSrcPix.uAlphaCnt )
				{
					rTrgPix.a() = rSrcPix.a();
				}
				else
				{
					rTrgPix.a() = xRange.tMax;
				}
			}
		}
	}

	return true;
}
