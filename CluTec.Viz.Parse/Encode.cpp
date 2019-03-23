////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      Encode.cpp
//
// summary:   Implements the encode class
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
#include "Encode.h"
#include "zlib\zlib.h"

CEncode::CEncode()
{
	m_sEncodeVersion = "0001";

	GETTIME(&m_tmTime);
	m_Random.seed3(unsigned(m_tmTime.millitm));

	GETTIME(&m_tmTime);
	m_Random2.seed3(unsigned(m_tmTime.millitm));
}

CEncode::~CEncode()
{
}

///////////////////////////////////////////////////////////////////////////
// Generate a random seed

unsigned CEncode::GenRanSeed()
{
	unsigned uSeed;

	GETTIME(&m_tmTime);
	uSeed  = unsigned(m_tmTime.time);
	uSeed ^= m_Random.ran3u();

	return uSeed;
}

///////////////////////////////////////////////////////////////////////////
// Encode Image Memory
bool CEncode::EncodeImage_v1(unsigned uSeed1, unsigned uSeed2, COGLImage& rImage)
{
	unsigned uByteCnt;
	int iWidth, iHeight;
	int iImgType, iDataType, iBPP;

	rImage.GetSize(iWidth, iHeight);
	rImage.GetType(iImgType, iDataType, iBPP);

	uByteCnt = unsigned(iWidth) * unsigned(iHeight) * unsigned(iBPP);

	::LockImageAccess();
	bool bRes = EncodeMemory_v2(uSeed1, uSeed2, (void*) rImage.GetDataPtr(), uByteCnt);
	::UnlockImageAccess();
	return bRes;
}

///////////////////////////////////////////////////////////////////////////
// Encode image variable. Is used by write var function
// Encoded and compressed data is returned in mData
// mData: Encoded and compressed data is appended to contents of mData
// rImage: The source image

//bool CEncode::EncodeImageVar_v1( Mem<char> &mData, const COGLImage& rImage, int iCompressLevel, unsigned uSeed1, unsigned uSeed2 )
//{
//	int iWidth, iHeight;
//	int iImgType, iDataType, iBPP;
//	unsigned uDataIdx;
//	unsigned uByteCnt;
//	unsigned uSeed1, uSeed2, uWriteSeed1, uWriteSeed2;
//
//	uSeed1 = (uWriteSeed1 = GenRanSeed()) ^ uKey1;
//	Sleep( 2 );
//	uSeed2 = (uWriteSeed2 = GenRanSeed()) ^ uKey2;
//
//	unsigned uItemLen = sizeof( unsigned );
//
//	unsigned uHeadDataCnt = 9;
//	unsigned uHeadLen = uHeadDataCnt * uItemLen;
//
//	uDataIdx = mData.Count();
//	mData.Add( uHeadLen );
//
//	rImage.GetSize( iWidth, iHeight );
//	rImage.GetType( iImgType, iDataType, iBPP );
//	uByteCnt = unsigned(iWidth) * unsigned(iHeight) * unsigned(iBPP);
//
//	unsigned *puData = ((unsigned *) &mData[uDataIdx]);
//	puData[0] = uHeadDataCnt * uItemLen; // number of bytes in data header
//	puData[1] = iWidth;
//	puData[2] = iHeight;
//	puData[3] = iImgType;
//	puData[4] = iDataType;
//	puData[5] = iBPP;
//	puData[6] = uWriteSeed1;
//	puData[7] = uWriteSeed2;
//
//	unsigned uRet;
//	unsigned uBufCnt = unsigned( ceil(double(uByteCnt) * 1.015) ) + 12;
//	Mem<char> mBuf;
//	mBuf.Set( uBufCnt );
//
//	if ( (uRet = ::compress2( (Bytef *) mBuf.Data(), (uLongf *) &uBufCnt, (Bytef *) rImage.GetDataPtr(), uByteCnt, iCompressLevel )) != Z_OK )
//	{
//		m_sError = "Error compressing image.";
//		return false;
//	}
//
//	// Write actual length of data buffer
//	puData[8] = uBufCnt;
//
//	if ( !EncodeMemory_v2( uSeed1, uSeed2, mBuf.Data(), uBufCnt ) )
//	{
//		m_sError = "Error encoding image.";
//		return false;
//	}
//
//	mData.Add( uBufCnt );
//	memcpy( &mData[ uDataIdx + uHeadLen ], mBuf.Data(), uBufCnt );
//
//	return true;
//}

