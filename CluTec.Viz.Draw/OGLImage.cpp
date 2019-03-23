////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLImage.cpp
//
// summary:   Implements the ogl image class
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

#include "StdAfx.h"
#include <iostream>
//#include "CvFltk\glut.h"
#include "OGLImage.h"
#include "limits.h"
#include "CluTec.Viz.Fltk\Fl_math.h"
#include <vector>
#include <algorithm>
#include <functional>		// For greater<int>( )

using namespace std;

//extern "C"
//{
//	int ConvertJPEGtoPS(const char* pcSrcFilename, const char* pcTrgFilename);
//}

////////////////////////////////////////////////////////////////////////////////////
/// Template Functions

#include "OGLImage_Pixel.cxx"
#include "OGLImage_Form.cxx"
#include "OGLImage_Mask.cxx"
#include "OGLImage_Arithmetic.cxx"
#include "OGLImage_Color.cxx"

#include "OGLImage_Exec_Single.cxx"
#include "OGLImage_Exec_EqualType.cxx"
#include "OGLImage_Exec_EqualDataType.cxx"
#include "OGLImage_Exec_AnyType.cxx"

/////////////////////////////////////////////////////////////////////////////////

COGLImage::COGLImage(void)
{
	ResetVars();
}

COGLImage::COGLImage(const COGLImage& OGLImage)
{
	*this = OGLImage;
}

COGLImage::~COGLImage(void)
{
	::LockImageAccess();
	ilDeleteImages(1, &m_uImgID);
	::UnlockImageAccess();
}

void COGLImage::ResetVars()
{
	::LockImageAccess();

	m_iWidth = 0;
	m_iHeight = 0;

	m_iOrigX = 0;
	m_iOrigY = 0;

	m_uImgID = 0;
	ilGenImages(1, &m_uImgID);
	ilBindImage(m_uImgID);
	ilClearColor(0, 0, 0, 1);

	// Convert the image to RGBA, uchar
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	iluEnlargeCanvas(1, 1, 1);
	ilClearImage();
	ilRegisterOrigin(IL_ORIGIN_UPPER_LEFT);

	m_csFilename = "";

	ResetPixelIdxLum();

	::UnlockImageAccess();
}

void COGLImage::ResetPixelIdxLum()
{
	if (m_pvecPixIdxLum[0].size())
	{
		for (int i = 0; i < 4; ++i)
		{
			m_pvecPixIdxLum[i].clear();
		}
	}
}

void COGLImage::ReversePixelIdxLum()
{
	for (int i = 0; i < 4; ++i)
	{
		vector<unsigned>& vecIdx = m_pvecPixIdxLum[i];
		size_t uIdx1, uIdx2, uCnt = vecIdx.size();
		size_t uMax = uCnt >> 1;
		size_t uVal;
		for (uIdx1 = 0, uIdx2 = uCnt - 1; uIdx1 < uMax; ++uIdx1, --uIdx2)
		{
			uVal = vecIdx[uIdx1];
			vecIdx[uIdx1] = vecIdx[uIdx2];
			vecIdx[uIdx2] = uint(uVal);
		}
	}
}

COGLImage& COGLImage::operator=(const COGLImage& OGLImage)
{
	::LockImageAccess();

	m_iWidth = OGLImage.m_iWidth;
	m_iHeight = OGLImage.m_iHeight;

	m_iOrigX = OGLImage.m_iOrigX;
	m_iOrigY = OGLImage.m_iOrigY;

	m_csFilename = OGLImage.m_csFilename;

	ilBindImage(OGLImage.m_uImgID);
	m_uImgID = ilCloneCurImage();

	for (int i = 0; i < 4; ++i)
	{
		m_pvecPixIdxLum[i] = OGLImage.m_pvecPixIdxLum[i];
	}

	::UnlockImageAccess();
	return *this;
}

/////////////////////////////////////////////////////////////////////
// Generate OpenGL texture

uint COGLImage::GenTexture()
{
	::LockImageAccess();

	ilBindImage(m_uImgID);
	uint uResult = ilutGLBindMipmaps();

	::UnlockImageAccess();
	return uResult;
}

/////////////////////////////////////////////////////////////////////
// Get Image and Data Type

void COGLImage::GetType(int& iImgType, int& iDataType, int& iBytesPerPixel) const
{
	::LockImageAccess();
	ilBindImage(m_uImgID);

	iImgType = ilGetInteger(IL_IMAGE_FORMAT);
	iDataType = ilGetInteger(IL_IMAGE_TYPE);
	iBytesPerPixel = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);

	::UnlockImageAccess();
}

/////////////////////////////////////////////////////////////////////
// Get number of color pixels

void COGLImage::GetComponentCount(int& iColorCnt, int& iAlphaCnt) const
{
	int iImgType, iDataType, iBPP;
	GetType(iImgType, iDataType, iBPP);

	switch (iImgType)
	{
	case CLUVIZ_IMG_RGB:
	case CLUVIZ_IMG_BGR:
		iColorCnt = 3;
		iAlphaCnt = 0;
		break;

	case CLUVIZ_IMG_RGBA:
	case CLUVIZ_IMG_BGRA:
		iColorCnt = 3;
		iAlphaCnt = 1;
		break;

	case CLUVIZ_IMG_LUMINANCE:
		iColorCnt = 1;
		iAlphaCnt = 0;
		break;

	case CLUVIZ_IMG_LUMINANCE_ALPHA:
		iColorCnt = 1;
		iAlphaCnt = 1;
		break;

	default:
		iColorCnt = 0;
		iAlphaCnt = 0;
		break;
	}
}

/////////////////////////////////////////////////////////////////////
// Get Pixel value at given position

bool COGLImage::GetPixel(int iX, int iY, double& dR, double& dG, double& dB, double& dA)
{
	if ((iX < 0) || (iX >= m_iWidth) ||
		(iY < 0) || (iY >= m_iHeight))
	{
		return false;
	}

	bool bRes = false;
	PGetPixelColor rPar(iX, iY, m_iWidth, m_iHeight);

	::LockImageAccess();

	bRes = ExecutePixelOperator<FGetPixelColor, PGetPixelColor>(*this, rPar);

	::UnlockImageAccess();

	dR = rPar.dR;
	dG = rPar.dG;
	dB = rPar.dB;
	dA = rPar.dA;

	return bRes;
}

/////////////////////////////////////////////////////////////////////
// Get Pixel normal of luminance at given position

bool COGLImage::GetPixelNormalLum(int iX, int iY, double& dX, double& dY, double& dZ)
{
	if ((iX < 0) || (iX >= m_iWidth) ||
		(iY < 0) || (iY >= m_iHeight))
	{
		return false;
	}

	bool bRes = false;
	PGetPixelNormalLum rPar(iX, iY, m_iWidth, m_iHeight);

	::LockImageAccess();

	bRes = ExecutePixelOperator<FGetPixelNormalLum, PGetPixelNormalLum>(*this, rPar);

	::UnlockImageAccess();

	dX = rPar.dX;
	dY = rPar.dY;
	dZ = rPar.dZ;

	return bRes;
}

/////////////////////////////////////////////////////////////////////
// Get Pixel with maximal luminance

bool COGLImage::GetPixelMaxLum(int piX[4], int piY[4], double pdCol[4], double dPart)
{
	bool bRes = false;
	PGetPixelMinMaxLum rPar(dPart, m_pvecPixIdxLum, m_iWidth, m_iHeight);

	::LockImageAccess();

	bRes = ExecutePixelOperator<FGetPixelMaxLum, PGetPixelMinMaxLum>(*this, rPar);

	for (int i = 0; i < 4; ++i)
	{
		piX[i] = rPar.piX[i];
		piY[i] = rPar.piY[i];
		pdCol[i] = rPar.pdCol[i];
	}

	::UnlockImageAccess();

	return bRes;
}

/////////////////////////////////////////////////////////////////////
// Get Pixel with minimal luminance

bool COGLImage::GetPixelMinLum(int piX[4], int piY[4], double pdCol[4], double dPart)
{
	bool bRes = false;
	PGetPixelMinMaxLum rPar(dPart, m_pvecPixIdxLum, m_iWidth, m_iHeight);

	::LockImageAccess();

	bRes = ExecutePixelOperator<FGetPixelMinLum, PGetPixelMinMaxLum>(*this, rPar);

	for (int i = 0; i < 4; ++i)
	{
		piX[i] = rPar.piX[i];
		piY[i] = rPar.piY[i];
		pdCol[i] = rPar.pdCol[i];
	}

	::UnlockImageAccess();

	return bRes;
}

