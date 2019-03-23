////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLFrame.h
//
// summary:   Declares the ogl frame class
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

// OGLFrame.h: Schnittstelle f�r die Klasse COGLFrame.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLFRAME_H__INCLUDED_)
#define AFX_OGLFRAME_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLBaseElement.h"
#include "CluTec.Viz.Base\TensorData.h"
#include "CluTec.Viz.Base\clu.h"

class CLUDRAW_API COGLFrame  : public COGLBaseElement
{
public:
	enum EFrameMode
	{
		NONE = 0,
		MODELVIEW,
		PROJECTION,
		TEXTURE
	};

public:
	COGLFrame();
	COGLFrame( const Matrix<double>& rMatrix );
	COGLFrame(const COGLFrame& rTrans);

	virtual COGLBaseElement* Copy()
	{
		return (COGLBaseElement *) new COGLFrame( *this );
	}

	COGLFrame& operator= (const COGLFrame& rTrans);

	void EnableMultiplyMatrix( bool bVal = true )
	{ m_bMultiplyMatrix = bVal; }

	// Set matrix to unit matrix
	void Reset();

	// Reset frame content
	void ResetContent();

	// Set identity matrix
	void SetIdentity();

	// Set 4x4 matrix. The given matrix is transposed before it is passed to OpenGL
	void Set( const Matrix<double>& rMatrix );

	// Set 4x4 matrix. The given matrix is transposed before it is passed to OpenGL
	void Set( const CTensorData<double>& rTensor );

	// Multiply 4x4 matrix with frame from right. The given matrix is transposed before it is passed to OpenGL
	void Multiply( const Matrix<double>& rMatrix );

	// Multiply 4x4 tensor with frame from right. The given matrix is transposed before it is passed to OpenGL
	void Multiply( const CTensorData<double>& rTensor );

	// Multiply MV operator with frame from right.
	void Multiply( const MultiV<double>& rMV );

	// Rotate Frame
	void Rotate( double dAngle, double dX, double dY, double dZ, bool bMultiply = true );

	// Reflect Frame
	void Reflect( double dX, double dY, double dZ, bool bMultiply = true  );

	// Translate Frame
	void Translate( double dX, double dY, double dZ, bool bMultiply = true  );

	// Scale Frame
	void Scale( double dX, double dY, double dZ, bool bMultiply = true  );

	// Pixel Zoom
	void PixelZoom( double dX, double dY )
	{ m_dPixelZoomX = dX; m_dPixelZoomY = dY; }

	// Apply scaling such that dWidth, dHeight in local coordinates represent 
	// size dPixelWidth, dPixelHeight in Pixel coordinates.
	void AutoScaleToPixelSize( double dWidth, double dHeight, double dPixelWidth, double dPixelHeight,
								bool bAllowScaleUp = true, bool bAllowScaleDown = true )
	{
		m_bAutoScaleToPixelSize = true;
		m_bAllowTextureScaleUp = bAllowScaleUp;
		m_bAllowTextureScaleDown = bAllowScaleDown;
		m_dASWidth = dWidth; m_dASHeight = dHeight;
		m_dASPWidth = dPixelWidth; m_dASPHeight = dPixelHeight;
	}

	// Enable Auto Billboard
	void EnableAutoBillboard( bool bVal = true )
	{ m_bAutoBillboard = bVal; }

	// Enable use of origin
	void EnableOrigin( bool bVal = true )
	{ m_bUseOrigin = bVal; }

	// Set origin
	void SetOrigin( double dX, double dY, double dZ )
	{ m_dOrigX = dX; m_dOrigY = dY; m_dOrigZ = dZ; }

	// Set Animated frame
	void SetAnimateFrame( const COGLBEReference &refAnim )
	{ m_refAnimateFrame = refAnim; }

	void SetFrameMode( EFrameMode eFrameMode )
	{ m_eFrameMode = eFrameMode; }

	// Get 4x4 Matrix
	Matrix<double> Get()
	{ return ~m_matFrame; }

	// Get Data Pointer
	double* GetDataPtr()
	{ return m_matFrame.Data(); }

	bool Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData &rData);

protected:
	void CheckFrameFrontFace();
	void TellParentContentChanged();

protected:
	// Transformation matrix, projection and modelview
	Matrix<double> m_matFrame;

	// Multiply (true) or set (false) matrix
	bool m_bMultiplyMatrix;

	// Frame necessitates an inversion of the front face
	bool m_bInvertFrontFace;

	// Use origin
	bool m_bUseOrigin;

	// Origin about which frame transformation is applied
	double m_dOrigX, m_dOrigY, m_dOrigZ;

	// Pixel Zoom Values
	double m_dPixelZoomX, m_dPixelZoomY;

	// Do Auto Scale To Pixel Size
	bool m_bAutoScaleToPixelSize;

	// Allow texture scale up. Used in AutoScaleToPixelSize
	bool m_bAllowTextureScaleUp;

	// Allow texture scale down. Used in AutoScaleToPixelSize
	bool m_bAllowTextureScaleDown;

	// Auto rotate frame so that x-y-plane is always parallel to screen
	bool m_bAutoBillboard;

	// Auto Scale To Pixel Values
	double m_dASWidth, m_dASHeight;
	double m_dASPWidth, m_dASPHeight;

	// Frame Mode
	EFrameMode m_eFrameMode;

	// BE Reference to Animated Frame
	COGLBEReference m_refAnimateFrame;
};

#endif // !defined(AFX_OGLMATERIAL_H__2296DD19_C205_454A_82DB_D1881903B593__INCLUDED_)
