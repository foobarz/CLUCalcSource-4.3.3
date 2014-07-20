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


// ------ Matrix Class Library ------
/*
  Although the Matrix class is a template class, it may only be used with
  types that are data only. That is, they may not create memory block by 
  themselves, nor are they allowed to have member functions. However,
  friend functions have to defined for casting from integer, multiplication
  addition, subtraction. comparison, division. Also a function Magnitude has
  to be defined.
*/


#ifndef _MATRIX_TEMPLATE_
#define _MATRIX_TEMPLATE_

#include<string.h>
#include<math.h>
#include<stdio.h>
#include<float.h>

//#include "eispack.h"
#include"matrix.h"
#include"matinst.h"

#ifndef WIN32

#include <math.h>
#define _isnan isnan

#endif

extern "C" 
{
	/* convert matrix to upper Hessenberg form */
	void elmhes(double **a,int n,int lo, int hi,int intch[]);

	/* solve upper Hessenberg matrix for eigenvalues */
	void hqr(int n,int lo,int hi,double **h,double *wr,double *wi,int *ierr);

	/* solve upper Hessenberg matrix for eigenvalues and eigenvectors */
	void hqr2(int n,int lo,int hi,double **h,double *wr, double *wi,double **z,int *ierr);

	/* accumulates stabilized similarity transformations */
	void eltran(int n,int lo,int hi,double **a,int intch[],double **z);

	/* Balance Matrix */
	void balanc(double **a,int n, int *lo, int *hi, double scale[]);

	/* Transform Eigenvectors of Balanced Matrix back to original matrix */
	void balbak(int n,int lo, int hi, double *scale,int m,double **z);

	/* does not normalize eigenvectors */ 
	//void pnormwr(int n,double **z,double *wr,double *wi);
}



// ------------------- Additional Functions ----------------

template<class CType>
void printMat(Matrix<CType> &a, const char *title, uint augcol)
{
  uint r = a.Rows();
  uint len = strlen(title)+2;
  char *buf = new char[len+1];

  memset(buf, ' ', len);
  buf[len] = 0;

  a.FindColWidth();

  for(uint i=1;i<=r;i++)
  {
	  if (i == 1)
	  { 
		  printf("%s: %s\n", title, a.Str(i, augcol));
		  if (r == 1) printf("\n");
	  }
	  else if (i == r)
		  printf("%s%s\n\n", buf, a.Str(i, augcol));
	  else
		  printf("%s%s\n", buf, a.Str(i, augcol));
  }
  
  delete[] buf;
}




// ------------------ Class Functions -------------------------


// Default Constructor
template<class CType>
Matrix<CType>::Matrix()
{
  str = "";
  colwidth = 1;
  augcolwidth = 1;
  tiny = CType(1e-14);
  invtype = MATINV_SVD;

  rows = 0;
  cols = 0;
  mat = new Mem<CType>(0);
  aug = new Mem<CType>(0);

  rowidx = new Mem<uint>(0);
  colidx = new Mem<uint>(0);

  ResetRowIdx();
  ResetColIdx();
}

// Constructor
template<class CType>
Matrix<CType>::Matrix(uint nrows, uint ncols, const CType nmat[])
{
  str = "";
  colwidth = 1;
  augcolwidth = 1;
  tiny = CType(1e-14);
  invtype = MATINV_SVD;

  if (nrows && ncols)
  { rows = nrows;
    cols = ncols;
    mat = new Mem<CType>(rows*cols);
    aug = new Mem<CType>(rows);
    if (mat && nmat)
      memcpy(mat->Data(), nmat, mat->Count()*sizeof(CType));
    else if (mat)
    { uint no = rows*cols;
      CType *data = mat->Data();

      for(uint i=0;i<no;i++)
        data[i] = 0;
    }

    if (aug)
      for(uint i=0;i<rows;i++)
        aug[0][i] = CType(0);

  }
  else
  { rows = 0;
    cols = 0;
    mat = new Mem<CType>(0);
    aug = new Mem<CType>(0);
  }

  rowidx = new Mem<uint>(rows);
  colidx = new Mem<uint>(cols);

  ResetRowIdx();
  ResetColIdx();
}


// Copy Constructor
template<class CType>
Matrix<CType>::Matrix(const Matrix<CType> &a)
{
  colwidth = a.colwidth;
  augcolwidth = a.augcolwidth;
  tiny = a.tiny;
  invtype = a.invtype;

  rows = a.rows;
  cols = a.cols;
  rowpar = a.rowpar;
  colpar = a.colpar;

  mat = new Mem<CType>(a.mat[0]);
  aug = new Mem<CType>(a.aug[0]);
  rowidx = new Mem<uint>(a.rowidx[0]);
  colidx = new Mem<uint>(a.colidx[0]);
}

// Destructor
template<class CType>
Matrix<CType>::~Matrix()
{
  if (mat) delete mat;
  if (aug) delete aug;
  if (rowidx) delete rowidx;
  if (colidx) delete colidx;
}


// Assignment of Matrix
template<class CType>
Matrix<CType>& Matrix<CType>::operator= (const Matrix<CType> &a)
{
  if (a.rows && a.cols)
  { str = "";
    colwidth = a.colwidth;
    augcolwidth = a.augcolwidth;
    tiny = a.tiny;
    invtype = a.invtype;
    rows = a.rows;
    cols = a.cols;
    mat[0] = a.mat[0];
    aug[0] = a.aug[0];
    rowidx[0] = a.rowidx[0];
    colidx[0] = a.colidx[0];
    rowpar = a.rowpar;
    colpar = a.colpar;  
  }

  return this[0];
}


// Assignment of Matrix
template<class CType>
Matrix<CType>& Matrix<CType>::operator= (const CType nmat[])
{
  if (nmat) memcpy(mat->Data(), nmat, mat->Count()*sizeof(CType));
  else 
  { uint no = rows*cols;
    CType *data = mat->Data();

    for(uint i=0;i<no;i++)
      data[i] = 0;
  }

  return this[0];
}


// Assignment of value to all comps of Matrix
template<class CType>
Matrix<CType>& Matrix<CType>::operator= (const CType val)
{
  CType *data;

  if((data = mat->Data()))
  { uint no = rows*cols;

    for(uint i=0;i<no;i++)
      data[i] = val;
  }

  return this[0];
}


// Accessing component of Matrix
template<class CType>
CType& Matrix<CType>::operator() (uint row, uint col)
{
  if (row && col && row <= rows && col <= cols)
    return mat->Data()[(row-1)*cols + col-1];

  return mat[0][0];
}
 
// Accessing component of Matrix
template<class CType>
CType Matrix<CType>::operator() (uint row, uint col) const
{
	if (row && col && row <= rows && col <= cols)
		return mat->Data()[(row-1)*cols + col-1];

	return mat[0][0];
}



// Comparing Matrices ==
template<class CType>
int operator==(const Matrix<CType> &a, const Matrix<CType> &b)
{
  if (a.rows != b.rows || a.cols != b.cols) return 0;

  CType *adata = a.mat->Data();
  CType *bdata = b.mat->Data();
  uint size = a.rows*a.cols;

  for(uint i=0;i<size;i++)
    if (adata[i] != bdata[i]) return 0;

  return 1;
}



// Comparing Matrices !=
template<class CType>
int operator!=(const Matrix<CType> &a, const Matrix<CType> &b)
{
  if (a.rows != b.rows || a.cols != b.cols) return 1;

  CType *adata = a.mat->Data();
  CType *bdata = b.mat->Data();
  uint size = a.rows*a.cols;

  for(uint i=0;i<size;i++)
    if (adata[i] != bdata[i]) return 1;

  return 0;
}


// Checking Matrix
template<class CType>
uint Matrix<CType>::isOK()
{
  if (!rows || !cols || !mat || !aug || !rowidx || !colidx) return 0;

  uint size = rows*cols;

  if (mat->Count() != size) return 0;

  if (aug->Count() != rows) return 0;

  if (rowidx->Count() != rows) return 0;

  if (colidx->Count() != cols) return 0;

  return 1;
}