/////////////////////////////////////////////////////////////////////
// Multiply image with scalar

bool COGLImage::OpMultScalar(COGLImage& rSrcImg, double dFactor)
{
	int iWidth, iHeight;
	int iImgType, iDataType, iBPP;

	::LockImageAccess();

	rSrcImg.GetSize(iWidth, iHeight);
	rSrcImg.GetType(iImgType, iDataType, iBPP);

	ResizeCanvas(1, 1);
	ConvertType(iImgType, iDataType);
	ResizeCanvas(iWidth, iHeight);

	bool bRes = ExecutePixelOperator_EqualType<FOpMultScalar, POpScalar>(*this, rSrcImg, POpScalar(dFactor, m_iWidth, m_iHeight));

	::UnlockImageAccess();
	return bRes;
}

/////////////////////////////////////////////////////////////////////
// Take reciprocal of each pixel and multiply with scalar

bool COGLImage::OpRecipScalar(COGLImage& rSrcImg, double dFactor)
{
	int iWidth, iHeight;
	int iImgType, iDataType, iBPP;

	::LockImageAccess();

	ResetPixelIdxLum();

	rSrcImg.GetSize(iWidth, iHeight);
	rSrcImg.GetType(iImgType, iDataType, iBPP);

	ResizeCanvas(1, 1);
	ConvertType(iImgType, iDataType);
	ResizeCanvas(iWidth, iHeight);

	bool bRes = ExecutePixelOperator_EqualType<FOpRecipScalar, POpScalar>(*this, rSrcImg, POpScalar(dFactor, m_iWidth, m_iHeight));

	::UnlockImageAccess();
	return bRes;
}

/////////////////////////////////////////////////////////////////////
// Add scalar to image

bool COGLImage::OpAddScalar(COGLImage& rSrcImg, double dValue)
{
	int iWidth, iHeight;
	int iImgType, iDataType, iBPP;

	::LockImageAccess();

	rSrcImg.GetSize(iWidth, iHeight);
	rSrcImg.GetType(iImgType, iDataType, iBPP);

	ResizeCanvas(1, 1);
	ConvertType(iImgType, iDataType);
	ResizeCanvas(iWidth, iHeight);

	bool bRes = ExecutePixelOperator_EqualType<FOpAddScalar, POpScalar>(*this, rSrcImg, POpScalar(dValue, m_iWidth, m_iHeight));

	::UnlockImageAccess();
	return bRes;
}

/////////////////////////////////////////////////////////////////////
// Negate image and add scalar

bool COGLImage::OpNegAddScalar(COGLImage& rSrcImg, double dValue)
{
	int iWidth, iHeight;
	int iImgType, iDataType, iBPP;

	::LockImageAccess();

	ResetPixelIdxLum();

	rSrcImg.GetSize(iWidth, iHeight);
	rSrcImg.GetType(iImgType, iDataType, iBPP);

	ResizeCanvas(1, 1);
	ConvertType(iImgType, iDataType);
	ResizeCanvas(iWidth, iHeight);

	bool bRes = ExecutePixelOperator_EqualType<FOpNegAddScalar, POpScalar>(*this, rSrcImg, POpScalar(dValue, m_iWidth, m_iHeight));

	::UnlockImageAccess();
	return bRes;
}

/////////////////////////////////////////////////////////////////////
// Multiply image with scalar

bool COGLImage::OpMultColor(COGLImage& rSrcImg, const COGLColor& colVal)
{
	int iWidth, iHeight;
	int iImgType, iDataType, iBPP;

	::LockImageAccess();

	rSrcImg.GetSize(iWidth, iHeight);
	rSrcImg.GetType(iImgType, iDataType, iBPP);

	ResizeCanvas(1, 1);
	ConvertType(iImgType, iDataType);
	ResizeCanvas(iWidth, iHeight);

	bool bRes = ExecutePixelOperator_EqualType<FOpMultColor, POpColor>(*this, rSrcImg, POpColor(colVal, m_iWidth, m_iHeight));

	::UnlockImageAccess();
	return bRes;
}

/////////////////////////////////////////////////////////////////////
// Take reciprocal of each pixel and multiply with color

bool COGLImage::OpRecipColor(COGLImage& rSrcImg, const COGLColor& colVal)
{
	int iWidth, iHeight;
	int iImgType, iDataType, iBPP;

	::LockImageAccess();

	ResetPixelIdxLum();

	rSrcImg.GetSize(iWidth, iHeight);
	rSrcImg.GetType(iImgType, iDataType, iBPP);

	ResizeCanvas(1, 1);
	ConvertType(iImgType, iDataType);
	ResizeCanvas(iWidth, iHeight);

	bool bRes = ExecutePixelOperator_EqualType<FOpRecipColor, POpColor>(*this, rSrcImg, POpColor(colVal, m_iWidth, m_iHeight));

	::UnlockImageAccess();
	return bRes;
}

/////////////////////////////////////////////////////////////////////
// Add color to image

bool COGLImage::OpAddColor(COGLImage& rSrcImg, const COGLColor& colVal)
{
	int iWidth, iHeight;
	int iImgType, iDataType, iBPP;

	::LockImageAccess();

	rSrcImg.GetSize(iWidth, iHeight);
	rSrcImg.GetType(iImgType, iDataType, iBPP);

	ResizeCanvas(1, 1);
	ConvertType(iImgType, iDataType);
	ResizeCanvas(iWidth, iHeight);

	bool bRes = ExecutePixelOperator_EqualType<FOpAddColor, POpColor>(*this, rSrcImg, POpColor(colVal, m_iWidth, m_iHeight));

	::UnlockImageAccess();
	return bRes;
}

/////////////////////////////////////////////////////////////////////
// Negate image and add color

bool COGLImage::OpNegAddColor(COGLImage& rSrcImg, const COGLColor& colVal)
{
	int iWidth, iHeight;
	int iImgType, iDataType, iBPP;

	::LockImageAccess();

	ResetPixelIdxLum();

	rSrcImg.GetSize(iWidth, iHeight);
	rSrcImg.GetType(iImgType, iDataType, iBPP);

	ResizeCanvas(1, 1);
	ConvertType(iImgType, iDataType);
	ResizeCanvas(iWidth, iHeight);

	bool bRes = ExecutePixelOperator_EqualType<FOpNegAddColor, POpColor>(*this, rSrcImg, POpColor(colVal, m_iWidth, m_iHeight));

	::UnlockImageAccess();
	return bRes;
}

/////////////////////////////////////////////////////////////////////
// Negate image

bool COGLImage::OpNeg(COGLImage& rSrcImg)
{
	int iWidth, iHeight;
	int iImgType, iDataType, iBPP;

	::LockImageAccess();

	ResetPixelIdxLum();

	rSrcImg.GetSize(iWidth, iHeight);
	rSrcImg.GetType(iImgType, iDataType, iBPP);

	ResizeCanvas(1, 1);
	ConvertType(iImgType, iDataType);
	ResizeCanvas(iWidth, iHeight);

	bool bRes = ExecutePixelOperator_EqualType<FOpNeg, PPixCnt>(*this, rSrcImg, PPixCnt(m_iWidth * m_iHeight));

	::UnlockImageAccess();
	return bRes;
}

/////////////////////////////////////////////////////////////////////
// Reduce Image to given size

bool COGLImage::ExtractSubImage(COGLImage& rSrcImg, int iX, int iY, int iCX, int iCY)
{
	int iWidth, iHeight;

	ResetPixelIdxLum();

	rSrcImg.GetSize(iWidth, iHeight);

	if ((iCX <= 0) || (iCY <= 0) ||
		(iX < 0) || (iX + iCX - 1 >= iWidth) ||
		(iY < 0) || (iY + iCY - 1 >= iHeight))
	{
		return false;
	}

	::LockImageAccess();

	ResizeCanvas(iCX, iCY);

	bool bRes = ExecutePixelOperator_EqualType<FGetSubImg, PGetSubImg>(*this, rSrcImg, PGetSubImg(iWidth, iHeight, iX, iY, iCX, iCY));

	::UnlockImageAccess();
	return bRes;

	//m_iWidth = iCX;
	//m_iHeight = iCY;

	//return true;

	//ilBindImage(m_uImgID);
	//TRGBAPixel *pSrc = (TRGBAPixel *) rSrcImg.GetDataPtr();
	//TRGBAPixel *pTrg = (TRGBAPixel *) GetDataPtr();

	////ilCopyImage(rSrcImg.m_uImgID);
	////if (iluCrop(iX, iY, 0, iCX, iCY, 1) == IL_FALSE)
	//	//return false;

	//iSrcPos = (iHeight - iY - 1 - iCY + 1) * iWidth + iX;
	//iTrgPos = 0;
	//for(iRow = 0; iRow < iCY; iRow++, iSrcPos += iWidth, iTrgPos += iCX)
	//{
	//	memcpy(&pTrg[iTrgPos], &pSrc[iSrcPos], sizeof(TRGBAPixel) * iCX);
	//}

	//m_iWidth = iCX;
	//m_iHeight = iCY;

	//return true;
}

