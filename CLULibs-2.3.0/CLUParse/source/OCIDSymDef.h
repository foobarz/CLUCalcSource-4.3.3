/// LICENSE START
////////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the CLUParse Library.
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


#if !defined(_OC_IDSYM_DEF_H_INCLUDED_)
#define _OC_IDSYM_DEF_H_INCLUDED_

// Symbol that switches to special OC ID List
#define OC_IDSYM_SPECIAL_CHAR	'\x01'
#define OC_IDSYM_SPECIAL_STR	"\x01"

// Symbol to separate namespaces names and variable names in labels
#define OC_IDSYM_GLOBAL_CHAR	'\x02'
#define OC_IDSYM_GLOBAL_STR		"\x02"



#define OC_IDSYM_IF_CHAR		'\x01'
#define OC_IDSYM_IF_STR			OC_IDSYM_SPECIAL_STR "\x01"

#define OC_IDSYM_THEN_CHAR		'\x02'
#define OC_IDSYM_THEN_STR		OC_IDSYM_SPECIAL_STR "\x02"

#define OC_IDSYM_ELSE_CHAR		'\x03'
#define OC_IDSYM_ELSE_STR		OC_IDSYM_SPECIAL_STR "\x03"

#define OC_IDSYM_EQUAL_CHAR		'\x04'
#define OC_IDSYM_EQUAL_STR		OC_IDSYM_SPECIAL_STR "\x04"

#define OC_IDSYM_NOTEQUAL_CHAR	'\x05'
#define OC_IDSYM_NOTEQUAL_STR	OC_IDSYM_SPECIAL_STR "\x05"

#define OC_IDSYM_AND_CHAR		'\x06'
#define OC_IDSYM_AND_STR		OC_IDSYM_SPECIAL_STR "\x06"

#define OC_IDSYM_OR_CHAR		'\x07'
#define OC_IDSYM_OR_STR			OC_IDSYM_SPECIAL_STR "\x07"

#define OC_IDSYM_NOT_CHAR		'\x08'
#define OC_IDSYM_NOT_STR		OC_IDSYM_SPECIAL_STR "\x08"

#define OC_IDSYM_LSHIFT_CHAR	'\x09'
#define OC_IDSYM_LSHIFT_STR		OC_IDSYM_SPECIAL_STR "\x09"

#define OC_IDSYM_LOOP_CHAR		'\x0A'
#define OC_IDSYM_LOOP_STR		OC_IDSYM_SPECIAL_STR "\x0A"

#define OC_IDSYM_BREAK_CHAR		'\x0B'
#define OC_IDSYM_BREAK_STR		OC_IDSYM_SPECIAL_STR "\x0B"

#define OC_IDSYM_GE_CHAR		'\x0C'
#define OC_IDSYM_GE_STR			OC_IDSYM_SPECIAL_STR "\x0C"

#define OC_IDSYM_LE_CHAR		'\x0D'
#define OC_IDSYM_LE_STR			OC_IDSYM_SPECIAL_STR "\x0D"

#define OC_IDSYM_CALL_CHAR		'\x0E'
#define OC_IDSYM_CALL_STR		OC_IDSYM_SPECIAL_STR "\x0E"

#define OC_IDSYM_REF_CHAR		'\x0F'
#define OC_IDSYM_REF_STR		OC_IDSYM_SPECIAL_STR "\x0F"

#define OC_IDSYM_PAND_CHAR		'\x10'
#define OC_IDSYM_PAND_STR		OC_IDSYM_SPECIAL_STR "\x10"

#define OC_IDSYM_PDIV_CHAR		'\x11'
#define OC_IDSYM_PDIV_STR		OC_IDSYM_SPECIAL_STR "\x11"

#define OC_IDSYM_PMUL_CHAR		'\x12'
#define OC_IDSYM_PMUL_STR		OC_IDSYM_SPECIAL_STR "\x12"

#define OC_IDSYM_PIP_CHAR		'\x13'
#define OC_IDSYM_PIP_STR		OC_IDSYM_SPECIAL_STR "\x13"

#define OC_IDSYM_PMINUS_CHAR	'\x14'
#define OC_IDSYM_PMINUS_STR		OC_IDSYM_SPECIAL_STR "\x14"

#define OC_IDSYM_PGT_CHAR		'\x15'
#define OC_IDSYM_PGT_STR		OC_IDSYM_SPECIAL_STR "\x15"

#define OC_IDSYM_PPLUS_CHAR		'\x16'
#define OC_IDSYM_PPLUS_STR		OC_IDSYM_SPECIAL_STR "\x16"

#define OC_IDSYM_POP_CHAR		'\x17'
#define OC_IDSYM_POP_STR		OC_IDSYM_SPECIAL_STR "\x17"

#define OC_IDSYM_PLSHIFT_CHAR	'\x18'
#define OC_IDSYM_PLSHIFT_STR	OC_IDSYM_SPECIAL_STR "\x18"

#define OC_IDSYM_POR_CHAR		'\x19'
#define OC_IDSYM_POR_STR		OC_IDSYM_SPECIAL_STR "\x19"

#define OC_IDSYM_PEQUAL_CHAR	'\x1A'
#define OC_IDSYM_PEQUAL_STR		OC_IDSYM_SPECIAL_STR "\x1A"

#define OC_IDSYM_PNEQUAL_CHAR	'\x1B'
#define OC_IDSYM_PNEQUAL_STR	OC_IDSYM_SPECIAL_STR "\x1B"

#define OC_IDSYM_PLT_CHAR		'\x1C'
#define OC_IDSYM_PLT_STR		OC_IDSYM_SPECIAL_STR "\x1C"

#define OC_IDSYM_PLTE_CHAR		'\x1D'
#define OC_IDSYM_PLTE_STR		OC_IDSYM_SPECIAL_STR "\x1D"

#define OC_IDSYM_PGTE_CHAR		'\x1E'
#define OC_IDSYM_PGTE_STR		OC_IDSYM_SPECIAL_STR "\x1E"

#define OC_IDSYM_POW_CHAR		'\x1F'
#define OC_IDSYM_POW_STR		OC_IDSYM_SPECIAL_STR "\x1F"



//#define OC_IDSYM__CHAR		'\x0'
//#define OC_IDSYM__STR		"\x0"




#endif