// Set Tiny elements to zero
template<class CType>
void Matrix<CType>::TinyToZero(CType fPrec)
{
  if (!rows || !cols) return;

  CType *data = mat->Data();
  CType big = CType(0);
  CType h;
  uint size = rows*cols;
  uint i;

  if (fPrec <= 0)
	  fPrec = tiny;

  for(i=0;i<size;i++)
    if ((h = Mag(data[i])) > big) big = h;

  big *= fPrec;

  for(i=0;i<size;i++)
    if (Mag(data[i]) <= big) data[i] = CType(0);
}


/////////////////////////////////////////////////////
/// Round elements of Matrix
  
template<class CType>
Matrix<CType>& Matrix<CType>::Round(CType fPrec)
{
	if (!rows || !cols) return *this;
	
	CType *data = mat->Data();
	CType fTiny;
	uint size = rows*cols;
	uint i;
	
	if (fPrec == CType(0))
		fPrec = ::Tiny(fTiny);

	for(i=0;i<size;i++)
		data[i] = ::Round((CType) data[i], (CType) fPrec);

	return *this;
}

/////////////////////////////////////////////////////
/// Take absolute value of elements of Matrix
  
template<class CType>
bool Matrix<CType>::AbsComps()
{
	if (!rows || !cols) 
		return true;
	
	CType *data = mat->Data();
	uint size = rows*cols;
	uint i;
	
	for(i=0;i<size;i++)
		data[i] = Mag(data[i]);

	return true;
}


/////////////////////////////////////////////////////
/// Take exponential value of elements of Matrix
  
template<class CType>
bool Matrix<CType>::ExpComps()
{
	if (!rows || !cols) 
		return true;
	
	CType *data = mat->Data();
	uint size = rows*cols;
	uint i;
	
	for(i=0;i<size;i++)
		data[i] = CType(exp(double(data[i])));

	return true;
}


/////////////////////////////////////////////////////
/// Take logarithmic value of elements of Matrix
  
template<class CType>
bool Matrix<CType>::LogComps()
{
	if (!rows || !cols) 
		return true;
	
	CType dVal;
	CType *data = mat->Data();
	uint size = rows*cols;
	uint i;
	
	for(i=0;i<size;i++)
	{
		dVal = CType(log(double(data[i])));
		if (_isnan(double(dVal)))
			return false;
		data[i] = dVal;
	}

	return true;
}


/////////////////////////////////////////////////////
/// Take square root of elements of Matrix
  
template<class CType>
bool Matrix<CType>::SqrtComps(CType fPrec)
{
	if (!rows || !cols) 
		return true;
	
	CType *data = mat->Data();
	uint size = rows*cols;
	uint i;
	double dVal;

	CType fTiny;
	if (fPrec == CType(0))
		fPrec = ::Tiny(fTiny);

	for(i=0;i<size;i++)
	{
		dVal = double(data[i]);
		if (dVal < 0.0 && dVal >= -fPrec)
			data[i] = CType(0);
		else if (dVal >= 0.0)
			data[i] = CType(sqrt(dVal));
		else
			return false;
	}

	return true;
}


/////////////////////////////////////////////////////
/// Take power value of elements of Matrix
  
template<class CType>
bool Matrix<CType>::PowComps(CType dVal, CType fPrec)
{
	if (!rows || !cols) 
		return true;
	
	CType fTiny;
	if (fPrec == CType(0))
		fPrec = ::Tiny(fTiny);

	CType dRes;
	CType *data = mat->Data();
	uint size = rows*cols;
	uint i;
	
	for(i=0;i<size;i++)
	{
		if ((data[i] > -fPrec && data[i] < fPrec) && dVal < CType(0))
			return false;

		dRes = CType(pow(double(data[i]), double(dVal)));
		if ( _isnan(double(dRes)) )
			return false;

		data[i] = dRes;
	}

	return true;
}


/////////////////////////////////////////////////////
/// Take sin value of elements of Matrix
  
template<class CType>
bool Matrix<CType>::SinComps()
{
	if (!rows || !cols) return true;
	
	CType *data = mat->Data();
	uint size = rows*cols;
	uint i;
	
	for(i=0;i<size;i++)
		data[i] = CType(sin(double(data[i])));

	return true;
}


/////////////////////////////////////////////////////
/// Take cos value of elements of Matrix
  
template<class CType>
bool Matrix<CType>::CosComps()
{
	if (!rows || !cols) return true;
	
	CType *data = mat->Data();
	uint size = rows*cols;
	uint i;
	
	for(i=0;i<size;i++)
		data[i] = CType(cos(double(data[i])));

	return true;
}

/////////////////////////////////////////////////////
/// Take tan value of elements of Matrix
  
template<class CType>
bool Matrix<CType>::TanComps()
{
	if (!rows || !cols) return true;
	
	CType *data = mat->Data();
	uint size = rows*cols;
	uint i;
	
	for(i=0;i<size;i++)
		data[i] = CType(tan(double(data[i])));

	return true;
}



/////////////////////////////////////////////////////
/// Take asin value of elements of Matrix
  
template<class CType>
bool Matrix<CType>::ArcSinComps(CType fPrec)
{
	if (!rows || !cols) 
		return true;
	
	CType fTiny;
	if (fPrec == CType(0))
		fPrec = ::Tiny(fTiny);

	CType dVal;
	CType *data = mat->Data();
	uint size = rows*cols;
	uint i;
	
	for(i=0;i<size;i++)
	{
		dVal = data[i];
		
		if (dVal < CType(-1.0) && dVal >= CType(-1.0) - fPrec)
			dVal = CType(-1.0);
		else if (dVal < CType(-1.0))
			return false;

		if (dVal > CType(1.0) && dVal <= CType(1.0) + fPrec)
			dVal = CType(1.0);
		else if (dVal > CType(1.0))
			return false;

		data[i] = CType(asin(double(dVal)));
	}

	return true;
}


/////////////////////////////////////////////////////
/// Take acos value of elements of Matrix
  
template<class CType>
bool Matrix<CType>::ArcCosComps(CType fPrec)
{
	if (!rows || !cols) 
		return true;
	
	CType fTiny;
	if (fPrec == CType(0))
		fPrec = ::Tiny(fTiny);

	CType dVal;
	CType *data = mat->Data();
	uint size = rows*cols;
	uint i;
	
	for(i=0;i<size;i++)
	{
		dVal = data[i];
		
		if (dVal < CType(-1.0) && dVal >= CType(-1.0) - fPrec)
			dVal = CType(-1.0);
		else if (dVal < CType(-1.0))
			return false;

		if (dVal > CType(1.0) && dVal <= CType(1.0) + fPrec)
			dVal = CType(1.0);
		else if (dVal > CType(1.0))
			return false;

		data[i] = CType(acos(double(dVal)));
	}

	return true;
}

/////////////////////////////////////////////////////
/// Take atan value of elements of Matrix
  
template<class CType>
bool Matrix<CType>::ArcTanComps()
{
	if (!rows || !cols) 
		return true;
	
	CType *data = mat->Data();
	uint size = rows*cols;
	uint i;
	
	for(i=0;i<size;i++)
		data[i] = CType(atan(double(data[i])));

	return true;
}




// Augmenting Matrix
template<class CType>
Matrix<CType> operator| (const Matrix<CType> &a, const Matrix<CType> &b)
{
  if (a.rows != b.rows) return a;

  Matrix<CType> c(a);
  CType *data = b.mat->Data();
  CType *adata = c.aug->Data();

  for(uint r=0;r<c.rows;r++)
    adata[r] = data[c.rowidx[0][r]*b.cols];

  return c;
}

// Augmenting this Matrix
template<class CType>
Matrix<CType>& operator|= (Matrix<CType> &a, const Matrix<CType> &b)
{
  if (a.rows != b.rows) return a;

  CType *data = b.mat->Data();
  CType *adata = a.aug->Data();

  for(uint r=0;r<a.rows;r++)
    adata[r] = data[a.rowidx[0][r]*b.cols];

  return a;
}

// Augmenting Matrix
template<class CType>
Matrix<CType> operator| (const Matrix<CType> &a, const CType naug[])
{
  Matrix<CType> c(a);
  CType *adata = c.aug->Data();

  for(uint r=0;r<c.rows;r++)
    adata[r] = naug[c.rowidx[0][r]];

  return c;
}


// Augmenting this Matrix
template<class CType>
Matrix<CType>& operator|= (Matrix<CType> &a, const CType naug[])
{
  CType *adata = a.aug->Data();

  for(uint r=0;r<a.rows;r++)
    adata[r] = naug[a.rowidx[0][r]];

  return a;
}



