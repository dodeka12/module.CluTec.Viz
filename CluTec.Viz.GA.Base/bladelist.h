////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.GA.Base
// file:      bladelist.h
//
// summary:   Declares the bladelist class
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

#ifndef _BLADELIST_H_
#define _BLADELIST_H_


#include "base.h"
#include "blade.h"

#include "CluTec.Viz.Base\mem.h"
#include "CluTec.Viz.Base\dynlist.h"


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
	#define _TMPL_
	#endif
#	ifndef _FRIEND_
#		define _FRIEND_  friend 
#	endif
#endif


typedef unsigned uint;
typedef unsigned long ulong;

template<class CType> class MultiV;
template<class CType> class BladeList;

template<class CType> int operator== (const BladeList<CType> &,const Blade<CType> &); 

template<class CType> CType Scalar (const BladeList<CType> &bl);

template<class CType> BladeList<CType> operator+ (const BladeList<CType> &,
				const CType &);
template<class CType> BladeList<CType> operator+ (const CType &,
				const BladeList<CType> &);

template<class CType> BladeList<CType> operator- (const BladeList<CType> &,
				const CType &);
template<class CType> BladeList<CType> operator- (const CType &,
				const BladeList<CType> &);

template<class CType> BladeList<CType>& operator+= (BladeList<CType> &,const CType &);
template<class CType> BladeList<CType>& operator-= (BladeList<CType> &,const CType &);
template<class CType> BladeList<CType>& operator<< (BladeList<CType> &,const CType &);

template<class CType> BladeList<CType> operator+ (const BladeList<CType> &,
				const Blade<CType> &);
template<class CType> BladeList<CType> operator+ (const Blade<CType> &,
				const BladeList<CType> &);

template<class CType> BladeList<CType> operator- (const BladeList<CType> &,
				const Blade<CType> &);
template<class CType> BladeList<CType> operator- (const Blade<CType> &,
				const BladeList<CType> &);

template<class CType> BladeList<CType>& operator+= (BladeList<CType> &,const Blade<CType> &);
template<class CType> BladeList<CType>& operator-= (BladeList<CType> &,const Blade<CType> &);

// see note in cga.cc for comparison of << and += operator
template<class CType> BladeList<CType>& operator<< (BladeList<CType> &,
	const Blade<CType> &); 

template<class CType> BladeList<CType> operator+ (const BladeList<CType> &,
				const BladeList<CType> &);
template<class CType> BladeList<CType> operator- (const BladeList<CType> &,
				const BladeList<CType> &);
template<class CType> BladeList<CType>& operator+= (BladeList<CType> &,
	const BladeList<CType> &);
template<class CType> BladeList<CType>& operator-= (BladeList<CType> &,
	const BladeList<CType> &);

// see note in cga.cc for comparison of << and += operator
template<class CType> BladeList<CType>& operator<< (BladeList<CType> &,
	const BladeList<CType> &);

template<class CType> BladeList<CType> operator^ (const BladeList<CType> &,
				const CType &);
template<class CType> BladeList<CType> operator^ (const CType &,
				const BladeList<CType> &);
template<class CType> BladeList<CType>& operator^= (BladeList<CType> &,
	const CType &);

template<class CType> BladeList<CType> operator& (const BladeList<CType> &,
				const Blade<CType> &);
template<class CType> BladeList<CType> operator& (const Blade<CType> &,
				const BladeList<CType> &);
template<class CType> BladeList<CType>& operator&= (BladeList<CType> &,
	const Blade<CType> &);

template<class CType> BladeList<CType> operator* (const BladeList<CType> &,
				const Blade<CType> &);
template<class CType> BladeList<CType> operator* (const Blade<CType> &,
				const BladeList<CType> &);
template<class CType> BladeList<CType>& operator*= (BladeList<CType> &,
	const Blade<CType> &);

template<class CType> BladeList<CType> operator^ (const BladeList<CType> &,
				const Blade<CType> &);
template<class CType> BladeList<CType> operator^ (const Blade<CType> &,
				const BladeList<CType> &);
template<class CType> BladeList<CType>& operator^= (BladeList<CType> &,
	const Blade<CType> &);

template<class CType> BladeList<CType> operator& (const BladeList<CType> &,
				const BladeList<CType> &);
template<class CType> BladeList<CType>& operator&= (BladeList<CType> &,
	const BladeList<CType> &);
