// VarList.cpp: Implementierung der Klasse CVarList.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "VarList.h"
#include "CodeVar.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CVarList::CVarList()
{
	m_pCodeList = 0;
}

CVarList::CVarList(const CVarList& rVarList)
{
	*this = rVarList;
}

CVarList::~CVarList()
{

}


CVarList& CVarList::operator= (const CVarList& rVarList)
{
	int i, iNo = (int) rVarList.Count();
	
	Set(iNo);

	for(i=0;i<iNo;i++)
	{
		(*this)[i].CopyInstance(rVarList[i]);	
	}

	return *this;
}

//////////////////////////////////////////////////////////////////////
// Return reference to dereferences var ptr variable

CCodeVar& CVarList::operator() (int iPos)
{
	return (*this)[iPos].DereferenceVarPtr(true);
}


//////////////////////////////////////////////////////////////////////
// Dereference Pointer variables in list

CVarList& CVarList::DereferencePtr()
{
	int i, iLen = Count();
	
	for(i=0;i<iLen;i++)
	{
		(*this)[i].DereferencePtr();
	}

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Order List according to index list

bool CVarList::Order(vector<int> &rIdxList)
{
	Mem<CCodeVar*> mNewData; // Data: Pointers to CType 	
	int iPos, iCount = rIdxList.size();

	if (iCount != m_mData.Count())
		return false;

	mNewData.Set(iCount);
	for (iPos = 0; iPos < iCount; iPos++)
	{
		mNewData[iPos] = m_mData[rIdxList[iPos]];
	}

	m_mData = mNewData;

	return true;
}

