////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Net.View
// file:      ScriptVar.cpp
//
// summary:   Implements the script variable class
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

#include "stdafx.h"
#include "ScriptVar.h"
#include "CluScript.h"

#pragma managed (push, off)
#include "CluTec.Viz.View/Clu.Viz.View.h"
#pragma managed(pop)

#include "Defines.h"


namespace Clu
{
	namespace Viz
	{
		namespace Net
		{
			namespace View
			{

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				ScriptVar::ScriptVar(System::String^ sName, CluScript^ xScript)
				{
					m_pName = new Clu::CIString(Clu::Net::Converter::ToIString(sName));
					if (m_pName)
					{
						m_pcName = m_pName->ToCString();
						m_xScript = xScript;
					}

					SendMessageToScriptOnChange = false;
					ScriptMessageName = "";
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				ScriptVar::~ScriptVar()
				{
					if (m_pName != nullptr)
					{
						this->!ScriptVar();
					}
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				ScriptVar::!ScriptVar()
				{
					delete m_pName;
					m_pName = nullptr;
				}


				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				System::String^ ScriptVar::Name::get()
				{
					return Clu::Net::Converter::ToString(m_pcName);
				}


				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				void ScriptVar::_ValueChanged()
				{
					if (SendMessageToScriptOnChange)
					{
						Script->Execute(ScriptMessageName);
					}
				}

#pragma region Script Variable get/set
				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				int ScriptVar::GetInt()
				{
					try
					{
						int iVal;
						Clu::Viz::View::GetVarNumber(Script->Handle, m_pcName, iVal);
						return iVal;
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED(__FUNCTION__)
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				bool ScriptVar::GetBool()
				{
					try
					{
						int iVal;
						Clu::Viz::View::GetVarNumber(Script->Handle, m_pcName, iVal);

						return (iVal == 1);
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED(__FUNCTION__)
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				float ScriptVar::GetFloat()
				{
					try
					{
						double dVal;
						Clu::Viz::View::GetVarNumber(Script->Handle, m_pcName, dVal);
						return (float)dVal;
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED(__FUNCTION__)
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				double ScriptVar::GetDouble()
				{
					try
					{
						double dVal;
						Clu::Viz::View::GetVarNumber(Script->Handle, m_pcName, dVal);
						return dVal;
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED(__FUNCTION__)
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				System::Decimal ScriptVar::GetDecimal()
				{
					try
					{
						double dVal;
						Clu::Viz::View::GetVarNumber(Script->Handle, m_pcName, dVal);
						return System::Convert::ToDecimal(dVal);
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED(__FUNCTION__)
				}


				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				System::String^ ScriptVar::GetString()
				{
					try
					{
						return GetString(4096);
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED(__FUNCTION__)
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				System::String^ ScriptVar::GetString(int iMaxLen)
				{
					try
					{
						std::vector<char> vecValue(iMaxLen);
						Clu::Viz::View::GetVarString(Script->Handle, m_pcName, vecValue.data(), iMaxLen);
						return Clu::Net::Converter::ToString(vecValue.data());
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED(__FUNCTION__)
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				System::Drawing::Color ScriptVar::GetColor()
				{
					try
					{
						double pdColor[4];
						Clu::Viz::View::GetVarColor(Script->Handle, m_pcName, pdColor);

						double dVal;
						unsigned char cVal, cRed, cGreen, cBlue, cAlpha;

						dVal = floor(255.0 * pdColor[0]);
						if (dVal < 0.0)
						{
							cVal = 0;
						}
						else if (dVal > 255.0)
						{
							cVal = 255;
						}
						else
						{
							cVal = (unsigned char)dVal;
						}

						cRed = cVal;

						dVal = floor(255.0 * pdColor[1]);
						if (dVal < 0.0)
						{
							cVal = 0;
						}
						else if (dVal > 255.0)
						{
							cVal = 255;
						}
						else
						{
							cVal = (unsigned char)dVal;
						}

						cGreen = cVal;

						dVal = floor(255.0 * pdColor[2]);
						if (dVal < 0.0)
						{
							cVal = 0;
						}
						else if (dVal > 255.0)
						{
							cVal = 255;
						}
						else
						{
							cVal = (unsigned char)dVal;
						}

						cBlue = cVal;

						dVal = floor(255.0 * pdColor[3]);
						if (dVal < 0.0)
						{
							cVal = 0;
						}
						else if (dVal > 255.0)
						{
							cVal = 255;
						}
						else
						{
							cVal = (unsigned char)dVal;
						}

						cAlpha = cVal;
						return System::Drawing::Color::FromArgb(cAlpha, cRed, cGreen, cBlue);
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED(__FUNCTION__)
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				void ScriptVar::GetTensor(cli::array<int>^ % aiDim, cli::array<double>^ % adValue)
				{
					try
					{
						unsigned uDim, uEl, uDimCnt, uElCnt;
						Clu::Viz::View::GetVarTensorProp(Script->Handle, m_pcName, uDimCnt, uElCnt);

						std::vector<unsigned> vecDims(uDimCnt);
						std::vector<double> vecData(uElCnt);
						Clu::Viz::View::GetVarTensorData(Script->Handle, m_pcName, vecDims.data(), vecData.data());

						aiDim = gcnew cli::array<int>(uDimCnt);
						adValue = gcnew cli::array<double>(uElCnt);

						for (uDim = 0; uDim < uDimCnt; ++uDim)
						{
							aiDim[uDim] = int(vecDims[uDim]);
						}

						for (uEl = 0; uEl < uElCnt; ++uEl)
						{
							adValue[uEl] = vecData[uEl];
						}
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED(__FUNCTION__)
				}


				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				Clu::Net::CluImage^ ScriptVar::GetImage()
				{
					try
					{
						Clu::CIImage xImage;
						Clu::Viz::View::GetVarImage(Script->Handle, m_pcName, xImage);
						char* pcData = (char*) xImage.DataPointer();
						return gcnew Clu::Net::CluImage(xImage);
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED(__FUNCTION__)
				}


				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				void ScriptVar::Set(int iValue)
				{
					try
					{
						Clu::Viz::View::SetVarNumber(Script->Handle, m_pcName, iValue);
						_ValueChanged();
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED(__FUNCTION__)
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				void ScriptVar::Set(bool bValue)
				{
					try
					{
						Set((int)(bValue ? 1 : 0));
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED(__FUNCTION__)
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				void ScriptVar::Set(System::Decimal dValue)
				{
					try
					{
						Set(System::Convert::ToDouble(dValue));
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED(__FUNCTION__)
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				void ScriptVar::Set(double dValue)
				{
					try
					{
						Clu::Viz::View::SetVarNumber(Script->Handle, m_pcName, dValue);
						_ValueChanged();
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED(__FUNCTION__)
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				void ScriptVar::Set(float fValue)
				{
					try
					{
						Set((double)fValue);
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED(__FUNCTION__)
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				void ScriptVar::Set(System::String^ sValue)
				{
					try
					{
						Clu::Viz::View::SetVarString(Script->Handle, m_pcName, Clu::Net::Converter::ToIString(sValue).ToCString());
						_ValueChanged();
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED(__FUNCTION__)
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//void ScriptVar::SetVarStringList(array<String^>^ asValue)
				//{
				//	Clu::CArrayString xArrayString;
				//	xArrayString.New(asValue->Length);

				//	// convert array to const char* vector
				//	for (int i = 0; i < asValue->Length; i++)
				//	{
				//		xArrayString[i] = Clu::Net::Converter::ToIString(asValue[i]);
				//	}

				//	try
				//	{
				//		SetVarNumberVarStringList(Script->Handle, m_pcName, xArrayString);
				//	}
				//	CLU_CATCH_RETHROW_ALL_MANAGED(__FUNCTION__)
				//}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				void ScriptVar::Set(System::Drawing::Color colValue)
				{
					try
					{
						double pdColor[4];
						pdColor[0] = double(colValue.R) / 255.0;
						pdColor[1] = double(colValue.G) / 255.0;
						pdColor[2] = double(colValue.B) / 255.0;
						pdColor[3] = double(colValue.A) / 255.0;

						Clu::Viz::View::SetVarColor(Script->Handle, m_pcName, pdColor);
						_ValueChanged();
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED(__FUNCTION__)
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//void ScriptVar::SetVarColor(Rx::Net::Vector4D colValue)
				//{
				//	try
				//	{
				//		double pdColor[4];
				//		pdColor[0] = colValue.X;
				//		pdColor[1] = colValue.Y;
				//		pdColor[2] = colValue.Z;
				//		pdColor[3] = colValue.W;

				//		SetVarNumberVarColor(Script->Handle, m_pcName, pdColor);
				//	}
				//	CLU_CATCH_RETHROW_ALL_MANAGED(__FUNCTION__)
				//}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				void ScriptVar::SetTensor(cli::array<int>^ aiDim, cli::array<double>^ adData)
				{
					try
					{
						std::vector<int> vecDims(aiDim->Length);
						std::vector<double> vecData(adData->Length);

						unsigned uDimCnt = unsigned(aiDim->Length);
						unsigned uElCnt = unsigned(adData->Length);

						for (unsigned uDim = 0; uDim < uDimCnt; ++uDim)
						{
							vecDims[uDim] = aiDim[uDim];
						}

						for (unsigned uEl = 0; uEl < uElCnt; ++uEl)
						{
							vecData[uEl] = adData[uEl];
						}

						Clu::Viz::View::SetVarTensor(Script->Handle, m_pcName, int(uDimCnt), vecDims.data(), vecData.data());
						_ValueChanged();
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED(__FUNCTION__)
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				void ScriptVar::Set(Clu::Net::CluImage^ xImage)
				{
					try
					{
						Clu::Viz::View::SetVarImage(Script->Handle, m_pcName, xImage->ToIImage());
						_ValueChanged();
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED(__FUNCTION__)

				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				void ScriptVar::Set(Clu::Net::DataContainer^ xData)
				{
					try
					{
						Clu::Viz::View::SetVarData(Script->Handle, m_pcName, xData->ToIDataContainer());
						_ValueChanged();
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED(__FUNCTION__)
				}

#pragma endregion

			} // namespace View
		} // namespace Net
	} // namespace Viz
} // namespace Clu
