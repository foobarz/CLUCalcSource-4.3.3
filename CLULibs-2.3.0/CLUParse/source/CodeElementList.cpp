// OGLBaseElementList.cpp: Implementierung der Klasse CCodeElementList.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "CodeElementList.h"
#include "CodeBase.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCodeElementList::CCodeElementList()
{

}

CCodeElementList::~CCodeElementList()
{
	// Delete all owned elements.
	Delete(0, m_mElementList.Count());
}

//////////////////////////////////////////////////////////////////////
// Apply

bool CCodeElementList::Apply(CCodeBase* pCodeBase, SCodeData *pData)
{
	if (!pCodeBase) 
		return false;

	int i, n = m_mElementList.Count();

	if (pCodeBase->LockStack() < 0)
		return false;

	SCodeData sData;

	for(i=0;i<n;i++)
	{
		SCodeElementPtr &rEl = m_mElementList[i];

		sData.Set(rEl.iTextLine, rEl.iTextPos, this);

		if (!rEl.pElement->Apply(pCodeBase, &sData))
		{
			pCodeBase->UnlockStack();
			return false;
		}
	}

	pCodeBase->UnlockStack();

	return true;
}

//////////////////////////////////////////////////////////////////////
// Insert

bool CCodeElementList::Insert(CCodeElement& rElement, int iPos, bool bOwner,
							  int iTextLine, int iTextPos)
{
	if (iPos < 0)
		iPos = m_mElementList.Count();
	else if (iPos > (int) m_mElementList.Count())
		return false;

	if (!m_mElementList.Insert(iPos))
		return false;

	SCodeElementPtr &rEl = m_mElementList[iPos];

	rEl.pElement = &rElement;
	rEl.bOwner = bOwner;
	rEl.iTextLine = iTextLine;
	rEl.iTextPos = iTextPos;


	return true;
}


//////////////////////////////////////////////////////////////////////
// Delete

bool CCodeElementList::Delete(CCodeElement& rElement, int DeleteLists)
{
	int n = m_mElementList.Count(), i;
	CCodeElementList *pDummy;

	for(i = 0; i < n; i++)
	{
		if(m_mElementList[i].pElement == &rElement)
		{
			if (m_mElementList[i].bOwner)
				delete m_mElementList[i].pElement;

			m_mElementList.Del(i,1);
		}
		else
		{
			if(DeleteLists != 0)
			{
				pDummy = dynamic_cast <CCodeElementList*>(m_mElementList[i].pElement);
				if(pDummy)
				{
					pDummy->Delete(rElement,DeleteLists-1);
				}
			}
		}
	}


	return true;

}

//////////////////////////////////////////////////////////////////////
/// Delete

bool CCodeElementList::Delete(cStr& rName, int DeleteLists)
{
	int n = m_mElementList.Count(), i;
	CCodeElementList *pDummy;


	for(i = 0; i < n; i++)
	{
		if(m_mElementList[i].pElement->GetName() == rName)
		{
			if (m_mElementList[i].bOwner)
				delete m_mElementList[i].pElement;

			m_mElementList.Del(i,1);
		}
		else
		{
			if(DeleteLists != 0)
			{
				pDummy = dynamic_cast<CCodeElementList*>(m_mElementList[i].pElement);
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
// Delete iNo elements starting from iPos

bool CCodeElementList::Delete(int iPos, int iNo)
{
	int i, iLen = m_mElementList.Count();
	int iMax = iPos + iNo;

	if (iPos < 0 || iMax > iLen)
		return false;

	for(i=iPos;i<iMax;i++)
	{
		if (m_mElementList[i].bOwner)
			delete m_mElementList[i].pElement;
	}

	m_mElementList.Del(iPos, iNo);

	return true;
}


