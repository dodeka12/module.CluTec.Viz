////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Net.Wnd
// file:      Defines.h
//
// summary:   Declares the defines class
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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	A macro that catches native and managed exceptions and re-throws them. </summary>
///
/// <remarks>	Perwass,. </remarks>
///
/// <param name="theMsg">	Message describing the exception. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////
#define CLU_CATCH_RETHROW_ALL_LE_MANAGED(theMsg) \
	catch (std::exception& xEx) \
	{ \
		throw gcnew Clu::Net::CluException(theMsg, __FILE__, __FUNCTION__, __LINE__, gcnew System::Exception(Clu::Net::Converter::ToString(xEx.what()))); \
	} \
	catch (Clu::CIException& xEx) \
	{ \
		throw gcnew Clu::Net::CluException(Clu::Viz::Net::Wnd::Engine::_FormatLastError(theMsg), __FILE__, __FUNCTION__, __LINE__, Clu::Net::Converter::ToException(xEx)); \
	} \
	catch (System::Exception^ xEx) \
	{ \
		throw gcnew Clu::Net::CluException(theMsg, __FILE__, __FUNCTION__, __LINE__, xEx); \
	} 

#define CLU_CATCH_RETHROW_ALL_LE_VIEW_MANAGED(theMsg) \
	catch (std::exception& xEx) \
	{ \
		throw gcnew Clu::Net::CluException(theMsg, __FILE__, __FUNCTION__, __LINE__, gcnew System::Exception(Clu::Net::Converter::ToString(xEx.what()))); \
	} \
	catch (Clu::CIException& xEx) \
	{ \
		throw gcnew Clu::Net::CluException(_FormatAndSetLastError(theMsg), __FILE__, __FUNCTION__, __LINE__, Clu::Net::Converter::ToException(xEx)); \
	} \
	catch (System::Exception^ xEx) \
	{ \
		throw gcnew Clu::Net::CluException(theMsg, __FILE__, __FUNCTION__, __LINE__, xEx); \
	} 


#define CLU_CATCH_RETHROW_ALL_MANAGED(theMsg) \
	catch (std::exception& xEx) \
				{ \
		throw gcnew Clu::Net::CluException(theMsg, __FILE__, __FUNCTION__, __LINE__, gcnew System::Exception(Clu::Net::Converter::ToString(xEx.what()))); \
				} \
	catch (Clu::CIException& xEx) \
				{ \
		throw gcnew Clu::Net::CluException(theMsg, __FILE__, __FUNCTION__, __LINE__, Clu::Net::Converter::ToException(xEx)); \
				} \
	catch (System::Exception^ xEx) \
				{ \
		throw gcnew Clu::Net::CluException(theMsg, __FILE__, __FUNCTION__, __LINE__, xEx); \
				} 


#define CLU_THROW_MANAGED(theMsg) \
	throw gcnew Clu::Net::CluException(theMsg, __FILE__, __FUNCTION__, __LINE__);
