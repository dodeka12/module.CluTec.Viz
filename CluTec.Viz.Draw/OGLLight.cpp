////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLLight.cpp
//
// summary:   Implements the ogl light class
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

// OGLLight.cpp: Implementierung der Klasse COGLLight.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLLight.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLLight::COGLLight()
{
	m_sTypeName = "Light";

	Reset();

	m_eLight = GL_LIGHT0;
	m_bIsEnabled = false;
}

COGLLight::COGLLight(const COGLLight& rLight)
{
	m_sTypeName = "Light";

	*this = rLight;
}


COGLLight& COGLLight::operator= (const COGLLight& rLight)
{
	m_eLight = rLight.m_eLight;
	m_bIsEnabled = rLight.m_bIsEnabled;
	m_uChangeFlag = rLight.m_uChangeFlag;

	m_Ambient = rLight.m_Ambient;
	m_Diffuse = rLight.m_Diffuse;
	m_Specular = rLight.m_Specular;
	m_Position = rLight.m_Position;
	m_SpotDir = rLight.m_SpotDir;

	m_fSpotExp = rLight.m_fSpotExp;
	m_fSpotCutOff = rLight.m_fSpotCutOff;
	
	m_fConstAtt = rLight.m_fConstAtt;
	m_fLinAtt = rLight.m_fLinAtt;
	m_fQuadAtt = rLight.m_fQuadAtt;

	return *this;
}


//////////////////////////////////////////////////////////////////////
/// Reset

void COGLLight::Reset()
{
	m_Ambient.Set(0.0, 0.0, 0.0, 1.0);
	m_Diffuse.Set(1.0, 1.0, 1.0, 1.0);
	m_Specular.Set(1.0, 1.0, 1.0, 1.0);

	m_Position[0] = 0.0;
	m_Position[1] = 0.0;
	m_Position[2] = 1.0;
	m_Position[3] = 0.0;

	m_SpotDir[0] = 0.0;
	m_SpotDir[1] = 0.0;
	m_SpotDir[2] = -1.0;

	m_fSpotExp = 0.0;
	m_fSpotCutOff = 180.0;

	m_fConstAtt = 1.0;
	m_fLinAtt = 0.0;
	m_fQuadAtt = 0.0;

	m_uChangeFlag = /*EChange::*/ALL;
}

//////////////////////////////////////////////////////////////////////
// Apply
bool COGLLight::Apply( COGLBaseElement::EApplyMode eMode, 
					  COGLBaseElement::SApplyData &rData)
{
	if (m_bIsEnabled)
	{
		glEnable( m_eLight );

		glLightfv(m_eLight, GL_AMBIENT, m_Ambient.Data());
		glLightfv(m_eLight, GL_DIFFUSE, m_Diffuse.Data());
		glLightfv(m_eLight, GL_SPECULAR, m_Specular.Data());
		glLightfv(m_eLight, GL_POSITION, m_Position.Data());
		glLightfv(m_eLight, GL_SPOT_DIRECTION, m_SpotDir.Data());
		glLightf(m_eLight, GL_SPOT_EXPONENT, m_fSpotExp);
		glLightf(m_eLight, GL_SPOT_CUTOFF, m_fSpotCutOff);
		glLightf(m_eLight, GL_CONSTANT_ATTENUATION, m_fConstAtt);
		glLightf(m_eLight, GL_LINEAR_ATTENUATION, m_fLinAtt);
		glLightf(m_eLight, GL_QUADRATIC_ATTENUATION, m_fQuadAtt);
	}
	else
	{
		glDisable( m_eLight );
	}

	return true;
}


