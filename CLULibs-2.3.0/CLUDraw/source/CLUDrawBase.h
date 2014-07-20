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


// cluDrawBase.h: Schnittstelle für die Klasse CCLUDrawBase.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLUDRAWBASE_H__6EB9C283_EE5B_11D5_BA34_00E07D8AFD6A__INCLUDED_)
#define AFX_CLUDRAWBASE_H__6EB9C283_EE5B_11D5_BA34_00E07D8AFD6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLColor.h"
#include "OGLLight.h"
#include "OGLMaterial.h"
#include "IOGLWinBase.h"

#include <vector>
#include <string>

using namespace std;

#define MRHLEN	10
#define CLUDRAWBASE_MAXLIGHTCOUNT 7

struct SPerspective
{
	GLfloat fAngle;
	GLfloat fAspect;
	GLfloat fNear;
	GLfloat fFar;

	void Set(GLfloat _fAngle, GLfloat _fAspect, GLfloat _fNear, GLfloat _fFar)
	{ fAngle = _fAngle; fAspect = _fAspect; fNear = _fNear; fFar = _fFar; }
};

struct SOrtho
{
	GLfloat fLeft;
	GLfloat fRight;
	GLfloat fBottom;
	GLfloat fTop;
	GLfloat fNear;
	GLfloat fFar;

	void Set(GLfloat _fLeft, GLfloat _fRight, GLfloat _fBottom, GLfloat _fTop, 
			 GLfloat _fNear, GLfloat _fFar)
	{ fLeft = _fLeft; fRight = _fRight; fBottom = _fBottom; fTop = _fTop;
	  fNear = _fNear; fFar = _fFar; }
};

struct SMousePos
{
	int iX, iY;
};

struct STransform
{
	GLfloat pfRot[3];
	GLfloat pfTrans[3];

	int iFlags;

	void Reset()
	{ for(int i=0;i<3;i++) { pfRot[i] = 0.0f; pfTrans[i] = 0.0f; } iFlags = 0; }

	bool HasRotChanged(int i)
	{ if (i >= 0 && i < 3) return ((iFlags & (1 << i)) ? true : false);
		else return false;
	}

	bool HasTransChanged(int i)
	{ if (i >= 0 && i < 3) return ((iFlags & (0x10 << i)) ? true : false);
		else return false;
	}

	void SetRotChanged(int i)
	{ if (i >= 0 && i < 3) iFlags |= (1 << i);	}

	void SetTransChanged(int i)
	{ if (i >= 0 && i < 3) iFlags |= (0x10 << i);	}

	void ResetTransChanged()
	{ iFlags &= 0x0F; }

	void ResetRotChanged()
	{ iFlags &= 0xF0; }
};

class CCLUDrawBase : public IOGLWinBase
{
public:
	CCLUDrawBase();
	virtual ~CCLUDrawBase(); 
	
	void SetPerspectiveAngle(GLfloat fAngle, bool bUpdateMatrices = true);
	void SetPerspective(GLfloat fAngle, GLfloat fNear, GLfloat fFar);
	void SetOrtho(GLfloat fLeft, GLfloat fRight, GLfloat fBottom, GLfloat fTop, 
			      GLfloat fNear, GLfloat fFar);
	
	void GetViewport(int &iX, int &iY, int &iW, int &iH);
	bool SetViewport(int iX, int iY, int iW, int iH);

	void AdjustRotAngle(float& fDeg);
	float DegToRad(float fDeg)
	{ return fDeg * m_fRadPerDeg; }

	void SetRotFac(float fRotFac) { m_fRotFac = fRotFac; }
	void SetTransFac(float fTransFac) { m_fTransFac = fTransFac; }

	float GetRotFac() { return m_fRotFac; }
	float GetTransFac() { return m_fTransFac; }

	bool SetRTMode(int iMode);
	int GetActRTMode() { return (m_bCtrlDown ? 0 : m_iRTMode); }

