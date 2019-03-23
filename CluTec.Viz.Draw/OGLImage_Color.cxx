////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLImage_Color.cxx
//
// summary:   ogl image color class
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

template< class TPxL >
bool COGLImage::FFlushRGB<TPxL>::Execute(TPxL* pTrg, const PFlushRGB& rPar)
{
	int iPixel;

	if (TPxL::uColorCnt == 3)
	{
		TPxL pxVal;
		Float2Pixel(pxVal.r(), rPar.pfCol[0]);
		Float2Pixel(pxVal.g(), rPar.pfCol[1]);
		Float2Pixel(pxVal.b(), rPar.pfCol[2]);

		for (iPixel = 0; iPixel < rPar.iPixelCount; iPixel++)
		{
			pTrg[iPixel].r() = pxVal.r();
			pTrg[iPixel].g() = pxVal.g();
			pTrg[iPixel].b() = pxVal.b();
		}
	}
	else if (TPxL::uColorCnt == 1)
	{
		TPxL::TType xVal;
		Float2Pixel(xVal, (rPar.pfCol[0] + rPar.pfCol[1] + rPar.pfCol[2]) / 3.0f);

		for (iPixel = 0; iPixel < rPar.iPixelCount; iPixel++)
		{
			pTrg[iPixel].r() = xVal;
		}
	}

	return true;
}

/// Make Transparent
template< class TPxL >
bool COGLImage::FMakeTrans<TPxL>::Execute(TPxL* pTrg, const PMakeTrans& rPar)
{
	int iPixel;

	if (TPxL::uAlphaCnt == 0)
	{
		return true;
	}

	if (TPxL::uColorCnt == 3)
	{
		TPxL pxVal, pxWhite;
		float fLumTrans, fLumPx, fVal;

		fLumTrans = (rPar.pfCol[0] + rPar.pfCol[1] + rPar.pfCol[2]) / 3.0f;

		Float2Pixel(pxVal.r(), rPar.pfCol[0]);
		Float2Pixel(pxVal.g(), rPar.pfCol[1]);
		Float2Pixel(pxVal.b(), rPar.pfCol[2]);
		Float2Pixel(pxVal.a(), 0.0f);

		Float2Pixel(pxWhite.r(), 1.0f);
		Float2Pixel(pxWhite.g(), 1.0f);
		Float2Pixel(pxWhite.b(), 1.0f);
		Float2Pixel(pxWhite.a(), 1.0f);

		if (rPar.eMode == COGLImage::EXACT)
		{
			for (iPixel = 0; iPixel < rPar.iPixelCount; iPixel++)
			{
				if ((pTrg[iPixel].r() == pxVal.r()) &&
				    (pTrg[iPixel].g() == pxVal.g()) &&
				    (pTrg[iPixel].b() == pxVal.b()))
				{
					pTrg[iPixel].a() = pxVal.a();
				}
			}
		}
		else if (rPar.eMode == COGLImage::LUMINANCE)
		{
			for (iPixel = 0; iPixel < rPar.iPixelCount; iPixel++)
			{
				fLumPx = 0.0f;
				Pixel2Float(fVal, pTrg[iPixel].r());
				fLumPx += fVal;
				Pixel2Float(fVal, pTrg[iPixel].g());
				fLumPx += fVal;
				Pixel2Float(fVal, pTrg[iPixel].b());
				fLumPx += fVal;
				fLumPx /= 3.0f;

				if (fLumPx > fLumTrans)
				{
					Float2Pixel(pTrg[iPixel].a(), fLumPx - fLumTrans);
				}
				else
				{
					Float2Pixel(pTrg[iPixel].a(), fLumTrans - fLumPx);
				}
			}
		}
		else if (rPar.eMode == COGLImage::FONT)
		{
			for (iPixel = 0; iPixel < rPar.iPixelCount; iPixel++)
			{
				fLumPx = 0.0f;
				Pixel2Float(fVal, pTrg[iPixel].r());
				fLumPx += fVal;
				Pixel2Float(fVal, pTrg[iPixel].g());
				fLumPx += fVal;
				Pixel2Float(fVal, pTrg[iPixel].b());
				fLumPx += fVal;
				fLumPx /= 3.0f;

				pTrg[iPixel] = pxWhite;

				if (fLumPx > fLumTrans)
				{
					Float2Pixel(pTrg[iPixel].a(), fLumPx - fLumTrans);
				}
				else
				{
					Float2Pixel(pTrg[iPixel].a(), fLumTrans - fLumPx);
				}
			}
		}
	}
	else if (TPxL::uColorCnt == 1)
	{
		TPxL pxWhite;
		TPxL::TType xVal, xZero;
		float fLumPx, fLumTrans;

		fLumTrans = (rPar.pfCol[0] + rPar.pfCol[1] + rPar.pfCol[2]) / 3.0f;
		Float2Pixel(xVal, fLumTrans);
		Float2Pixel(xZero, 0.0f);

		Float2Pixel(pxWhite.r(), 1.0f);
		Float2Pixel(pxWhite.a(), 1.0f);

		if (rPar.eMode == COGLImage::EXACT)
		{
			for (iPixel = 0; iPixel < rPar.iPixelCount; iPixel++)
			{
				if (pTrg[iPixel].r() == xVal)
				{
					pTrg[iPixel].a() = xZero;
				}
			}
		}
		else if (rPar.eMode == COGLImage::LUMINANCE)
		{
			for (iPixel = 0; iPixel < rPar.iPixelCount; iPixel++)
			{
				Pixel2Float(fLumPx, pTrg[iPixel].r());

				if (fLumPx > fLumTrans)
				{
					Float2Pixel(pTrg[iPixel].a(), fLumPx - fLumTrans);
				}
				else
				{
					Float2Pixel(pTrg[iPixel].a(), fLumTrans - fLumPx);
				}
			}
		}
		else if (rPar.eMode == COGLImage::FONT)
		{
			for (iPixel = 0; iPixel < rPar.iPixelCount; iPixel++)
			{
				Pixel2Float(fLumPx, pTrg[iPixel].r());
				pTrg[iPixel] = pxWhite;

				if (fLumPx > fLumTrans)
				{
					Float2Pixel(pTrg[iPixel].a(), fLumPx - fLumTrans);
				}
				else
				{
					Float2Pixel(pTrg[iPixel].a(), fLumTrans - fLumPx);
				}
			}
		}
	}

	return true;
}

