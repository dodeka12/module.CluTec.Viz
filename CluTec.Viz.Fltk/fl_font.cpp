//
// "$Id: fl_font.cxx,v 1.1 2004/02/18 16:10:39 chp Exp $"
//
// Font selection code for the Fast Light Tool Kit (FLTK).
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

// Select fonts from the FLTK font table.
#include "flstring.h"
#include "Fl.H"
#include "fl_draw.H"
#include "x.H"
#include "Fl_Font.H"

#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
#  include "fl_font_win32.cpp"
#elif defined(__APPLE__)
#  include "fl_font_mac.cpp"
#elif USE_XFT
#  include "fl_font_xft.cpp"
#else
#  include "fl_font_x.cpp"
#endif // WIN32


double fl_width(const char* c) {
  if (c) return fl_width(c, (int)strlen(c));
  else return 0.0f;
}

void fl_draw(const char* str, int x, int y) {
  fl_draw(str, (int)strlen(str), x, y);
}

//
// End of "$Id: fl_font.cxx,v 1.1 2004/02/18 16:10:39 chp Exp $".
//
