////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.ImgRepo.rtl
// file:      CLUImgRep.h
//
// summary:   Declares the clu image rep class
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

#ifdef CLUIR_EXPORTS
	#define CLUIR_API __declspec(dllexport)
#else
	#define CLUIR_API __declspec(dllimport)
#endif

CLUIR_API void InitImageRepository(void* pvRep);
CLUIR_API void ReleaseImageRepository();
CLUIR_API void* GetImageRepositoryPtr();

CLUIR_API bool LockImageAccess(int iWait = 0xFFFFFFFF);
CLUIR_API void UnlockImageAccess();
