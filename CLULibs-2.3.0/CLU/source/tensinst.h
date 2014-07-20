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


// Instantiating Tensor Class

//template Tensor<TYPE>& operator, (Tensor<TYPE>& t, uint pos);
//template Tensor<TYPE>& operator, (Tensor<TYPE>& t, uint* idx);

#define Instantiate_Tensor(TYPE)\
\
template class Tensor<TYPE>;\
\
template Tensor<TYPE> operator- (const Tensor<TYPE>& t);\
\
template Tensor<TYPE> operator* (const Tensor<TYPE>& t1, const Tensor<TYPE>& t2);\
template Tensor<TYPE> operator*(const Tensor<TYPE>& t1, const TYPE a);\
template Tensor<TYPE> operator*(const TYPE a, const Tensor<TYPE>& t1);\
template Tensor<TYPE>& operator*=(Tensor<TYPE>& t1, const Tensor<TYPE>& t2);\
template Tensor<TYPE>& operator*=(Tensor<TYPE>& t1, const TYPE a);\
\
template Tensor<TYPE> operator& (const Tensor<TYPE>& t1, const Tensor<TYPE>& t2);\
template Tensor<TYPE>& operator&=(Tensor<TYPE>& t1, const Tensor<TYPE>& t2);\
\
template Tensor<TYPE> operator/ (const Tensor<TYPE>& t1, const Tensor<TYPE>& t2);\
template Tensor<TYPE> operator/(const Tensor<TYPE>& t1, const TYPE a);\
template Tensor<TYPE>& operator/=(Tensor<TYPE>& t1, const Tensor<TYPE>& t2);\
template Tensor<TYPE>& operator/=(Tensor<TYPE>& t1, const TYPE a);\
\
template Tensor<TYPE> operator% (const Tensor<TYPE>& t1, const Tensor<TYPE>& t2);\
template Tensor<TYPE>& operator%=(Tensor<TYPE>& t1, const Tensor<TYPE>& t2);\
\
template Tensor<TYPE> operator+ (const Tensor<TYPE>& t1, const Tensor<TYPE>& t2);\
template Tensor<TYPE>& operator+= (Tensor<TYPE>& t1, const Tensor<TYPE>& t2);\
template Tensor<TYPE> operator+(const Tensor<TYPE>& t1, const TYPE& a);\
template Tensor<TYPE>& operator+=(Tensor<TYPE>& t1, const TYPE& a);\
\
template Tensor<TYPE> operator- (const Tensor<TYPE>& t1, const Tensor<TYPE>& t2);\
template Tensor<TYPE>& operator-= (Tensor<TYPE>& t1, const Tensor<TYPE>& t2);\
template Tensor<TYPE> operator-(const Tensor<TYPE>& t1, const TYPE& a);\
template Tensor<TYPE>& operator-=(Tensor<TYPE>& t1, const TYPE& a);\
\
template int InitCLoops(Tensor<TYPE>& nt1, Tensor<TYPE>& nt2, uint apos[], Mem<Tensor<TYPE>::cloopVar>& loops, Tensor<TYPE>& nt0);\
\
template int InitMLoops(Tensor<TYPE>& nt1, Tensor<TYPE>& nt2, uint apos[], Mem<Tensor<TYPE>::cloopVar>& loops, Tensor<TYPE>& nt0);\
\