/////////////////////////////////////////////////////////////////////
// Insert sub-Image

bool COGLImage::InsertSubImage(COGLImage& rSrcImg, int iSrcX, int iSrcY, int iSrcW, int iSrcH, int iTrgX, int iTrgY)
{
	int iWidth, iHeight;	//, iRow, iSrcPos, iTrgPos, iRowLen;

	ResetPixelIdxLum();

	rSrcImg.GetSize(iWidth, iHeight);

	// Check whether source sub image dimensions are OK
	if ((iSrcW <= 0) || (iSrcH <= 0) ||
		(iSrcX < 0) || (iSrcY < 0) ||
		(iSrcX + iSrcW - 1 >= iWidth) ||
		(iSrcY + iSrcH - 1 >= iHeight))
	{
		return false;
	}

	// Check whether sub image fits into this image
	if ((iTrgX < 0) || (iTrgY < 0) ||
		(iTrgX + iSrcW - 1 >= m_iWidth) ||
		(iTrgY + iSrcH - 1 >= m_iHeight))
	{
		return false;
	}

	::LockImageAccess();

	bool bRes = ExecutePixelOperator_EqualType<FInsSubImg, PInsSubImg>(*this, rSrcImg,
		PInsSubImg(iWidth, iHeight, m_iWidth, m_iHeight, iSrcX, iSrcY, iSrcW, iSrcH, iTrgX, iTrgY));

	::UnlockImageAccess();
	return bRes;

	//ilBindImage( m_uImgID );
	//TRGBAPixel *pSrc = (TRGBAPixel *) rSrcImg.GetDataPtr();
	//TRGBAPixel *pTrg = (TRGBAPixel *) GetDataPtr();

	//iSrcPos = (iHeight - iSrcY - 1 - iSrcH + 1) * iWidth + iSrcX;
	//iTrgPos = (m_iHeight - iTrgY - 1 - iSrcH + 1) * m_iWidth + iTrgX;
	//iRowLen = sizeof(TRGBAPixel) * iSrcW;

	//for(iRow = 0; iRow < iSrcH; iRow++, iSrcPos += iWidth, iTrgPos += m_iWidth)
	//{
	//	memcpy( &pTrg[iTrgPos], &pSrc[iSrcPos], iRowLen );
	//	//for ( int i = 0; i < iSrcW; ++i )
	//	//{
	//	//	pTrg[iTrgPos + i] = pSrc[iSrcPos + i];
	//	//}
	//}

	//return true;
}

/////////////////////////////////////////////////////////////////////
/// Get Data Pointer

COGLImage::uchar* COGLImage::GetDataPtr()
{
	ilBindImage(m_uImgID);
	return (uchar*)ilGetData();
}

