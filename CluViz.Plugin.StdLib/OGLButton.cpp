////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      OGLButton.cpp
//
// summary:   Implements the ogl button class
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
#include "OGLButton.h"
#include "SceneDraw.h"


COGLButton::COGLButton(CCLUCodeBase& rCodeBase)
{
	m_sTypeName = "Tool:Button";

	m_pCodeBase = &rCodeBase;

	m_colText.Set( 0.0f, 0.0f, 0.0f, 1.0f );
	m_colBGAct.Set( 0.9f, 0.9f, 0.9f, 1.0f );
	m_colBGStd.Set( 0.6f, 0.6f, 0.6f, 1.0f );
}

COGLButton::COGLButton( const COGLButton& oglBut )
{
	m_sTypeName = "Tool:Button";

	m_pCodeBase = oglBut.m_pCodeBase;
	m_colBGStd = oglBut.m_colBGStd;
	m_colBGAct = oglBut.m_colBGAct;

	*this = oglBut;
}

COGLButton::~COGLButton(void)
{
}


COGLButton& COGLButton::operator= (const COGLButton& oglBut )
{
	COGLTool::operator=( oglBut );


	return *this;
}


///////////////////////////////////////////////////////////////////
// Event: Drag Start
bool COGLButton::OnDragStart( int iButton, bool bShift, bool bCtrl, bool bAlt, 
								double dX, double dY, double dZ, double dLX, double dLY, double dLZ )
{
	return false;
}

///////////////////////////////////////////////////////////////////
// Event: Drag End
bool COGLButton::OnDragEnd( int iButton, bool bShift, bool bCtrl, bool bAlt, 
							double dX, double dY, double dZ, double dLX, double dLY, double dLZ )
{
	return true;
}

///////////////////////////////////////////////////////////////////
// Event: Drag
bool COGLButton::OnDrag( int iButton, bool bShift, bool bCtrl, bool bAlt, 
							double dX, double dY, double dZ, double dDX, double dDY, double dDZ, 
							double dLX, double dLY, double dLZ, double dLDX, double dLDY, double dLDZ )
{
	return false;
}

///////////////////////////////////////////////////////////////////
// Event: Click
bool COGLButton::OnClick( int iButton, bool bShift, bool bCtrl, bool bAlt, 
							double dX, double dY, double dZ, double dLX, double dLY, double dLZ )
{
	return true;
}

///////////////////////////////////////////////////////////////////
// Create the tool visualization

bool COGLButton::CreateTool(COGLBEReference& refTool)
{
	CCLUCodeBase &rCB = *m_pCodeBase;
	COGLDrawBase &rDB = *rCB.GetOGLDrawBase();
	COGLBERepository &rSR = *rDB.GetSceneRepository();
	
	refTool = rSR.New( this );
	TScene refDrawScene = rDB.GetScene();

	COGLVertex xP, xA, xB;

	COGLScene *pToolStd = new COGLScene();
	COGLScene *pToolPick = new COGLScene();
	COGLScene *pToolDrag = new COGLScene();
	if ( !pToolStd || !pToolPick || !pToolDrag )
	{
		m_sLastError = "Out of memory while creating button tool.";
		return false;
	}
	TScene refToolStd = rSR.New( pToolStd );
	TScene refToolPick = rSR.New( pToolPick );
	TScene refToolDrag = rSR.New( pToolDrag );

	EnableResetFrame( true );
	SetView( float(m_dX), float(m_dY), float(m_dWidth), float(m_dHeight), 
			m_dX < 1.0001, m_dY < 1.0001, m_dWidth < 1.0001, m_dHeight < 1.0001 );
	EnableLocalView( true );

	//SetProjVolume(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 10.0f );
	//SetProjType( COGLScene::CENTRAL );
	SetOrthoProj(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f );
	SetProjType( COGLScene::ORTHO );
	EnableOrthoViewportAspectRatio( false );
	EnableLocalProj( true );

	//SetViewAlign();
	//SetViewLocal(); //??
	//SetViewOriginRef();
	SetDepthLevel(1.0);
	SetDepthLevelType(COGLScene::SViewport::DL_Relative);

	EnablePick( true );
	EnableNotifyMouseDrag( true );
	EnableDrag(0, true);
	EnableNotifyMouseClick( true );

	////////////////////////////////////
	// Draw Standard Button
	rDB.SetScene( refToolStd );
	
	if ( !SetTextureText( rCB, m_sText, m_colText, 2.0, 2.0, m_sLastError, 0.0 ) )
		return false;
	
	xP.Set( 0.0f, 0.0f, 0.0f );
	xA.Set( 2.0f, 0.0f, 0.0f );
	xB.Set( 0.0f, 2.0f, 0.0f );
	rCB.GetOGLDrawBase()->DrawPlane( xP, xA, xB, false, false );

	SetTextureEmpty( rCB, m_sLastError );

	////////////////////////////////////
	// Draw Pick Button
	rDB.SetScene( refToolPick );

	rDB.DrawBaseElement( rSR.New(new TOGLColor(m_colBGAct)) );
	rDB.DrawBaseElement( refToolStd );

	////////////////////////////////////
	// Draw Drag Button
	rDB.SetScene( refToolDrag );

	rDB.DrawBaseElement( rSR.New(new TOGLColor(m_colBGAct)) );
	{
		COGLFrame *pFrame = new COGLFrame();
		pFrame->SetFrameMode( COGLFrame::MODELVIEW );
		pFrame->Scale( 0.9, 0.9, 1.0, false );
		rDB.DrawBaseElement( rSR.New(pFrame) );
	}
	rDB.DrawBaseElement( refToolStd );

	////////////////////////////////////
	// Draw Button
	rDB.SetScene( refTool );

	rDB.DrawBaseElement( rSR.New(new TOGLColor(m_colBGStd)) );
	rDB.DrawBaseElement( refToolStd );

	SetPickScene( refToolPick );
	SetPickScene( refToolDrag, false, false, false, false, true );

	// Draw to previously stored scene
	rDB.SetScene( refDrawScene );

	return true;
}
