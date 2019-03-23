////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLShader.h
//
// summary:   Declares the ogl shader class
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

//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLSHADER_H__INCLUDED_)
	#define AFX_OGLSHADER_H__INCLUDED_

#include "ImageReference.h"
#include "OGLBaseElement.h"
//#include "OGLVertex.h"
//#include "OGLColor.h"
//#include "OGLImage.h"

#include <vector>
#include <string>

	using namespace std;

	class CLUDRAW_API COGLShader : public COGLBaseElement
	{
	public:

		typedef unsigned char uchar;

		class CUniVarBase
		{
		public:

			CUniVarBase(GLenum eType) : m_eType(eType) { };

			int GetVarLocation(GLuint uProgID, const string& sVarName);
			GLenum Type() { return m_eType; }

			virtual void SetElCount(int iElCnt) = 0;
			virtual size_t Count() = 0;
			virtual bool Apply(GLuint uProgID, const string& sVarName) = 0;

		protected:

			GLenum m_eType;
		};

		template<class T, unsigned int uElSize>
		class CStaticArray
		{
		public:

			T Value[uElSize];
		};

		template<class T, unsigned int uElSize, GLenum eType>
		class CUniVar : public CUniVarBase
		{
		public:

			CUniVar(int iElCnt = 1) : CUniVarBase(eType)
			{
				SetElCount(iElCnt);
			}

			bool Set(unsigned int uElIdx, T* pData, unsigned int uCnt = 1)
			{
				if (uElIdx + uCnt - 1 >= m_vecVal.size())
				{
					return false;
				}

				memcpy(&m_vecVal[uElIdx], pData, uCnt * uElSize * sizeof(T));

				return true;
			}

			void SetElCount(int iElCnt)
			{
				if (iElCnt <= 0)
				{
					iElCnt = 1;
				}

				m_vecVal.resize(iElCnt);
				for (int i = 0; i < iElCnt; ++i)
				{
					memset(&m_vecVal[i], 0, uElSize * sizeof(T));
				}
			}

			size_t Count() { return m_vecVal.size(); }
			virtual bool Apply(GLuint uProgID, const string& sVarName);

		protected:

			vector<CStaticArray<T, uElSize> > m_vecVal;
		};

	public:

		COGLShader(void);
		COGLShader(const COGLShader& oglShader);
		virtual ~COGLShader(void);

		virtual COGLBaseElement* Copy()
		{
			return (COGLBaseElement*) new COGLShader(*this);
		}

		COGLShader& operator=(const COGLShader& oglShader);

		const string& GetVSCompileLog() const
		{ return m_sVSCompileLog; }

		const string& GetFSCompileLog() const
		{ return m_sFSCompileLog; }

		const string& GetGSCompileLog() const
		{ return m_sGSCompileLog; }

		const string& GetLinkLog() const
		{ return m_sProgLinkLog; }

		bool IsVSOK() { return m_bVSOK; }
		bool IsFSOK() { return m_bFSOK; }
		bool IsGSOK() { return m_bGSOK; }
		bool IsProgOK() { return m_bProgOK; }

		void Enable(bool bVal = true) { m_bEnabled = bVal; }
		bool IsEnabled() { return m_bEnabled; }

		void EnableForPicking(bool bVal = true) { m_bEnabledForPicking = bVal; }
		bool IsEnabledForPicking() { return m_bEnabledForPicking; }

		// Compile & Link Vertex and Fragment Shader
		bool Build(const vector<string>& vecVSCode, const vector<string>& vecFSCode);

		// Compile & Link Vertex, Fragment and Geometry Shader
		bool Build(const vector<string>& vecVSCode, const vector<string>& vecFSCode, const vector<string>& vecGeoCode);

		// Is Uniform Variable available?
		bool HasUniformVar(const string& sVarName)
		{
			if (m_mapUniformVar.find(sVarName) == m_mapUniformVar.end())
			{
				return false;
			}
			else
			{
				return true;
			}
		}

		// Get Type String of uniform variable
		bool GetUniformVarType(string& sVarType, const string& sVarName);
		// Get size of variable array
		bool GetUniformVarArraySize(unsigned int& uCnt, const string& sVarName);

		// Set Uniform Variable
		template<class T, unsigned int uElSize>
		bool SetUniformVar(const string& sVarName, T* pVal, unsigned int uIdx = 0, unsigned int uElCount = 1);

		bool SetUniformVarSampler(const string& sVarName, uint* puVal, uint uIdx = 0, uint uElCount = 1);

		template<unsigned int uMatSize>
		bool SetUniformVarMatrix(const string& sVarName, float* pVal, uint uIdx = 0, uint uElCount = 1);

		unsigned GetVertexShaderVersion() { return m_uVertexShaderVersion; }
		unsigned GetFragmentShaderVersion() { return m_uFragmentShaderVersion; }
		unsigned GetGeometryShaderVersion() { return m_uGeometryShaderVersion; }
		unsigned GetShaderGlId() { return m_uProgID; }

		// Apply Shader
		bool Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData);

	protected:

		void ResetVars();
		void ResetShader();
		void ResetUniformVarMap();
		bool CreateUniformVarMap();
		void ApplyUniformVarList();

		bool SetShaderSource(GLuint uID, const vector<string>& vecCode, unsigned& uMaxVersion);
		bool CompileShader(GLuint& uID, GLuint uType, const vector<string>& vecCode, string& sInfoLog, unsigned& uMaxVersion);

		void TellParentContentChanged();

	protected:

		GLuint m_uVSID;
		GLuint m_uFSID;
		GLuint m_uGSID;
		GLuint m_uProgID;

		unsigned m_uVertexShaderVersion;
		unsigned m_uFragmentShaderVersion;
		unsigned m_uGeometryShaderVersion;

		vector<string> m_vecVSCode;
		vector<string> m_vecFSCode;
		vector<string> m_vecGSCode;
		map<string, CUniVarBase*> m_mapUniformVar;

		string m_sVSCompileLog;
		string m_sFSCompileLog;
		string m_sGSCompileLog;
		string m_sProgLinkLog;

		bool m_bVSOK;
		bool m_bFSOK;
		bool m_bGSOK;
		bool m_bProgOK;

		bool m_bEnabled;
		bool m_bEnabledForPicking;
	};

#endif
