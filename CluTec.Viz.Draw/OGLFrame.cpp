////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLFrame.cpp
//
// summary:   Implements the ogl frame class
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

// OGLFrame.cpp: Implementierung der Klasse COGLFrame.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLFrame.h"
#include "OGLBaseElementList.h"

#include "CluTec.Math\Matrix.h"
#include "CluTec.Viz.Fltk\Fl_math.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLFrame::COGLFrame()
{
	m_sTypeName = "Frame";

	Reset();
}

COGLFrame::COGLFrame(const Matrix<double>& rMatrix)
{
	m_sTypeName = "Frame";

	m_bMultiplyMatrix = false;
	Set(rMatrix);
	m_bAutoScaleToPixelSize = false;
}

COGLFrame::COGLFrame(const COGLFrame& rTrans)
{
	m_sTypeName = "Frame";

	*this = rTrans;
}

COGLFrame& COGLFrame::operator=(const COGLFrame& rTrans)
{
	COGLBaseElement::operator=(rTrans);

	m_bAutoScaleToPixelSize = rTrans.m_bAutoScaleToPixelSize;
	m_bAutoBillboard        = rTrans.m_bAutoBillboard;

	m_bAllowTextureScaleUp   = rTrans.m_bAllowTextureScaleUp;
	m_bAllowTextureScaleDown = rTrans.m_bAllowTextureScaleDown;

	m_bMultiplyMatrix  = rTrans.m_bMultiplyMatrix;
	m_bInvertFrontFace = rTrans.m_bInvertFrontFace;

	m_dASWidth  = rTrans.m_dASWidth;
	m_dASHeight = rTrans.m_dASHeight;

	m_dASPWidth  = rTrans.m_dASPWidth;
	m_dASPHeight = rTrans.m_dASPHeight;

	m_matFrame   = rTrans.m_matFrame;
	m_eFrameMode = rTrans.m_eFrameMode;

	m_dPixelZoomX = rTrans.m_dPixelZoomX;
	m_dPixelZoomY = rTrans.m_dPixelZoomY;

	m_bUseOrigin = rTrans.m_bUseOrigin;
	m_dOrigX     = rTrans.m_dOrigX;
	m_dOrigY     = rTrans.m_dOrigY;
	m_dOrigZ     = rTrans.m_dOrigZ;

	m_refAnimateFrame = rTrans.m_refAnimateFrame;

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Reset

void COGLFrame::Reset()
{
	ResetContent();

	m_bMultiplyMatrix = true;
	m_eFrameMode      = MODELVIEW;
}

//////////////////////////////////////////////////////////////////////
/// Reset Content

void COGLFrame::ResetContent()
{
	m_bInvertFrontFace = false;

	m_matFrame.Resize(4, 4);
	m_matFrame.Identity();

	m_dPixelZoomX = 1.0;
	m_dPixelZoomY = 1.0;

	m_bAutoScaleToPixelSize = false;
	m_bAutoBillboard        = false;

	m_bAllowTextureScaleUp   = true;
	m_bAllowTextureScaleDown = true;

	m_dASWidth  = m_dASHeight = 0.0;
	m_dASPWidth = m_dASPHeight = 0.0;

	m_bUseOrigin = false;
	m_dOrigX     = m_dOrigY = m_dOrigZ = 0.0;

	m_refAnimateFrame.Clear();
}

//////////////////////////////////////////////////////////////////////
/// Check whether frame necessitates an inversion of the front face

void COGLFrame::CheckFrameFrontFace()
{
	// Check whether frame has negative determinant.
	// If yes, have to use inverted front face.
	//glGetDoublev( GL_MODELVIEW_MATRIX, mFrame.Data() );
	Matrix<double> mFrame = m_matFrame;
	mFrame.Resize(3, 3);
	if (mFrame.Det() < 0)
	{
		m_bInvertFrontFace = true;
	}
	else
	{
		m_bInvertFrontFace = false;
	}
}

//////////////////////////////////////////////////////////////////////
/// Set Id Frame

void COGLFrame::SetIdentity()
{
	m_matFrame.Identity();
	CheckFrameFrontFace();
	TellParentContentChanged();
}

//////////////////////////////////////////////////////////////////////
/// Set Frame

void COGLFrame::Set(const Matrix<double>& rMatrix)
{
	if ((rMatrix.Rows() == 4) && (rMatrix.Cols() == 4))
	{
		// Map to column ordering of OpenGL
		m_matFrame = ~rMatrix;
		CheckFrameFrontFace();
		TellParentContentChanged();
	}
}

//////////////////////////////////////////////////////////////////////
/// Set Frame

void COGLFrame::Set(const CTensorData<double>& rTensor)
{
	if (rTensor.Valence() != 2)
	{
		return;
	}

	if ((rTensor.DimSize(0) != 4) || (rTensor.DimSize(1) != 4))
	{
		return;
	}

	m_matFrame = rTensor.Data();

	// Map to column ordering of OpenGL
	m_matFrame = ~m_matFrame;
	CheckFrameFrontFace();
	TellParentContentChanged();
}

//////////////////////////////////////////////////////////////////////
/// Multiply Matrix with Frame

void COGLFrame::Multiply(const Matrix<double>& rMatrix)
{
	if ((rMatrix.Rows() == 4) && (rMatrix.Cols() == 4))
	{
		// Map to column ordering of OpenGL
		m_matFrame = ~rMatrix * m_matFrame;
		CheckFrameFrontFace();
		TellParentContentChanged();
	}
}

//////////////////////////////////////////////////////////////////////
/// Multiply Tensor with Frame

void COGLFrame::Multiply(const CTensorData<double>& rTensor)
{
	if (rTensor.Valence() != 2)
	{
		return;
	}

	if ((rTensor.DimSize(0) != 4) || (rTensor.DimSize(1) != 4))
	{
		return;
	}

	Matrix<double> matA(4, 4);

	matA = rTensor.Data();

	// Map to column ordering of OpenGL
	m_matFrame = ~matA * m_matFrame;
	CheckFrameFrontFace();
	TellParentContentChanged();
}

//////////////////////////////////////////////////////////////////////
// Multiply MV operator with frame from right.

void COGLFrame::Multiply(const MultiV<double>& rMV)
{
	if (!rMV.BasePtr())
	{
		return;
	}

	uint uBaseID = rMV.BasePtr()->BaseID();

	if (uBaseID == ID_CONFGA)
	{
		// Conformal transformation operator
		// Transform homogeneous basis with given operator
		MultiV<double> pvA[4], vB, rMVRev;
		Matrix<double> matT(4, 4);

		rMVRev = ~rMV;

		pvA[0] = ConfGA<double>::vE1 ^ ConfGA<double>::vE;
		pvA[1] = ConfGA<double>::vE2 ^ ConfGA<double>::vE;
		pvA[2] = ConfGA<double>::vE3 ^ ConfGA<double>::vE;
		pvA[3] = ConfGA<double>::vE0 ^ ConfGA<double>::vE;

		for (int i = 0; i < 4; ++i)
		{
			vB = ((rMV & pvA[i]) & rMVRev);

			matT(i + 1, 1) = (vB[ConfGA < double > ::iBp1] + vB[ConfGA < double > ::iBm1]) / 2.0;
			matT(i + 1, 2) = (vB[ConfGA < double > ::iBp2] + vB[ConfGA < double > ::iBm2]) / 2.0;
			matT(i + 1, 3) = (vB[ConfGA < double > ::iBp3] + vB[ConfGA < double > ::iBm3]) / 2.0;
			matT(i + 1, 4) = -vB[ConfGA < double > ::iBpm];
		}

		m_matFrame = matT * m_matFrame;
	}
	else if (uBaseID == ID_E3GA)
	{
		// Euclidean transformation operator
		// Transform Euclidean basis with given operator
		MultiV<double> pvA[3], vB, rMVRev;
		Matrix<double> matT(4, 4);

		rMVRev = ~rMV;

		pvA[0] = E3GA<double>::vE1;
		pvA[1] = E3GA<double>::vE2;
		pvA[2] = E3GA<double>::vE3;

		for (int i = 0; i < 3; ++i)
		{
			vB = ((rMV & pvA[i]) & rMVRev);

			matT(i + 1, 1) = vB[E3GA < double > ::iE1];
			matT(i + 1, 2) = vB[E3GA < double > ::iE2];
			matT(i + 1, 3) = vB[E3GA < double > ::iE3];
			matT(i + 1, 4) = 0;
		}

		matT(4, 1) = 0;
		matT(4, 2) = 0;
		matT(4, 3) = 0;
		matT(4, 4) = 1.0;

		m_matFrame = matT * m_matFrame;
	}
	else if (uBaseID == ID_PGA)
	{
		// Euclidean transformation operator
		// Transform Euclidean basis with given operator
		MultiV<double> pvA[4], vB, rMVRev;
		Matrix<double> matT(4, 4);

		rMVRev = ~rMV;

		pvA[0] = PGA<double>::vE1;
		pvA[1] = PGA<double>::vE2;
		pvA[2] = PGA<double>::vE3;
		pvA[3] = PGA<double>::vE4;

		for (int i = 0; i < 4; ++i)
		{
			vB = ((rMV & pvA[i]) & rMVRev);

			matT(i + 1, 1) = vB[PGA < double > ::iE1];
			matT(i + 1, 2) = vB[PGA < double > ::iE2];
			matT(i + 1, 3) = vB[PGA < double > ::iE3];
			matT(i + 1, 4) = vB[PGA < double > ::iE4];
		}

		m_matFrame = matT * m_matFrame;
	}

	CheckFrameFrontFace();
	TellParentContentChanged();
}

//////////////////////////////////////////////////////////////////////
// Rotate Frame

void COGLFrame::Rotate(double dAngle, double dX, double dY, double dZ, bool bMultiply)
{
	double dSin, dCos, dNorm;
	double dS1, dS2, dS3;
	double dCS1, dCS2, dCS3, dS12, dS23, dS13;
	double dQC, dQS1, dQS2, dQS3;
	Matrix<double> matRot(4, 4);
	double* pData = matRot.Data();

	dSin  = sin(-dAngle / 2.0);
	dCos  = cos(dAngle / 2.0);
	dNorm = sqrt(dX * dX + dY * dY + dZ * dZ);

	if (dNorm < 1e-10)
	{
		return;
	}

	dS1 = dSin * dX / dNorm;
	dS2 = dSin * dY / dNorm;
	dS3 = dSin * dZ / dNorm;

	dCS1 = dCos * dS1;
	dCS2 = dCos * dS2;
	dCS3 = dCos * dS3;
	dS12 = dS1 * dS2;
	dS13 = dS1 * dS3;
	dS23 = dS2 * dS3;

	dQC  = dCos * dCos;
	dQS1 = dS1 * dS1;
	dQS2 = dS2 * dS2;
	dQS3 = dS3 * dS3;

	pData[0] = dQC + dQS1 - dQS2 - dQS3;
	pData[1] = 2.0 * (dS12 - dCS3);
	pData[2] = 2.0 * (dS13 + dCS2);
	pData[3] = 0.0;

	pData[4] = 2.0 * (dS12 + dCS3);
	pData[5] = dQC - dQS1 + dQS2 - dQS3;
	pData[6] = 2.0 * (dS23 - dCS1);
	pData[7] = 0.0;

	pData[8]  = 2.0 * (dS13 - dCS2);
	pData[9]  = 2.0 * (dS23 + dCS1);
	pData[10] = dQC - dQS1 - dQS2 + dQS3;
	pData[11] = 0.0;

	pData[12] = 0.0;
	pData[13] = 0.0;
	pData[14] = 0.0;
	pData[15] = 1.0;

	if (bMultiply)
	{
		m_matFrame = matRot * m_matFrame;
	}
	else
	{
		m_matFrame = matRot;
	}

	TellParentContentChanged();
}

//////////////////////////////////////////////////////////////////////
// Reflect Frame

void COGLFrame::Reflect(double dX, double dY, double dZ, bool bMultiply)
{
	double dNorm;
	double dN1, dN2, dN3;
	double dN12, dN13, dN23;
	double dQN1, dQN2, dQN3;

	Matrix<double> matRef(4, 4);
	double* pData = matRef.Data();

	dNorm = sqrt(dX * dX + dY * dY + dZ * dZ);

	if (dNorm < 1e-10)
	{
		return;
	}

	dN1 = dX / dNorm;
	dN2 = dY / dNorm;
	dN3 = dZ / dNorm;

	dN12 = -2.0 * dN1 * dN2;
	dN13 = -2.0 * dN1 * dN3;
	dN23 = -2.0 * dN2 * dN3;

	dQN1 = dN1 * dN1;
	dQN2 = dN2 * dN2;
	dQN3 = dN3 * dN3;

	pData[0] = -dQN1 + dQN2 + dQN3;
	pData[1] = dN12;
	pData[2] = dN13;
	pData[3] = 0.0;

	pData[4] = dN12;
	pData[5] = dQN1 - dQN2 + dQN3;
	pData[6] = dN23;
	pData[7] = 0.0;

	pData[8]  = dN13;
	pData[9]  = dN23;
	pData[10] = dQN1 + dQN2 - dQN3;
	pData[11] = 0.0;

	pData[12] = 0.0;
	pData[13] = 0.0;
	pData[14] = 0.0;
	pData[15] = 1.0;

	if (bMultiply)
	{
		m_matFrame = matRef * m_matFrame;
	}
	else
	{
		m_matFrame = matRef;
	}

	CheckFrameFrontFace();
	TellParentContentChanged();
}

//////////////////////////////////////////////////////////////////////
// Translate Frame

void COGLFrame::Translate(double dX, double dY, double dZ, bool bMultiply)
{
	Matrix<double> matTrans(4, 4);
	double* pData = matTrans.Data();

	pData[0] = 1.0;
	pData[1] = 0.0;
	pData[2] = 0.0;
	pData[3] = 0.0;

	pData[4] = 0.0;
	pData[5] = 1.0;
	pData[6] = 0.0;
	pData[7] = 0.0;

	pData[8]  = 0.0;
	pData[9]  = 0.0;
	pData[10] = 1.0;
	pData[11] = 0.0;

	pData[12] = dX;
	pData[13] = dY;
	pData[14] = dZ;
	pData[15] = 1.0;

	if (bMultiply)
	{
		m_matFrame = matTrans * m_matFrame;
	}
	else
	{
		m_matFrame = matTrans;
	}

	TellParentContentChanged();
}

//////////////////////////////////////////////////////////////////////
// Scale Frame

void COGLFrame::Scale(double dX, double dY, double dZ, bool bMultiply)
{
	Matrix<double> matTrans(4, 4);
	double* pData = matTrans.Data();

	pData[0] = dX;
	pData[1] = 0.0;
	pData[2] = 0.0;
	pData[3] = 0.0;

	pData[4] = 0.0;
	pData[5] = dY;
	pData[6] = 0.0;
	pData[7] = 0.0;

	pData[8]  = 0.0;
	pData[9]  = 0.0;
	pData[10] = dZ;
	pData[11] = 0.0;

	pData[12] = 0.0;
	pData[13] = 0.0;
	pData[14] = 0.0;
	pData[15] = 1.0;

	if (bMultiply)
	{
		m_matFrame = matTrans * m_matFrame;
	}
	else
	{
		m_matFrame = matTrans;
	}

	CheckFrameFrontFace();
	TellParentContentChanged();
}

//////////////////////////////////////////////////////////////////////
/// Tell parents that content has changed

void COGLFrame::TellParentContentChanged()
{
	list<COGLBaseElement*>::iterator itEl;

	for (itEl = m_listParent.begin();
	     itEl != m_listParent.end();
	     ++itEl)
	{
		COGLBaseElementList* pList = dynamic_cast<COGLBaseElementList*>((COGLBaseElement*) (*itEl));
		if (pList)
		{
			pList->SetContentChanged(true, true, false);
		}
	}
}

//////////////////////////////////////////////////////////////////////
/// Apply

bool COGLFrame::Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData)
{
	if (m_bInvertFrontFace)
	{
		rData.bInvertFrontFace = true;
	}

	int iMatrixMode;
	glGetIntegerv(GL_MATRIX_MODE, &iMatrixMode);

	switch (m_eFrameMode)
	{
	case MODELVIEW:
		glMatrixMode(GL_MODELVIEW);
		break;

	case PROJECTION:
		glMatrixMode(GL_PROJECTION);
		break;

	case TEXTURE:
		glMatrixMode(GL_TEXTURE);
		break;
	}

	// Having a texture frame and autoscale mode, we need to reset the texture matrix. Otherwise the repeated calls to texture frames will multiply the scale factor!
	if (m_bAutoScaleToPixelSize && (m_eFrameMode == TEXTURE))
	{
		glLoadIdentity();
	}

	if (m_bUseOrigin)
	{
		glTranslated(-m_dOrigX, -m_dOrigY, -m_dOrigZ);
	}

	if ((m_bAutoBillboard || m_bAutoScaleToPixelSize) && (m_eFrameMode == MODELVIEW))
	{
		int piViewport[4];
		double pdModelView[16];
		double pdProjection[16];
		double dWO1, dWO2, dWO3;
		double dO1, dO2, dO3;
		double dX1, dX2, dX3;
		double dY1, dY2, dY3;
		double dZ1, dZ2, dZ3;
		double dAbs;
		double dPW, dPH;
		double dFacW, dFacH;
		Matrix<double> mT(4, 4);

		Matrix<double> mTInv(4, 4);

		glGetIntegerv(GL_VIEWPORT, piViewport);
		glGetDoublev(GL_MODELVIEW_MATRIX, pdModelView);
		glGetDoublev(GL_PROJECTION_MATRIX, pdProjection);

		// Transform origin to window coordinates
		gluProject(0.0, 0.0, 0.0,
				pdModelView, pdProjection, piViewport,
				&dWO1, &dWO2, &dWO3);

		dWO1 = double(piViewport[0]) + double(piViewport[2]) / 2.0;
		dWO2 = double(piViewport[1]) + double(piViewport[3]) / 2.0;

		// Unproject target window coordinates
		gluUnProject(dWO1, dWO2, dWO3,
				pdModelView, pdProjection, piViewport,
				&dO1, &dO2, &dO3);

		gluUnProject(dWO1 + 100.0, dWO2, dWO3,
				pdModelView, pdProjection, piViewport,
				&dX1, &dX2, &dX3);

		gluUnProject(dWO1, dWO2 + 100.0, dWO3,
				pdModelView, pdProjection, piViewport,
				&dY1, &dY2, &dY3);

		gluUnProject(dWO1, dWO2, dWO3 + 100.0,
				pdModelView, pdProjection, piViewport,
				&dZ1, &dZ2, &dZ3);

		// Now subtract the Origin
		dX1 -= dO1;
		dX2 -= dO2;
		dX3 -= dO3;
		dY1 -= dO1;
		dY2 -= dO2;
		dY3 -= dO3;
		dZ1 -= dO1;
		dZ2 -= dO2;
		dZ3 -= dO3;

		// And normalize the vectors
		dAbs = sqrt(dX1 * dX1 + dX2 * dX2 + dX3 * dX3);
		dX1 /= dAbs;
		dX2 /= dAbs;
		dX3 /= dAbs;

		dAbs = sqrt(dY1 * dY1 + dY2 * dY2 + dY3 * dY3);
		dY1 /= dAbs;
		dY2 /= dAbs;
		dY3 /= dAbs;

		dAbs = sqrt(dZ1 * dZ1 + dZ2 * dZ2 + dZ3 * dZ3);
		dZ1 /= dAbs;
		dZ2 /= dAbs;
		dZ3 /= dAbs;

		// Compose the vectors to a new matrix
		mT(1, 1) = dX1;
		mT(2, 1) = dX2;
		mT(3, 1) = dX3;
		mT(1, 2) = dY1;
		mT(2, 2) = dY2;
		mT(3, 2) = dY3;
		mT(1, 3) = dZ1;
		mT(2, 3) = dZ2;
		mT(3, 3) = dZ3;
		mT(4, 4) = 1.0;

		// Store the inverse matrix
		mTInv = mT.InvSVD();
		glMultMatrixd(mTInv.Data());

		if (m_bAutoScaleToPixelSize)
		{
			glGetDoublev(GL_MODELVIEW_MATRIX, pdModelView);

			gluUnProject(dWO1, dWO2, dWO3,
					pdModelView, pdProjection, piViewport,
					&dO1, &dO2, &dO3);

			// Transform width to window coordinates
			gluProject(dO1 + m_dASWidth, dO2, 0.0,
					pdModelView, pdProjection, piViewport,
					&dX1, &dX2, &dX3);

			// Transform height to window coordinates
			gluProject(dO1, dO2 + m_dASHeight, 0.0,
					pdModelView, pdProjection, piViewport,
					&dY1, &dY2, &dY3);

			dPW = Mag(dX1 - dWO1);
			dPH = Mag(dY2 - dWO2);

			dFacW = m_dASPWidth / dPW;
			dFacH = m_dASPHeight / dPH;

			// if we disabled Auto Billboarding, we undo the billboaring
			if (m_bAutoBillboard == false)
			{
				// undo the billboarding
				glMultMatrixd(mT.Data());
			}

			glScaled(dFacW, dFacH, 1.0);
		}
	}
	else if (m_bAutoScaleToPixelSize && (m_eFrameMode == TEXTURE))
	{
		// Adjust texture frame such that image is drawn in correct
		// correct aspect ratio relative to the screen coordinates

		int piViewport[4];
		double pdModelView[16];
		double pdProjection[16];
		double dWO1, dWO2, dWO3;
		double dO1, dO2, dO3;
		double dX1, dX2, dX3;
		double dY1, dY2, dY3;
		double dLX, dLY;
		double dAspectLocal, dAspectTarget, dAspectPixel;
		double dScale, dPixH, dScale2;

		glGetIntegerv(GL_VIEWPORT, piViewport);
		glGetDoublev(GL_MODELVIEW_MATRIX, pdModelView);
		glGetDoublev(GL_PROJECTION_MATRIX, pdProjection);

		// Transform origin to window coordinates
		gluProject(0.0, 0.0, 0.0,
				pdModelView, pdProjection, piViewport,
				&dWO1, &dWO2, &dWO3);

		dWO1 = double(piViewport[0]) + double(piViewport[2]) / 2.0;
		dWO2 = double(piViewport[1]) + double(piViewport[3]) / 2.0;

		// Unproject target window coordinates
		gluUnProject(dWO1, dWO2, dWO3,
				pdModelView, pdProjection, piViewport,
				&dO1, &dO2, &dO3);

		gluUnProject(dWO1 + 100.0, dWO2, dWO3,
				pdModelView, pdProjection, piViewport,
				&dX1, &dX2, &dX3);

		gluUnProject(dWO1, dWO2 + 100.0, dWO3,
				pdModelView, pdProjection, piViewport,
				&dY1, &dY2, &dY3);

		dX1 -= dO1;
		dX2 -= dO2;
		dX3 -= dO3;
		dY1 -= dO1;
		dY2 -= dO2;
		dY3 -= dO3;

		// Lengths of screen vectors in local coordinates parallel to screen
		dLX = sqrt(dX1 * dX1 + dX2 * dX2 + dX3 * dX3);
		dLY = sqrt(dY1 * dY1 + dY2 * dY2 + dY3 * dY3);

		dAspectLocal  = dLY / dLX;
		dAspectTarget = (m_dASHeight / m_dASWidth) / dAspectLocal;

		dAspectPixel = m_dASPHeight / m_dASPWidth;
		if (dAspectTarget > dAspectPixel)
		{
			dScale = dAspectTarget / dAspectPixel;

			// Height of scaled plane in pixels
			dPixH = Mag(m_dASHeight * 100.0 / dLY / dScale);
			// Is height of texture range in screen pixel larger than pixel height of texture?
			if (((dPixH > m_dASPHeight) && !m_bAllowTextureScaleUp)
			    || ((dPixH < m_dASPHeight) && !m_bAllowTextureScaleDown))
			{
				dScale2 = dPixH / m_dASPHeight;
			}
			else
			{
				dScale2 = 1.0;
			}
			glScaled(dScale2, dScale * dScale2, 1.0);
		}
		else
		{
			dScale = dAspectPixel / dAspectTarget;

			// Height of scaled plane in pixels
			dPixH = Mag(m_dASHeight * 100.0 / dLY);
			// Is height of texture range in screen pixel larger than pixel height of texture?
			if (((dPixH > m_dASPHeight) && !m_bAllowTextureScaleUp)
			    || ((dPixH < m_dASPHeight) && !m_bAllowTextureScaleDown))
			{
				dScale2 = dPixH / m_dASPHeight;
			}
			else
			{
				dScale2 = 1.0;
			}
			glScaled(dScale * dScale2, dScale2, 1.0);
		}
	}

	if (m_bMultiplyMatrix)
	{
		glMultMatrixd(m_matFrame.Data());
	}
	else
	{
		glLoadMatrixd(m_matFrame.Data());
	}

	if (m_refAnimateFrame.IsValid())
	{
		if (m_eFrameMode == TEXTURE)
		{
			glTranslatef(0.5, 0.5, 0);
			m_refAnimateFrame->Apply(eMode, rData);
			glTranslatef(-0.5, -0.5, 0);
		}
		else
		{
			m_refAnimateFrame->Apply(eMode, rData);
		}
	}

	if (m_bUseOrigin)
	{
		glTranslated(m_dOrigX, m_dOrigY, m_dOrigZ);
	}

	glMatrixMode(iMatrixMode);

	glPixelZoom(float(m_dPixelZoomX), float(m_dPixelZoomY));

	return true;
}
