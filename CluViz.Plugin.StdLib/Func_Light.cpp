////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Light.cpp
//
// summary:   Implements the function light class
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
#include "Func_Light.h"

//////////////////////////////////////////////////////////////////////
/// Get AmbientLight as base element reference
///
/// Parameters:
///		none
///
///	Returns:
///		AmbientLight variable as BaseElement Reference


bool AmbientLightFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	
	int iVarCount = int(mVars.Count());
	
	if (iVarCount != 1 && iVarCount != 2)
	{
		int piPar[] = {1, 2};

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError( "First parameter has to be the name of the ambient light.", iLine, iPos );
		return false;
	}

	COGLAmbientLight *pLight = new COGLAmbientLight;
	pLight->SetName( mVars(0).GetStringPtr()->Str() );

	if (iVarCount > 1)
	{
		if (mVars(1).BaseType() != PDT_COLOR)
		{
			rCB.GetErrorList().GeneralError( "Second optional parameter has to be a color.", iLine, iPos );
			return false;
		}								

		pLight->Set( *mVars(1).GetOGLColorPtr() );
	}

	rVar = rCB.GetOGLDrawBase()->GetSceneRepository()->New( pLight );

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Set Ambient Light Function
///
/// Parameters:
///		1. Color

bool SetAmbientLightFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	
	if (iVarCount != 2)
	{
		int piPar[] = {2};

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{	
		rCB.GetErrorList().GeneralError( "First parameter has to be an ambient light variable.", iLine, iPos );
		return false;
	}

	TScene Ref = *mVars(0).GetScenePtr();
	if ( !Ref.IsValid() )
	{
		rCB.GetErrorList().GeneralError( "Invalid ambient light variable.", iLine, iPos );
		return false;
	}

	COGLAmbientLight *pLight = dynamic_cast<COGLAmbientLight*>( (COGLBaseElement*) Ref );
	if ( !pLight )
	{
		rCB.GetErrorList().GeneralError( "Invalid ambient light variable.", iLine, iPos );
		return false;
	}

	if (mVars(1).BaseType() != PDT_COLOR)
	{
		rCB.GetErrorList().GeneralError( "Second parameter has to be a color.", iLine, iPos );
		return false;
	}								

	pLight->Set( *mVars(1).GetOGLColorPtr() );

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Get Light of given number as base element reference
///
/// Parameters:
///		1. Light No.
///
///	Returns:
///		Light variable as BaseElement Reference


bool LightFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	
	int iVarCount = int(mVars.Count());
	int iLight;
	
	if (iVarCount != 2)
	{
		int piPar[] = {2};

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError( "First parameter has to be the name of the light.", iLine, iPos );
		return false;
	}

	if (!mVars(1).CastToCounter(iLight))
	{
		rCB.GetErrorList().GeneralError( "Expect second parameter to be the number of a light between 1 and 8.", iLine, iPos);
		return false;
	}								

	if (iLight < 1 || iLight > CLUDRAWBASE_MAXLIGHTCOUNT)
	{
		rCB.GetErrorList().GeneralError("Light number out of range.", iLine, iPos);
		return false;
	}

	COGLLight *pLight = new COGLLight;
	pLight->SetName( mVars(0).GetStringPtr()->Str() );
	pLight->LightNo( iLight - 1 );
	pLight->Ambient( 0.5f, 0.5f, 0.5f, 1.0f );
	pLight->Specular( 0.2f, 0.2f, 0.2f, 1.0f );
	pLight->Diffuse( 0.5f, 0.5f, 0.5f, 1.0f );
	pLight->Position( 0.0f, 0.0f, 0.0f, 1.0f );
	pLight->LinAtt( 0.2f );
	pLight->ConstAtt( 0.0f );
	pLight->QuadAtt( 0.0f );
	pLight->Enable();

	rVar = rCB.GetOGLDrawBase()->GetSceneRepository()->New( pLight );

	return true;
}



//////////////////////////////////////////////////////////////////////
/// Enable Light Function
///
/// Parameters:
/// 1. Light No.
/// 2. 1 / 0 representing enable/disable

bool EnableLightFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	
	int iVarCount = int(mVars.Count());
	TCVCounter iStatus;
	
	if (iVarCount != 2)
	{
		int piPar[] = {2};

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError( "First parameter has to be a light.", iLine, iPos );
		return false;
	}								

	COGLLight *pLight = dynamic_cast<COGLLight*>( (COGLBaseElement*) *mVars(0).GetScenePtr() );
	if ( !pLight )
	{
		rCB.GetErrorList().GeneralError( "Invalid light parameter.", iLine, iPos );
		return false;
	}								

	if (!mVars(1).CastToCounter(iStatus))
	{
		rCB.GetErrorList().InvalidParType(mVars(1), 1, iLine, iPos);
		return false;
	}								

	pLight->Enable( (iStatus ? true : false) );

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Enable Lighting in general Function
///
/// Parameters:
/// 1. 1 / 0 representing enable/disable

