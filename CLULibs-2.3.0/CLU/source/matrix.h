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


// ------------------------- Matrix -----------------------------
#ifndef _MATRIX_HH_
#define _MATRIX_HH_

#include"mem.h"
#include"makestr.h"
#include"mathelp.h"
#include"cstr.h"


#ifdef _GNUCPP3_
	#ifndef _TMPL_
	#define _TMPL_ <>
	#endif
#	ifndef _FRIEND_
#		define _FRIEND_  friend 
#	endif
#else
	#ifndef _TMPL_
	#define _TMPL_ <>
	#endif
#	ifndef _FRIEND_
#		define _FRIEND_  friend 
#	endif
#endif

#define _MAXSVDITS_ 30   // Maximum Iterations in SVD algorithm

#define MATINV_LU  0x0000  // Use LU to calc. inverse
#define MATINV_SVD 0x0001  // Use SVD ------- " ------
#define MATINV_MAX 0x0001  // Maximal allowed type number

#ifndef _MAXSTRSIZE_
  #define _MAXSTRSIZE_ 1024
#endif


typedef unsigned int uint;


template<class CType> class Matrix;


template<class CType> void printMat (Matrix<CType> &a, const char *title, uint augcol = 0);

template<class CType> void SVDOrder (Matrix<CType> &w, Matrix<CType> &u, Matrix<CType> &v, 
					   bool bDescending = true);

template<class CType> bool EigenValues (Matrix<CType> &xMat, Matrix<CType> &xREVals, Matrix<CType> &xIEVals);
template<class CType> bool Eigen(Matrix<CType> &xMat, Matrix<CType> &xREVals, Matrix<CType> &xIEVals,
								  Matrix<CType> &xREVecs, Matrix<CType> &xIEVecs);

template<class CType> int operator== (const Matrix<CType> &a, const Matrix<CType> &b);
template<class CType> int operator!= (const Matrix<CType> &a, const Matrix<CType> &b);


  // Augmenting a matrix takes the row index into account!
template<class CType> Matrix<CType> operator|  (const Matrix<CType> &a, 
				  const Matrix<CType> &b);
template<class CType> Matrix<CType>& operator|=  (Matrix<CType> &a, 
				    const Matrix<CType> &b);
template<class CType> Matrix<CType> operator|  (const Matrix<CType> &a, 
				  const CType naug[]);
template<class CType> Matrix<CType>& operator|=  (Matrix<CType> &a, 
				    const CType naug[]);


template<class CType> Matrix<CType> LUSolve (const Matrix<CType> &a); 
template<class CType> Matrix<CType> Solve (const Matrix<CType> &a); 
                        
template<class CType> CType LUDet (const Matrix<CType> &a);
template<class CType> CType Det (const Matrix<CType> &a);

  // Finds eigenvectors and eigenvalues of a symmetric matrix
template<class CType> void EigenOrder (Matrix<CType> &eval, Matrix<CType> &evec);

  // Order w in decending order and adjust COLS of v and u.
template<class CType> void SVDOrder (Matrix<CType> &w, Matrix<CType> &u, Matrix<CType> &v, 
					   bool bDescending);


template<class CType> Matrix<CType> operator~  (const Matrix<CType> &a); // Transpose
template<class CType> Matrix<CType> operator!  (const Matrix<CType> &a); // Inverse
template<class CType> Matrix<CType> operator-  (const Matrix<CType> &a); // Negate

template<class CType> Matrix<CType> operator+  (const Matrix<CType> &a, 
				  const Matrix<CType> &b); // Matrix Add.
template<class CType> Matrix<CType>& operator+=  (Matrix<CType> &a, 
				    const Matrix<CType> &b); // Matrix Add.
template<class CType> Matrix<CType> operator+  (const Matrix<CType> &a, 
				  const CType &b); // Add scalar to all Matrix comps.
template<class CType> Matrix<CType>& operator+=  (Matrix<CType> &a, 
				    const CType &b); // Add scalar to all Matrix comps.

template<class CType> Matrix<CType> operator-  (const Matrix<CType> &a, 
				  const Matrix<CType> &b); // Matrix Add.
template<class CType> Matrix<CType>& operator-=  (Matrix<CType> &a, 
				    const Matrix<CType> &b); // Matrix Add.
template<class CType> Matrix<CType> operator-  (const Matrix<CType> &a, 
				  const CType &b); // Add scalar to all Matrix comps.
template<class CType> Matrix<CType>& operator-=  (Matrix<CType> &a, 
				    const CType &b); // Add scalar to all Matrix comps.

template<class CType> Matrix<CType> operator*  (const Matrix<CType> &a, const CType &b);
template<class CType> Matrix<CType> operator*  (CType &b, const Matrix<CType> &a);
template<class CType> Matrix<CType> operator*  (const Matrix<CType> &a, 
				  const Matrix<CType> &b); // Matrix Mul.

template<class CType> Matrix<CType>& operator*= (Matrix<CType> &a, 
				    const Matrix<CType> &b); // Matrix Mul.


//////////////////////////////////////////////////////////////////