const COGLImage::uchar* COGLImage::GetDataPtr() const
{
	ilBindImage(m_uImgID);
	return (const uchar*)ilGetData();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 	Sample an area instead of a single pixel.
/// </summary>
///
/// <param name="fPosX">			 Zero-based index of the position x coordinate. </param>
/// <param name="fPosY">			 Zero-based index of the position y coordinate. </param>
/// <param name="fAreaSize">		 Size of the area. </param>
/// <param name="pucSampleType">	 [in,out] If non-null, the puc sample type. </param>
/// <param name="pucSampleAreaType"> [in,out] If non-null, the puc sample area type. </param>
/// <param name="fValueToIgnore">    The value to ignore. </param>
///
/// <returns> . </returns>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<float> COGLImage::SampleArea(float fPosX, float fPosY, float fAreaSize, char* pucSampleType, char* pucSampleAreaType, float fValueToIgnore)
{
	std::string sSampleType(pucSampleType);
	std::string sSampleAreaType(pucSampleAreaType);

	// Check if we have circular or box shape
	bool bCircleShape = false;
	if (sSampleAreaType.compare("circle") == 0)
	{
		bCircleShape = true;
	}

	// Init the result vector with 0
	std::vector<float> vfColor(4);
	for (int i = 0; i < 4; i++)
	{
		vfColor[i] = 0;
	}

	// Prepare a vector of all values for median sampling
	std::vector< std::list<float> > vlfMedianColor(4);

	// Check if the ValueToIgnore is Nan.
	// If this is the case we have to test this separate, because NaN will always fail every compare operation
	bool bIgnoreNan = false;
	if (isnan(fValueToIgnore))
	{
		bIgnoreNan = true;
	}

	// Get the radius of the are
	float fRadius = ((fAreaSize - 1.0f) / 2.0f);
	// Iterate over a rectangular area and perform sampling
	int iSampleCounter = 0;
	for (float x = -fRadius; x <= fRadius; x += 1.0)
	{
		for (float y = -fRadius; y <= fRadius; y += 1.0)
		{
			// Get the integer Sample position and add -0.5 pitch to behave according to the PXA sampling pattern
			int iSamplePosX = (int)(fPosX + x - 0.5f);
			int iSamplePosY = (int)(fPosY + y - 0.5f);

			// Check the image bounds
			if ((iSamplePosX < 0) || (iSamplePosX >= m_iWidth))
			{
				continue;
			}

			// Check image bounds
			if ((iSamplePosY < 0) || (iSamplePosY >= m_iHeight))
			{
				continue;
			}

			// If a circular shape is selected, skip if we are out of the circle
			if (bCircleShape)
			{
				// The distance in pixel units has to be pitched by one to adjust the offset when sampling the image (first pixel has index 1)
				float fDX = fPosX - ((float)(iSamplePosX)+1.0f);
				float fDY = fPosY - ((float)(iSamplePosY)+1.0f);
				float fEucledianDistance = sqrt(fDX * fDX + fDY * fDY);
				if (fEucledianDistance > (fAreaSize / 2.0))
				{
					continue;
				}
			}

			// Get the pixel value
			double r, g, b, a;
			GetPixel(iSamplePosX, iSamplePosY, r, g, b, a);

			// If we want to ignore NaN and got one -> skip this pixel
			if (bIgnoreNan && isnan(r))
			{
				continue;
			}

			// This value is ignored, skip the pixel
			if (r == fValueToIgnore)
			{
				continue;
			}

			// With median sampling, we need to store ALL sample values in a sortable list
			if (sSampleType.compare("median") == 0)
			{
				vlfMedianColor[0].push_back((float)r);
				vlfMedianColor[1].push_back((float)g);
				vlfMedianColor[2].push_back((float)b);
				vlfMedianColor[3].push_back((float)a);
			}
			else
			{
				vfColor[0] += (float)r;
				vfColor[1] += (float)g;
				vfColor[2] += (float)b;
				vfColor[3] += (float)a;
			}

			iSampleCounter++;
		}
	}

	// Get the average value
	if (sSampleType.compare("mean") == 0)
	{
		for (int i = 0; i < 4; i++)
		{
			if (iSampleCounter > 0)
			{
				vfColor[i] /= (float)iSampleCounter;
			}
			else
			{
				vfColor[i] = nanf("");
			}
		}
	}	// Sort the list and get the median value
	else if (sSampleType.compare("median") == 0)
	{
		for (int i = 0; i < 4; i++)
		{
			if (iSampleCounter > 0)
			{
				// Sort the sample values
				vlfMedianColor[i].sort();

				// Pop first half
				size_t iSize = vlfMedianColor[i].size();
				for (int j = 0; j < (int)iSize / 2; j++)
				{
					vlfMedianColor[i].pop_front();
				}

				// Retrieve the center element
				vfColor[i] = vlfMedianColor[i].front();
			}
			else
			{
				// Without samples, the result is 0
				vfColor[i] = nanf("");
			}
		}
	}

	return vfColor;
}

/////////////////////////////////////////////////////////////////////
/// Set Text

void COGLImage::SetFilename(const char* pcFilename)
{
	if (pcFilename)
	{
		m_csFilename = pcFilename;
	}
}

/////////////////////////////////////////////////////////////////////
/// Flush RGB

void COGLImage::FlushRGB(COGLColor& rCol)
{
	::LockImageAccess();

	ResetPixelIdxLum();

	ExecutePixelOperator<FFlushRGB, PFlushRGB>(*this, PFlushRGB(m_iWidth * m_iHeight, rCol));

	::UnlockImageAccess();
}

/////////////////////////////////////////////////////////////////////
/// Set Size of bitmap

bool COGLImage::SetSize(int iWidth, int iHeight, COGLColor* pcolClear)
{
	::LockImageAccess();

	if ((iWidth <= 0) || (iHeight <= 0))
	{
		::UnlockImageAccess();
		return false;
	}

	ResetPixelIdxLum();
	ilBindImage(m_uImgID);

	int iImgType = ilGetInteger(IL_IMAGE_FORMAT);
	int iDataType = ilGetInteger(IL_IMAGE_TYPE);

	ilDeleteImage(m_uImgID);
	ilGenImages(1, &m_uImgID);
	ilBindImage(m_uImgID);
	ilClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	if (pcolClear)
	{
		ilClearColor((*pcolClear)[0], (*pcolClear)[1], (*pcolClear)[2], (*pcolClear)[3]);
	}

	// Convert the image to the previous type
	ilConvertImage(iImgType, iDataType);
	iluEnlargeCanvas(iWidth, iHeight, 1);
	ilClearImage();
	ilRegisterOrigin(IL_ORIGIN_UPPER_LEFT);

	m_iWidth = iWidth;
	m_iHeight = iHeight;

	::UnlockImageAccess();
	return true;
}

/////////////////////////////////////////////////////////////////////
// Clear Image

bool COGLImage::ClearImage(COGLColor* pcolClear)
{
	::LockImageAccess();
	ResetPixelIdxLum();
	ilBindImage(m_uImgID);

	if (pcolClear)
	{
		ilClearColor((*pcolClear)[0], (*pcolClear)[1], (*pcolClear)[2], (*pcolClear)[3]);
	}

	ilClearImage();

	if (pcolClear)
	{
		ilClearColor(0, 0, 0, 1);
	}
	::UnlockImageAccess();

	return true;
}

/////////////////////////////////////////////////////////////////////
/// Resize the Image

bool COGLImage::ResizeImage(int iWidth, int iHeight)
{
	if ((iWidth <= 0) || (iHeight <= 0))
	{
		return false;
	}

	::LockImageAccess();

	ResetPixelIdxLum();
	ilBindImage(m_uImgID);
	iluImageParameter(ILU_FILTER, ILU_BILINEAR);

	if ((iWidth < m_iWidth) || (iHeight < m_iHeight))
	{
		//int iFactorW = m_iWidth / iWidth + 1;
		//int iFactorH = m_iHeight / iHeight + 1;

		//int iFactor = ( iFactorW < iFactorH ? iFactorH : iFactorW );

		if (iluBlurGaussian(1) == IL_FALSE)
		{
			::UnlockImageAccess();
			return false;
		}
	}

	if (iluScale(iWidth, iHeight, 1) == IL_FALSE)
	{
		::UnlockImageAccess();
		return false;
	}

	m_iWidth = iWidth;
	m_iHeight = iHeight;

	::UnlockImageAccess();

	return true;
}

/////////////////////////////////////////////////////////////////////
/// Rotate the Image

bool COGLImage::RotateImage(float fAngle)
{
	//ILimage *pCurImg, *pRotImg;
	::LockImageAccess();

	ResetPixelIdxLum();
	ilBindImage(m_uImgID);
	//pCurImg = ilGetCurImage();
	iluRotate(fAngle);
	/*
	pRotImg = iluRotate_(pCurImg, fAngle);

	ilTexImage(pRotImg->Width, pRotImg->Height, pRotImg->Depth, pRotImg->Bpp,
	pRotImg->Format, pRotImg->Type, pRotImg->Data);
	ilCopyImageAttr(pCurImg, pRotImg);
	ilCloseImg(pRotImg);
	*/
	::UnlockImageAccess();

	return true;
}

/////////////////////////////////////////////////////////////////////
/// Flip Image horizontally or vertically
// If bHorizontal == false, flips vertically.

bool COGLImage::FlipImage(bool bHorizontal)
{
	::LockImageAccess();

	ilBindImage(m_uImgID);

	if (bHorizontal)
	{
		iluMirror();
	}
	else
	{
		iluFlipImage();
	}

	::UnlockImageAccess();

	return true;
}

/////////////////////////////////////////////////////////////////////
/// Resize the Canvas

bool COGLImage::ResizeCanvas(int iWidth, int iHeight, COGLColor* pcolClear, double dAlignX, double dAlignY)
{
	if ((iWidth <= 0) || (iHeight <= 0))
	{
		return false;
	}
	else if ((iWidth == m_iWidth) && (iHeight == m_iHeight))
	{
		return true;
	}

	int iSrcX, iSrcY, iSrcW, iSrcH;
	int iTrgX, iTrgY;
	int iSrcMinX, iSrcMinY, iSrcMaxX, iSrcMaxY;
	int iImgType, iDataType, iBpP;

	COGLImage* pImgSrc;

	if (!(pImgSrc = new COGLImage(*this)))
	{
		return false;
	}

	ResetPixelIdxLum();
	iSrcX = iSrcY = 0;
	pImgSrc->GetSize(iSrcW, iSrcH);
	pImgSrc->GetType(iImgType, iDataType, iBpP);

	iTrgX = iTrgY = 0;

	// Position of lower left corner of source image in target image
	iSrcMinX = int(floor(double(iWidth) * dAlignX)) - int(floor(double(iSrcW) * dAlignX));
	iSrcMinY = int(floor(double(iHeight) * dAlignY)) - int(floor(double(iSrcH) * dAlignY));

	// Position of upper right corner of source image in target image
	iSrcMaxX = iSrcMinX + iSrcW - 1;
	iSrcMaxY = iSrcMinY + iSrcH - 1;

	// Origin in source image
	if (iSrcMinX < 0)
	{
		iTrgX = 0;
		iSrcX = -iSrcMinX;
		iSrcW += iSrcMinX;
	}
	else
	{
		iTrgX = iSrcMinX;
	}

	if (iSrcMinY < 0)
	{
		iTrgY = 0;
		iSrcY = -iSrcMinY;
		iSrcH += iSrcMinY;
	}
	else
	{
		iTrgY = iSrcMinY;
	}

	// Width / Height of source image that remains in target image
	if (iSrcMaxX >= iWidth)
	{
		iSrcW -= iSrcMaxX - iWidth + 1;
	}

	if (iSrcMaxY >= iHeight)
	{
		iSrcH -= iSrcMaxY - iHeight + 1;
	}

	::LockImageAccess();

	// Delete current image and create new one of asked for size
	ilDeleteImages(1, &m_uImgID);
	m_uImgID = 0;

	ilGenImages(1, &m_uImgID);
	ilBindImage(m_uImgID);

	if (pcolClear)
	{
		ilClearColor((*pcolClear)[0], (*pcolClear)[1], (*pcolClear)[2], (*pcolClear)[3]);
	}
	else
	{
		ilClearColor(0, 0, 0, 1);
	}

	// Convert the image to previous type
	ilConvertImage(iImgType, iDataType);
	iluEnlargeCanvas(iWidth, iHeight, 1);
	ilClearImage();
	ilRegisterOrigin(IL_ORIGIN_UPPER_LEFT);

	// Set new width and height
	m_iWidth = iWidth;
	m_iHeight = iHeight;

	// Now insert previous image into new one if necessary
	if ((iSrcW > 0) && (iSrcH > 0) && (iTrgX < iWidth) && (iTrgY < iHeight))
	{
		InsertSubImage(*pImgSrc, iSrcX, iSrcY, iSrcW, iSrcH, iTrgX, iTrgY);
	}

	// Delete previous image
	delete pImgSrc;

	// Reset clear color
	if (pcolClear)
	{
		ilClearColor(0, 0, 0, 1);
	}

	//iluImageParameter(ILU_PLACEMENT, ILU_CENTER);
	//
	//if (pcolClear)
	//{
	//	ilClearColor((*pcolClear)[0],(*pcolClear)[1],(*pcolClear)[2],(*pcolClear)[3]);
	//}

	//if (iWidth >= m_iWidth && iHeight >= m_iHeight)
	//{
	//	if (iluEnlargeCanvas(iWidth, iHeight, 1) == IL_FALSE)
	//	{
	//		if (pcolClear)
	//			ilClearColor(0,0,0,1);
	//		return false;
	//	}
	//}
	//else if (iWidth <= m_iWidth && iHeight <= m_iHeight)
	//{
	//	iOffX = (m_iWidth - iWidth) >> 1;
	//	iOffY = (m_iHeight - iHeight) >> 1;

	//	if (iluCrop(iOffX, iOffY, 0, iWidth, iHeight, 1) == IL_FALSE)
	//	{
	//		if (pcolClear)
	//			ilClearColor(0,0,0,1);
	//		return false;
	//	}
	//}
	//else if (iWidth <= m_iWidth && iHeight >= m_iHeight)
	//{
	//	if (iluEnlargeCanvas(m_iWidth, iHeight, 1) == IL_FALSE)
	//	{
	//		if (pcolClear)
	//			ilClearColor(0,0,0,1);
	//		return false;
	//	}

	//	iOffX = (m_iWidth - iWidth) >> 1;

	//	if (iluCrop(iOffX, 0, 0, iWidth, iHeight, 1) == IL_FALSE)
	//	{
	//		if (pcolClear)
	//			ilClearColor(0,0,0,1);
	//		return false;
	//	}
	//}
	//else if (iWidth >= m_iWidth && iHeight <= m_iHeight)
	//{
	//	if (iluEnlargeCanvas(iWidth, m_iHeight, 1) == IL_FALSE)
	//	{
	//		if (pcolClear)
	//			ilClearColor(0,0,0,1);
	//		return false;
	//	}

	//	iOffY = (m_iHeight - iHeight) >> 1;

	//	if (iluCrop(0, iOffY, 0, iWidth, iHeight, 1) == IL_FALSE)
	//	{
	//		if (pcolClear)
	//			ilClearColor(0,0,0,1);
	//		return false;
	//	}
	//}

	//if (pcolClear)
	//	ilClearColor(0,0,0,1);

	//m_iWidth = iWidth;
	//m_iHeight = iHeight;

	::UnlockImageAccess();

	return true;
}

/////////////////////////////////////////////////////////////////////
/// Add a single pixel border. Color is given by nearest pixel in image.

bool COGLImage::AddBorder()
{
	::LockImageAccess();

	ResetPixelIdxLum();
	ilBindImage(m_uImgID);
	iluImageParameter(ILU_PLACEMENT, ILU_CENTER);
	if (iluEnlargeCanvas(m_iWidth + 2, m_iHeight + 2, 1) == IL_FALSE)
	{
		::UnlockImageAccess();
		return false;
	}

	m_iWidth += 2;
	m_iHeight += 2;

	int iBytesPerPixel = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
	uchar* pData = (uchar*)ilGetData();

	// Top Border
	memcpy(&pData[iBytesPerPixel],
		&pData[(m_iWidth + 1) * iBytesPerPixel],
		(m_iWidth - 2) * iBytesPerPixel);

	// Bottom Border
	memcpy(&pData[((m_iHeight - 1) * m_iWidth + 1) * iBytesPerPixel],
		&pData[((m_iHeight - 2) * m_iWidth + 1) * iBytesPerPixel],
		(m_iWidth - 2) * iBytesPerPixel);

	int iY, iOffset;
	for (iY = 0, iOffset = 0; iY < m_iHeight; ++iY, iOffset += m_iWidth)
	{
		memcpy(&pData[iOffset * iBytesPerPixel],
			&pData[(iOffset + 1) * iBytesPerPixel],
			iBytesPerPixel);

		memcpy(&pData[(iOffset + m_iWidth - 1) * iBytesPerPixel],
			&pData[(iOffset + m_iWidth - 2) * iBytesPerPixel],
			iBytesPerPixel);
	}

	::UnlockImageAccess();

	return true;
}

/////////////////////////////////////////////////////////////////////
/// Insert Image
/// Overwrite part of this image with given image starting at pos iX, iY.
/// If given image would extend over this image, then false is returned
/// and this image is not changed.

bool COGLImage::InsertImage(COGLImage& rSrcImg, int iX, int iY, int iTrgC, int iSrcC)
{
	//int iSrcW, iSrcH, iSrcPos, iTrgPos, iSrcY, iLineSize, iY2;
	//TRGBAPixel *pTrgData, *pSrcData;

	int iSrcW, iSrcH;

	ResetPixelIdxLum();
	rSrcImg.GetSize(iSrcW, iSrcH);

	if ((iX + iSrcW > m_iWidth) ||
		(iY + iSrcH > m_iHeight) ||
		(iX < 0) || (iY < 0))
	{
		return false;
	}

	::LockImageAccess();

	bool bRes = ExecutePixelOperator_EqualType<FInsChanImg, PInsChanImg>(*this, rSrcImg,
		PInsChanImg(iSrcW, iSrcH, m_iWidth, m_iHeight, iX, iY, iSrcC, iTrgC));

	::UnlockImageAccess();
	return bRes;
	/*
			pSrcData = (TRGBAPixel *) rSrcImg.GetDataPtr();

			ilBindImage(m_uImgID);
			pTrgData = (TRGBAPixel *) ilGetData();

			iY2 = m_iHeight - iY - 1 - iSrcH + 1;
			iLineSize = iSrcW * sizeof(TRGBAPixel);
			if (iTrgC < 0 || iTrgC > 3 ||
			iSrcC < 0 || iSrcC > 3)
			{
			for (iSrcY = 0, iSrcPos = 0, iTrgPos = iY2*m_iWidth + iX;
			iSrcY < iSrcH;
			iSrcY++, iSrcPos += iSrcW, iTrgPos += m_iWidth)
			{
			memcpy(&pTrgData[iTrgPos], &pSrcData[iSrcPos], iLineSize);
			}
			}
			else
			{
			int iSrcX, iSrcPosX, iTrgPosX;

			for (iSrcY = 0, iSrcPos = 0, iTrgPos = iY2*m_iWidth + iX;
			iSrcY < iSrcH;
			iSrcY++, iSrcPos += iSrcW, iTrgPos += m_iWidth)
			{
			for (iSrcX = 0, iSrcPosX = iSrcPos, iTrgPosX = iTrgPos;
			iSrcX < iSrcW;
			iSrcX++, iSrcPosX++, iTrgPosX++)
			{
			pTrgData[iTrgPosX].c[iTrgC] = pSrcData[iSrcPosX].c[iSrcC];
			}
			}
			}
			*/
	//return true;
}

/////////////////////////////////////////////////////////////////////
/// Rotate Image in 90 degree steps
///

bool COGLImage::Rotate90Image(int iSteps)
{
	if (iSteps < 0)
	{
		return false;
	}

	iSteps = iSteps % 4;

	if (iSteps == 0)
	{
		return true;
	}

	::LockImageAccess();

	COGLImage imgSrc(*this);

	int iSrcW, iSrcH;
	imgSrc.GetSize(iSrcW, iSrcH);

	if ((iSteps == 1) || (iSteps == 3))
	{
		ResizeCanvas(m_iHeight, m_iWidth);
	}

	bool bRes = ExecutePixelOperator_EqualType<FRot90Img, PRot90Img>(*this, imgSrc,
		PRot90Img(iSrcW, iSrcH, m_iWidth, m_iHeight, iSteps));

	::UnlockImageAccess();
	return bRes;
}

/////////////////////////////////////////////////////////////////////
/// Returns mask of same dimension as image. The mask is set to 1
/// whereever the image is equal to the given color.

bool COGLImage::MaskEqual(COGLImage& rMask, COGLColor& rCol)
{
	int iImgType, iDataType, iBPP;

	::LockImageAccess();

	this->GetType(iImgType, iDataType, iBPP);
	rMask.ConvertType(iImgType, iDataType);
	rMask.SetSize(m_iWidth, m_iHeight);

	bool bRes = ExecutePixelOperator_EqualType<FMaskEqualCol, PMaskEqualCol>(rMask, *this,
		PMaskEqualCol(m_iWidth * m_iHeight, rCol));

	::UnlockImageAccess();
	return bRes;
}

/////////////////////////////////////////////////////////////////////
/// Returns mask of same dimension as image. The mask is set to 1
/// whereever the image is NOT equal to the given color.

bool COGLImage::MaskNotEqual(COGLImage& rMask, COGLColor& rCol)
{
	int iImgType, iDataType, iBPP;

	::LockImageAccess();

	this->GetType(iImgType, iDataType, iBPP);
	rMask.ConvertType(iImgType, iDataType);
	rMask.SetSize(m_iWidth, m_iHeight);

	bool bRes = ExecutePixelOperator_EqualType<FMaskNotEqualCol, PMaskEqualCol>(rMask, *this,
		PMaskEqualCol(m_iWidth * m_iHeight, rCol));

	::UnlockImageAccess();
	return bRes;
}

/////////////////////////////////////////////////////////////////////
/// Returns mask of same dimension as image. The mask is set to 1
/// whereever a pixel in this image is equal to the corresponding
/// image in rImage.

bool COGLImage::MaskEqual(COGLImage& rMask, const COGLImage& rImage)
{
	int iImgType, iDataType, iBPP;
	int iWidth, iHeight;

	rImage.GetSize(iWidth, iHeight);
	if ((iWidth != m_iWidth) || (iHeight != m_iHeight))
	{
		return false;
	}

	::LockImageAccess();

	this->GetType(iImgType, iDataType, iBPP);
	rMask.ConvertType(iImgType, iDataType);
	rMask.SetSize(m_iWidth, m_iHeight);

	bool bRes = ExecutePixelOperator_EqualType<FMaskEqualImg, PPixCnt>(rMask, *this, rImage,
		PPixCnt(m_iWidth * m_iHeight));

	::UnlockImageAccess();
	return bRes;
}

/////////////////////////////////////////////////////////////////////
/// Returns mask of same dimension as image. The mask is set to 1
/// whereever the image is NOT equal to the given color.

bool COGLImage::MaskNotEqual(COGLImage& rMask, const COGLImage& rImage)
{
	int iImgType, iDataType, iBPP;
	int iWidth, iHeight;

	rImage.GetSize(iWidth, iHeight);
	if ((iWidth != m_iWidth) || (iHeight != m_iHeight))
	{
		return false;
	}

	::LockImageAccess();

	this->GetType(iImgType, iDataType, iBPP);
	rMask.ConvertType(iImgType, iDataType);
	rMask.SetSize(m_iWidth, m_iHeight);

	bool bRes = ExecutePixelOperator_EqualType<FMaskNotEqualImg, PPixCnt>(rMask, *this, rImage,
		PPixCnt(m_iWidth * m_iHeight));

	::UnlockImageAccess();
	return bRes;
}

/////////////////////////////////////////////////////////////////////
/// Returns mask of same dimension as image.
/// The mask is the result of a logic AND operation between the two
/// images.

bool COGLImage::MaskLogicAND(COGLImage& rMask, const COGLImage& rImage)
{
	int iImgType, iDataType, iBPP;
	int iWidth, iHeight;

	rImage.GetSize(iWidth, iHeight);
	if ((iWidth != m_iWidth) || (iHeight != m_iHeight))
	{
		return false;
	}

	::LockImageAccess();

	this->GetType(iImgType, iDataType, iBPP);
	rMask.ConvertType(iImgType, iDataType);
	rMask.SetSize(m_iWidth, m_iHeight);

	bool bRes = ExecutePixelOperator_EqualType<FMaskLogicANDImg, PPixCnt>(rMask, *this, rImage,
		PPixCnt(m_iWidth * m_iHeight));

	::UnlockImageAccess();
	return bRes;
}

/////////////////////////////////////////////////////////////////////
/// Returns mask of same dimension as image.
/// The mask is the result of a logic OR operation between the two
/// images.

bool COGLImage::MaskLogicOR(COGLImage& rMask, const COGLImage& rImage)
{
	int iImgType, iDataType, iBPP;
	int iWidth, iHeight;

	rImage.GetSize(iWidth, iHeight);
	if ((iWidth != m_iWidth) || (iHeight != m_iHeight))
	{
		return false;
	}

	::LockImageAccess();

	this->GetType(iImgType, iDataType, iBPP);
	rMask.ConvertType(iImgType, iDataType);
	rMask.SetSize(m_iWidth, m_iHeight);

	bool bRes = ExecutePixelOperator_EqualType<FMaskLogicORImg, PPixCnt>(rMask, *this, rImage,
		PPixCnt(m_iWidth * m_iHeight));

	::UnlockImageAccess();
	return bRes;
}

/////////////////////////////////////////////////////////////////////
/// Returns image of same dimension as image.
/// The image is the result of a bitwise AND operation between the two
/// images.

bool COGLImage::ImageBitAND(COGLImage& rMask, const COGLImage& rImage)
{
	int iImgType, iDataType, iBPP;
	int iWidth, iHeight;

	rImage.GetSize(iWidth, iHeight);
	if ((iWidth != m_iWidth) || (iHeight != m_iHeight))
	{
		return false;
	}

	::LockImageAccess();

	this->GetType(iImgType, iDataType, iBPP);
	rMask.ConvertType(iImgType, iDataType);
	rMask.SetSize(m_iWidth, m_iHeight);

	bool bRes = ExecutePixelOperator_EqualType<FMaskBitANDImg, PPixCnt>(rMask, *this, rImage,
		PPixCnt(m_iWidth * m_iHeight));

	::UnlockImageAccess();
	return bRes;
}

/////////////////////////////////////////////////////////////////////
/// Returns image of same dimension as image.
/// The image is the result of a bitwise OR operation between the two
/// images.

bool COGLImage::ImageBitOR(COGLImage& rMask, const COGLImage& rImage)
{
	int iImgType, iDataType, iBPP;
	int iWidth, iHeight;

	rImage.GetSize(iWidth, iHeight);
	if ((iWidth != m_iWidth) || (iHeight != m_iHeight))
	{
		return false;
	}

	::LockImageAccess();

	this->GetType(iImgType, iDataType, iBPP);
	rMask.ConvertType(iImgType, iDataType);
	rMask.SetSize(m_iWidth, m_iHeight);

	bool bRes = ExecutePixelOperator_EqualType<FMaskBitORImg, PPixCnt>(rMask, *this, rImage,
		PPixCnt(m_iWidth * m_iHeight));

	::UnlockImageAccess();
	return bRes;
}

/////////////////////////////////////////////////////////////////////
/// Returns image of same dimension as image.
/// The image is the result of the division of two images

bool COGLImage::OpDivImage(COGLImage& rResult, const COGLImage& rImage)
{
	int iImgType, iDataType, iBPP;
	int iWidth, iHeight;
	int iColorCnt1, iColorCnt2;
	int iAlphaCnt1, iAlphaCnt2;
	bool bRes;

	rImage.GetSize(iWidth, iHeight);
	if ((iWidth != m_iWidth) || (iHeight != m_iHeight))
	{
		return false;
	}

	::LockImageAccess();

	GetComponentCount(iColorCnt1, iAlphaCnt1);
	rImage.GetComponentCount(iColorCnt2, iAlphaCnt2);

	if (iColorCnt1 >= iColorCnt2)
	{
		GetType(iImgType, iDataType, iBPP);
		rResult.ConvertType(iImgType, iDataType);
		rResult.SetSize(m_iWidth, m_iHeight);

		bRes = ExecutePixelOperator_EqualDataType112<FOpDivImg, PPixCnt>(rResult, *this, rImage,
			PPixCnt(m_iWidth * m_iHeight));
	}
	else
	{
		rImage.GetType(iImgType, iDataType, iBPP);
		rResult.ConvertType(iImgType, iDataType);
		rResult.SetSize(m_iWidth, m_iHeight);

		bRes = ExecutePixelOperator_EqualDataType121<FOpDivImg, PPixCnt>(rResult, *this, rImage,
			PPixCnt(m_iWidth * m_iHeight));
	}

	::UnlockImageAccess();
	return bRes;
}

/////////////////////////////////////////////////////////////////////
/// Returns image of same dimension as image.
/// The image is the result of the product of two images

bool COGLImage::OpMultImage(COGLImage& rResult, const COGLImage& rImage)
{
	int iImgType, iDataType, iBPP;
	int iWidth, iHeight;
	int iColorCnt1, iColorCnt2;
	int iAlphaCnt1, iAlphaCnt2;
	bool bRes;

	rImage.GetSize(iWidth, iHeight);
	if ((iWidth != m_iWidth) || (iHeight != m_iHeight))
	{
		return false;
	}

	::LockImageAccess();

	GetComponentCount(iColorCnt1, iAlphaCnt1);
	rImage.GetComponentCount(iColorCnt2, iAlphaCnt2);

	if (iColorCnt1 >= iColorCnt2)
	{
		GetType(iImgType, iDataType, iBPP);
		rResult.ConvertType(iImgType, iDataType);
		rResult.SetSize(m_iWidth, m_iHeight);

		bRes = ExecutePixelOperator_EqualDataType112<FOpMultImg, PPixCnt>(rResult, *this, rImage,
			PPixCnt(m_iWidth * m_iHeight));
	}
	else
	{
		rImage.GetType(iImgType, iDataType, iBPP);
		rResult.ConvertType(iImgType, iDataType);
		rResult.SetSize(m_iWidth, m_iHeight);

		bRes = ExecutePixelOperator_EqualDataType121<FOpMultImg, PPixCnt>(rResult, *this, rImage,
			PPixCnt(m_iWidth * m_iHeight));
	}

	::UnlockImageAccess();
	return bRes;
}

/////////////////////////////////////////////////////////////////////
/// Returns image of same dimension as image.
/// The image is the result of the sum of two images

bool COGLImage::OpAddImage(COGLImage& rResult, const COGLImage& rImage)
{
	int iImgType, iDataType, iBPP;
	int iWidth, iHeight;
	int iColorCnt1, iColorCnt2;
	int iAlphaCnt1, iAlphaCnt2;
	bool bRes;

	rImage.GetSize(iWidth, iHeight);
	if ((iWidth != m_iWidth) || (iHeight != m_iHeight))
	{
		return false;
	}

	::LockImageAccess();

	GetComponentCount(iColorCnt1, iAlphaCnt1);
	rImage.GetComponentCount(iColorCnt2, iAlphaCnt2);

	if (iColorCnt1 >= iColorCnt2)
	{
		GetType(iImgType, iDataType, iBPP);
		rResult.ConvertType(iImgType, iDataType);
		rResult.SetSize(m_iWidth, m_iHeight);

		bRes = ExecutePixelOperator_EqualDataType112<FOpAddImg, PPixCnt>(rResult, *this, rImage,
			PPixCnt(m_iWidth * m_iHeight));
	}
	else
	{
		rImage.GetType(iImgType, iDataType, iBPP);
		rResult.ConvertType(iImgType, iDataType);
		rResult.SetSize(m_iWidth, m_iHeight);

		bRes = ExecutePixelOperator_EqualDataType121<FOpAddImg, PPixCnt>(rResult, *this, rImage,
			PPixCnt(m_iWidth * m_iHeight));
	}

	::UnlockImageAccess();
	return bRes;
}

/////////////////////////////////////////////////////////////////////
/// Returns image of same dimension as image.
/// The image is the result of the difference of two images

bool COGLImage::OpSubImage(COGLImage& rResult, const COGLImage& rImage)
{
	int iImgType, iDataType, iBPP;
	int iWidth, iHeight;
	int iColorCnt1, iColorCnt2;
	int iAlphaCnt1, iAlphaCnt2;
	bool bRes;

	rImage.GetSize(iWidth, iHeight);
	if ((iWidth != m_iWidth) || (iHeight != m_iHeight))
	{
		return false;
	}

	::LockImageAccess();

	GetComponentCount(iColorCnt1, iAlphaCnt1);
	rImage.GetComponentCount(iColorCnt2, iAlphaCnt2);

	if (iColorCnt1 >= iColorCnt2)
	{
		GetType(iImgType, iDataType, iBPP);
		rResult.ConvertType(iImgType, iDataType);
		rResult.SetSize(m_iWidth, m_iHeight);

		bRes = ExecutePixelOperator_EqualDataType112<FOpSubImg, PPixCnt>(rResult, *this, rImage,
			PPixCnt(m_iWidth * m_iHeight));
	}
	else
	{
		rImage.GetType(iImgType, iDataType, iBPP);
		rResult.ConvertType(iImgType, iDataType);
		rResult.SetSize(m_iWidth, m_iHeight);

		bRes = ExecutePixelOperator_EqualDataType121<FOpSubImg, PPixCnt>(rResult, *this, rImage,
			PPixCnt(m_iWidth * m_iHeight));
	}

	::UnlockImageAccess();
	return bRes;
}

/////////////////////////////////////////////////////////////////////
// Convert Image Type

bool COGLImage::ConvertType(int iImageType, int iDataType)
{
	if (!((iImageType == CLUVIZ_IMG_RGB) ||
		(iImageType == CLUVIZ_IMG_RGBA) ||
		(iImageType == CLUVIZ_IMG_BGRA) ||
		(iImageType == CLUVIZ_IMG_BGR) ||
		(iImageType == CLUVIZ_IMG_LUMINANCE) ||
		(iImageType == CLUVIZ_IMG_LUMINANCE_ALPHA)))
	{
		return false;
	}

	if (!((iDataType == CLUVIZ_IMG_BYTE) ||
		(iDataType == CLUVIZ_IMG_UNSIGNED_BYTE) ||
		(iDataType == CLUVIZ_IMG_SHORT) ||
		(iDataType == CLUVIZ_IMG_UNSIGNED_SHORT) ||
		(iDataType == CLUVIZ_IMG_INT) ||
		(iDataType == CLUVIZ_IMG_UNSIGNED_INT) ||
		(iDataType == CLUVIZ_IMG_FLOAT) ||
		(iDataType == CLUVIZ_IMG_DOUBLE)))
	{
		return false;
	}

	::LockImageAccess();

	ResetPixelIdxLum();

	ilBindImage(m_uImgID);

	bool bRes = (ilConvertImage(iImageType, iDataType) == IL_TRUE);

	::UnlockImageAccess();

	return bRes;
}


/////////////////////////////////////////////////////////////////////
/// Load Bitmap

bool COGLImage::LoadImage(bool bForce, int iNewImgType, int iNewDataType)
{
	// Check whether Bitmap has already been loaded
	if (!bForce && (m_csFilename == m_csCurFilename))
	{
		return true;
	}

	int iImgType, iDataType;

	::LockImageAccess();

	ResetPixelIdxLum();
	ilBindImage(m_uImgID);

	// Load the image
	if (ilLoadImage(m_csFilename.Str()) == IL_FALSE)
	{
		::UnlockImageAccess();
		return false;
	}

	if ((iNewImgType > 0) && (iNewDataType > 0))
	{
		if (ilConvertImage(iNewImgType, iNewDataType) == IL_FALSE)
		{
			::UnlockImageAccess();
			return false;
		}
	}
	else
	{
		iImgType = ilGetInteger(IL_IMAGE_FORMAT);
		iDataType = ilGetInteger(IL_IMAGE_TYPE);

		if (iImgType == IL_COLOR_INDEX)
		{
			// Convert the image to RGBA, uchar
			if (ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE) == IL_FALSE)
			{
				::UnlockImageAccess();
				return false;
			}
		}
	}

	m_iWidth = ilGetInteger(IL_IMAGE_WIDTH);
	m_iHeight = ilGetInteger(IL_IMAGE_HEIGHT);

	m_csCurFilename = m_csFilename;

	::UnlockImageAccess();

	return true;
}

