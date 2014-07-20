/// LICENSE START
////////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the CLU Library.
//
// 
// Copyright (C) 1997-2004  Christian B.U. Perwass
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// The GNU Library General Public License can be found in the file
// license.txt distributed with this library.
//
// Please send all queries and comments to
//
// email: help@clucalc.info
// mail: Institut fuer Informatik, Olshausenstr. 40, 24098 Kiel, Germany.
//
////////////////////////////////////////////////////////////////////////////////////
/// LICENSE END


#ifndef __POINT_H__
#define __POINT_H__

#include "StaticArray.h"


template<class CType, int CNo>
class CPointND : public CStaticArray<CType,CNo>
{
public:
	CPointND() : CStaticArray<CType,CNo>() {};

	CPointND<CType, CNo>& operator= (const CType *na)
		{ CStaticArray<CType,CNo>::operator=(na); return *this;}

	CPointND<CType, CNo>& operator= (const CPointND<CType, CNo>& rPt)
		{ CStaticArray<CType,CNo>::operator=(rPt); return *this;}
	
	CPointND<CType,CNo>& operator *= (const CType& f)
	{ for(int i=0;i<CNo;this->pData[i++] *= f); return *this; }

	CPointND<CType,CNo>& operator /= (const CType& f)
	{ for(int i=0;i<CNo;this->pData[i++] /= f); return *this; }


	CPointND<CType,CNo> operator + (const CType& f)
	{ CPointND<CType,CNo> result;
	  for(int i=0;i<CNo;i++) result.pData[i] = this->pData[i] + f; 
	  return result; 
	}

	CPointND<CType,CNo> operator - (const CType& f)
	{ CPointND<CType,CNo> result;
	  for(int i=0;i<CNo;i++) result.pData[i] = this->pData[i] - f; 
	  return result; 
	}

	CPointND<CType,CNo> operator / (const CType& f)
	{ CPointND<CType,CNo> result;
	  for(int i=0;i<CNo;i++) result.pData[i] = this->pData[i] / f; 
	  return result; 
	}

	CPointND<CType,CNo> operator *(const CType& f) const
	{ CPointND<CType,CNo> result;
	  for(int i=0;i<CNo;i++) result.pData[i] = this->pData[i] * f; 
	  return result; 
	}

	
	CPointND<CType,CNo>& operator += (const CPointND<CType,CNo>& _Point)
	{ for(int i=0;i<CNo;i++) this->pData[i] += _Point.pData[i]; return *this; }

	CPointND<CType,CNo>& operator -= (const CPointND<CType,CNo>& _Point)
	{ for(int i=0;i<CNo;i++) this->pData[i] -= _Point.pData[i]; return *this; }

	CPointND<CType,CNo>& operator *= (const CPointND<CType,CNo>& _Point)
	{ for(int i=0;i<CNo;i++) this->pData[i] *= _Point.pData[i]; return *this; }

	CPointND<CType,CNo>& operator /= (const CPointND<CType,CNo>& _Point)
	{ for(int i=0;i<CNo;i++) this->pData[i] /= _Point.pData[i]; return *this; }


	CPointND<CType,CNo> operator + (const CPointND<CType,CNo>& _Point)
	{	CPointND<CType,CNo> result;
		for(int i=0;i<CNo;i++) result.pData[i] = this->pData[i] + _Point.pData[i]; 
		return result; 
	}

	CPointND<CType,CNo> operator - (const CPointND<CType,CNo>& _Point)
	{	CPointND<CType,CNo> result;
		for(int i=0;i<CNo;i++) result.pData[i] = this->pData[i] - _Point.pData[i]; 
		return result; 
	}


};



#endif

