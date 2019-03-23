////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Net.View
// file:      ScriptVar.h
//
// summary:   Declares the script variable class
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

#pragma managed (push, off)
#include "CluTec.Types1/IString.h"
#pragma managed (pop)

#include "ScriptMessageArgs.h"

namespace Clu
{
	namespace Viz
	{
		namespace Net
		{
			namespace View
			{
				ref class CluScript;

				public ref class ScriptVar
				{
				private:
					Clu::CIString *m_pName;
					const char* m_pcName;

					CluScript^ m_xScript;

				private:
					ScriptVar()
					{}

				public:
					ScriptVar(System::String^ sName, CluScript^ xScript);
					~ScriptVar();
					!ScriptVar();

				public:
					event ScriptMessageHandler^ ScriptMessage;

					property System::String^ Name
					{
						System::String^ get();
					}

					property CluScript^ Script
					{
						CluScript^ get()
						{
							return m_xScript;
						}
					}

					property bool SendMessageToScriptOnChange;
					property System::String^ ScriptMessageName;

				protected:
					void _ValueChanged();

				public:
					int GetInt();
					bool GetBool();
					float GetFloat();
					double GetDouble();
					System::Decimal GetDecimal();
					System::String^ GetString();
					System::String^ GetString(int iMaxLen);
					System::Drawing::Color GetColor();
					void GetTensor(cli::array<int>^% aiDim, cli::array<double>^% adValue);
					Clu::Net::CluImage^ GetImage();

					void Set(int iValue);
					void Set(bool bValue);
					void Set(float fValue);
					void Set(double dValue);
					void Set(System::Decimal dValue);
					void Set(System::String^ sValue);
					//void SetVarStringList(array<System::String^>^ asValue);
					void Set(System::Drawing::Color colValue);
					//void SetVarColor(Clu::Net::Vector4 colValue);
					void SetTensor(cli::array<int>^ aiDim, cli::array<double>^ adData);
					void Set(Clu::Net::CluImage^ xImage);
					void Set(Clu::Net::DataContainer^ xData);


					ScriptVar^ operator= (int iValue)
					{
						Set(iValue);
						return this;
					}

					ScriptVar^ operator= (bool bValue)
					{
						Set(bValue);
						return this;
					}

					ScriptVar^ operator= (float fValue)
					{
						Set(fValue);
						return this;
					}

					ScriptVar^ operator= (double dValue)
					{
						Set(dValue);
						return this;
					}

					ScriptVar^ operator= (System::String^ sValue)
					{
						Set(sValue);
						return this;
					}

					ScriptVar^ operator= (System::Drawing::Color colValue)
					{
						Set(colValue);
						return this;
					}

					operator int()
					{
						return GetInt();
					}

					operator unsigned()
					{
						return (unsigned)GetInt();
					}

					operator float()
					{
						return GetFloat();
					}

					operator double()
					{
						return GetDouble();
					}

					operator System::String ^ ()
					{
						return GetString();
					}

					operator System::Drawing::Color()
					{
						return GetColor();
					}
				};

			} // namespace View
		} // namespace Net
	} // namespace Viz
} // namespace Clu
