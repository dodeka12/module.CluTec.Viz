////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      GDITextRenderer.h
//
// summary:   Declares the GDI text renderer class
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

#pragma once

#include "dwrite.h"

class GdiTextRenderer : public IDWriteTextRenderer
{
public:

	GdiTextRenderer(IDWriteBitmapRenderTarget* bitmapRenderTarget, IDWriteRenderingParams* renderingParams);
	~GdiTextRenderer();

	IFACEMETHOD(IsPixelSnappingDisabled)(
			__maybenull void* clientDrawingContext,
			__out BOOL * isDisabled
			);

	IFACEMETHOD(GetCurrentTransform)(
			__maybenull void* clientDrawingContext,
			__out DWRITE_MATRIX * transform
			);

	IFACEMETHOD(GetPixelsPerDip)(
			__maybenull void* clientDrawingContext,
			__out FLOAT * pixelsPerDip
			);

	IFACEMETHOD(DrawGlyphRun)(
			__maybenull void* clientDrawingContext,
			FLOAT baselineOriginX,
			FLOAT baselineOriginY,
			DWRITE_MEASURING_MODE measuringMode,
			__in DWRITE_GLYPH_RUN const* glyphRun,
			__in DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
			IUnknown * clientDrawingEffect
			);

	IFACEMETHOD(DrawUnderline)(
			__maybenull void* clientDrawingContext,
			FLOAT baselineOriginX,
			FLOAT baselineOriginY,
			__in DWRITE_UNDERLINE const* underline,
			IUnknown * clientDrawingEffect
			);

	IFACEMETHOD(DrawStrikethrough)(
			__maybenull void* clientDrawingContext,
			FLOAT baselineOriginX,
			FLOAT baselineOriginY,
			__in DWRITE_STRIKETHROUGH const* strikethrough,
			IUnknown * clientDrawingEffect
			);

	IFACEMETHOD(DrawInlineObject)(
			__maybenull void* clientDrawingContext,
			FLOAT originX,
			FLOAT originY,
			IDWriteInlineObject * inlineObject,
			BOOL isSideways,
			BOOL isRightToLeft,
			IUnknown * clientDrawingEffect
			);

public:

	IFACEMETHOD_(unsigned long, AddRef) ();
	IFACEMETHOD_(unsigned long, Release) ();
	IFACEMETHOD(QueryInterface)(
			IID const & riid,
			void** ppvObject
			);

	void SetTextColor(int r, int g, int b) { _textColor = RGB(r, g, b); }
	COLORREF GetTextColor() const { return _textColor; }

	unsigned char* bytes;

private:

	unsigned long _refCount;
	COLORREF _textColor;
	IDWriteBitmapRenderTarget* _renderTarget;
	IDWriteRenderingParams* _renderingParams;
};
