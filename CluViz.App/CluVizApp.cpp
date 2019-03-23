////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluVizApp
// file:      CluVizApp.cpp
//
// summary:   This is the CluViz Application main file.
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


#include "StdAfx.h"

#include "CluTec.Types1/IString.h"
#include "CluTec.Base\Exception.h"
#include "CluTec.Viz.Wnd\Clu.Viz.Wnd.h"


#ifdef _GNUCPP3_
	#include <unistd.h>
	#define _getcwd getcwd
	#define _chdir chdir
#else
	#include <direct.h>
#endif

#include <string>
#include <vector>
using namespace std;

static HINSTANCE s_hInstance;

int main(int argc, char* argv[])
{
	int iHandle;

	/// Initial Script
	const char* pcIntro =
		"// Press CTRL+n to start a new script.\n"
		"/*\n"
		"	Written and conceived by Dr. Christian Perwass.\n"
		"*/\n\n"
		"DefVarsN3();\n"
		":N3_IPNS;\n"
		":N3_SOLID;\n"
		":N3_SHOW_IMAG;\n"
		"EnableAntiAliasing( true );\n"
		"SetSphereDetailLevel( 10 );\n"
		"EnableStdCtrl( true );\n"
		"EnableMenu( true );\n\n"
		"if (ExecMode & EM_CHANGE)\n"
		"{\n"
		"    SetTextFont(\"newcent\");\n\n"
		"    frTex = Frame(\"SphereTex\", true, \"texture\");\n"
		"    AnimTranslateFrame( frTex, VecE3(1,0), \"Constant\", 0.15);\n\n"
		"\n"
		"    SetTextMagStep(14);\n"
		"    SetImgPos( -1.5, 0.3, 1.5 );\n\n"
		"    scInfo = StartOverlay();\n"
		"    :White;\n"
		"    SetImagePos(1, 1, 0);\n"
		"    SetImageAlign(0, 1);\n"
		"    SetRenderTextSize(35);\n"
		"    :GetRenderTextImage(\"CluViz v%s\");"
		"    :MWhite;\n"
		"    SetImagePos(1, 99, 0);\n"
		"    SetImageAlign(0, 0);\n"
		"    SetRenderTextSize(15);\n"
		"    :GetRenderTextImage(\"Change to mouse mode 1 and move the red sphere\\n\"\n"
		"    + 	\"by holding down the right mouse button and\\n\"\n"
		"    +	\"moving the mouse in the visualization window.\\n\"\n"
		"    +	\"\\n\"\n"
		"    +	\"Press F1 in the editor window for more help.\");\n"
		"    EndOverlay();\n\n"
		"}\n"
		":Color(0.540, 0.991, 1.000);\n"
		"SetTextMagStep(24);\n"
		"SetTexture(\"\\\\sl CluViz\");\n"
		":frTex;\n"
		":MRed;\n"
		":Sr = VecN3(VecE3(1) + VecE3(-0.12,0,0.2)) - 0.5*e;\n"
		"SetTexture();\n\n"
		":Color(0, 0, 0.6, 0.6);\n"
		":Sb = VecN3(0.5, 0, 0) - 0.5*e;\n\n"
		":Color(0, 0.6, 0, 0.6);\n"
		":Sg = VecN3(-0.5, 0.5, 0) - 0.5*e;\n\n"
		":Color(0.540, 0.991, 1.000);\n"
		":Magenta;\n"
		":C = Sr^Sb;\n\n"
		":Cyan;\n"
		":C = Sr^Sg;\n\n"
		":Yellow;\n"
		":PP = Sr^Sb^Sg;\n\n"
		":VecC2(0,1)^VecC2(1,0)^VecC2(0,-1)^VecC2(-1,0)^VecC2(1.5,0.2);\n\n"
		":scInfo;\n";

	int i;
	int iX, iY, iWidth, iHeight;
	int iMSTrg, iMSWndStyle = 0x0000;
	bool bVizOnly         = false;
	bool bShowIntro       = true;
	void* pvIcon          = 0;
	char* pcScript        = 0;
	const char* pcCLUPath = 0;
	string sExecPath, sFile, sCLUPath, sAntiAlias;

	sExecPath = argv[0];
	int iPos = int(sExecPath.find_last_of('\\') + 1);
	sExecPath.erase(iPos, sExecPath.size() - iPos);

	for (i = 1; i < argc; i++)
	{
		if (strstr(argv[i], "--main-path="))
		{
			sCLUPath = &argv[i][12];
			if (sCLUPath.size())
			{
				pcCLUPath = sCLUPath.c_str();
			}
		}
		else if (strstr(argv[i], "--no-intro"))
		{
			bShowIntro = false;
		}
		else if (strstr(argv[i], "--viz-only"))
		{
			bVizOnly = true;
		}
		else if (strstr(argv[i], "--anti-alias="))
		{
			sAntiAlias = &argv[i][13];
			if (sscanf_s(sAntiAlias.c_str(), "%d", &iMSTrg) == 1)
			{
				if (iMSTrg >= 16)
				{
					iMSWndStyle = CLUVIZ_WNDSTYLE_MULTISAMPLE_16;
				}
				else if (iMSTrg >= 8)
				{
					iMSWndStyle = CLUVIZ_WNDSTYLE_MULTISAMPLE_8;
				}
				else if (iMSTrg >= 4)
				{
					iMSWndStyle = CLUVIZ_WNDSTYLE_MULTISAMPLE_4;
				}
				else
				{
					iMSWndStyle = 0x0000;
				}
			}
		}
		else
		{
			pcScript = argv[i];

			if (strstr(pcScript, ":") == 0)
			{
				sFile = sExecPath + pcScript;
			}
			else
			{
				sFile = pcScript;
			}

			if (strstr(pcScript, ".cbn"))
			{
				// TODO: 22.02.2009: If no intro is shown, CLUCalc crashes when run with a .cbn file that sets a texture.
				bShowIntro = true;
				bVizOnly   = true;
				#ifdef WIN32
					void* pvVal;
					string sIcon;

					sIcon = sFile;
					sIcon.erase(sIcon.find_last_of('.'), 4);
					sIcon += ".ico";

					pvVal = (void*) LoadImageA(GetModuleHandle(NULL), sIcon.c_str(), IMAGE_ICON,
						0, 0, LR_LOADFROMFILE);
					if (pvVal)
					{
//					DestroyIcon( (HICON) pvIcon );
						pvIcon = pvVal;
					}
				#endif
			}
		}
	}

	#ifdef WIN32
		RECT rectWnd;
		MONITORINFO infoMonitor;
		infoMonitor.cbSize = sizeof(MONITORINFO);

		rectWnd.left   = 50;
		rectWnd.top    = 50;
		rectWnd.right  = 100;
		rectWnd.bottom = 100;

		HMONITOR hMonitor = MonitorFromRect(&rectWnd, MONITOR_DEFAULTTONEAREST);
		GetMonitorInfo(hMonitor, &infoMonitor);

		int iScreenWidth  = infoMonitor.rcWork.right - infoMonitor.rcWork.left;
		int iScreenHeight = infoMonitor.rcWork.bottom - infoMonitor.rcWork.top;

		iX      = int(float(iScreenWidth) * 0.01f);
		iY      = int(float(iScreenHeight) * 0.03f);
		iWidth  = int((iScreenWidth - 2 * iX) / 2.5);
		iHeight = int(float(iScreenHeight - 2 * iY));

		iX    += iWidth + 10;
		iWidth = int(iScreenWidth - iWidth - 50);
		iY    += 10;

	#else
		iX      = 50;
		iY      = 50;
		iWidth  = 500;
		iHeight = 500;
	#endif

	try
	{
		Clu::Viz::Wnd::Start();

		#ifdef WIN32
			// If no script has an associated icon, then load CLUView Icon
			pvIcon = (void*) LoadIconA(s_hInstance, MAKEINTRESOURCEA(IDI_CLUCALC));
		#endif

		const size_t nMaxCodeSize = strlen(pcIntro) + 100;
		char* const pcCode = new char[nMaxCodeSize];
		if (pcCode == 0)
		{
			Clu::Viz::Wnd::InfoBox("Out of memory while starting CluViz.");
			return -1;
		}

		char pcVersion[32];
		Clu::Viz::Wnd::GetVersion(pcVersion, 32);

		if (sFile.size() == 0)
		{
			sprintf_s(pcCode, nMaxCodeSize, pcIntro, pcVersion);
		}
		else
		{
			char* pcSimpleIntro =
				"_BGColor = Black; SetImagePos(0,0,0); SetImageAlign(0.5, 0.5); :\"CluViz v%s loading script...\";";

			sprintf_s(pcCode, nMaxCodeSize, pcSimpleIntro, pcVersion);

		}

		Clu::Viz::Wnd::CreateEx2(iHandle, iX, iY, iWidth, iHeight, 0 /*"CLUCalc v" CLUVIZ_VERSION*/, pcCode, pcCLUPath,
				bVizOnly, bShowIntro, pvIcon, 0, CLUVIZ_WNDSTYLE_STD | iMSWndStyle);

		if (pcCode)
		{
			delete[] pcCode;
		}

		if (sFile.size())
		{
			Clu::Viz::Wnd::LoadScript(iHandle, sFile.c_str(), 0);
		}
	}
	catch (Clu::CIException& xEx)
	{
		char pcError[1024];
		Clu::CIString strEx = xEx.ToStringComplete();
		Clu::Viz::Wnd::GetLastError(pcError, 1024);
		Clu::CIString strMsg = pcError;
		strMsg += Clu::CIString("\n") + strEx;
		Clu::Viz::Wnd::InfoBox(strMsg.ToCString());
		return -1;
	}

	Clu::Viz::Wnd::WaitForEnd();
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int iRetVal;
	s_hInstance = hInstance;
	/* Run the standard main entry point function... */
	iRetVal = main(__argc, __argv);

	return iRetVal;
}
