////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLImage_Mask.cxx
//
// summary:   ogl image mask class
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

/// Mask Equal Color
template < class TPxL, class TPxR >
bool COGLImage::FMaskEqualCol<TPxL,TPxR>::Execute( TPxL *pTrg, TPxR *pSrc, const PMaskEqualCol& rPar )
{
	int iPixel;

	if ( TPxL::uColorCnt == 3 )
	{
		TPxR pxVal;
		TPxL pxTrue, pxFalse;

		Float2Pixel(pxVal.r(), rPar.pfCol[0]);
		Float2Pixel(pxVal.g(), rPar.pfCol[1]);
		Float2Pixel(pxVal.b(), rPar.pfCol[2]);

		Float2Pixel(pxTrue.r(), 1.0f);
		Float2Pixel(pxTrue.g(), 1.0f);
		Float2Pixel(pxTrue.b(), 1.0f);

		if ( pTrg->uAlphaCnt == 1 )
			Float2Pixel(pxTrue.a(), 1.0f);

		Float2Pixel(pxFalse.r(), 0.0f);
		Float2Pixel(pxFalse.g(), 0.0f);
		Float2Pixel(pxFalse.b(), 0.0f);

		if ( pTrg->uAlphaCnt == 1 )
			Float2Pixel(pxFalse.a(), 1.0f);

		for (iPixel=0; iPixel<rPar.iPixelCount; iPixel++)
		{
			if ( pSrc[iPixel].r() == pxVal.r() &&
				 pSrc[iPixel].g() == pxVal.g() &&
				 pSrc[iPixel].b() == pxVal.b() )
			{
				pTrg[iPixel] = pxTrue;
			}
			else
			{
				pTrg[iPixel] = pxFalse;
			}
		}
	}
	else if ( TPxL::uColorCnt == 1 )
	{
		TPxR::TType xVal;
		TPxL pxTrue, pxFalse;

		Float2Pixel( xVal, (rPar.pfCol[0] + rPar.pfCol[1] + rPar.pfCol[2]) / 3.0f );

		Float2Pixel( pxTrue.r(), 1.0f );
		if ( pTrg->uAlphaCnt == 1 )
			Float2Pixel(pxTrue.a(), 1.0f);

		Float2Pixel(pxFalse.r(), 0.0f);
		if ( pTrg->uAlphaCnt == 1 )
			Float2Pixel(pxFalse.a(), 1.0f);

		for (iPixel=0; iPixel<rPar.iPixelCount; iPixel++)
		{
			if ( pSrc[iPixel].r() == xVal )
				pTrg[iPixel] = pxTrue;
			else
				pTrg[iPixel] = pxFalse;
		}
	}

	return true;
}

/// Mask Not Equal Color
template < class TPxL, class TPxR >
bool COGLImage::FMaskNotEqualCol<TPxL,TPxR>::Execute( TPxL *pTrg, TPxR *pSrc, const PMaskEqualCol& rPar )
{
	int iPixel;

	if ( TPxL::uColorCnt == 3 )
	{
		TPxR pxVal;
		TPxL pxTrue, pxFalse;

		Float2Pixel(pxVal.r(), rPar.pfCol[0]);
		Float2Pixel(pxVal.g(), rPar.pfCol[1]);
		Float2Pixel(pxVal.b(), rPar.pfCol[2]);

		Float2Pixel(pxTrue.r(), 1.0f);
		Float2Pixel(pxTrue.g(), 1.0f);
		Float2Pixel(pxTrue.b(), 1.0f);

		if ( pTrg->uAlphaCnt == 1 )
			Float2Pixel(pxTrue.a(), 1.0f);

		Float2Pixel(pxFalse.r(), 0.0f);
		Float2Pixel(pxFalse.g(), 0.0f);
		Float2Pixel(pxFalse.b(), 0.0f);

		if ( pTrg->uAlphaCnt == 1 )
			Float2Pixel(pxFalse.a(), 1.0f);

		for (iPixel=0; iPixel<rPar.iPixelCount; iPixel++)
		{
			if ( pSrc[iPixel].r() != pxVal.r() ||
				 pSrc[iPixel].g() != pxVal.g() ||
				 pSrc[iPixel].b() != pxVal.b() )
			{
				pTrg[iPixel] = pxTrue;
			}
			else
			{
				pTrg[iPixel] = pxFalse;
			}
		}
	}
	else if ( TPxL::uColorCnt == 1 )
	{
		TPxR::TType xVal;
		TPxL pxTrue, pxFalse;

		Float2Pixel( xVal, (rPar.pfCol[0] + rPar.pfCol[1] + rPar.pfCol[2]) / 3.0f );

		Float2Pixel( pxTrue.r(), 1.0f );
		if ( pTrg->uAlphaCnt == 1 )
			Float2Pixel(pxTrue.a(), 1.0f);

		Float2Pixel(pxFalse.r(), 0.0f);
		if ( pTrg->uAlphaCnt == 1 )
			Float2Pixel(pxFalse.a(), 1.0f);

		for (iPixel=0; iPixel<rPar.iPixelCount; iPixel++)
		{
			if ( pSrc[iPixel].r() != xVal )
				pTrg[iPixel] = pxTrue;
			else
				pTrg[iPixel] = pxFalse;
		}
	}

	return true;
}