///////////////////////////////////////////////////////////////////////////
// Improved routine to encode data. Designed to encode image data.

bool CEncode::EncodeMemory_v2(unsigned uSeed1, unsigned uSeed2, void* pData, unsigned uSize)
{
	unsigned uIdx, puIdx[4], uCnt, uHalfCnt, uQuatCnt;

	uCnt     = unsigned(uSize) / sizeof(unsigned);
	uHalfCnt = uCnt >> 1;
	uQuatCnt = uCnt >> 2;

	unsigned puVal[4];
	unsigned* puData = (unsigned*) pData;

	m_Random.seed3(uSeed1);
	m_Random2.seed3(uSeed2);
	for (puIdx[0] = 0, puIdx[1] = uHalfCnt - 1, puIdx[2] = uHalfCnt, puIdx[3] = uCnt - 1;
	     puIdx[0] < uQuatCnt;
	     ++puIdx[0], --puIdx[1], ++puIdx[2], --puIdx[3])
	{
		uIdx = m_Random2.ran3u() % 4;

		puVal[0] = puData[puIdx[(uIdx + 0) % 4]];
		puVal[1] = puData[puIdx[(uIdx + 1) % 4]];
		puVal[2] = puData[puIdx[(uIdx + 2) % 4]];
		puVal[3] = puData[puIdx[(uIdx + 3) % 4]];

		puData[puIdx[0]] = (((puVal[0]  & 0xF000000F)) |
				    ((puVal[1]  & 0x00000FF0)) |
				    (((~puVal[2]) & 0x000FF000)) |
				    ((puVal[3]  & 0x0FF00000))
				    ) ^ m_Random.ran3u();

		puData[puIdx[1]] = ((((~puVal[0]) & 0x00000FF0)) |
				    ((puVal[1]  & 0x000FF000)) |
				    ((puVal[2]  & 0x0FF00000)) |
				    ((puVal[3]  & 0xF000000F))
				    ) ^ m_Random.ran3u();

		puData[puIdx[2]] = (((puVal[0]  & 0x000FF000)) |
				    (((~puVal[1]) & 0x0FF00000)) |
				    ((puVal[2]  & 0xF000000F)) |
				    ((puVal[3]  & 0x00000FF0))
				    ) ^ m_Random.ran3u();

		puData[puIdx[3]] = (((puVal[0]  & 0x0FF00000)) |
				    ((puVal[1]  & 0xF000000F)) |
				    ((puVal[2]  & 0x00000FF0)) |
				    (((~puVal[3]) & 0x000FF000))
				    ) ^ m_Random.ran3u();
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////
// Decode Image Memory
bool CEncode::DecodeImage_v1(unsigned uSeed1, unsigned uSeed2, COGLImage& rImage)
{
	unsigned uByteCnt;
	int iWidth, iHeight;
	int iImgType, iDataType, iBPP;

	rImage.GetSize(iWidth, iHeight);
	rImage.GetType(iImgType, iDataType, iBPP);

	uByteCnt = unsigned(iWidth) * unsigned(iHeight) * unsigned(iBPP);

	::LockImageAccess();
	bool bRes = DecodeMemory_v2(uSeed1, uSeed2, (void*) rImage.GetDataPtr(), uByteCnt);
	::UnlockImageAccess();
	return bRes;
}

///////////////////////////////////////////////////////////////////////////
// Decodes data encoded with EncodeMemory_v2

bool CEncode::DecodeMemory_v2(unsigned uSeed1, unsigned uSeed2, void* pData, unsigned uSize)
{
	unsigned uIdx, puIdx[4], uCnt, uHalfCnt, uQuatCnt;

	uCnt     = unsigned(uSize) / sizeof(unsigned);
	uHalfCnt = uCnt >> 1;
	uQuatCnt = uCnt >> 2;

	unsigned puVal[4];
	unsigned* puData = (unsigned*) pData;

	m_Random.seed3(uSeed1);
	m_Random2.seed3(uSeed2);
	for (puIdx[0] = 0, puIdx[1] = uHalfCnt - 1, puIdx[2] = uHalfCnt, puIdx[3] = uCnt - 1;
	     puIdx[0] < uQuatCnt;
	     ++puIdx[0], --puIdx[1], ++puIdx[2], --puIdx[3])
	{
		uIdx = m_Random2.ran3u() % 4;

		puVal[0] = puData[puIdx[0]] ^ m_Random.ran3u();
		puVal[1] = puData[puIdx[1]] ^ m_Random.ran3u();
		puVal[2] = puData[puIdx[2]] ^ m_Random.ran3u();
		puVal[3] = puData[puIdx[3]] ^ m_Random.ran3u();

		puData[puIdx[(uIdx + 0) % 4]]
			= (puVal[0]  & 0xF000000F) |
			  ((~puVal[1]) & 0x00000FF0) |
			  (puVal[2]  & 0x000FF000) |
			  (puVal[3]  & 0x0FF00000);

		puData[puIdx[(uIdx + 1) % 4]]
			= (puVal[0]  & 0x00000FF0) |
			  (puVal[1]  & 0x000FF000) |
			  ((~puVal[2]) & 0x0FF00000) |
			  (puVal[3]  & 0xF000000F);

		puData[puIdx[(uIdx + 2) % 4]]
			= ((~puVal[0]) & 0x000FF000) |
			  (puVal[1]  & 0x0FF00000) |
			  (puVal[2]  & 0xF000000F) |
			  (puVal[3]  & 0x00000FF0);

		puData[puIdx[(uIdx + 3) % 4]]
			= (puVal[0]  & 0x0FF00000) |
			  (puVal[1]  & 0xF000000F) |
			  (puVal[2]  & 0x00000FF0) |
			  ((~puVal[3]) & 0x000FF000);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 	Works only for Images having Byte Count <= UNSIGNED_MAX
/// 	 Encode image and write to file with given header
///
/// 	 Two random seeds are generated to encode the data. These seeds are written to the file header. If uKey1 and uKey2 are
/// 	 specified the random seeds are written to the file header but the encoding is done with the random seed XOR the keys.
/// </summary>
///
/// <param name="sFilename">	 	Filename of the file. </param>
/// <param name="sFileID">		 	Identifier for the file. </param>
/// <param name="rImage">		 	[in,out] The image. </param>
/// <param name="iCompressLevel">	Zero-based index of the compress level. </param>
/// <param name="uKey1">		 	The first u key. </param>
/// <param name="uKey2">		 	The second u key. </param>
///
/// <returns>	True if it succeeds, false if it fails. </returns>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CEncode::WriteImage(const string& sFilename, const string& sFileID, COGLImage& rImage, int iCompressLevel, unsigned uKey1, unsigned uKey2)
{
	m_sError = "";

	int iWidth, iHeight;
	int iImgType, iDataType, iBPP;
	size_t uByteCnt;
	unsigned uSeed1, uSeed2, uWriteSeed1, uWriteSeed2;

	uSeed1 = (uWriteSeed1 = GenRanSeed()) ^ uKey1;
	Sleep(2);
	uSeed2 = (uWriteSeed2 = GenRanSeed()) ^ uKey2;

	unsigned uItemLen = sizeof(unsigned);
	unsigned uIDLen   = unsigned(sFileID.size() + m_sEncodeVersion.size() + 2);

	unsigned uHeadDataCnt = 9;
	unsigned uHeadLen     = uIDLen + uHeadDataCnt * uItemLen;

	Mem<char> mHead;
	mHead.Set(uHeadLen);

	memcpy(mHead.Data(), sFileID.c_str(), sFileID.size() + 1);
	memcpy(&mHead[sFileID.size() + 1], m_sEncodeVersion.c_str(), m_sEncodeVersion.size() + 1);

	rImage.GetSize(iWidth, iHeight);
	rImage.GetType(iImgType, iDataType, iBPP);
	uByteCnt = unsigned(iWidth) * unsigned(iHeight) * unsigned(iBPP);
	// TODO:, delete when zlib can handle huge files / buffer sizes (64bit unsigned long)
	if (uByteCnt < ULONG_MAX)
	{
		return false;
	}
	unsigned* puData = ((unsigned*) &mHead[uIDLen]);
	puData[0] = uHeadDataCnt * uItemLen;	// number of bytes in data header
	puData[1] = iWidth;
	puData[2] = iHeight;
	puData[3] = iImgType;
	puData[4] = iDataType;
	puData[5] = iBPP;
	puData[6] = uWriteSeed1;
	puData[7] = uWriteSeed2;

	unsigned uRet;
	unsigned uBufCnt = unsigned(ceil(double(uByteCnt) * 1.015)) + 12;
	Mem<char> mBuf;
	mBuf.Set(uBufCnt);

	::LockImageAccess();
	if ((uRet = ::compress2((Bytef*) mBuf.Data(), (uLongf*) &uBufCnt, (Bytef*) rImage.GetDataPtr(), uLong(uByteCnt), iCompressLevel)) != Z_OK)
	{
		::UnlockImageAccess();
		m_sError = "Error compressing image.";
		return false;
	}
	::UnlockImageAccess();

	// Write actual length of data buffer
	puData[8] = uBufCnt;

	if (!EncodeMemory_v2(uSeed1, uSeed2, mBuf.Data(), uBufCnt))
	{
		m_sError = "Error encoding image.";
		return false;
	}

	FILE* pFile;
	fopen_s(&pFile, sFilename.c_str(), "wb");

	if (pFile)
	{
		fwrite(mHead.Data(), 1, mHead.Count(), pFile);
		fwrite(mBuf.Data(), 1, uBufCnt, pFile);
		fclose(pFile);
	}
	else
	{
		m_sError = "Could not open file for writing.";
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////
// Read image from file and decode it

bool CEncode::ReadImage(const string& sFilename, string& sFileID, COGLImage& rImage, unsigned uKey1, unsigned uKey2)
{
	m_sError = "";

	int iWidth, iHeight;
	int iImgType, iDataType, iBPP;
	unsigned uByteCnt, uBufCnt, uRet;
	unsigned uSeed1, uSeed2, uReadSeed1, uReadSeed2;

	unsigned uHeadLen;

	Mem<char> mData;

	FILE* pFile;
	fopen_s(&pFile, sFilename.c_str(), "rb");

	if (!pFile)
	{
		m_sError = "Error reading from file.";
		return false;
	}

	// Read Data from file
	const unsigned cuReadLen = 1 << 22;
	char* pcInput            = new char[cuReadLen];
	if (!pcInput)
	{
		m_sError = "Out of memory while reading file.";
		return false;
	}

	size_t uPos = 0, uCnt;
	mData.Set(0);

	while ((uCnt = fread(pcInput, 1, cuReadLen, pFile)) == cuReadLen)
	{
		mData.Add(uCnt);
		memcpy(&mData[uPos], pcInput, uCnt);
		uPos += uCnt;
	}

	fclose(pFile);

	mData.Add(uCnt);
	memcpy(&mData[uPos], pcInput, uCnt);
	delete[] pcInput;
	////////////////////////

	char* pcVal = mData.Data();

	sFileID = pcVal;
	pcVal  += strlen(pcVal) + 1;

	if (strcmp(pcVal, m_sEncodeVersion.c_str()) != 0)
	{
		m_sError = "Invalid file version.";
		return false;
	}

	pcVal += strlen(pcVal) + 1;
	unsigned* puData = (unsigned*) pcVal;

	uHeadLen = puData[0];
	if (uHeadLen < 9 * sizeof(unsigned))
	{
		m_sError = "Invalid file version.";
		return false;
	}

	iWidth     = puData[1];
	iHeight    = puData[2];
	iImgType   = puData[3];
	iDataType  = puData[4];
	iBPP       = puData[5];
	uReadSeed1 = puData[6];
	uReadSeed2 = puData[7];
	uBufCnt    = puData[8];

	uSeed1 = uReadSeed1 ^ uKey1;
	uSeed2 = uReadSeed2 ^ uKey2;

	rImage.ResizeCanvas(1, 1);
	rImage.ConvertType(iImgType, iDataType);
	rImage.ResizeCanvas(iWidth, iHeight);
	uByteCnt = unsigned(iWidth) * unsigned(iHeight) * unsigned(iBPP);

	pcVal += uHeadLen;

	if (!DecodeMemory_v2(uSeed1, uSeed2, pcVal, uBufCnt))
	{
		m_sError = "Error decoding image.";
		return false;
	}

	::LockImageAccess();
	if ((uRet = ::uncompress((Bytef*) rImage.GetDataPtr(), (uLongf*) &uByteCnt, (Bytef*) pcVal, uBufCnt)) != Z_OK)
	{
		::UnlockImageAccess();
		m_sError = "Error uncompressing image.";
		return false;
	}
	::UnlockImageAccess();

	return true;
}

///////////////////////////////////////////////////////////////////////////
// Encode Memory. Memory is replaced with encoded version

bool CEncode::EncodeMemory(unsigned uSeed1, unsigned uSeed2, Mem<char>& mData)
{
	unsigned uIdx, uShift;
	unsigned uPadLen = mData.Count() % sizeof(unsigned);
	mData.Add(uPadLen);
	unsigned uTotalLen = unsigned(mData.Count());
	unsigned uVal;

	for (uIdx = uTotalLen - uPadLen; uIdx < uTotalLen; uIdx++)
	{
		mData[uIdx] = 0;
	}

	unsigned uCnt    = uTotalLen / sizeof(unsigned);
	unsigned* puData = (unsigned*) mData.Data();

	m_Random.seed3(uSeed1);
	for (uIdx = 0; uIdx < uCnt; uIdx++)
	{
		puData[uIdx] ^= m_Random.ran3u();
	}

	m_Random.seed3(uSeed2);
	for (uIdx = 0; uIdx < uCnt; uIdx++)
	{
		uShift = m_Random.ran3u() % 32;

		uVal         = puData[uIdx];
		puData[uIdx] = (uVal << uShift) | (uVal >> (32 - uShift));
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////
// Decode Memory. Memory is replaced with decoded version

bool CEncode::DecodeMemory(unsigned uSeed1, unsigned uSeed2, Mem<char>& mData)
{
	unsigned uIdx, uShift;
	unsigned uTotalLen = unsigned(mData.Count());
	unsigned uVal;

	unsigned uCnt    = unsigned(uTotalLen / sizeof(unsigned));
	unsigned* puData = (unsigned*) mData.Data();

	m_Random.seed3(uSeed2);
	for (uIdx = 0; uIdx < uCnt; uIdx++)
	{
		uShift = m_Random.ran3u() % 32;

		uVal         = puData[uIdx];
		puData[uIdx] = (uVal >> uShift) | (uVal << (32 - uShift));
	}

	m_Random.seed3(uSeed1);
	for (uIdx = 0; uIdx < uCnt; uIdx++)
	{
		puData[uIdx] ^= m_Random.ran3u();
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////
// Encode data and write to file with given header

bool CEncode::WriteFile(const string& sFilename, const string& sFileID, const void* pvData, size_t nDataLen, const char* pcPass)
{
	m_sError = "";

	Mem<char> mData;
	unsigned uSeed1, uSeed2, uWriteSeed1, uWriteSeed2;

	mData.Set(nDataLen);
	memcpy(mData.Data(), pvData, nDataLen);

	uWriteSeed1 = uSeed1 = GenRanSeed();
	uWriteSeed2 = uSeed2 = GenRanSeed();

	if (pcPass)
	{
		if (strlen(pcPass) != 8)
		{
			m_sError = "Invalid pass phrase.";
			return false;
		}

		unsigned* puPass = (unsigned*) pcPass;
		uSeed1 = puPass[0];
		uSeed2 = puPass[1];
	}

	if (!EncodeMemory(uSeed1, uSeed2, mData))
	{
		m_sError = "Error encoding data.";
		return false;
	}

	unsigned uItemLen = sizeof(unsigned);
	unsigned uIDLen   = unsigned(sFileID.size() + m_sEncodeVersion.size() + 2);

	unsigned uHeadDataCnt = 4;
	unsigned uHeadLen     = uIDLen + uHeadDataCnt * uItemLen;

	mData.Insert(0, uHeadLen);
	memcpy(mData.Data(), sFileID.c_str(), sFileID.size() + 1);
	memcpy(&mData[sFileID.size() + 1], m_sEncodeVersion.c_str(), m_sEncodeVersion.size() + 1);

	unsigned* puHeadData = (unsigned*) &mData[uIDLen];

	puHeadData[0] = unsigned(mData.Count());
	puHeadData[1] = uHeadLen;
	puHeadData[2] = uWriteSeed1;
	puHeadData[3] = uWriteSeed2;

	FILE* pFile;
	fopen_s(&pFile, sFilename.c_str(), "wb");

	if (pFile)
	{
		fwrite(mData.Data(), 1, mData.Count(), pFile);
		fclose(pFile);
	}
	else
	{
		m_sError = "Could not open file for writing.";
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////
// Read from file and decode data

bool CEncode::ReadFile(const string& sFilename, string& sFileID, Mem<char>& mData, const char* pcPass)
{
	m_sError = "";

	if (pcPass && (strlen(pcPass) != 8))
	{
		m_sError = "Invalid pass phrase.";
		return false;
	}

	FILE* pFile;
	fopen_s(&pFile, sFilename.c_str(), "rb");

	if (!pFile)
	{
		m_sError = "Error reading from file.";
		return false;
	}

	// Read Data from file
	const unsigned cuReadLen = 1024;
	char pcInput[cuReadLen];
	unsigned uPos = 0, uCnt;
	mData.Set(0);

	while ((uCnt = (unsigned) fread(pcInput, 1, cuReadLen, pFile)) == cuReadLen)
	{
		mData.Add(uCnt);
		memcpy(&mData[uPos], pcInput, uCnt);
		uPos += uCnt;
	}

	fclose(pFile);

	mData.Add(uCnt);
	memcpy(&mData[uPos], pcInput, uCnt);
	////////////////////////

	char* pcVal = mData.Data();

	sFileID = pcVal;
	pcVal  += strlen(pcVal) + 1;

	if (strcmp(pcVal, m_sEncodeVersion.c_str()) != 0)
	{
		m_sError = "Invalid file version.";
		return false;
	}

	unsigned* puHead = (unsigned*) &pcVal[strlen(pcVal) + 1];

	unsigned uHeadLen = puHead[1];
	unsigned uSeed1   = puHead[2];
	unsigned uSeed2   = puHead[3];

	if (pcPass)
	{
		unsigned* puPass = (unsigned*) pcPass;
		uSeed1 = puPass[0];
		uSeed2 = puPass[1];
	}

	mData.Del(0, uHeadLen);

	if (!DecodeMemory(uSeed1, uSeed2, mData))
	{
		m_sError = "Error decoding data.";
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////
// Encode data and write to file with given header

bool CEncode::WriteFile_v2(const string& sFilename, const string& sFileID, const void* pvData, unsigned nDataLen, int iCompressLevel, unsigned uKey1, unsigned uKey2)
{
	m_sError = "";

	Mem<char> mData;
	unsigned uSeed1, uSeed2, uWriteSeed1, uWriteSeed2;

	uSeed1 = (uWriteSeed1 = GenRanSeed()) ^ uKey1;
	uSeed2 = (uWriteSeed2 = GenRanSeed()) ^ uKey2;

	unsigned uRet;
	unsigned uBufCnt = unsigned(ceil(double(nDataLen) * 1.015)) + 12;
	Mem<char> mBuf;
	mBuf.Set(uBufCnt);

	// TODO:, delete when zlib can handle huge files / buffer sizes (64bit unsigned long)
	if (nDataLen > ULONG_MAX)
	{
		return false;
	}
	if ((uRet = ::compress2((Bytef*) mBuf.Data(), (uLongf*) &uBufCnt, (Bytef*) pvData, nDataLen, iCompressLevel)) != Z_OK)
	{
		m_sError = "Error compressing image.";
		return false;
	}

	if (!EncodeMemory_v2(uSeed1, uSeed2, mBuf.Data(), uBufCnt))
	{
		m_sError = "Error encoding image.";
		return false;
	}

	unsigned uItemLen = sizeof(size_t);
	size_t uIDLen     = sFileID.size() + m_sEncodeVersion.size() + 2;

	unsigned uHeadDataCnt = 5;
	unsigned uHeadLen     = uHeadDataCnt * uItemLen;

	mData.Set(uIDLen + uHeadLen);
	memcpy(mData.Data(), sFileID.c_str(), sFileID.size() + 1);
	memcpy(&mData[sFileID.size() + 1], m_sEncodeVersion.c_str(), m_sEncodeVersion.size() + 1);

	unsigned* puHeadData = (unsigned*) &mData[uIDLen];

	puHeadData[0] = uHeadLen;		// Length of header in bytes
	puHeadData[1] = uBufCnt;		// Length of data stream in bytes
	puHeadData[2] = nDataLen;		// Length of uncompressed data stream in bytes
	puHeadData[3] = uWriteSeed1;
	puHeadData[4] = uWriteSeed2;

	FILE* pFile;
	fopen_s(&pFile, sFilename.c_str(), "wb");

	if (pFile)
	{
		fwrite(mData.Data(), 1, mData.Count(), pFile);
		fwrite(mBuf.Data(), 1, uBufCnt, pFile);
		fclose(pFile);
	}
	else
	{
		m_sError = "Could not open file for writing.";
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////
// Read from file and decode data

bool CEncode::ReadFile_v2(const string& sFilename, string& sFileID, Mem<char>& mData, unsigned uKey1, unsigned uKey2)
{
	m_sError = "";

	unsigned uRet;
	unsigned uSeed1, uSeed2, uReadSeed1, uReadSeed2;
	unsigned uHeadLen;
	Mem<char> mBuf;

	FILE* pFile;
	fopen_s(&pFile, sFilename.c_str(), "rb");

	if (!pFile)
	{
		m_sError = "Error reading from file.";
		return false;
	}

	// Read Data from file
	const unsigned cuReadLen = 1 << 22;
	char* pcInput            = new char[cuReadLen];
	if (!pcInput)
	{
		m_sError = "Out of memory while reading file.";
		return false;
	}

	unsigned uPos = 0, uCnt, uBufSize, uDataSize;
	mBuf.Set(0);

	while ((uCnt = unsigned(fread(pcInput, 1, cuReadLen, pFile))) == cuReadLen)
	{
		mBuf.Add(uCnt);
		memcpy(&mBuf[uPos], pcInput, uCnt);
		uPos += uCnt;
	}

	fclose(pFile);

	mBuf.Add(uCnt);
	memcpy(&mBuf[uPos], pcInput, uCnt);
	delete[] pcInput;
	////////////////////////

	char* pcVal = mBuf.Data();

	sFileID = pcVal;
	pcVal  += strlen(pcVal) + 1;

	if (strcmp(pcVal, m_sEncodeVersion.c_str()) != 0)
	{
		m_sError = "Invalid file version.";
		return false;
	}

	pcVal += strlen(pcVal) + 1;
	unsigned* puData = (unsigned*) pcVal;

	uHeadLen = puData[0];
	if (uHeadLen < 5 * sizeof(unsigned))
	{
		m_sError = "Invalid file version.";
		return false;
	}

	uBufSize   = puData[1];
	uDataSize  = puData[2];
	uReadSeed1 = puData[3];
	uReadSeed2 = puData[4];

	uSeed1 = uReadSeed1 ^ uKey1;
	uSeed2 = uReadSeed2 ^ uKey2;

	pcVal += uHeadLen;

	if (!DecodeMemory_v2(uSeed1, uSeed2, pcVal, uBufSize))
	{
		m_sError = "Error decoding image.";
		return false;
	}

	mData.Set(uDataSize);

	if ((uRet = ::uncompress((Bytef*) mData.Data(), (uLongf*) &uDataSize, (Bytef*) pcVal, uBufSize)) != Z_OK)
	{
		m_sError = "Error uncompressing image.";
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////
// Read file ID

bool CEncode::ReadFileID(const string& sFilename, string& sFileID)
{
	m_sError = "";

	FILE* pFile;
	fopen_s(&pFile, sFilename.c_str(), "rb");

	if (!pFile)
	{
		m_sError = "Error reading from file.";
		return false;
	}

	// Read Data from file
	const unsigned cuReadLen = 1024;
	char pcInput[cuReadLen];
	unsigned uCnt;

	if ((uCnt = (unsigned) fread(pcInput, 1, cuReadLen, pFile)) == 0)
	{
		m_sError = "Error reading from file.";
		return false;
	}
	fclose(pFile);

	pcInput[cuReadLen - 1] = 0;
	sFileID                = pcInput;

	return true;
}
