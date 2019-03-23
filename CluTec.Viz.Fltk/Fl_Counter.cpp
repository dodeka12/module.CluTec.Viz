//
// "$Id: Fl_Counter.cxx,v 1.2 2005/08/24 22:10:01 chp Exp $"
//
// Counter widget for the Fast Light Tool Kit (FLTK).
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
#include "Fl_Counter.H"
#include "fl_draw.H"

#include <stdio.h>
#include <string.h>

void Fl_Counter::draw() 
{
	int i; Fl_Boxtype boxtype[5];
	Fl_Color selcolor, colBorder;

	boxtype[0] = box();
	if (boxtype[0] == FL_UP_BOX) 
		boxtype[0] = FL_DOWN_BOX;

	//if ( bMouseOver )
		colBorder = FL_DARK_BLUE;
	//else
//		colBorder = FL_LIGHT2;

	for (i=1; i<5; i++)
	{
		if (mouseobj == i)
			boxtype[i] = fl_down(box());
		else
			boxtype[i] = box();
	}

	int xx[5], yy[5], ww[5], hh[5];
	int bx[5], by[5], bw[5], bh[5];

	if (type() == FL_NORMAL_COUNTER) 
	{
		int W = 12; //w()*15/100;
		xx[1] = x();	 ww[1] = W;
		xx[2] = x()+1*W;     ww[2] = W;
		xx[0] = x()+2*W;     ww[0] = w()-4*W;
		xx[3] = x()+w()-2*W; ww[3] = W;
		xx[4] = x()+w()-1*W; ww[4] = W;
	} 
	else if ( type() == FL_SIMPLE_COUNTER )
	{
		int W = 12; //w()*20/100;
		xx[2] = x();	 ww[2] = W;
		xx[0] = x()+W;	 ww[0] = w()-2*W;
		xx[3] = x()+w()-1*W; ww[3] = W;
	}
	else if ( type() == FL_UPDOWN_COUNTER )
	{
		int iArrowWidth = 11;
		int iArrowHeight = (h() - 1) / 2;

		// Down Arrow
		bx[2] = x();
		by[2] = y() + 1 + iArrowHeight;
		bw[2] = iArrowWidth + 2;
		bh[2] = iArrowHeight + 1;

		xx[2] = bx[2] + 2;
		yy[2] = by[2] + 2;
		ww[2] = bw[2] - 4;
		hh[2] = bh[2] - 4;

		// Up Arrow
		bx[3] = x();
		by[3] = y();
		bw[3] = iArrowWidth + 2;
		bh[3] = iArrowHeight + 1;

		xx[3] = bx[3] + 2;
		yy[3] = by[3] + 2;
		ww[3] = bw[3] - 4;
		hh[3] = bh[3] - 4;

		// Text
		bx[0] = x() + iArrowWidth + 1;
		by[0] = y();
		bw[0] = w() - iArrowWidth - 1;
		bh[0] = h();

		xx[0] = bx[0] + 1;
		yy[0] = by[0] + 1;
		ww[0] = bw[0] - 2;
		hh[0] = bh[0] - 2;
	}

	if ( bMouseOver )
		draw_box(FL_BORDER_FRAME, bx[0], by[0], bw[0], bh[0], colBorder );

	if ( active_r() )
		draw_box(boxtype[0], xx[0], yy[0], ww[0], hh[0], FL_BACKGROUND2_COLOR);
	else
		draw_box(boxtype[0], xx[0], yy[0], ww[0], hh[0], FL_LIGHT1 );

	//fl_font(textfont(), textsize());
	//fl_color(active_r() ? textcolor() : fl_inactive(textcolor()));

	char str[128];
	if ( pcFormat[0] )
		sprintf( str, pcFormat, value() );
	else
		format(str);

	if ( pcUnit[0] )
		strcat( str, pcUnit );

	Label.font = textfont_;
	Label.size = textsize_;
	Label.type = FL_NORMAL_LABEL;
	Label.color = active_r() ? textcolor() : fl_inactive(textcolor());
	Label.value = str;
	Label.draw( xx[0], yy[0], ww[0], hh[0], FL_ALIGN_CENTER );

	//fl_draw(str, xx[0], yy[0], ww[0], hh[0], FL_ALIGN_CENTER);

	if (Fl::focus() == this) 
		draw_focus(boxtype[0], xx[0], yy[0], ww[0], hh[0]);

	if (!(damage()&FL_DAMAGE_ALL)) 
		return; // only need to redraw text

	if (active_r())
		selcolor = labelcolor();
	else
		selcolor = fl_inactive(labelcolor());

	if (type() == FL_NORMAL_COUNTER) 
	{
		draw_box(boxtype[1], xx[1], y(), ww[1], h(), color());
		fl_draw_symbol("@-4<<", xx[1], y(), ww[1], h(), selcolor);

		draw_box(boxtype[4], xx[4], y(), ww[4], h(), color());
		fl_draw_symbol("@-4>>", xx[4], y(), ww[4], h(), selcolor);
	}

	if ( type() == FL_SIMPLE_COUNTER || type() == FL_NORMAL_COUNTER )
	{
		draw_box(boxtype[2], xx[2], y(), ww[2], h(), color());
		fl_draw_symbol("@-18<",  xx[2], y(), ww[2], h(), selcolor);

		draw_box(boxtype[3], xx[3], y(), ww[3], h(), color());
		fl_draw_symbol("@-18>",  xx[3], y(), ww[3], h(), selcolor);
	}

	if (type() == FL_UPDOWN_COUNTER && active_r() ) 
	{
		draw_box(boxtype[2], xx[2], yy[2], ww[2], hh[2], color());
		
		if ( bMouseOver )
			draw_box(FL_BORDER_FRAME, bx[2], by[2], bw[2], bh[2], colBorder);

		fl_draw_symbol("@8<",  xx[2]+2, yy[2]-4, ww[2]-4, hh[2]+8, FL_LIGHT1);

		draw_box(boxtype[3], xx[3], yy[3], ww[3], hh[3], color());

		if ( bMouseOver )
			draw_box(FL_BORDER_FRAME, bx[3], by[3], bw[3], bh[3], colBorder);

		fl_draw_symbol("@8>",  xx[3]+2, yy[3]-3, ww[3]-4, hh[3]+8, FL_LIGHT3);
	}
}


