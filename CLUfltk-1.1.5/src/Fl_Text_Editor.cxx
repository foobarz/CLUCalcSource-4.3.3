//
// "$Id: Fl_Text_Editor.cxx,v 1.17 2005/10/18 08:23:35 chp Exp $"
//
// Copyright 2001-2003 by Bill Spitzak and others.
// Original code Copyright Mark Edel.  Permission to distribute under
// the LGPL for the FLTK library granted by Mark Edel.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems to "fltk-bugs@fltk.org".
//

#include <stdio.h>
#include <stdlib.h>
#include "flstring.h"
#include <ctype.h>
#include <FL/Fl.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/fl_ask.H>


Fl_Text_Editor::Fl_Text_Editor(int X, int Y, int W, int H,  const char* l)
    : Fl_Text_Display(X, Y, W, H, l) {
  mCursorOn = 1;
  insert_mode_ = 1;
  key_bindings = 0;

  // handle the default key bindings
  add_default_key_bindings(&key_bindings);

  // handle everything else
  default_key_function(kf_default);
}

Fl_Text_Editor::Key_Binding* Fl_Text_Editor::global_key_bindings = 0;

static int ctrl_a(int, Fl_Text_Editor* e);

// These are the default key bindings every widget should start with
static struct {
  int key; 
  int state;
  Fl_Text_Editor::Key_Func func;
} default_key_bindings[] = {
  { FL_Escape,    FL_TEXT_EDITOR_ANY_STATE, Fl_Text_Editor::kf_ignore     },
  { FL_Enter,     FL_TEXT_EDITOR_ANY_STATE, Fl_Text_Editor::kf_enter      },
  { FL_KP_Enter,  FL_TEXT_EDITOR_ANY_STATE, Fl_Text_Editor::kf_enter      },
  { FL_BackSpace, FL_TEXT_EDITOR_ANY_STATE, Fl_Text_Editor::kf_backspace  },
  { FL_Insert,    FL_TEXT_EDITOR_ANY_STATE, Fl_Text_Editor::kf_insert     },
  { FL_Delete,    0,						Fl_Text_Editor::kf_delete     },
  { FL_KP + '0',  0,						Fl_Text_Editor::kf_insert     },
  { FL_KP + '.',  0,						Fl_Text_Editor::kf_delete     },
  { FL_KP + ',',  0,						Fl_Text_Editor::kf_delete     },
  { FL_Home,      0,                        Fl_Text_Editor::kf_move       },
  { FL_End,       0,                        Fl_Text_Editor::kf_move       },
  { FL_Left,      0,                        Fl_Text_Editor::kf_move       },
  { FL_Up,        0,                        Fl_Text_Editor::kf_move       },
  { FL_Right,     0,                        Fl_Text_Editor::kf_move       },
  { FL_Down,      0,                        Fl_Text_Editor::kf_move       },
  { FL_Page_Up,   0,                        Fl_Text_Editor::kf_move       },
  { FL_Page_Down, 0,                        Fl_Text_Editor::kf_move       },
  { FL_KP + '1',  0,                        Fl_Text_Editor::kf_move       },
  { FL_KP + '2',  0,                        Fl_Text_Editor::kf_move       },
  { FL_KP + '3',  0,                        Fl_Text_Editor::kf_move       },
  { FL_KP + '4',  0,                        Fl_Text_Editor::kf_move       },
  { FL_KP + '5',  0,                        Fl_Text_Editor::kf_move       },
  { FL_KP + '6',  0,                        Fl_Text_Editor::kf_move       },
  { FL_KP + '7',  0,                        Fl_Text_Editor::kf_move       },
  { FL_KP + '8',  0,                        Fl_Text_Editor::kf_move       },
  { FL_KP + '9',  0,                        Fl_Text_Editor::kf_move       },
  { FL_Home,      FL_SHIFT,                 Fl_Text_Editor::kf_shift_move },
  { FL_End,       FL_SHIFT,                 Fl_Text_Editor::kf_shift_move },
  { FL_Left,      FL_SHIFT,                 Fl_Text_Editor::kf_shift_move },
  { FL_Up,        FL_SHIFT,                 Fl_Text_Editor::kf_shift_move },
  { FL_Right,     FL_SHIFT,                 Fl_Text_Editor::kf_shift_move },
  { FL_Down,      FL_SHIFT,                 Fl_Text_Editor::kf_shift_move },
  { FL_Page_Up,   FL_SHIFT,                 Fl_Text_Editor::kf_shift_move },
  { FL_Page_Down, FL_SHIFT,                 Fl_Text_Editor::kf_shift_move },
  { FL_Home,      FL_CTRL,                  Fl_Text_Editor::kf_ctrl_move  },
  { FL_End,       FL_CTRL,                  Fl_Text_Editor::kf_ctrl_move  },
  { FL_Left,      FL_CTRL,                  Fl_Text_Editor::kf_ctrl_move  },
  { FL_Up,        FL_CTRL,                  Fl_Text_Editor::kf_ctrl_move  },
  { FL_Right,     FL_CTRL,                  Fl_Text_Editor::kf_ctrl_move  },
  { FL_Down,      FL_CTRL,                  Fl_Text_Editor::kf_ctrl_move  },
  { FL_Page_Up,   FL_CTRL,                  Fl_Text_Editor::kf_ctrl_move  },
  { FL_Page_Down, FL_CTRL,                  Fl_Text_Editor::kf_ctrl_move  },
  { FL_Tab,		  FL_CTRL,                  Fl_Text_Editor::kf_ctrl_move  },
  { FL_Home,      FL_CTRL|FL_SHIFT,         Fl_Text_Editor::kf_c_s_move   },
  { FL_End,       FL_CTRL|FL_SHIFT,         Fl_Text_Editor::kf_c_s_move   },
  { FL_Left,      FL_CTRL|FL_SHIFT,         Fl_Text_Editor::kf_c_s_move   },
  { FL_Up,        FL_CTRL|FL_SHIFT,         Fl_Text_Editor::kf_c_s_move   },
  { FL_Right,     FL_CTRL|FL_SHIFT,         Fl_Text_Editor::kf_c_s_move   },
  { FL_Down,      FL_CTRL|FL_SHIFT,         Fl_Text_Editor::kf_c_s_move   },
  { FL_Page_Up,   FL_CTRL|FL_SHIFT,         Fl_Text_Editor::kf_c_s_move   },
  { FL_Page_Down, FL_CTRL|FL_SHIFT,         Fl_Text_Editor::kf_c_s_move   },
  { FL_Tab,		  FL_CTRL|FL_SHIFT,         Fl_Text_Editor::kf_c_s_move   },
//  { FL_Clear,	  0,                        Fl_Text_Editor::delete_to_eol },
  { 'z',          FL_CTRL,                  Fl_Text_Editor::kf_undo	  },
  { '/',          FL_CTRL,                  Fl_Text_Editor::kf_undo	  },
  { 'x',          FL_CTRL,                  Fl_Text_Editor::kf_cut        },
  { FL_Delete,    FL_SHIFT,                 Fl_Text_Editor::kf_cut        },
  { 'c',          FL_CTRL,                  Fl_Text_Editor::kf_copy       },
  { FL_Insert,    FL_CTRL,                  Fl_Text_Editor::kf_copy       },
  { 'v',          FL_CTRL,                  Fl_Text_Editor::kf_paste      },
  { FL_Insert,    FL_SHIFT,                 Fl_Text_Editor::kf_paste      },
  { 'a',          FL_CTRL,                  ctrl_a                        },

#ifdef __APPLE__
  // Define CMD+key accelerators...
//  { 'z',          FL_COMMAND,               Fl_Text_Editor::kf_undo       },
  { 'x',          FL_COMMAND,               Fl_Text_Editor::kf_cut        },
  { 'c',          FL_COMMAND,               Fl_Text_Editor::kf_copy       },
  { 'v',          FL_COMMAND,               Fl_Text_Editor::kf_paste      },
  { 'a',          FL_COMMAND,               ctrl_a                        },
#endif // __APPLE__

  { 0,            0,                        0                             }
};

