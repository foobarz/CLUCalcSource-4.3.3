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


// OGLBaseTransform.h: Schnittstelle für die Klasse COGLBaseTransform.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLBASETRANSFORM_H__B9CF66CC_ABAC_40D4_93ED_4B8BBA70D662__INCLUDED_)
#define AFX_OGLBASETRANSFORM_H__B9CF66CC_ABAC_40D4_93ED_4B8BBA70D662__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLBaseElement.h"

class COGLBaseTransform : public COGLBaseElement
{
public:
	COGLBaseTransform();
	virtual ~COGLBaseTransform();

	virtual bool Apply(int iMode = 0, void *pvData = 0);

	/// ALL ANGLES ARE GIVEN IN DEGS

	// Set Rotation by Euler Angles
	bool SetRotation(float fRadAngleX, float fRadAngleY, float fRadAngleZ);

	// Set Rotation by angle and axis
	bool SetRotation(float fRadAngle, float fX, float fY, float fZ);

	// Set Translation
	bool SetTranslation(float fX, float fY, float fZ);
	bool AddToTranslation(float fX, float fY, float fZ)
	{ return SetTranslation(m_fTX+fX, m_fTY+fY, m_fTZ+fZ); }

	// Combined Versions
	bool SetTransformation(float RAngle, float RX, float RY, float RZ, float TX, float TY, float TZ)
	{ SetRotation(RAngle, RX, RY, RZ); SetTranslation(TX, TY, TZ); return true; }

	bool SetTransformation(float RXAngle, float RYAngle, float RZAngle, float TX, float TY, float TZ)
	{ SetRotation(RXAngle, RYAngle, RZAngle); SetTranslation(TX, TY, TZ); return true; }

	bool SetTransformation(float *pfTrans, bool bEuler = true)
	{ if (bEuler) return SetTransformation(pfTrans[0], pfTrans[1], pfTrans[2], pfTrans[3], pfTrans[4], pfTrans[5]); 
	  else return SetTransformation(pfTrans[0], pfTrans[1], pfTrans[2], pfTrans[3], pfTrans[4], pfTrans[5], pfTrans[6]);
	}

protected:
	float m_fRAngle, m_fRX, m_fRY, m_fRZ, m_fTX, m_fTY, m_fTZ;

	static double m_dPi, m_dRadPerDeg;
};

#endif // !defined(AFX_OGLBASETRANSFORM_H__B9CF66CC_ABAC_40D4_93ED_4B8BBA70D662__INCLUDED_)