void Fl_Counter::SetFormatStr( const char *pcStr )
{
	strncpy( pcFormat, pcStr, 99 );
	pcFormat[99] = 0;
}

void Fl_Counter::SetUnitStr( const char *pcStr )
{
	strncpy( pcUnit, pcStr, 19 );
	pcFormat[19] = 0;
}


void Fl_Counter::increment_cb() 
{
  if (!mouseobj) 
	  return;

  double v = value();

  switch (mouseobj) 
  {
  case 1: 
	  v -= lstep_; 
	  break;

  case 2: 
	  v = increment(v, -1); 
	  break;

  case 3: 
	  v = increment(v, 1); 
	  break;

  case 4: 
	  v += lstep_; 
	  break;
  }

  handle_drag(clamp(round(v)));
}

#define INITIALREPEAT .5
#define REPEAT .1

void Fl_Counter::repeat_callback(void* v) {
  Fl_Counter* b = (Fl_Counter*)v;
  if (b->mouseobj) {
    Fl::add_timeout(REPEAT, repeat_callback, b);
    b->increment_cb();
  }
}

int Fl_Counter::calc_mouseobj() 
{
	if (type() == FL_NORMAL_COUNTER) 
	{
		int W = 12; //w()*15/100;
		if (Fl::event_inside(x(), y(), W, h())) return 1;
		if (Fl::event_inside(x()+W, y(), W, h())) return 2;
		if (Fl::event_inside(x()+w()-2*W, y(), W, h())) return 3;
		if (Fl::event_inside(x()+w()-W, y(), W, h())) return 4;
	} 
	else if ( type() == FL_SIMPLE_COUNTER )
	{
		int W = 12; //w()*20/100;
		if (Fl::event_inside(x(), y(), W, h())) return 2;
		if (Fl::event_inside(x()+w()-W, y(), W, h())) return 3;
	}
	else if ( type() == FL_UPDOWN_COUNTER )
	{
		int iArrowWidth = 11;
		int iArrowHeight = (h() - 1) / 2;

		// Down Arrow
		if ( Fl::event_inside( x(), y() + 1 + iArrowHeight, iArrowWidth, iArrowHeight ) )
			return 2;

		// Up Arrow
		if ( Fl::event_inside( x(), y(), iArrowWidth, iArrowHeight ) )
			return 3;

	}

	return -1;
}

