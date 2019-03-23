////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      CameraTransform.h
//
// summary:   Declares the camera transform class
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



#include "CluTec.Math/Static.Vector.h"
#include "CluTec.Math/Static.Matrix.h"
#include "CluTec.Math/Static.Vector.Math.h"
#include "CluTec.Math/Static.Matrix.Math.h"


class CCameraTransform
{
public:
	using TValue = float;
	using TVec3 = Clu::SVector3<TValue>;
	using TMat4 = Clu::SMatrix<float, 4>;

private:

	GLfloat m_afTrans[3];
	TMat4 m_matRotation;

	bool m_bInvertXRotation;
	bool m_bInvertYRotation;

public:

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 	Default constructor.
	/// </summary>
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CCameraTransform()
	{
		m_bInvertXRotation = false;
		m_bInvertYRotation = false;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 	Invert rotation in x direction.
	/// </summary>
	///
	/// <param name="bEnable"> true to enable inversion, false to disable. </param>
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void InvertXAxis(bool bEnable)
	{
		m_bInvertXRotation = bEnable;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 	Invert rotation in y direction
	/// </summary>
	///
	/// <param name="bEnable"> true to enable inversion, false to disable. </param>
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void InvertYAxis(bool bEnable)
	{
		m_bInvertYRotation = bEnable;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 	Convert this object into an array representation.
	/// </summary>
	///
	/// <param name="afData"> The array to populate with the rotation matrix data. </param>
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void RotationMatToArray(float afData[16]) const
	{
		m_matRotation.ToArray(afData);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 	Resets this object.
	/// </summary>
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Reset()
	{
		for (int i = 0; i < 3; i++)
		{
			m_afTrans[i] = 0.0f;
		}

		m_afTrans[2] = 5.0;

		m_matRotation.SetIdentity();
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 	Gets view vector.
	/// </summary>
	///
	/// <returns> The view vector. </returns>
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	TVec3 GetViewVector() const
	{
		return TVec3(m_matRotation(0, 2), m_matRotation(1, 2), m_matRotation(2, 2));
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 	Gets right vector.
	/// </summary>
	///
	/// <returns> The right vector. </returns>
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	TVec3 GetRightVector() const
	{
		return TVec3(m_matRotation(0, 0), m_matRotation(1, 0), m_matRotation(2, 0));
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 	Gets up vector.
	/// </summary>
	///
	/// <returns> The up vector. </returns>
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	TVec3 GetUpVector() const
	{
		return TVec3(m_matRotation(0, 1), m_matRotation(1, 1), m_matRotation(2, 1));
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 	Gets camera position.
	/// </summary>
	///
	/// <returns> The camera position. </returns>
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	TVec3 GetCameraPosition() const
	{
		return TVec3(m_afTrans[0], m_afTrans[1], m_afTrans[2]);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 	Sets camera position.
	/// </summary>
	///
	/// <param name="vPosition"> The position to set the camera to. </param>
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void SetCameraPosition(TVec3 vfPosition)
	{
		m_afTrans[0] = vfPosition.x();
		m_afTrans[1] = vfPosition.y();
		m_afTrans[2] = vfPosition.z();
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 	Translate in view direction.
	/// </summary>
	///
	/// <param name="fDistance"> The distance the camera is moved in view direction. </param>
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void TranslateInViewDirection(float fDistance)
	{
		TVec3 vfPos = GetCameraPosition();
		SetCameraPosition(vfPos + fDistance * GetViewVector());
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 	Translate strafe to view direction. This is a movement perpendicular to the view direction.
	/// </summary>
	///
	/// <param name="fDistance"> The distance the camera is moved perpendicular to the view direction. </param>
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void TranslateStrafeToViewDirection(float fDistance)
	{
		TVec3 vfPos = GetCameraPosition();
		SetCameraPosition(vfPos + fDistance * GetRightVector());
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 	Rotate around y Axis.
	/// </summary>
	///
	/// <param name="fValRad"> The rotation angle in radians. </param>
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void RotateAroundY(float fValRad)
	{
		TVec3 vRotationAxis = TVec3(0, 1, 0);

		if (m_bInvertXRotation)
		{
			vRotationAxis = TVec3(0, -1, 0);
		}

		TVec3 vUp = GetUpVector();
		TMat4 matRotY = Clu::RotMat4<float>(fValRad, vRotationAxis);
		m_matRotation = m_matRotation * matRotY;

		// Apply look at correct the rotation around the Z-Axis
		LookAt(GetCameraPosition() + GetViewVector(), vUp);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 	Rotate around x Axis.
	/// </summary>
	///
	/// <param name="fValRad"> The rotation angle in radians. </param>
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void RotateAroundX(float fValRad)
	{
		TVec3 vRotationAxis = TVec3(1, 0, 0);

		if (m_bInvertYRotation)
		{
			vRotationAxis = TVec3(-1, 0, 0);
		}

		TVec3 vUp = GetUpVector();
		TMat4 matRotX = Clu::RotMat4<float>(fValRad, vRotationAxis);
		m_matRotation = m_matRotation * matRotX;

		// Apply look at correct the rotation around the Z-Axis
		LookAt(GetCameraPosition() + GetViewVector(), vUp);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 	Look at specified target point. The new UP vector of the camera is on the plane spanned by:
	/// 	- Camera Position
	/// 	- Target Point
	/// 	- given Up Vector (vfCameraUp)
	/// </summary>
	///
	/// <param name="vfTarget">   Target to look at. </param>
	/// <param name="vfCameraUp"> (Optional) the camera up vector. </param>
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void LookAt(TVec3 vfTarget, TVec3 vfCameraUp = TVec3(0, 1, 0))
	{
		TVec3 vfCameraPos = GetCameraPosition();

		Reset();

		SetCameraPosition(vfCameraPos);

		TVec3 vfViewVec = Normalize(vfTarget - vfCameraPos);

		TVec3 vfVecRight = Normalize(vfViewVec ^ vfCameraUp);
		TVec3 vfVecUp = Normalize(vfVecRight) ^ vfViewVec;

		// Apply the Right, Up and View vector to the transformation matrix
		m_matRotation.SetIdentity();

		m_matRotation(0, 0) = vfVecRight[0];
		m_matRotation(1, 0) = vfVecRight[1];
		m_matRotation(2, 0) = vfVecRight[2];

		m_matRotation(0, 1) = vfVecUp[0];
		m_matRotation(1, 1) = vfVecUp[1];
		m_matRotation(2, 1) = vfVecUp[2];

		m_matRotation(0, 2) = -vfViewVec[0];
		m_matRotation(1, 2) = -vfViewVec[1];
		m_matRotation(2, 2) = -vfViewVec[2];
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 	Look at bounding sphere.
	/// </summary>
	///
	/// <param name="vfSphereCenter">		 The sphere center. </param>
	/// <param name="fSphereRadius">		 The sphere radius. </param>
	/// <param name="fCameraFieldOfViewRad"> The camera field of view radians. </param>
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void LookAtBoundingSphere(TVec3 vfSphereCenter, float fSphereRadius, float fCameraFieldOfViewRad)
	{
		LookAt(vfSphereCenter);

		TVec3 vfCameraPos = GetCameraPosition();

		// Compute the distance the camera should be to fit the entire bounding sphere
		float fDesiredViewDistance = (fSphereRadius * 2.0f) / (float)tan(fCameraFieldOfViewRad / 2.0f);

		float fCurrentViewDistance = Length(vfSphereCenter - vfCameraPos);

		TranslateInViewDirection(fDesiredViewDistance - fCurrentViewDistance);
	}
};
