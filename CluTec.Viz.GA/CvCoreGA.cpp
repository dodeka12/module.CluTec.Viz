////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.GA.rtl
// file:      CvCoreGA.cpp
//
// summary:   Implements the cv core ga class
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
#include "CvCoreGA.h"
#include "CluTec.Viz.Base/config.h"

CLUGA_API bool InitCLUGA()
{
	m_pE3GABase  = new E3GA<double>;
	m_pfE3GABase = new E3GA<float>;

	m_pP3GABase  = new PGA<double>;
	m_pfP3GABase = new PGA<float>;

	m_pN3GABase  = new ConfGA<double>;
	m_pfN3GABase = new ConfGA<float>;

	m_pEl2GABase  = new El2GA<double>;
	m_pfEl2GABase = new El2GA<float>;

	#ifdef USE_GA_E8
		m_pE8GABase  = new E8GA<double>;
		m_pfE8GABase = new E8GA<float>;
	#endif

	return true;
}

CLUGA_API bool FinalizeCLUGA()
{
	m_pE3GABase->ResetInit();
	m_pfE3GABase->ResetInit();

	m_pP3GABase->ResetInit();
	m_pfP3GABase->ResetInit();

	m_pN3GABase->ResetInit();
	m_pfN3GABase->ResetInit();

	m_pEl2GABase->ResetInit();
	m_pfEl2GABase->ResetInit();

	#ifdef USE_GA_E8
		m_pE8GABase->ResetInit();
		m_pfE8GABase->ResetInit();
	#endif

	delete m_pN3GABase;
	delete m_pfN3GABase;

	delete m_pP3GABase;
	delete m_pfP3GABase;

	delete m_pE3GABase;
	delete m_pfE3GABase;

	delete m_pEl2GABase;
	delete m_pfEl2GABase;

	#ifdef USE_GA_E8
		delete m_pE8GABase;
		delete m_pfE8GABase;
	#endif

	return true;
}