template<class CType> BladeList<CType> operator* (const BladeList<CType> &,
				const BladeList<CType> &);
template<class CType> BladeList<CType>& operator*= (BladeList<CType> &,
	const BladeList<CType> &);
template<class CType> BladeList<CType> operator^ (const BladeList<CType> &,
				const BladeList<CType> &);
template<class CType> BladeList<CType>& operator^= (BladeList<CType> &,
	const BladeList<CType> &);
// Reversion of Blades
template<class CType> BladeList<CType> operator~ (const BladeList<CType> &);   
// Negation of factors of Blades
template<class CType> BladeList<CType> operator- (const BladeList<CType> &); 
// Inversion of Blades
template<class CType> BladeList<CType> operator! (const BladeList<CType> &);   

template<class CType> BladeList<CType> cp (const BladeList<CType> &,
			    const Blade<CType> &);
template<class CType> BladeList<CType> acp (const BladeList<CType> &,
			const Blade<CType> &);
template<class CType> BladeList<CType> cp (const BladeList<CType> &,
			    const BladeList<CType> &);
template<class CType> BladeList<CType> acp (const BladeList<CType> &,
	const BladeList<CType> &);


// Blade List Class ----------------------------------------

template<class CType>
class BladeList : public DynList<Blade<CType> >
{
public:
	//  typedef Blade<CType> * PBlade;
	
	BladeList();
	BladeList(Base<CType> &nbase);
	BladeList(const BladeList<CType> &a);
	virtual ~BladeList();
	
	void Reset(Base<CType> &nbase);
	
	// Gives Position, where Blade can be found
//	_FRIEND_ int operator== _TMPL_(const BladeList<CType> &,const Blade<CType> &); 
	
	BladeList<CType> operator()(uint grade) const; // Returns List of Blades of grade
	CType operator()() const;  // Returns Scalar Part as CType
//	_FRIEND_ CType Scalar _TMPL_(const BladeList<CType> &bl);
	
