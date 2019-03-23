////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.GA.rtl
// file:      xgalib2.h
//
// summary:   Declares the xgalib 2 class
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

// Includes all Headers for GA Library

#ifndef _XGALIB_H_
	#define _XGALIB_H_

//disable warnings on extern before template instantiation
#pragma warning (disable : 4231)


//#pragma warning (disable : 4251)

#ifdef CLUGA_EXPORTS

    #include "CluTec.Viz.GA.Base\cgainst.h"

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

    #include "CluTec.Viz.Base\dynlist.cxx"
    #include "CluTec.Viz.Base\mem.cxx"
    #include "CluTec.Viz.Base\memobj.cxx"
    #include "CluTec.Viz.GA.Base\MVInfo.cxx"
    #include "CluTec.Viz.Base\ringbuf.cxx"

#else

    #include "CluTec.Viz.GA.Base\cgainst.h"

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

    #include "CluTec.Viz.GA.Base\MVInfo.cxx"

//#include "CluTec.Viz.GA.Base\cgainst.h"
//
//#include "CluTec.Viz.GA.Base\cga.h"
//#include "CluTec.Viz.GA.Base\pga.h"
//#include "CluTec.Viz.GA.Base\e3ga.h"
//#include "CluTec.Viz.GA.Base\e8ga.h"
//#include "CluTec.Viz.GA.Base\confga.h"
//#include "CluTec.Viz.GA.Base\el2ga.h"

#endif

	class CLUGA_API CStrMem;
	class CLUGA_API MemBase;


//template class Mem<uint>;
//template class Mem<float>;
//template class Mem<double>;

	CLUGA_EXT template class CLUGA_API Mem<char>;
	CLUGA_EXT template class CLUGA_API Mem<uint>;
	CLUGA_EXT template class CLUGA_API Mem<float>;
	CLUGA_EXT template class CLUGA_API Mem<double>;

	CLUGA_EXT template class CLUGA_API Mem<float*>;
	CLUGA_EXT template class CLUGA_API Mem<double*>;
	CLUGA_EXT template class CLUGA_API Mem<Mem<uint>*>;
	CLUGA_EXT template class CLUGA_API Mem<CPointND<double, 3> >;
	CLUGA_EXT template class CLUGA_API Mem<CPointND<float, 3> >;

	CLUGA_EXT template class CLUGA_API MemObj<float>;
	CLUGA_EXT template class CLUGA_API MemObj<double>;
	CLUGA_EXT template class CLUGA_API MemObj<Mem<uint> >;

	CLUGA_EXT template class CLUGA_API RingBuf<uint>;

	CLUGA_EXT template class CLUGA_API CMVInfo<double>;
	CLUGA_EXT template class CLUGA_API CMVInfo<float>;

	CLUGA_EXT template class CLUGA_API Blade<double>;
	CLUGA_EXT template class CLUGA_API MultiV<double>;
	CLUGA_EXT template class CLUGA_API MultiVStyle<double>;
	CLUGA_EXT template class CLUGA_API RingBuf<double>;

	CLUGA_EXT template class CLUGA_API Blade<float>;
	CLUGA_EXT template class CLUGA_API MultiV<float>;
	CLUGA_EXT template class CLUGA_API MultiVStyle<float>;
	CLUGA_EXT template class CLUGA_API RingBuf<float>;

	CLUGA_EXT template class CLUGA_API Mem<MultiV<double>* >;
	CLUGA_EXT template class CLUGA_API Mem<MultiV<float>* >;

	CLUGA_EXT template class CLUGA_API MemObj<MultiV<double> >;
	CLUGA_EXT template class CLUGA_API MemObj<MultiV<float> >;

	CLUGA_EXT template class CLUGA_API E3GA<double>;
	CLUGA_EXT template class CLUGA_API E3GA<float>;

#ifdef USE_GA_E8
		CLUGA_EXT template class CLUGA_API E8GA<double>;
		CLUGA_EXT template class CLUGA_API E8GA<float>;
#endif

	CLUGA_EXT template class CLUGA_API El2GA<double>;
	CLUGA_EXT template class CLUGA_API El2GA<float>;

	CLUGA_EXT template class CLUGA_API ConfGA<double>;
	CLUGA_EXT template class CLUGA_API ConfGA<float>;

	CLUGA_EXT template class CLUGA_API PGA<double>;
	CLUGA_EXT template class CLUGA_API PGA<float>;

	Instantiate_CGA(double);
	Instantiate_CGA(float);

//#include "CluTec.Viz.GA.Base\cga.h"
//#include "CluTec.Viz.Base\CStrMem.h"
//#include "CluTec.Viz.Base\dynlist.h"
//#include "CluTec.Viz.Base\makestr.h"
//#include "CluTec.Viz.Base\mathelp.h"
//#include "CluTec.Viz.Base\mem.h"
//#include "CluTec.Viz.Base\memobj.h"
//#include "CluTec.Viz.Base\ringbuf.h"
//
//#include "CluTec.Viz.GA.Base\cgainst.h"
//
///*
//#ifndef TRACE
//
//	#ifndef _WINDOWS
//		#ifdef _DEBUG
//			#define TRACE printf
//		#endif
//	#endif
//
//	#ifndef TRACE
//		#define TRACE ;//
//	#endif
//
//#endif
//
//*/
//extern unsigned _xgalib_version[5];
//extern char *_xgalib_license_text;

#endif
