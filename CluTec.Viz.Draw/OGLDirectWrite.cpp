////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLDirectWrite.cpp
//
// summary:   Implements the ogl direct write class
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

#include "stdafx.h"

#include "OGLDirectWrite.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SFontParameter::operator!=(const SFontParameter& xParamA)
{
	return !operator==(xParamA);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The font parameters are considered equal if all data fields considering the IDWriteTextFormat struct are equal
// The color parameter for example is NOT used by the IDWriteTextFormat struct and therefore NOT used to evaluate the equality
bool SFontParameter::operator==(const SFontParameter& xParamA)
{
	bool bIsEqual = true;

	if (fFontSize != xParamA.fFontSize)
	{
		bIsEqual = false;
	}

	if (xFontWeight != xParamA.xFontWeight)
	{
		bIsEqual = false;
	}

	if (xFontStyle != xParamA.xFontStyle)
	{
		bIsEqual = false;
	}

	if (xFontStretch != xParamA.xFontStretch)
	{
		bIsEqual = false;
	}

	if (xFontAlignment != xParamA.xFontAlignment)
	{
		bIsEqual = false;
	}

	if (strcmp(sFontName.Str(), xParamA.sFontName.Str()) != 0)
	{
		bIsEqual = false;
	}

	return bIsEqual;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
OGLDirectWrite::OGLDirectWrite()
{
	m_pxDWriteFactory      = nullptr;
	m_pxGDITextRenderer    = nullptr;
	m_pxTextFormat         = nullptr;
	m_pxBitmapRenderTarget = nullptr;
	m_pxRenderingParams    = nullptr;
	m_pxGDIInterop         = nullptr;

	m_HDCScreen = nullptr;

	// We use a fixed FullHD Resolution for our offscreen buffer
	m_iRenderTargetW = 1920;
	m_iRenderTargetH = 1080;

	SetDefaultFontParameter();
	m_xPreviousFontParameter = m_xCurrentFontParameter;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OGLDirectWrite::SetDefaultFontParameter()
{
	m_xCurrentFontParameter.fFontSize      = 24.0f;
	m_xCurrentFontParameter.fBaseAlpha     = 1.0;
	m_xCurrentFontParameter.sFontName      = "Arial";
	m_xCurrentFontParameter.xFontAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;
	m_xCurrentFontParameter.xFontStretch   = DWRITE_FONT_STRETCH_NORMAL;
	m_xCurrentFontParameter.xFontStyle     = DWRITE_FONT_STYLE_NORMAL;
	m_xCurrentFontParameter.xFontWeight    = DWRITE_FONT_WEIGHT_REGULAR;

	m_xCurrentFontParameter.iWordWrapBoxWidthPx = m_iRenderTargetW;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OGLDirectWrite::Init()
{
	// Get the default DC from the currently used window
	m_HDCScreen = GetDC(NULL);

	// Create a DirectWrite factory. We do not want to interop with Direct2D, therefore we use the ISOLATED mode for the
	HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_ISOLATED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&m_pxDWriteFactory));

	if (!SUCCEEDED(hr))
	{
		return false;
	}

	// Create a GDI interop interface.
	hr = m_pxDWriteFactory->GetGdiInterop(&m_pxGDIInterop);

	if (!SUCCEEDED(hr))
	{
		return false;
	}

	// Create a render target with a fixed full HD resolution. No rendered text can exceed the boundaries of the off screen buffer.
	hr = m_pxGDIInterop->CreateBitmapRenderTarget(m_HDCScreen, m_iRenderTargetW, m_iRenderTargetH, &m_pxBitmapRenderTarget);
	if (!SUCCEEDED(hr))
	{
		return false;
	}

	// Create the default rendering parameter
	hr = m_pxDWriteFactory->CreateRenderingParams(&m_pxRenderingParams);
	if (!SUCCEEDED(hr))
	{
		return false;
	}

	// Use the Bitmap render target and the default rendering params to create a custom text renderer. The custom renderer will forward
	// the DrawGlyphRun calls to the bitmap renderer
	m_pxGDITextRenderer = new GdiTextRenderer(m_pxBitmapRenderTarget, m_pxRenderingParams);

	// Apply the default font parameter
	ApplyTextFormat(m_xCurrentFontParameter);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OGLDirectWrite::RenderTextImage(CImageReference& xImage, CStrMem& sText, SFontParameter xFontParameter)
{
	// Create a textFormat using the font parameter
	SetFontParameter(xFontParameter);

	return RenderTextImage(xImage, sText);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OGLDirectWrite::RenderTextImage(CImageReference& xImage, CStrMem& sText)
{
	// Check if the font parameter has changed, since last font render pass
	if (m_xPreviousFontParameter != m_xCurrentFontParameter)
	{
		ApplyTextFormat(m_xCurrentFontParameter);
	}

	m_xPreviousFontParameter = m_xCurrentFontParameter;

	// Set the font color
	m_pxGDITextRenderer->SetTextColor(m_xCurrentFontParameter.ucFontColor[0], m_xCurrentFontParameter.ucFontColor[1], m_xCurrentFontParameter.ucFontColor[2]);

	// We need to convert our string to a wchar_t
	wchar_t* sWCharText = toWChar(sText);
	UINT32 uTextLen     = (UINT32) wcslen(sWCharText);

	// Create a TextLayout, using the given string and the TextFormat
	HRESULT hr = m_pxDWriteFactory->CreateTextLayout(
			sWCharText,	// The string to be laid out and formatted.
			uTextLen,	// The length of the string.
			m_pxTextFormat,	// The text format to apply to the string (contains font information, etc).
			(float) m_xCurrentFontParameter.iWordWrapBoxWidthPx,	// The width of the layout box.
			(float) m_iRenderTargetH,	// The height of the layout box.
			&m_pTextLayout	// The IDWriteTextLayout interface pointer.
			);

	delete sWCharText;

	if (!SUCCEEDED(hr))
	{
		return false;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Render the text via the custom text renderer. The renderer will draw the into a offscreen buffer
	m_pTextLayout->Draw(NULL, m_pxGDITextRenderer, 0, 0);

	// Retrieve the text metrics (Origin and width/height in Pixel coordinates of the offscreen buffer)
	DWRITE_TEXT_METRICS metrics;
	DWRITE_OVERHANG_METRICS overhangmetrics;
	m_pTextLayout->GetMetrics(&metrics);
	m_pTextLayout->GetOverhangMetrics(&overhangmetrics);

	//int iResultW = (int)(metrics.layoutWidth + overhangmetrics.left + overhangmetrics.right) + 4;
	// The initial way of calculating the width seems to be better.
	// In Addition we add a small boundary of 4 pixel to the width and height
	int iResultW     = (int) (metrics.width) + 4;
	int iResultH     = (int) (metrics.height) + 4;
	int iResultOrigX = (int) metrics.left;
	int iResultOrigY = (int) metrics.top;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Drawing to render target is complete, we can now bitBlit the result into a regular Device dependent bitmap

	// Get the memory of the off screen buffer
	HDC memoryHdc = m_pxBitmapRenderTarget->GetMemoryDC();

	// Create a device context for the bitmap
	HDC hdcBmp = CreateCompatibleDC(memoryHdc);

	// Create a bitmap and attach it to the device context we created above...
	HBITMAP bmp = CreateCompatibleBitmap(m_HDCScreen, iResultW, iResultH);
	SelectObject(hdcBmp, bmp);

	// And blit the off screen data with the text metrics to the bitmap
	BitBlt(hdcBmp, 0, 0, iResultW, iResultH, memoryHdc, iResultOrigX, iResultOrigY, SRCCOPY | NOMIRRORBITMAP);

	// Blit a black rectangle back into the memory DC to erase the content of the off screen buffer
	//BitBlt(memoryHdc, iResultOrigX, iResultOrigY, iResultW, iResultH, 0, 0, 0, BLACKNESS);
	BitBlt(memoryHdc, 0, 0, m_iRenderTargetW, m_iRenderTargetH, 0, 0, 0, BLACKNESS);

	// Now copy the bitmap into a CluViz compatible image format
	CopyBitmapToImage(hdcBmp, bmp, xImage, m_xCurrentFontParameter.fBaseAlpha);

	// Clean up
	DeleteDC(hdcBmp);
	DeleteObject(bmp);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OGLDirectWrite::CopyBitmapToImage(HDC xHDCBitmap, HBITMAP xHBmp, CImageReference& xImage, float fBaseAlpha)
{
	// Create a new Bitmap info struct and initialize all values with 0
	BITMAPINFO MyBMInfo = { 0 };
	MyBMInfo.bmiHeader.biSize = sizeof(MyBMInfo.bmiHeader);

	// Get the BITMAPINFO structure from the bitmap
	if (0 == GetDIBits(xHDCBitmap, xHBmp, 0, 0, NULL, &MyBMInfo, DIB_RGB_COLORS))
	{
		return false;
	}

	// Set the bitmap info to have a 32 bit RGB(A) bitmap. The alpha channel will be zero for all pixel.
	MyBMInfo.bmiHeader.biBitCount    = 32;
	MyBMInfo.bmiHeader.biCompression = BI_RGB;			// use interleaved RGB values
	MyBMInfo.bmiHeader.biHeight      = abs(MyBMInfo.bmiHeader.biHeight);

	// Allocate some memory to retrieve the bitmap data
	BYTE* lpPixels = new BYTE[MyBMInfo.bmiHeader.biSizeImage];

	// Call GetDIBits a second time, this time to (format and) store the actual
	// bitmap data (the "pixels") in the buffer lpPixels
	if (0 == GetDIBits(xHDCBitmap, xHBmp, 0, MyBMInfo.bmiHeader.biHeight,
			    lpPixels, &MyBMInfo, DIB_RGB_COLORS))
	{
		return false;
	}

	// Now we do some post processing on the retrieved bitmap
	// - Change the Font color to solid White (eliminate color speckles from ClearType rendering)
	// - Flip the image in y direction to compensate Left and RightHand Coordinate systems used
	// - calculate a fitting alpha value based on the original RGB values

	int iResultW = MyBMInfo.bmiHeader.biWidth;
	int iResultH = MyBMInfo.bmiHeader.biHeight;

	// Allocate memory for the resulting image
	unsigned char* pDataRGBA = new unsigned char[4 * iResultW * iResultH];

	// Now iterate over all pixel
	for (int i = 0; i < iResultW * iResultH; i++)
	{
		// extract x/y coordinates from index
		int iX = i % iResultW;
		int iY = i / iResultW;

		// Flip y
		iY = iResultH - iY - 1;

		// Generate a new index for the source
		int idxSource = iY * iResultW + iX;

		unsigned char r, g, b, a;
		// Native format is BGR
		b = lpPixels[idxSource * 4 + 0];
		g = lpPixels[idxSource * 4 + 1];
		r = lpPixels[idxSource * 4 + 2];

		// New alpha value is the average of the rgb-values
		float fAlpha = (float(r) + float(g) + float(b)) / 3.0f;

		// Clamp and cast the new value
		if (fAlpha > 255.0f)
		{
			a = 255;
		}
		else
		{
			a = (unsigned char) floor(fAlpha);
		}

		// And write the result
		pDataRGBA[i * 4 + 0] = m_xCurrentFontParameter.ucFontColor[0];
		pDataRGBA[i * 4 + 1] = m_xCurrentFontParameter.ucFontColor[1];
		pDataRGBA[i * 4 + 2] = m_xCurrentFontParameter.ucFontColor[2];
		pDataRGBA[i * 4 + 3] = unsigned char((float) a * fBaseAlpha);
	}

	xImage->SetSize(iResultW, iResultH);
	xImage->CopyImage(iResultW, iResultH, IL_RGBA, IL_UNSIGNED_BYTE, pDataRGBA);

	delete lpPixels;
	delete pDataRGBA;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OGLDirectWrite::ApplyTextFormat(const SFontParameter& xFontParameter)
{
	// Cast the String to a wchar_t pointer
	wchar_t* sWCharFontName = toWChar(xFontParameter.sFontName);

	// Create the new text format using the FontParameter struct
	HRESULT hr = m_pxDWriteFactory->CreateTextFormat(
			sWCharFontName,			// Font family name.
			NULL,			// Font collection (NULL sets it to use the system font collection).
			xFontParameter.xFontWeight,
			xFontParameter.xFontStyle,
			xFontParameter.xFontStretch,
			xFontParameter.fFontSize,
			L"en-us",
			&m_pxTextFormat
			);

	// Align the text (center, left, right)
	if (SUCCEEDED(hr))
	{
		hr = m_pxTextFormat->SetTextAlignment(xFontParameter.xFontAlignment);
	}

	// Always align the text to the top of the off screen buffer
	if (SUCCEEDED(hr))
	{
		hr = m_pxTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	}

	delete sWCharFontName;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
wchar_t* OGLDirectWrite::toWChar(const CStrMem& sString)
{
	size_t nTextLen = sString.Len();
	// reserve an additional character for the terminating 0s
	wchar_t* wcharStr = new wchar_t[nTextLen + 1];

	// We should maybe use mbstowcs_s to avoid compiler warnings
	#pragma warning( disable : 4996)
	mbstowcs(wcharStr, sString.Str(), nTextLen);

	// Add a string terminating 0
	wcharStr[nTextLen] = 0;

	return wcharStr;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OGLDirectWrite::Destroy()
{
	delete m_pxTextFormat;
	delete m_pxRenderingParams;
	delete m_pxDWriteFactory;
	delete m_pxBitmapRenderTarget;
	delete m_pxGDITextRenderer;
	delete m_pTextLayout;
	delete m_pxGDIInterop;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OGLDirectWrite::SetFontParameter(SFontParameter& xFontParameter)
{
	m_xCurrentFontParameter = xFontParameter;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SFontParameter OGLDirectWrite::GetFontParameter()
{
	return m_xCurrentFontParameter;
}