/*
bool COGLLight::Apply( COGLBaseElement::EApplyMode eMode, 
					  COGLBaseElement::SApplyData &rData)
{
	if ( (m_uChangeFlag & uint(EChange::AMBIENT)) )
	{
		glLightfv(m_eLight, GL_AMBIENT, m_Ambient.Data());
		m_uChangeFlag &= ~uint(EChange::AMBIENT);
	}

	if ( (m_uChangeFlag & uint(EChange::DIFFUSE)) )
	{
		glLightfv(m_eLight, GL_DIFFUSE, m_Diffuse.Data());
		m_uChangeFlag &= ~uint(EChange::DIFFUSE);
	}

	if ( (m_uChangeFlag & uint(EChange::SPECULAR)) )
	{
		glLightfv(m_eLight, GL_SPECULAR, m_Specular.Data());
		m_uChangeFlag &= ~uint(EChange::SPECULAR);
	}

	if ( (m_uChangeFlag & uint(EChange::POSITION)) )
	{
		glLightfv(m_eLight, GL_POSITION, m_Position.Data());
		m_uChangeFlag &= ~uint(EChange::POSITION);
	}

	if ( (m_uChangeFlag & uint(EChange::SPOTDIR)) )
	{
		glLightfv(m_eLight, GL_SPOT_DIRECTION, m_SpotDir.Data());
		m_uChangeFlag &= ~uint(EChange::SPOTDIR);
	}

	if ( (m_uChangeFlag & uint(EChange::SPOTEXP)) )
	{
		glLightf(m_eLight, GL_SPOT_EXPONENT, m_fSpotExp);
		m_uChangeFlag &= ~uint(EChange::SPOTEXP);
	}

	if ( (m_uChangeFlag & uint(EChange::SPOTCUTOFF)) )
	{
		glLightf(m_eLight, GL_SPOT_CUTOFF, m_fSpotCutOff);
		m_uChangeFlag &= ~uint(EChange::SPOTCUTOFF);
	}

	if ( (m_uChangeFlag & uint(EChange::CONSTATT)) )
	{
		glLightf(m_eLight, GL_CONSTANT_ATTENUATION, m_fConstAtt);
		m_uChangeFlag &= ~uint(EChange::CONSTATT);
	}

	if ( (m_uChangeFlag & uint(EChange::LINATT)) )
	{
		glLightf(m_eLight, GL_LINEAR_ATTENUATION, m_fLinAtt);
		m_uChangeFlag &= ~uint(EChange::LINATT);
	}

	if ( (m_uChangeFlag & uint(EChange::QUADATT)) )
	{
		glLightf(m_eLight, GL_QUADRATIC_ATTENUATION, m_fQuadAtt);
		m_uChangeFlag &= ~uint(EChange::QUADATT);
	}

	return true;
}
*/

//////////////////////////////////////////////////////////////////////
// Apply

bool COGLLight::Apply()
{
	if (m_bIsEnabled)
	{
		glEnable( m_eLight );

		glLightfv(m_eLight, GL_AMBIENT, m_Ambient.Data());
		glLightfv(m_eLight, GL_DIFFUSE, m_Diffuse.Data());
		glLightfv(m_eLight, GL_SPECULAR, m_Specular.Data());

		glLightfv(m_eLight, GL_POSITION, m_Position.Data());
		glLightfv(m_eLight, GL_SPOT_DIRECTION, m_SpotDir.Data());

		glLightf(m_eLight, GL_SPOT_EXPONENT, m_fSpotExp);
		glLightf(m_eLight, GL_SPOT_CUTOFF, m_fSpotCutOff);

		glLightf(m_eLight, GL_CONSTANT_ATTENUATION, m_fConstAtt);
		glLightf(m_eLight, GL_LINEAR_ATTENUATION, m_fLinAtt);
		glLightf(m_eLight, GL_QUADRATIC_ATTENUATION, m_fQuadAtt);
	}
	else
	{
		glDisable( m_eLight );
	}

	return true;
}

/*
bool COGLLight::Apply(GLenum eWhat)
{
	if (eWhat == GL_AMBIENT) glLightfv(m_eLight, GL_AMBIENT, m_Ambient.Data());
	else if (eWhat == GL_DIFFUSE) glLightfv(m_eLight, GL_DIFFUSE, m_Diffuse.Data());
	else if (eWhat == GL_SPECULAR) glLightfv(m_eLight, GL_SPECULAR, m_Specular.Data());
	else if (eWhat == GL_POSITION) glLightfv(m_eLight, GL_POSITION, m_Position.Data());
	else if (eWhat == GL_SPOT_DIRECTION) glLightfv(m_eLight, GL_SPOT_DIRECTION, m_SpotDir.Data());
	else if (eWhat == GL_SPOT_EXPONENT) glLightf(m_eLight, GL_SPOT_EXPONENT, m_fSpotExp);
	else if (eWhat == GL_SPOT_CUTOFF) glLightf(m_eLight, GL_SPOT_CUTOFF, m_fSpotCutOff);
	else if (eWhat == GL_CONSTANT_ATTENUATION) glLightf(m_eLight, GL_CONSTANT_ATTENUATION, m_fConstAtt);
	else if (eWhat == GL_LINEAR_ATTENUATION) glLightf(m_eLight, GL_LINEAR_ATTENUATION, m_fLinAtt);
	else if (eWhat == GL_QUADRATIC_ATTENUATION) glLightf(m_eLight, GL_QUADRATIC_ATTENUATION, m_fQuadAtt);

	return true;
}
*/
