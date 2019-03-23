////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CodeBase.h
//
// summary:   Declares the code base class
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

#if !defined(AFX_CODEBASE_H__85899394_3862_4967_B06C_A84E787CB1DE__INCLUDED_)
	#define AFX_CODEBASE_H__85899394_3862_4967_B06C_A84E787CB1DE__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif	// _MSC_VER > 1000

#include "CodeVarList.h"
#include "Stack.h"
#include "CodeErrorList.h"

	#define NS_CURRENT "current"
	#define NS_LOCAL "local"
	#define NS_GLOBAL "global"

	typedef CCodeVar* TCodeVarPtr;
	typedef CCodeVarList* TCodeVarListPtr;

	class CCodeBase
	{
	public:

		CCodeBase();
		virtual ~CCodeBase();

		bool Reset() { ResetConstVarList(); ResetVarList(); ResetStack(); ResetTextOutput(); return true; }
		bool ResetConstVarList() { m_mConstVarList.Reset(); return true; }
		bool ResetVarList() { m_mVarList.Reset(); return true; }
		bool ResetStack() { return m_mVarStack.Reset(); }
		bool ResetTextOutput() { m_csOutput = ""; return true; }
		void ResetTempVars() { DeleteTempVar(0, int(m_mTempVarList.Count())); }

		void ReserveStack(uint uStackCnt, uint uLocalStackCnt)
		{
			m_mVarStack.Reserve(uStackCnt);
			m_mLocalVarStack.Reserve(uLocalStackCnt);
		}

		void ReserveTempVars(uint uCount)
		{
			m_mTempVarList.Reserve(uCount);
		}

		bool Push(CCodeVar* pVar) { return m_mVarStack.Push(pVar); }
		bool Pop(TCodeVarPtr& pVar) { return m_mVarStack.Pop(pVar); }
		int GetActStackDepth() { return m_mVarStack.GetActStackDepth(); }

		int LockStack() { return m_mVarStack.LockStack(); }
		int UnlockStack() { return m_mVarStack.UnlockStack(); }

		bool PushLocal(TCodeVarListPtr pVarList) { return m_mLocalVarStack.Push(pVarList); }
		bool PopLocal(TCodeVarListPtr& pVarList) { return m_mLocalVarStack.Pop(pVarList); }
		TCodeVarListPtr GetStackLocalVarList(int i)
		{ int iC = int(m_mLocalVarStack.Count()) - i - 1; if (iC < 0) { return 0; } else{ return m_mLocalVarStack[iC]; } }

		// Returns Variable of type PDT_NOTYPE if error occured.
		// Creates variable in namespace. If pcNamespace == 0, the creates variable
		// in local if local exists. If not creates in global.
		CCodeVar& NewVar(const char* pcName, ECodeDataType _nType = PDT_INT, const char* pcNamespace = 0);
		// Deletes variable using same principle as in NewVar.
		bool DeleteVar(const char* pcName, const char* pcNamespace = 0);

		// Returns Variable of type PDT_NOTYPE if error occured.
		CCodeVar& NewConstVar(const char* pcName, ECodeDataType _nType = PDT_INT)
		{ m_mConstVarList.New(pcName, _nType); return m_mConstVarList[pcName]; }

		bool DeleteConstVar(const char* pcName) { return m_mConstVarList.Delete(pcName); }

		// Return a temporary variable. Throws exception if error occured.
		CCodeVar& NewTempVar(ECodeDataType _nType = PDT_NOTYPE);

		// Check whether variable of given name is temporary variable.
		bool IsTempVar(CCodeVar* pVar);

		// Delete a list of temporary variables
		void DeleteTempVar(int iStartIdx, int iCount = 1);

		// Forget a temporary variable. That is, remove the reference
		// from the temp var list, but do not delete the instance.
		// This is used to move a variable from the temp var list
		// into a var list.
		// returns true if the given variable pointer could be found
		// in the temp var list.
		bool ForgetTempVar(CCodeVar* pVar);

		// Get current number of temp vars
		int TempVarCount() { return (int) m_mTempVarList.Count(); }

		// Returns Variable of type PDT_NOTYPE if error occured.
		// Searches in namespace but always first in ConstVarList.
		// If pcNamespace == 0, then searches in all namespaces
		CCodeVar& GetVar(const char* pcName, const char* pcNamespace = 0);

//	CCodeVar& GetVar(int i) { return m_mVarList[i]; }
//	CCodeVar& GetConstVar(int i) { return m_mConstVarList[i]; }

		// Returns i'th element on stack where 0 is element on top of stack.
		// Returns 0 if error occured.
		TCodeVarPtr GetStackVar(int i)
		{ int iC = int(m_mVarStack.Count()) - i - 1; if (iC < 0) { return 0; } else{ return m_mVarStack[iC]; } }

		void SetCurrentNamespace(const char* pcNamespace);

		int ConstVarCount() { return m_mConstVarList.Count(); }
		CCodeVarList::TVarMapIt GetVarBegin() { return m_mVarList.Begin(); }
		CCodeVarList::TVarMapIt GetVarEnd() { return m_mVarList.End(); }
		int StackCount() { return (int) m_mVarStack.Count(); }
		int ActStackCount() { return m_mVarStack.GetActStackDepth(); }

		void SetLoopCountLimit(int iLimit) { m_iLoopCountLimit = iLimit; }
		int GetLoopCountLimit() { return m_iLoopCountLimit; }

		CStrMem& GetTextOutput() { return m_csOutput; }

		CCodeErrorList m_ErrorList;

	protected:

		MemObj<CCodeVar> m_mTempVarList;

		CCodeVarList m_mConstVarList;	// Variable List for pre-defined constants
		CCodeVarList m_mVarList;	// Variable List for user variables.
		CStack<TCodeVarPtr> m_mVarStack;
		CStack<TCodeVarListPtr> m_mLocalVarStack;	// Stack of pointer to lists of local variables

		CStrMem m_csOutput;
		CStrMem m_csCurNamespace;	// Current namespace

		int m_iLoopCountLimit;	// Maximum evaluations of a loop before error.
	};

#endif	// !defined(AFX_CODEBASE_H__85899394_3862_4967_B06C_A84E787CB1DE__INCLUDED_)
