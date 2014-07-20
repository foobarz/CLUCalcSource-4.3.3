/// LICENSE START
////////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the CLUCalc program.
// CLUCalc is an interactive Clifford calculator with OpenGL visualiations.
//
// 
// Copyright (C) 2002-2004  Christian B.U. Perwass
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

#if !defined( _FLTKEDITOR_H__INCLUDED_ )
#define _FLTKEDITOR_H__INCLUDED_

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Editor.H>

#include "clu/mem.h"
#include "clu/memobj.h"

#include <string>
#include <vector>
#include <list>

using namespace std;



class CFLTKEditor : public Fl_Double_Window
{
public:
	struct SUndoData
	{
		int iPos;			// Position of Change
		bool bInsert;		// true: inserted text, false: deleted text
		string sText;		// text which was inserted or deleted
	};

	typedef list<SUndoData> TUndoBuf;

	struct SEditorData
	{
		SEditorData();
		~SEditorData();
		bool Create(CFLTKEditor& rParent);
		bool Destroy();

		bool				m_bChanged;

		char				m_pcSearchString[256];

		string				m_sFilename;
		string				m_sName;
		string				m_sPath;
		string				m_sTitle;

		TUndoBuf			m_bufUndo;	// Undo Buffer
		TUndoBuf			m_bufUndo2;	// Undo Undo Buffer

		Fl_Text_Buffer		*m_pTextBuf;
		Fl_Text_Buffer		*m_pStyleBuf;
		Fl_Text_Editor		*m_pEditor;
	};

public:
	CFLTKEditor();
	CFLTKEditor(int iPosX, int iPosY, int iWidth, int iHeight, const char* pcFilename = 0,
				void *pvIcon = 0);
	~CFLTKEditor(void);

	virtual bool Create(int iPosX, int iPosY, int iWidth, int iHeight, const char* pcFilename = 0,
						void *pvIcon = 0);
	void Show();

	// Helper Functions
	bool LoadFile(const char* pcFilename, int iPos = -1);
	void SaveFile(const char* pcFilename);

	// Goto given position and highlight line.
	bool HighlightLinePos(const char *pcFilename, int iInputPos);

	vector<Fl_Text_Display::Style_Table_Entry>& GetStyleTable() { return m_vecStyleTable; }

	void UpdateFileList();
	bool SetCurEditor(const char *pcFilename);

//	vector<Fl_Menu_Item>& GetContextMenuItems() { return m_vecCMenuItem; }
	Fl_Menu_Button* GetContextMenu() { return m_pContextMenu; }

protected:
	bool CheckSave();
	bool SetTitle();
	void SetNameAndPath();

protected:
	// Virtual Functions that influence Editor behaviour

	// Override this function to create your own menus
	virtual void InitMenu();

	// Override this function to add your own styles
	virtual void InitStyleTable();

protected:
	// Virtual Functions called by Callbacks
	virtual void Copy();
	virtual void Cut();
	virtual void Delete();
	virtual void Paste();
	virtual void Undo();
	virtual void Undo2();
	virtual void Indent();
	virtual void UnIndent();

	virtual void Find();
	virtual void Find2();

	virtual void Replace();
	virtual void Replace2();
	virtual void ReplaceAll();
	virtual void ReplaceCan();

	virtual void New();
	virtual void Load();
	virtual void Insert();
	virtual void Close();
	virtual void Save();
	virtual void SaveAs();
	virtual void NextFile();
	virtual void PrevFile();
	virtual void SetFile();

	virtual void Quit();

	virtual void Changed(int iVal, int nInserted, int nDeleted, 
						int iVal2, const char* pcVal);

	virtual void StyleUnfinished(int iVal);
	virtual void StyleUpdate(int iPos, int nInserted,	int nDeleted,
							 int nRestyled, const char *pcDeletedText);

	virtual void StyleParse(const char *pcText, char *pcStyle, int iLength);

	// Is called when text is pasted into editor with pasted text in sText.
	// If text need not be adapted returns false. If it returns true,
	// then the text in sNewText is inserted instead of original text.
	virtual bool AdaptText(string &sNewText, string &sText);

protected:
	// Callbacks
	static void CB_Copy(Fl_Widget* pWidget, void *pData);
	static void CB_Cut(Fl_Widget* pWidget, void *pData);
	static void CB_Delete(Fl_Widget* pWidget, void *pData);
	static void CB_Paste(Fl_Widget* pWidget, void *pData);
	static void CB_Undo(Fl_Widget* pWidget, void *pData);
	static void CB_Undo2(Fl_Widget* pWidget, void *pData);
	static void CB_Indent(Fl_Widget* pWidget, void *pData);
	static void CB_UnIndent(Fl_Widget* pWidget, void *pData);

