//
// "$Id: fl_set_fonts.cxx,v 1.1 2004/02/18 16:10:41 chp Exp $"
//
// More font utilities for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2003 by Bill Spitzak and others.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems to "fltk-bugs@fltk.org".
//

#include "Fl.H"
#include "x.H"
#include "Fl_Font.H"
#include "flstring.h"
#include <stdlib.h>

#ifdef WIN32
#  include "fl_set_fonts_win32.cpp"
#elif defined(__APPLE__)
#  include "fl_set_fonts_mac.cpp"
#elif USE_XFT
#  include "fl_set_fonts_xft.cpp"
#else
#  include "fl_set_fonts_x.cpp"
#endif // WIN32

//
// End of "$Id: fl_set_fonts.cxx,v 1.1 2004/02/18 16:10:41 chp Exp $".
//