int Fl_Counter::handle(int event) 
{
	int /*i,*/ iBut = 0;
	double dVal;

	switch (event) 
	{
	case FL_RELEASE:
/*
		if (mouseobj) 
		{
			Fl::remove_timeout(repeat_callback, this);
			mouseobj = 0;
			redraw();
		}
*/
		//increment_cb();
		iBut = calc_mouseobj();
		dVal = value();
		if ( iBut == 1 ) dVal -= lstep_;
		else if ( iBut == 2 ) dVal = increment(dVal, -1);
		else if ( iBut == 3 ) dVal = increment(dVal, 1);
		else if ( iBut == 4 ) dVal += lstep_;

		handle_drag( clamp( dVal ) );
		handle_release();
		redraw();
		return 1;

	case FL_PUSH:
		if (Fl::visible_focus()) Fl::focus(this);
		handle_push();
		iBut = calc_mouseobj();
		return 1;

	case FL_DRAG:
		if ( !Fl::event_inside(x()+1, y()+1, w()-2, h()-2) )
		{
			bMouseOver = false;
			redraw();
		}
		return 1;
/*
		if ( event == FL_DRAG && bMouseOver )
		{
			bMouseOver = false;
			redraw();
		}

		i = calc_mouseobj();
		if (i != mouseobj) 
		{
			Fl::remove_timeout(repeat_callback, this);
			mouseobj = (uchar)i;
			if (i) Fl::add_timeout(INITIALREPEAT, repeat_callback, this);
			increment_cb();
			//bMouseOver = true;
            redraw();
		}
		return 1;
*/
	case FL_KEYBOARD :
		switch (Fl::event_key()) 
		{
		case FL_Left:
			handle_drag(clamp(increment(value(),-1)));
			return 1;
		case FL_Right:
			handle_drag(clamp(increment(value(),1)));
			return 1;
		case FL_Up:
			handle_drag(clamp(round(value() + lstep_)));
			return 1;
		case FL_Down:
			handle_drag(clamp(round(value() - lstep_)));
			return 1;
		default:
			return 0;
		}
		// break not required because of switch...
	case FL_FOCUS :
	case FL_UNFOCUS :
		if (Fl::visible_focus()) 
		{
			//bMouseOver = true;
			redraw();
			return 1;
		} else return 0;

	case FL_ENTER :
		if ( active() )
		{
			bMouseOver = true;
			redraw();
		}
		return 1;

	case FL_LEAVE :
		if ( active() )
		{
			bMouseOver = false;
			redraw();
/*
			if ( !Fl::pushed() && !iRelease )
			{
				bMouseOver = false;
				redraw();
			}

			if ( !Fl::pushed() && iRelease > 0 )
				--iRelease;
*/
		}
		return 1;

	default:
		return 0;
	}
}

Fl_Counter::~Fl_Counter() 
{
  Fl::remove_timeout(repeat_callback, this);
}

Fl_Counter::Fl_Counter(int X, int Y, int W, int H, const char* l)
: Fl_Valuator(X, Y, W, H, l) 
{
	box(FL_UP_BOX);
	selection_color(FL_INACTIVE_COLOR); // was FL_BLUE
	align(FL_ALIGN_BOTTOM);
	bounds(-1000000.0, 1000000.0);
	Fl_Valuator::step(1, 10);
	lstep_ = 1.0;
	mouseobj = 0;
	textfont_ = FL_HELVETICA;
	textsize_ = (uchar)FL_NORMAL_SIZE;
	textcolor_ = FL_FOREGROUND_COLOR;

	Label.font = textfont_;
	Label.size = textsize_;
	Label.type = FL_NORMAL_LABEL;
	Label.value = 0;
	Label.image = 0;
	Label.deimage = 0;
	Label.color = FL_FOREGROUND_COLOR;

	pcFormat[0] = 0;
	pcUnit[0] = 0;

	bMouseOver = false;
	iRelease = 0;
}

//
// End of "$Id: Fl_Counter.cxx,v 1.2 2005/08/24 22:10:01 chp Exp $".
//
