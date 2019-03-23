////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      xutlib.cpp
//
// summary:   Implements the xutlib class
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

#include "CStrMem.h"

////////////////////////////////////////////////////////////////////////////////////
// Template Library for Mem Class

//#include "mem.cxx"
#include "mem.h"

//template class Mem<float>;
//template class Mem<double>;
//template class Mem<uint>;

//#include "mem.cxx"
//template class Mem<char>;
//template class Mem<uint*>;
//template class Mem<int>;
//template class Mem<cStr *>;


////////////////////////////////////////////////////////////////////////////////////
// DynList Template Library

#include"dynlist.cxx"

// To instantiate DynList include statements of the following type
//
// template class DynList<[Class]>;
//

////////////////////////////////////////////////////////////////////////////////////
/// Template library for RingBuf Class

#include"ringbuf.cxx"

Instantiate_RingBuf(uint);


////////////////////////////////////////////////////////////////////////////////////
// Template Library for MemObj Class

#include"memobj.h"

// Enter here Template Instanciations of the type
//
// template class MemObj<[Class]>;
//
// for all those objects that need to have constructor/destructor called
// and define their own operator= for copying.

//#include"memobj.cxx"
//template class MemObj<cStr>;
//template class MemObj<Mem<uint> >;

////////////////////////////////////////////////////////////////////////////////////
// Template Library for MessageList class

#include "MessageList.cxx"

template struct tMessageElement<CStrMem>;
//template class Mem<tMessageElement<cStr>* >;
//template class MemObj<tMessageElement<cStr> >;
template class MessageList<CStrMem>;



////////////////////////////////////////////////////////////////////////////////////

// The first two number give major and minor version.
// The last three numbers give the date in the format day,month,year.
unsigned _xutlib_version[5] = {1,3,14,8,1999};

char *_xutlib_license_text = 
"This program uses the CLU Library\n"
"Version 1.3, August 1999\n\n"
//
"The library provides utility classes.\n\n"
//
"Copyright (C) 1997-2001  Christian B.U. Perwass\n\n"
//
"This library is free software; you can redistribute it and/or\n"
"modify it under the terms of the GNU Library General Public\n"
"License as published by the Free Software Foundation; either\n"
"version 2 of the License, or (at your option) any later version.\n\n"
//
"This library is distributed in the hope that it will be useful,\n"
"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n"
"Library General Public License for more details.\n\n"
//
"You should have received a copy of the GNU Library General Public\n"
"License along with this library; if not, write to the\n"
"Free Software Foundation, Inc., 59 Temple Place - Suite 330,\n"
"Boston, MA  02111-1307, USA.\n\n"
//
"The GNU Library General Public License can be found in the file\n"
"license.txt distributed with this library.\n\n"
//
"Please send all queries and comments regarding the XUT Library to\n\n"
//
"email: public@perwass.com\n"
"mail: Institut fuer Informatik, Preusserstr. 1-9, 24105 Kiel, Germany.\n\n";

