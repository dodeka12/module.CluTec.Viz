////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Scene.cpp
//
// summary:   Implements the function scene class
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

#include "Func_Scene.h"

//////////////////////////////////////////////////////////////////////
/// Function creates a scene with an orthographic coordinate system

bool StartOverlayFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 0 && iVarCount != 6 && iVarCount != 7)
	{
		int piPar[] = { 0, 6, 7 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 3, iLine, iPos);
		return false;
	}

	TCVCounter iTrueAspect = 0;
	TCVScalar dVal[6] = { 0.0, 100.0, 100.0, 0.0, -100.0, 100.0 };

	if (iVarCount >= 6)
	{
		for (int i = 0; i < 6; ++i)
		{
			if (!mVars(i).CastToScalar(dVal[i], rCB.GetSensitivity()))
			{
				rCB.GetErrorList().GeneralError("One of the six coordinate values is not a scalar.", iLine, iPos);
				return false;
			}
		}
	}

	if (iVarCount >= 7)
	{
		if (!mVars(6).CastToCounter(iTrueAspect))
		{
			rCB.GetErrorList().GeneralError("Expect 7th parameter to be a boolean value, indicating whether "
				"coordinate dimensions are to be adjusted to keep the true aspect ratio (true), or not (false, default).",
				iLine, iPos);
			return false;
		}
	}

	COGLScene *pScene = new COGLScene();
	if (!pScene)
	{
		rCB.GetErrorList().OutOfMemory(iLine, iPos);
		return false;
	}

	COGLBEReference refScene = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pScene);
	//rCB.GetOGLDrawBase()->DrawBaseElement( refScene );

	pScene->SetName("Overlay");
	pScene->EnableResetFrame(true);
	pScene->EnableLocalProj(true);
	pScene->SetProjType(COGLScene::ORTHO);
	pScene->EnableOrthoViewportAspectRatio((iTrueAspect != 0));
	pScene->SetOrthoProj(float(dVal[0]), float(dVal[1]),
		float(dVal[2]), float(dVal[3]),
		float(dVal[4]), float(dVal[5]));

	rCB.GetOGLDrawBase()->SetScene(refScene);

	rVar = refScene;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Function ends a scene with an orthographic coordinate system

bool EndOverlayFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 0)
	{
		int piPar[] = { 0 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	rCB.GetOGLDrawBase()->SetScene(rCB.GetMainSceneRef());

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Function creates a scene with a separate view

bool StartViewFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 0 && iVarCount != 4 && iVarCount != 5 &&
		iVarCount != 6 && iVarCount != 7)
	{
		int piPar[] = { 0, 4, 5, 6, 7 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 5, iLine, iPos);
		return false;
	}

	TCVCounter iResetFrame = 0;
	TCVCounter iAutoTrans = 1;
	TCVScalar dVal[4] = { 0.0, 0.0, 1.0, 1.0 };
	TCVScalar dViewAngle = 60.0;

	if (iVarCount >= 4)
	{
		for (int i = 0; i < 4; ++i)
		{
			if (!mVars(i).CastToScalar(dVal[i], rCB.GetSensitivity()))
			{
				rCB.GetErrorList().GeneralError("One of the six view extent values is not a scalar.", iLine, iPos);
				return false;
			}
		}
	}

	if (iVarCount >= 5)
	{
		if (!mVars(4).CastToCounter(iResetFrame))
		{
			rCB.GetErrorList().GeneralError("Expect 5th parameter to be a boolean value, indicating whether "
				"coordinate is to be reset (true), or not (false, default).",
				iLine, iPos);
			return false;
		}
	}

	if (iVarCount >= 6)
	{
		if (!mVars(5).CastToCounter(iAutoTrans))
		{
			rCB.GetErrorList().GeneralError("Expect 6th parameter to be a boolean value, indicating whether "
				"view can be transformed independently (true, default), or not (false).",
				iLine, iPos);
			return false;
		}
	}

	if (iVarCount >= 7)
	{
		if (!mVars(6).CastToScalar(dViewAngle))
		{
			rCB.GetErrorList().GeneralError("Expect 7th parameter to be a scalar value giving view angle.",
				iLine, iPos);
			return false;
		}

		if (dViewAngle < 1.0 || dViewAngle > 179)
		{
			rCB.GetErrorList().GeneralError("View angle has to lie in range [1.0, 179.0]", iLine, iPos);
			return false;
		}
	}


	COGLScene *pScene = new COGLScene();
	if (!pScene)
	{
		rCB.GetErrorList().OutOfMemory(iLine, iPos);
		return false;
	}

	COGLBEReference refScene = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pScene);
	//rCB.GetOGLDrawBase()->DrawBaseElement( refScene );

	pScene->SetName("View");
	pScene->EnableResetFrame((iResetFrame ? true : false));
	pScene->EnableLocalProj(true);
	pScene->SetProjType(COGLScene::CENTRAL);
	pScene->SetProj(float(dViewAngle), 1.0f, 0.1f, 100.0f);

	//pScene->EnableOrthoViewportAspectRatio( (iTrueAspect != 0) );
	//pScene->SetOrthoProj( float( dVal[0] ), float( dVal[1] ), 
	//					  float( dVal[2] ), float( dVal[3] ), 
	//					  float( dVal[4] ), float( dVal[5] ) );

	pScene->EnableLocalView(true);
	pScene->SetView(float(dVal[0]), float(dVal[1]), float(dVal[2]), float(dVal[3]),
		(dVal[0] <= 1.0 ? true : false), (dVal[1] <= 1.0 ? true : false),
		(dVal[2] <= 1.0 ? true : false), (dVal[3] <= 1.0 ? true : false));

	if (iAutoTrans)
	{
		pScene->EnablePick(true);
		pScene->EnablePickableView(true);
		pScene->EnableAutoTranslate(true);
		pScene->EnableAutoRotate2(true);
		pScene->EnableDrag(0, true);
		pScene->EnableDrag(2, true);
	}

	rCB.GetOGLDrawBase()->SetScene(refScene);

	rVar = refScene;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Function ends a scene with a separate view