bool EnableLightingFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVCounter iStatus;

	if (iVarCount != 1)
	{
		int piPar[] = {1};

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iStatus))
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 0, iLine, iPos);
		return false;
	}								

	COGLLighting *pLighting = new COGLLighting();
	pLighting->Enable((iStatus != 0));

	COGLBEReference rObj = rCB.GetOGLDrawBase()->GetSceneRepository()->New( pLighting );
	rCB.GetOGLDrawBase()->DrawBaseElement(rObj);
	
	return true;
}



//////////////////////////////////////////////////////////////////////
/// Set Light Position Function
///
/// Parameters:
/// 1. Light No.
/// 2. Vector (is mapped to projective space)
/// OR 2. - 5. Projective coordinates of point

bool SetLightPosFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	
	int iVarCount = int(mVars.Count());
	TCVScalar fVal[4];
	
	if (!rCB.GetCLUDrawBase())
	{
		return false;
	}

	if (iVarCount != 2 && iVarCount != 5)
	{
		int piPar[] = {2, 5};

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError( "First parameter has to be a light.", iLine, iPos );
		return false;
	}								

	COGLLight *pLight = dynamic_cast<COGLLight*>( (COGLBaseElement*) *mVars(0).GetScenePtr() );
	if ( !pLight )
	{
		rCB.GetErrorList().GeneralError( "Invalid light parameter.", iLine, iPos );
		return false;
	}								

	if (iVarCount == 2)
	{
		if (mVars(1).BaseType() != PDT_MULTIV)
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}
		
		TMultiV &vA = *mVars(1).GetMultiVPtr();
		TMultiV vX;

		if (!rCB.CastMVtoP3(vA, vX))
		{
			rCB.GetErrorList().GeneralError("Multivector does not represent a vector.", iLine, iPos);
			return false;
		}

		int i;
		for(i = 0; i < 4; i++)
		{
			fVal[i] = vX[i+1];
		}
	}
	else if (iVarCount == 5)
	{
		int i;
		for (i = 1; i < 5; i++)
		{
			if (!mVars(i).CastToScalar(fVal[i-1], rCB.GetSensitivity()))
			{
				rCB.GetErrorList().InvalidParType(mVars(i), i+1, iLine, iPos);
				return false;
			}
		}
	}

	pLight->Position(float(fVal[0]), float(fVal[1]), float(fVal[2]), float(fVal[3]));

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Set Light Ambient Color Function
///
/// Parameters:
/// 1. Light No.
/// 2. Ambient Color

bool SetLightAmbientColFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	
	int iVarCount = int(mVars.Count());
	
	if (iVarCount != 2)
	{
		int piPar[] = {2};

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError( "First parameter has to be a light.", iLine, iPos );
		return false;
	}								

	COGLLight *pLight = dynamic_cast<COGLLight*>( (COGLBaseElement*) *mVars(0).GetScenePtr() );
	if ( !pLight )
	{
		rCB.GetErrorList().GeneralError( "Invalid light parameter.", iLine, iPos );
		return false;
	}								

	if (mVars(1).BaseType() != PDT_COLOR)
	{
		rCB.GetErrorList().InvalidParType(mVars(1), 1, iLine, iPos);
		return false;
	}								

	COGLColor& rCol = *mVars(1).GetOGLColorPtr();

	pLight->Ambient( rCol );

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Set Light Diffuse Color Function
///
/// Parameters:
/// 1. Light No.
/// 2. Diffuse Color

bool SetLightDiffuseColFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	
	int iVarCount = int(mVars.Count());

	if (iVarCount != 2)
	{
		int piPar[] = {2};

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError( "First parameter has to be a light.", iLine, iPos );
		return false;
	}								

	COGLLight *pLight = dynamic_cast<COGLLight*>( (COGLBaseElement*) *mVars(0).GetScenePtr() );
	if ( !pLight )
	{
		rCB.GetErrorList().GeneralError( "Invalid light parameter.", iLine, iPos );
		return false;
	}								

	if (mVars(1).BaseType() != PDT_COLOR)
	{
		rCB.GetErrorList().InvalidParType(mVars(1), 1, iLine, iPos);
		return false;
	}								

	COGLColor& rCol = *mVars(1).GetOGLColorPtr();
	pLight->Diffuse( rCol );

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Set Light Specular Color Function
///
/// Parameters:
/// 1. Light No.
/// 2. Specular Color

bool SetLightSpecularColFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	
	if (iVarCount != 2)
	{
		int piPar[] = {2};

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError( "First parameter has to be a light.", iLine, iPos );
		return false;
	}								

	COGLLight *pLight = dynamic_cast<COGLLight*>( (COGLBaseElement*) *mVars(0).GetScenePtr() );
	if ( !pLight )
	{
		rCB.GetErrorList().GeneralError( "Invalid light parameter.", iLine, iPos );
		return false;
	}								

	if (mVars(1).BaseType() != PDT_COLOR)
	{
		rCB.GetErrorList().InvalidParType(mVars(1), 1, iLine, iPos);
		return false;
	}								

	COGLColor& rCol = *mVars(1).GetOGLColorPtr();
	pLight->Specular( rCol );

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Set Light Spot Direction Function
///
/// Parameters:
/// 1. Light No.
/// 2. Vector (is mapped to Euclidean space)
/// OR 2. - 4. coordinates of direction

