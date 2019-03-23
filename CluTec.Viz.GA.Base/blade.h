////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.GA.Base
// file:      blade.h
//
// summary:   Declares the blade class
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

#ifndef _BLADE_H_
#define _BLADE_H_


#include "base.h"
#include "CluTec.Viz.Base\mem.h"


#define BS_NORMAL   0x0001 // Normal Blade String Output
#define BS_EXPLICIT 0x0002 // Explicit Blade String output

#ifndef _MAXSTRSIZE_
#define _MAXSTRSIZE_ 1024   // Maximum String Size for String output
#endif


#ifdef _GNUCPP3_
	#ifndef _TMPL_
	#define _TMPL_ <>
	#endif
#	ifndef _FRIEND_
#		define _FRIEND_  friend 
#	endif
#else
	#ifndef _TMPL_
	#define _TMPL_ <>
	#endif
#	ifndef _FRIEND_
#		define _FRIEND_  friend 
#	endif
#endif

typedef unsigned uint;
typedef unsigned long ulong;

template<class CType> class MultiV;
template<class CType> class Blade;


template<class CType> int operator== (const Blade<CType> &,const Blade<CType> &);

template<class CType> Blade<CType> operator+ (const Blade<CType> &,const Blade<CType> &);
template<class CType> Blade<CType>& operator+= (Blade<CType> &,const Blade<CType> &);
template<class CType> Blade<CType> operator- (const Blade<CType> &, const Blade<CType> &);
template<class CType> Blade<CType>& operator-= (Blade<CType> &,const Blade<CType> &);

template<class CType> Blade<CType> operator^ (const Blade<CType> &, const CType &);
template<class CType> Blade<CType> operator^ (const CType &, const Blade<CType> &);
template<class CType> Blade<CType>& operator^= (Blade<CType> &,const CType &a);

template<class CType> Blade<CType> operator- (const Blade<CType> &);  // Negation
template<class CType> Blade<CType> operator~ (const Blade<CType> &);  // Reverse
template<class CType> Blade<CType> operator! (const Blade<CType> &);  // Inverse

// Geometric Product &
template<class CType> Blade<CType> operator& (const Blade<CType> &,const Blade<CType> &);
// Inner Product *
template<class CType> Blade<CType> operator* (const Blade<CType> &,const Blade<CType> &);
// Outer Product ^
template<class CType> Blade<CType> operator^ (const Blade<CType> &,const Blade<CType> &);

// Commutator Product
template<class CType> Blade<CType> cp (const Blade<CType> &a,const Blade<CType> &b);

// Anti-Commutator Product
template<class CType> Blade<CType> acp (const Blade<CType> &a,const Blade<CType> &b);

template<class CType> Blade<CType> join (const Blade<CType> &, const Blade<CType> &);
template<class CType> Blade<CType> meet (const Blade<CType> &, const Blade<CType> &);


template<class CType> MultiV<CType> operator+  (const MultiV<CType>&,const Blade<CType>&);
template<class CType> MultiV<CType> operator+  (const Blade<CType>&,const MultiV<CType>&);
template<class CType> MultiV<CType>& operator+=  (MultiV<CType>&,const Blade<CType>&);
template<class CType> MultiV<CType>& operator<<  (MultiV<CType>&,const Blade<CType>&);
template<class CType> MultiV<CType> operator-  (const MultiV<CType>&,const Blade<CType>&);
template<class CType> MultiV<CType> operator-  (const Blade<CType>&,const MultiV<CType>&);
template<class CType> MultiV<CType>& operator-=  (MultiV<CType>&,const Blade<CType>&);


// Blade Class --------------------------------------------------

template <class CType>
class Blade
{
public:
	Blade();
	Blade(Base<CType> &nbase, CType nfact=0, uint ngrade=0, 
		uint ncomps[]=0, char* nname=0);
	Blade(const Blade<CType> &a);
	virtual ~Blade();
	
	void Reset(Base<CType> &nbase, CType nfact=0, uint ngrade=0, 
		uint ncomps[]=0, char* nname=0);


