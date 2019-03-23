////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLShader.cpp
//
// summary:   Implements the ogl shader class
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
#include "OGLShader.h"
#include "OGLBaseElementList.h"

#include "CluTec.Viz.OpenGL.Extensions\Extensions.h"
#include "CluTec.Viz.OpenGL/Api.h"

COGLShader::COGLShader(void)
{
	m_sTypeName = "Shader";

	ResetVars();
}

COGLShader::COGLShader(const COGLShader& oglShader)
{
	m_sTypeName = "Shader";

	*this = oglShader;
}

COGLShader::~COGLShader(void)
{
	ResetShader();
}

void COGLShader::ResetUniformVarMap()
{
	map<string, CUniVarBase*>::iterator itEl, itEnd;

	itEnd = m_mapUniformVar.end();
	for (itEl = m_mapUniformVar.begin();
	     itEl != itEnd;
	     ++itEl)
	{
		if (itEl->second)
		{
			delete itEl->second;
		}
	}

	m_mapUniformVar.clear();
}

void COGLShader::ResetVars()
{
	m_uVSID   = 0;
	m_uFSID   = 0;
	m_uGSID   = 0;
	m_uProgID = 0;

	m_uVertexShaderVersion   = 0;
	m_uFragmentShaderVersion = 0;
	m_uGeometryShaderVersion = 0;

	ResetUniformVarMap();
	m_vecVSCode.resize(0);
	m_vecFSCode.resize(0);
	m_vecGSCode.resize(0);

	m_sVSCompileLog = "";
	m_sFSCompileLog = "";
	m_sGSCompileLog = "";
	m_sProgLinkLog  = "";

	m_bVSOK   = false;
	m_bFSOK   = false;
	m_bGSOK   = false;
	m_bProgOK = false;

	m_bEnabled           = true;
	m_bEnabledForPicking = false;
}

COGLShader& COGLShader::operator=(const COGLShader& rShader)
{
	m_uVSID   = 0;
	m_uFSID   = 0;
	m_uGSID   = 0;
	m_uProgID = 0;

	m_vecVSCode = rShader.m_vecVSCode;
	m_vecFSCode = rShader.m_vecFSCode;
	m_vecGSCode = rShader.m_vecGSCode;
	//m_mapUniformVar = m_mapUniformVar;

	m_sVSCompileLog = "";
	m_sFSCompileLog = "";
	m_sGSCompileLog = "";
	m_sProgLinkLog  = "";

	m_bVSOK   = false;
	m_bFSOK   = false;
	m_bGSOK   = false;
	m_bProgOK = false;

	m_bEnabled           = rShader.m_bEnabled;
	m_bEnabledForPicking = rShader.m_bEnabledForPicking;

	return *this;
}

/////////////////////////////////////////////////////////////////////
// Reset Shader

void COGLShader::ResetShader()
{
	//GLboolean bIsProg = 0, bIsVS = 0, bIsFS = 0;

	if (m_uProgID)
	{
		GLint iProg;
		glGetIntegerv(GL_CURRENT_PROGRAM, &iProg);

		if (unsigned(iProg) == m_uProgID)
		{
			glUseProgram(0);
		}

		glDeleteProgram(m_uProgID);
		//bIsProg = glIsProgram( m_uProgID );
	}

	if (m_uVSID)
	{
		glDeleteShader(m_uVSID);
		//bIsVS = glIsShader( m_uVSID );
	}

	if (m_uFSID)
	{
		glDeleteShader(m_uFSID);
		//bIsFS = glIsShader( m_uFSID );
	}

	if (m_uGSID)
	{
		glDeleteShader(m_uGSID);
	}

	//bIsVS = glIsShader( m_uVSID );
	//bIsFS = glIsShader( m_uFSID );

	m_uProgID = 0;
	m_uVSID   = 0;
	m_uFSID   = 0;
	m_uGSID   = 0;

	m_bProgOK = false;
	m_bVSOK   = false;
	m_bFSOK   = false;
	m_bGSOK   = false;

	m_vecVSCode.resize(0);
	m_vecFSCode.resize(0);
	m_vecGSCode.resize(0);
	ResetUniformVarMap();

	m_sVSCompileLog = "";
	m_sFSCompileLog = "";
	m_sGSCompileLog = "";
	m_sProgLinkLog  = "";
}

/////////////////////////////////////////////////////////////////////
/// Set Shader Source Code

bool COGLShader::SetShaderSource(GLuint uID, const vector<string>& vecCode, unsigned& uMaxVersion)
{
	int iIdx, iCnt = int(vecCode.size());

	const GLchar** pcCodeList = new const GLchar*[iCnt];
	GLint* piStrLen           = new GLint[iCnt];
	unsigned uVersion         = 0;
	uMaxVersion = 0;

	if ((pcCodeList == 0) || (piStrLen == 0))
	{
		return false;
	}

	for (iIdx = 0; iIdx < iCnt; ++iIdx)
	{
		pcCodeList[iIdx] = (const GLchar*) vecCode[iIdx].c_str();
		piStrLen[iIdx]   = GLint(vecCode[iIdx].length());

		std::string::size_type nVerPos = vecCode[iIdx].find("#version");
		if (nVerPos != std::string::npos)
		{
			std::string::size_type nStartPos = vecCode[iIdx].find_first_of("0123456789", nVerPos + 8);
			if (nStartPos != std::string::npos)
			{
				std::string::size_type nEndPos = vecCode[iIdx].find_first_not_of("0123456789", nStartPos);
				if (nEndPos  != std::string::npos)
				{
					std::string sVersion = vecCode[iIdx].substr(nStartPos, nEndPos - nStartPos + 1);
					if (sscanf_s(sVersion.c_str(), "%d", &uVersion) == 1)
					{
						if (uVersion > uMaxVersion)
						{
							uMaxVersion = uVersion;
						}
					}
				}
			}
		}
	}

	glShaderSource(uID, iCnt, pcCodeList, piStrLen);

	delete[] pcCodeList;
	delete[] piStrLen;

	return true;
}

/////////////////////////////////////////////////////////////////////
/// Compile Shader