// Resize Matrix
template<class CType>
uint Matrix<CType>::Resize(uint nrows, uint ncols)
{
  if (!nrows || !ncols) return 0;

  Mem<CType> *nmat = new Mem<CType>(nrows*ncols);
  Mem<CType> *naug = new Mem<CType>(nrows);
  if (!nmat) 
  { if (naug) delete naug;
    return 0;
  }
  if (!naug)
  { delete nmat;
    return 0;
  }
  if (!nmat->Data() || !naug->Data())
  { delete nmat;
    delete naug;
    return 0;
  } 
   
  uint i, size = nrows*ncols;

  for(i=0;i<size;i++)
    nmat[0][i] = CType(0);

  for(i=0;i<nrows;i++)
    naug[0][i] = CType(0);

  if (rows && cols)
  { CType *data = mat->Data();
    CType *ndata = nmat->Data();
    CType *adata = aug->Data();
    CType *nadata = naug->Data();
    uint xr = ( (rows > nrows) ? (nrows) : (rows) );
    uint xc = ( (cols > ncols) ? (ncols) : (cols) );
    uint r,c,rp,nrp;

    for(r=0;r<xr;r++)
    { nadata[r] = adata[r]; 
      for(c=0,rp=r*cols,nrp=r*ncols;c<xc;c++)
	ndata[nrp+c] = data[rp+c];
    }
  }

  delete mat;
  delete aug;
  mat = nmat;
  aug = naug;
  
  rows = nrows;
  cols = ncols;

  rowidx[0] = rows;
  colidx[0] = cols;

  ResetRowIdx();
  ResetColIdx();

  return 1;
}


// Reshape Matrix
template<class CType>
uint Matrix<CType>::Reshape(uint nrows, uint ncols)
{
  if (!nrows || !ncols ||
	  nrows*ncols != rows*cols) 
	  return 0;

  rows = nrows;
  cols = ncols;

  rowidx[0] = rows;
  colidx[0] = cols;

  ResetRowIdx();
  ResetColIdx();

  return 1;
}



// Reset Row Index
template<class CType>
void Matrix<CType>::ResetRowIdx()
{
  if (rowidx && rows)
  { uint *ri = rowidx->Data();

    for(uint i=0;i<rows;i++)
      ri[i] = i;

    rowpar = 1;
  }
}



// Reset Column Index
template<class CType>
void Matrix<CType>::ResetColIdx()
{
  if (colidx && cols)
  { uint *ci = colidx->Data();

    for(uint i=0;i<cols;i++)
      ci[i] = i;

    colpar = 1;
  }
}


// Return Row Index
template<class CType>
uint Matrix<CType>::RowIdx(uint row)
{
  if (!rowidx || !row || row > rows) return 0;

  return (rowidx[0][row-1]+1);
}


// Return Column Index
template<class CType>
uint Matrix<CType>::ColIdx(uint col)
{
  if (!colidx || !col || col > cols) return 0;

  return (colidx[0][col-1]+1);
}








// Get Row of Matrix
template<class CType>
void Matrix<CType>::GetRow(uint row, CType nmat[])
{
  if (row > rows) return;

  CType *data = mat->Data();

  memcpy(nmat, &data[(row-1)*cols], cols*sizeof(CType));
}


// Set Row of Matrix
template<class CType>
void Matrix<CType>::SetRow(uint row, CType nmat[])
{
  if (row > rows) return;

  CType *data = mat->Data();

  memcpy(&data[(row-1)*cols], nmat, cols*sizeof(CType));
}


// Get Column of Matrix
template<class CType>
void Matrix<CType>::GetCol(uint col, CType nmat[])
{
  if (!col || col > cols) return;

  CType *data = mat->Data();

  for(uint i=0;i<rows;i++)
    nmat[i] = data[i*cols + col-1];
}


// Set Column of Matrix
template<class CType>
void Matrix<CType>::SetCol(uint col, CType nmat[])
{
  if (!col || col > cols) return;

  CType *data = mat->Data();

  for(uint i=0;i<rows;i++)
    data[i*cols + col-1] = nmat[i];
}


// Swap Rows of Matrix
template<class CType>
void Matrix<CType>::SwapRows(uint row1, uint row2)
{
  if (row1 == row2 || !row1 || !row2 
      || row1 > rows || row2 > rows) return;

  uint size = cols*sizeof(CType);
  char *srow = new char[size];
  CType *data = mat->Data();
  CType dum;

  row1--; row2--;

  memcpy(srow, &data[row1*cols], size);
  memmove(&data[row1*cols], &data[row2*cols], size);
  memcpy(&data[row2*cols], srow, size);

  dum = aug[0][row1];
  aug[0][row1] = aug[0][row2];
  aug[0][row2] = dum;

  uint s = rowidx[0][row1];
  rowidx[0][row1] = rowidx[0][row2];
  rowidx[0][row2] = s;

  rowpar = -rowpar;
}



// Swap Columns of Matrix
template<class CType>
void Matrix<CType>::SwapCols(uint col1, uint col2)
{
  if (col1 == col2 || !col1 || !col2
      || col1 > cols || col2 > cols) return;

  CType s;
  CType *data = mat->Data();

  col1--; col2--;

  for(uint i=0;i<rows;i++)
  { s = data[i*cols + col1];
    data[i*cols+col1] = data[i*cols+col2];
    data[i*cols+col2] = s;
  }

  uint swap = colidx[0][col1];
  colidx[0][col1] = colidx[0][col2];
  colidx[0][col2] = swap;

  colpar = -colpar;
}


// Multiply a Row - including augmented element - with a constant
template<class CType>
void Matrix<CType>::MulRow(uint row, const CType &a)
{
  if (!row || row > rows) return; 

  CType *data = mat->Data();
  uint rp = (row-1)*cols;

  aug[0][row-1] *= a;

  for(uint i=0;i<cols;i++)
    data[rp+i] *= a;
}


// Multiply a Column with a constant
template<class CType>
void Matrix<CType>::MulCol(uint col, const CType &a)
{
  if (!col || col > cols) return; 

  CType *data = mat->Data();

  for(uint i=0;i<rows;i++)
    data[i*cols+col-1] *= a;
}



// Identity Matrix
template<class CType>
void Matrix<CType>::Identity()
{
	if (rows != cols) return;

	CType *data = mat->Data();
	uint i,j;
	uint size = rows*cols;

	for(i=0, j=0; i < size; i++)
	{
		if (i == j) 
		{ 
			data[i] = CType(1);
			j += cols+1;
		}
		else 
		{
			data[i] = CType(0);
		}
	}
}

// Transpose Matrix
template<class CType>
Matrix<CType> operator~ (const Matrix<CType> &a)
{
  Matrix<CType> b;

  if (!a.rows || !a.cols) return b;

  b.Resize(a.cols, a.rows);

  if (a.rows == 1 || a.cols == 1)
  { b.mat[0] = a.mat[0];
  }
  else 
  { CType *adata = a.mat->Data();
    CType *bdata = b.mat->Data();
    uint i,j,rp;
 
    for(i=0;i<a.rows;i++)
      for(j=0,rp=i*a.cols;j<a.cols;j++)
        bdata[j*b.cols+i] = adata[rp+j];

  }

  return b;
}


// Left Part of Matrix
template<class CType>
Matrix<CType> Matrix<CType>::Left()
{
  Matrix<CType> a(rows, cols);

  if (!rows || !cols || rows != cols) return a;

  CType *data = mat->Data();
  CType *adata = a.mat->Data();
  uint r,c,rp;

  for(r=1;r<rows;r++)
    for(c=0,rp=r*cols;c<r;c++)
      adata[rp+c] = data[rp+c];

  return a;
}


// Upper Part of Matrix
template<class CType>
Matrix<CType> Matrix<CType>::Upper()
{
  Matrix<CType> a(rows, cols);

  if (!rows || !cols || rows != cols) return a;

  CType *data = mat->Data();
  CType *adata = a.mat->Data();
  uint r,c,rp;

  for(r=0;r<rows;r++)
    for(c=r,rp=r*cols;c<rows;c++)
      adata[rp+c] = data[rp+c];

  return a;
}




