////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      Encode.h
//
// summary:   Declares the encode class
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

#if !defined(_ENCODE_H__INCLUDED_)
	#define _ENCODE_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif	// _MSC_VER > 1000

#include <vector>
#include <string>
#include <time.h>
#include <sys/timeb.h>

#include "CluTec.Viz.Draw\OGLImage.h"

//using namespace std;

#ifdef WIN32
		typedef struct _timeb TTimeStruct;
#ifndef GETTIME
			#define GETTIME(S) _ftime_s(S)
#endif
#else
		typedef struct timeb TTimeStruct;
		#define GETTIME(S) ftime(S)
#endif

////////////////////////////////////////////////////////////////////////////////////
// Encode Class
//
// Encodes data and reads/writes encoded files
//
	class CEncode
	{
	public:

		CEncode();
		virtual ~CEncode();

		// Generate a random seed
		unsigned GenRanSeed();

		// Encode Memory
		bool EncodeMemory(unsigned uSeed1, unsigned uSeed2, Mem<char>& mData);
		// Improved routine to encode data. Designed to encode image data.
		bool EncodeMemory_v2(unsigned uSeed1, unsigned uSeed2, void* pData, unsigned uSize);

		// Decode Memory
		bool DecodeMemory(unsigned uSeed1, unsigned uSeed2, Mem<char>& mData);
		// Decodes data encoded with EncodeMemory_v2
		bool DecodeMemory_v2(unsigned uSeed1, unsigned uSeed2, void* pData, unsigned uSize);

		// Encode data and write to file with given header
		bool WriteFile(const string& sFilename, const string& sFileID, const void* pvData, size_t uDataLen, const char* pcPass = 0);

		// Read from file and decode data
		bool ReadFile(const string& sFilename, string& sFileID,	Mem<char>& mData, const char* pcPass = 0);

		// Encode data and write to file with given header
		bool WriteFile_v2(const string& sFilename, const string& sFileID, const void* pvData, unsigned uDataLen, int iCompressLevel = 0, unsigned uKey1 = 0, unsigned uKey2 = 0);

		// Read from file and decode data
		bool ReadFile_v2(const string& sFilename, string& sFileID, Mem<char>& mData, unsigned uKey1 = 0, unsigned uKey2 = 0);

		// Read file ID
		bool ReadFileID(const string& sFilename, string& sFileID);

		// Encode Image Memory
		bool EncodeImage_v1(unsigned uSeed1, unsigned uSeed2, COGLImage& rImage);

		// Decode Image Memory
		bool DecodeImage_v1(unsigned uSeed1, unsigned uSeed2, COGLImage& rImage);

		// Encode image variable. Is used by write var function
		// Encoded and compressed data is returned in mData
		// mData: Encoded and compressed data is appended to contents of mData
		// rImage: The source image
		//bool EncodeImageVar_v1( Mem<char> &mData, const COGLImage& rImage, int iCompressLevel, unsigned uSeed1, unsigned uSeed2 );

		// Encode image and write to file with given header
		bool WriteImage(const string& sFilename, const string& sFileID,	COGLImage& rImage, int iCompressLevel = 0, unsigned uKey1 = 0, unsigned uKey2 = 0);

		// Read image from file and decode it
		bool ReadImage(const string& sFilename, string& sFileID, COGLImage& rImage, unsigned uKey1 = 0, unsigned uKey2 = 0);

		// Get Error String
		string GetError()
		{
			return m_sError;
		}

	protected:

		Rand m_Random, m_Random2;
		TTimeStruct m_tmTime;

		string m_sEncodeVersion;
		string m_sError;
	};

#endif