bool COGLShader::CompileShader(GLuint& uID, GLuint uType, const vector<string>& vecCode, string& sInfoLog, unsigned& uMaxVersion)
{
	GLint iCompileStatus;
	sInfoLog = "";

	if (!vecCode.size())
	{
		sInfoLog = "No code given.";
		return false;
	}

	if ((uID = glCreateShader(uType)) == 0)
	{
		sInfoLog = "Error creating shader.";
		return false;
	}

	if (!SetShaderSource(uID, vecCode, uMaxVersion))
	{
		sInfoLog = "Error setting shader source.";
		return false;
	}

	glCompileShader(uID);
	glGetShaderiv(uID, GL_COMPILE_STATUS, &iCompileStatus);

	int iMaxLen;
	GLchar* pcInfoLog;

	glGetShaderiv(uID, GL_INFO_LOG_LENGTH, &iMaxLen);
	pcInfoLog = new GLchar[iMaxLen + 1];

	glGetShaderInfoLog(uID, iMaxLen, &iMaxLen, pcInfoLog);
	sInfoLog = pcInfoLog;

	delete[] pcInfoLog;

	if (!iCompileStatus)
	{
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////
/// Compile & Link Vertex and Fragment Shader

bool COGLShader::Build(const vector<string>& vecVSCode, const vector<string>& vecFSCode)
{
	vector<string> vecGeoCode;
	return Build(vecVSCode, vecFSCode, vecGeoCode);
}

bool COGLShader::Build(const vector<string>& vecVSCode, const vector<string>& vecFSCode, const vector<string>& vecGSCode)
{
	if (!Clu::OpenGL::Extensions::IsShaderAvailable())
	{
		return false;
	}

	// Reset old shaders and program if present
	ResetShader();

	bool bUseVS = false, bUseFS = false, bUseGeo = false;

	if (vecVSCode.size())
	{
		bUseVS = true;

		if (!CompileShader(m_uVSID, GL_VERTEX_SHADER, vecVSCode, m_sVSCompileLog, m_uVertexShaderVersion))
		{
			return false;
		}

		m_bVSOK     = true;
		m_vecVSCode = vecVSCode;
	}

	if (vecFSCode.size())
	{
		bUseFS = true;

		if (!CompileShader(m_uFSID, GL_FRAGMENT_SHADER, vecFSCode, m_sFSCompileLog, m_uFragmentShaderVersion))
		{
			return false;
		}

		m_bFSOK     = true;
		m_vecFSCode = vecFSCode;
	}

	if (vecGSCode.size())
	{
		bUseGeo = true;

		if (!CompileShader(m_uGSID, GL_GEOMETRY_SHADER, vecGSCode, m_sGSCompileLog, m_uGeometryShaderVersion))
		{
			return false;
		}

		m_bGSOK     = true;
		m_vecGSCode = vecFSCode;
	}

	int iLinkStatus;

	m_uProgID = glCreateProgram();

	if (bUseVS)
	{
		glAttachShader(m_uProgID, m_uVSID);
	}

	if (bUseFS)
	{
		glAttachShader(m_uProgID, m_uFSID);
	}

	if (bUseGeo)
	{
		glAttachShader(m_uProgID, m_uGSID);
	}

	// //////////////////////////////////////////////////////////////////////
	// Attach the vertex attribute IDs to default input names
	glBindAttribLocation(m_uProgID, CLUGL_VAA_VERTEX, "clu_in_vVertex");
	glBindAttribLocation(m_uProgID, CLUGL_VAA_COLOR, "clu_in_vColor");
	glBindAttribLocation(m_uProgID, CLUGL_VAA_NORMAL, "clu_in_vNormal");
	glBindAttribLocation(m_uProgID, CLUGL_VAA_TEX0, "clu_in_vTex0");
	glBindAttribLocation(m_uProgID, CLUGL_VAA_TEX1, "clu_in_vTex1");
	glBindAttribLocation(m_uProgID, CLUGL_VAA_TEX2, "clu_in_vTex2");
	glBindAttribLocation(m_uProgID, CLUGL_VAA_TEX3, "clu_in_vTex3");
	glBindAttribLocation(m_uProgID, CLUGL_VAA_TEX4, "clu_in_vTex4");
	glBindAttribLocation(m_uProgID, CLUGL_VAA_TEX5, "clu_in_vTex5");
	glBindAttribLocation(m_uProgID, CLUGL_VAA_TEX6, "clu_in_vTex6");
	glBindAttribLocation(m_uProgID, CLUGL_VAA_TEX7, "clu_in_vTex7");
	glBindAttribLocation(m_uProgID, CLUGL_VAA_PARTID, "clu_in_iPartId");
	// //////////////////////////////////////////////////////////////////////

	glLinkProgram(m_uProgID);
	glGetProgramiv(m_uProgID, GL_LINK_STATUS, &iLinkStatus);

	int iMaxLen;
	GLchar* pcInfoLog;

	glGetProgramiv(m_uProgID, GL_INFO_LOG_LENGTH, &iMaxLen);
	pcInfoLog = new GLchar[iMaxLen + 1];

	glGetProgramInfoLog(m_uProgID, iMaxLen, &iMaxLen, pcInfoLog);
	m_sProgLinkLog = pcInfoLog;

	delete[] pcInfoLog;

	if (!iLinkStatus)
	{
		return false;
	}

	m_bProgOK = true;

	CreateUniformVarMap();

	return true;
}

/////////////////////////////////////////////////////////////////////
/// Create Uniform Var Map

bool COGLShader::CreateUniformVarMap()
{
	//return true;

	if (!m_bProgOK)
	{
		return false;
	}

	ResetUniformVarMap();

	unsigned int uUniVar, uUniVarCnt;
	unsigned int uMaxVarNameLength;
	int iArrayCnt;
	GLenum eVarType;
	string sVarName;

	glGetProgramiv(m_uProgID, GL_ACTIVE_UNIFORMS, (GLint*) &uUniVarCnt);
	glGetProgramiv(m_uProgID, GL_ACTIVE_UNIFORM_MAX_LENGTH, (GLint*) &uMaxVarNameLength);

	char* pcName = new char[uMaxVarNameLength + 1];
	if (!pcName)
	{
		return false;
	}

	for (uUniVar = 0; uUniVar < uUniVarCnt; ++uUniVar)
	{
		glGetActiveUniform(m_uProgID, uUniVar, uMaxVarNameLength + 1,
				0, &iArrayCnt, &eVarType, pcName);

		sVarName = pcName;

		if (sVarName[sVarName.length() - 1] == ']')
		{
			map<string, CUniVarBase*>::iterator itEl;
			string sIdx;
			uint uIdx;
			int iEnd = int(sVarName.find_last_of("["));
			sIdx     = sVarName.substr(iEnd + 1, sVarName.length() - iEnd - 2);
			uIdx     = atoi(sIdx.c_str());
			sVarName = sVarName.substr(0, iEnd);

			// Check whether array of numbers has already been registered
			if ((itEl = m_mapUniformVar.find(sVarName)) != m_mapUniformVar.end())
			{
				// Check whether current index is larger than initially set array size
				if (uIdx >= itEl->second->Count())
				{
					itEl->second->SetElCount(uIdx + 1);
				}

				continue;
			}
		}

		switch (eVarType)
		{
		case GL_FLOAT:
			m_mapUniformVar[sVarName] = new CUniVar<float, 1, GL_FLOAT>(iArrayCnt);
			break;

		case GL_FLOAT_VEC2:
			m_mapUniformVar[sVarName] = new CUniVar<float, 2, GL_FLOAT_VEC2>(iArrayCnt);
			break;

		case GL_FLOAT_VEC3:
			m_mapUniformVar[sVarName] = new CUniVar<float, 3, GL_FLOAT_VEC3>(iArrayCnt);
			break;

		case GL_FLOAT_VEC4:
			m_mapUniformVar[sVarName] = new CUniVar<float, 4, GL_FLOAT_VEC4>(iArrayCnt);
			break;

		case GL_INT:
			m_mapUniformVar[sVarName] = new CUniVar<int, 1, GL_INT>(iArrayCnt);
			break;

		case GL_INT_VEC2:
			m_mapUniformVar[sVarName] = new CUniVar<int, 2, GL_INT_VEC2>(iArrayCnt);
			break;

		case GL_INT_VEC3:
			m_mapUniformVar[sVarName] = new CUniVar<int, 3, GL_INT_VEC3>(iArrayCnt);
			break;

		case GL_INT_VEC4:
			m_mapUniformVar[sVarName] = new CUniVar<int, 4, GL_INT_VEC4>(iArrayCnt);
			break;

		case GL_BOOL:
			m_mapUniformVar[sVarName] = new CUniVar<uint, 1, GL_BOOL>(iArrayCnt);
			break;

		case GL_BOOL_VEC2:
			m_mapUniformVar[sVarName] = new CUniVar<uint, 2, GL_BOOL_VEC2>(iArrayCnt);
			break;

		case GL_BOOL_VEC3:
			m_mapUniformVar[sVarName] = new CUniVar<uint, 3, GL_BOOL_VEC3>(iArrayCnt);
			break;

		case GL_BOOL_VEC4:
			m_mapUniformVar[sVarName] = new CUniVar<uint, 4, GL_BOOL_VEC4>(iArrayCnt);
			break;

		case GL_FLOAT_MAT2:
			m_mapUniformVar[sVarName] = new CUniVar<float, 4, GL_FLOAT_MAT2>(iArrayCnt);
			break;

		case GL_FLOAT_MAT3:
			m_mapUniformVar[sVarName] = new CUniVar<float, 9, GL_FLOAT_MAT3>(iArrayCnt);
			break;

		case GL_FLOAT_MAT4:
			m_mapUniformVar[sVarName] = new CUniVar<float, 16, GL_FLOAT_MAT4>(iArrayCnt);
			break;

		case GL_SAMPLER_1D:
			m_mapUniformVar[sVarName] = new CUniVar<uint, 1, GL_SAMPLER_1D>(iArrayCnt);
			break;

		case GL_SAMPLER_2D:
			m_mapUniformVar[sVarName] = new CUniVar<uint, 1, GL_SAMPLER_2D>(iArrayCnt);
			break;

		case GL_UNSIGNED_INT_SAMPLER_2D:
			m_mapUniformVar[sVarName] = new CUniVar<uint, 1, GL_UNSIGNED_INT_SAMPLER_2D>(iArrayCnt);
			break;

		case GL_SAMPLER_3D:
			m_mapUniformVar[sVarName] = new CUniVar<uint, 1, GL_SAMPLER_3D>(iArrayCnt);
			break;

		case GL_SAMPLER_CUBE:
			m_mapUniformVar[sVarName] = new CUniVar<uint, 1, GL_SAMPLER_CUBE>(iArrayCnt);
			break;

		case GL_SAMPLER_1D_SHADOW:
			m_mapUniformVar[sVarName] = new CUniVar<uint, 1, GL_SAMPLER_1D_SHADOW>(iArrayCnt);
			break;

		case GL_SAMPLER_2D_SHADOW:
			m_mapUniformVar[sVarName] = new CUniVar<uint, 1, GL_SAMPLER_2D_SHADOW>(iArrayCnt);
			break;
		}
	}

	delete[] pcName;

	return true;
}

/////////////////////////////////////////////////////////////////////
/// Get Location of Variable

int COGLShader::CUniVarBase::GetVarLocation(GLuint uProgID, const string& sVarName)
{
	//int iLoc;
	//if ( (iLoc = glGetUniformLocation( uProgID, sVarName.c_str() )) == -1 )
	//{
	//	string sArName = sVarName;
	//	sArName += "[0]";

	//	iLoc = glGetUniformLocation( uProgID, sArName.c_str() );
	//}

	//return iLoc;
	return glGetUniformLocation(uProgID, sVarName.c_str());
}

/////////////////////////////////////////////////////////////////////
// Apply Uniform Variable

bool COGLShader::CUniVar<float, 1, GL_FLOAT>::Apply(GLuint uProgID, const string& sVarName)
{
	int iLoc;
	if ((iLoc = GetVarLocation(uProgID, sVarName)) == -1)
	{
		return false;
	}

	glUniform1fv(iLoc, GLsizei(Count()), (const GLfloat*) &m_vecVal[0]);

	return true;
}

bool COGLShader::CUniVar<float, 2, GL_FLOAT_VEC2>::Apply(GLuint uProgID, const string& sVarName)
{
	int iLoc;
	if ((iLoc = GetVarLocation(uProgID, sVarName)) == -1)
	{
		return false;
	}

	glUniform2fv(iLoc, GLsizei(Count()), (const GLfloat*) &m_vecVal[0]);

	return true;
}

bool COGLShader::CUniVar<float, 3, GL_FLOAT_VEC3>::Apply(GLuint uProgID, const string& sVarName)
{
	int iLoc;
	if ((iLoc = GetVarLocation(uProgID, sVarName)) == -1)
	{
		return false;
	}

	glUniform3fv(iLoc, GLsizei(Count()), (const GLfloat*) &m_vecVal[0]);

	return true;
}

bool COGLShader::CUniVar<float, 4, GL_FLOAT_VEC4>::Apply(GLuint uProgID, const string& sVarName)
{
	int iLoc;
	if ((iLoc = GetVarLocation(uProgID, sVarName)) == -1)
	{
		return false;
	}

	glUniform4fv(iLoc, GLsizei(Count()), (const GLfloat*) &m_vecVal[0]);

	return true;
}

bool COGLShader::CUniVar<int, 1, GL_INT>::Apply(GLuint uProgID, const string& sVarName)
{
	int iLoc;
	if ((iLoc = GetVarLocation(uProgID, sVarName)) == -1)
	{
		return false;
	}

	glUniform1iv(iLoc, GLsizei(Count()), (const GLint*) &m_vecVal[0]);

	return true;
}

bool COGLShader::CUniVar<int, 2, GL_INT_VEC2>::Apply(GLuint uProgID, const string& sVarName)
{
	int iLoc;
	if ((iLoc = GetVarLocation(uProgID, sVarName)) == -1)
	{
		return false;
	}

	glUniform2iv(iLoc, GLsizei(Count()), (const GLint*) &m_vecVal[0]);

	return true;
}

bool COGLShader::CUniVar<int, 3, GL_INT_VEC3>::Apply(GLuint uProgID, const string& sVarName)
{
	int iLoc;
	if ((iLoc = GetVarLocation(uProgID, sVarName)) == -1)
	{
		return false;
	}

	glUniform3iv(iLoc, GLsizei(Count()), (const GLint*) &m_vecVal[0]);

	return true;
}

bool COGLShader::CUniVar<int, 4, GL_INT_VEC4>::Apply(GLuint uProgID, const string& sVarName)
{
	int iLoc;
	if ((iLoc = GetVarLocation(uProgID, sVarName)) == -1)
	{
		return false;
	}

	glUniform4iv(iLoc, GLsizei(Count()), (const GLint*) &m_vecVal[0]);

	return true;
}

bool COGLShader::CUniVar<uint, 1, GL_BOOL>::Apply(GLuint uProgID, const string& sVarName)
{
	int iLoc;
	if ((iLoc = GetVarLocation(uProgID, sVarName)) == -1)
	{
		return false;
	}

	glUniform1iv(iLoc, GLsizei(Count()), (const GLint*) &m_vecVal[0]);

	return true;
}

bool COGLShader::CUniVar<uint, 2, GL_BOOL_VEC2>::Apply(GLuint uProgID, const string& sVarName)
{
	int iLoc;
	if ((iLoc = GetVarLocation(uProgID, sVarName)) == -1)
	{
		return false;
	}

	glUniform2iv(iLoc, GLsizei(Count()), (const GLint*) &m_vecVal[0]);

	return true;
}

bool COGLShader::CUniVar<uint, 3, GL_BOOL_VEC3>::Apply(GLuint uProgID, const string& sVarName)
{
	int iLoc;
	if ((iLoc = GetVarLocation(uProgID, sVarName)) == -1)
	{
		return false;
	}

	glUniform3iv(iLoc, GLsizei(Count()), (const GLint*) &m_vecVal[0]);

	return true;
}

bool COGLShader::CUniVar<uint, 4, GL_BOOL_VEC4>::Apply(GLuint uProgID, const string& sVarName)
{
	int iLoc;
	if ((iLoc = GetVarLocation(uProgID, sVarName)) == -1)
	{
		return false;
	}

	glUniform4iv(iLoc, GLsizei(Count()), (const GLint*) &m_vecVal[0]);

	return true;
}

bool COGLShader::CUniVar<float, 4, GL_FLOAT_MAT2>::Apply(GLuint uProgID, const string& sVarName)
{
	int iLoc;
	if ((iLoc = GetVarLocation(uProgID, sVarName)) == -1)
	{
		return false;
	}

	glUniformMatrix2fv(iLoc, GLsizei(Count()), GL_TRUE, (const GLfloat*) &m_vecVal[0]);

	return true;
}

bool COGLShader::CUniVar<float, 9, GL_FLOAT_MAT3>::Apply(GLuint uProgID, const string& sVarName)
{
	int iLoc;
	if ((iLoc = GetVarLocation(uProgID, sVarName)) == -1)
	{
		return false;
	}

	glUniformMatrix3fv(iLoc, GLsizei(Count()), GL_TRUE, (const GLfloat*) &m_vecVal[0]);

	return true;
}

bool COGLShader::CUniVar<float, 16, GL_FLOAT_MAT4>::Apply(GLuint uProgID, const string& sVarName)
{
	int iLoc;
	if ((iLoc = GetVarLocation(uProgID, sVarName)) == -1)
	{
		return false;
	}

	glUniformMatrix4fv(iLoc, GLsizei(Count()), GL_TRUE, (const GLfloat*) &m_vecVal[0]);

	return true;
}

bool COGLShader::CUniVar<uint, 1, GL_SAMPLER_1D>::Apply(GLuint uProgID, const string& sVarName)
{
	int iLoc;
	if ((iLoc = GetVarLocation(uProgID, sVarName)) == -1)
	{
		return false;
	}

	glUniform1iv(iLoc, GLsizei(Count()), (const GLint*) &m_vecVal[0]);

	return true;
}

bool COGLShader::CUniVar<uint, 1, GL_SAMPLER_2D>::Apply(GLuint uProgID, const string& sVarName)
{
	int iLoc;
	if ((iLoc = GetVarLocation(uProgID, sVarName)) == -1)
	{
		return false;
	}

	glUniform1iv(iLoc, GLsizei(Count()), (const GLint*) &m_vecVal[0]);

	return true;
}

bool COGLShader::CUniVar<uint, 1, GL_UNSIGNED_INT_SAMPLER_2D>::Apply(GLuint uProgID, const string& sVarName)
{
	int iLoc;
	if ((iLoc = GetVarLocation(uProgID, sVarName)) == -1)
	{
		return false;
	}

	glUniform1iv(iLoc, GLsizei(Count()), (const GLint*) &m_vecVal[0]);

	return true;
}

bool COGLShader::CUniVar<uint, 1, GL_SAMPLER_3D>::Apply(GLuint uProgID, const string& sVarName)
{
	int iLoc;
	if ((iLoc = GetVarLocation(uProgID, sVarName)) == -1)
	{
		return false;
	}

	glUniform1iv(iLoc, GLsizei(Count()), (const GLint*) &m_vecVal[0]);

	return true;
}

bool COGLShader::CUniVar<uint, 1, GL_SAMPLER_CUBE>::Apply(GLuint uProgID, const string& sVarName)
{
	int iLoc;
	if ((iLoc = GetVarLocation(uProgID, sVarName)) == -1)
	{
		return false;
	}

	glUniform1iv(iLoc, GLsizei(Count()), (const GLint*) &m_vecVal[0]);

	return true;
}

bool COGLShader::CUniVar<uint, 1, GL_SAMPLER_1D_SHADOW>::Apply(GLuint uProgID, const string& sVarName)
{
	int iLoc;
	if ((iLoc = GetVarLocation(uProgID, sVarName)) == -1)
	{
		return false;
	}

	glUniform1iv(iLoc, GLsizei(Count()), (const GLint*) &m_vecVal[0]);

	return true;
}

bool COGLShader::CUniVar<uint, 1, GL_SAMPLER_2D_SHADOW>::Apply(GLuint uProgID, const string& sVarName)
{
	int iLoc;
	if ((iLoc = GetVarLocation(uProgID, sVarName)) == -1)
	{
		return false;
	}

	glUniform1iv(iLoc, GLsizei(Count()), (const GLint*) &m_vecVal[0]);

	return true;
}

/////////////////////////////////////////////////////////////////////
// Set Uniform Variable

template<>
bool COGLShader::SetUniformVar<float, 1>(const string& sVarName, float* pVal, unsigned int uIdx, unsigned int uElCount)
{
	map<string, CUniVarBase*>::iterator itEl;

	if ((itEl = m_mapUniformVar.find(sVarName)) == m_mapUniformVar.end())
	{
		return false;
	}

	CUniVarBase* pVarBase = m_mapUniformVar[sVarName];
	if (pVarBase->Type() != GL_FLOAT)
	{
		return false;
	}

	CUniVar<float, 1, GL_FLOAT>* pVar = dynamic_cast<CUniVar<float, 1, GL_FLOAT>* >(pVarBase);
	if (uIdx >= pVar->Count())
	{
		return false;
	}

	if (!pVar->Set(uIdx, pVal, uElCount))
	{
		return false;
	}

	TellParentContentChanged();

	return true;
}

template<>
bool COGLShader::SetUniformVar<float, 2>(const string& sVarName, float* pVal, unsigned int uIdx, unsigned int uElCount)
{
	map<string, CUniVarBase*>::iterator itEl;

	if ((itEl = m_mapUniformVar.find(sVarName)) == m_mapUniformVar.end())
	{
		return false;
	}

	CUniVarBase* pVarBase = m_mapUniformVar[sVarName];
	if (pVarBase->Type() != GL_FLOAT_VEC2)
	{
		return false;
	}

	CUniVar<float, 2, GL_FLOAT_VEC2>* pVar = dynamic_cast<CUniVar<float, 2, GL_FLOAT_VEC2>* >(pVarBase);
	if (uIdx >= pVar->Count())
	{
		return false;
	}

	if (!pVar->Set(uIdx, pVal, uElCount))
	{
		return false;
	}

	TellParentContentChanged();

	return true;
}

template<>
bool COGLShader::SetUniformVar<float, 3>(const string& sVarName, float* pVal, unsigned int uIdx, unsigned int uElCount)
{
	map<string, CUniVarBase*>::iterator itEl;

	if ((itEl = m_mapUniformVar.find(sVarName)) == m_mapUniformVar.end())
	{
		return false;
	}

	CUniVarBase* pVarBase = m_mapUniformVar[sVarName];
	if (pVarBase->Type() != GL_FLOAT_VEC3)
	{
		return false;
	}

	CUniVar<float, 3, GL_FLOAT_VEC3>* pVar = dynamic_cast<CUniVar<float, 3, GL_FLOAT_VEC3>* >(pVarBase);
	if (uIdx >= pVar->Count())
	{
		return false;
	}

	if (!pVar->Set(uIdx, pVal, uElCount))
	{
		return false;
	}

	TellParentContentChanged();

	return true;
}

template<>
bool COGLShader::SetUniformVar<float, 4>(const string& sVarName, float* pVal, unsigned int uIdx, unsigned int uElCount)
{
	map<string, CUniVarBase*>::iterator itEl;

	if ((itEl = m_mapUniformVar.find(sVarName)) == m_mapUniformVar.end())
	{
		return false;
	}

	CUniVarBase* pVarBase = m_mapUniformVar[sVarName];
	if (pVarBase->Type() != GL_FLOAT_VEC4)
	{
		return false;
	}

	CUniVar<float, 4, GL_FLOAT_VEC4>* pVar = dynamic_cast<CUniVar<float, 4, GL_FLOAT_VEC4>* >(pVarBase);
	if (uIdx >= pVar->Count())
	{
		return false;
	}

	if (!pVar->Set(uIdx, pVal, uElCount))
	{
		return false;
	}

	TellParentContentChanged();

	return true;
}

template<>
bool COGLShader::SetUniformVar<int, 1>(const string& sVarName, int* pVal, unsigned int uIdx, unsigned int uElCount)
{
	map<string, CUniVarBase*>::iterator itEl;

	if ((itEl = m_mapUniformVar.find(sVarName)) == m_mapUniformVar.end())
	{
		return false;
	}

	CUniVarBase* pVarBase = m_mapUniformVar[sVarName];
	if (pVarBase->Type() != GL_INT)
	{
		return false;
	}

	CUniVar<int, 1, GL_INT>* pVar = dynamic_cast<CUniVar<int, 1, GL_INT>* >(pVarBase);
	if (uIdx >= pVar->Count())
	{
		return false;
	}

	if (!pVar->Set(uIdx, pVal, uElCount))
	{
		return false;
	}

	TellParentContentChanged();

	return true;
}

template<>
bool COGLShader::SetUniformVar<int, 2>(const string& sVarName, int* pVal, unsigned int uIdx, unsigned int uElCount)
{
	map<string, CUniVarBase*>::iterator itEl;

	if ((itEl = m_mapUniformVar.find(sVarName)) == m_mapUniformVar.end())
	{
		return false;
	}

	CUniVarBase* pVarBase = m_mapUniformVar[sVarName];
	if (pVarBase->Type() != GL_INT_VEC2)
	{
		return false;
	}

	CUniVar<int, 2, GL_INT_VEC2>* pVar = dynamic_cast<CUniVar<int, 2, GL_INT_VEC2>* >(pVarBase);
	if (uIdx >= pVar->Count())
	{
		return false;
	}

	if (!pVar->Set(uIdx, pVal, uElCount))
	{
		return false;
	}

	TellParentContentChanged();

	return true;
}

template<>
bool COGLShader::SetUniformVar<int, 3>(const string& sVarName, int* pVal, unsigned int uIdx, unsigned int uElCount)
{
	map<string, CUniVarBase*>::iterator itEl;

	if ((itEl = m_mapUniformVar.find(sVarName)) == m_mapUniformVar.end())
	{
		return false;
	}

	CUniVarBase* pVarBase = m_mapUniformVar[sVarName];
	if (pVarBase->Type() != GL_INT_VEC3)
	{
		return false;
	}

	CUniVar<int, 3, GL_INT_VEC3>* pVar = dynamic_cast<CUniVar<int, 3, GL_INT_VEC3>* >(pVarBase);
	if (uIdx >= pVar->Count())
	{
		return false;
	}

	if (!pVar->Set(uIdx, pVal, uElCount))
	{
		return false;
	}

	TellParentContentChanged();

	return true;
}

template<>
bool COGLShader::SetUniformVar<int, 4>(const string& sVarName, int* pVal, unsigned int uIdx, unsigned int uElCount)
{
	map<string, CUniVarBase*>::iterator itEl;

	if ((itEl = m_mapUniformVar.find(sVarName)) == m_mapUniformVar.end())
	{
		return false;
	}

	CUniVarBase* pVarBase = m_mapUniformVar[sVarName];
	if (pVarBase->Type() != GL_INT_VEC4)
	{
		return false;
	}

	CUniVar<int, 4, GL_INT_VEC4>* pVar = dynamic_cast<CUniVar<int, 4, GL_INT_VEC4>* >(pVarBase);
	if (uIdx >= pVar->Count())
	{
		return false;
	}

	if (!pVar->Set(uIdx, pVal, uElCount))
	{
		return false;
	}

	TellParentContentChanged();

	return true;
}

template<>
bool COGLShader::SetUniformVar<uint, 1>(const string& sVarName, uint* pVal, unsigned int uIdx, unsigned int uElCount)
{
	map<string, CUniVarBase*>::iterator itEl;

	if ((itEl = m_mapUniformVar.find(sVarName)) == m_mapUniformVar.end())
	{
		return false;
	}

	CUniVarBase* pVarBase = m_mapUniformVar[sVarName];
	if (pVarBase->Type() != GL_BOOL)
	{
		return false;
	}

	CUniVar<uint, 1, GL_BOOL>* pVar = dynamic_cast<CUniVar<uint, 1, GL_BOOL>* >(pVarBase);
	if (uIdx >= pVar->Count())
	{
		return false;
	}

	if (!pVar->Set(uIdx, pVal, uElCount))
	{
		return false;
	}

	TellParentContentChanged();

	return true;
}

template<>
bool COGLShader::SetUniformVar<uint, 2>(const string& sVarName, uint* pVal, unsigned int uIdx, unsigned int uElCount)
{
	map<string, CUniVarBase*>::iterator itEl;

	if ((itEl = m_mapUniformVar.find(sVarName)) == m_mapUniformVar.end())
	{
		return false;
	}

	CUniVarBase* pVarBase = m_mapUniformVar[sVarName];
	if (pVarBase->Type() != GL_BOOL_VEC2)
	{
		return false;
	}

	CUniVar<uint, 2, GL_BOOL_VEC2>* pVar = dynamic_cast<CUniVar<uint, 2, GL_BOOL_VEC2>* >(pVarBase);
	if (uIdx >= pVar->Count())
	{
		return false;
	}

	if (!pVar->Set(uIdx, pVal, uElCount))
	{
		return false;
	}

	TellParentContentChanged();

	return true;
}

template<>
bool COGLShader::SetUniformVar<uint, 3>(const string& sVarName, uint* pVal, unsigned int uIdx, unsigned int uElCount)
{
	map<string, CUniVarBase*>::iterator itEl;

	if ((itEl = m_mapUniformVar.find(sVarName)) == m_mapUniformVar.end())
	{
		return false;
	}

	CUniVarBase* pVarBase = m_mapUniformVar[sVarName];
	if (pVarBase->Type() != GL_BOOL_VEC3)
	{
		return false;
	}

	CUniVar<uint, 3, GL_BOOL_VEC3>* pVar = dynamic_cast<CUniVar<uint, 3, GL_BOOL_VEC3>* >(pVarBase);
	if (uIdx >= pVar->Count())
	{
		return false;
	}

	if (!pVar->Set(uIdx, pVal, uElCount))
	{
		return false;
	}

	TellParentContentChanged();

	return true;
}

template<>
bool COGLShader::SetUniformVar<uint, 4>(const string& sVarName, uint* pVal, unsigned int uIdx, unsigned int uElCount)
{
	map<string, CUniVarBase*>::iterator itEl;

	if ((itEl = m_mapUniformVar.find(sVarName)) == m_mapUniformVar.end())
	{
		return false;
	}

	CUniVarBase* pVarBase = m_mapUniformVar[sVarName];
	if (pVarBase->Type() != GL_BOOL_VEC4)
	{
		return false;
	}

	CUniVar<uint, 4, GL_BOOL_VEC4>* pVar = dynamic_cast<CUniVar<uint, 4, GL_BOOL_VEC4>* >(pVarBase);
	if (uIdx >= pVar->Count())
	{
		return false;
	}

	if (!pVar->Set(uIdx, pVal, uElCount))
	{
		return false;
	}

	TellParentContentChanged();

	return true;
}

template<>
bool COGLShader::SetUniformVarMatrix<2>(const string& sVarName, float* pVal, uint uIdx, uint uElCount)
{
	map<string, CUniVarBase*>::iterator itEl;

	if ((itEl = m_mapUniformVar.find(sVarName)) == m_mapUniformVar.end())
	{
		return false;
	}

	CUniVarBase* pVarBase = m_mapUniformVar[sVarName];
	if (pVarBase->Type() != GL_FLOAT_MAT2)
	{
		return false;
	}

	CUniVar<float, 4, GL_FLOAT_MAT2>* pVar = dynamic_cast<CUniVar<float, 4, GL_FLOAT_MAT2>* >(pVarBase);
	if (uIdx >= pVar->Count())
	{
		return false;
	}

	if (!pVar->Set(uIdx, pVal, uElCount))
	{
		return false;
	}

	TellParentContentChanged();

	return true;
}

template<>
bool COGLShader::SetUniformVarMatrix<3>(const string& sVarName, float* pVal, uint uIdx, uint uElCount)
{
	map<string, CUniVarBase*>::iterator itEl;

	if ((itEl = m_mapUniformVar.find(sVarName)) == m_mapUniformVar.end())
	{
		return false;
	}

	CUniVarBase* pVarBase = m_mapUniformVar[sVarName];
	if (pVarBase->Type() != GL_FLOAT_MAT3)
	{
		return false;
	}

	CUniVar<float, 9, GL_FLOAT_MAT3>* pVar = dynamic_cast<CUniVar<float, 9, GL_FLOAT_MAT3>* >(pVarBase);
	if (uIdx >= pVar->Count())
	{
		return false;
	}

	if (!pVar->Set(uIdx, pVal, uElCount))
	{
		return false;
	}

	TellParentContentChanged();

	return true;
}

template<>
bool COGLShader::SetUniformVarMatrix<4>(const string& sVarName, float* pVal, uint uIdx, uint uElCount)
{
	map<string, CUniVarBase*>::iterator itEl;

	if ((itEl = m_mapUniformVar.find(sVarName)) == m_mapUniformVar.end())
	{
		return false;
	}

	CUniVarBase* pVarBase = m_mapUniformVar[sVarName];
	if (pVarBase->Type() != GL_FLOAT_MAT4)
	{
		return false;
	}

	CUniVar<float, 16, GL_FLOAT_MAT4>* pVar = dynamic_cast<CUniVar<float, 16, GL_FLOAT_MAT4>* >(pVarBase);
	if (uIdx >= pVar->Count())
	{
		return false;
	}

	if (!pVar->Set(uIdx, pVal, uElCount))
	{
		return false;
	}

	TellParentContentChanged();

	return true;
}

bool COGLShader::SetUniformVarSampler(const string& sVarName, uint* puVal, uint uIdx, uint uElCount)
{
	map<string, CUniVarBase*>::iterator itEl;

	if ((itEl = m_mapUniformVar.find(sVarName)) == m_mapUniformVar.end())
	{
		return false;
	}

	CUniVarBase* pVarBase = m_mapUniformVar[sVarName];
	if (pVarBase->Type() == GL_SAMPLER_1D)
	{
		CUniVar<uint, 1, GL_SAMPLER_1D>* pVar = dynamic_cast<CUniVar<uint, 1, GL_SAMPLER_1D>* >(pVarBase);
		if (uIdx >= pVar->Count())
		{
			return false;
		}

		if (!pVar->Set(uIdx, puVal, uElCount))
		{
			return false;
		}
	}
	else if (pVarBase->Type() == GL_SAMPLER_2D)
	{
		CUniVar<uint, 1, GL_SAMPLER_2D>* pVar = dynamic_cast<CUniVar<uint, 1, GL_SAMPLER_2D>* >(pVarBase);
		if (uIdx >= pVar->Count())
		{
			return false;
		}

		if (!pVar->Set(uIdx, puVal, uElCount))
		{
			return false;
		}
	}
	else if (pVarBase->Type() == GL_UNSIGNED_INT_SAMPLER_2D)
	{
		CUniVar<uint, 1, GL_UNSIGNED_INT_SAMPLER_2D>* pVar = dynamic_cast<CUniVar<uint, 1, GL_UNSIGNED_INT_SAMPLER_2D>* >(pVarBase);
		if (uIdx >= pVar->Count())
		{
			return false;
		}

		if (!pVar->Set(uIdx, puVal, uElCount))
		{
			return false;
		}
	}
	else if (pVarBase->Type() == GL_SAMPLER_3D)
	{
		CUniVar<uint, 1, GL_SAMPLER_3D>* pVar = dynamic_cast<CUniVar<uint, 1, GL_SAMPLER_3D>* >(pVarBase);
		if (uIdx >= pVar->Count())
		{
			return false;
		}

		if (!pVar->Set(uIdx, puVal, uElCount))
		{
			return false;
		}
	}
	else if (pVarBase->Type() == GL_SAMPLER_CUBE)
	{
		CUniVar<uint, 1, GL_SAMPLER_CUBE>* pVar = dynamic_cast<CUniVar<uint, 1, GL_SAMPLER_CUBE>* >(pVarBase);
		if (uIdx >= pVar->Count())
		{
			return false;
		}

		if (!pVar->Set(uIdx, puVal, uElCount))
		{
			return false;
		}
	}
	else if (pVarBase->Type() == GL_SAMPLER_1D_SHADOW)
	{
		CUniVar<uint, 1, GL_SAMPLER_1D_SHADOW>* pVar = dynamic_cast<CUniVar<uint, 1, GL_SAMPLER_1D_SHADOW>* >(pVarBase);
		if (uIdx >= pVar->Count())
		{
			return false;
		}

		if (!pVar->Set(uIdx, puVal, uElCount))
		{
			return false;
		}
	}
	else if (pVarBase->Type() == GL_SAMPLER_2D_SHADOW)
	{
		CUniVar<uint, 1, GL_SAMPLER_2D_SHADOW>* pVar = dynamic_cast<CUniVar<uint, 1, GL_SAMPLER_2D_SHADOW>* >(pVarBase);
		if (uIdx >= pVar->Count())
		{
			return false;
		}

		if (!pVar->Set(uIdx, puVal, uElCount))
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	TellParentContentChanged();

	return true;
}

/////////////////////////////////////////////////////////////////////
// Get Type String of uniform variable

bool COGLShader::GetUniformVarType(string& sVarType, const string& sVarName)
{
	if (HasUniformVar(sVarName))
	{
		switch (m_mapUniformVar[sVarName]->Type())
		{
		case GL_FLOAT:
			sVarType = "float";
			break;

		case GL_FLOAT_VEC2:
			sVarType = "vec2";
			break;

		case GL_FLOAT_VEC3:
			sVarType = "vec3";
			break;

		case GL_FLOAT_VEC4:
			sVarType = "vec4";
			break;

		case GL_INT:
			sVarType = "int";
			break;

		case GL_INT_VEC2:
			sVarType = "ivec2";
			break;

		case GL_INT_VEC3:
			sVarType = "ivec3";
			break;

		case GL_INT_VEC4:
			sVarType = "ivec4";
			break;

		case GL_BOOL:
			sVarType = "bool";
			break;

		case GL_BOOL_VEC2:
			sVarType = "bvec2";
			break;

		case GL_BOOL_VEC3:
			sVarType = "bvec3";
			break;

		case GL_BOOL_VEC4:
			sVarType = "bvec4";
			break;

		case GL_FLOAT_MAT2:
			sVarType = "mat2";
			break;

		case GL_FLOAT_MAT3:
			sVarType = "mat3";
			break;

		case GL_FLOAT_MAT4:
			sVarType = "mat4";
			break;

		case GL_SAMPLER_1D:
			sVarType = "sampler_1";
			break;

		case GL_SAMPLER_2D:
			sVarType = "sampler_2";
			break;

		case GL_UNSIGNED_INT_SAMPLER_2D:
			sVarType = "usampler_2";
			break;

		case GL_SAMPLER_3D:
			sVarType = "sampler_3";
			break;

		case GL_SAMPLER_CUBE:
			sVarType = "sampler_cube";
			break;

		case GL_SAMPLER_1D_SHADOW:
			sVarType = "sampler_shadow_1";
			break;

		case GL_SAMPLER_2D_SHADOW:
			sVarType = "sampler_shadow_2";
			break;

		default:
			sVarType = "";
			return false;
		}

		return true;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////
// Get size of variable array

bool COGLShader::GetUniformVarArraySize(unsigned int& uCnt, const string& sVarName)
{
	uCnt = 0;
	if (!HasUniformVar(sVarName))
	{
		return false;
	}

	uCnt = uint(m_mapUniformVar[sVarName]->Count());
	return true;
}

/////////////////////////////////////////////////////////////////////
/// Apply Uniform Variables

void COGLShader::ApplyUniformVarList()
{
	//return;

	map<string, CUniVarBase*>::iterator itEl, itEnd;

	itEnd = m_mapUniformVar.end();
	for (itEl = m_mapUniformVar.begin();
	     itEl != itEnd;
	     ++itEl)
	{
		if (itEl->second)
		{
			itEl->second->Apply(m_uProgID, itEl->first);
		}
	}
}

//////////////////////////////////////////////////////////////////////
/// Tell parents that content has changed

void COGLShader::TellParentContentChanged()
{
	list<COGLBaseElement*>::iterator itEl;

	for (itEl = m_listParent.begin();
	     itEl != m_listParent.end();
	     ++itEl)
	{
		COGLBaseElementList* pList = dynamic_cast<COGLBaseElementList*>((COGLBaseElement*) (*itEl));
		if (pList)
		{
			pList->SetContentChanged(true, true, false);
		}
	}
}

/////////////////////////////////////////////////////////////////////
/// Apply
bool COGLShader::Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData)
{
	// Cannot use shader in picking mode, as we use the object color to identify picked objects.
	// If the shader is enabled in picking mode, then it should use the uniform variable
	// 'clu_iInPickDrawMode' to draw only the flat selection color.

	// If not enabled return directly without switching off shading!!!
	if (!m_bEnabled)
	{
		return true;
	}

	// If the shader is enabled and we are in picking mode, but the shader is not enabled for picking,
	// then switch of shading, since in drawing mode some shader would have been set.
	if ((eMode == COGLBaseElement::PICK) && !m_bEnabledForPicking)
	{
		glUseProgram(0);
		rData.pCurShader = nullptr;
		return true;
	}

	// If the shader is enabled but the program is not OK, then switch off shading.
	if (!m_bProgOK)
	{
		glUseProgram(0);
		rData.pCurShader = nullptr;
		return true;
	}

	// The shader is enabled and is valid.
	rData.pCurShader = this;

	glUseProgram(m_uProgID);

	if (m_bEnabledForPicking)
	{
		int iInPickState = int((eMode == COGLBaseElement::PICK) ? 1 : 0);
		SetUniformVar<int, 1>("clu_iInPickDrawMode", &iInPickState);
	}

	if (rData.bInPickState)
	{
		// Try to set uniform variable in shader that gives currently picked part ID
		SetUniformVar<int, 1>("clu_iPickedPartId", (int*) &rData.uCurPickPartID);
	}
	else
	{
		int iInvalidPartID = 0x000FFFFF;
		SetUniformVar<int, 1>("clu_iPickedPartId", &iInvalidPartID);
	}

	ApplyUniformVarList();

	return true;
}
