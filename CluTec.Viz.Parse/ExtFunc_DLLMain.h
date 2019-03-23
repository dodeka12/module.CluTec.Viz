////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      ExtFunc_DLLMain.h
//
// summary:   Declares the extent function DLL main class
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


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}

extern "C" bool InitModule()
{
	#if defined(_HAS_DLL_INIT_)
	#pragma message( "Use Initialization" )
		return Initialize();
	#else
		return true;
	#endif
}

extern "C" bool FinalizeModule()
{
	#if defined(_HAS_DLL_INIT_)
	#pragma message( "Use Initialization" )
		return Finalize();
	#else
		return true;
	#endif
}

extern "C" bool InitViz()
{
	#if defined(_HAS_OGL_INIT_)
	#pragma message( "Use OGL Init" )
		return InitOGL();
	#else
		return true;
	#endif
}

extern "C" bool GetFuncList(const SMapExtFunc** pMapFunc, int* piFuncCnt)
{
	*pMapFunc = pFuncList;

	int i = 0;
	while (pFuncList[i].pcName)
		++i;

	*piFuncCnt = i;

	return true;
}