/// Mask Equal Image
template < class TPxL, class TPxR1, class TPxR2 >
bool COGLImage::FMaskEqualImg<TPxL,TPxR1,TPxR2>::Execute( TPxL *pTrg, TPxR1 *pSrc1, TPxR2 *pSrc2, const PPixCnt& rPar )
{
	int iPix;
 
	if ( TPxL::uColorCnt == 3 )
	{
		TPxL pxTrue, pxFalse;

		Float2Pixel(pxTrue.r(), 1.0f);
		Float2Pixel(pxTrue.g(), 1.0f);
		Float2Pixel(pxTrue.b(), 1.0f);

		if ( pTrg->uAlphaCnt == 1 )
			Float2Pixel(pxTrue.a(), 1.0f);

		Float2Pixel(pxFalse.r(), 0.0f);
		Float2Pixel(pxFalse.g(), 0.0f);
		Float2Pixel(pxFalse.b(), 0.0f);

		if ( pTrg->uAlphaCnt == 1 )
			Float2Pixel(pxFalse.a(), 1.0f);

		for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
		{
			if (pSrc1[iPix].r() == pSrc2[iPix].r() &&
				pSrc1[iPix].g() == pSrc2[iPix].g() &&
				pSrc1[iPix].b() == pSrc2[iPix].b())
			{
				pTrg[iPix] = pxTrue;
			}
			else
			{
				pTrg[iPix] = pxFalse;
			}
		}
	}
	else if ( TPxL::uColorCnt == 1 )
	{
		TPxL pxTrue, pxFalse;

		Float2Pixel( pxTrue.r(), 1.0f );
		if ( pTrg->uAlphaCnt == 1 )
			Float2Pixel(pxTrue.a(), 1.0f);

		Float2Pixel(pxFalse.r(), 0.0f);
		if ( pTrg->uAlphaCnt == 1 )
			Float2Pixel(pxFalse.a(), 1.0f);

		for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
		{
			if (pSrc1[iPix].r() == pSrc2[iPix].r())
			{
				pTrg[iPix] = pxTrue;
			}
			else
			{
				pTrg[iPix] = pxFalse;
			}
		}
	}

	return true;
}

/// Mask Not Equal Image
template < class TPxL, class TPxR1, class TPxR2 >
bool COGLImage::FMaskNotEqualImg<TPxL,TPxR1,TPxR2>::Execute( TPxL *pTrg, TPxR1 *pSrc1, TPxR2 *pSrc2, const PPixCnt& rPar )
{
	int iPix;

	if ( TPxL::uColorCnt == 3 )
	{
		TPxL pxTrue, pxFalse;

		Float2Pixel(pxTrue.r(), 1.0f);
		Float2Pixel(pxTrue.g(), 1.0f);
		Float2Pixel(pxTrue.b(), 1.0f);

		if ( pTrg->uAlphaCnt == 1 )
			Float2Pixel(pxTrue.a(), 1.0f);

		Float2Pixel(pxFalse.r(), 0.0f);
		Float2Pixel(pxFalse.g(), 0.0f);
		Float2Pixel(pxFalse.b(), 0.0f);

		if ( pTrg->uAlphaCnt == 1 )
			Float2Pixel(pxFalse.a(), 1.0f);

		for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
		{
			if (pSrc1[iPix].r() != pSrc2[iPix].r() ||
				pSrc1[iPix].g() != pSrc2[iPix].g() ||
				pSrc1[iPix].b() != pSrc2[iPix].b())
			{
				pTrg[iPix] = pxTrue;
			}
			else
			{
				pTrg[iPix] = pxFalse;
			}
		}
	}
	else if ( TPxL::uColorCnt == 1 )
	{
		TPxL pxTrue, pxFalse;

		Float2Pixel( pxTrue.r(), 1.0f );
		if ( pTrg->uAlphaCnt == 1 )
			Float2Pixel(pxTrue.a(), 1.0f);

		Float2Pixel(pxFalse.r(), 0.0f);
		if ( pTrg->uAlphaCnt == 1 )
			Float2Pixel(pxFalse.a(), 1.0f);

		for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
		{
			if (pSrc1[iPix].r() != pSrc2[iPix].r())
			{
				pTrg[iPix] = pxTrue;
			}
			else
			{
				pTrg[iPix] = pxFalse;
			}
		}
	}

	return true;
}