	bool IsAnimated() { return m_bIsAnimated; }
	bool Is2dView() { return m_b2dView; }

	void Enable2dView(bool bVal = true);
	void EnableLighting(bool bLight = true) { m_bUseLighting = bLight; }
	void EnableLocalRot(bool bVal = true) { m_bUseLocalRot = bVal; }

	void ResetTransformVars() { for(int i=1;i<10;i++) m_mTransform[i].Reset(); }

	COGLLight& GetStdLight() { return m_StdLight; }
	COGLLight& GetSurLight(int iNo)
	{
		if (iNo < 0 || iNo >= CLUDRAWBASE_MAXLIGHTCOUNT) return m_SurLight[0];
		else return m_SurLight[iNo];
	}

	bool EnableSurLightPosFixed(int iNo, bool bFixed = true)
	{
		if (iNo < 0 || iNo >= CLUDRAWBASE_MAXLIGHTCOUNT) return false;
		m_bSurLightPosFixed[iNo] = bFixed;
		return true;
	}

	bool IsSurLightPosFixed(int iNo)
	{
		if (iNo < 0 || iNo >= CLUDRAWBASE_MAXLIGHTCOUNT) return false;
		return m_bSurLightPosFixed[iNo];
	}

	virtual bool ResetLights(bool bFixation = true);

	// Tool Functions do nothing in standard implementation
	virtual bool GetToolValue(const char *pcName, double &rVal);
	virtual bool GetToolValue(const char *pcName, string &rVal);
	virtual bool AddToolSlider(const char *pcName, double dMin, double dMax, double dStep,
								double dVal, bool bFixed = false, const char *pcDesc = 0);
	virtual bool AddToolCounter(const char *pcName, double dMin, double dMax, double dStep,
								double dLStep, double dVal, bool bFixed = false, const char *pcDesc = 0);
	virtual bool AddToolInput(const char *pcName, double dMin, double dMax,
								double dVal, bool bFixed = false, const char *pcDesc = 0);
	virtual bool AddToolInputText(const char *pcName, bool bImmediateCallback, const char* pcInitText,
						  bool bFixed = false, const char *pcDesc = 0, bool bDoUpdate = true);
	virtual bool AddToolCheckBox(const char *pcName, bool bSet, bool bFixed = false, 
								const char *pcDesc = 0);
	virtual bool AddToolButton(const char *pcName, bool bFixed = false, 
								const char *pcDesc = 0);	
	virtual bool AddToolChoice(const char *pcName, vector<string>& vecData, const char* pcCoice,
						bool bFixed = false, const char *pcDesc = 0, bool bDoUpdate = true);

	// Info Window Functions
	virtual bool SetInfoText(const char *pcText);
	virtual bool SetInfoWidth(double dWidth);

	virtual bool SetOGLSize(double dWidth, double dHeight);

	virtual bool SetMouseMode(int iMode);
	virtual bool GetMouseMode(int &iMode);

	virtual bool SetAnimationTimeStep(int iTimeStep);

	virtual bool Init();

	// the following virtual functions are defined in the window system dependent derived classes
	virtual bool BeginDraw();
	virtual void Display();
	virtual bool EndDraw(); 

	virtual COGLColor GetBGColor();
	virtual void SetBGColor(COGLColor &rBGCol);
	virtual void SetBoxColor(COGLColor &rCol, COGLColor &rXCol, COGLColor &rYCol, COGLColor &rZCol);

	virtual unsigned char CLU_LEFT_BUTTON() {return 99;};
	virtual unsigned char CLU_RIGHT_BUTTON() {return 99;};	
	virtual unsigned char CLU_DOWN() {return 99;};
	virtual unsigned char CLU_UP() {return 99;};		