template<class CType>
class Matrix
{
public:
  Matrix();
  Matrix(uint nrows, uint ncols, const CType nmat[] = 0);
  Matrix(const Matrix<CType> &a);
  ~Matrix();

  Matrix<CType>& operator= (const Matrix<CType> &a);
  Matrix<CType>& operator= (const CType nmat[]);
  Matrix<CType>& operator= (const CType val); // Sets all comps. to val.
  CType& operator() (uint row, uint col);
  CType operator() (uint row, uint col) const;

  _FRIEND_ int operator== _TMPL_(const Matrix<CType> &a, const Matrix<CType> &b);
  _FRIEND_ int operator!= _TMPL_(const Matrix<CType> &a, const Matrix<CType> &b);


  // Augmenting a matrix takes the row index into account!
  _FRIEND_ Matrix<CType> operator|  _TMPL_(const Matrix<CType> &a, 
				  const Matrix<CType> &b);
  _FRIEND_ Matrix<CType>& operator|=  _TMPL_(Matrix<CType> &a, 
				    const Matrix<CType> &b);
  _FRIEND_ Matrix<CType> operator|  _TMPL_(const Matrix<CType> &a, 
				  const CType naug[]);
  _FRIEND_ Matrix<CType>& operator|=  _TMPL_(Matrix<CType> &a, 
				    const CType naug[]);

  uint AddRows(uint dest, uint src, const Matrix<CType> &a, uint no = 1);
  uint AddCols(uint dest, uint src, const Matrix<CType> &a, uint no = 1);

  Matrix<CType> SubRows(uint row, uint no = 1);
  Matrix<CType> SubCols(uint row, uint no = 1);

  uint Resize(uint nrows, uint ncols);
  // Keep number of elements but change values for rows and cols.
  uint Reshape(uint nrows, uint ncols);
  uint Rows() const { return rows; }
  uint Cols() const { return cols; }
  uint isOK();

  uint InvType() { return invtype; }
  void InvType(uint ninvtype) 
       { if (ninvtype <=  MATINV_MAX)
	   invtype = ninvtype; 
       }

  void ResetRowIdx();
  void ResetColIdx();

  uint RowIdx(uint row);
  uint ColIdx(uint col);
  int RowPar() { return rowpar; }
  int ColPar() { return colpar; }

	bool GetSubMatrix(Matrix<CType> &rMat, int iTRow, int iLCol, int iRowNo, int iColNo);

  void GetRow(uint row, CType nmat[]);
  void SetRow(uint row, CType nmat[]);
  void GetCol(uint col, CType nmat[]);
  void SetCol(uint col, CType nmat[]);

  void SwapRows(uint row1, uint row2); // Also swaps elements in aug
  void SwapCols(uint col1, uint col2);

  void MulRow(uint row, const CType &a); // Also multiplies augmented column
  void MulCol(uint col, const CType &a);

  void Identity(); // Make Matrix Identity MAtrix
  void Diagonal(); // Takes Row or Col vector as diagonal of new matrix
  void Trace();    // Makes Row vector from Trace of Matrix

  uint LUDecomp();    // LU Decomposition as described in Numerical Recipes
                      // Only works for square matrices
  Matrix<CType> Left(); // Obtain left part of LU Decomposed Matrix
  Matrix<CType> Upper(); // Obtain upper part of LU Decomposed Matrix

  Matrix<CType> LUSolve(); // Solves Already decomposed Matrix
  Matrix<CType> Solve(); // Uses Augmented Column to Solve Matrix as system of 
                         // coupled linear equations
  CType LUDet(); // Determinant of LU decomposed Matrix
  CType Det(); // Calculates Determinant