void Fl_Text_Editor::add_default_key_bindings(Key_Binding** list) {
  for (int i = 0; default_key_bindings[i].key; i++) {
    add_key_binding(default_key_bindings[i].key,
                    default_key_bindings[i].state,
                    default_key_bindings[i].func,
                    list);
  }
}

Fl_Text_Editor::Key_Func
Fl_Text_Editor::bound_key_function(int key, int state, Key_Binding* list) {
  Key_Binding* cur;
  for (cur = list; cur; cur = cur->next)
    if (cur->key == key)
      if (cur->state == FL_TEXT_EDITOR_ANY_STATE || cur->state == state)
        break;
  if (!cur) return 0;
  return cur->function;
}

void
Fl_Text_Editor::remove_all_key_bindings(Key_Binding** list) {
  Key_Binding *cur, *next;
  for (cur = *list; cur; cur = next) {
    next = cur->next;
    delete cur;
  }
  *list = 0;
}

void
Fl_Text_Editor::remove_key_binding(int key, int state, Key_Binding** list) {
  Key_Binding *cur, *last = 0;
  for (cur = *list; cur; last = cur, cur = cur->next)
    if (cur->key == key && cur->state == state) break;
  if (!cur) return;
  if (last) last->next = cur->next;
  else *list = cur->next;
  delete cur;
}

