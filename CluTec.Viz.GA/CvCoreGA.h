////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.GA.rtl
// file:      CvCoreGA.h
//
// summary:   Declares the cv core ga class
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

// Folgender ifdef-Block ist die Standardmethode zum Erstellen von Makros, die das Exportieren 
// aus einer DLL vereinfachen. Alle Dateien in dieser DLL werden mit dem CLUGA_EXPORTS-Symbol
// kompiliert, das in der Befehlszeile definiert wurde. Das Symbol darf nicht f�r ein Projekt definiert werden,
// das diese DLL verwendet. Alle anderen Projekte, deren Quelldateien diese Datei beinhalten, erkennen 
// CLUGA_API-Funktionen als aus einer DLL importiert, w�hrend die DLL
// mit diesem Makro definierte Symbole als exportiert ansieht.
#ifdef CLUGA_EXPORTS
#define CLUGA_API __declspec(dllexport)
#define CLUGA_EXT
#else
#define CLUGA_API __declspec(dllimport)
#define CLUGA_EXT extern
#endif

#include "xgalib2.h"

#ifndef _CLUGALIB_HEADER
#define _CLUGALIB_HEADER

CLUGA_API E3GA<double> *m_pE3GABase;
CLUGA_API E3GA<float> *m_pfE3GABase;

CLUGA_API PGA<double> *m_pP3GABase;
CLUGA_API PGA<float> *m_pfP3GABase;

CLUGA_API ConfGA<double> *m_pN3GABase;
CLUGA_API ConfGA<float> *m_pfN3GABase;

CLUGA_API El2GA<double> *m_pEl2GABase;
CLUGA_API El2GA<float> *m_pfEl2GABase;

#ifdef USE_GA_E8
CLUGA_API E8GA<double> *m_pE8GABase;
CLUGA_API E8GA<float> *m_pfE8GABase;
#endif

CLUGA_API bool InitCLUGA();
CLUGA_API bool FinalizeCLUGA();

#endif