/// Get Pixel Color
template< class TPxL >
bool COGLImage::FGetPixelColor<TPxL>::Execute(TPxL* pTrg, PGetPixelColor& rPar)
{
	int iIdx;

	if (rPar.iX < 0)
	{
		rPar.iX = 0;
	}
	else if (rPar.iX >= rPar.iWidth)
	{
		rPar.iX = rPar.iWidth - 1;
	}

	if (rPar.iY < 0)
	{
		rPar.iY = 0;
	}
	else if (rPar.iY >= rPar.iHeight)
	{
		rPar.iY = rPar.iHeight - 1;
	}

	iIdx = (rPar.iHeight - rPar.iY - 1) * rPar.iWidth + rPar.iX;

	TPxL& xVal = pTrg[iIdx];

	Pixel2Float(rPar.dR, xVal.r());
	Pixel2Float(rPar.dG, xVal.g());
	Pixel2Float(rPar.dB, xVal.b());

	if (xVal.uAlphaCnt)
	{
		Pixel2Float(rPar.dA, xVal.a());
	}
	else
	{
		rPar.dA = 1.0;
	}

	return true;
}

/// Get Pixel Normal
template< class TPxL >
bool COGLImage::FGetPixelNormalLum<TPxL>::Execute(TPxL* pTrg, PGetPixelNormalLum& rPar)
{
	int iIdxC, iIdxX, iIdxY;
	double dIC, dIX, dIY;
	double dDIX, dDIY;

	if (rPar.iX >= rPar.iWidth - 1)
	{
		rPar.iX--;
	}

	if (rPar.iY >= rPar.iHeight - 1)
	{
		rPar.iY--;
	}

	iIdxC = (rPar.iHeight - rPar.iY - 1) * rPar.iWidth + rPar.iX;
	iIdxX = iIdxC + 1;
	iIdxY = iIdxC - rPar.iWidth;

	TPxL& xVC = pTrg[iIdxC];
	TPxL& xVX = pTrg[iIdxX];
	TPxL& xVY = pTrg[iIdxY];

	Pixel2Float(dIC, max(max(xVC.r(), xVC.g()), xVC.b()));
	Pixel2Float(dIX, max(max(xVX.r(), xVX.g()), xVX.b()));
	Pixel2Float(dIY, max(max(xVY.r(), xVY.g()), xVY.b()));

	dDIX = dIX - dIC;
	dDIY = dIY - dIC;

	rPar.dX = -dDIX;
	rPar.dY = -dDIY;
	rPar.dZ = 1.0;

	//dNorm = sqrt( rPar.dX*rPar.dX + rPar.dY*rPar.dY + rPar.dZ*rPar.dZ );

	//rPar.dX /= dNorm;
	//rPar.dY /= dNorm;
	//rPar.dZ /= dNorm;

	return true;
}