	BladeList<CType>& operator=(const BladeList<CType> &a);
	BladeList<CType>& operator=(const Blade<CType> &a);
	BladeList<CType>& operator=(const MultiV<CType> &a);
/*	
	_FRIEND_ BladeList<CType> operator+ _TMPL_(const BladeList<CType> &,
				    const CType &);
	_FRIEND_ BladeList<CType> operator+ _TMPL_(const CType &,
				    const BladeList<CType> &);
	
	_FRIEND_ BladeList<CType> operator- _TMPL_(const BladeList<CType> &,
				    const CType &);
	_FRIEND_ BladeList<CType> operator- _TMPL_(const CType &,
				    const BladeList<CType> &);
	
	_FRIEND_ BladeList<CType>& operator+= _TMPL_(BladeList<CType> &,const CType &);
	_FRIEND_ BladeList<CType>& operator-= _TMPL_(BladeList<CType> &,const CType &);
	_FRIEND_ BladeList<CType>& operator<< _TMPL_(BladeList<CType> &,const CType &);
	
	_FRIEND_ BladeList<CType> operator+ _TMPL_(const BladeList<CType> &,
				    const Blade<CType> &);
	_FRIEND_ BladeList<CType> operator+ _TMPL_(const Blade<CType> &,
				    const BladeList<CType> &);
	
	_FRIEND_ BladeList<CType> operator- _TMPL_(const BladeList<CType> &,
				    const Blade<CType> &);
	_FRIEND_ BladeList<CType> operator- _TMPL_(const Blade<CType> &,
				    const BladeList<CType> &);
	
	_FRIEND_ BladeList<CType>& operator+= _TMPL_(BladeList<CType> &,const Blade<CType> &);
	_FRIEND_ BladeList<CType>& operator-= _TMPL_(BladeList<CType> &,const Blade<CType> &);
	
	// see note in cga.cc for comparison of << and += operator
	_FRIEND_ BladeList<CType>& operator<< _TMPL_(BladeList<CType> &,
		const Blade<CType> &); 
	
	_FRIEND_ BladeList<CType> operator+ _TMPL_(const BladeList<CType> &,
				    const BladeList<CType> &);
	_FRIEND_ BladeList<CType> operator- _TMPL_(const BladeList<CType> &,
				    const BladeList<CType> &);
	_FRIEND_ BladeList<CType>& operator+= _TMPL_(BladeList<CType> &,
		const BladeList<CType> &);
	_FRIEND_ BladeList<CType>& operator-= _TMPL_(BladeList<CType> &,
		const BladeList<CType> &);
	
	// see note in cga.cc for comparison of << and += operator
	_FRIEND_ BladeList<CType>& operator<< _TMPL_(BladeList<CType> &,
		const BladeList<CType> &);
    
	_FRIEND_ BladeList<CType> operator^ _TMPL_(const BladeList<CType> &,
				    const CType &);
	_FRIEND_ BladeList<CType> operator^ _TMPL_(const CType &,
				    const BladeList<CType> &);
	_FRIEND_ BladeList<CType>& operator^= _TMPL_(BladeList<CType> &,
		const CType &);
	
	_FRIEND_ BladeList<CType> operator& _TMPL_(const BladeList<CType> &,
				    const Blade<CType> &);
	_FRIEND_ BladeList<CType> operator& _TMPL_(const Blade<CType> &,
				    const BladeList<CType> &);
	_FRIEND_ BladeList<CType>& operator&= _TMPL_(BladeList<CType> &,
		const Blade<CType> &);
	
	_FRIEND_ BladeList<CType> operator* _TMPL_(const BladeList<CType> &,
				    const Blade<CType> &);
	_FRIEND_ BladeList<CType> operator* _TMPL_(const Blade<CType> &,
				    const BladeList<CType> &);
	_FRIEND_ BladeList<CType>& operator*= _TMPL_(BladeList<CType> &,
		const Blade<CType> &);
	
	_FRIEND_ BladeList<CType> operator^ _TMPL_(const BladeList<CType> &,
				    const Blade<CType> &);
	_FRIEND_ BladeList<CType> operator^ _TMPL_(const Blade<CType> &,
				    const BladeList<CType> &);
	_FRIEND_ BladeList<CType>& operator^= _TMPL_(BladeList<CType> &,
		const Blade<CType> &);
	
	_FRIEND_ BladeList<CType> operator& _TMPL_(const BladeList<CType> &,
				    const BladeList<CType> &);
	_FRIEND_ BladeList<CType>& operator&= _TMPL_(BladeList<CType> &,
		const BladeList<CType> &);
	_FRIEND_ BladeList<CType> operator* _TMPL_(const BladeList<CType> &,
				    const BladeList<CType> &);
	_FRIEND_ BladeList<CType>& operator*= _TMPL_(BladeList<CType> &,
		const BladeList<CType> &);
	_FRIEND_ BladeList<CType> operator^ _TMPL_(const BladeList<CType> &,
				    const BladeList<CType> &);
	_FRIEND_ BladeList<CType>& operator^= _TMPL_(BladeList<CType> &,
		const BladeList<CType> &);
	// Reversion of Blades
	_FRIEND_ BladeList<CType> operator~ _TMPL_(const BladeList<CType> &);   
	// Negation of factors of Blades
	_FRIEND_ BladeList<CType> operator- _TMPL_(const BladeList<CType> &); 
	// Inversion of Blades
	_FRIEND_ BladeList<CType> operator! _TMPL_(const BladeList<CType> &);   
	
	_FRIEND_ BladeList<CType> cp _TMPL_(const BladeList<CType> &,
			     const Blade<CType> &);
	_FRIEND_ BladeList<CType> acp _TMPL_(const BladeList<CType> &,
				const Blade<CType> &);
	_FRIEND_ BladeList<CType> cp _TMPL_(const BladeList<CType> &,
			     const BladeList<CType> &);
	_FRIEND_ BladeList<CType> acp _TMPL_(const BladeList<CType> &,
		const BladeList<CType> &);
*/
// The following are not implemented correctly
/*	
	_FRIEND_ BladeList<CType> join(const BladeList<CType> &, 
		const Blade<CType> &);
	_FRIEND_ BladeList<CType> meet(const BladeList<CType> &, 
		const Blade<CType> &);
	_FRIEND_ BladeList<CType> join(const BladeList<CType> &, 
		const BladeList<CType> &);
	_FRIEND_ BladeList<CType> meet(const BladeList<CType> &, 
		const BladeList<CType> &);
*/	
	
	
	CType mag2() const; // Magnitude of Blade List SQUARED(!)
	char* Str();
	Base<CType>* BasePtr() const { return base; }
	
//protected:
	Base<CType> *base;
	CStrMem str;
	
	uint check();
};


#endif // _BLADELIST_H_