// LU Decomposition
template<class CType>
uint Matrix<CType>::LUDecomp()
{
  if (rows != cols) return 0;

  CType *data = mat->Data();
  CType *impscal = new CType[rows]; // Keeps track of implicit scaling of rows
  CType big; // Biggest element in row
  CType dum; // Dummy Variable
  CType sum; // Used for summation
  uint bestrow = 0; // Row with largest figure of merit

  uint r,c,rp,i; 

  for(r=0;r<rows;r++)
  { big = CType(0);
    rp=r*cols;
    
    for(c=0;c<cols;c++)
    { dum = (CType) Mag(data[rp+c]);
      if (dum > big) big = dum;
    }  

    if (big == CType(0)) 
	{ delete[] impscal;
      return 0;
	}

    impscal[r] = CType(1) / big;
  }

  for(c=0;c<cols;c++)
  { // printf("Loop 1:----- \n\n");
    for(r=0;r<c;r++)
    { rp = r*cols;
      sum = data[rp+c];
      for(i=0;i<r;i++)
        sum -= data[rp+i] * data[i*cols+c];
      data[rp+c] = sum;
      //      printf("rp: %u, c: %u, sum: %lf\n", rp, c, sum);
    }

    //    printf("Loop 2:----- \n\n");
    big = CType(0);
    for(r=c;r<rows;r++)
    { rp = r*cols;
      sum = data[rp+c];
      for(i=0;i<c;i++)
        sum -= data[rp+i] * data[i*cols+c];
      data[rp+c] = sum;

      //      printf("\nrp: %u, c: %u, sum: %lf\n", rp, c, sum);

      dum = impscal[r] * Mag(sum);
      if (dum >= big)
      { big = dum;
        bestrow = r;
      }
      //      printf("c: %d, r: %d\n", c, r);
      //      printf("sum: %lf, dum: %lf, big: %lf, bestrow: %u\n\n",
      //	     sum, dum, big, bestrow);
    }

    if (c != bestrow)
    { SwapRows(c+1, bestrow+1);
      dum = impscal[bestrow];
      impscal[bestrow] = impscal[c];
      impscal[c] = dum;
    }
   
    if (data[c*cols+c] == CType(0)) // May want to set element to
	{ delete[] impscal;				// a tiny element and carry on
	  return 0;						
	} 

    if (c+1 < cols)
    { dum = CType(1) / data[c*cols+c];
      for(r=c+1;r<rows;r++)
	data[r*cols+c] *= dum;
    }
  } // Do the same for next column

  delete[] impscal;
  return 1;
}


// Backsubstitution of LU decomposed MAtrix
template<class CType>
Matrix<CType> Matrix<CType>::LUBackSub()
{
  Matrix<CType> X(rows, 1, aug->Data());  // Resultant Vector

  CType *data = mat->Data();
  CType *Xdata = X.mat->Data();
  CType sum;

  uint r, c, rp;
  int first = -1;

  X.rowidx[0] = rowidx[0];
  X.rowpar = rowpar;

  for(r=0;r<rows;r++)
  { rp = r*cols;
    sum = Xdata[r];
    if (first > -1)
      for(c=first;c<r;c++)
	sum -= data[rp+c] * Xdata[c];
    else if (sum != CType(0))
      first = r;

    Xdata[r] = sum;
  }

  for(r=rows-1;r>=0;r--)
  { rp = r*cols;
    sum = Xdata[r];
    for(c=r+1;c<cols;c++)
      sum -= data[rp+c] * Xdata[c];

    Xdata[r] = sum / data[rp+r];
  }

  return X;
}


// Inverse of this matrix using LU
template<class CType>
Matrix<CType> Matrix<CType>::InvLU()
{
  Matrix<CType> Inv(rows, cols);

  if (rows != cols || !rows || !cols) return Inv;

  if (!LUDecomp()) return Inv;
 
  Matrix<CType> X;
  CType *adata = aug->Data();

  uint c, r;

  for(c=0;c<cols;c++)
  { for(r=0;r<rows;r++)
      if (rowidx[0][r] == c) adata[r] = CType(1);
      else adata[r] = CType(0);
    
    X = LUBackSub();
    Inv.SetCol(c+1, X.mat->Data());
  }

  return Inv;
}


// Solve Matrix
template<class CType>
Matrix<CType> Matrix<CType>::Solve()
{
  Matrix<CType> a(this[0]), X;

  if (a.LUDecomp()) X = a.LUBackSub();

  return X;
}


// Solve LU Decomposed Matrix
template<class CType>
Matrix<CType> Matrix<CType>::LUSolve()
{
  Matrix<CType> X;

  X = LUBackSub();

  return X;
}

// Solve Matrix friend
template<class CType>
Matrix<CType> Solve(const Matrix<CType> &a)
{
  Matrix<CType> b(a);

  return b.Solve();
}


// LU Solve Matrix friend
template<class CType>
Matrix<CType> LUSolve(const Matrix<CType> &a)
{
  Matrix<CType> b(a);

  return b.LUSolve();
}






// Order Matrix
template<class CType>
Matrix<CType>& Matrix<CType>::Order()
{
  if (!rows || !cols) return this[0];

  uint r, ri;

  for(r=0;r<rows;r++)
  { ri = rowidx[0][r];
  //    printf("r: %d, ri: %d\n", r, ri);
    if (r != ri)
    { SwapRows(r+1,ri+1);
      r--;
    }
  }

  return this[0];
}

// Determinant of Matrix
template<class CType>
CType Matrix<CType>::Det()
{
  Matrix<CType> a(this[0]);
  CType r = CType(0);

  if (a.LUDecomp())
  { CType *data = a.mat->Data();
    uint i,j;
    r = CType(a.rowpar);

    for(i=0,j=0;i<a.rows;i++,j+=a.cols+1)
      r *= data[j];
  }

  return r;
}


// Determinant of LU Decomposed Matrix
template<class CType>
CType Matrix<CType>::LUDet()
{
  if (!rows || !cols || cols != rows) return CType(0);

  CType r = CType(rowpar);
  CType *data = mat->Data();
  uint i,j;

  for(i=0,j=0;i<rows;i++,j+=cols+1)
    r *= data[j];

  return r;
}



// Det. of friend Matrix
template<class CType>
CType Det(const Matrix<CType> &a)
{
  Matrix<CType> b(a);

  return b.Det();
}


// Det. of friend LU Matrix
template<class CType>
CType LUDet(const Matrix<CType> &a)
{
  Matrix<CType> b(a);

  return b.LUDet();
}




// Inverse of  Matrix
template<class CType>
Matrix<CType> operator! (const Matrix<CType> &a)
{
  Matrix<CType> b(a);

  switch(a.invtype)
  { case MATINV_LU: 
      if (b.cols == b.rows) return b.InvLU();

    case MATINV_SVD:
      return b.InvSVD();
  }

  return b;
}



// Negative of Matrix
template<class CType>
Matrix<CType> operator- (const Matrix<CType> &a)
{
  Matrix<CType> b(a);

  CType *data = b.mat->Data();
  uint size = a.rows*a.cols;

  for(uint i=0;i<size;i++)
    data[i] = -data[i];

  return b;
}


// Multiplication of Matrix with constant
template<class CType>
Matrix<CType> operator* (const Matrix<CType> &a, const CType &b)
{
  Matrix<CType> c(a);

  CType *data = c.mat->Data();
  uint size = a.rows*a.cols;

  for(uint i=0;i<size;i++)
    data[i] *= b;

  return c;
}


// Multiplication of Matrix with constant
template<class CType>
Matrix<CType> operator* (CType &b, const Matrix<CType> &a)
{

  Matrix<CType> c(a);

  CType *data = c.mat->Data();
  uint size = a.rows*a.cols;

  for(uint i=0;i<size;i++)
    data[i] *= b;

  return c;

}