	CType Factor() const { return m_xFact; }
	uint Grade() const { return m_uGrade; }
	void makePS();
	
	Blade<CType>& operator=(const CType f);
	Blade<CType>& operator=(const uint ncomps[]);
	Blade<CType>& operator=(const Blade<CType> &a);
/*
	_FRIEND_ int operator== _TMPL_(const Blade<CType> &,const Blade<CType> &);
	
	_FRIEND_ Blade<CType> operator+ _TMPL_(const Blade<CType> &,const Blade<CType> &);
	_FRIEND_ Blade<CType>& operator+= _TMPL_(Blade<CType> &,const Blade<CType> &);
	_FRIEND_ Blade<CType> operator- _TMPL_(const Blade<CType> &, const Blade<CType> &);
	_FRIEND_ Blade<CType>& operator-= _TMPL_(Blade<CType> &,const Blade<CType> &);
	
	_FRIEND_ Blade<CType> operator^ _TMPL_(const Blade<CType> &, const CType &);
	_FRIEND_ Blade<CType> operator^ _TMPL_(const CType &, const Blade<CType> &);
	_FRIEND_ Blade<CType>& operator^= _TMPL_(Blade<CType> &,const CType &a);
	
	_FRIEND_ Blade<CType> operator- _TMPL_(const Blade<CType> &);  // Negation
	_FRIEND_ Blade<CType> operator~ _TMPL_(const Blade<CType> &);  // Reverse
	_FRIEND_ Blade<CType> operator! _TMPL_(const Blade<CType> &);  // Inverse
    
	// Geometric Product &
	_FRIEND_ Blade<CType> operator& _TMPL_(const Blade<CType> &,const Blade<CType> &);
	// Inner Product *
	_FRIEND_ Blade<CType> operator* _TMPL_(const Blade<CType> &,const Blade<CType> &);
	// Outer Product ^
	_FRIEND_ Blade<CType> operator^ _TMPL_(const Blade<CType> &,const Blade<CType> &);

	// Commutator Product
	_FRIEND_ Blade<CType> cp _TMPL_(const Blade<CType> &a,const Blade<CType> &b);

	// Anti-Commutator Product
	_FRIEND_ Blade<CType> acp _TMPL_(const Blade<CType> &a,const Blade<CType> &b);
	
	_FRIEND_ Blade<CType> join _TMPL_(const Blade<CType> &, const Blade<CType> &);
	_FRIEND_ Blade<CType> meet _TMPL_(const Blade<CType> &, const Blade<CType> &);
	
	
	_FRIEND_ MultiV<CType> operator+  _TMPL_(const MultiV<CType>&,const Blade<CType>&);
	_FRIEND_ MultiV<CType> operator+  _TMPL_(const Blade<CType>&,const MultiV<CType>&);
	_FRIEND_ MultiV<CType>& operator+=  _TMPL_(MultiV<CType>&,const Blade<CType>&);
	_FRIEND_ MultiV<CType>& operator<<  _TMPL_(MultiV<CType>&,const Blade<CType>&);
	_FRIEND_ MultiV<CType> operator-  _TMPL_(const MultiV<CType>&,const Blade<CType>&);
	_FRIEND_ MultiV<CType> operator-  _TMPL_(const Blade<CType>&,const MultiV<CType>&);
	_FRIEND_ MultiV<CType>& operator-=  _TMPL_(MultiV<CType>&,const Blade<CType>&);
*/	
	
	char* Name() const { return m_sName.Str(); }
	char* Str(uint strtype = BS_NORMAL);
	
//protected:
	Base<CType> *m_pxBase;
	uint m_uGrade;
	CType m_xFact;
	Mem<uint> m_xComps;  //TODO: change to Mem<size_t>?
	CStrMem m_sName;
	CStrMem m_sStr;
	
	void takeOut(size_t);
	void putIn(uint,size_t);
	int check();
	
};



#endif // _BLADE_H_

