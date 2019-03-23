////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLImage_Form.cxx
//
// summary:   ogl image Windows Form
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Extract sub image
template < class TPxL, class TPxR >
bool COGLImage::FGetSubImg<TPxL, TPxR>::Execute( TPxL *pTrg, TPxL *pSrc, const PGetSubImg& rPar )
{
	int iRow, iSrcPos, iTrgPos;

	iSrcPos = (rPar.iSrcHeight - rPar.iY - 1 - rPar.iCY + 1) * rPar.iSrcWidth + rPar.iX;
	iTrgPos = 0;
	for(iRow = 0; iRow < rPar.iCY; iRow++, iSrcPos += rPar.iSrcWidth, iTrgPos += rPar.iCX)
	{
		memcpy(&pTrg[iTrgPos], &pSrc[iSrcPos], sizeof(TPxL) * rPar.iCX);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
/// Insert sub image
template < class TPxL, class TPxR >
bool COGLImage::FInsSubImg<TPxL, TPxR>::Execute( TPxL *pTrg, TPxL *pSrc, const PInsSubImg& rPar )
{
	int iSrcPos, iTrgPos, iRowLen, iRow;

	iSrcPos = (rPar.iSrcHeight - rPar.iSrcY - 1 - rPar.iSrcH + 1) * rPar.iSrcWidth + rPar.iSrcX;
	iTrgPos = (rPar.iTrgHeight - rPar.iTrgY - 1 - rPar.iSrcH + 1) * rPar.iTrgWidth + rPar.iTrgX;
	iRowLen = sizeof(TPxL) * rPar.iSrcW;

	for(iRow = 0; iRow < rPar.iSrcH; iRow++, iSrcPos += rPar.iSrcWidth, iTrgPos += rPar.iTrgWidth)
	{
		memcpy( &pTrg[iTrgPos], &pSrc[iSrcPos], iRowLen );
		//for ( int i = 0; i < iSrcW; ++i )
		//{
		//	pTrg[iTrgPos + i] = pSrc[iSrcPos + i];
		//}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
/// Insert image channel
template < class TPxL, class TPxR >
bool COGLImage::FInsChanImg<TPxL, TPxR>::Execute( TPxL *pTrg, TPxL *pSrc, const PInsChanImg& rPar )
{
	int iSrcPos, iTrgPos;
	int iSrcY, iLineSize, iY2;

	iY2 = rPar.iTrgHeight - rPar.iTrgY - 1 - rPar.iSrcHeight + 1;
	iLineSize = rPar.iSrcWidth * sizeof(TPxL);
	if (rPar.iTrgC < 0 || rPar.iTrgC > int(TPxL::uColorCnt) ||
		rPar.iSrcC < 0 || rPar.iSrcC > int(TPxL::uColorCnt))
	{
		for (iSrcY = 0, iSrcPos = 0, iTrgPos = iY2*rPar.iTrgWidth + rPar.iTrgX; 
			iSrcY < rPar.iSrcHeight;
			iSrcY++, iSrcPos += rPar.iSrcWidth, iTrgPos += rPar.iTrgWidth)
		{
			memcpy(&pTrg[iTrgPos], &pSrc[iSrcPos], iLineSize);
		}
	}
	else
	{
		int iSrcX, iSrcPosX, iTrgPosX;

		for (iSrcY = 0, iSrcPos = 0, iTrgPos = iY2*rPar.iTrgWidth + rPar.iTrgX; 
			iSrcY < rPar.iSrcHeight;
			iSrcY++, iSrcPos += rPar.iSrcWidth, iTrgPos += rPar.iTrgWidth)
		{
			for (iSrcX = 0, iSrcPosX = iSrcPos, iTrgPosX = iTrgPos;
				iSrcX < rPar.iSrcWidth;
				 iSrcX++, iSrcPosX++, iTrgPosX++)
			{
				pTrg[iTrgPosX].value[rPar.iTrgC] = pSrc[iSrcPosX].value[rPar.iSrcC];
			}
		}
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////////////////////
/// Rotate Image in 90� steps
template < class TPxL, class TPxR >
bool COGLImage::FRot90Img<TPxL, TPxR>::Execute( TPxL *pTrg, TPxL *pSrc, const PRot90Img& rPar )
{
	int iSrcPos, iTrgPos, iSrcX, iSrcY, iSize;

	iSize = rPar.iTrgWidth * rPar.iTrgHeight;

	if (rPar.iSteps == 1)
	{
		for (iSrcY = 0, iSrcPos = 0; iSrcY < rPar.iSrcHeight; iSrcY++)
		{
			iTrgPos = rPar.iTrgWidth - iSrcY - 1;
			for (iSrcX = 0; iSrcX < rPar.iSrcWidth; iSrcX++, iSrcPos++, iTrgPos += rPar.iTrgWidth)
			{
				pTrg[iTrgPos] = pSrc[iSrcPos];
			} 
		}
	}
	else if (rPar.iSteps == 2)
	{
		for (iTrgPos = iSize - 1, iSrcPos = 0; iTrgPos >= 0; iTrgPos--, iSrcPos++)
		{
			pTrg[iTrgPos] = pSrc[iSrcPos];
		}
	}
	else if (rPar.iSteps == 3)
	{
		for (iSrcY = 0, iSrcPos = 0; iSrcY < rPar.iSrcHeight; iSrcY++)
		{
			iTrgPos = iSize - rPar.iTrgWidth + iSrcY;
			for (iSrcX = 0; iSrcX < rPar.iSrcWidth; iSrcX++, iSrcPos++, iTrgPos -= rPar.iTrgWidth)
			{
				pTrg[iTrgPos] = pSrc[iSrcPos];
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
/// Get Bounding Box
template < class TPxL >
bool COGLImage::FGetBoundBox<TPxL>::Execute( TPxL *pTrg, PGetBoundBox& rPar )
{
	int iMinX, iMaxX, iMinY, iMaxY, iX, iY, iIdx;

	if ( TPxL::uColorCnt == 3 )
	{
		TPxL pxVal;

		Float2Pixel(pxVal.r(), rPar.pfCol[0]);
		Float2Pixel(pxVal.g(), rPar.pfCol[1]);
		Float2Pixel(pxVal.b(), rPar.pfCol[2]);

		if ( TPxL::uAlphaCnt == 1 )
			Float2Pixel(pxVal.a(), 0.0f);
	
		iMinX = rPar.iWidth;
		iMaxX = 0;
		iMinY = rPar.iHeight;
		iMaxY = 0;

		for ( iY = rPar.iHeight-1, iIdx = 0; iY >= 0; iY-- )
		{
			for ( iX = 0; iX < rPar.iWidth; iX++, iIdx++ )
			{
				if ( pTrg[iIdx].r() != pxVal.r() ||
					 pTrg[iIdx].g() != pxVal.g() ||
					 pTrg[iIdx].b() != pxVal.b() )
				{
					if ( iX > iMaxX )
						iMaxX = iX;
					if ( iX < iMinX )
						iMinX = iX;
					if ( iY > iMaxY )
						iMaxY = iY;
					if ( iY < iMinY )
						iMinY = iY;
				}
			}
		}
	}
	else if ( TPxL::uColorCnt == 1 )
	{
		TPxL::TType xVal;

		Float2Pixel( xVal, (rPar.pfCol[0] + rPar.pfCol[1] + rPar.pfCol[2]) / 3.0f );

		iMinX = rPar.iWidth;
		iMaxX = 0;
		iMinY = rPar.iHeight;
		iMaxY = 0;

		for ( iY = rPar.iHeight-1, iIdx = 0; iY >= 0; iY-- )
		{
			for ( iX = 0; iX < rPar.iWidth; iX++, iIdx++ )
			{
				if ( pTrg[iIdx].r() != xVal )
				{
					if ( iX > iMaxX )
						iMaxX = iX;
					if ( iX < iMinX )
						iMinX = iX;
					if ( iY > iMaxY )
						iMaxY = iY;
					if ( iY < iMinY )
						iMinY = iY;
				}
			}
		}

	}

	if ( iMinX > iMaxX || iMinY > iMaxY )
	{
		rPar.piBox[0] = 0;
		rPar.piBox[1] = 0;
		rPar.piBox[2] = 0;
		rPar.piBox[3] = 0;
	}
	else
	{
		rPar.piBox[0] = iMinX;
		rPar.piBox[1] = iMinY;
		rPar.piBox[2] = iMaxX - iMinX + 1;
		rPar.piBox[3] = iMaxY - iMinY + 1;
	}

	return true;
}