void
Fl_Text_Editor::add_key_binding(int key, int state, Key_Func function,
                                Key_Binding** list) {
  Key_Binding* kb = new Key_Binding;
  kb->key = key;
  kb->state = state;
  kb->function = function;
  kb->next = *list;
  *list = kb;
}

////////////////////////////////////////////////////////////////

#define NORMAL_INPUT_MOVE 0

static void kill_selection(Fl_Text_Editor* e) {
  if (e->buffer()->selected()) {
    e->insert_position(e->buffer()->primary_selection()->start());
    e->buffer()->remove_selection();
  }
}

int Fl_Text_Editor::kf_default(int c, Fl_Text_Editor* e) {
  if (!c || (!isprint(c) && c != '\t')) return 0;
  char s[2] = "\0";
  s[0] = (char)c;
  kill_selection(e);
  if (e->insert_mode()) e->insert(s);
  else e->overstrike(s);
  e->show_insert_position();
  if (e->when()&FL_WHEN_CHANGED) e->do_callback(); else e->set_changed();
  return 1;
}

int Fl_Text_Editor::kf_ignore(int, Fl_Text_Editor*) {
  return 0; // don't handle
}

int Fl_Text_Editor::kf_backspace(int, Fl_Text_Editor* e) {
  if (!e->buffer()->selected() && e->move_left())
    e->buffer()->select(e->insert_position(), e->insert_position()+1);
  kill_selection(e);
  e->show_insert_position();
  if (e->when()&FL_WHEN_CHANGED) e->do_callback(); else e->set_changed();
  return 1;
}

int Fl_Text_Editor::kf_enter(int, Fl_Text_Editor* e) 
{
	kill_selection(e);

	Fl_Text_Buffer *pBuffer = e->buffer();
	int iTextPos = e->insert_position();
	int iLineStartPos = pBuffer->line_start(iTextPos);
	int iLineEndPos = pBuffer->line_end(iTextPos);
	char *pcLine = pBuffer->line_text(iTextPos);
	char pcIndent[501];
	int i, j, p, iIndentStartPos, iSpaceCount, iTabSize;

	strcpy(pcIndent, "\n");
	p = iIndentStartPos = strlen(pcIndent);
	
	iSpaceCount = 0;
	iTabSize = pBuffer->tab_distance();

	for (i = 0; i < iTextPos-iLineStartPos; i++)
	{
/*
		if (iLineStartPos+i == iLineEndPos)
		{
			pcIndent[iIndentStartPos] = 0;
			break;
		}
*/
		if (pcLine[i] == ' ')
			++iSpaceCount;
		else if (pcLine[i] == '\t')
			iSpaceCount += iTabSize - (iSpaceCount%iTabSize);
		else
		{
			for (j = 0; j < iSpaceCount; j++)
				pcIndent[p++] = ' ';
			pcIndent[p] = 0;
			break;
		}

		if (iSpaceCount >= iTabSize)
		{
			iSpaceCount -= iTabSize;
			pcIndent[p++] = '\t';
		}
	}

	if (i == iTextPos-iLineStartPos)
	{
		for (j = 0; j < iSpaceCount; j++)
			pcIndent[p++] = ' ';
		pcIndent[p] = 0;
	}

	if (iLineStartPos+i == iLineEndPos)
	{
		pBuffer->insert(iLineEndPos, pcIndent);
		e->insert_position(iLineEndPos+i+1);
	}
	else
	{
		e->insert(pcIndent);
	}

	if (i > 0 && iTextPos != iLineStartPos)
	{
		pBuffer->remove(iLineStartPos, iLineStartPos+i);
		if (pcIndent[iIndentStartPos] != 0)
			pBuffer->insert(iLineStartPos, &pcIndent[iIndentStartPos]);
	}

	e->show_insert_position();
	if (e->when()&FL_WHEN_CHANGED) 
		e->do_callback(); 
	else 
		e->set_changed();

	free(pcLine);

	return 1;
}