// Multiplication of Matrix with Matrix
template<class CType>
Matrix<CType> operator* (const Matrix<CType> &a, const Matrix<CType> &b)
{

	Matrix<CType> c(a.rows, b.cols);

	if (!a.cols || !a.rows || !b.cols || !b.rows || a.cols != b.rows) return c;  

	CType *adata = a.mat->Data();
	CType *bdata = b.mat->Data();
	CType *cdata = c.mat->Data();

	CType dA, dB;
	uint ra, ca, cb;
	uint rpa, rpc, cpos, apos, bpos;

	for(ra=0;ra<a.rows;ra++)
	{ 
		rpa = ra*a.cols;
		rpc = ra*b.cols;
		for(cb=0;cb<b.cols;cb++)
		{ 
			cpos = rpc + cb;
			for(ca=0, apos=rpa, bpos=cb;ca<a.cols;ca++, apos++, bpos += b.cols)
			{
				dA = adata[apos];
				dB = bdata[bpos];
				if (dA != CType(0) && dB != CType(0))
					cdata[cpos] +=  dA * dB;
			}
		}
	}

	return c;
}



// Multiplication of this Matrix with Matrix
template<class CType>
Matrix<CType>& operator*= (Matrix<CType> &a, const Matrix<CType> &b)
{
	a = a * b;

	return a;
}




// Addition of Matrix to Matrix
template<class CType>
Matrix<CType> operator+ (const Matrix<CType> &a, const Matrix<CType> &b)
{

  Matrix<CType> c(a.rows, a.cols);

  if (!a.cols || !a.rows || !b.cols || !b.rows || 
      a.cols != b.cols || a.rows != b.rows) return c;  

  CType *adata = a.mat->Data();
  CType *bdata = b.mat->Data();
  CType *cdata = c.mat->Data();
  uint size = a.rows*a.cols;

  for(uint i=0;i<size;i++)
    cdata[i] = adata[i] + bdata[i];

  return c;
}


// Addition of Matrix to Matrix
template<class CType>
Matrix<CType>& operator+= (Matrix<CType> &a, const Matrix<CType> &b)
{
  if (!a.cols || !a.rows || !b.cols || !b.rows || 
      a.cols != b.cols || a.rows != b.rows) return a;  

  CType *adata = a.mat->Data();
  CType *bdata = b.mat->Data();
  uint size = a.rows*a.cols;

  for(uint i=0;i<size;i++)
    adata[i] += bdata[i];

  return a;
}


// Addition of scalar to all comps. of Matrix
template<class CType>
Matrix<CType> operator+ (const Matrix<CType> &a, const CType &b)
{

  Matrix<CType> c(a.rows, a.cols);

  if (!a.cols || !a.rows) return c;  

  CType *adata = a.mat->Data();
  CType *cdata = c.mat->Data();
  uint size = a.rows*a.cols;

  for(uint i=0;i<size;i++)
    cdata[i] = adata[i] + b;

  return c;
}


// Addition of scalar to all comps. of Matrix
template<class CType>
Matrix<CType>& operator+= (Matrix<CType> &a, const CType &b)
{
  if (!a.cols || !a.rows) return a;  

  CType *adata = a.mat->Data();
  uint size = a.rows*a.cols;

  for(uint i=0;i<size;i++)
    adata[i] += b;

  return a;
}



// Subtraction of Matrix from Matrix
template<class CType>
Matrix<CType> operator- (const Matrix<CType> &a, const Matrix<CType> &b)
{

  Matrix<CType> c(a.rows, a.cols);

  if (!a.cols || !a.rows || !b.cols || !b.rows || 
      a.cols != b.cols || a.rows != b.rows) return c;  

  CType *adata = a.mat->Data();
  CType *bdata = b.mat->Data();
  CType *cdata = c.mat->Data();
  uint size = a.rows*a.cols;

  for(uint i=0;i<size;i++)
    cdata[i] = adata[i] - bdata[i];

  return c;
}


// Subtraction of Matrix from this Matrix
template<class CType>
Matrix<CType>& operator-= (Matrix<CType> &a, const Matrix<CType> &b)
{
  if (!a.cols || !a.rows || !b.cols || !b.rows || 
      a.cols != b.cols || a.rows != b.rows) return a;  

  CType *adata = a.mat->Data();
  CType *bdata = b.mat->Data();
  uint size = a.rows*a.cols;

  for(uint i=0;i<size;i++)
    adata[i] -= bdata[i];

  return a;
}



// Subtraction of scalar to all comps. of Matrix
template<class CType>
Matrix<CType> operator- (const Matrix<CType> &a, const CType &b)
{

  Matrix<CType> c(a.rows, a.cols);

  if (!a.cols || !a.rows) return c;  

  CType *adata = a.mat->Data();
  CType *cdata = c.mat->Data();
  uint size = a.rows*a.cols;

  for(uint i=0;i<size;i++)
    cdata[i] = adata[i] - b;

  return c;
}


// Subtraction of scalar to all comps. of Matrix
template<class CType>
Matrix<CType>& operator-= (Matrix<CType> &a, const CType &b)
{
  if (!a.cols || !a.rows) return a;  

  CType *adata = a.mat->Data();
  uint size = a.rows*a.cols;

  for(uint i=0;i<size;i++)
    adata[i] -= b;

  return a;
}





// Add Rows to Matrix
// no rows of matrix a starting from src are inserted into this matrix before row dest.

template<class CType>
uint Matrix<CType>::AddRows(uint dest, uint src, const Matrix<CType> &a, 
			    uint no)
{
  if (!rows || !cols || !dest || !src || src+no-1 > a.rows
      || cols != a.cols) return 0;

  uint m;

  if (dest > rows) 
  { dest = rows+1;
    m = 0;
  }
  else m = rows - dest + 1;

  if (!Resize(rows+no,cols)) return 0;

  CType *data = mat->Data();
  CType *adata = a.mat->Data();

  if (m) mat->Move((dest+no-1)*cols, (dest-1)*cols, m*cols); 

  memcpy(&data[(dest-1)*cols], &adata[(src-1)*a.cols], no*cols*sizeof(CType));

  return 1;
}


// Add Columns to Matrix
template<class CType>
uint Matrix<CType>::AddCols(uint dest, uint src, const Matrix<CType> &a, 
			    uint no)
{
  if (!rows || !cols || !dest || !src || src+no-1 > a.cols
      || rows != a.rows) return 0;

  uint m;

  if (dest > cols) 
  { dest = cols+1;
    m = 0;
  }
  else m = cols - dest + 1;

  if (!Resize(rows, cols+no)) return 0;

  CType *data = mat->Data();
  CType *adata = a.mat->Data();
  uint rp;

  for(uint r=0;r<rows;r++)
  { rp = r*cols;
    if (m) mat->Move(rp+dest+no-1, rp+dest-1, m);
    
    for(uint j=0;j<no;j++)
      data[rp+dest-1+j] = adata[r*a.cols+src-1+j];
  }

  return 1;
}


// Sub Rows from Matrix
template<class CType>
Matrix<CType> Matrix<CType>::SubRows(uint row, uint no)
{
  Matrix<CType> a(no, cols);

  if (!rows || !cols || row+no-1 > rows) return a;

  CType *data = mat->Data();
  CType *adata = a.mat->Data();
  uint m;

  if (row >= rows)
  { row = rows;
    m = 0;
  }
  else m = rows - row - no + 1;

  memcpy(adata, &data[(row-1)*cols], no*cols*sizeof(CType));

  if (m) mat->Move((row-1)*cols, (row-1+no)*cols, m*cols);

  Resize(rows-no,cols);

  return a;
}



// Sub Columns from Matrix
template<class CType>
Matrix<CType> Matrix<CType>::SubCols(uint col, uint no)
{
  Matrix<CType> a(rows, no);

  if (!rows || !cols || col+no-1 > cols) return a;

  CType *data = mat->Data();
  CType *adata = a.mat->Data();
  uint r, rp, m;

  if (col >= cols)
  { col = cols;
    m = 0;
  }
  else m = cols - col - no + 1;

  for(r=0;r<rows;r++)
  { rp = r*cols;
    memcpy(&adata[r*no], &data[rp+col-1], no*sizeof(CType));

    if (m) mat->Move(rp+col-1, rp+col+no-1, m);
  }

  Resize(rows,cols-no);

  return a;
}



// Find Column Width
template<class CType>
void Matrix<CType>::FindColWidth()
{
  colwidth = 0;
  augcolwidth = 0;

  if (!rows || !cols) return;

  cStr hstr;
  CType *data = mat->Data();
  uint size = rows*cols;
  uint i,w;

  for(i=0;i<size;i++)
    if ((w = (hstr = MakeStr(data[i])).Len()) > colwidth)
      colwidth = w;
  colwidth++;

  data = aug->Data();
  for(i=0;i<rows;i++)
    if ((w = (hstr = MakeStr(data[i])).Len()) > augcolwidth)
      augcolwidth = w;
  augcolwidth++;
}