/// Get Pixel with maximal luminance
template< class TPxL >
bool COGLImage::FGetPixelMaxLum<TPxL>::Execute(TPxL* pTrg, PGetPixelMinMaxLum& rPar)
{
	int iX, iY;	//, iCol;
	unsigned uIdx, uCnt, uLumIdxMax = 0, uAIdxMax = 0, puIdxMax[3] = { 0, 0, 0 };
	double dLumMax     = 0.0, dAMax = 0.0;	//dLum, dA
	double pdColMax[3] = { 0.0, 0.0, 0.0 };
	//double pdCol[3];

	if ((rPar.dPart <= 0.0) || (rPar.iWidth <= 0) || (rPar.iHeight <= 0))
	{
		for (int i = 0; i < 4; ++i)
		{
			rPar.piX[i]   = 0;
			rPar.piY[i]   = 0;
			rPar.pdCol[i] = 0;
		}

		return false;
	}

	uCnt = unsigned(rPar.iWidth) * unsigned(rPar.iHeight);

	if (rPar.dPart > 1.0)
	{
		// Standard maximum of all pixels
		rPar.dPart = 1.0;
	}
	else if (rPar.dPart < 0.0)
	{
		rPar.dPart = 0.0;
	}

	unsigned uPartIdx, uPartCnt = unsigned(floor(double(uCnt) * rPar.dPart));

	if (uPartCnt > uCnt)
	{
		uPartCnt = uCnt;
	}

	if (uPartCnt == 0)
	{
		uPartIdx = 0;
	}
	else
	{
		uPartIdx = uPartCnt - 1;
	}

	if (TPxL::uColorCnt == 1)
	{
		vector<unsigned>& vecLumIdx = *rPar.pvecIdx[0];
		vector<unsigned>& vecAIdx   = *rPar.pvecIdx[3];

		SPixelChanSmaller<TPxL, TPxL::iRedIdx> predSmallerRed(pTrg);
		SPixelChanSmaller<TPxL, TPxL::iAlphaIdx> predSmallerAlpha(pTrg);

		if (vecLumIdx.size() != uCnt)
		{
			vecLumIdx.resize(uCnt);
			for (uIdx = 0; uIdx < uCnt; ++uIdx)
			{
				vecLumIdx[uIdx] = uIdx;
			}

			// Sort pixel indices in ascending order of red channel luminances
			std::sort(vecLumIdx.begin(), vecLumIdx.end(), predSmallerRed);
		}

		uLumIdxMax = vecLumIdx[uPartIdx];
		Pixel2Float(dLumMax, pTrg[uLumIdxMax].r());

		if (TPxL::uAlphaCnt == 1)
		{
			if (vecAIdx.size() != uCnt)
			{
				vecAIdx.resize(uCnt);
				for (uIdx = 0; uIdx < uCnt; ++uIdx)
				{
					vecAIdx[uIdx] = uIdx;
				}

				// Sort pixel indices in ascending order of alpha channel luminances
				std::sort(vecAIdx.begin(), vecAIdx.end(), predSmallerAlpha);
			}

			uAIdxMax = vecAIdx[uPartIdx];
			Pixel2Float(dAMax, pTrg[uAIdxMax].a());
		}

		iY = int(unsigned(rPar.iHeight) - (uLumIdxMax / unsigned(rPar.iWidth)) - 1);
		iX = int(uLumIdxMax % unsigned(rPar.iWidth));

		for (int i = 0; i < 3; ++i)
		{
			rPar.piX[i]   = iX;
			rPar.piY[i]   = iY;
			rPar.pdCol[i] = dLumMax;
		}

		if (TPxL::uAlphaCnt == 1)
		{
			iY = int(unsigned(rPar.iHeight) - (uAIdxMax / unsigned(rPar.iWidth)) - 1);
			iX = int(uAIdxMax % unsigned(rPar.iWidth));

			rPar.piX[3]   = iX;
			rPar.piY[3]   = iY;
			rPar.pdCol[3] = dAMax;
		}
		else
		{
			rPar.piX[3]   = 0;
			rPar.piY[3]   = 0;
			rPar.pdCol[3] = 0.0;
		}
	}
	else if (TPxL::uColorCnt == 3)
	{
		vector<unsigned>& vecRIdx = *rPar.pvecIdx[0];
		vector<unsigned>& vecGIdx = *rPar.pvecIdx[1];
		vector<unsigned>& vecBIdx = *rPar.pvecIdx[2];
		vector<unsigned>& vecAIdx = *rPar.pvecIdx[3];

		SPixelChanSmaller<TPxL, TPxL::iRedIdx> predSmallerRed(pTrg);
		SPixelChanSmaller<TPxL, TPxL::iGreenIdx> predSmallerGreen(pTrg);
		SPixelChanSmaller<TPxL, TPxL::iBlueIdx> predSmallerBlue(pTrg);
		SPixelChanSmaller<TPxL, TPxL::iAlphaIdx> predSmallerAlpha(pTrg);

		if (vecRIdx.size() != uCnt)
		{
			vecRIdx.resize(uCnt);
			for (uIdx = 0; uIdx < uCnt; ++uIdx)
			{
				vecRIdx[uIdx] = uIdx;
			}

			// Sort pixel indices in ascending order of red channel luminances
			std::sort(vecRIdx.begin(), vecRIdx.end(), predSmallerRed);
		}

		if (vecGIdx.size() != uCnt)
		{
			vecGIdx.resize(uCnt);
			for (uIdx = 0; uIdx < uCnt; ++uIdx)
			{
				vecGIdx[uIdx] = uIdx;
			}

			// Sort pixel indices in ascending order of red channel luminances
			std::sort(vecGIdx.begin(), vecGIdx.end(), predSmallerGreen);
		}

		if (vecBIdx.size() != uCnt)
		{
			vecBIdx.resize(uCnt);
			for (uIdx = 0; uIdx < uCnt; ++uIdx)
			{
				vecBIdx[uIdx] = uIdx;
			}

			// Sort pixel indices in ascending order of red channel luminances
			std::sort(vecBIdx.begin(), vecBIdx.end(), predSmallerBlue);
		}

		puIdxMax[0] = vecRIdx[uPartIdx];
		Pixel2Float(pdColMax[0], pTrg[puIdxMax[0]].r());

		puIdxMax[1] = vecGIdx[uPartIdx];
		Pixel2Float(pdColMax[1], pTrg[puIdxMax[1]].g());

		puIdxMax[2] = vecBIdx[uPartIdx];
		Pixel2Float(pdColMax[2], pTrg[puIdxMax[2]].b());

		if (TPxL::uAlphaCnt == 1)
		{
			if (vecAIdx.size() != uCnt)
			{
				vecAIdx.resize(uCnt);
				for (uIdx = 0; uIdx < uCnt; ++uIdx)
				{
					vecAIdx[uIdx] = uIdx;
				}

				// Sort pixel indices in ascending order of red channel luminances
				std::sort(vecAIdx.begin(), vecAIdx.end(), predSmallerAlpha);
			}

			uAIdxMax = vecAIdx[uPartIdx];
			Pixel2Float(dAMax, pTrg[uAIdxMax].a());
		}

		for (int i = 0; i < 3; ++i)
		{
			rPar.piX[i]   = int(puIdxMax[i] % unsigned(rPar.iWidth));
			rPar.piY[i]   = int(unsigned(rPar.iHeight) - (puIdxMax[i] / unsigned(rPar.iWidth)) - 1);
			rPar.pdCol[i] = pdColMax[i];
		}

		if (TPxL::uAlphaCnt == 1)
		{
			iY = int(unsigned(rPar.iHeight) - (uAIdxMax / unsigned(rPar.iWidth)) - 1);
			iX = int(uAIdxMax % unsigned(rPar.iWidth));

			rPar.piX[3]   = iX;
			rPar.piY[3]   = iY;
			rPar.pdCol[3] = dAMax;
		}
		else
		{
			rPar.piX[3]   = 0;
			rPar.piY[3]   = 0;
			rPar.pdCol[3] = 0.0;
		}
	}

	return true;
}