extern void fl_text_drag_me(int pos, Fl_Text_Display* d);

int Fl_Text_Editor::kf_move(int c, Fl_Text_Editor* e) 
{

	int i, iLineStart, iLineEnd, iTextStart, iTextEnd, iTextPos, iNewPos;
	int selected = e->buffer()->selected();
	Fl_Text_Buffer *pcBuffer = e->buffer();
	char *pcLine;

	if (!selected)
		e->dragPos = e->insert_position();
	e->buffer()->unselect();

	switch (c) 
	{
	case FL_KP + '7':
	case FL_Home:
		iTextPos = e->insert_position();
		iLineStart = pcBuffer->line_start(iTextPos);
		iLineEnd = pcBuffer->line_end(iTextPos);
		pcLine = pcBuffer->line_text(iTextPos);
		// Find start of text in line
		for(i = 0; i < iLineEnd - iLineStart; i++)
		{
			if (pcLine[i] != ' ' && pcLine[i] != '\t')
				break;
		}
		iTextStart = iLineStart + i;
		if (iTextPos == iTextStart)
			iNewPos = iLineStart;
		else if (iTextPos == iLineStart)
			iNewPos = iTextStart;
		else
			iNewPos = iTextStart;

		free(pcLine);
		e->insert_position(iNewPos);
		break;

	case FL_KP + '1':
	case FL_End:
		iTextPos = e->insert_position();
		iLineStart = pcBuffer->line_start(iTextPos);
		iLineEnd = pcBuffer->line_end(iTextPos);
		pcLine = pcBuffer->line_text(iTextPos);
		// Find end of text in line
		for(i = 0; i < iLineEnd - iLineStart; i++)
		{
			if (pcLine[i] == ';')
			{
				i++;
				break;
			}
		}
		iTextEnd = iLineStart + i;

		if (iTextPos == iTextEnd)
			iNewPos = iLineEnd;
		else 
			iNewPos = iTextEnd;

		free(pcLine);

		e->insert_position(iNewPos);
		break;
	case FL_KP + '4':
	case FL_Left:
		e->move_left();
		break;
	case FL_KP + '6':
	case FL_Right:
		e->move_right();
		break;
	case FL_KP + '8':
	case FL_Up:
		e->move_up();
		break;
	case FL_KP + '2':
	case FL_Down:
		e->move_down();
		break;
	case FL_KP + '9':
	case FL_Page_Up:
		for (i = 0; i < e->mNVisibleLines - 1; i++) e->move_up();
		break;
	case FL_KP + '3':
	case FL_Page_Down:
		for (i = 0; i < e->mNVisibleLines - 1; i++) e->move_down();
		break;
	}
	e->show_insert_position();
	return 1;
}

int Fl_Text_Editor::kf_shift_move(int c, Fl_Text_Editor* e) {
	kf_move(c, e);
	fl_text_drag_me(e->insert_position(), e);
	return 1;
}

int Fl_Text_Editor::kf_ctrl_move(int c, Fl_Text_Editor* e) 
{
	if (!e->buffer()->selected())
		e->dragPos = e->insert_position();

	if (c != FL_Up && c != FL_Down && c != FL_Tab) 
	{
		e->buffer()->unselect();
		e->show_insert_position();
	}

/*
	bool bChangedBuffer = false;
	int iSelStart, iSelEnd, iCurPos;
	Fl_Text_Buffer *pcBuffer = e->buffer();
*/
	switch (c) 
	{
	case FL_Home:
		e->insert_position(0);
		break;
	case FL_End:
		e->insert_position(e->buffer()->length());
		break;
	case FL_Left:
		e->previous_word();
		break;
	case FL_Right:
		e->next_word();
		break;
	case FL_Up:
		e->scroll(e->mTopLineNum-1, e->mHorizOffset);
		break;
	case FL_Down:
		e->scroll(e->mTopLineNum+1, e->mHorizOffset);
		break;
	case FL_Page_Up:
		e->insert_position(e->mLineStarts[0]);
		break;
	case FL_Page_Down:
		e->insert_position(e->mLineStarts[e->mNVisibleLines-2]);
		break;
/*
	case FL_Tab:
		pcBuffer->selection_position(&iSelStart, &iSelEnd);
		if (iSelEnd <= iSelStart)
			break;
		iCurPos = pcBuffer->line_start(iSelStart);
		while (pcBuffer->line_end(iCurPos) < iSelEnd)
		{
			bChangedBuffer = true;
			pcBuffer->insert(iCurPos, "\t");
			iCurPos = pcBuffer->skip_lines(iCurPos, 1);
			++iSelEnd;
		}
		if (bChangedBuffer)
			pcBuffer->select(pcBuffer->line_start(iSelStart), iSelEnd);
		break;
*/
	}
	return 1;
}