// String Output
template<class CType>
char* Matrix<CType>::Str(uint row, uint augrow)
{
  if (!rows || !cols || !row || row > rows) 
		return str = "Error";

  cStr hstr2;

  CType *data = mat->Data();
  uint rp = (row-1)*cols;
  int sp;

  str = "|";

  for(uint c=0;c<cols;c++)
  { hstr2 = MakeStr(data[rp+c]);
    sp = colwidth - hstr2.Len();
    if (sp <= 0) sp = 1;
    str = (str|sp) + hstr2;    
  }

  if (augrow)
  { str += " /";
    hstr2 = MakeStr(aug[0][row-1]);
    sp = augcolwidth - hstr2.Len();
    if (sp <= 0) sp = 1;
    str = (str|sp) + hstr2;
  }

  str += "|";

  return str;
}

// Make diagonal matrix out of row vector
template<class CType>
void Matrix<CType>::Diagonal()
{
  if (!rows || !cols || (rows > 1 && cols > 1)) return;

  if (cols == 1)
  { cols = rows;
    rows = 1;
  }

  Resize(cols, cols);
  
  CType *data = Data();
  uint i,j;

  for(i=1,j=cols+1;i<cols;i++,j+=cols+1)
  { data[j] = data[i];
    data[i] = CType(0);
  }

}

// Make Row vector from Trace of square matrix
template<class CType>
void Matrix<CType>::Trace()
{
  if (!rows || !cols || rows != cols) return;

  CType *data = Data();
  uint i,j;

  for(i=1,j=cols+1;i<cols;i++,j+=cols+1)
    data[i] = data[j];

  Resize(1, cols);

}


// Jacobi Decomposition to find Eigenvalues and Eigenvectors
template<class CType>
uint Matrix<CType>::Jacobi(Matrix<CType> &eval, Matrix<CType> &evec)
{
	uint rots = 0;
	uint j, iq, ip, i, ipr, iqr, jrp;
	CType tresh, theta, tau, t, sm, s, h, g, c;
	CType *b, *z;
	
	if (!rows || !cols || rows != cols) return 0;
	
	if (!(b = new CType[rows])) return 0;
	
	if (!(z = new CType[rows]))
	{ 
		delete[] b;
		return 0;
	}
	
	Matrix<CType> a(this[0]);
	
	eval.Resize(1, cols);
	evec.Resize(rows, cols);
	
	CType *deval = eval.Data();
	CType *devec = evec.Data();
	CType *data = a.Data();
	
	evec.Identity();
	
	for(ip=0;ip<cols;ip++)
	{ 
		deval[ip] = b[ip] = data[ip*cols+ip];
		z[ip] = CType(0);
	}
	
	for(i=0;i<50;i++)
	{ 
		sm = CType(0);
		for(ip=0;ip<rows;ip++)
		{ 
			ipr = ip*cols;
			for(iq=0;iq<rows;iq++)
				sm += Mag(data[ipr+iq]);
		}
	
		if (sm == CType(0)) break;       // Exit
	
		if (i < 3) tresh = CType(0.2)*sm / CType(rows*rows);
		else tresh = CType(0);
	
		for(ip=0;ip<rows-1;ip++)
		{ 
			ipr = ip*cols;
			for(iq=ip+1;iq<rows;iq++)
			{ 
				iqr = iq*cols;
	
				g = CType(100) * Mag(data[ipr+iq]);
				if (i > 3 && Mag(deval[ip])+g == Mag(deval[ip]) &&
					Mag(deval[iq])+g == Mag(deval[iq])) 
				{ 
					data[ipr+iq] = CType(0);
				}
				else if (Mag(data[ipr+iq]) > tresh)
				{ 
					h = deval[iq] - deval[ip];
					if(Mag(h)+g == Mag(h)) t = data[ipr+iq] / h;
					else
					{ 
						theta = CType(0.5)*h / data[ipr+iq];
						t = CType(1) / ( Mag(theta) + CType(sqrt(double(CType(1)+theta*theta))) );
						if (theta < CType(0)) t = -t;
					}
					
					c = CType(1) / CType(sqrt(double(1 + t*t)));
					s = t*c;
					tau = s / (CType(1) + c);
					h = t * data[ipr+iq];
					z[ip] -= h;
					z[iq] += h;
					deval[ip] -= h;
					deval[iq] += h;
					data[ipr+iq] = CType(0);
					
					for(j=0;j<ip;j++)
					{ 
						jrp = j*cols;
						g = data[jrp+ip];
						h = data[jrp+iq];
						data[jrp+ip] = g - s*(h + g*tau);
						data[jrp+iq] = h + s*(g - h*tau);
					}
					
					for(j=ip+1;j<iq;j++)
					{
						jrp = j*cols;
						g = data[ipr+j];
						h = data[jrp+iq];
						data[ipr+j] = g - s*(h + g*tau);
						data[jrp+iq] = h + s*(g - h*tau);
					}
					
					for(j=iq+1;j<rows;j++)
					{ 
						g = data[ipr+j];
						h = data[iqr+j];
						data[ipr+j] = g - s*(h + g*tau);
						data[iqr+j] = h + s*(g - h*tau);
					}
					
					for(j=0;j<rows;j++)
					{ 
						jrp = j*cols;
						g = devec[jrp+ip];
						h = devec[jrp+iq];
						devec[jrp+ip] = g - s*(h + g*tau);
						devec[jrp+iq] = h + s*(g - h*tau);
					}
					
					rots++;
				} // else if (Mag(data[ipr+iq]) > tresh)
			} // for(iq=ip+1;iq<rows;iq++)
		} // for(ip=0;ip<rows-1;ip++)
		
		for(ip=0;ip<rows;ip++)
		{ 
			deval[ip] = b[ip] += z[ip];
			z[ip] = CType(0);
		}
	} //  for(i=0;i<50;i++)
	
	delete[] b;
	delete[] z;
	
	return rots;
}


// Order Eigenvalues and Eigenvectors
// Expects Eigenvalues in a row vector
template<class CType>
void EigenOrder(Matrix<CType> &eval, Matrix<CType> &evec)
{
  int k,j,i,n;
  CType p;

  n = eval.cols;

  if (!n) return;

  CType *deval = eval.Data();

  for(i=0;i<n-1;i++)
  { k = i;
    p = deval[i];
    for(j=i+1;j<n;j++)
    { if (Mag(deval[j]) >= Mag(p))
      { k = j;
        p = deval[j];
      }
    }
    if (k != i)
    { deval[k] = deval[i];
      deval[i] = p;
      evec.SwapCols(i+1,k+1);
    }
  }
}



// Calculate sqrt(a^2 + b^2) without destructive over- or underflow
template<class CType>
CType Matrix<CType>::pythag(CType a, CType b)
{
  CType at, bt, h;

  at = Mag(a);
  bt = Mag(b);

  if (at > bt)
  { h = bt / at;
    return (at * CType(sqrt(double(CType(1) + h*h))));
  }
  else if (bt == CType(0))
    return CType(0);
  else
  { h = at / bt;
    return (bt * CType(sqrt(double(CType(1) + h*h))));
  }
}


