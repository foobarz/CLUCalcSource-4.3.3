/// LICENSE START
////////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the CLU Library.
//
// 
// Copyright (C) 1997-2004  Christian B.U. Perwass
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


template<class CType> int E3GA<CType>::piGrades[] = {
	0, 1, 1, 1, 2, 2, 2, 3
};

template<class CType> int E3GA<CType>::piGPTable[] = {
	1, 2, 3, 4, 5, 6, 7, 8, 2, 1, 7, -6, 8, -4, 3, 5, 3, -7, 1, 5, 
	4, 8, -2, 6, 4, 6, -5, 1, -3, 2, 8, 7, 5, 8, -4, 3, -1, -7, 6, 
	-2, 6, 4, 8, -2, 7, -1, -5, -3, 7, -3, 2, 8, -6, 5, -1, -4, 8, 
	5, 6, 7, -2, -3, -4, -1
};

template<class CType> int E3GA<CType>::piIPTable[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 1, 0, 0, 0, -4, 3, 5, 
	0, 0, 1, 0, 4, 0, -2, 6, 
	0, 0, 0, 1, -3, 2, 0, 7, 
	0, 0, -4, 3, -1, 0, 0, -2, 
	0, 4, 0, -2, 0, -1, 0, -3, 
	0, -3, 2, 0, 0, 0, -1, -4, 
	0, 5, 6, 7, -2, -3, -4, -1
};

template<class CType> int E3GA<CType>::piOPTable[] = {
	1, 2, 3, 4, 5, 6, 7, 8, 2, 0, 7, -6, 8, 0, 0, 0, 3, -7, 0, 5, 
	0, 8, 0, 0, 4, 6, -5, 0, 0, 0, 8, 0, 5, 8, 0, 0, 0, 0, 0, 0, 
	6, 0, 8, 0, 0, 0, 0, 0, 7, 0, 0, 8, 0, 0, 0, 0, 8, 0, 0, 0, 0, 
	0, 0, 0
};

template<class CType> int E3GA<CType>::piVITable[] = {
	1, 1, 1, 1, 1, 1, 1, 1
};

template<class CType> int E3GA<CType>::piDualTable[] = {
	-8, -5, -6, -7, 2, 3, 4, 1
};