int Fl_Text_Editor::kf_c_s_move(int c, Fl_Text_Editor* e) 
{
	if (c != FL_Tab)
	{
		kf_ctrl_move(c, e);
		fl_text_drag_me(e->insert_position(), e);
	}
/*
	else
	{
		bool bChangedBuffer = false;
		int iSelStart, iSelEnd, iCurPos, iTabSize, i;
		char *pcLine;
		Fl_Text_Buffer *pBuffer = e->buffer();
	
		pBuffer->selection_position(&iSelStart, &iSelEnd);
		if (iSelEnd > iSelStart)
		{
			iCurPos = pBuffer->line_start(iSelStart);
			iTabSize = pBuffer->tab_distance();

			while (pBuffer->line_end(iCurPos) < iSelEnd)
			{
				pcLine = pBuffer->line_text(iCurPos);
				if (pcLine[0] == '\t')
				{
					bChangedBuffer = true;
					pBuffer->remove(iCurPos, iCurPos+1);
					--iSelEnd;
				}
				else if (pcLine[0] == ' ')
				{
					for (i = 1; i < iTabSize; i++)
					{
						if (pcLine[i] != ' ')
							break;
					}

					if (i == iTabSize) // same number of spaces as a tab
					{
						bChangedBuffer = true;
						pBuffer->remove(iCurPos, iCurPos + iTabSize);
						iSelEnd -= iTabSize;
					}
				}
				iCurPos = pBuffer->skip_lines(iCurPos, 1);
				free(pcLine);
			}
			if (bChangedBuffer)
				pBuffer->select(pBuffer->line_start(iSelStart), iSelEnd);
		}
	}
*/

	return 1;
}

static int ctrl_a(int, Fl_Text_Editor* e) {
  // make 2+ ^A's in a row toggle select-all:
  int i = e->buffer()->line_start(e->insert_position());
  if (i != e->insert_position())
    return Fl_Text_Editor::kf_move(FL_Home, e);
  else {
    if (e->buffer()->selected())
      e->buffer()->unselect();
    else
      Fl_Text_Editor::kf_select_all(0, e);
  }
  return 1;
}

int Fl_Text_Editor::kf_home(int, Fl_Text_Editor* e) {
    return kf_move(FL_Home, e);
}

int Fl_Text_Editor::kf_end(int, Fl_Text_Editor* e) {
  return kf_move(FL_End, e);
}

int Fl_Text_Editor::kf_left(int, Fl_Text_Editor* e) {
  return kf_move(FL_Left, e);
}

int Fl_Text_Editor::kf_up(int, Fl_Text_Editor* e) {
  return kf_move(FL_Up, e);
}

int Fl_Text_Editor::kf_right(int, Fl_Text_Editor* e) {
  return kf_move(FL_Right, e);
}

int Fl_Text_Editor::kf_down(int, Fl_Text_Editor* e) {
  return kf_move(FL_Down, e);
}

int Fl_Text_Editor::kf_page_up(int, Fl_Text_Editor* e) {
  return kf_move(FL_Page_Up, e);
}

int Fl_Text_Editor::kf_page_down(int, Fl_Text_Editor* e) {
  return kf_move(FL_Page_Down, e);
}


int Fl_Text_Editor::kf_insert(int, Fl_Text_Editor* e) 
{
	if (e->insert_mode())
	{
		e->insert_mode(0);
		e->cursor_style(BLOCK_CURSOR);
	}
	else
	{
		e->insert_mode(1);
		e->cursor_style(NORMAL_CURSOR);
	}

	return 1;
}

int Fl_Text_Editor::kf_delete(int, Fl_Text_Editor* e) {
  if (!e->buffer()->selected())
    e->buffer()->select(e->insert_position(), e->insert_position()+1);
  kill_selection(e);
  e->show_insert_position();
  if (e->when()&FL_WHEN_CHANGED) e->do_callback(); else e->set_changed();
  return 1;
}

