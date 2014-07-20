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

// OGLFilterN3.h: Schnittstelle für die Klasse COGLFilterN3.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLFILTEREL2_H__INCLUDED_)
#define AFX_OGLFILTEREL2_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLMVFilterBase.h"

#define EL2M_AUTO	(ID_EL2GA << 16) + 0x0001
#define EL2M_BLADE	(ID_EL2GA << 16) + 0x0002
#define EL2M_VERSOR	(ID_EL2GA << 16) + 0x0003
#define EL2M_OPNS	(ID_EL2GA << 16) + 0x0004
#define EL2M_IPNS	(ID_EL2GA << 16) + 0x0005
#define EL2M_SHOW_IMAG	(ID_EL2GA << 16) + 0x0006
#define EL2M_HIDE_IMAG	(ID_EL2GA << 16) + 0x0007


class COGLFilterEl2 : public COGLMVFilterBase  
{

public:
	COGLFilterEl2();
	virtual ~COGLFilterEl2();

	virtual bool DrawMV(const MultiV<double>& vA, bool bAnalyzeOnly = false);
	virtual bool DrawMV(const MultiV<float>& vA, bool bAnalyzeOnly = false);
	virtual bool SetMode(int iMode);
	virtual bool ResetModes();

	bool DrawMVInfo(CMVInfo<float>& rInfo);

	void EnableOPNSMode(bool bOPNS = true) { m_bDrawOPNS = bOPNS; }
	bool IsOPNSEnabled() { return m_bDrawOPNS; }

protected:
	bool m_bDrawOPNS;		// Draw Outer Product Nullspace of blades
	bool m_bMVIsVersor;		// Interpret Multivectors as versors
	bool m_bMVIsBlade;		// Interpret Multivectors as blade
};

#endif // !defined(AFX_OGLFILTERN3_H__9301AEE9_2C49_4AFC_8E87_DE0B12AAE99A__INCLUDED_)