	static void CB_Find(Fl_Widget* pWidget, void *pData);
	static void CB_Find2(Fl_Widget* pWidget, void *pData);

	static void CB_Replace(Fl_Widget* pWidget, void *pData);
	static void CB_Replace2(Fl_Widget* pWidget, void *pData);
	static void CB_ReplaceAll(Fl_Widget* pWidget, void *pData);
	static void CB_ReplaceCan(Fl_Widget* pWidget, void *pData);

	static void CB_New(Fl_Widget* pWidget, void *pData);
	static void CB_Load(Fl_Widget* pWidget, void *pData);
	static void CB_Insert(Fl_Widget* pWidget, void *pData);
	static void CB_Close(Fl_Widget* pWidget, void *pData);
	static void CB_Save(Fl_Widget* pWidget, void *pData);
	static void CB_SaveAs(Fl_Widget* pWidget, void *pData);
	static void CB_NextFile(Fl_Widget* pWidget, void *pData);
	static void CB_PrevFile(Fl_Widget* pWidget, void *pData);
	static void CB_SetFile(Fl_Widget* pWidget, void *pData);

	static void CB_Quit(Fl_Widget* pWidget, void *pData);

	static void CB_Changed(int iVal, int nInserted, int nDeleted, 
						int iVal2, const char* pcVal, void* pvVal);

	static void CB_StyleUnfinished(int iVal, void *pvVal);
	static void CB_StyleUpdate(int iPos, int nInserted,	int nDeleted,
								int nRestyled, const char *deletedText,
								void *pvData);


	bool& IsFileChanged() { return m_mEditorData[m_iCurEditorID].m_bChanged; }
	char *GetSearchString() { return m_mEditorData[m_iCurEditorID].m_pcSearchString; }
	string& GetFilename() { return m_mEditorData[m_iCurEditorID].m_sFilename; }
	string& GetName() { return m_mEditorData[m_iCurEditorID].m_sName; }
	string& GetPath() { return m_mEditorData[m_iCurEditorID].m_sPath; }
	string& GetTitle() { return m_mEditorData[m_iCurEditorID].m_sTitle; }
	TUndoBuf& GetUndoBuf() { return m_mEditorData[m_iCurEditorID].m_bufUndo; }
	TUndoBuf& GetUndo2Buf() { return m_mEditorData[m_iCurEditorID].m_bufUndo2; }
	Fl_Text_Buffer* GetTextBuffer() { return m_mEditorData[m_iCurEditorID].m_pTextBuf; }
	Fl_Text_Buffer* GetStyleBuffer() { return m_mEditorData[m_iCurEditorID].m_pStyleBuf; }
	Fl_Text_Editor* GetEditor() { return m_mEditorData[m_iCurEditorID].m_pEditor; }

	// Creates New Editor
	// Returns -1 if unsuccessful, otherwise the id number of the new editor
	int NewEditor();
	bool DeleteEditor(int iFileID);
	bool SetCurEditor(int iFileID);
	int GetEditorID(const char *pcPath, const char *pcName);

protected:
	Fl_Window          *m_pReplaceDlg;
	Fl_Input           *m_pReplaceFind;
	Fl_Input           *m_pReplaceWith;
	Fl_Button          *m_pReplaceAll;
	Fl_Return_Button   *m_pReplaceNext;
	Fl_Button          *m_pReplaceCancel;
	Fl_Choice		   *m_pFileChoice;
	Fl_Pack				*m_pToolBar;
	Fl_Box				*m_pHSpaceBox;

	bool			   m_bLoading;
	bool				m_bIsOK;
	bool				m_bReparsing;
	bool				m_bReparseAll;

/*
	bool               m_bChanged;
	char				m_pcSearchString[256];
	string             m_sFilename;
	string			   m_sName;
	string			   m_sPath;
	string             m_sTitle;
	Fl_Text_Buffer     *m_pTextBuf;
	Fl_Text_Editor     *m_pEditor;
	Fl_Text_Buffer     *m_pStyleBuf;
*/

	int m_iCurEditorID;
	MemObj<SEditorData> m_mEditorData;
	static int sm_iNewFileNo;

	int m_iMaxUndoSteps;
	bool m_bInUndo;		// Is set true while undoing changes.

	vector<Fl_Text_Display::Style_Table_Entry> m_vecStyleTable;

	Fl_Menu_Bar			*m_pMenuBar;
	Fl_Menu_Button		*m_pContextMenu;
	vector<Fl_Menu_Item> m_vecMenuItem;
	vector<Fl_Menu_Item> m_vecCMenuItem;

	string m_sFileChooserLoadTitle;
	string m_sFileChooserSaveTitle;
	string m_sFileChooserPattern;

	string m_sTitle;
};


#endif  // #define _FLTKEDITOR_H__INCLUDED_