/// Mask Logic AND Image
template < class TPxL, class TPxR1, class TPxR2 >
bool COGLImage::FMaskLogicANDImg<TPxL,TPxR1,TPxR2>::Execute( TPxL *pTrg, TPxR1 *pSrc1, TPxR2 *pSrc2, const PPixCnt& rPar )
{
	int iPix;
	TPxL::TType xTrue, xFalse;

	Float2Pixel(xTrue, 1.0f);
	Float2Pixel(xFalse, 0.0f);

	if ( TPxL::uColorCnt == 3 )
	{
		for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
		{
			pTrg[iPix].r() = (pSrc1[iPix].r() && pSrc2[iPix].r() ? xTrue : xFalse );
			pTrg[iPix].g() = (pSrc1[iPix].g() && pSrc2[iPix].g() ? xTrue : xFalse );
			pTrg[iPix].b() = (pSrc1[iPix].b() && pSrc2[iPix].b() ? xTrue : xFalse );

			if ( TPxL::uAlphaCnt )
				Float2Pixel( pTrg[iPix].a(), 1.0 );
		}
	}
	else if ( TPxL::uColorCnt == 1 )
	{
		for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
		{
			pTrg[iPix].r() = (pSrc1[iPix].r() && pSrc2[iPix].r() ? xTrue : xFalse );

			if ( TPxL::uAlphaCnt )
				Float2Pixel( pTrg[iPix].a(), 1.0 );
		}
	}

	return true;
}


/// Mask Logic OR Image
template < class TPxL, class TPxR1, class TPxR2 >
bool COGLImage::FMaskLogicORImg<TPxL,TPxR1,TPxR2>::Execute( TPxL *pTrg, TPxR1 *pSrc1, TPxR2 *pSrc2, const PPixCnt& rPar )
{
	int iPix;
	TPxL::TType xTrue, xFalse;

	Float2Pixel(xTrue, 1.0f);
	Float2Pixel(xFalse, 0.0f);

	if ( TPxL::uColorCnt == 3 )
	{
		for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
		{
			pTrg[iPix].r() = (pSrc1[iPix].r() || pSrc2[iPix].r() ? xTrue : xFalse );
			pTrg[iPix].g() = (pSrc1[iPix].g() || pSrc2[iPix].g() ? xTrue : xFalse );
			pTrg[iPix].b() = (pSrc1[iPix].b() || pSrc2[iPix].b() ? xTrue : xFalse );

			if ( TPxL::uAlphaCnt )
				Float2Pixel( pTrg[iPix].a(), 1.0 );
		}
	}
	else if ( TPxL::uColorCnt == 1 )
	{
		for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
		{
			pTrg[iPix].r() = (pSrc1[iPix].r() || pSrc2[iPix].r() ? xTrue : xFalse );

			if ( TPxL::uAlphaCnt )
				Float2Pixel( pTrg[iPix].a(), 1.0 );
		}
	}

	return true;
}

/// Bit AND Image
template < class TPxL, class TPxR1, class TPxR2 >
bool COGLImage::FMaskBitANDImg<TPxL,TPxR1,TPxR2>::Execute( TPxL *pTrg, TPxR1 *pSrc1, TPxR2 *pSrc2, const PPixCnt& rPar )
{
	int iPix;
	TPxL::TType xTrue, xFalse;

	Float2Pixel(xTrue, 1.0f);
	Float2Pixel(xFalse, 0.0f);

	if ( TPxL::uColorCnt == 3 )
	{
		for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
		{
			pTrg[iPix].r() = PixelBitAND(pSrc1[iPix].r(), pSrc2[iPix].r());
			pTrg[iPix].g() = PixelBitAND(pSrc1[iPix].g(), pSrc2[iPix].g());
			pTrg[iPix].b() = PixelBitAND(pSrc1[iPix].b(), pSrc2[iPix].b());

			if ( TPxL::uAlphaCnt )
				Float2Pixel( pTrg[iPix].a(), 1.0 );
		}
	}
	else if ( TPxL::uColorCnt == 1 )
	{
		for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
		{
			pTrg[iPix].r() = PixelBitAND(pSrc1[iPix].r(), pSrc2[iPix].r());

			if ( TPxL::uAlphaCnt )
				Float2Pixel( pTrg[iPix].a(), 1.0 );
		}
	}

	return true;
}

/// Bit OR Image
template < class TPxL, class TPxR1, class TPxR2 >
bool COGLImage::FMaskBitORImg<TPxL,TPxR1,TPxR2>::Execute( TPxL *pTrg, TPxR1 *pSrc1, TPxR2 *pSrc2, const PPixCnt& rPar )
{
	int iPix;
	TPxL::TType xTrue, xFalse;

	Float2Pixel(xTrue, 1.0f);
	Float2Pixel(xFalse, 0.0f);

	if ( TPxL::uColorCnt == 3 )
	{
		for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
		{
			pTrg[iPix].r() = PixelBitOR(pSrc1[iPix].r(), pSrc2[iPix].r());
			pTrg[iPix].g() = PixelBitOR(pSrc1[iPix].g(), pSrc2[iPix].g());
			pTrg[iPix].b() = PixelBitOR(pSrc1[iPix].b(), pSrc2[iPix].b());

			if ( TPxL::uAlphaCnt )
				Float2Pixel( pTrg[iPix].a(), 1.0 );
		}
	}
	else if ( TPxL::uColorCnt == 1 )
	{
		for (iPix = 0; iPix < rPar.iPixelCount; iPix++)
		{
			pTrg[iPix].r() = PixelBitOR(pSrc1[iPix].r(), pSrc2[iPix].r());

			if ( TPxL::uAlphaCnt )
				Float2Pixel( pTrg[iPix].a(), 1.0 );
		}
	}

	return true;
}