// Calculate Singular Value Decomposition
template<class CType>
int Matrix<CType>::SVD(Matrix<CType> &u, 
		       Matrix<CType> &w, Matrix<CType> &v)
{
  uint urp, urpi, urpj;
  uint vrp;//, vrpi, vrpj;
  uint cr, l, k, j, jj, its, i, rcmin, hi, hj;
  int retval = 0;
  CType x, y, z, scale, s, h, g, f, c, anorm;
  CType *rv1;


  if (!cols || !rows) return 0;

  if (!(rv1 = new CType[cols])) return 0;

  u = this[0];
  v.Resize(cols, cols);
  w.Resize(1, cols);

  CType *udata = u.Data();
  CType *vdata = v.Data();
  CType *wdata = w.Data();

	/* Use the following for debugging array overshoots
	Mem<CType> &udata = *u.mat;
	Mem<CType> &vdata = *v.mat;
	Mem<CType> &wdata = *w.mat;
	*/

  g = CType(0);
  scale = CType(0);
  anorm = CType(0);

  // Householder reduction to bidiagonal form
  for(i=0,l=1;i<cols;i++,l++)
  { urpi = i * cols;

    rv1[i] = scale * g;
    g = CType(0);
    s = CType(0);
    scale = CType(0);

    if (i < rows)
    { for(k=i;k<rows;k++)
        scale += Mag(udata[k*cols+i]);

      if (scale != CType(0))
      { for(k=i;k<rows;k++)
				{ urp = k*cols+i;
					udata[urp] /= scale;
					s += udata[urp]*udata[urp];
				}
        f = udata[urpi+i];
				g = -sign(CType(sqrt(double(s))), f);
				h = f*g - s;
				udata[urpi+i] = f-g;

				for(j=l;j<cols;j++)
				{ s = CType(0);
					for(k=i;k<rows;k++)
					{ urp = k*cols; 
						s += udata[urp+i] * udata[urp+j];
					}
					f = s / h;
					for(k=i;k<rows;k++)
					{ urp = k * cols;
						udata[urp+j] += f * udata[urp+i];
					}
				} // for(j=l;j<cols;j++)

				for(k=i;k<rows;k++)
					udata[k*cols+i] *= scale;
      } // if (scale != CType(0))
    } // if (i < rows)

    wdata[i] = scale * g;
    g = CType(0);
    s = CType(0);
    scale = CType(0);

    if (i < rows && i != cols)
    { for(k=l;k<cols;k++)
        scale += Mag(udata[urpi+k]);

      if (scale != CType(0))
      { for(k=l;k<cols;k++)
				{ urp = urpi + k;
					udata[urp] /= scale;
					s += udata[urp] * udata[urp];
				}
   
        f = udata[urpi+l];
				g = -sign(CType(sqrt(double(s))), f);
				h = f*g - s;
				udata[urpi+l] = f-g;

				for(k=l;k<cols;k++)
					rv1[k] = udata[urpi+k] / h;

				for(j=l;j<rows;j++)
				{ urpj = j * cols;

				  s = CType(0);
					for(k=l;k<cols;k++)
						s += udata[urpj+k] * udata[urpi+k];

				  for(k=l;k<cols;k++)
				    udata[urpj+k] += s * rv1[k];
				} // for(j=l;j<rows;j++)

				for(k=l;k<cols;k++)
					udata[urpi+k] *= scale;

      } // if (scale != CType(0))
    } // if (i < rows && i != cols)

    anorm = maxi(anorm, (Mag(wdata[i]) + Mag(rv1[i])));

  } // for(i=0,l=1;i<cols;i++,l++)

  // Accumulation of right-hand transformations

  for(i=cols-1,l=cols;l>0;i--,l--)
  { urpi = i * cols;

    if (i < cols-1)
    { if (g != CType(0))
      { for(j=l;j<cols;j++)
				  vdata[j*cols+i] = (udata[urpi+j] / udata[urpi+l]) / g;

        for(j=l;j<cols;j++)
				{ s = CType(0);
					for(k=l;k<cols;k++)
						s += udata[urpi+k] * vdata[k*cols+j];

				  for(k=l;k<cols;k++)
					{ vrp = k * cols; 
						vdata[vrp+j] += s * vdata[vrp+i];
					}
				} // for(j=l;j<cols;j++)
      } // if (g != CType(0))

      for(j=l;j<cols;j++)
      { vdata[urpi+j] = CType(0);
        vdata[j*cols+i] = CType(0);
      }
    } // if (i < cols-1)

    vdata[urpi+i] = CType(1);
    g = rv1[i];
  } // for(i=cols-1,l=cols;l>0;i--,l--)

  // Accumulation of left-hand transformations

  if (rows < cols) rcmin = rows;
  else rcmin = cols;

  for(i=rcmin-1,l=rcmin;l>0;i--,l--)
  { urpi = i*cols;

    g = wdata[i];
    
    for(j=l;j<cols;j++)
      udata[urpi+j] = CType(0);

    if (g != CType(0))
    { g = CType(1) / g;
      
      for(j=l;j<cols;j++)
      { s = CType(0);
        for(k=l;k<rows;k++)
				{ urp = k * cols;
					s += udata[urp+i] * udata[urp+j];
				}
				f = (s / udata[urpi+i]) * g;

				for(k=i;k<rows;k++)
				{ urp = k * cols;
					udata[urp+j] += f * udata[urp+i];
				}
      } // for(j=l;j<cols;j++)

      for(j=i;j<rows;j++)
				udata[j*cols+i] *= g;
    } // if (g != CType(0))
    else
    { for(j=i;j<rows;j++)
        udata[j*cols+i] = CType(0);
    }
    udata[urpi+i] += CType(1);
  } // for(i=rcmin-1,l=rcmin;l>0;i--,l--)

  // Diagonalization of the bidiagonal form

  int status = 0;

  for(k=cols-1,hi=cols;hi>0;k--,hi--)     // Loop over singular values
  { for(its=0;its<_MAXSVDITS_;its++)  // Loop over allowed iteration
    { status = 0;
      
      for(l=k,cr=k-1,hj=k+1;hj>0;l--,cr--,hj--) // Test for splitting
      { if ( (Mag(rv1[l])+anorm) == anorm )
				{ status = 1;
					break;
				}
        if ( cr >= 0 )
					if ( (Mag(wdata[cr])+anorm) == anorm )
						break;
      } // Test for splitting

      if (!status)
      { c = CType(0);
        s = CType(1);

				for(i=l;i<=k;i++)
				{ f = s * rv1[i];
					rv1[i] *= c;

				  if ( (Mag(f)+anorm) == anorm )
						break;

					g = wdata[i];
					h = pythag(f, g);
					wdata[i] = h;
					h = CType(1) / h;
					c = g * h;
					s = -(f * h);

				  for(j=0;j<rows;j++)
					{ urpj = j * cols;
	  
						y = udata[urpj+cr];
						z = udata[urpj+i];
						udata[urpj+cr] = (y*c) + (z*s);
						udata[urpj+i] = -(y*s) + (z*c);
					} // for(j=0;j<rows;j++)
				} // for(i=l;i<=k;i++)
      } // if (!status)

      z = wdata[k];
      if (l == k)                             // Convergence
      { if (z < CType(0))
				{ wdata[k] = -z;
					for(j=0;j<cols;j++)
					{ vrp = j * cols + k;
						vdata[vrp] = -vdata[vrp];
					}
				} // if (z < CType(0))
				retval = 1;
				break;                                // Exit
      }		// if (l == k)

      if (its == _MAXSVDITS_)
				retval = -1;
 
      x = wdata[l];
      cr = k - 1;
      y = wdata[cr];
      g = rv1[cr];
      h = rv1[k];

      f = ( (y-z) * (y+z) + (g-h) * (g+h) ) / (CType(2)*h*y);
      g = pythag(f, CType(1));
      f = ( (x-z) * (x+z) + h * ( (y / (f+sign(g,f))) - h)) / x;

      // Next QR transformation

      c = CType(1);
      s = CType(1);
      for(j=l,i=l+1;j<=cr;j++,i++)
      { g = rv1[i];
        y = wdata[i];
				h = s*g;
				g *= c;
				z = pythag(f, h);
				rv1[j] = z;

				c = f / z;
				s = h / z;
				f = (x*c) + (g*s);
				g = -(x*s) + (g*c);
				h = y*s;
				y *= c;

				for(jj=0;jj<cols;jj++)
				{ vrp = jj * cols;
					x = vdata[vrp+j];
					z = vdata[vrp+i];
					vdata[vrp+j] = (x*c) + (z*s);
					vdata[vrp+i] = -(x*s) + (z*c);
				}

				z = pythag(f, h);
				wdata[j] = z;
	
				if (z != CType(0))
				{ z = CType(1) / z;
					c = f * z;
					s = h * z;
				}

				f = (c*g) + (s*y);
				x = -(s*g) + (c*y);
	
				for(jj=0;jj<rows;jj++)
				{ urp = jj * cols;
					y = udata[urp+j];
					z = udata[urp+i];
					udata[urp+j] = (y*c) + (z*s);
					udata[urp+i] = -(y*s) + (z*c);
				}
      } // for(j=l,i=l+1;j<cr;j++,i++)

      rv1[l] = CType(0);
      rv1[k] = f;
      wdata[k] = x;
    } // for(its=0;its<_MAXSVDITS_;its++)
  } // for(k=cols-1;k>=0;k--)     // Loop over singular values

  delete[] rv1;

  return retval;
}

 
// Order w in decending or ascending order and adjust COLS of v and u.
template<class CType>
void SVDOrder(Matrix<CType> &w, Matrix<CType> &u, Matrix<CType> &v, 
			  bool bDescending)
{
	int k,j,i,n;
	CType p;
	
	n = w.cols;
	
	if (!n) return;
	
	CType *dw = w.Data();
	
	for(i=0;i<n-1;i++)
	{ 
		k = i;
		p = dw[i];
		for(j=i+1;j<n;j++)
		{ 
			if ((Mag(dw[j]) >= Mag(p) && bDescending) ||
				(Mag(dw[j]) <= Mag(p) && !bDescending))
			{ 
				k = j;
				p = dw[j];
			}
		}
		if (k != i)
		{ 
			dw[k] = dw[i];
			dw[i] = p;
			v.SwapCols(i+1,k+1);
			u.SwapCols(i+1,k+1);
		}
	}
}