/////////////////////////////////////////////////////////////////////
/// Save Bitmap

bool COGLImage::SaveImage(const char* pcFilename)
{
	char* pcName;
	CStrMem csFilename, csBaseName, csExt;
	int iExtPos;
	//bool bEPSFile = false;
	//bool bJPS = false;

	if (!pcFilename)
	{
		pcName = m_csFilename.Str();
	}
	else
	{
		pcName = (char*)pcFilename;
	}

	::LockImageAccess();

	ilBindImage(m_uImgID);

	csBaseName = csFilename = pcName;

	iExtPos = int('.' < csBaseName);
	csExt = csBaseName.Last(int(csFilename.Len() - iExtPos), 1);
	csBaseName |= -1;

	//if (csExt == "eps")
	//{
	//	csFilename = csBaseName + ".tmp";
	//	bEPSFile = true;
	//}
	//else if (csExt == "jps")
	//{
	//	csFilename = csBaseName + ".jpg";
	//	bEPSFile = true;
	//	bJPS = true;
	//}

	remove(csFilename.Str());

	//if (bEPSFile)
	//{
	//	if (ilSave(IL_JPG, (ILstring)csFilename.Str()) == IL_FALSE)
	//	{
	//		::UnlockImageAccess();
	//		return false;
	//	}
	//}
	//else
	//{
		if (ilSaveImage((ILstring)csFilename.Str()) == IL_FALSE)
		{
			::UnlockImageAccess();
			return false;
		}
	//}

	//if (bEPSFile)
	//{
	//	csBaseName += ".eps";

	//	if (!ConvertJPEGtoPS(csFilename.Str(), csBaseName.Str()))
	//	{
	//		::UnlockImageAccess();
	//		return false;
	//	}

	//	if (!bJPS)
	//	{
	//		remove(csFilename.Str());
	//	}
	//}

	::UnlockImageAccess();
	return true;
}