	virtual bool SaveScreen2PGM(const char *pcFilename);	
	virtual bool SaveScreen2BMP(const char *pcFilename);	

protected:
	virtual void Reshape(int iWidth, int iHeight);
	virtual void Mouse(int iButton, int iState, int iX, int iY);
	virtual void Key(unsigned char cKey, int iX, int iY);
	virtual void ActiveMouseMove(int iX, int iY);
	virtual void Menu(int iIdx);
	virtual void IdleFunc() {}
	 
	virtual void GetModifiers() {}
	virtual bool StartAnimation(unsigned int uiTime) { return true; }
	virtual bool StopAnimation() { return true; }
	
	virtual bool TimedFunc();
	virtual bool AtAnimStart() { return true; }
	virtual bool AtAnimEnd() { return true; }

	// returns milliseconds since last call of idlefunc
	float timeElapsed() {return m_fTimeStep;} 
	void UpdateMatrices();

	void DrawBase();
	void DrawBox(float fSize = 4.0f);

	MultiV<float> GetMouseRotHistSum() 
	{ MultiV<float> vSum = m_pvMouseRotHist[0]; for(int i=1;i<MRHLEN;i++) vSum += m_pvMouseRotHist[i]; return vSum; }

	void ResetMouseRotHist() { for(int i=0;i<MRHLEN;i++) m_pvMouseRotHist[i] = 0; m_iMRHPos = 0; m_bMRHChanged = true; }
	bool HasMouseRotHistChanged() { bool bVal = m_bMRHChanged; m_bMRHChanged = false; return bVal; }

protected:
	int m_iPosX, m_iPosY, m_iSizeX, m_iSizeY;	
	bool m_bMouseLDown;
	bool m_bMouseRDown;
	bool m_bShiftDown;
	bool m_bCtrlDown;
	bool m_bAltDown;
	bool m_b2dView;
	

	float m_fTimeStep;	// Time past since last animation call
	float m_fTotalTime;	// Total time past since start of animation
	unsigned int m_uAnimateTimeStep;
	bool m_bIsAnimated;	
	int m_iAnimRotType;
	
	SOrtho m_sOrtho;
	SPerspective m_sPer;

	bool m_bUseLighting;
	bool m_bUseLocalRot;

	SMousePos m_sMousePos;

	int m_iRTMode;
	float m_fFrameAngle;
	Mem<STransform> m_mTransform;

	// Background Color
	COGLColor m_BGCol;

	// Color for Box
	COGLColor m_BoxCol, m_BoxXCol, m_BoxYCol, m_BoxZCol;

	// Global Colors
	COGLColor Red, Green, Blue, Cyan, Magenta, Yellow;
	COGLColor DRed, DGreen, DBlue, DCyan, DMagenta, DYellow;
	COGLColor MRed, MGreen, MBlue, MCyan, MMagenta, MYellow;
	COGLColor Orange, DOrange, MOrange, White, Black, DGray, MGray;

	// if bool value is true, then the position of the light is relative to the view frame.
	// otherwise relative to the model-view frame.
	bool m_bSurLightPosFixed[CLUDRAWBASE_MAXLIGHTCOUNT];
	COGLLight m_StdLight, m_SurLight[CLUDRAWBASE_MAXLIGHTCOUNT];
	COGLMaterial m_StdMaterial;

	float m_fPi, m_fRadPerDeg;

	float m_fRotFac, m_fTransFac;
	float m_fOrthoFac, m_fOrthoZoomFac;

	int m_iMainMenu;	

	// 3D Space for rotor evaluation
	E3GA<float> m_E3Base;

	// Main Rotor. Gives rotation of space.
	MultiV<float> m_vRMain;
	MultiV<float> m_vU, m_vAxis;
	MultiV<float> m_pvMouseRotHist[MRHLEN];
	int m_iMRHPos;
	bool m_bMRHChanged;

	int m_iSaveScreenNo;
	int m_iAnimPause;
};

#endif // !defined(AFX_CLUDRAWBASE_H__6EB9C283_EE5B_11D5_BA34_00E07D8AFD6A__INCLUDED_)