// Inverse of this matrix using SVD
template<class CType>
Matrix<CType> Matrix<CType>::InvSVD(CType fPrec)
{
  Matrix<CType> Inv(rows, cols);

  if (!rows || !cols) return Inv;

  Matrix<CType> u, v, w;

  if (SVD(u, w, v) <= 0) return Inv;

  w.TinyToZero(fPrec);
  w.InvComps();
  w.Diagonal();

  Inv = v * w * (~u);
  Inv.invtype = invtype;

  return Inv;
}


// Invert Components of matrix. Zero values are set to inf.
template<class CType>
bool Matrix<CType>::InvComps(CType inf, CType fPrec)
{
	if (!cols || !rows) 
		return true;

	CType fTiny;
	if (fPrec == CType(0))
		fPrec = ::Tiny(fTiny);

	CType *data = Data();
	uint size = rows*cols;

	for(uint i=0;i<size;i++)
	{
		if (!::IsZero(data[i], fPrec))
		{
			data[i] = CType(1) / data[i];
		}
		else 
		{
			data[i] = inf;
		}
	}

	return true;
}


/// Extract Submatrix from this matrix
template<class CType>
bool Matrix<CType>::GetSubMatrix(Matrix<CType> &rMat, 
								 int iTRow, int iLCol, int iRowNo, int iColNo)
{
	if (!rows || !cols || 
		iLCol < 1 || iLCol > int(cols) ||
		iTRow < 1 || iTRow > int(rows) ||
		iColNo <= 0 || iRowNo <= 0 ||
		iLCol + iColNo - 1 > int(cols) ||
		iTRow + iRowNo - 1 > int(rows)) 
		return false;

	rMat.Resize(iRowNo, iColNo);
	CType *pfMatData = rMat.Data();

	if (!pfMatData)
		return false;

	CType *pfData = Data();

	int iRow, iCol, iMatPos, iRowPos;

	for(iRow = 0, iMatPos = 0; iRow < iRowNo; iRow++)
	{
		iRowPos = (iTRow + iRow - 1) * cols + iLCol - 1;
		for(iCol = 0; iCol < iColNo; iCol++, iMatPos++, iRowPos++)
		{
			pfMatData[iMatPos] = pfData[iRowPos];
		}
	}

	return true;
}


////////////////////////////////////////////////////////////////////////
// Evaluate Eigenvalues of arbitrary matrix. 
// Returns row vectors of real and imaginary part of eigenvalues.

template<class CType>
bool EigenValues(Matrix<CType> &xMat, Matrix<CType> &xREVals, Matrix<CType> &xIEVals)
{
    double **a,*wr,*wi;
    int *inter,i,j,n,ierr;

    n = xMat.Rows();
	if (n != xMat.Cols())
		return false;

    a = (double **)calloc(n,sizeof(double *));
    for (i=0;i<n;i++) 
	{
        a[i] = (double *)calloc(n,sizeof(double));
    }
    wr = (double *)calloc(n,sizeof(double));
    wi = (double *)calloc(n,sizeof(double));
    inter = (int *)calloc(n,sizeof(int));

    for (i=0;i<n;i++) 
	{
		for(j = 0; j < n; j++)
		{
			a[i][j] = xMat(i+1,j+1);
		}
    }

    elmhes(a,n,0,n-1,inter);
    hqr(n,0,n-1,a,wr,wi,&ierr);

	xREVals.Resize(1, n);
	xIEVals.Resize(1, n);

    for (i=0;i<n;i++) 
	{
		xREVals(1,i+1) = CType(wr[i]);
		xIEVals(1,i+1) = CType(wi[i]);
    }


    for (i=0;i<n;i++) 
	{
        free(a[i]);
    }
    free(a);
    free(inter);
    free(wr);
    free(wi);

	return true;
}



////////////////////////////////////////////////////////////////////////
// Evaluate Eigenvalues and Eigenvectors of arbitrary matrix. 
// Returns row vectors of real and imaginary part of eigenvalues and
// matrices of real and imaginary part of eigenvectors.

template<class CType>
bool Eigen(Matrix<CType> &xMat, Matrix<CType> &xREVals, Matrix<CType> &xIEVals,
		   Matrix<CType> &xREVecs, Matrix<CType> &xIEVecs)
{
    double **a,**v,*wr,*wi,**z, *scale;
	double dSum, dVal;
    int *inter,i,j,n,ierr, low, high;

    n = xMat.Rows();
	if (n != xMat.Cols())
		return false;

    a = (double **)calloc(n,sizeof(double *));
    v = (double **)calloc(n,sizeof(double *));
    z = (double **)calloc(n,sizeof(double *));
    for (i=0;i<n;i++) 
	{
        a[i] = (double *)calloc(n,sizeof(double));
        v[i] = (double *)calloc(n,sizeof(double));
        z[i] = (double *)calloc(2*n,sizeof(double));
    }
    wr = (double *)calloc(n,sizeof(double));
    wi = (double *)calloc(n,sizeof(double));
    scale = (double *)calloc(n,sizeof(double));
    inter = (int *)calloc(n,sizeof(int));

    for (i=0;i<n;i++) 
	{
		for(j = 0; j < n; j++)
		{
			a[i][j] = xMat(i+1,j+1);
		}
    }

	balanc(a, n, &low, &high, scale);

    elmhes(a,n, low, high,inter);
	eltran(n, low, high , a, inter, z);
    hqr2(n, low, high, a, wr, wi, z, &ierr);
	//pnormwr(n, z, wr, wi); // does not normalize

	balbak(n, low, high, scale, n, z);

	xREVals.Resize(1, n);
	xIEVals.Resize(1, n);
	xREVecs.Resize(n, n);
	xIEVecs.Resize(n, n);

    for (i=0;i<n;i++) 
	{
		xREVals(1,i+1) = CType(wr[i]);
		xIEVals(1,i+1) = CType(wi[i]);
		
		if (wi[i] == 0.0)
		{
			dSum = 0;
			for(j = 0; j < n; j++)
			{
				dVal = z[j][i];
				dSum += dVal * dVal;
			}
			dVal = sqrt(dSum);

			if (dVal != 0.0)
			{
				for(j = 0; j < n; j++)
					xREVecs(j+1, i+1) = CType(z[j][i] / dVal);
			}
			else
			{
				for(j = 0; j < n; j++)
					xREVecs(j+1, i+1) = CType(0);
			}
		}
		else if (wi[i] > 0.0)
		{
			// complex eigenvector
			for(j = 0; j < n; j++)
			{
				xREVecs(j+1, i+1) = CType(z[j][i]);
				xIEVecs(j+1, i+1) = CType(z[j][i+1]);
			}
		}	
		else // wi[i] < 0.0
		{
			// conjugate complex eigenvector
			for(j = 0; j < n; j++)
			{
				xREVecs(j+1, i+1) = CType(z[j][i-1]);
				xIEVecs(j+1, i+1) = -CType(z[j][i]);
			}
		}	
    }


    for (i=0;i<n;i++) 
	{
        free(a[i]);
        free(v[i]);
        free(z[i]);
    }
    free(a);
	free(v);
	free(z);
    free(inter);
    free(wr);
    free(wi);
	free(scale);

	return true;
}




#endif