/////////////////////////////////////////////////////////////////////
/// Make part of the image transparent

bool COGLImage::MakeTransparent(ETransparencyMode eMode, COGLColor& rTransColor)
{
	::LockImageAccess();

	ResetPixelIdxLum();

	bool bRes = ExecutePixelOperator<FMakeTrans, PMakeTrans>(*this, PMakeTrans(m_iWidth * m_iHeight, rTransColor, eMode));

	::UnlockImageAccess();
	return bRes;
}

/////////////////////////////////////////////////////////////////////
/// Evaluate bounding box of image with respect to given background color

bool COGLImage::GetBoundingBox(Mem<int>& mBox, COGLColor& rBGColor)
{
	bool bRes = false;

	::LockImageAccess();

	PGetBoundBox rPar(m_iWidth, m_iHeight, rBGColor);

	bRes = ExecutePixelOperator<FGetBoundBox, PGetBoundBox>(*this, rPar);

	mBox.Set(4);
	memcpy(mBox.Data(), rPar.piBox, 4 * sizeof(int));

	::UnlockImageAccess();

	return bRes;
}

/////////////////////////////////////////////////////////////////////
Clu::CIImage COGLImage::ToIImage()
{
	Clu::CIImage xImage;

	if (m_iWidth <= 0 || m_iHeight <= 0)
	{
		return xImage;
	}


	::LockImageAccess();
	try
	{
		ilBindImage(m_uImgID);

		Clu::EDataType eDataType;
		Clu::EPixelType ePixelType;

		int iImgType = ilGetInteger(IL_IMAGE_FORMAT);
		int iDataType = ilGetInteger(IL_IMAGE_TYPE);

		switch (iImgType)
		{
		case IL_RGB:
			ePixelType = Clu::EPixelType::RGB;
			break;
		case IL_RGBA:
			ePixelType = Clu::EPixelType::RGBA;
			break;
		case IL_BGR:
			ePixelType = Clu::EPixelType::BGR;
			break;
		case IL_BGRA:
			ePixelType = Clu::EPixelType::BGRA;
			break;
		case IL_LUMINANCE:
			ePixelType = Clu::EPixelType::Lum;
			break;
		case IL_LUMINANCE_ALPHA:
			ePixelType = Clu::EPixelType::LumA;
			break;
		default:
			throw Clu::CIException("Unknown pixel type", __FILE__, __FUNCTION__, __LINE__);
		}

		switch (iDataType)
		{
		case IL_BYTE:
			eDataType = Clu::EDataType::Int8;
			break;
		case IL_UNSIGNED_BYTE:
			eDataType = Clu::EDataType::UInt8;
			break;
		case IL_SHORT:
			eDataType = Clu::EDataType::Int16;
			break;
		case IL_UNSIGNED_SHORT:
			eDataType = Clu::EDataType::UInt16;
			break;
		case IL_INT:
			eDataType = Clu::EDataType::Int32;
			break;
		case IL_UNSIGNED_INT:
			eDataType = Clu::EDataType::UInt32;
			break;
		case IL_FLOAT:
			eDataType = Clu::EDataType::Single;
			break;
		case IL_DOUBLE:
			eDataType = Clu::EDataType::Double;
			break;
		default:
			throw Clu::CIException("Unknown data type", __FILE__, __FUNCTION__, __LINE__);
		}

		xImage.Create(Clu::SImageFormat(m_iWidth, m_iHeight, ePixelType, eDataType), GetDataPtr(), true);
	}
	catch (Clu::CIException& xEx)
	{
		::UnlockImageAccess();
		throw Clu::CIException("Error loading image.", __FILE__, __FUNCTION__, __LINE__, std::move(xEx));
	}

	::UnlockImageAccess();
	return xImage;
}