/// Get Pixel with minimal luminance
template< class TPxL >
bool COGLImage::FGetPixelMinLum<TPxL>::Execute(TPxL* pTrg, PGetPixelMinMaxLum& rPar)
{
	int iX, iY;	//, iCol;
	unsigned uIdx, uCnt, uLumIdxMax = 0, uAIdxMax = 0, puIdxMax[3] = { 0, 0, 0 };
	double dLumMax     = 0.0, dAMax = 0.0;	//dLum, dA
	double pdColMax[3] = { 0.0, 0.0, 0.0 };
	//double pdCol[3];

	if ((rPar.dPart <= 0.0) || (rPar.iWidth <= 0) || (rPar.iHeight <= 0))
	{
		for (int i = 0; i < 4; ++i)
		{
			rPar.piX[i]   = 0;
			rPar.piY[i]   = 0;
			rPar.pdCol[i] = 0;
		}

		return false;
	}

	uCnt = unsigned(rPar.iWidth) * unsigned(rPar.iHeight);

	if (rPar.dPart > 1.0)
	{
		// Standard maximum of all pixels
		rPar.dPart = 1.0;
	}
	else if (rPar.dPart < 0.0)
	{
		rPar.dPart = 0.0;
	}

	unsigned uPartIdx, uPartCnt = unsigned(floor(double(uCnt) * rPar.dPart));

	if (uPartCnt >= uCnt)
	{
		uPartIdx = 0;
	}
	else
	{
		uPartIdx = uCnt - uPartCnt;
	}

	if (TPxL::uColorCnt == 1)
	{
		vector<unsigned>& vecLumIdx = *rPar.pvecIdx[0];
		vector<unsigned>& vecAIdx   = *rPar.pvecIdx[3];

		SPixelChanSmaller<TPxL, TPxL::iRedIdx> predSmallerRed(pTrg);
		SPixelChanSmaller<TPxL, TPxL::iAlphaIdx> predSmallerAlpha(pTrg);

		if (vecLumIdx.size() != uCnt)
		{
			vecLumIdx.resize(uCnt);
			for (uIdx = 0; uIdx < uCnt; ++uIdx)
			{
				vecLumIdx[uIdx] = uIdx;
			}

			// Sort pixel indices in ascending order of red channel luminances
			std::sort(vecLumIdx.begin(), vecLumIdx.end(), predSmallerRed);
		}

		uLumIdxMax = vecLumIdx[uPartIdx];
		Pixel2Float(dLumMax, pTrg[uLumIdxMax].r());

		if (TPxL::uAlphaCnt == 1)
		{
			if (vecAIdx.size() != uCnt)
			{
				vecAIdx.resize(uCnt);
				for (uIdx = 0; uIdx < uCnt; ++uIdx)
				{
					vecAIdx[uIdx] = uIdx;
				}

				// Sort pixel indices in ascending order of alpha channel luminances
				std::sort(vecAIdx.begin(), vecAIdx.end(), predSmallerAlpha);
			}

			uAIdxMax = vecAIdx[uPartIdx];
			Pixel2Float(dAMax, pTrg[uAIdxMax].a());
		}

		iY = int(unsigned(rPar.iHeight) - (uLumIdxMax / unsigned(rPar.iWidth)) - 1);
		iX = int(uLumIdxMax % unsigned(rPar.iWidth));

		for (int i = 0; i < 3; ++i)
		{
			rPar.piX[i]   = iX;
			rPar.piY[i]   = iY;
			rPar.pdCol[i] = dLumMax;
		}

		if (TPxL::uAlphaCnt == 1)
		{
			iY = int(unsigned(rPar.iHeight) - (uAIdxMax / unsigned(rPar.iWidth)) - 1);
			iX = int(uAIdxMax % unsigned(rPar.iWidth));

			rPar.piX[3]   = iX;
			rPar.piY[3]   = iY;
			rPar.pdCol[3] = dAMax;
		}
		else
		{
			rPar.piX[3]   = 0;
			rPar.piY[3]   = 0;
			rPar.pdCol[3] = 0.0;
		}
	}
	else if (TPxL::uColorCnt == 3)
	{
		vector<unsigned>& vecRIdx = *rPar.pvecIdx[0];
		vector<unsigned>& vecGIdx = *rPar.pvecIdx[1];
		vector<unsigned>& vecBIdx = *rPar.pvecIdx[2];
		vector<unsigned>& vecAIdx = *rPar.pvecIdx[3];

		SPixelChanSmaller<TPxL, TPxL::iRedIdx> predSmallerRed(pTrg);
		SPixelChanSmaller<TPxL, TPxL::iGreenIdx> predSmallerGreen(pTrg);
		SPixelChanSmaller<TPxL, TPxL::iBlueIdx> predSmallerBlue(pTrg);
		SPixelChanSmaller<TPxL, TPxL::iAlphaIdx> predSmallerAlpha(pTrg);

		if (vecRIdx.size() != uCnt)
		{
			vecRIdx.resize(uCnt);
			for (uIdx = 0; uIdx < uCnt; ++uIdx)
			{
				vecRIdx[uIdx] = uIdx;
			}

			// Sort pixel indices in ascending order of red channel luminances
			std::sort(vecRIdx.begin(), vecRIdx.end(), predSmallerRed);
		}

		if (vecGIdx.size() != uCnt)
		{
			vecGIdx.resize(uCnt);
			for (uIdx = 0; uIdx < uCnt; ++uIdx)
			{
				vecGIdx[uIdx] = uIdx;
			}

			// Sort pixel indices in ascending order of red channel luminances
			std::sort(vecGIdx.begin(), vecGIdx.end(), predSmallerGreen);
		}

		if (vecBIdx.size() != uCnt)
		{
			vecBIdx.resize(uCnt);
			for (uIdx = 0; uIdx < uCnt; ++uIdx)
			{
				vecBIdx[uIdx] = uIdx;
			}

			// Sort pixel indices in ascending order of red channel luminances
			std::sort(vecBIdx.begin(), vecBIdx.end(), predSmallerBlue);
		}

		puIdxMax[0] = vecRIdx[uPartIdx];
		Pixel2Float(pdColMax[0], pTrg[puIdxMax[0]].r());

		puIdxMax[1] = vecGIdx[uPartIdx];
		Pixel2Float(pdColMax[1], pTrg[puIdxMax[1]].g());

		puIdxMax[2] = vecBIdx[uPartIdx];
		Pixel2Float(pdColMax[2], pTrg[puIdxMax[2]].b());

		if (TPxL::uAlphaCnt == 1)
		{
			if (vecAIdx.size() != uCnt)
			{
				vecAIdx.resize(uCnt);
				for (uIdx = 0; uIdx < uCnt; ++uIdx)
				{
					vecAIdx[uIdx] = uIdx;
				}

				// Sort pixel indices in ascending order of red channel luminances
				std::sort(vecAIdx.begin(), vecAIdx.end(), predSmallerAlpha);
			}

			uAIdxMax = vecAIdx[uPartIdx];
			Pixel2Float(dAMax, pTrg[uAIdxMax].a());
		}

		for (int i = 0; i < 3; ++i)
		{
			rPar.piX[i]   = int(puIdxMax[i] % unsigned(rPar.iWidth));
			rPar.piY[i]   = int(unsigned(rPar.iHeight) - (puIdxMax[i] / unsigned(rPar.iWidth)) - 1);
			rPar.pdCol[i] = pdColMax[i];
		}

		if (TPxL::uAlphaCnt == 1)
		{
			iY = int(unsigned(rPar.iHeight) - (uAIdxMax / unsigned(rPar.iWidth)) - 1);
			iX = int(uAIdxMax % unsigned(rPar.iWidth));

			rPar.piX[3]   = iX;
			rPar.piY[3]   = iY;
			rPar.pdCol[3] = dAMax;
		}
		else
		{
			rPar.piX[3]   = 0;
			rPar.piY[3]   = 0;
			rPar.pdCol[3] = 0.0;
		}
	}

	return true;
}