  /*
  _FRIEND_ Matrix<CType> LUSolve _TMPL_(const Matrix<CType> &a); 
  _FRIEND_ Matrix<CType> Solve _TMPL_(const Matrix<CType> &a); 
                        
  _FRIEND_ CType LUDet _TMPL_(const Matrix<CType> &a);
  _FRIEND_ CType Det _TMPL_(const Matrix<CType> &a);
*/
  // Finds eigenvectors and eigenvalues of a symmetric matrix
  uint Jacobi(Matrix<CType> &eval, Matrix<CType> &evec);
/*
  _FRIEND_ void EigenOrder _TMPL_(Matrix<CType> &eval, Matrix<CType> &evec);

  // Evaluate Eigenvalues of arbitrary matrix. 
  // Returns row vectors of real and imaginary part of eigenvalues.
  _FRIEND_ bool EigenValues _TMPL_(Matrix<CType> &xMat, Matrix<CType> &xREVals, Matrix<CType> &xIEVals);

	// Evaluate Eigenvectors and Eigenvalues of arbitrary matrix
  _FRIEND_ bool Eigen _TMPL_(Matrix<CType> &xMat, Matrix<CType> &xREVals, Matrix<CType> &xIEVals,
					   Matrix<CType> &xREVecs, Matrix<CType> &xIEVecs);
*/
  // Computing Singular Value Decomposition
  int SVD(Matrix<CType> &u, Matrix<CType> &w, Matrix<CType> &v); 
  // Order w in decending order and adjust COLS of v and u.
/*  _FRIEND_ void SVDOrder _TMPL_(Matrix<CType> &w, Matrix<CType> &u, Matrix<CType> &v, 
					   bool bDescending);


  _FRIEND_ Matrix<CType> operator~  _TMPL_(const Matrix<CType> &a); // Transpose
  _FRIEND_ Matrix<CType> operator!  _TMPL_(const Matrix<CType> &a); // Inverse
  _FRIEND_ Matrix<CType> operator-  _TMPL_(const Matrix<CType> &a); // Negate

  _FRIEND_ Matrix<CType> operator+  _TMPL_(const Matrix<CType> &a, 
				  const Matrix<CType> &b); // Matrix Add.
  _FRIEND_ Matrix<CType>& operator+=  _TMPL_(Matrix<CType> &a, 
				    const Matrix<CType> &b); // Matrix Add.

  _FRIEND_ Matrix<CType> operator+  _TMPL_(const Matrix<CType> &a, 
				  const CType &b); // Add scalar to all Matrix comps.
  _FRIEND_ Matrix<CType>& operator+=  _TMPL_(Matrix<CType> &a, 
				    const CType &b); // Add scalar to all Matrix comps.

  _FRIEND_ Matrix<CType> operator-  _TMPL_(const Matrix<CType> &a, 
				  const Matrix<CType> &b); // Matrix Add.
  _FRIEND_ Matrix<CType>& operator-=  _TMPL_(Matrix<CType> &a, 
				    const Matrix<CType> &b); // Matrix Add.

  _FRIEND_ Matrix<CType> operator-  _TMPL_(const Matrix<CType> &a, 
				  const CType &b); // Add scalar to all Matrix comps.
  _FRIEND_ Matrix<CType>& operator-=  _TMPL_(Matrix<CType> &a, 
				    const CType &b); // Add scalar to all Matrix comps.

  _FRIEND_ Matrix<CType> operator*  _TMPL_(const Matrix<CType> &a, const CType &b);
  _FRIEND_ Matrix<CType> operator*  _TMPL_(CType &b, const Matrix<CType> &a);
  _FRIEND_ Matrix<CType> operator*  _TMPL_(const Matrix<CType> &a, 
				  const Matrix<CType> &b); // Matrix Mul.
  _FRIEND_ Matrix<CType>& operator*=  _TMPL_(Matrix<CType> &a, 
				    const Matrix<CType> &b); // Matrix Mul.
*/
  CType Tiny() { return tiny; }
  void Tiny(CType ntiny) { tiny = ntiny; }
  void TinyToZero(CType fPrec = CType(0));
  Matrix<CType>& Round(CType fPrec = 0);
  // Eval absolute value of all components separately
  bool AbsComps();
  // Eval exponential value of all components separately
  bool ExpComps();
  // Eval exponential value of all components separately
  bool LogComps();
  // Eval power value of all components separately
  bool PowComps(CType dPow, CType fPrec = 0);
  // Eval square root of all components separately. Negative values are set to zero.
  bool SqrtComps(CType fPrec = 0);
  // Invert components of matrix. Zero values are set to inf.
  bool InvComps(CType inf = CType(0), CType fPrec = 0); 
  Matrix<CType> InvSVD(CType fPrec = CType(0)); // Inverse of this matrix using SVD

  // Eval sin of matrix components                                      
  bool SinComps();
  // Eval cos of matrix components                                      
  bool CosComps();
  // Eval tan of matrix components                                      
  bool TanComps();

  // Eval asin of matrix components                                      
  bool ArcSinComps(CType fPrec = 0);
  // Eval acos of matrix components                                      
  bool ArcCosComps(CType fPrec = 0);
  // Eval atan of matrix components                                      
  bool ArcTanComps();


  void FindColWidth();
  char* Str(uint row, uint augrow = 0);
  //_FRIEND_ void printMat _TMPL_(Matrix<CType> &a, const char *title, uint augcol);

  CType* Data() { return mat->Data(); }
  const CType* Data() const { return mat->Data(); }

//protected:
  cStr str;
  uint colwidth, augcolwidth;

  Mem<CType> *mat;   // Matrix components
  Mem<CType> *aug;   // Augmented Column
  uint rows, cols;   // x and y dimensions of Matrix

  Mem<uint> *rowidx;      // Keeps track of row interchanges
  Mem<uint> *colidx;      // Keeps track of column interchanges
  int rowpar;        // -1 if odd number of row interchanges, else 1
  int colpar;        // -1 if odd number of column interchanges, else 1

  CType tiny;

  uint invtype;     // Sets way inverse is calculated

  Matrix<CType> LUBackSub();   // Backsubstitution of LU decomposed Matrix
  Matrix<CType>& Order();
  Matrix<CType> InvLU();  // Inverse of this matrix using LU

  CType pythag(CType a, CType b); // Calc sqrt(a^2 + b^2)

  inline CType sign(CType a, CType b)
        { return ( (b >= CType(0)) ? Mag(a) : -Mag(a) ); }

  inline CType maxi(CType a, CType b) 
        { return ( (a >= b) ? a : b ); }


};

#endif