/////////////////////////////////////////////////////////////////////
void COGLImage::Create(const Clu::CIImage& xImage)
{
	int iW, iH;

	if (!xImage.IsValid())
		throw Clu::CIException("Given image is invalid", __FILE__, __FUNCTION__, __LINE__);

	xImage.GetSize(iW, iH);

	int iILImgType, iILDataType;

	switch (xImage.PixelType())
	{
	case Clu::EPixelType::RGB:
		iILImgType = IL_RGB;
		break;
	case Clu::EPixelType::RGBA:
		iILImgType = IL_RGBA;
		break;
	case Clu::EPixelType::BGR:
		iILImgType = IL_BGR;
		break;
	case Clu::EPixelType::BGRA:
		iILImgType = IL_BGRA;
		break;
	case Clu::EPixelType::Lum:
		iILImgType = IL_LUMINANCE;
		break;
	case Clu::EPixelType::LumA:
		iILImgType = IL_LUMINANCE_ALPHA;
		break;
		// Interpret Bayer Pattern image as luminance image so we can see the raw pattern.
	case Clu::EPixelType::BayerBG:
	case Clu::EPixelType::BayerGB:
	case Clu::EPixelType::BayerGR:
	case Clu::EPixelType::BayerRG:
		iILImgType = IL_LUMINANCE;
		break;
	default:
		throw Clu::CIException("Unknown data type", __FILE__, __FUNCTION__, __LINE__);
	}

	switch (xImage.DataType())
	{
	case Clu::EDataType::Int8:
		iILDataType = IL_BYTE;
		break;
	case Clu::EDataType::UInt8:
		iILDataType = IL_UNSIGNED_BYTE;
		break;
	case Clu::EDataType::Int16:
		iILDataType = IL_SHORT;
		break;
	case Clu::EDataType::UInt16:
		iILDataType = IL_UNSIGNED_SHORT;
		break;
	case Clu::EDataType::Int32:
		iILDataType = IL_INT;
		break;
	case Clu::EDataType::UInt32:
		iILDataType = IL_UNSIGNED_INT;
		break;
	case Clu::EDataType::Single:
		iILDataType = IL_FLOAT;
		break;
	case Clu::EDataType::Double:
		iILDataType = IL_DOUBLE;
		break;
	default:
		throw Clu::CIException("Unknown pixel type", __FILE__, __FUNCTION__, __LINE__);
	}

	::LockImageAccess();
	try
	{
		if (!CopyImage((int)iW, (int)iH, iILImgType, iILDataType, xImage.DataPointer()))
		{
			throw Clu::CIException("Error copying image", __FILE__, __FUNCTION__, __LINE__);
		}
	}
	catch (Clu::CIException& xEx)
	{
		::UnlockImageAccess();
		throw Clu::CIException("Error creating image", __FILE__, __FUNCTION__, __LINE__, std::move(xEx));
	}
	::UnlockImageAccess();
}


