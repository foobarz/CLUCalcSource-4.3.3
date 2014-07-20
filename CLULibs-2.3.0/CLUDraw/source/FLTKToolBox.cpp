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

// OGLWinFLTK.cpp: Implementierung der Klasse CFLTKToolBox.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#ifdef CLUDRAW_FLTK
#include "FLTKToolBox.h"


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CFLTKToolBox::CFLTKToolBox()
{
	m_pVPackBox = 0;
	m_pScrollBox = 0;
	m_pToolCallback = 0;
	m_pToolCallbackData = 0;
	m_iCurWidth = 0;
	m_bToolsChanged = false;

	m_iToolHeight = 40;
	m_iToolWidth = 190;
	m_iToolWBorder = 3;
}

CFLTKToolBox::~CFLTKToolBox()
{
	Destroy();
}


//////////////////////////////////////////////////////////////////////
// Create the Tool Box

bool CFLTKToolBox::Create(int iPosX, int iPosY, int iSizeX, int iSizeY, 
							Fl_Callback *pCallback, void *pData, int iToolHeight)
{
	if (iToolHeight > 0)
		m_iToolHeight = iToolHeight;

	m_pToolCallback = pCallback;
	m_pToolCallbackData = pData;

	// Create the outer scroll box
	m_pScrollBox = new Fl_Scroll(iPosX, iPosY, iSizeX, iSizeY);
	m_pScrollBox->box(FL_DOWN_BOX);
	m_pScrollBox->align(FL_ALIGN_CLIP);
	m_pScrollBox->type(Fl_Scroll::VERTICAL);
	m_pScrollBox->begin();

	// Create the vertical Pack Box
	// The tools are placed in horizontal packs which themselves
	// are stacked in this vertical stack box
	m_pVPackBox = new Fl_Pack(5, iPosY+2, iSizeX, iToolHeight);
	m_pVPackBox->type(FL_VERTICAL);
	m_pVPackBox->spacing(2);
	m_pVPackBox->begin();

	m_pVPackBox->end();
	m_pScrollBox->end();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Detach window

bool CFLTKToolBox::Destroy()
{
	if (m_pScrollBox)
		delete m_pScrollBox;

	return true;
}

//////////////////////////////////////////////////////////////////////
// Get Upper Left corner of scroll box enclosing tools

bool CFLTKToolBox::GetPosition(int &iX, int &iY)
{
	if (!m_pScrollBox)
	{
		iX = iY = 0;
		return false;
	}
	
	iX = m_pScrollBox->x();
	iY = m_pScrollBox->y();
	
	return true;
}
	
//////////////////////////////////////////////////////////////////////
// Get Size of scroll box enclosing tools

bool CFLTKToolBox::GetSize(int &iWidth, int &iHeight)
{
	if (!m_pScrollBox)
	{
		iWidth = iHeight = 0;
		return false;
	}
	
	iWidth = m_pScrollBox->w();
	iHeight = m_pScrollBox->h();
	
	return true;
}

//////////////////////////////////////////////////////////////////////
// Update Tool Box Packs
// This first resets the tool box and then walks through map
// m_mapTool to redistribute all tools

bool CFLTKToolBox::Update()
{
	// Get current width of scroll box.
	int iSizeX = m_pScrollBox->w();

	// If neither the tools changed nor the size of the window,
	// then do not do any updating.
	if (!m_bToolsChanged && m_iCurWidth == iSizeX)
		return true;

	// Reset flags
	m_iCurWidth = iSizeX;
	m_bToolsChanged = false;

	// Reset all packs
	ResetPacks();

	// Now build packs up again from map of tools

	int iTotalToolWidth = 0;
	int iToolWidth;
	int iHPackCount = 0;
	TToolMap::iterator it_El, it_End;

	// Check whether there are tool at all.
	if (m_mapTool.size() == 0)
	{
		// Set Height of vertical pack to appropriate value
		m_pVPackBox->size(iSizeX, m_iToolHeight);
		m_pVPackBox->redraw();
		m_pScrollBox->redraw();
		return true;
	}

	// Create first horizontal pack
	Fl_Pack *pHPack;

	m_pVPackBox->init_sizes();
	m_pVPackBox->begin();
	pHPack = new Fl_Pack(0, 0, iSizeX, m_iToolHeight);
	pHPack->type(FL_HORIZONTAL);
	pHPack->spacing(2);
	pHPack->begin();
	pHPack->end();
	pHPack->init_sizes();
	m_pVPackBox->end();
	m_listHPackBox.push_back(pHPack);
	// Now there is one horizontal pack
	iHPackCount = 1;

	// Iterate over all tools
	it_End = m_mapTool.end();

	for (it_El = m_mapTool.begin();
		 it_El != it_End;
		 ++it_El)
	{
		STool &rTool = it_El->second;

		// Get Width of tool
		iToolWidth = rTool.pBox->w();

		// If at least one tool already exists in current horizontal pack,
		// and adding a new tool to this horizontal pack would extend over
		// the width of the scroll window, then add a new horizontal pack.
		if (iTotalToolWidth > 0 && iTotalToolWidth + iToolWidth + 30 > iSizeX)
		{
			pHPack->redraw();

			// Add new horizontal pack
			m_pVPackBox->init_sizes();
			m_pVPackBox->begin();
			pHPack = new Fl_Pack(0, 0, iSizeX, m_iToolHeight);
			pHPack->type(FL_HORIZONTAL);
			pHPack->spacing(2);
			pHPack->begin();
			pHPack->end();
			pHPack->init_sizes();
			m_pVPackBox->end();
			m_listHPackBox.push_back(pHPack);
			// Now there is one horizontal pack more
			++iHPackCount;
			// Reset total tool width
			iTotalToolWidth = 0;
		}

		// Add tool to horizontal pack
		pHPack->add(*rTool.pBox);

		// Increase total tool width by width of this tool
		iTotalToolWidth += iToolWidth;
	}

	pHPack->redraw();

	// Set Height of vertical pack to appropriate value
	m_pVPackBox->size(iSizeX, iHPackCount * m_iToolHeight);
	m_pVPackBox->redraw();
	m_pScrollBox->redraw();

	return true;
}

//////////////////////////////////////////////////////////////////////
// Reset Packs
// Remove tools from packs and remove horizonzal packs from vertical pack

bool CFLTKToolBox::ResetPacks()
{
	TPackList::iterator it_El, it_End;

	it_End = m_listHPackBox.end();
	for (it_El = m_listHPackBox.begin(); 
		 it_El != it_End;
		 ++it_El)
	{
		Fl_Pack *pPack = *it_El;
		if (!pPack)
			continue;

		// Remove tools from horizontal pack
		int iTool, iToolCount = pPack->children();
		for (iTool = 0; iTool < iToolCount; iTool++)
		{
			// Always delete child at pos 0, since
			// the list is reduced after a remove operation.
			pPack->remove(*pPack->child(0));
		}

		// Remove horizontal pack from vertical pack
		m_pVPackBox->remove(*pPack);

		// Delete horizontal pack
		delete pPack;
	}

	// Empty vector of horizontal packs
	m_listHPackBox.erase(m_listHPackBox.begin(), it_End);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Reset Tool bInUse Flags

void CFLTKToolBox::ResetToolInUseFlags()
{
	TToolMap::iterator it_El, it_End;

	it_End = m_mapTool.end();

	for (it_El = m_mapTool.begin();
		 it_El != it_End;
		 ++it_El)
	{
		it_El->second.bInUse = false;
	}
}

//////////////////////////////////////////////////////////////////////
/// Reset Tool bInUse Flags

void CFLTKToolBox::PruneToolMap(bool bKeepFixedTools, bool bRedraw)
{
	TToolMap::iterator it_El, it_End;

	it_End = m_mapTool.end();

	it_El = m_mapTool.begin();
	while (it_El != it_End)
	{
		if (it_El->second.bInUse == false &&
			(it_El->second.bFixed == false || 
			 (it_El->second.bFixed == true && bKeepFixedTools == false)))
		{
			RemoveTool(it_El->second.sName.c_str(), false);
			it_El = m_mapTool.begin();
			m_bToolsChanged = true;
		}
		else
		{
			++it_El;
		}
	}

	// Update packs
	if (bRedraw)
		Update();
}

//////////////////////////////////////////////////////////////////////
/// Remove Tool

bool CFLTKToolBox::RemoveTool(const char *pcName, bool bRedraw)
{
	TToolMap::iterator it_El;
	TPackList::iterator it_ListEl, it_End;

	if ((it_El = m_mapTool.find(pcName)) == m_mapTool.end())
		return false;

	STool &rTool = it_El->second;

	// Look through horizontal packs and remove tool
	it_End = m_listHPackBox.end();
	for (it_ListEl = m_listHPackBox.begin();
		 it_ListEl != it_End;
		 ++it_ListEl)
	{
		(*it_ListEl)->remove(rTool.pBox);
	}
	delete rTool.pBox;

	m_mapTool.erase(it_El);
	m_bToolsChanged = true;

	// Update the pack
	if (bRedraw)
		Update();

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Get Tool

bool CFLTKToolBox::GetTool(const char *pcName, STool &rTool)
{
	TToolMap::iterator it_El;

	if ((it_El = m_mapTool.find(pcName)) == m_mapTool.end())
		return false;

	rTool = it_El->second;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Get the double value of a tool
/// returns false if the tool has no double value 

bool CFLTKToolBox::GetToolValue(const char *pcName, double &rVal)
{
	TToolMap::iterator it_El;

	if ((it_El = m_mapTool.find(pcName)) == m_mapTool.end())
		return false;

	STool &rTool = it_El->second;
	
	if (rTool.eType == TOOL_SLIDER)
	{
		rVal = ((Fl_Value_Slider *) rTool.pWidget)->value();
		rTool.bInUse = true;
	}
	else if (rTool.eType == TOOL_COUNTER)
	{
		rVal = ((Fl_Counter *) rTool.pWidget)->value();
		rTool.bInUse = true;
	}
	else if (rTool.eType == TOOL_INPUT)
	{
		rVal = ((Fl_Value_Input *) rTool.pWidget)->value();
		rTool.bInUse = true;
	}
	else if (rTool.eType == TOOL_CHECKBOX)
	{
		rVal = ((Fl_Button *) rTool.pWidget)->value();
		rTool.bInUse = true;
	}
	else if (rTool.eType == TOOL_BUTTON)
	{
		rVal = 0;
		rTool.bInUse = true;
	}
	else if (rTool.eType == TOOL_CHOICE)
	{
		rVal = ((Fl_Choice *) rTool.pWidget)->value() + 1;
		rTool.bInUse = true;
	}
	else
	{
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Get the string value of a tool
/// returns false if the tool has no string value 

bool CFLTKToolBox::GetToolValue(const char *pcName, string &rVal)
{
	TToolMap::iterator it_El;

	if ((it_El = m_mapTool.find(pcName)) == m_mapTool.end())
		return false;

	STool &rTool = it_El->second;
	
	if (rTool.eType == TOOL_TEXT)
	{
		rVal = ((Fl_Input *) rTool.pWidget)->value();
		rTool.bInUse = true;
	}
	else
	{
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Add a tool to the tool window

bool CFLTKToolBox::AddToolSlider(const char *pcName, 
								double dMin, double dMax, double dStep, double dVal,
								bool bFixed, const char *pcDesc, bool bDoUpdate)
{
	if (!m_pVPackBox)
		return false;

	Fl_Group *pBox;
	Fl_Value_Slider *pSlider;
	TToolMap::iterator it_El;

	// Check whether slider already exists
	if ((it_El = m_mapTool.find(pcName)) != m_mapTool.end())
	{
		// If already a different tool with the same name exists
		// then return with false.
		if (it_El->second.eType != TOOL_SLIDER)
			return false;

		// don't create anew if yes
		Fl_Value_Slider *pSlider = (Fl_Value_Slider *) it_El->second.pWidget;

		// Get reference to tool data
		STool &rTool = it_El->second;

		if (dVal != rTool.dInitVal || dMin != rTool.dInitMin ||
			dMax != rTool.dInitMax || dStep != rTool.dInitStep)
		{
			rTool.dInitVal = dVal;
			rTool.dInitMin = dMin;
			rTool.dInitMax = dMax;
			rTool.dInitStep = dStep;

			pSlider->range(dMin, dMax);
			pSlider->step(dStep);
			pSlider->value(pSlider->round(dVal));
			pSlider->redraw();
		}

		it_El->second.bInUse = true;
		return true;
	}

	STool &rTool = m_mapTool[pcName];

	if (!(pBox = new Fl_Group(0, 0, m_iToolWidth, 30)))
		return false;
	pBox->box(FL_DOWN_BOX);
	pBox->begin();

	rTool.sName = pcName;

	if (!(pSlider = new Fl_Value_Slider(3, 12, m_iToolWidth - 2*m_iToolWBorder, 
											15, rTool.sName.c_str())))
		return false;

	rTool.eType = TOOL_SLIDER;
	rTool.pWidget = (Fl_Widget *) pSlider;
	rTool.pBox = pBox;
	rTool.bInUse = true;
	rTool.bFixed = bFixed;
	rTool.dInitVal = dVal;
	rTool.dInitMin = dMin;
	rTool.dInitMax = dMax;
	rTool.dInitStep = dStep;

	if (pcDesc)
		rTool.sDesc = pcDesc;

	pSlider->type(FL_HOR_NICE_SLIDER);
	pSlider->box(FL_FLAT_BOX);
	pSlider->slider_size(0.1f);
	pSlider->labelsize(12);
	pSlider->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
	pSlider->color(0x9F9F9F00);
	pSlider->selection_color(1);
	pSlider->range(dMin, dMax);
	pSlider->step(dStep);
	pSlider->value(pSlider->round(dVal));
	pSlider->redraw();

	if (pcDesc)
		pSlider->tooltip(rTool.sDesc.c_str());

	pSlider->callback(m_pToolCallback, m_pToolCallbackData);

	pBox->end();

	m_bToolsChanged = true;

	// Now update the packs
	if (bDoUpdate)
		Update();

	return true;
}


//////////////////////////////////////////////////////////////////////
// Add a tool to the tool window

bool CFLTKToolBox::AddToolCounter(const char *pcName, 
								double dMin, double dMax, double dStep, double dLStep,
								double dVal, bool bFixed, const char *pcDesc, bool bDoUpdate)
{
	if (!m_pVPackBox)
		return false;

	Fl_Group *pBox;
	Fl_Counter *pCounter;
	TToolMap::iterator it_El;

	// Check whether slider already exists
	if ((it_El = m_mapTool.find(pcName)) != m_mapTool.end())
	{
		// If already a different tool with the same name exists
		// then return with false.
		if (it_El->second.eType != TOOL_COUNTER)
			return false;

		// don't create anew if yes
		Fl_Counter *pCounter = (Fl_Counter *) it_El->second.pWidget;

		// Get reference to tool data
		STool &rTool = it_El->second;

		if (dVal != rTool.dInitVal || dMin != rTool.dInitMin ||
			dMax != rTool.dInitMax || dStep != rTool.dInitStep ||
			dLStep != rTool.dInitLStep)
		{
			rTool.dInitVal = dVal;
			rTool.dInitMin = dMin;
			rTool.dInitMax = dMax;
			rTool.dInitStep = dStep;
			rTool.dInitLStep = dLStep;

			pCounter->range(dMin, dMax);
			pCounter->step(dStep);
			pCounter->lstep(dLStep);
			pCounter->value(pCounter->round(dVal));
			pCounter->redraw();
		}

		it_El->second.bInUse = true;
		return true;
	}

	STool &rTool = m_mapTool[pcName];

	if (!(pBox = new Fl_Group(0, 0, m_iToolWidth, 30)))
		return false;
	pBox->box(FL_DOWN_BOX);
	pBox->begin();

	rTool.sName = pcName;

	if (!(pCounter = new Fl_Counter(3, 12, m_iToolWidth - 2*m_iToolWBorder, 
										15, rTool.sName.c_str())))
		return false;

	rTool.eType = TOOL_COUNTER;
	rTool.pWidget = (Fl_Widget *) pCounter;
	rTool.pBox = pBox;
	rTool.bInUse = true;
	rTool.bFixed = bFixed;
	rTool.dInitVal = dVal;
	rTool.dInitMin = dMin;
	rTool.dInitMax = dMax;
	rTool.dInitStep = dStep;
	rTool.dInitLStep = dLStep;

	if (pcDesc)
		rTool.sDesc = pcDesc;

	pCounter->type(FL_NORMAL_COUNTER);
	pCounter->box(FL_FLAT_BOX);
	pCounter->labelsize(12);
	pCounter->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
	pCounter->color(0x9F9F9F00);
	pCounter->selection_color(1);
	pCounter->range(dMin, dMax);
	pCounter->step(dStep);
	pCounter->lstep(dLStep);
	pCounter->value(pCounter->round(dVal));
	pCounter->redraw();

	if (pcDesc)
		pCounter->tooltip(rTool.sDesc.c_str());

	pCounter->callback(m_pToolCallback, m_pToolCallbackData);

	pBox->end();

	m_bToolsChanged = true;

	// Now update the packs
	if (bDoUpdate)
		Update();

	return true;
}



//////////////////////////////////////////////////////////////////////
// Add a tool to the tool window

bool CFLTKToolBox::AddToolInput(const char *pcName, 
								double dMin, double dMax, double dVal,
								bool bFixed, const char *pcDesc, bool bDoUpdate)
{
	if (!m_pVPackBox)
		return false;

	Fl_Group *pBox;
	Fl_Value_Input *pInput;
	TToolMap::iterator it_El;

	// Check whether slider already exists
	if ((it_El = m_mapTool.find(pcName)) != m_mapTool.end())
	{
		// If already a different tool with the same name exists
		// then return with false.
		if (it_El->second.eType != TOOL_INPUT)
			return false;

		// Get reference to tool data
		STool &rTool = it_El->second;

		// don't create anew if yes
		Fl_Value_Input *pInput = (Fl_Value_Input *) it_El->second.pWidget;

		if (dVal != rTool.dInitVal || dMin != rTool.dInitMin ||
			dMax != rTool.dInitMax)
		{
			rTool.dInitVal = dVal;
			rTool.dInitMin = dMin;
			rTool.dInitMax = dMax;

			pInput->range(dMin, dMax);
			pInput->value(pInput->clamp(pInput->value()));
			pInput->redraw();
		}

		it_El->second.bInUse = true;
		return true;
	}

	STool &rTool = m_mapTool[pcName];

	if (!(pBox = new Fl_Group(0, 0, m_iToolWidth, 30)))
		return false;
	pBox->box(FL_DOWN_BOX);
	pBox->begin();

	rTool.sName = pcName;

	if (!(pInput = new Fl_Value_Input(3, 12, m_iToolWidth-2*m_iToolWBorder, 15, rTool.sName.c_str())))
		return false;

	rTool.eType = TOOL_INPUT;
	rTool.pWidget = (Fl_Widget *) pInput;
	rTool.pBox = pBox;
	rTool.bInUse = true;
	rTool.bFixed = bFixed;
	rTool.dInitMin = dMin;
	rTool.dInitMax = dMax;
	rTool.dInitVal = dVal;

	if (pcDesc)
		rTool.sDesc = pcDesc;

	pInput->box(FL_FLAT_BOX);
	pInput->labelsize(12);
	pInput->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
	//pInput->color(0x9F9F9F00);
	pInput->range(dMin, dMax);
	pInput->value(pInput->round(dVal));
	pInput->redraw();

	if (pcDesc)
		pInput->tooltip(rTool.sDesc.c_str());

	pInput->when(FL_WHEN_ENTER_KEY);
	pInput->callback(m_pToolCallback, m_pToolCallbackData);

	pBox->end();

	m_bToolsChanged = true;

	// Now update the packs
	if (bDoUpdate)
		Update();

	return true;
}


//////////////////////////////////////////////////////////////////////
// Add a tool to the tool window

bool CFLTKToolBox::AddToolInputText(const char *pcName, 
								bool bImmediateCallback, 
								const char* pcInitText,
								bool bFixed, const char *pcDesc, bool bDoUpdate)
{
	if (!m_pVPackBox)
		return false;

	Fl_Group *pBox;
	CInput *pInput;
	TToolMap::iterator it_El;

	// Check whether slider already exists
	if ((it_El = m_mapTool.find(pcName)) != m_mapTool.end())
	{
		// If already a different tool with the same name exists
		// then return with false.
		if (it_El->second.eType != TOOL_TEXT)
			return false;

		// Get reference to tool data
		STool &rTool = it_El->second;

		// don't create anew if yes
		CInput *pInput = (CInput *) it_El->second.pWidget;

		if (rTool.sInitVal != pcInitText || 
			rTool.bInitVal != bImmediateCallback)
		{
			rTool.sInitVal = pcInitText;
			rTool.bInitVal = bImmediateCallback;

			pInput->value(pcInitText);
			if (bImmediateCallback)
				pInput->when(FL_WHEN_CHANGED);
			else
				pInput->when(FL_WHEN_ENTER_KEY);

			pInput->redraw();
		}

		it_El->second.bInUse = true;
		return true;
	}

	STool &rTool = m_mapTool[pcName];

	if (!(pBox = new Fl_Group(0, 0, m_iToolWidth, 30)))
		return false;
	pBox->box(FL_DOWN_BOX);
	pBox->begin();

	rTool.sName = pcName;

	if (!(pInput = new CInput(3, 12, m_iToolWidth-2*m_iToolWBorder, 15, rTool.sName.c_str())))
		return false;

	rTool.eType = TOOL_TEXT;
	rTool.pWidget = (Fl_Widget *) pInput;
	rTool.pBox = pBox;
	rTool.bInUse = true;
	rTool.bFixed = bFixed;
	rTool.sInitVal = pcInitText;
	rTool.bInitVal = bImmediateCallback;

	if (pcDesc)
		rTool.sDesc = pcDesc;

	pInput->box(FL_FLAT_BOX);
	pInput->labelsize(12);
	pInput->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
	//pInput->color(0x9F9F9F00);
	pInput->value(pcInitText);
	pInput->redraw();

	if (pcDesc)
		pInput->tooltip(rTool.sDesc.c_str());

	if (bImmediateCallback)
		pInput->when(FL_WHEN_CHANGED);
	else
		pInput->when(FL_WHEN_ENTER_KEY);

	pInput->callback(m_pToolCallback, m_pToolCallbackData);

	pBox->end();

	m_bToolsChanged = true;

	// Now update the packs
	if (bDoUpdate)
		Update();

	return true;
}



//////////////////////////////////////////////////////////////////////
// Add a check box tool to the tool window

bool CFLTKToolBox::AddToolCheckBox(const char *pcName, bool bSet, 
								bool bFixed, const char *pcDesc, bool bDoUpdate)
{
	if (!m_pVPackBox)
		return false;

	Fl_Group *pBox;
	Fl_Light_Button *pButton;
	TToolMap::iterator it_El;

	// Check whether slider already exists
	if ((it_El = m_mapTool.find(pcName)) != m_mapTool.end())
	{
		// If already a different tool with the same name exists
		// then return with false.
		if (it_El->second.eType != TOOL_CHECKBOX)
			return false;

		// don't create anew if yes
		it_El->second.bInUse = true;

		if (bSet != it_El->second.bInitVal)
		{
			it_El->second.bInitVal = bSet;

			pButton = (Fl_Light_Button *) it_El->second.pWidget;
			pButton->value((bSet ? 1 : 0));
			pButton->redraw();
		}

		return true;
	}

	STool &rTool = m_mapTool[pcName];

	m_pVPackBox->init_sizes();

	if (!(pBox = new Fl_Group(0, 0, m_iToolWidth, 30)))
		return false;
	pBox->box(FL_DOWN_BOX);
	pBox->begin();

	rTool.sName = pcName;

	if (!(pButton = new Fl_Light_Button(3, 4, m_iToolWidth-2*m_iToolWBorder, 23, rTool.sName.c_str())))
		return false;

	rTool.eType = TOOL_CHECKBOX;
	rTool.pWidget = (Fl_Widget *) pButton;
	rTool.pBox = pBox;
	rTool.bInUse = true;
	rTool.bFixed = bFixed;
	rTool.bInitVal = bSet;

	if (pcDesc)
		rTool.sDesc = pcDesc;

	pButton->box(FL_FLAT_BOX);
	pButton->labelsize(12);
	//pButton->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
	//pButton->color(0x9F9F9F00);
	pButton->value((bSet ? 1 : 0));
	pButton->redraw();

	if (pcDesc)
		pButton->tooltip(rTool.sDesc.c_str());

	pButton->callback(m_pToolCallback, m_pToolCallbackData);

	pBox->end();

	m_bToolsChanged = true;

	// Now update the packs
	if (bDoUpdate)
		Update();

	return true;
}


//////////////////////////////////////////////////////////////////////
// Add a button tool to the tool window

bool CFLTKToolBox::AddToolButton(const char *pcName, 
								bool bFixed, const char *pcDesc, bool bDoUpdate)
{
	if (!m_pVPackBox)
		return false;

	Fl_Group *pBox;
	Fl_Button *pButton;
	TToolMap::iterator it_El;

	// Check whether slider already exists
	if ((it_El = m_mapTool.find(pcName)) != m_mapTool.end())
	{
		// If already a different tool with the same name exists
		// then return with false.
		if (it_El->second.eType != TOOL_BUTTON)
			return false;

		// don't create anew if yes
		it_El->second.bInUse = true;

		return true;
	}

	STool &rTool = m_mapTool[pcName];

	m_pVPackBox->init_sizes();

	if (!(pBox = new Fl_Group(0, 0, m_iToolWidth, 30)))
		return false;
	pBox->box(FL_DOWN_BOX);
	pBox->begin();

	rTool.sName = pcName;

	if (!(pButton = new Fl_Button(3, 4, m_iToolWidth-2*m_iToolWBorder, 23, rTool.sName.c_str())))
		return false;

	rTool.eType = TOOL_BUTTON;
	rTool.pWidget = (Fl_Widget *) pButton;
	rTool.pBox = pBox;
	rTool.bInUse = true;
	rTool.bFixed = bFixed;

	if (pcDesc)
		rTool.sDesc = pcDesc;

	//pButton->box(FL_FLAT_BOX);
	pButton->labelsize(12);
	pButton->when(FL_WHEN_RELEASE);
	//pButton->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
	//pButton->color(0x9F9F9F00);
	//pButton->value((bSet ? 1 : 0));
	pButton->redraw();

	if (pcDesc)
		pButton->tooltip(rTool.sDesc.c_str());

	pButton->callback(m_pToolCallback, m_pToolCallbackData);

	pBox->end();

	m_bToolsChanged = true;

	// Now update the packs
	if (bDoUpdate)
		Update();

	return true;
}

//////////////////////////////////////////////////////////////////////////
// Add a ComboBox tool

bool CFLTKToolBox::AddToolChoice(const char *pcName, vector<string>& vecData,
								 const char* pcChoice,
				bool bFixed, const char *pcDesc, bool bDoUpdate)
{
	if (!m_pVPackBox)
		return false;

	Fl_Group *pBox;
	Fl_Choice *pChoice;
	TToolMap::iterator it_El;

	// Check whether slider already exists
	if ((it_El = m_mapTool.find(pcName)) != m_mapTool.end())
	{
		// If already a different tool with the same name exists
		// then return with false.
		if (it_El->second.eType != TOOL_CHOICE)
			return false;


		// don't create anew if yes
		pChoice = (Fl_Choice *) it_El->second.pWidget;

		int iChoiceSize = pChoice->size();
		int iCurVal = pChoice->value();
		bool bReset = true;

		string sCurVal;
		if (iCurVal < iChoiceSize && iCurVal >= 0)
			sCurVal = pChoice->menu()[iCurVal].text;
		else
			sCurVal = "";

		int i, iCount = vecData.size();
		int iSel = 0, iCurSel = -1;

		if (iChoiceSize == iCount+1)
		{
			for( i = 0; i < iCount; i++ )
			{
				if (pChoice->menu()[i].text != vecData[i])
					break;

				if (pcChoice && vecData[i] == pcChoice)
				{
					iSel = i;
				}

				if (vecData[i] == sCurVal)
				{
					iCurSel = i;
				}
			}

			if (i == iCount)
				bReset = false;
		}

		if (bReset)
		{
			iSel = 0;
			iCurSel = -1;

			pChoice->clear();
			for( i = 0; i < iCount; i++ )
			{
				pChoice->add(vecData[i].c_str());
				if (pcChoice && vecData[i] == pcChoice)
				{
					iSel = i;
				}

				if (vecData[i] == sCurVal)
				{
					iCurSel = i;
				}
			}
		}
		
		if (vecData[iSel] != it_El->second.sInitVal)
		{
			pChoice->value(iSel);
			it_El->second.sInitVal = vecData[iSel];

			pChoice->redraw();
		}
		else
		{
			if (iCurSel >= 0)
				pChoice->value(iCurSel);
			else
				pChoice->value(iSel);

			if (bReset)
				pChoice->redraw();		
		}

		// don't create anew if yes
		it_El->second.bInUse = true;

		return true;
	}

	STool &rTool = m_mapTool[pcName];

	m_pVPackBox->init_sizes();

	if (!(pBox = new Fl_Group(0, 0, m_iToolWidth, 30)))
		return false;
	pBox->box(FL_DOWN_BOX);
	pBox->begin();

	rTool.sName = pcName;

	if (!(pChoice = new Fl_Choice(4, 12, m_iToolWidth-2*m_iToolWBorder, 15, rTool.sName.c_str())))
		return false;

	rTool.eType = TOOL_CHOICE;
	rTool.pWidget = (Fl_Widget *) pChoice;
	rTool.pBox = pBox;
	rTool.bInUse = true;
	rTool.bFixed = bFixed;
	if (pcChoice)
		rTool.sInitVal = pcChoice;
	else
		rTool.sInitVal = "";

	if (pcDesc)
		rTool.sDesc = pcDesc;

	//pButton->box(FL_FLAT_BOX);
	pChoice->labelsize(10);
	pChoice->textsize(12);
	pChoice->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
	//pChoice->when(FL_WHEN_RELEASE);
	//pButton->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
	//pButton->color(0x9F9F9F00);
	//pButton->value((bSet ? 1 : 0));

	int i, iCount = vecData.size();
	int iSel = 0;
	for( i = 0; i < iCount; i++ )
	{
		pChoice->add(vecData[i].c_str());
		if (pcChoice && vecData[i] == pcChoice)
		{
			iSel = i;
		}
	}

	pChoice->value(iSel);
	pChoice->redraw();

	if (pcDesc)
		pChoice->tooltip(rTool.sDesc.c_str());

	pChoice->callback(m_pToolCallback, m_pToolCallbackData);

	pBox->end();

	m_bToolsChanged = true;

	// Now update the packs
	if (bDoUpdate)
		Update();

	return true;
}



#endif // CLUDRAW_FLTK
