////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.GA.rtl
// file:      xgaext2_double.cpp
//
// summary:   Implements the xgaext 2 double class
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


// Instantiation of needed Base Extention templates
#include "xgalib2.h"


#include "CluTec.Viz.GA.Base\cga.cxx"
#include "CluTec.Viz.GA.Base\pga.cxx"
#include "CluTec.Viz.GA.Base\sta.cxx"
#include "CluTec.Viz.GA.Base\e3ga.cxx"

#ifdef USE_GA_E8
#include "CluTec.Viz.GA.Base\e8ga.cxx"
#endif

#include "CluTec.Viz.GA.Base\GA1p2m.cxx"
#include "CluTec.Viz.GA.Base\confga.cxx"
#include "CluTec.Viz.GA.Base\Cl22.cxx"
#include "CluTec.Viz.GA.Base\el2ga.cxx"

#ifdef USE_GA_CC2
#include "CluTec.Viz.GA.Base\cc2ga.cxx"
#endif

#include "CluTec.Viz.Base\mem.cxx"
#include "CluTec.Viz.Base\memobj.cxx"
#include "CluTec.Viz.GA.Base\MVInfo.cxx"


//#include "matrix.cpp"

////////////////////////////////////////////////////////////////////////////////////
// double instantiation

//Instantiate_CGA(double);

Instantiate_MVInfo(double);

Instantiate_PGA(double);
Instantiate_STA(double); 
Instantiate_E3GA(double);

#ifdef USE_GA_E8
	Instantiate_E8GA(double);
#endif

Instantiate_GA1p2m(double);
Instantiate_ConfGA(double);
Instantiate_Cl22(double);
Instantiate_El2GA(double);

#ifdef USE_GA_CC2
	Instantiate_CC2GA(double);
#endif
