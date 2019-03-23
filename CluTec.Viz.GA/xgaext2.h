////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.GA.rtl
// file:      xgaext2.h
//
// summary:   Declares the xgaext 2 class
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

// Includes for XGAExt2 Library


#ifndef _XGAEXT2_H_
#define _XGAEXT2_H_

#include "IL\config.h"

#include "xgalib2.h"

#include "CluTec.Viz.GA.Base\MVInfo.h"
#include "CluTec.Viz.GA.Base\pga.h"
#include "CluTec.Viz.GA.Base\sta.h"
#include "CluTec.Viz.GA.Base\e3ga.h"

#ifdef USE_GA_E8
#include "CluTec.Viz.GA.Base\e8ga.h"
#endif

#include "CluTec.Viz.GA.Base\GA1p2m.h"
#include "CluTec.Viz.GA.Base\confga.h"
#include "CluTec.Viz.GA.Base\Cl22.h"
#include "CluTec.Viz.GA.Base\el2ga.h"

#ifdef USE_GA_CC2
#include "CluTec.Viz.GA.Base\cc2ga.h"
#endif

#include "CluTec.Viz.GA.Base\pgainst.h"
#include "CluTec.Viz.GA.Base\stainst.h"
#include "CluTec.Viz.GA.Base\e3gainst.h"
#ifdef USE_GA_E8
#include "CluTec.Viz.GA.Base\e8gainst.h"
#endif
#include "CluTec.Viz.GA.Base\GA1p2mInst.h"
#include "CluTec.Viz.GA.Base\confgainst.h"
#include "CluTec.Viz.GA.Base\el2gainst.h"
#ifdef USE_GA_CC2
#include "CluTec.Viz.GA.Base\cc2gainst.h"
#endif

extern unsigned _xgaext_version[5];
extern char *_xgaext_license_text;


#endif