/////////////////////////////////////////////////////////////////////
// Copy given data to this image, clearing current image

bool COGLImage::Create(int iWidth, int iHeight, int iImgType, int iDataType)
{
	::LockImageAccess();

	ResetPixelIdxLum();
	ilBindImage(m_uImgID);

	int _iImgType = ilGetInteger(IL_IMAGE_FORMAT);
	int _iDataType = ilGetInteger(IL_IMAGE_TYPE);

	if ((_iImgType != iImgType) || (_iDataType != iDataType))
	{
		// First convert to external type
		ilConvertImage(iImgType, iDataType);
	}

	if ((iWidth != m_iWidth) || (iHeight != m_iHeight))
	{
		// Create memory block
		SetSize(iWidth, iHeight);
		// Image ID may have changed
		ilBindImage(m_uImgID);
	}

	::UnlockImageAccess();
	return true;
}

/////////////////////////////////////////////////////////////////////
// Copy given data to this image, clearing current image

bool COGLImage::CopyImage(int iWidth, int iHeight, int iImgType, int iDataType, const void* pvData)
{
	::LockImageAccess();

	if (!Create(iWidth, iHeight, iImgType, iDataType))
	{
		return false;
	}

	ilBindImage(m_uImgID);

	// Get bytes per pixel value
	int iBytesPerPixel = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);

	// Copy the image
	memcpy(ilGetData(), pvData, iWidth * iHeight * iBytesPerPixel);

	::UnlockImageAccess();
	return true;
}
