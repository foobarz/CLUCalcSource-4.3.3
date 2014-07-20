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







//// IMPORTANT IMPORTANT IMPORTANT
/*
	Never write this:

		m_VexList.AddVex(m_VexList[0]);

	since internally the memory of m_VexList is enlarged, such that m_VexList[0]
	may not be a valid reference anymore!!!
*/






// OGLVertexList.cpp: Implementierung der Klasse COGLVertexList.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLVertexList.h"


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLVertexList::COGLVertexList()
{

}

COGLVertexList::~COGLVertexList()
{

}

COGLVertexList::COGLVertexList(const COGLVertexList& rVexList)
{
	*this = rVexList;
}

//////////////////////////////////////////////////////////////////////
/// Operator =

COGLVertexList& COGLVertexList::operator= (const COGLVertexList& rVexList)
{
	COGLBaseElement::operator=(rVexList);

	m_eMode = rVexList.m_eMode;
	m_mVexList = rVexList.m_mVexList;
	m_mTexList = rVexList.m_mTexList;
	m_mNormalList = rVexList.m_mNormalList;
	m_mColList = rVexList.m_mColList;
	m_mIdxList = rVexList.m_mIdxList;

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Add Vertex

bool COGLVertexList::AddVex(const COGLVertex& rVex)
{
	if (!(m_mVexList++)) return false;

	m_mVexList.Last() = rVex;
	m_mVexList.Last().Clamp();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add Vertex

bool COGLVertexList::AddVex(const float *m_pfVex)
{
	if (!(m_mVexList++)) return false;

	m_mVexList.Last() = m_pfVex;
	m_mVexList.Last().Clamp();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add Vertex

bool COGLVertexList::AddVex(float fX, float fY, float fZ)
{
	if (!(m_mVexList++)) return false;

	m_mVexList.Last().Set(fX, fY, fZ);
	m_mVexList.Last().Clamp();

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Add Texture Coordinates

bool COGLVertexList::AddTex(const COGLVertex& rVex)
{
	if (!(m_mTexList++)) return false;

	m_mTexList.Last() = rVex;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add Texture Coordinates

bool COGLVertexList::AddTex(const float *m_pfVex)
{
	if (!(m_mTexList++)) return false;

	m_mTexList.Last() = m_pfVex;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add Texture Coordinates

bool COGLVertexList::AddTex(float fX, float fY, float fZ)
{
	if (!(m_mTexList++)) return false;

	m_mTexList.Last().Set(fX, fY, fZ);

	return true;
}



//////////////////////////////////////////////////////////////////////
/// Add Normal

bool COGLVertexList::AddNormal(const COGLVertex& rNormal)
{
	if (!(m_mNormalList++)) return false;

	m_mNormalList.Last() = rNormal;
	m_mNormalList.Last().Norm();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add Normal

bool COGLVertexList::AddNormal(const float *m_pfNormal)
{
	if (!(m_mNormalList++)) return false;

	m_mNormalList.Last() = m_pfNormal;
	m_mNormalList.Last().Norm();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add Normal

bool COGLVertexList::AddNormal(float fX, float fY, float fZ)
{
	if (!(m_mNormalList++)) return false;

	m_mNormalList.Last().Set(fX, fY, fZ);
	m_mNormalList.Last().Norm();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add Color 

bool COGLVertexList::AddCol(const COGLColor& rCol)
{
	if (!(m_mColList++)) return false;

	m_mColList.Last() = rCol.Data();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add Color 

bool COGLVertexList::AddCol(const float *pfCol)
{
	if (!(m_mColList++)) return false;

	m_mColList.Last() = pfCol;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add Color 

bool COGLVertexList::AddCol(float fR, float fG, float fB, float fA)
{
	if (!(m_mColList++)) return false;

	TColor &rCol = m_mColList.Last();
	
	rCol[0] = fR;
	rCol[1] = fG;
	rCol[2] = fB;
	rCol[3] = fA;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Index List

bool COGLVertexList::SetIdxList(int iNo, int *pIdx)
{
	if (iNo < 0 || !m_mIdxList.Set(iNo))
		return false;

	memcpy(m_mIdxList.Data(), pIdx, iNo*sizeof(int));

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Index List

bool COGLVertexList::SetIdxList(Mem<int>& mIdx)
{
	m_mIdxList = mIdx;

	return true;
}

//////////////////////////////////////////////////////////////////////
// Invert Normals
// Invert direction of all normals for backface of objects

void COGLVertexList::InvertNormals(float fFac)
{
	int i, n = m_mNormalList.Count();

	for(i=0;i<n;i++)
	{
		m_mNormalList[i] = -m_mNormalList[i] * fFac;
	}
}

//////////////////////////////////////////////////////////////////////
/// Apply
bool COGLVertexList::Apply(int iMode, void *pvData)
{
	int n, iVexCount = m_mVexList.Count();
	int iColCount, iNormCount, iIdxCount, iTexCount;
	bool bUseCol = false, bUseNorm = false, bUseIdx = false, bUseTex = false;

	n = iVexCount;

	if ((iTexCount = m_mTexList.Count()) != 0)
		bUseTex = true;

	if ((iColCount = m_mColList.Count()) != 0)
		bUseCol = true;

	if ((iNormCount = m_mNormalList.Count()) != 0)
		bUseNorm = true;

	if ((iIdxCount = m_mIdxList.Count()) != 0)
	{
		bUseIdx = true;
		n = iIdxCount;
	}

/////
/*
	FILE *pFile;
	if ((pFile = fopen("Test.dat", "w")))
	{
		fprintf(pFile, "Vex: %d, Cols: %d, Norms: %d, Idxs: %d\n", 
			iVexCount, iColCount, iNormCount, iIdxCount);
*/
//////

	if (m_eMode == GL_POINTS || m_eMode == GL_LINE_STRIP)
	{
		glDisable( GL_CULL_FACE		);
	}

	if (bUseTex)
	{
		glEnable(GL_TEXTURE_2D);
	}

#ifdef CLUDRAW_USE_VEX_ARRAY

	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer(3, GL_FLOAT, sizeof(COGLVertex), (const GLvoid*) m_mVexList.Data());

	if (bUseCol)
	{
		glEnableClientState( GL_COLOR_ARRAY );
		glColorPointer(4, GL_FLOAT, sizeof(TColor), (const GLvoid*) m_mColList.Data());
	}
	else
	{
		glDisableClientState( GL_COLOR_ARRAY );
	}

	if (bUseNorm)
	{
		glEnableClientState( GL_NORMAL_ARRAY );
		glNormalPointer(GL_FLOAT, sizeof(COGLVertex), (const GLvoid*) m_mNormalList.Data());
	}
	else
	{
		glDisableClientState( GL_NORMAL_ARRAY );
	}

	if (bUseTex)
	{
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer(2, GL_FLOAT, sizeof(COGLVertex), (const GLvoid*) m_mTexList.Data());
	}
	else
	{
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	}

	if (m_mIdxList.Count())
	  glDrawElements(m_eMode, m_mIdxList.Count(), GL_UNSIGNED_INT, m_mIdxList.Data());
	else
	  glDrawArrays(m_eMode, 0, m_mVexList.Count());

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );

#else

	int i, iIdx;

	glBegin(m_eMode);
	for(i=0;i<n;i++)
	{
		if (bUseIdx)
			iIdx = m_mIdxList[i];
		else
			iIdx = i;

		if (bUseTex)
			glTexCoord3fv(m_mTexList[iIdx]);

		if (bUseCol)
			glColor4fv(m_mColList[iIdx].Data());

		if (bUseNorm)
		{
			glNormal3fv(m_mNormalList[iIdx]);
//			fprintf(pFile, "(%f %f %f)\n", m_mNormalList[iIdx][0], 
//				m_mNormalList[iIdx][1], m_mNormalList[iIdx][2]);
		}

		glVertex3fv(m_mVexList[iIdx]);
//		fprintf(pFile, "(%f %f %f)\n ", m_mVexList[iIdx][0], 
//			m_mVexList[iIdx][1], m_mVexList[iIdx][2]);

	}
	glEnd();

#endif	// CLUDRAW_USE_VEX_ARRAY
	if (bUseTex)
	{
		glDisable(GL_TEXTURE_2D);
	}

	if (m_eMode == GL_POINTS || m_eMode == GL_LINE_STRIP)
	{
		glEnable( GL_CULL_FACE		);
	}
///////
/*
		fclose(pFile);
	}
*/
////////
	return true;
}
