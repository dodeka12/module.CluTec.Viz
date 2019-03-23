// stdafx.h : Includedatei f�r Standardsystem-Includedateien
// oder h�ufig verwendete projektspezifische Includedateien,
// die nur in unregelm��igen Abst�nden ge�ndert werden.
//

#pragma once

#include "CluTec.Viz.View.Base\config_cluviz.h"

// �ndern Sie folgende Definitionen f�r Plattformen, die �lter als die unten angegebenen sind.
// In MSDN finden Sie die neuesten Informationen �ber die entsprechenden Werte f�r die unterschiedlichen Plattformen.
#ifndef WINVER				// Lassen Sie die Verwendung spezifischer Features von Windows XP oder sp�ter zu.
#define WINVER 0x0501		// �ndern Sie dies in den geeigneten Wert f�r andere Versionen von Windows.
#endif

#ifndef _WIN32_WINNT		// Lassen Sie die Verwendung spezifischer Features von Windows XP oder sp�ter zu.                   
#define _WIN32_WINNT 0x0501	// �ndern Sie dies in den geeigneten Wert f�r andere Versionen von Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Lassen Sie die Verwendung spezifischer Features von Windows 98 oder sp�ter zu.
#define _WIN32_WINDOWS 0x0410 // �ndern Sie dies in den geeigneten Wert f�r Windows Me oder h�her.
#endif

#ifndef _WIN32_IE			// Lassen Sie die Verwendung spezifischer Features von IE 6.0 oder sp�ter zu.
#define _WIN32_IE 0x0600	// �ndern Sie dies in den geeigneten Wert f�r andere Versionen von IE.
#endif

#define WIN32_LEAN_AND_MEAN		// Selten verwendete Teile der Windows-Header nicht einbinden.
// Windows-Headerdateien:
#include <windows.h>

#	define _WIN32_DCOM 
#	define _ATL_APARTMENT_THREADED
#	define _ATL_NO_AUTOMATIC_NAMESPACE

#	define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
#	define _ATL_ATTRIBUTES 1

#	include <atlbase.h>
#	include <atlcom.h>
#	include <atlwin.h>
#	include <atltypes.h>
#	include <atlctl.h>  
#	include <atlhost.h>

using namespace ATL;


#include "CluTec.Viz.Base\clu.h"
#include "CluTec.Viz.GA\CvCoreGA.h"
#include "CluTec.Viz.Draw\cludraw.h"
#include "CluTec.Viz.Parse\cluparsing.h"