bool EndViewFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 0)
	{
		int piPar[] = { 0 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	rCB.GetOGLDrawBase()->SetScene(rCB.GetMainSceneRef());

	return true;
}




//////////////////////////////////////////////////////////////////////
// Get elements in base element list recursively

bool GetElementList(CCLUCodeBase& rCB, COGLBEReference refScene, CCodeVar& rVar, int iLine, int iPos)
{
	COGLBEReference refEl;
	COGLBaseElementList* pList = 0, * pSubList = 0;

	rVar.New(PDT_VARLIST);
	TVarList& rVarList = *rVar.GetVarListPtr();
	rVarList.Set(2);

	rVarList(0) = refScene;

	pList = dynamic_cast< COGLBaseElementList* >((COGLBaseElement*) refScene);
	if (!pList)
	{
		rCB.GetErrorList().GeneralError("Error in scene graph.", iLine, iPos);
		return false;
	}

	rVarList(1).New(PDT_VARLIST, "Constant");
	TVarList& rSubList = *rVarList(1).GetVarListPtr();

	const std::list<COGLBEReference>& rElList = pList->GetElementList();
	std::list<COGLBEReference>::const_iterator itEl;

	unsigned uIdx, uCnt = int(rElList.size());
	rSubList.Set(uCnt);

	for (itEl = rElList.begin(), uIdx = 0; itEl != rElList.end(); ++itEl, ++uIdx)
	{
		if ((pSubList = dynamic_cast< COGLBaseElementList* >((COGLBaseElement*) (*itEl))))
		{
			if (!GetElementList(rCB, (*itEl), rSubList(uIdx), iLine, iPos))
			{
				return false;
			}
		}
		else
		{
			rSubList(uIdx) = (*itEl);
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Get the scene graph of the given scene

bool GetSceneGraphFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount              = int(mVars.Count());
	COGLBaseElementList* pList = 0;
	COGLBEReference refScene;

	if (iVarCount > 1)
	{
		int piPar[] = { 0, 1 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (iVarCount == 0)
	{
		// Use main scene if no scene is given
		refScene = rCB.GetMainSceneRef();
		pList    = dynamic_cast< COGLBaseElementList* >((COGLBaseElement*) rCB.GetMainSceneRef());
	}
	else if (iVarCount >= 1)
	{
		if (mVars(0).BaseType() != PDT_SCENE)
		{
			rCB.GetErrorList().GeneralError("Expect a scene variable as parameter.", iLine, iPos);
			return false;
		}

		refScene = *mVars(0).GetScenePtr();

		if (!refScene.IsValid())
		{
			rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
			return false;
		}

		pList = dynamic_cast< COGLBaseElementList* >((COGLBaseElement*) refScene);
		if (!pList)
		{
			rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
			return false;
		}
	}

	return GetElementList(rCB, refScene, rVar, iLine, iPos);
}

//////////////////////////////////////////////////////////////////////
// Create a Scene Variable

bool SceneFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount != 1)
	{
		int piPar[] = { 1 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Expect name of scene as parameter.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = new COGLScene;

	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Out of memory while creating scene.", iLine, iPos);
		return false;
	}

	pScene->SetName(mVars(0).GetStringPtr()->Str());
	COGLBEReference SceneRef = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pScene);

	//AddPickable( SceneRef );
	rVar = SceneRef;

	return true;
}

//////////////////////////////////////////////////////////////////////
// Copy Scene

bool CopySceneFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount != 1)
	{
		int piPar[] = { 1 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a scene variable as parameter.", iLine, iPos);
		return false;
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pNewScene = new COGLScene(*pScene);

	if (!pNewScene)
	{
		rCB.GetErrorList().GeneralError("Out of memory while copying scene.", iLine, iPos);
		return false;
	}

	COGLBEReference SceneRef = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pNewScene);

	rVar = SceneRef;

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set scenes that are used as trigger for pick scene of this scene.

bool SetScenePickTriggerFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 2)
	{
		int piPar[] = { 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a scene variable as first parameter.", iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().GeneralError("Expect a list of scenes as second parameter.", iLine, iPos);
		return false;
	}

	TScene& rScene = *mVars(0).GetScenePtr();

	if (!rScene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Given scene is invalid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) rScene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Given scene is invalid.", iLine, iPos);
		return false;
	}

	TVarList& rList = *mVars(1).GetVarListPtr();

	int iIdx, iCnt = int(rList.Count());
	vector<COGLBEReference> vecPickScene;
	vecPickScene.resize(iCnt);

	for (iIdx = 0; iIdx < iCnt; ++iIdx)
	{
		if (rList(iIdx).BaseType() != PDT_SCENE)
		{
			rCB.GetErrorList().GeneralError("Element in list is not a scene.", iLine, iPos);
			return false;
		}

		vecPickScene[iIdx] = *rList(iIdx).GetScenePtr();
	}

	pScene->SetPickTriggerScenes(vecPickScene);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Enable/Disable auto adapt front face of vertex lists

bool EnableSceneAutoAdaptFrontFaceFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount      = int(mVars.Count());
	TCVCounter iEnable = 1;

	if ((iVarCount < 1) || (iVarCount > 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (iVarCount > 1)
	{
		if (!mVars(1).CastToCounter(iEnable))
		{
			rCB.GetErrorList().GeneralError("Expect true or false as second parameter.", iLine, iPos);
			return false;
		}
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	if (iEnable)
	{
		pScene->EnableAutoAdaptFrontFace(true);
	}
	else
	{
		pScene->EnableAutoAdaptFrontFace(false);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Enable/Disable adaptive redraw of scene.
// Only Works in conjunction with local viewport

bool EnableSceneAdaptiveRedrawFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount      = int(mVars.Count());
	TCVCounter iEnable = 1;

	if ((iVarCount < 1) || (iVarCount > 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (iVarCount > 1)
	{
		if (!mVars(1).CastToCounter(iEnable))
		{
			rCB.GetErrorList().GeneralError("Expect true or false as second parameter.", iLine, iPos);
			return false;
		}
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	//pScene->EnableAdaptiveRedraw( (iEnable != 0) );

	return true;
}

//////////////////////////////////////////////////////////////////////
// Force scene to redraw itself.
// Only Works in conjunction with local viewport

bool DoSceneAdaptiveRedrawFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 1)
	{
		int piPar[] = { 1 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a scene variable as parameter.", iLine, iPos);
		return false;
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	pScene->DoAdaptiveRedraw();

	return true;
}

//////////////////////////////////////////////////////////////////////
// Enable/Disable pickability on Mouse Over Scene

bool EnableScenePickFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount      = int(mVars.Count());
	TCVCounter iEnable = 1;

	if ((iVarCount < 1) || (iVarCount > 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (iVarCount > 1)
	{
		if (!mVars(1).CastToCounter(iEnable))
		{
			rCB.GetErrorList().GeneralError("Expect true or false as second parameter.", iLine, iPos);
			return false;
		}
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	if (iEnable)
	{
		pScene->EnablePick(true);
	}
	else
	{
		pScene->EnablePick(false);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Enable/Disable drawing in pick mode

bool EnableScenePickTargetFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount      = int(mVars.Count());
	TCVCounter iEnable = 1;

	if ((iVarCount < 1) || (iVarCount > 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (iVarCount > 1)
	{
		if (!mVars(1).CastToCounter(iEnable))
		{
			rCB.GetErrorList().GeneralError("Expect true or false as second parameter.", iLine, iPos);
			return false;
		}
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	if (iEnable)
	{
		pScene->EnablePickDraw(true);
	}
	else
	{
		pScene->EnablePickDraw(false);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Enable/Disable local time

bool EnableSceneLocalTimeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount      = int(mVars.Count());
	TCVCounter iEnable = 1;

	if ((iVarCount < 1) || (iVarCount > 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (iVarCount > 1)
	{
		if (!mVars(1).CastToCounter(iEnable))
		{
			rCB.GetErrorList().GeneralError("Expect true or false as second parameter.", iLine, iPos);
			return false;
		}
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	if (iEnable)
	{
		pScene->EnableLocalTime(true);
	}
	else
	{
		pScene->EnableLocalTime(false);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Enable/Disable Draw Modes

bool EnableSceneDrawModesFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount           = int(mVars.Count());
	TCVCounter iOpaque      = 1;
	TCVCounter iTransparent = 1;

	if (iVarCount != 3)
	{
		int piPar[] = { 3 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToCounter(iOpaque))
	{
		rCB.GetErrorList().GeneralError("Expect true/false for drawing opaque elements as second parameter.", iLine, iPos);
		return false;
	}

	if (!mVars(2).CastToCounter(iTransparent))
	{
		rCB.GetErrorList().GeneralError("Expect true/false for drawing transparent elements as second parameter.", iLine, iPos);
		return false;
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	pScene->EnableOpaqueDraw((iOpaque != 0));
	pScene->EnableTransparentDraw((iTransparent != 0));

	return true;
}

//////////////////////////////////////////////////////////////////////
// Enable/Disable Notification on Mouse Over Scene

bool EnableSceneNotifyFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount      = int(mVars.Count());
	TCVCounter iEnable = 1;
	TString csType;
	bool bVal;

	csType = "all";

	if ((iVarCount < 1) || (iVarCount > 3))
	{
		int piPar[] = { 1, 2, 3 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 3, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (iVarCount > 1)
	{
		if (!mVars(1).CastToCounter(iEnable))
		{
			rCB.GetErrorList().GeneralError("Expect true or false as second parameter.", iLine, iPos);
			return false;
		}
	}

	if (iVarCount > 2)
	{
		if (mVars(2).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().GeneralError("Expect 'MouseOver', 'MouseDrag' or 'MouseClick' as third parameter.", iLine, iPos);
			return false;
		}

		csType = *mVars(2).GetStringPtr();
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	if (iEnable)
	{
		bVal = true;
	}
	else
	{
		bVal = false;
	}

	csType.ToLowerCase();

	if (csType == "all")
	{
		pScene->EnableNotify(bVal);
	}
	else if (csType == "mouseclick")
	{
		pScene->EnableNotifyMouseClick(bVal);
	}
	else if (csType == "mouseover")
	{
		pScene->EnableNotifyMouseOver(bVal);
	}
	else if (csType == "mousedrag")
	{
		pScene->EnableNotifyMouseDrag(bVal);
	}
	else
	{
		rCB.GetErrorList().GeneralError("Expect 'MouseOver', 'MouseDrag', 'MouseClick', or 'All' as third parameter.", iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Enable/Disable Auto Adjust Frame

bool EnableSceneAutoAdjustFrameFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount      = int(mVars.Count());
	TCVCounter iEnable = 1;

	if ((iVarCount < 1) || (iVarCount > 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (iVarCount > 1)
	{
		if (!mVars(1).CastToCounter(iEnable))
		{
			rCB.GetErrorList().GeneralError("Expect true or false as second parameter.", iLine, iPos);
			return false;
		}
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	if (iEnable)
	{
		pScene->EnableAutoAdjustFrame(true);
	}
	else
	{
		pScene->EnableAutoAdjustFrame(false);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Enable/Disable Local Transformation

bool EnableSceneLocalTransformFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount      = int(mVars.Count());
	TCVCounter iEnable = 1;

	if ((iVarCount < 1) || (iVarCount > 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (iVarCount > 1)
	{
		if (!mVars(1).CastToCounter(iEnable))
		{
			rCB.GetErrorList().GeneralError("Expect true or false as second parameter.", iLine, iPos);
			return false;
		}
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	if (iEnable)
	{
		pScene->EnableLocalFrame(true);
	}
	else
	{
		pScene->EnableLocalFrame(false);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Enable/Disable Auto Translation

bool EnableSceneAutoTranslateFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount      = int(mVars.Count());
	TCVCounter iEnable = 1;

	if ((iVarCount < 1) || (iVarCount > 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (iVarCount > 1)
	{
		if (!mVars(1).CastToCounter(iEnable))
		{
			rCB.GetErrorList().GeneralError("Expect true or false as second parameter.", iLine, iPos);
			return false;
		}
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	if (iEnable)
	{
		pScene->EnableAutoTranslate(true);
	}
	else
	{
		pScene->EnableAutoTranslate(false);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Enable/Disable Auto Rotate
//
// 1. Scene Variable
// 2. true/false for enable/disable
// 3. true/false for rotate2/rotate1 (after/before translation)
//

bool EnableSceneAutoRotateFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount       = int(mVars.Count());
	TCVCounter iEnable  = 1;
	TCVCounter iRotate2 = 1;

	if ((iVarCount < 1) || (iVarCount > 3))
	{
		int piPar[] = { 1, 2, 3 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 3, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (iVarCount > 1)
	{
		if (!mVars(1).CastToCounter(iEnable))
		{
			rCB.GetErrorList().GeneralError("Expect true or false as second parameter.", iLine, iPos);
			return false;
		}
	}

	if (iVarCount > 2)
	{
		if (!mVars(2).CastToCounter(iRotate2))
		{
			rCB.GetErrorList().GeneralError("Expect true or false as third parameter.", iLine, iPos);
			return false;
		}
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	if (iEnable)
	{
		if (iRotate2)
		{
			pScene->EnableAutoRotate2(true);
		}
		else
		{
			pScene->EnableAutoRotate1(true);
		}
	}
	else
	{
		if (iRotate2)
		{
			pScene->EnableAutoRotate2(false);
		}
		else
		{
			pScene->EnableAutoRotate1(false);
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Enable/Disable Auto Scale

bool EnableSceneAutoScaleFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount      = int(mVars.Count());
	TCVCounter iEnable = 1;

	if ((iVarCount < 1) || (iVarCount > 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (iVarCount > 1)
	{
		if (!mVars(1).CastToCounter(iEnable))
		{
			rCB.GetErrorList().GeneralError("Expect true or false as second parameter.", iLine, iPos);
			return false;
		}
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	if (iEnable)
	{
		pScene->EnableAutoScale(true);
	}
	else
	{
		pScene->EnableAutoScale(false);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Enable/Disable Auto Pixel Zoom

bool EnableSceneAutoPixelZoomFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount      = int(mVars.Count());
	TCVCounter iEnable = 1;

	if ((iVarCount < 1) || (iVarCount > 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (iVarCount > 1)
	{
		if (!mVars(1).CastToCounter(iEnable))
		{
			rCB.GetErrorList().GeneralError("Expect true or false as second parameter.", iLine, iPos);
			return false;
		}
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	if (iEnable)
	{
		pScene->EnableAutoPixelZoom(true);
	}
	else
	{
		pScene->EnableAutoPixelZoom(false);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Enable/Disable Auto Scale About Local Origin

bool SetSceneScaleTypeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount != 2)
	{
		int piPar[] = { 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Expect as second parameter the scale type: 'global' or 'local' or 'mouse'.", iLine, iPos);
		return false;
	}

	CStrMem sType = *mVars(1).GetStringPtr();

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	if (sType == "global")
	{
		pScene->EnableAutoScaleAboutLocalOrigin(false);
		pScene->EnableAutoScaleAboutMouseOrigin(false);
	}
	else if (sType == "local")
	{
		pScene->EnableAutoScaleAboutLocalOrigin(true);
		pScene->EnableAutoScaleAboutMouseOrigin(false);
	}
	else if (sType == "mouse")
	{
		pScene->EnableAutoScaleAboutLocalOrigin(true);
		pScene->EnableAutoScaleAboutMouseOrigin(true);
	}
	else
	{
		rCB.GetErrorList().GeneralError("Expect as second parameter the scale type: 'global' or 'local' or 'mouse'.", iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Enable/Disable Normalize Normals

bool EnableSceneNormalizeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount      = int(mVars.Count());
	TCVCounter iEnable = 1;

	if ((iVarCount < 1) || (iVarCount > 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (iVarCount > 1)
	{
		if (!mVars(1).CastToCounter(iEnable))
		{
			rCB.GetErrorList().GeneralError("Expect true or false as second parameter.", iLine, iPos);
			return false;
		}
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	if (iEnable)
	{
		pScene->EnableNormalize(true);
	}
	else
	{
		pScene->EnableNormalize(false);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Enable/Disable Reset Frame

bool EnableSceneResetFrameFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount      = int(mVars.Count());
	TCVCounter iEnable = 1;

	if ((iVarCount < 1) || (iVarCount > 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (iVarCount > 1)
	{
		if (!mVars(1).CastToCounter(iEnable))
		{
			rCB.GetErrorList().GeneralError("Expect true or false as second parameter.", iLine, iPos);
			return false;
		}
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	if (iEnable)
	{
		pScene->EnableResetFrame(true);
	}
	else
	{
		pScene->EnableResetFrame(false);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Enable/Disable Draw Scene

bool EnableSceneFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount      = int(mVars.Count());
	TCVCounter iEnable = 1;

	if ((iVarCount < 1) || (iVarCount > 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (iVarCount > 1)
	{
		if (!mVars(1).CastToCounter(iEnable))
		{
			rCB.GetErrorList().GeneralError("Expect true or false as second parameter.", iLine, iPos);
			return false;
		}
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	pScene->EnableDrawScene(iEnable ? true : false);
	return true;
}

//////////////////////////////////////////////////////////////////////
// Enable/Disable Mouse Dragging
// Parameters:
//	1. The scene
//	2. the mouse button (1: left, 2: right)
//	3. is control pressed (true/false)
//	4. enable (true/false)
//

bool EnableSceneDragFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount      = int(mVars.Count());
	TCVCounter iEnable = 1, iButton, iCtrl;

	if (iVarCount != 4)
	{
		int piPar[] = { 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToCounter(iButton))
	{
		rCB.GetErrorList().GeneralError("Expect as second parameter the mouse button index 1 or 2.", iLine, iPos);
		return false;
	}

	if ((iButton < 1) || (iButton > 2))
	{
		rCB.GetErrorList().GeneralError("The second parameter 'mouse button' has to be 1 or 2.", iLine, iPos);
		return false;
	}

	if (!mVars(2).CastToCounter(iCtrl))
	{
		rCB.GetErrorList().GeneralError("Expect as thrid parameter true/false whether CTRL is pressed.", iLine, iPos);
		return false;
	}

	if (!mVars(3).CastToCounter(iEnable))
	{
		rCB.GetErrorList().GeneralError("Expect as fourth parameter true/false to enable/disable dragging.", iLine, iPos);
		return false;
	}

	int iIdx              = (iButton - 1) * 2 + (iCtrl ? 1 : 0);
	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	if (iEnable)
	{
		pScene->EnableDrag(iIdx, true);
	}
	else
	{
		pScene->EnableDrag(iIdx, false);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Enable/Disable Animate Mouse Dragging
// Parameters:
//	1. The scene
//	2. the mouse button (1: left, 2: right)
//	3. is control pressed (true/false)
//	4. enable (true/false)
//

bool EnableSceneAnimDragFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount      = int(mVars.Count());
	TCVCounter iEnable = 1, iButton, iCtrl;

	if (iVarCount != 4)
	{
		int piPar[] = { 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToCounter(iButton))
	{
		rCB.GetErrorList().GeneralError("Expect as second paramter the mouse button index 1 or 2.", iLine, iPos);
		return false;
	}

	if ((iButton < 1) || (iButton > 2))
	{
		rCB.GetErrorList().GeneralError("The second parameter 'mouse button' has to be 1 or 2.", iLine, iPos);
		return false;
	}

	if (!mVars(2).CastToCounter(iCtrl))
	{
		rCB.GetErrorList().GeneralError("Expect as thrid parameter true/false whether CTRL is pressed.", iLine, iPos);
		return false;
	}

	if (!mVars(3).CastToCounter(iEnable))
	{
		rCB.GetErrorList().GeneralError("Expect as fourth parameter true/false to enable/disable animated dragging.", iLine, iPos);
		return false;
	}

	int iIdx              = (iButton - 1) * 2 + (iCtrl ? 1 : 0);
	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	if (iEnable)
	{
		pScene->EnableAnimDrag(iIdx, true);
	}
	else
	{
		pScene->EnableAnimDrag(iIdx, false);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Enable/Disable Mouse Dragging in screen plane
// Parameters:
//	1. The scene
//	2. the mouse button (1: left, 2: right)
//	3. is control pressed (true/false)
//	4. enable (true/false)
//

bool EnableSceneDragInScreenPlaneFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount      = int(mVars.Count());
	TCVCounter iEnable = 1, iButton, iCtrl;

	if (iVarCount != 4)
	{
		int piPar[] = { 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToCounter(iButton))
	{
		rCB.GetErrorList().GeneralError("Expect as second paramter the mouse button index 1 or 2.", iLine, iPos);
		return false;
	}

	if ((iButton < 1) || (iButton > 2))
	{
		rCB.GetErrorList().GeneralError("The second parameter 'mouse button' has to be 1 or 2.", iLine, iPos);
		return false;
	}

	if (!mVars(2).CastToCounter(iCtrl))
	{
		rCB.GetErrorList().GeneralError("Expect as thrid parameter true/false whether CTRL is pressed.", iLine, iPos);
		return false;
	}

	if (!mVars(3).CastToCounter(iEnable))
	{
		rCB.GetErrorList().GeneralError("Expect as fourth parameter true/false to enable/disable dragging.", iLine, iPos);
		return false;
	}

	int iIdx              = (iButton - 1) * 2 + (iCtrl ? 1 : 0);
	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	if (iEnable)
	{
		pScene->EnableDragInScreenPlane(iIdx, true);
	}
	else
	{
		pScene->EnableDragInScreenPlane(iIdx, false);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Enable/Disable Mouse Dragging in projected basis directions
// Parameters:
//	1. The scene
//	2. the mouse button (1: left, 2: right)
//	3. is control pressed (true/false)
//	4. enable (true/false)
//

bool EnableSceneDragInProjBasisFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount      = int(mVars.Count());
	TCVCounter iEnable = 1, iButton, iCtrl;

	if (iVarCount != 4)
	{
		int piPar[] = { 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToCounter(iButton))
	{
		rCB.GetErrorList().GeneralError("Expect as second paramter the mouse button index 1 or 2.", iLine, iPos);
		return false;
	}

	if ((iButton < 1) || (iButton > 2))
	{
		rCB.GetErrorList().GeneralError("The second parameter 'mouse button' has to be 1 or 2.", iLine, iPos);
		return false;
	}

	if (!mVars(2).CastToCounter(iCtrl))
	{
		rCB.GetErrorList().GeneralError("Expect as thrid parameter true/false whether CTRL is pressed.", iLine, iPos);
		return false;
	}

	if (!mVars(3).CastToCounter(iEnable))
	{
		rCB.GetErrorList().GeneralError("Expect as fourth parameter true/false to enable/disable dragging.", iLine, iPos);
		return false;
	}

	int iIdx              = (iButton - 1) * 2 + (iCtrl ? 1 : 0);
	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	if (iEnable)
	{
		pScene->EnableDragInProjDir(iIdx, true);
	}
	else
	{
		pScene->EnableDragInProjDir(iIdx, false);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Enable/Disable Drag Range
// Parameters:
//	1. The scene
//	2. the mouse button (1: left, 2: right)
//	3. is control pressed (true/false)
//	4. enable (true/false)
//

bool EnableSceneDragRangeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount      = int(mVars.Count());
	TCVCounter iEnable = 1, iButton, iCtrl;

	if (iVarCount != 4)
	{
		int piPar[] = { 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToCounter(iButton))
	{
		rCB.GetErrorList().GeneralError("Expect as second paramter the mouse button index 1 or 2.", iLine, iPos);
		return false;
	}

	if ((iButton < 1) || (iButton > 2))
	{
		rCB.GetErrorList().GeneralError("The second parameter 'mouse button' has to be 1 or 2.", iLine, iPos);
		return false;
	}

	if (!mVars(2).CastToCounter(iCtrl))
	{
		rCB.GetErrorList().GeneralError("Expect as thrid parameter true/false whether CTRL is pressed.", iLine, iPos);
		return false;
	}

	if (!mVars(3).CastToCounter(iEnable))
	{
		rCB.GetErrorList().GeneralError("Expect as fourth parameter true/false to enable/disable drag range clamping.", iLine, iPos);
		return false;
	}

	int iIdx              = (iButton - 1) * 2 + (iCtrl ? 1 : 0);
	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	if (iEnable)
	{
		pScene->EnableDragRange(iIdx, true);
	}
	else
	{
		pScene->EnableDragRange(iIdx, false);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Draw To a given Scene
//
// Parameters:
//	1. The scene to draw to
//	2. clear scene before drawing? (true/false)

bool DrawToSceneFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount > 2)
	{
		int piPar[] = { 0, 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (iVarCount == 0)
	{
		rCB.GetOGLDrawBase()->SetScene(rCB.GetMainSceneRef());
	}
	else
	{
		TCVCounter iClearScene = 0;

		if (mVars(0).BaseType() != PDT_SCENE)
		{
			rCB.GetErrorList().GeneralError("Expect a scene variable as parameter.", iLine, iPos);
			return false;
		}

		if (iVarCount > 1)
		{
			if (!mVars(1).CastToCounter(iClearScene))
			{
				rCB.GetErrorList().GeneralError("Expect optionally as second paramter, whether scene should be cleared before drawing.", iLine, iPos);
				return false;
			}
		}

		COGLBEReference Ref = *mVars(0).GetScenePtr();
		if (!Ref.IsValid())
		{
			rCB.GetErrorList().GeneralError("Given scene is invalid.", iLine, iPos);
			return false;
		}

		COGLScene* pScene = dynamic_cast<COGLScene*>((COGLBaseElement*) Ref);
		if (!pScene)
		{
			rCB.GetErrorList().GeneralError("Given scene is invalid.", iLine, iPos);
			return false;
		}

		if (iClearScene)
		{
			pScene->Reset();
		}

		if (!rCB.GetOGLDrawBase()->SetScene(Ref))
		{
			rCB.GetErrorList().GeneralError("Variable passed does not represent a scene.", iLine, iPos);
			return false;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Scene to be drawn with given scene is picked
//
// Parameters:
//	1. (scene) The scene whose pick scene is to be set
//	2. (scene) The pick scene
//	3. (bool) Single pick scene flag.
//	4. (bool) Drag flag
//	5. (bool) Shift flag
//	6. (bool) Ctrl flag
//	7. (bool) Alt flag
//	8. (string) Type: "standard", "intro", "outro"
//  9. (scalar) Timer for intro and outro scenes
// 10. (scalar) Time factor for intro and outro scenes. default = 1.0

bool SetPickSceneFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount                  = int(mVars.Count());
	TCVScalar dPickTimer           = 0.0;
	TCVScalar dPickTimeFactor      = 1.0;
	COGLScene::EPickType ePickType = COGLScene::PICK_STANDARD;
	TCVCounter iSingle             = 0;
	TCVCounter iShift              = 0, iCtrl = 0, iAlt = 0, iDrag = 0;

	if ((iVarCount < 2) || (iVarCount > 10))
	{
		int piPar[] = { 2, 3, 4, 5, 6, 7, 8, 9, 10 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 9, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a scene whose pick-scene is to be set as first parameter.", iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect the pick-scene as second parameter.", iLine, iPos);
		return false;
	}

	if (iVarCount > 2)
	{
		if (!mVars(2).CastToCounter(iSingle))
		{
			rCB.GetErrorList().GeneralError("Expect true/false as third parameter to indicate whether single pick scene is set.", iLine, iPos);
			return false;
		}
	}

	if (iVarCount > 3)
	{
		if (!mVars(3).CastToCounter(iDrag))
		{
			rCB.GetErrorList().GeneralError("Expect true/false as fourth parameter to give 'drag' flag.", iLine, iPos);
			return false;
		}
	}

	if (iVarCount > 4)
	{
		if (!mVars(4).CastToCounter(iShift))
		{
			rCB.GetErrorList().GeneralError("Expect true/false as fifth parameter to give 'shift' flag.", iLine, iPos);
			return false;
		}
	}

	if (iVarCount > 5)
	{
		if (!mVars(5).CastToCounter(iCtrl))
		{
			rCB.GetErrorList().GeneralError("Expect true/false as sixth parameter to give 'ctrl' flag.", iLine, iPos);
			return false;
		}
	}

	if (iVarCount > 6)
	{
		if (!mVars(6).CastToCounter(iAlt))
		{
			rCB.GetErrorList().GeneralError("Expect true/false as seventh parameter to give 'alt' flag.", iLine, iPos);
			return false;
		}
	}

	if (iVarCount >= 8)
	{
		if (mVars(7).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().GeneralError("Expect pick scene type: [\"standard\"|\"intro\"|\"outro\"].", iLine, iPos);
			return false;
		}

		TString sType = *mVars(7).GetStringPtr();
		sType.ToLowerCase();

		if (sType == "standard")
		{
			ePickType = COGLScene::PICK_STANDARD;
		}
		else if (sType == "intro")
		{
			ePickType = COGLScene::PICK_INTRO;
		}
		else if (sType == "outro")
		{
			ePickType = COGLScene::PICK_OUTRO;
		}
		else
		{
			rCB.GetErrorList().GeneralError("Expect pick scene type: [\"standard\"|\"intro\"|\"outro\"].", iLine, iPos);
			return false;
		}
	}

	if (iVarCount >= 9)
	{
		if (!mVars(8).CastToScalar(dPickTimer))
		{
			rCB.GetErrorList().GeneralError("Expect 9th parameter to give intro/outro timer.", iLine, iPos);
			return false;
		}
	}

	if (iVarCount >= 10)
	{
		if (!mVars(9).CastToScalar(dPickTimeFactor))
		{
			rCB.GetErrorList().GeneralError("Expect 10th parameter to give intro/outro time factor.", iLine, iPos);
			return false;
		}
	}

	COGLBEReference Scene, PickScene;

	Scene     = *mVars(0).GetScenePtr();
	PickScene = *mVars(1).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	if (!PickScene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Pick-Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene;
	pScene = dynamic_cast<COGLScene*>((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene variable does not represent a scene.", iLine, iPos);
		return false;
	}

	pScene->SetPickScene(PickScene, iSingle != 0, iShift != 0, iCtrl != 0, iAlt != 0, iDrag != 0,
			ePickType, dPickTimer, dPickTimeFactor);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Scene whose frame context is used for auto translation and rotation

bool SetSceneFrameContextFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if ((iVarCount < 2) || (iVarCount > 2))
	{
		int piPar[] = { 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a scene whose frame context is to be set.", iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect the scene whose frame context is to be used as second parameter.", iLine, iPos);
		return false;
	}

	COGLBEReference Scene, ContextScene;

	Scene        = *mVars(0).GetScenePtr();
	ContextScene = *mVars(1).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	if (!ContextScene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Frame Context Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene;
	pScene = dynamic_cast<COGLScene*>((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene variable does not represent a scene.", iLine, iPos);
		return false;
	}

	pScene->SetFrameContextScene(ContextScene);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Auto Adjust Frame

bool SetSceneAutoAdjustFrameFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 2)
	{
		int piPar[] = { 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a scene whose pick-scene is to be set as first parameter.", iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a frame as second parameter.", iLine, iPos);
		return false;
	}

	COGLBEReference Scene, Frame;

	Scene = *mVars(0).GetScenePtr();
	Frame = *mVars(1).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	if (!Frame.IsValid())
	{
		rCB.GetErrorList().GeneralError("Frame is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene;
	pScene = dynamic_cast<COGLScene*>((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene variable does not represent a scene.", iLine, iPos);
		return false;
	}

	pScene->SetAutoAdjustFrame(Frame);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Remove Scene to be drawn when given scene is picked

bool ClearPickSceneFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 1)
	{
		int piPar[] = { 1 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a scene whose pick-scene is to be cleared.", iLine, iPos);
		return false;
	}

	COGLBEReference Scene;

	Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene;
	pScene = dynamic_cast<COGLScene*>((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene variable does not represent a scene.", iLine, iPos);
		return false;
	}

	pScene->ClearPickScene();

	return true;
}

//////////////////////////////////////////////////////////////////////
// Clear a given Scene

bool ClearSceneFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 1)
	{
		int piPar[] = { 1 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a scene variable as parameter.", iLine, iPos);
		return false;
	}

	COGLBaseElementList* pList;

	pList = dynamic_cast< COGLBaseElementList*>((COGLBaseElement*) *mVars(0).GetScenePtr());

	if (!pList)
	{
		rCB.GetErrorList().GeneralError("Variable passed does not represent a scene.", iLine, iPos);
		return false;
	}

	pList->Reset();

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Local Time

bool SetSceneLocalTimeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount        = int(mVars.Count());
	TCVScalar dLocalTime = 0.0;

	if (iVarCount != 2)
	{
		int piPar[] = { 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToScalar(dLocalTime))
	{
		rCB.GetErrorList().GeneralError("Expect a scalar value giving the local scene time.", iLine, iPos);
		return false;
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	pScene->SetLocalTime(dLocalTime);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Local Time Scene reference
//
// Parameters:
//	1. (scene) The scene whose local time reference is to be set
//	2. (opt) (scene) The local time reference scene
//		if this parameter is not given the local time reference is cleared.

bool SetSceneLocalTimeRefFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if ((iVarCount != 1) && (iVarCount != 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a scene whose local time reference scene is to be set as first parameter.", iLine, iPos);
		return false;
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	if (iVarCount >= 2)
	{
		if (mVars(1).BaseType() != PDT_SCENE)
		{
			rCB.GetErrorList().GeneralError("Expect the local time reference scene as second parameter.", iLine, iPos);
			return false;
		}

		COGLBEReference LTScene = *mVars(1).GetScenePtr();

		if (!LTScene.IsValid())
		{
			rCB.GetErrorList().GeneralError("Local time scene is not valid.", iLine, iPos);
			return false;
		}

		pScene->SetLocalTimeSceneRef(LTScene);
	}
	else
	{
		pScene->ClearLocalTimeSceneRef();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Scale Function

bool SetSceneScaleFuncFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	TString csFunc;

	if (iVarCount != 2)
	{
		int piPar[] = { 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Expect a string specifying the scale function as second parameter.", iLine, iPos);
		return false;
	}

	csFunc = *mVars(1).GetStringPtr();

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	if (csFunc == "linear")
	{
		pScene->SetScaleFunc(COGLScene::/*EScaleFunc::*/ LINEAR);
	}
	else if (csFunc == "exp")
	{
		pScene->SetScaleFunc(COGLScene::/*EScaleFunc::*/ EXP);
	}
	else
	{
		rCB.GetErrorList().GeneralError("Unknown scale function.", iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Get Scene Drag Data
// Parameters:
//	1. The scene
//	2. the mouse button (1: left, 2: right)
//	3. is control pressed (true/false)
//
// Return:
//		list of 3 data values

bool GetSceneDragDataFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVCounter iButton, iCtrl;

	if (iVarCount != 3)
	{
		int piPar[] = { 3 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToCounter(iButton))
	{
		rCB.GetErrorList().GeneralError("Expect as second paramter the mouse button index 1 or 2.", iLine, iPos);
		return false;
	}

	if ((iButton < 1) || (iButton > 2))
	{
		rCB.GetErrorList().GeneralError("The second parameter 'mouse button' has to be 1 or 2.", iLine, iPos);
		return false;
	}

	if (!mVars(2).CastToCounter(iCtrl))
	{
		rCB.GetErrorList().GeneralError("Expect as third parameter true/false whether CTRL is pressed.", iLine, iPos);
		return false;
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	float fX, fY, fZ;
	int iIdx = (iButton - 1) * 2 + (iCtrl ? 1 : 0);

	pScene->GetDragData(iIdx, fX, fY, fZ);

	rVar.New(PDT_VARLIST);
	TVarList& rList = *rVar.GetVarListPtr();

	rList.Add(3);
	rList(0) = TCVScalar(fX);
	rList(1) = TCVScalar(fY);
	rList(2) = TCVScalar(fZ);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Scene Drag Data
// Parameters:
//	1. The scene
//	2. the mouse button (1: left, 2: right)
//	3. is control pressed (true/false)
//	4. List of three scalar values
//
// Return:
//		nothing

bool SetSceneDragDataFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVCounter iButton, iCtrl;

	if (iVarCount != 4)
	{
		int piPar[] = { 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToCounter(iButton))
	{
		rCB.GetErrorList().GeneralError("Expect as second paramter the mouse button index 1 or 2.", iLine, iPos);
		return false;
	}

	if ((iButton < 1) || (iButton > 2))
	{
		rCB.GetErrorList().GeneralError("The second parameter 'mouse button' has to be 1 or 2.", iLine, iPos);
		return false;
	}

	if (!mVars(2).CastToCounter(iCtrl))
	{
		rCB.GetErrorList().GeneralError("Expect as third parameter true/false whether CTRL is pressed.", iLine, iPos);
		return false;
	}

	if (mVars(3).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().GeneralError("Expect as fourth parameter a list of three scalar values.", iLine, iPos);
		return false;
	}

	TVarList& rList = *mVars(3).GetVarListPtr();
	if (rList.Count() != 3)
	{
		rCB.GetErrorList().GeneralError("Expect as fourth parameter a list of exactly three scalar values.", iLine, iPos);
		return false;
	}

	TCVScalar pdVal[3];
	int i;

	for (i = 0; i < 3; i++)
	{
		if (!rList(i).CastToScalar(pdVal[i], rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Element in list is not a scalar.", iLine, iPos);
			return false;
		}
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	int iIdx = (iButton - 1) * 2 + (iCtrl ? 1 : 0);

	pScene->SetDragData(iIdx, float(pdVal[0]), float(pdVal[1]), float(pdVal[2]));

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Scene Animated Drag Attenuation
// Parameters:
//	1. The scene
//	2. the mouse button (1: left, 2: right)
//	3. is control pressed (true/false)
//	4. List of three scalar values
//
// Return:
//		nothing

bool SetSceneAnimDragAttenuationFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVCounter iButton, iCtrl;

	if (iVarCount != 4)
	{
		int piPar[] = { 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToCounter(iButton))
	{
		rCB.GetErrorList().GeneralError("Expect as second paramter the mouse button index 1 or 2.", iLine, iPos);
		return false;
	}

	if ((iButton < 1) || (iButton > 2))
	{
		rCB.GetErrorList().GeneralError("The second parameter 'mouse button' has to be 1 or 2.", iLine, iPos);
		return false;
	}

	if (!mVars(2).CastToCounter(iCtrl))
	{
		rCB.GetErrorList().GeneralError("Expect as third parameter true/false whether CTRL is pressed.", iLine, iPos);
		return false;
	}

	if (mVars(3).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().GeneralError("Expect as fourth parameter a list of three scalar values.", iLine, iPos);
		return false;
	}

	TVarList& rList = *mVars(3).GetVarListPtr();
	if (rList.Count() != 3)
	{
		rCB.GetErrorList().GeneralError("Expect as fourth parameter a list of exactly three scalar values.", iLine, iPos);
		return false;
	}

	TCVScalar pdVal[3];
	int i;

	for (i = 0; i < 3; i++)
	{
		if (!rList(i).CastToScalar(pdVal[i], rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Element in list is not a scalar.", iLine, iPos);
			return false;
		}

		if (pdVal[i] <  0.0)
		{
			pdVal[i] = 0.0;
		}
		else if (pdVal[i] > 1.0)
		{
			pdVal[i] = 1.0;
		}
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	int iIdx = (iButton - 1) * 2 + (iCtrl ? 1 : 0);

	pScene->SetAnimDragAttenuation(iIdx, float(pdVal[0]), float(pdVal[1]), float(pdVal[2]));

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Scene Drag Data Range
// Parameters:
//	1. The scene
//	2. the mouse button (1: left, 2: right)
//	3. is control pressed (true/false)
//	4. List of three lists of pairs of scalar values
//
// Return:
//		nothing

bool SetSceneDragRangeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVCounter iButton, iCtrl;

	if (iVarCount != 4)
	{
		int piPar[] = { 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToCounter(iButton))
	{
		rCB.GetErrorList().GeneralError("Expect as second paramter the mouse button index 1 or 2.", iLine, iPos);
		return false;
	}

	if ((iButton < 1) || (iButton > 2))
	{
		rCB.GetErrorList().GeneralError("The second parameter 'mouse button' has to be 1 or 2.", iLine, iPos);
		return false;
	}

	if (!mVars(2).CastToCounter(iCtrl))
	{
		rCB.GetErrorList().GeneralError("Expect as third parameter true/false whether CTRL is pressed.", iLine, iPos);
		return false;
	}

	if (mVars(3).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().GeneralError("Expect as fourth parameter a list of three scalar values.", iLine, iPos);
		return false;
	}

	TVarList& rList = *mVars(3).GetVarListPtr();
	if (rList.Count() != 3)
	{
		rCB.GetErrorList().GeneralError("Expect as fourth parameter a list of three lists of pairs of scalar values.", iLine, iPos);
		return false;
	}

	TCVScalar pdVal[3][2];
	int iPair, iVal;

	for (iPair = 0; iPair < 3; ++iPair)
	{
		if (rList(iPair).BaseType() != PDT_VARLIST)
		{
			rCB.GetErrorList().GeneralError("Expect as fourth parameter a list of three lists of pairs of scalar values.", iLine, iPos);
			return false;
		}

		TVarList& rPair = *rList(iPair).GetVarListPtr();
		if (rPair.Count() != 2)
		{
			rCB.GetErrorList().GeneralError("Expect as fourth parameter a list of three lists of pairs of scalar values.", iLine, iPos);
			return false;
		}

		for (iVal = 0; iVal < 2; ++iVal)
		{
			if (!rPair(iVal).CastToScalar(pdVal[iPair][iVal], rCB.GetSensitivity()))
			{
				rCB.GetErrorList().GeneralError("Element in list is not a scalar.", iLine, iPos);
				return false;
			}
		}
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	int iIdx = (iButton - 1) * 2 + (iCtrl ? 1 : 0);

	pScene->SetDragRange(iIdx,
			float(pdVal[0][0]), float(pdVal[0][1]),
			float(pdVal[1][0]), float(pdVal[1][1]),
			float(pdVal[2][0]), float(pdVal[2][1]));

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Scene Auto Drag Control
// Parameters:
//	1. The scene
//	2. List of four lists of pairs of values indicating mouse control
//		for auto scale, auto rotate1, auto translate, auto rotate2.
//		[ [ (mouse button: 0|1), (control pressed: true|false) ], [...], [...], [...] ]
//
// Return:
//		nothing

bool SetSceneAutoDragControlFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
//	TCVCounter iButton, iCtrl;

	if (iVarCount != 2)
	{
		int piPar[] = { 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().GeneralError("Expect as second parameter a list of four sublists specifying mouse control modes.", iLine, iPos);
		return false;
	}

	TVarList& rList = *mVars(1).GetVarListPtr();
	if (rList.Count() != 4)
	{
		rCB.GetErrorList().GeneralError("Expect as second parameter a list of four lists of pairs of values.", iLine, iPos);
		return false;
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	TCVCounter piVal[2];
	int iPair, iVal;

	for (iPair = 0; iPair < 4; ++iPair)
	{
		if (rList(iPair).BaseType() != PDT_VARLIST)
		{
			rCB.GetErrorList().GeneralError("Expect as second parameter a list of four lists of pairs of values.", iLine, iPos);
			return false;
		}

		TVarList& rPair = *rList(iPair).GetVarListPtr();
		if (rPair.Count() != 2)
		{
			rCB.GetErrorList().GeneralError("Expect as second parameter a list of four lists of pairs of values.", iLine, iPos);
			return false;
		}

		for (iVal = 0; iVal < 2; ++iVal)
		{
			if (!rPair(iVal).CastToCounter(piVal[iVal], false))
			{
				if (iVal == 0)
				{
					rCB.GetErrorList().GeneralError("Expect as first sublist paramter the mouse button index 1 or 2.", iLine, iPos);
				}
				else
				{
					rCB.GetErrorList().GeneralError("Expect as second sublist parameter true/false whether CTRL is pressed.", iLine, iPos);
				}

				return false;
			}
		}

		if ((piVal[0] < 1) || (piVal[0] > 2))
		{
			rCB.GetErrorList().GeneralError("The second sublist parameter 'mouse button' has to be 1 or 2.", iLine, iPos);
			return false;
		}

		int iIdx = (piVal[0] - 1) * 2 + (piVal[1] ? 1 : 0);

		if (iPair == 0)
		{
			pScene->SetDragIdxScale(iIdx);
		}
		else if (iPair == 1)
		{
			pScene->SetDragIdxRotate1(iIdx);
		}
		else if (iPair == 2)
		{
			pScene->SetDragIdxTranslate(iIdx);
		}
		else
		{
			pScene->SetDragIdxRotate2(iIdx);
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Get Scene Drag Factor
// Parameters:
//	1. The scene
//	2. the mouse button (1: left, 2: right)
//	3. is control pressed (true/false)
//
// Return:
//		list of 3 factor values

bool GetSceneDragFactorFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVCounter iButton, iCtrl;

	if (iVarCount != 3)
	{
		int piPar[] = { 3 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToCounter(iButton))
	{
		rCB.GetErrorList().GeneralError("Expect as second paramter the mouse button index 1 or 2.", iLine, iPos);
		return false;
	}

	if ((iButton < 1) || (iButton > 2))
	{
		rCB.GetErrorList().GeneralError("The second parameter 'mouse button' has to be 1 or 2.", iLine, iPos);
		return false;
	}

	if (!mVars(2).CastToCounter(iCtrl))
	{
		rCB.GetErrorList().GeneralError("Expect as third parameter true/false whether CTRL is pressed.", iLine, iPos);
		return false;
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	float fX, fY, fZ;
	int iIdx = (iButton - 1) * 2 + (iCtrl ? 1 : 0);

	pScene->GetDragFactor(iIdx, fX, fY, fZ);

	rVar.New(PDT_VARLIST);
	TVarList& rList = *rVar.GetVarListPtr();

	rList.Add(3);
	rList(0) = TCVScalar(fX);
	rList(1) = TCVScalar(fY);
	rList(2) = TCVScalar(fZ);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Scene Drag Factor
// Parameters:
//	1. The scene
//	2. the mouse button (1: left, 2: right)
//	3. is control pressed (true/false)
//	4. List of three scalar values
//
// Return:
//		nothing

bool SetSceneDragFactorFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVCounter iButton, iCtrl;

	if (iVarCount != 4)
	{
		int piPar[] = { 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToCounter(iButton))
	{
		rCB.GetErrorList().GeneralError("Expect as second paramter the mouse button index 1 or 2.", iLine, iPos);
		return false;
	}

	if ((iButton < 1) || (iButton > 2))
	{
		rCB.GetErrorList().GeneralError("The second parameter 'mouse button' has to be 1 or 2.", iLine, iPos);
		return false;
	}

	if (!mVars(2).CastToCounter(iCtrl))
	{
		rCB.GetErrorList().GeneralError("Expect as third parameter true/false whether CTRL is pressed.", iLine, iPos);
		return false;
	}

	if (mVars(3).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().GeneralError("Expect as fourth parameter a list of three scalar values.", iLine, iPos);
		return false;
	}

	TVarList& rList = *mVars(3).GetVarListPtr();
	if (rList.Count() != 3)
	{
		rCB.GetErrorList().GeneralError("Expect as fourth parameter a list of exactly three scalar values.", iLine, iPos);
		return false;
	}

	TCVScalar pdVal[3];
	int i;

	for (i = 0; i < 3; i++)
	{
		if (!rList(i).CastToScalar(pdVal[i], rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Element in list is not a scalar.", iLine, iPos);
			return false;
		}
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	int iIdx = (iButton - 1) * 2 + (iCtrl ? 1 : 0);

	pScene->SetDragFactor(iIdx, float(pdVal[0]), float(pdVal[1]), float(pdVal[2]));

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set drag basis for scene
// Parameters:
//	1. The scene
//	2. the mouse button (1: left, 2: right)
//	3. is control pressed (true/false)
//	4. List of three vectors
//
// Return:
//		nothing

bool SetSceneDragBasisFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	int iButton, iCtrl;

	if (iVarCount != 4)
	{
		int piPar[] = { 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToCounter(iButton))
	{
		rCB.GetErrorList().GeneralError("Expect as second paramter the mouse button index 1 or 2.", iLine, iPos);
		return false;
	}

	if ((iButton < 1) || (iButton > 2))
	{
		rCB.GetErrorList().GeneralError("The second parameter 'mouse button' has to be 1 or 2.", iLine, iPos);
		return false;
	}

	if (!mVars(2).CastToCounter(iCtrl))
	{
		rCB.GetErrorList().GeneralError("Expect as third parameter true/false whether CTRL is pressed.", iLine, iPos);
		return false;
	}

	if (mVars(3).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().GeneralError("Expect as fourth parameter a list of three vectors.", iLine, iPos);
		return false;
	}

	TVarList& rList = *mVars(3).GetVarListPtr();
	if (rList.Count() != 3)
	{
		rCB.GetErrorList().GeneralError("Expect as fourth parameter a list of exactly three vectors defining the basis.", iLine, iPos);
		return false;
	}

	int iIdx;
	TMultiV mvBase;
	COGLVertex pvexBasis[3];

	for (iIdx = 0; iIdx < 3; ++iIdx)
	{
		if (rList(iIdx).BaseType() != PDT_MULTIV)
		{
			char pcText[400];
			sprintf_s(pcText, "Element %d in basis vector list is not a multivector.", iIdx + 1);
			rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
			return false;
		}

		TMultiV& rBase = *rList(iIdx).GetMultiVPtr();

		if (!rCB.CastMVtoE3(rBase, mvBase))
		{
			char pcText[400];
			sprintf_s(pcText, "Element %d in basis vector list is not a vector.", iIdx + 1);
			rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
			return false;
		}

		pvexBasis[iIdx].Set(float(mvBase[1]), float(mvBase[2]), float(mvBase[3]));
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	iIdx = (iButton - 1) * 2 + (iCtrl ? 1 : 0);

	pScene->SetDragBasis(iIdx, pvexBasis[0], pvexBasis[1], pvexBasis[2]);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Perspective Projection
// Parameters:
//	1. The scene
//	2. The view angle
//	3. (opt) double: Near
//  4. (opt) double: Far
// Return:
//		nothing

bool SetSceneViewAngleFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount    = int(mVars.Count());
	TCVScalar dAngle = 0, dNear = 0.1, dFar = 100.0;

	if ((iVarCount != 1) && (iVarCount != 2) && (iVarCount != 4))
	{
		int piPar[] = { 1, 2, 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 3, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (iVarCount >= 2)
	{
		if (!mVars(1).CastToScalar(dAngle))
		{
			rCB.GetErrorList().GeneralError("Expect as second paramter a view angle", iLine, iPos);
			return false;
		}

		if ((dAngle < 1.0) || (dAngle > 179.0))
		{
			rCB.GetErrorList().GeneralError("View angle has to be in range 1 to 179.", iLine, iPos);
			return false;
		}
	}

	if (iVarCount >= 3)
	{
		if (!mVars(2).CastToScalar(dNear))
		{
			rCB.GetErrorList().GeneralError("Expect as third paramter the near value.", iLine, iPos);
			return false;
		}

		if (!mVars(3).CastToScalar(dFar))
		{
			rCB.GetErrorList().GeneralError("Expect as fourth paramter the far value.", iLine, iPos);
			return false;
		}
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	if (iVarCount > 1)
	{
		pScene->EnableLocalProj(true);
		pScene->SetProjType(COGLScene::/*EProjType::*/ CENTRAL);
		pScene->SetProj(float(dAngle), 1.0f, float(dNear), float(dFar));
	}
	else
	{
		pScene->EnableLocalProj(false);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Perspective Projection Volume
// Parameters:
//	1. The scene
//	2. double: left
//	3. double: right
//	4. double: bottom
//	5. double: top
//	6. double: near
//	7. double: far
//
// Return:
//		nothing

bool SetSceneViewVolumeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVScalar pdVal[6];

	if ((iVarCount != 1) && (iVarCount != 7))
	{
		int piPar[] = { 1, 7 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (iVarCount >= 2)
	{
		int i;
		for (i = 0; i < 6; ++i)
		{
			if (!mVars(1 + i).CastToScalar(pdVal[i]))
			{
				rCB.GetErrorList().GeneralError("Parameters two to seven have to be scalars.", iLine, iPos);
				return false;
			}
		}
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	if (iVarCount > 1)
	{
		pScene->EnableLocalProj(true);
		pScene->SetProjType(COGLScene::/*EProjType::*/ CENTRAL);
		pScene->SetProjVolume(float(pdVal[0]), float(pdVal[1]),
				float(pdVal[2]), float(pdVal[3]),
				float(pdVal[4]), float(pdVal[5]));
	}
	else
	{
		pScene->EnableLocalProj(false);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Orthographic Projection
// Parameters:
//	1. The scene
//	2. Left
//	3. Right
//  4. Bottom
//	5. Top
//	6. Near, distance (!) from viewpoint to near plane
//	7. Far, distance (!) from viewpoint to far plane
//	8. (opt) bool: true, adapt aspect ratio to viewport; false (default), fixed aspect ratio.
//
//
// Return:
//		nothing
//
// While Near, Far give the distances from the viepoint to the near and far clipping planes,
// the z-axis points from far to near.

bool SetSceneOrthoProjFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVScalar pdVal[6];
	TCVCounter iVarAspectRatio = 0;

	if ((iVarCount != 1) && (iVarCount != 7) && (iVarCount != 8))
	{
		int piPar[] = { 1, 7, 8 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 3, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (iVarCount >= 7)
	{
		int i;
		for (i = 0; i < 6; i++)
		{
			if (!mVars(i + 1).CastToScalar(pdVal[i]))
			{
				rCB.GetErrorList().GeneralError("Orthographic projection parameters have to be scalars.", iLine, iPos);
				return false;
			}
		}
	}

	if (iVarCount >= 8)
	{
		if (!mVars(7).CastToCounter(iVarAspectRatio))
		{
			rCB.GetErrorList().GeneralError("8th parameter has to be 'bool' indication whether aspect ratio is variable.", iLine, iPos);
			return false;
		}
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	if (iVarCount > 1)
	{
		pScene->EnableLocalProj(true);
		pScene->SetProjType(COGLScene::/*EProjType::*/ ORTHO);
		pScene->EnableOrthoViewportAspectRatio((iVarAspectRatio != 0));
		pScene->SetOrthoProj(float(pdVal[0]), float(pdVal[1]),
				float(pdVal[2]), float(pdVal[3]),
				float(pdVal[4]), float(pdVal[5]));
	}
	else
	{
		pScene->EnableLocalProj(false);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Pixel Orthographic Projection
// Parameters:
//	1. The scene
//
// Return:
//		nothing

bool SetScenePixelOrthoProjFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount != 1)
	{
		int piPar[] = { 1 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a scene variable as parameter.", iLine, iPos);
		return false;
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	pScene->EnableLocalProj(true);
	pScene->SetProjType(COGLScene::/*EProjType::*/ PIXEL);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Viewport
// Parameters:
//	1. The scene
//	2. Left
//	3. Bottom
//  4. Width
//	5. Height
//	6. (opt) is view pickable independent of whether something is drawn?
//
//	If parameters are > 0 and <= 1 they denote percentage values.
//	Otherwise they are absolute pixel values.
//
// Return:
//		nothing

bool SetSceneViewportFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVScalar pdVal[4];
	TCVCounter iPickable = 0;
	bool bRel[4];

	if ((iVarCount != 1) && (iVarCount != 5) &&
	    (iVarCount != 6))
	{
		int piPar[] = { 1, 5, 6 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 3, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (iVarCount >= 2)
	{
		int i;
		for (i = 0; i < 4; i++)
		{
			if (!mVars(i + 1).CastToScalar(pdVal[i]))
			{
				rCB.GetErrorList().GeneralError("View parameters have to be scalars.", iLine, iPos);
				return false;
			}

			if (pdVal[i] >= 1.0001)
			{
				bRel[i] = false;
			}
			else
			{
				bRel[i] = true;
			}
		}
	}

	if (iVarCount >= 6)
	{
		if (!mVars(5).CastToCounter(iPickable))
		{
			rCB.GetErrorList().GeneralError("Sixth parameter has to be a boolean indicating whether view area is pickable.", iLine, iPos);
			return false;
		}
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	if (iVarCount > 1)
	{
		if ((pdVal[2] < 0.0) || (pdVal[3] < 0.0))
		{
			rCB.GetErrorList().GeneralError("Width and height must not be negative.", iLine, iPos);
			return false;
		}

		pScene->EnableLocalView(true);
		if (iPickable)
		{
			pScene->EnablePickableView(true);
			pScene->EnablePick(true);
		}
		else
		{
			pScene->EnablePickableView(false);
		}

		pScene->SetView(float(pdVal[0]), float(pdVal[1]),
				float(pdVal[2]), float(pdVal[3]),
				bRel[0], bRel[1], bRel[2], bRel[3]);
	}
	else
	{
		pScene->EnableLocalView(false);
		pScene->EnablePickableView(false);
	}

	return true;
}

bool GetSceneViewportSizeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	// We expect the to have 1 parameter: The scene the size we want to retrieve
	if (iVarCount != 1)
	{
		int piPar[] = { 1 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	// check if param is a scene object
	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a scene variable as parameter.", iLine, iPos);
		return false;
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast<COGLScene*>((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	// Create a new parameter list
	rVar.New(PDT_VARLIST);
	TVarList& rVarList = *rVar.GetVarListPtr();
	rVarList.Set(2);

	// retrieve the viewport size
	int iW, iH;
	pScene->GetViewportSize(iW, iH);

	// store the results
	rVarList(0) = iW;
	rVarList(1) = iH;

	return true;
}

//////////////////////////////////////////////////////////////////////
// Enable/Disable viewport scissor

bool EnableSceneViewportScissorFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount      = int(mVars.Count());
	TCVCounter iEnable = 1;

	if ((iVarCount < 1) || (iVarCount > 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (iVarCount > 1)
	{
		if (!mVars(1).CastToCounter(iEnable))
		{
			rCB.GetErrorList().GeneralError("Expect true or false as second parameter.", iLine, iPos);
			return false;
		}
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	if (iEnable)
	{
		pScene->EnableViewScissor(true);
	}
	else
	{
		pScene->EnableViewScissor(false);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Viewport Origin Reference
// Parameters:
//	1. The scene
//	2. The scene whose viewport is to be used as origin
//	3. bool: true/false indicates left/right coordinate of viewport
//  4. bool: true/false indicates bottom/top coordinate of viewport
//
// Return:
//		nothing

bool SetSceneViewportOriginRefFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount    = int(mVars.Count());
	TCVCounter iLeft = 1, iBottom = 1;

	if ((iVarCount != 1) && (iVarCount != 4))
	{
		int piPar[] = { 1, 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a scene variable as parameter.", iLine, iPos);
		return false;
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	if (iVarCount == 1)
	{
		pScene->SetViewOriginRef(COGLBEReference(), true, true);
	}
	else
	{
		if (mVars(1).BaseType() != PDT_SCENE)
		{
			rCB.GetErrorList().GeneralError("Expect as second parameter the scene whose viewport is to be used as origin.", iLine, iPos);
			return false;
		}

		COGLBEReference SceneOrigin = *mVars(1).GetScenePtr();

		if (!SceneOrigin.IsValid())
		{
			rCB.GetErrorList().GeneralError("Origin scene is not valid.", iLine, iPos);
			return false;
		}

		COGLScene* pSceneOrigin = dynamic_cast< COGLScene* >((COGLBaseElement*) SceneOrigin);
		if (!pSceneOrigin)
		{
			rCB.GetErrorList().GeneralError("Origin scene is not valid.", iLine, iPos);
			return false;
		}

		if (!mVars(2).CastToCounter(iLeft))
		{
			rCB.GetErrorList().GeneralError("Third parameter has to be a boolean indicating whether left or right end of viewport is origin.", iLine, iPos);
			return false;
		}

		if (!mVars(3).CastToCounter(iBottom))
		{
			rCB.GetErrorList().GeneralError("Fourth parameter has to be a boolean indicating whether bottom or top end of viewport is origin.", iLine, iPos);
			return false;
		}

		pScene->SetViewOriginRef(SceneOrigin, iLeft != 0, iBottom != 0);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Viewport Local
// Parameters:
//	1. The scene
//  2. true/false: viewport origin coordinates are given relative to local coordinate system
//	3. true/false: viewport extent coordinates are given relative to local coordinate system.
//  4. (opt) double: depth at which viewport extents are estimated.
//
// Return:
//		nothing

bool SetSceneViewportLocalFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount           = int(mVars.Count());
	TCVScalar dDepth        = 0.0;
	TCVCounter iLocalOrigin = 0, iLocalExtent = 0;

	if ((iVarCount != 3) && (iVarCount != 4))
	{
		int piPar[] = { 3, 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (iVarCount >= 3)
	{
		if (!mVars(1).CastToCounter(iLocalOrigin))
		{
			rCB.GetErrorList().GeneralError("Second parameter has to be a boolean indicating whether viewport origin coordinates are given in local coordinate system.", iLine, iPos);
			return false;
		}

		if (!mVars(2).CastToCounter(iLocalExtent))
		{
			rCB.GetErrorList().GeneralError("Third parameter has to be a boolean indicating whether viewport extent coordinates are given in local coordinate system.", iLine, iPos);
			return false;
		}
	}

	if (iVarCount >= 4)
	{
		if (!mVars(3).CastToScalar(dDepth))
		{
			rCB.GetErrorList().GeneralError("Fourth parameter has to give z-coordinate in local coordinate system.", iLine, iPos);
			return false;
		}
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	pScene->SetViewLocal(iLocalOrigin != 0, iLocalExtent != 0, float(dDepth));

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Viewport Align
// Parameters:
//	1. The scene
//  2.-3. double: alignment of viewport relative to given coordinates.
//
// Return:
//		nothing

bool SetSceneViewportAlignFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount     = int(mVars.Count());
	TCVScalar dAlignX = 0.0, dAlignY = 0.0;

	if (iVarCount != 3)
	{
		int piPar[] = { 3 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToScalar(dAlignX))
	{
		rCB.GetErrorList().GeneralError("Second parameter has to give viewport coordinate alignment in x-direction.", iLine, iPos);
		return false;
	}

	if (!mVars(2).CastToScalar(dAlignY))
	{
		rCB.GetErrorList().GeneralError("Third parameter has to give viewport coordinate alignment in y-direction.", iLine, iPos);
		return false;
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	pScene->SetViewAlign(float(dAlignX), float(dAlignY));

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Viewport Border
// Parameters:
//	1. The scene
//	2.-5. double: border (left, bottom, right, top) in pixel
//
//
// Return:
//		nothing

bool SetSceneViewportBorderFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVCounter piBorder[4];

	if (iVarCount != 5)
	{
		int piPar[] = { 5 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a scene variable as parameter.", iLine, iPos);
		return false;
	}

	int i;
	for (i = 0; i < 4; ++i)
	{
		if (!mVars(1 + i).CastToCounter(piBorder[i]))
		{
			rCB.GetErrorList().GeneralError("Parameters 2 to 5 have to give the border in pixel.", iLine, iPos);
			return false;
		}
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	pScene->SetViewBorder(piBorder[0], piBorder[2], piBorder[1], piBorder[3]);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Scene Repeat Count
// Parameters:
//	1. The scene
//	2. Repeat Count
//
// Return:
//		nothing

bool SetSceneRepeatCountFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVCounter iCount;

	if (iVarCount != 2)
	{
		int piPar[] = { 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToCounter(iCount))
	{
		rCB.GetErrorList().GeneralError("Second parameter has to be a counter giving the number of times the scene is repeatedly drawn.", iLine, iPos);
		return false;
	}

	if (iCount < 0)
	{
		rCB.GetErrorList().GeneralError("Repeat count has to be greater or equal to zero.", iLine, iPos);
		return false;
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	pScene->SetRepeatCount(iCount);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Scene Depth Level
// Parameters:
//	1. The scene
//	2. The depth level [-10,10]
//
// Return:
//		nothing

bool SetSceneDepthLevelFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVScalar dLevel;

	if (iVarCount != 2)
	{
		int piPar[] = { 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToScalar(dLevel))
	{
		rCB.GetErrorList().GeneralError("Second parameter has to be the depth level in the range [-10,10].", iLine, iPos);
		return false;
	}

	if ((dLevel < -10.0) || (dLevel > 10.0))
	{
		rCB.GetErrorList().GeneralError("The depth level has to be in the range [-10,10].", iLine, iPos);
		return false;
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	pScene->SetDepthLevel(dLevel);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Scene Depth Level Type
// Parameters:
//	1. The scene
//	2. The depth level type: "relative", "absolute", "local"
//
// Return:
//		nothing

bool SetSceneDepthLevelTypeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TString csDLType;

	if (iVarCount != 2)
	{
		int piPar[] = { 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Second parameter has to be the depth level type [\"relative\", \"absolute\", \"local\"].", iLine, iPos);
		return false;
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	csDLType = *mVars(1).GetStringPtr();

	if (csDLType == "relative")
	{
		pScene->SetDepthLevelType(COGLScene::SViewport::DL_Relative);
	}
	else if (csDLType == "absolute")
	{
		pScene->SetDepthLevelType(COGLScene::SViewport::DL_Absolute);
	}
	else if (csDLType == "local")
	{
		pScene->SetDepthLevelType(COGLScene::SViewport::DL_Local);
	}
	else
	{
		rCB.GetErrorList().GeneralError("The depth level type has to be one of: [\"relative\", \"absolute\", \"local\"].", iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Scene Depth Level Local Range
// Parameters:
//	1. The scene
//	2. Min local depth value
//  3. Max local depth value
//  4. Min depth level
//  5. Max depth level
//
// Return:
//		nothing

bool SetSceneDepthLevelLocalRangeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVScalar dLocalNear, dLocalFar, dLevelNear, dLevelFar;

	if (iVarCount != 5)
	{
		int piPar[] = { 5 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a scene variable as parameter.", iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToScalar(dLocalNear))
	{
		rCB.GetErrorList().GeneralError("Second parameter has to be the near local depth value.", iLine, iPos);
		return false;
	}

	if (!mVars(2).CastToScalar(dLocalFar))
	{
		rCB.GetErrorList().GeneralError("Third parameter has to be the far local depth value.", iLine, iPos);
		return false;
	}

	if (!mVars(3).CastToScalar(dLevelNear))
	{
		rCB.GetErrorList().GeneralError("Fourth parameter has to be the near depth level.", iLine, iPos);
		return false;
	}

	if ((dLevelNear < -10.0) || (dLevelNear > 10.0))
	{
		rCB.GetErrorList().GeneralError("The near depth level has to be in the range [-10,10].", iLine, iPos);
		return false;
	}

	if (!mVars(4).CastToScalar(dLevelFar))
	{
		rCB.GetErrorList().GeneralError("Fifth parameter has to be the far depth level.", iLine, iPos);
		return false;
	}

	if ((dLevelFar < -10.0) || (dLevelFar > 10.0))
	{
		rCB.GetErrorList().GeneralError("The far depth level has to be in the range [-10,10].", iLine, iPos);
		return false;
	}

	if (dLevelFar > dLevelNear - 0.01)
	{
		rCB.GetErrorList().GeneralError("The far depth level has to be a smaller value than the near depth level by at least 0.01.", iLine, iPos);
		return false;
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Scene);
	if (!pScene)
	{
		rCB.GetErrorList().GeneralError("Scene is not valid.", iLine, iPos);
		return false;
	}

	pScene->SetDepthLevelLocalRange(dLocalNear, dLocalFar, dLevelNear, dLevelFar);

	return true;
}
