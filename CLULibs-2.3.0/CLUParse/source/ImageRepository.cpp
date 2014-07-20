/// LICENSE START
////////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the CLUParse Library.
//
// 
// Copyright (C) 2001-2004  Christian B.U. Perwass
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

// Stack.cpp: Implementierung der Klasse CStack.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ImageRepository.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CImageRepository::CImageRepository()
{

}


CImageRepository::~CImageRepository()
{	
	Reset();
}


//////////////////////////////////////////////////////////////////////
// Register Image Reference

bool CImageRepository::Register(CImageReference& rRef)
{
	TImgToRefMap::iterator it_End, it_El;

	if (rRef.m_pImage == 0)
		return false;

	// Check whether Ref references image that is already in repository
	if ((it_El = m_mapImgToRef.find(rRef.m_pImage)) == m_mapImgToRef.end())
	{
		// if not, then add it to repository
		m_mapImgToRef[rRef.m_pImage].push_back(&rRef);
	}
	else
	{
		// If yes, then add Ref to list
		it_El->second.push_back(&rRef);
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
// DeRegister Image Reference

bool CImageRepository::DeRegister(CImageReference& rRef)
{
	TImgToRefMap::iterator it_End, it_El;

	if (rRef.m_pImage == 0)
		return false;

	// Check whether Ref references image that is in repository
	if ((it_El = m_mapImgToRef.find(rRef.m_pImage)) == m_mapImgToRef.end())
	{
		// if not, then there is a problem
		return false;
	}
	else
	{
		// If yes, then find it and remove it
		TImgRefList &rList = it_El->second;

		// Remove reference from list
		rList.remove(&rRef);

		// Check whether there are any references left
		if (rList.empty())
		{
			// if nothing is left then delete image
			delete it_El->first;

			// remove the entry we found from the map
			m_mapImgToRef.erase(it_El);
		}
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
// Create a new image and return the reference

CImageReference* CImageRepository::New()
{
	TImagePtr pImage;
	CImageReference *pImgRef;

	pImgRef = new CImageReference;
	
	if (!(pImage = new TImage))
		return pImgRef;

	pImgRef->Set(pImage, this);

	Register(*pImgRef);

	return pImgRef;
}


//////////////////////////////////////////////////////////////////////
// Create a new image which is a copy of the one given

CImageReference* CImageRepository::Copy(CImageReference& rRef)
{
	TImagePtr pImage;
	CImageReference *pImgRef;
	
	pImgRef = new CImageReference;

	if (!(pImage = new TImage(*rRef.m_pImage)))
		return pImgRef;

	pImgRef->Set(pImage, this);

	Register(*pImgRef);

	return pImgRef;	
}


//////////////////////////////////////////////////////////////////////
// Delete all images

void CImageRepository::Reset()
{
	TImgToRefMap::iterator it_El, it_End;
	TImgRefList::iterator it_LEl, it_LEnd;

	it_End = m_mapImgToRef.end();

	for( it_El = m_mapImgToRef.begin();
		 it_El != it_End;
		 ++it_El)
	{
		TImgRefList rList = it_El->second;
		it_LEnd = rList.end();

		// Invalidate all references
		for( it_LEl = rList.begin();
			 it_LEl != it_LEnd;
			 ++it_LEl)
		{
			(*it_LEl)->Invalidate();
		}

		delete it_El->first;
	}

	// Delete the whole map
	m_mapImgToRef.erase(m_mapImgToRef.begin(), m_mapImgToRef.end());
}

