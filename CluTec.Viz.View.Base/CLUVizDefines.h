////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.View.Base
// file:      CLUVizDefines.h
//
// summary:   Declares the clu viz defines class
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

#pragma once

// Image Data Types
#define CLUVIZ_IMG_BYTE           0x1400
#define CLUVIZ_IMG_UNSIGNED_BYTE  0x1401
#define CLUVIZ_IMG_SHORT          0x1402
#define CLUVIZ_IMG_UNSIGNED_SHORT 0x1403
#define CLUVIZ_IMG_INT            0x1404
#define CLUVIZ_IMG_UNSIGNED_INT   0x1405
#define CLUVIZ_IMG_FLOAT          0x1406
#define CLUVIZ_IMG_DOUBLE         0x140A

// Image Color Modes
#define CLUVIZ_IMG_RGB              0x1907
#define CLUVIZ_IMG_RGBA             0x1908
#define CLUVIZ_IMG_BGR              0x80E0
#define CLUVIZ_IMG_BGRA             0x80E1
#define CLUVIZ_IMG_LUMINANCE        0x1909
#define CLUVIZ_IMG_LUMINANCE_ALPHA  0x190A

// Window Styles
#define CLUVIZ_WNDSTYLE_NONE                    0x0000
#define CLUVIZ_WNDSTYLE_SIZEBORDER              0x0001
#define CLUVIZ_WNDSTYLE_CAPTION                 0x0002
#define CLUVIZ_WNDSTYLE_TOPMOST                 0x0004
#define CLUVIZ_WNDSTYLE_SYSMENU                 0x0008
#define CLUVIZ_WNDSTYLE_TOOL                    0x0010
#define CLUVIZ_WNDSTYLE_MULTISAMPLE_4   0x0100
#define CLUVIZ_WNDSTYLE_MULTISAMPLE_8   0x0200
#define CLUVIZ_WNDSTYLE_MULTISAMPLE_16  0x0300
#define CLUVIZ_WNDSTYLE_MULTISAMPLE_MASK 0x0300
#define CLUVIZ_WNDSTYLE_STD                     (CLUVIZ_WNDSTYLE_SIZEBORDER | CLUVIZ_WNDSTYLE_CAPTION | CLUVIZ_WNDSTYLE_SYSMENU)

#define CLUVIZ_MOUSE_LBUT       0
#define CLUVIZ_MOUSE_MBUT       1
#define CLUVIZ_MOUSE_RBUT       2

#define CLUVIZ_MOUSE_DOWN       1
#define CLUVIZ_MOUSE_UP         0

/////////////////////////////////////////////////////
// CLU Key IDs. Identical to FL key IDs

#define CLUVIZ_KEY_BackSpace            0xff08
#define CLUVIZ_KEY_Tab                          0xff09
#define CLUVIZ_KEY_Enter                        0xff0d
#define CLUVIZ_KEY_Pause                        0xff13
#define CLUVIZ_KEY_Scroll_Lock          0xff14
#define CLUVIZ_KEY_Escape                       0xff1b
#define CLUVIZ_KEY_Home                         0xff50
#define CLUVIZ_KEY_Left                         0xff51
#define CLUVIZ_KEY_Up                           0xff52
#define CLUVIZ_KEY_Right                        0xff53
#define CLUVIZ_KEY_Down                         0xff54
#define CLUVIZ_KEY_Page_Up                      0xff55
#define CLUVIZ_KEY_Page_Down            0xff56
#define CLUVIZ_KEY_End                          0xff57
#define CLUVIZ_KEY_Print                        0xff61
#define CLUVIZ_KEY_Insert                       0xff63
#define CLUVIZ_KEY_Menu                         0xff67	// the "menu/apps" key on XFree86
#define CLUVIZ_KEY_Help                         0xff68	// the 'help' key on Mac keyboards
#define CLUVIZ_KEY_Num_Lock                     0xff7f
#define CLUVIZ_KEY_KP                           0xff80	// use CLUVIZ_KEY_KP+'x' for 'x' on numeric keypad
#define CLUVIZ_KEY_KP_Enter                     0xff8d	// same as Fl_KP+'\r'
#define CLUVIZ_KEY_KP_Last                      0xffbd	// use to range-check keypad
#define CLUVIZ_KEY_F                            0xffbd	// use CLUVIZ_KEY_F+n for function key n
#define CLUVIZ_KEY_F_Last                       0xffe0	// use to range-check function keys
#define CLUVIZ_KEY_Shift_L                      0xffe1
#define CLUVIZ_KEY_Shift_R                      0xffe2
#define CLUVIZ_KEY_Control_L            0xffe3
#define CLUVIZ_KEY_Control_R            0xffe4
#define CLUVIZ_KEY_Caps_Lock            0xffe5
#define CLUVIZ_KEY_Meta_L                       0xffe7	// the left MSWindows key on XFree86
#define CLUVIZ_KEY_Meta_R                       0xffe8	// the right MSWindows key on XFree86
#define CLUVIZ_KEY_Alt_L                        0xffe9
#define CLUVIZ_KEY_Alt_R                        0xffea
#define CLUVIZ_KEY_Delete                       0xffff
