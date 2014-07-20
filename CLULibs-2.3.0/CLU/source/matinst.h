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


#ifndef _MATINST_HH_
#define _MATINST_HH_

#define Instantiate_Matrix(TYPE)\
\
template class Matrix<TYPE>;\
\
template int operator==(const Matrix<TYPE> &a, const Matrix<TYPE> &b);\
template int operator!=(const Matrix<TYPE> &a, const Matrix<TYPE> &b);\
\
template Matrix<TYPE> operator| (const Matrix<TYPE> &a, const Matrix<TYPE> &b);\
template Matrix<TYPE>& operator|= (Matrix<TYPE> &a, const Matrix<TYPE> &b);\
template Matrix<TYPE> operator| (const Matrix<TYPE> &a, const TYPE naug[]);\
template Matrix<TYPE>& operator|= (Matrix<TYPE> &a, const TYPE naug[]);\
\
template Matrix<TYPE> operator~ (const Matrix<TYPE> &a);\
template Matrix<TYPE> operator! (const Matrix<TYPE> &a);\
template Matrix<TYPE> operator- (const Matrix<TYPE> &a);\
\
template Matrix<TYPE> operator+ (const Matrix<TYPE> &a, const Matrix<TYPE> &b);\
template Matrix<TYPE>& operator+= (Matrix<TYPE> &a, const Matrix<TYPE> &b);\
template Matrix<TYPE> operator+ (const Matrix<TYPE> &a, const TYPE &b);\
template Matrix<TYPE>& operator+= (Matrix<TYPE> &a, const TYPE &b);\
template Matrix<TYPE> operator- (const Matrix<TYPE> &a, const Matrix<TYPE> &b);\
template Matrix<TYPE>& operator-= (Matrix<TYPE> &a, const Matrix<TYPE> &b);\
template Matrix<TYPE> operator- (const Matrix<TYPE> &a, const TYPE &b);\
template Matrix<TYPE>& operator-= (Matrix<TYPE> &a, const TYPE &b);\
\
template Matrix<TYPE> operator* (const Matrix<TYPE> &a, const TYPE &b);\
template Matrix<TYPE> operator* (TYPE &b, const Matrix<TYPE> &a);\
template Matrix<TYPE> operator*(const Matrix<TYPE> &a, const Matrix<TYPE> &b);\
template Matrix<TYPE>& operator*= (Matrix<TYPE> &a, const Matrix<TYPE> &b);\
template Matrix<TYPE> LUSolve(const Matrix<TYPE> &a);\
template Matrix<TYPE> Solve(const Matrix<TYPE> &a);\
template TYPE LUDet(const Matrix<TYPE> &a);\
template TYPE Det(const Matrix<TYPE> &a);\
\
template void EigenOrder(Matrix<TYPE> &eval, Matrix<TYPE> &evec);\
\
template void printMat(Matrix<TYPE> &a, const char *title, uint augcol);\
\
template void SVDOrder(Matrix<TYPE> &w, Matrix<TYPE> &u, Matrix<TYPE> &v, bool bDescending);\
\
template bool EigenValues(Matrix<TYPE> &xMat, Matrix<TYPE> &xREVals, Matrix<TYPE> &xIEVals);\
\
template bool Eigen(Matrix<TYPE> &xMat, Matrix<TYPE> &xREVals, Matrix<TYPE> &xIEVals, \
					   Matrix<TYPE> &xREVecs, Matrix<TYPE> &xIEVecs); \
\





#endif