bool SetLightSpotDirFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	
	int iVarCount = int(mVars.Count());
	TCVScalar fVal[4];
	
	if (iVarCount < 2 || iVarCount > 4)
	{
		int piPar[] = {2, 4};

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError( "First parameter has to be a light.", iLine, iPos );
		return false;
	}								

	COGLLight *pLight = dynamic_cast<COGLLight*>( (COGLBaseElement*) *mVars(0).GetScenePtr() );
	if ( !pLight )
	{
		rCB.GetErrorList().GeneralError( "Invalid light parameter.", iLine, iPos );
		return false;
	}								

	if (iVarCount == 2)
	{
		if (mVars(1).BaseType() != PDT_MULTIV)
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}
		
		TMultiV &vA = *mVars(1).GetMultiVPtr();
		TMultiV vX;

		if (!rCB.CastMVtoE3(vA, vX))
		{
			rCB.GetErrorList().GeneralError("Multivector does not represent a vector.", iLine, iPos);
			return false;
		}

		int i;
		for(i = 0; i < 3; i++)
		{
			fVal[i] = vX[i+1];
		}
	}
	else if (iVarCount == 4)
	{
		int i;
		for (i = 1; i < 4; i++)
		{
			if (!mVars(i).CastToScalar(fVal[i-1], rCB.GetSensitivity()))
			{
				rCB.GetErrorList().InvalidParType(mVars(i), i+1, iLine, iPos);
				return false;
			}
		}
	}

	pLight->SpotDir(float(fVal[0]), float(fVal[1]), float(fVal[2]));

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Set Light Spot Exponent Function
///
/// Parameters:
/// 1. Light No.
/// 2. Exponent value (scalar)

bool SetLightSpotExpFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVScalar fVal;
	
	if (iVarCount != 2)
	{
		int piPar[] = {2};

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError( "First parameter has to be a light.", iLine, iPos );
		return false;
	}								

	COGLLight *pLight = dynamic_cast<COGLLight*>( (COGLBaseElement*) *mVars(0).GetScenePtr() );
	if ( !pLight )
	{
		rCB.GetErrorList().GeneralError( "Invalid light parameter.", iLine, iPos );
		return false;
	}								

	if (!mVars(1).CastToScalar(fVal, rCB.GetSensitivity()))
	{
		rCB.GetErrorList().InvalidParType(mVars(1), 1, iLine, iPos);
		return false;
	}								

	pLight->SpotExp(float(fVal));

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Light Spot Cut Off Function
///
/// Parameters:
/// 1. Light No.
/// 2. Cut off value (scalar)

bool SetLightSpotCutOffFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	
	int iVarCount = int(mVars.Count());
	TCVScalar fVal;
	
	if (iVarCount != 2)
	{
		int piPar[] = {2};

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError( "First parameter has to be a light.", iLine, iPos );
		return false;
	}								

	COGLLight *pLight = dynamic_cast<COGLLight*>( (COGLBaseElement*) *mVars(0).GetScenePtr() );
	if ( !pLight )
	{
		rCB.GetErrorList().GeneralError( "Invalid light parameter.", iLine, iPos );
		return false;
	}								

	if (!mVars(1).CastToScalar(fVal, rCB.GetSensitivity()))
	{
		rCB.GetErrorList().InvalidParType(mVars(1), 1, iLine, iPos);
		return false;
	}								

	pLight->SpotCutOff(float(fVal));

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Light Attenuation Function
///
/// Parameters:
/// 1. Light No.
/// 2. Constant attenuation value (scalar)
/// 3. Linear attenuation value (scalar)
/// 4. Quadratic attenuation value (scalar)

bool SetLightAttenuationFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	
	int iVarCount = int(mVars.Count());
	TCVScalar fVal[3];
	
	if (iVarCount != 4)
	{
		int piPar[] = {4};

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError( "First parameter has to be a light.", iLine, iPos );
		return false;
	}								

	COGLLight *pLight = dynamic_cast<COGLLight*>( (COGLBaseElement*) *mVars(0).GetScenePtr() );
	if ( !pLight )
	{
		rCB.GetErrorList().GeneralError( "Invalid light parameter.", iLine, iPos );
		return false;
	}								

	int i;

	for (i = 0; i < 3; i++)
	{
		if (!mVars(i+1).CastToScalar(fVal[i], rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(mVars(i+1), i+2, iLine, iPos);
			return false;
		}								
	}

	pLight->ConstAtt(float(fVal[0]));
	pLight->LinAtt(float(fVal[1]));
	pLight->QuadAtt(float(fVal[2]));

	return true;
}





