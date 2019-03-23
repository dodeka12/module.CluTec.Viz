////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLPeek.cpp
//
// summary:   Implements the ogl peek class
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

// OGLTranslation.cpp: Implementierung der Klasse COGLPeek.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLPeek.h"


const int COGLPeek::PT_NONE = 0;
const int COGLPeek::PT_FRAMES = 1;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLPeek::COGLPeek()
{
	m_sName = "Peek";
	m_sTypeName = "Peek";

	Reset();
}

COGLPeek::COGLPeek( const string &sName,  unsigned int uType )
{
	m_sName = sName;
	m_sTypeName = "Peek";

	Reset();
	SetType( uType );
}

COGLPeek::COGLPeek(const COGLPeek& rPeek )
{
	m_sTypeName = "Peek";

	Reset();
	*this = rPeek;
}

COGLPeek& COGLPeek::operator= (const COGLPeek& rPeek)
{
	if ( &rPeek == this )
		return *this;

	COGLBaseElement::operator =( rPeek );

	m_uType = rPeek.m_uType;
	m_mProjMat = rPeek.m_mProjMat;
	m_mModelMat = rPeek.m_mModelMat;
	m_mTexMat = rPeek.m_mTexMat;

	memcpy( m_piViewport, rPeek.m_piViewport, 4*sizeof(int) );

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Reset

void COGLPeek::Reset()
{	
	m_uType = PT_NONE;

	m_mProjMat.Resize( 4, 4 );
	m_mProjMat.Identity();

	m_mModelMat.Resize( 4, 4 );
	m_mModelMat.Identity();

	m_mTexMat.Resize( 4, 4 );
	m_mTexMat.Identity();

	memset( m_piViewport, 0, 4 * sizeof(int) );
}

//////////////////////////////////////////////////////////////////////
// Ensure that variable is a variable list

//bool COGLPeek::ValidateVar()
//{
//	CCodeVar *pVar = &m_pCodeBase->GetVar( m_sVarName.c_str() );
//	if ( pVar->Type() == PDT_NOTYPE )
//	{
//		m_pCodeBase->NewVar( m_sVarName.c_str(), PDT_VARLIST );
//	}
//	else if ( pVar->Type() != PDT_VARLIST )
//	{
//		m_pCodeBase->DeleteVar( m_sVarName.c_str() );
//		m_pCodeBase->NewVar( m_sVarName.c_str(), PDT_VARLIST );
//	}
//
//	return true;
//}

//////////////////////////////////////////////////////////////////////
/// Get extent of view in local coordinates

bool COGLPeek::GetViewExtentInLocalCoords( double pdBL[3], double pdBR[3], double pdTR[3], double pdTL[3], 
										   double dDepth )
{
	double dWX, dWY, dWZ;

	gluProject( 0.0, 0.0, dDepth, m_mModelMat.Data(), m_mProjMat.Data(), m_piViewport, &dWX, &dWY, &dWZ );

	gluUnProject( double(m_piViewport[0]), double(m_piViewport[1]), dWZ, 
		m_mModelMat.Data(), m_mProjMat.Data(), m_piViewport,
		&pdBL[0], &pdBL[1], &pdBL[2] );

	gluUnProject( double(m_piViewport[0] + m_piViewport[2]), double(m_piViewport[1]), dWZ, 
		m_mModelMat.Data(), m_mProjMat.Data(), m_piViewport,
		&pdBR[0], &pdBR[1], &pdBR[2] );

	gluUnProject( double(m_piViewport[0] + m_piViewport[2]), double(m_piViewport[1] + m_piViewport[3]), dWZ, 
		m_mModelMat.Data(), m_mProjMat.Data(), m_piViewport,
		&pdTR[0], &pdTR[1], &pdTR[2] );

	gluUnProject( double(m_piViewport[0]), double(m_piViewport[1] + m_piViewport[3]), dWZ, 
		m_mModelMat.Data(), m_mProjMat.Data(), m_piViewport,
		&pdTL[0], &pdTL[1], &pdTL[2] );

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Get extent of view in screen coords

bool COGLPeek::GetViewExtentInScreenCoords( int &iX1, int &iY1, int &iX2, int &iY2 )
{
	iX1 = m_piViewport[0];
	iX2 = iX1 + m_piViewport[2] - 1;
	
	iY1 = m_piViewport[1];
	iY2 = iY1 + m_piViewport[3] - 1;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Apply

bool COGLPeek::Apply(COGLBaseElement::EApplyMode eMode, 
							COGLBaseElement::SApplyData &rData)
{
	if ( eMode != COGLBaseElement::DRAW || !rData.bInFirstRenderPass )
		return true;

	if ( m_uType == PT_NONE )
		return true;

	if ( m_uType & PT_FRAMES )
	{
		glGetDoublev( GL_PROJECTION_MATRIX, m_mProjMat.Data() );
		glGetDoublev( GL_MODELVIEW_MATRIX, m_mModelMat.Data() );
		glGetDoublev( GL_TEXTURE_MATRIX, m_mTexMat.Data() );
		glGetIntegerv( GL_VIEWPORT, m_piViewport );

		//mViewport(1,1) = double( m_piViewport[0] );
		//mViewport(2,1) = double( m_piViewport[1] );
		//mViewport(3,1) = double( m_piViewport[2] );
		//mViewport(4,1) = double( m_piViewport[3] );


		//if ( ValidateVar() )
		//{
		//	CCodeVar &rVar = m_pCodeBase->GetVar( m_sVarName.c_str() );
		//	TVarList &rList = *rVar.GetVarListPtr();
		//	rList.Set( 4 );
		//	rList(0) = mProjMat;
		//	rList(1) = mModelMat;
		//	rList(2) = mTexMat;
		//	rList(3) = mViewport;
		//}
	}
	//else if ( m_sType == "viewsize_local" )
	//{
	//	// Size of current viewport in local coordinate system
	//	double dX1, dY1, dZ1, dX2, dY2, dZ2, dWX, dWY, dWZ;
	//	double pdProjMat[16], pdModelMat[16];
	//	int m_piViewport[4];

	//	glGetDoublev( GL_PROJECTION_MATRIX, pdProjMat );
	//	glGetDoublev( GL_MODELVIEW_MATRIX, pdModelMat );
	//	glGetIntegerv( GL_VIEWPORT, m_piViewport );

	//	gluProject( 0.0, 0.0, 0.0, pdModelMat, pdProjMat, m_piViewport, &dWX, &dWY, &dWZ );
	//	gluUnProject( double(m_piViewport[0]), double(m_piViewport[1]), dWZ, 
	//					pdModelMat, pdProjMat, m_piViewport,
	//					&dX1, &dY1, &dZ1 );

	//	gluUnProject( double(m_piViewport[0] + m_piViewport[2]), double(m_piViewport[1] + m_piViewport[3]), dWZ, 
	//					pdModelMat, pdProjMat, m_piViewport,
	//					&dX2, &dY2, &dZ2 );

	//	if ( ValidateVar() )
	//	{
	//		CCodeVar &rVar = m_pCodeBase->GetVar( m_sVarName.c_str() );
	//		TVarList &rList = *rVar.GetVarListPtr();
	//		rList.Set( 6 );
	//		rList(0) = dX1;
	//		rList(1) = dY1;
	//		rList(2) = dZ1;
	//		rList(3) = dX2;
	//		rList(4) = dY2;
	//		rList(5) = dZ2;
	//	}
	//}

	return true;
}
