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

// Class Declaration for statistic operations

#ifndef _STATISTIC_H_
#define _STATISTIC_H_

#include "clu.h"

typedef unsigned int uint;

class Statistic
{
  
public:
	Statistic();
	Statistic(const Statistic& S);
	
	Statistic& operator= (const Statistic& S);
	Statistic& operator= (const Mem<double>& S);
	
	Statistic& operator<< (const float& dVal);
	Statistic& operator<< (const double& dVal);
	Statistic& operator<< (const Mem<double>& mVal);

	bool Reset() { return (m_mData.Set(0)); }
	bool SetSize(uint nSize) { return m_mData.Set(nSize); }

	double& operator[] (uint i) { return m_mData[i]; }
	uint Count() { return m_mData.Count(); }
	
	double Min() { return m_dMin; }
	double Max() { return m_dMax; }
	
	double Mean() { return m_dMean; }
	double Variance() { return m_dVariance; }
	double MeanVar() { return m_dMeanVar; }
	
	// Analyze Data
	bool Analyze();
	
	bool EvalMean();
	bool EvalVariance();
	bool EvalMeanVar();

protected:
	Mem<double> m_mData;

	double m_dMean, m_dMin, m_dMax, m_dVariance;
	double m_dMeanVar;
};

#endif