int Fl_Text_Editor::kf_copy(int, Fl_Text_Editor* e) 
{
	char *copy, *pcLine;

	if (!e->buffer()->selected()) 
	{
		pcLine = e->buffer()->line_text(e->insert_position());
		copy = (char *) malloc(strlen(pcLine)+strlen("\n")+1);
		strcpy(copy, pcLine);
		strcat(copy, "\n");
		free(pcLine);
	}
	else
		copy = e->buffer()->selection_text();

	if (*copy) 
		Fl::copy(copy, strlen(copy), 1);

	free((void*)copy);
	e->show_insert_position();
	return 1;
}

int Fl_Text_Editor::kf_cut(int c, Fl_Text_Editor* e) {
  kf_copy(c, e);
  kill_selection(e);
  if (e->when()&FL_WHEN_CHANGED) e->do_callback(); else e->set_changed();
  return 1;
}

int Fl_Text_Editor::kf_paste(int, Fl_Text_Editor* e) {
  kill_selection(e);
  Fl::paste(*e, 1);
  e->show_insert_position();
  if (e->when()&FL_WHEN_CHANGED) e->do_callback(); else e->set_changed();
  return 1;
}

int Fl_Text_Editor::kf_select_all(int, Fl_Text_Editor* e) {
  e->buffer()->select(0, e->buffer()->length());
  return 1;
}

int Fl_Text_Editor::kf_undo(int , Fl_Text_Editor* e) {
  e->buffer()->unselect();
  int crsr;
  int ret = e->buffer()->undo(&crsr);
  e->insert_position(crsr);
  e->show_insert_position();
  if (e->when()&FL_WHEN_CHANGED) e->do_callback(); else e->set_changed();
  return ret;
}

int Fl_Text_Editor::handle_key() {

  // Call fltk's rules to try to turn this into a printing character.
  // This uses the right-hand ctrl key as a "compose prefix" and returns
  // the changes that should be made to the text, as a number of
  // bytes to delete and a string to insert:

	int del;
	if (Fl::compose(del)) 
	{
		if (del) 
			buffer()->select(insert_position()-del, insert_position());

		kill_selection(this);

		if (Fl::event_length()) 
		{
			if (insert_mode()) 
				insert(Fl::event_text());
			else 
				overstrike(Fl::event_text());
		}
		show_insert_position();
		return 1;
	}


  int key = Fl::event_key(), state = Fl::event_state(), c = Fl::event_text()[0];
  state &= FL_SHIFT|FL_CTRL|FL_ALT|FL_META; // only care about these states
  Key_Func f;
  f = bound_key_function(key, state, global_key_bindings);
  if (!f) f = bound_key_function(key, state, key_bindings);

  if (f) 
	  return f(key, this);

  if (default_key_function_ && !state) 
	return default_key_function_(c, this);
  
  return 0;
}

void Fl_Text_Editor::maybe_do_callback() {
  if (changed() || (when()&FL_WHEN_NOT_CHANGED)) {
    clear_changed(); do_callback();}
}

int Fl_Text_Editor::handle(int event) {
  if (!buffer()) return 0;

  if (event == FL_PUSH && Fl::event_button() == 2) {
    dragType = -1;
    Fl::paste(*this, 0);
    Fl::focus(this);
    if (when()&FL_WHEN_CHANGED) do_callback(); else set_changed();
    return 1;
  }

  switch (event) {
    case FL_FOCUS:
      show_cursor(mCursorOn); // redraws the cursor
      Fl::focus(this);
      return 1;

    case FL_UNFOCUS:
      show_cursor(mCursorOn); // redraws the cursor
    case FL_HIDE:
      if (when() & FL_WHEN_RELEASE) maybe_do_callback();
      return 1;

    case FL_KEYBOARD:
      return handle_key();

    case FL_PASTE:
      if (!Fl::event_text()) {
        fl_beep();
	return 1;
      }
      buffer()->remove_selection();
      if (insert_mode()) insert(Fl::event_text());
      else overstrike(Fl::event_text());
      show_insert_position();
      if (when()&FL_WHEN_CHANGED) do_callback(); else set_changed();
      return 1;

// CET - FIXME - this will clobber the window's current cursor state!
//    case FL_ENTER:
//    case FL_MOVE:
//    case FL_LEAVE:
//      if (Fl::event_inside(text_area)) fl_cursor(FL_CURSOR_INSERT);
//      else fl_cursor(FL_CURSOR_DEFAULT);
  }

  return Fl_Text_Display::handle(event);
}

//
// End of "$Id: Fl_Text_Editor.cxx,v 1.17 2005/10/18 08:23:35 chp Exp $".
//
