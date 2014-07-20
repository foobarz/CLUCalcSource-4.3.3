/// LICENSE START
////////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the CLUDraw Library.
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



// Class written by Vladimir Banarer, 3.12.2001.


// OGLBaseElementList.cpp: Implementierung der Klasse COGLBaseElementList.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "OGLBaseElementList.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLBaseElementList::COGLBaseElementList()
{

}

COGLBaseElementList::~COGLBaseElementList()
{

}

//////////////////////////////////////////////////////////////////////
// Apply
//////////////////////////////////////////////////////////////////////
bool COGLBaseElementList::Apply(int iMode, void *pvData)
{
	int i, n = m_mElementList.Count();

	glPushMatrix();


	for(i=0;i<n;i++)
	{
		if (!m_mElementList[i]->Apply(iMode, pvData))
		{
			glPopMatrix();
			return false;
		}
	}

	glPopMatrix();


	return true;
}
//////////////////////////////////////////////////////////////////////
// Add
//////////////////////////////////////////////////////////////////////

bool COGLBaseElementList::Add(COGLBaseElement& rElement)
{
	int n = m_mElementList.Count();
	m_mElementList.Add(1);
	m_mElementList[n] = &rElement;


	return true;
}
//////////////////////////////////////////////////////////////////////
// Delete
//////////////////////////////////////////////////////////////////////
bool COGLBaseElementList::Delete(COGLBaseElement& rElement, int DeleteLists)
{
	int n = m_mElementList.Count(), i;
	COGLBaseElementList *pDummy;

	for(i = 0; i < n; i++)
	{
		if(m_mElementList[i] == &rElement)
		{
			m_mElementList.Del(i,1);
		}
		else
		{
			if(DeleteLists != 0)
			{
				pDummy = dynamic_cast <COGLBaseElementList*>(m_mElementList[i]);
				if(pDummy)
				{
					pDummy->Delete(rElement,DeleteLists-1);
				}
			}
		}
	}


	return true;

}

bool COGLBaseElementList::Delete(cStr& rName, int DeleteLists)
{
	int n = m_mElementList.Count(), i;
	COGLBaseElementList *pDummy;


	for(i = 0; i < n; i++)
	{
		if(m_mElementList[i]->GetName() == rName)
		{
			m_mElementList.Del(i,1);
		}
		else
		{
			if(DeleteLists != 0)
			{
				pDummy = dynamic_cast <COGLBaseElementList*>(m_mElementList[i]);
				if(pDummy)
				{
					pDummy->Delete(rName,DeleteLists-1);
				}
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Replace
//////////////////////////////////////////////////////////////////////
bool COGLBaseElementList::Replace(COGLBaseElement& rOldElement, COGLBaseElement& rNewElement, int ReplaceLists)
{
	int n = m_mElementList.Count(), i;
	COGLBaseElementList *pDummy;

	for(i = 0; i < n; i++)
	{
		if(m_mElementList[i] == &rOldElement)
		{
			m_mElementList[i] = &rNewElement;
		}
		else
		{
			if(ReplaceLists != 0)
			{
				pDummy = dynamic_cast <COGLBaseElementList*>(m_mElementList[i]);
				if(pDummy)
				{
					pDummy->Replace(rOldElement,rNewElement,ReplaceLists-1);
				}
			}
		}
	}
	return true;

}

bool COGLBaseElementList::Replace(cStr& rName, COGLBaseElement& rNewElement, int ReplaceLists)
{
	int n = m_mElementList.Count(), i;
	COGLBaseElementList *pDummy;

	for(i = 0; i < n; i++)
	{
		if(m_mElementList[i]->GetName() == rName)
		{
			m_mElementList[i] = &rNewElement;
		}
		else
		{
			if(ReplaceLists != 0)
			{
				pDummy = dynamic_cast <COGLBaseElementList*>(m_mElementList[i]);
				if(pDummy)
				{
					pDummy->Replace(rName,rNewElement,ReplaceLists-1);
				}
			}
		}
	}
	return true;
}

