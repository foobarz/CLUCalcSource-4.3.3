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


// Library for use of tensors

#include<math.h>
#include<stdio.h>
#include<string.h>

#include"tensor.h"
#include"tensinst.h"

// ------------------------ Default Constructor ------------------------

template<class CType>
Tensor<CType>::Tensor()
{
  setDims(0, 0); // Make default tensor a scalar:
                 // Zero Rank -> 1 Dimension with 1 entry.

  fidx = 1;
  actidx = 0;
	Terrno = 0;

	//((*this),1);
}



// -------------------------- Constructor ---------------------------

template<class CType>
Tensor<CType>::Tensor(uint nrank, uint* ndims, uint nfidx)
{
  setDims(nrank, ndims);

  fidx = nfidx;
  actidx = 0;
	Terrno = 0;
}


// -------------------------- Copy Constructor ---------------------------

template<class CType>
Tensor<CType>::Tensor(const Tensor<CType>& t)
{
  copyTensor(t);
}

/*
// -------------------------- Destructor ---------------------------
template<class CType>
Tensor<CType>::~Tensor()
{
  if (Tcpy) delete Tcpy;
}
*/

// ---------------------------- Copy Tensor --------------------------

template<class CType>
void Tensor<CType>::copyTensor(const Tensor<CType>& t)
{
	Terrno = t.Terrno;
  fidx = t.fidx;
  actidx = t.actidx;
  actpos = t.actpos;

  rank = t.rank;
  dims = t.dims;
  idxrange = t.idxrange;

  idxlist = t.idxlist;
  idxstep = t.idxstep;

  size = t.size;
  comps = t.comps;
  
  looplist = t.looplist;
}


// -------------------------- Reset Tensor ---------------------------

template<class CType>
void Tensor<CType>::Reset(uint nrank, uint* ndims, uint nfidx)
{
  setDims(nrank, ndims);

  fidx = nfidx;
  actidx = 0;
	Terrno = 0;
}


// ------------------------------ Set Dimensions ----------------------

template<class CType>
void Tensor<CType>::setDims(uint nrank, uint *ndims)
{
  uint i, d;

  rank = nrank;

  if (rank)
  { dims = rank;
    for(i=0;i<rank;i++)
      dims[i] = ((d=ndims[i]) == 0 ? 1 : d);
  }
  else
  { dims = 1;
    dims[0] = 1;
  }

  idxlist = rank;
  idxrange = rank;
  for(i=0;i<rank;i++)
  { idxrange[i].first = 0;
    idxrange[i].last = dims[i] - 1;
  }

  size = Size();
  if (size == 0) size = 1;

  comps = size;

  CType *data = comps.Data();
  for(i=0;i<size;i++)
    data[i] = CType(0);

  //memset(data, 0, size*sizeof(CType));

  // set step sizes for indices in component array.
  if (rank)
  { idxstep = rank;
    idxstep[rank-1] = 1;
  }
  else
  { idxstep = 1;
    idxstep[0] = 0;
  }

  if (rank > 1)
  { idxstep[rank-2] = dims[rank-1];
    for(i=rank-2;i>0;i--)
    { idxstep[i-1] = idxstep[i] * dims[i];
    }
  }
}


// ------------------------ Resize Dimension --------------------------
// Changes size of a dimension
// return 1 if succesful, 0 otherwise.

template<class CType>
uint Tensor<CType>::ResizeDim(uint d, uint no)
{
  if (d == 0 || d > rank) return 0;
  d--;

  if (no == 0) no = 1;

  uint i;
  uint *cidx = new uint[rank];  // Counting indices

  Mem<uint> ndims(dims);
  ndims[d] = no;         // Set size of new dimension
    
  Tensor<CType> T(rank, ndims.Data(), fidx);  // Create Resized Tensor
  if (!T.isOK()) 
  { delete[] cidx;
	return 0;
  }

  if (no > dims[d])        // Restrict range if new dim bigger than old
    T.IdxRange(d+1, fidx, dims[d] + fidx - 1);

  this->ResetIdxRange();   // Free index range of old tensor
  
  T.actidx = 0;
  this->actidx = 0;

  for(i=0;i<rank;i++)     // Set counting indices
  { T,&cidx[i];
    this[0],&cidx[i];
  }

  T = this[0];
  T.ResetIdxRange();
  this[0] << T;

  delete[] cidx;
  return 1;
}


// ---------------------------- Is Ok --------------------------------
// Tests whether instance is OK

template<class CType>
uint Tensor<CType>::isOK()
{
  Terrno = 1;
  if (!dims.Data()) return 0;
  Terrno++;
  if (!idxstep.Data()) return 0;
  Terrno++;
  if (!looplist.Data()) return 0;
  Terrno++;
  if (!comps.Data()) return 0;

  Terrno = 0;
  return 1;
}






// ------------------------------- Get Component -------------------------

// expects idx starting at zero
// Also, does NO range checking
template<class CType>
CType& Tensor<CType>::Comp(uint posa[]) 
{
  if (!rank) return comps[0];

  actpos = 0;

  for(uint i=0;i<rank;i++)
    actpos += posa[i] * idxstep[i];

  return comps[actpos];
}



// -------------------------- Get Active Component ----------------------

template<class CType>
CType& Tensor<CType>::ActComp()
{
  return comps[ActPos()];
}


// -------------------------- Get Active Position ----------------------

template<class CType>
uint Tensor<CType>::ActPos()
{
  
  if (!rank) return 0;

  actpos = 0;

  for(uint i=0;i<rank;i++)
  { if (idxlist[i].type == Number)
      actpos += idxlist[i].data.pos * idxstep[i];
    else
    { actpos += idxlist[i].data.idx[0] * idxstep[i];
    }
  }

  return actpos;
}

// ----------------------------- Get total size ------------------------

template<class CType>
uint Tensor<CType>::Size() const
{
  uint sz = dims[0];

  for(uint i=1;i<rank;i++)
    sz *= dims[i];

  return sz;
}



// --------------------------- Set Index Ranges ------------------------

template<class CType>
void Tensor<CType>::IdxRange(uint rgn[])
{
  uint r;

  for(uint i=0;i<rank;i++)
  { if ((r=rgn[i*2]) <= fidx) r = 0;
    else if ((r-=fidx) >= dims[i]) r = dims[i] - 1;
    idxrange[i].first = r;

    if ((r=rgn[i*2+1]) <= fidx) r = 0;
    else if ((r-=fidx) >= dims[i]) r = dims[i] - 1;    
    idxrange[i].last = r;
  }
}

// ----------------------- Set Certain Index Range ------------------------

template<class CType>
void Tensor<CType>::IdxRange(uint d, uint f, uint l)
{
  if (d == 0 || d > rank) return;
  d--;
  
  if (f <= fidx) f = 0;
  else if ((f-=fidx) >= dims[d]) f = dims[d] - 1;    
  
  if (l <= fidx) l = 0;
  else if ((l-=fidx) >= dims[d]) l = dims[d] - 1;    
  
  idxrange[d].first = f;
  idxrange[d].last = l;
}

// --------------------------- Reset Index Ranges ------------------------

template<class CType>
void Tensor<CType>::ResetIdxRange()
{
  for(uint i=0;i<rank;i++)
  { idxrange[i].first = 0;
    idxrange[i].last = dims[i] - 1;
  }
}



// --------------------------- Create String Output -----------------

template<class CType>
char* Tensor<CType>::Str()
{
  CType *data = comps.Data();

  uint n = InitLoops();
  uint l = n;

  str = "| ";
  do
  { if (n-l == 1) str += "|\n| ";
    else if (n-l > 1) str += "|\n\n| ";
    str << MakeStr(data[actpos]) << " ";
  } while ((l = StepLoops()));

  str += "|";

  return str;
}


// ------------------------ Initialize Loops ----------------------

// returns no of loops
// returns 0 if no loops exist

template<class CType>
uint Tensor<CType>::InitLoops()
{
  uint i,j,k;
  uint n, isNew;
  uint *ptr;

  actpos = 0;
  looplist = 0;

  for(i=0,j=0;i<rank;i++)
  { if (idxlist[i].type == Tensor<CType>::Pointer)
    { isNew = 1;
      n = looplist.Count();
      ptr = idxlist[i].data.idx;

      for(k=0;k<n;k++)
      { if (looplist[k].var == ptr) 
				{ isNew = 0;
					break;
				}
      }

      if (isNew)
      { looplist++;
        Tensor<CType>::loopVar &loop = looplist[j];
        loop.var = ptr;
        loop.var[0] = idxrange[i].first;
        loop.count.first = idxrange[i].first;
        loop.count.last = idxrange[i].last;
        loop.step = idxstep[i];
				loop.unique = 1;
				loop.dim = i;
        j++;
      }
      else
      { looplist[k].step += idxstep[i];
      }
    }
    else
    { actpos += idxlist[i].data.pos * idxstep[i];
    }
  }
  
  for(i=0;i<j;i++)
  { Tensor<CType>::loopVar &loop = looplist[i];
    actpos += loop.count.first * loop.step;
  }

#ifdef _SHOWLOOPS_
  printf("*** No of nested loops: %d\n", j);
  printf("actpos: %d\n", actpos);
#endif

  return j;
}


// ------------------------- Step through Loops ------------------

// InitLoops has to be called beforehand
// Changes actpos which points to the next component in comps

// returns 0 if all loops are finished
// returns n if step in loop n has been performed 
// where n=1 is outermost loop, n=2 is next inner loop, etc.

template<class CType>
uint Tensor<CType>::StepLoops()
{
  int n = looplist.Count();
  int i;

  if (!n) return 0;

  for(i=n-1;i>=0;i--)
  { Tensor<CType>::loopVar &loop = looplist[i];
    if (++loop.var[0] > loop.count.last)
    { 
#ifdef _SHOWLOOPS_
      printf("*** End of Loop\n");
#endif
      if (i == 0) return 0;                     // End of outermost loop
      actpos -= (loop.var[0] - loop.count.first - 1) * loop.step;
      loop.var[0] = loop.count.first;
    }
    else
    { actpos += loop.step;
#ifdef _SHOWLOOPS_ 
       printf("Step: %d -> Pos: %d\n", loop.step, actpos);
#endif
      return i+1;
    }
  }
   
  return 0; // Should never be encountered
}



//-------------------- Init Contraction Loops --------------------------
// Returns number of loops
template<class CType>
int InitCLoops(Tensor<CType>& t1, Tensor<CType>& t2,
			   uint apos[], Mem<Tensor::cloopVar>& loops, Tensor<CType>& nt0)
{
  Mem<uint>  ndims;
  Mem<uint*> nvars;
  int lno;

  CType *data1 = t1.comps.Data();
  CType *data2 = t2.comps.Data();

  int n1, n2;
  int nrank;
  int i,j;
  
  n1 = t1.InitLoops();
  n2 = t2.InitLoops();

#ifdef _SHOWCLOOPS_
  printf("n1: %d, n2: %d\n",n1,n2);
#endif

  for(i=0,lno=0;i<n1;i++,lno++)
  { loops++;
    Tensor<CType>::cloopVar &lp = loops[lno];
    Tensor<CType>::loopVar &llist = t1.looplist[i];
    lp.var = llist.var;
    lp.var[0] = llist.count.first;
    lp.count.first = llist.count.first;
    lp.count.last = llist.count.last;
    lp.step[0] = 0;
    lp.step[1] = llist.step;
    lp.step[2] = 0;

    for(j=0;j<n2;j++)
    { if (llist.var == t2.looplist[j].var)
      { llist.unique = 0;
        t2.looplist[j].unique = 0;	
	lp.step[2] = t2.looplist[j].step;

#ifdef _SHOWCLOOPS_
	printf("Same Index> t1: %d, t2: %d\n", i, j);
#endif

	break;
      }
    }
  }

  nrank = 0;

  for(i=0;i<n1;i++)
    if (t1.looplist[i].unique)
    { ndims++;
      nvars++;
      ndims[nrank] = t1.dims[t1.looplist[i].dim];
      nvars[nrank] = t1.looplist[i].var;
      nrank++;

#ifdef _SHOWCLOOPS_
      printf("Unique t1: %d\n", i);
#endif

    }

  for(i=0;i<n2;i++)
    if (t2.looplist[i].unique)
    { Tensor<CType>::loopVar &llist = t2.looplist[i];
    
      ndims++;
      nvars++;
      ndims[nrank] = t2.dims[llist.dim];
      nvars[nrank] = llist.var;
      nrank++;

      loops++;
      Tensor<CType>::cloopVar &lp = loops[lno];
      
      lp.var = llist.var;
      lp.var[0] = llist.count.first;
      lp.count.first = llist.count.first;
      lp.count.last = llist.count.last;
      lp.step[0] = 0;
      lp.step[1] = 0;
      lp.step[2] = llist.step;
      lno++;

#ifdef _SHOWCLOOPS_
      printf("Unique t2: %d\n", i);
#endif
    }

#ifdef _SHOWCLOOPS_
  printf("t0> rank: %d, dims: ", nrank);
  for(i=0;i<nrank;i++)
    printf("%d, ", ndims[i]);
  printf("\n");
#endif

  Tensor<CType> t0(nrank, ndims.Data(), t1.fidx);
  CType *data0 = t0.comps.Data();

  for(i=0;i<nrank;i++)
  { t0.idxlist[i].type = Tensor<CType>::Pointer;
    t0.idxlist[i].data.idx = nvars[i];
    for(j=0;j<lno;j++)
      if (loops[j].var == nvars[i])
      { loops[j].step[0] = t0.idxstep[i];
        t0.idxrange[i].first = loops[j].count.first;
        t0.idxrange[i].last = loops[j].count.last;
        break;
      }
  }


#ifdef _SHOWCLOOPS_
  printf("\n>>> LOOPS <<<\n");
  for(i=0;i<lno;i++)
  { printf("[%d] ----\n", i);
    printf("    Var  : %p\n", loops[i].var);
    printf("    First: %d\n", loops[i].count.first);
    printf("    Last : %d\n", loops[i].count.last);
    printf("    Step : %d, %d, %d\n", 
	   loops[i].step[0], loops[i].step[1], loops[i].step[2]);
  }
#endif

  apos[0] = t0.ActPos();
  apos[1] = t1.ActPos();
  apos[2] = t2.ActPos();
  
#ifdef _SHOWCLOOPS_
  printf("\n*** Into LOOP ***\n");
#endif

  nt0 << t0;

  return lno;
}

//-------------------- Init Multiple Loops --------------------------
// Doesn't contract identical indices
// Returns number of loops
template<class CType>
int InitMLoops(Tensor<CType>& t1, Tensor<CType>& t2,
			   uint apos[], Mem<Tensor::cloopVar>& loops, Tensor<CType>& nt0)
{
  Mem<uint>  ndims;
  Mem<uint*> nvars;
  int lno;

  CType *data1 = t1.comps.Data();
  CType *data2 = t2.comps.Data();

  int n1, n2;
  int nrank;
  int i,j;
  
  n1 = t1.InitLoops();
  n2 = t2.InitLoops();

#ifdef _SHOWCLOOPS_
  printf("n1: %d, n2: %d\n",n1,n2);
#endif

  for(i=0,lno=0;i<n1;i++,lno++)
  { loops++;
    Tensor<CType>::cloopVar &lp = loops[lno];
    Tensor<CType>::loopVar &llist = t1.looplist[i];
    lp.var = llist.var;
    lp.var[0] = llist.count.first;
    lp.count.first = llist.count.first;
    lp.count.last = llist.count.last;
    lp.step[0] = 0;
    lp.step[1] = llist.step;
    lp.step[2] = 0;

    for(j=0;j<n2;j++)
    { if (llist.var == t2.looplist[j].var)
      { llist.unique = 0;
        t2.looplist[j].unique = 0;	
	lp.step[2] = t2.looplist[j].step;

#ifdef _SHOWCLOOPS_
	printf("Same Index> t1: %d, t2: %d\n", i, j);
#endif

	break;
      }
    }
  }

  nrank = 0;

  for(i=0;i<n1;i++) // Only difference to InitCLoop here
  { ndims++;
    nvars++;
    ndims[nrank] = t1.dims[t1.looplist[i].dim];
    nvars[nrank] = t1.looplist[i].var;
    nrank++;

#ifdef _SHOWCLOOPS_
    printf("Unique t1: %d\n", i);
#endif
  }

  for(i=0;i<n2;i++)
    if (t2.looplist[i].unique)
    { Tensor<CType>::loopVar &llist = t2.looplist[i];
    
      ndims++;
      nvars++;
      ndims[nrank] = t2.dims[llist.dim];
      nvars[nrank] = llist.var;
      nrank++;

      loops++;
      Tensor<CType>::cloopVar &lp = loops[lno];
      
      lp.var = llist.var;
      lp.var[0] = llist.count.first;
      lp.count.first = llist.count.first;
      lp.count.last = llist.count.last;
      lp.step[0] = 0;
      lp.step[1] = 0;
      lp.step[2] = llist.step;
      lno++;

#ifdef _SHOWCLOOPS_
      printf("Unique t2: %d\n", i);
#endif
    }

#ifdef _SHOWCLOOPS_
  printf("t0> rank: %d, dims: ", nrank);
  for(i=0;i<nrank;i++)
    printf("%d, ", ndims[i]);
  printf("\n");
#endif

  Tensor<CType> t0(nrank, ndims.Data(), t1.fidx);
  CType *data0 = t0.comps.Data();

  for(i=0;i<nrank;i++)
  { t0.idxlist[i].type = Tensor<CType>::Pointer;
    t0.idxlist[i].data.idx = nvars[i];
    for(j=0;j<lno;j++)
      if (loops[j].var == nvars[i])
      { loops[j].step[0] = t0.idxstep[i];
        t0.idxrange[i].first = loops[j].count.first;
        t0.idxrange[i].last = loops[j].count.last;
        break;
      }
  }


#ifdef _SHOWCLOOPS_
  printf("\n>>> LOOPS <<<\n");
  for(i=0;i<lno;i++)
  { printf("[%d] ----\n", i);
    printf("    Var  : %p\n", loops[i].var);
    printf("    First: %d\n", loops[i].count.first);
    printf("    Last : %d\n", loops[i].count.last);
    printf("    Step : %d, %d, %d\n", 
	   loops[i].step[0], loops[i].step[1], loops[i].step[2]);
  }
#endif

  apos[0] = t0.ActPos();
  apos[1] = t1.ActPos();
  apos[2] = t2.ActPos();
  
#ifdef _SHOWCLOOPS_
  printf("\n*** Into LOOP ***\n");
#endif

  nt0 << t0;

  return lno;
}

//------------------- Step Contraction and Multiple Loops --------------
// Returns 0 if all loops are finished
// Returns n if step in loop n has been performed, where
// n=1 is outermost loop, n=2 is next inner loop, etc    
template<class CType>
int StepCMLoops(int lno, uint apos[], Mem<Tensor::cloopVar>& loops)
{
  int i;

    for(i=lno-1;i>=0;i--)
    { Tensor<CType>::cloopVar &loop = loops[i];

      if (++loop.var[0] > loop.count.last)
      { 
#ifdef _SHOWCLOOPS_
        printf("*** End of Loop\n");
#endif
        if (i == 0) return 0;               // End of outermost loop
	else
        { apos[0] -= (loop.var[0] - loop.count.first - 1) * loop.step[0];
          apos[1] -= (loop.var[0] - loop.count.first - 1) * loop.step[1];
          apos[2] -= (loop.var[0] - loop.count.first - 1) * loop.step[2];
          loop.var[0] = loop.count.first;
	}
      }
      else
      { apos[0] += loop.step[0];
        apos[1] += loop.step[1];
        apos[2] += loop.step[2];

#ifdef _SHOWCLOOPS_ 
	for(j=0;j<3;j++)
          printf("[%d] > Step: %d -> Pos: %d\n", j, loop.step[j], apos[j]);
#endif
        return i+1;
      }
    }
    
    return 0;   // should never be ecountered
}




// --------------------- Normalize Tensor ---------------------

template<class CType>
Tensor<CType>& Tensor<CType>::Norm(CType *_pmax)
{

  CType *data = comps.Data();
  CType a, max = 0;

  InitLoops();

  do
  { if ((a = Mag(data[actpos])) > max) max = a;
  } while(StepLoops());

  //  printf("max> %g\n", max);

  InitLoops();

  do
  { data[actpos] /= max;
  } while(StepLoops());

  if (_pmax) *_pmax = max;

  return this[0];
}

// --------------------- Calculate Frobenius Norm of Tensor ----------------
// Sum of squares of components

template<class CType>
CType Tensor<CType>::FrobNorm()
{

  CType *data = comps.Data();
  CType a = 0;

  InitLoops();

  do
  { a += data[actpos] * data[actpos];
  } while(StepLoops());

  return a;
}


// --------------------- Calculate Mean of Tensor ----------------
// Mean of components

template<class CType>
CType Tensor<CType>::Mean(bool useMag)
{

  CType *data = comps.Data();
  CType a = 0;
	uint n = 0;

  InitLoops();

	if (useMag)
	{ do
		{ a += Mag(data[actpos]);
			n++;
		} while(StepLoops());
	}
	else
  { do
		{ a += data[actpos];
			n++;
	  } while(StepLoops());
	}

  return a / CType(n);
}









// ------------------- Set active component index pointers ----------
/*
template<class CType>
Tensor<CType>& operator, (Tensor<CType>& t, uint* idx)
{
  Tensor<CType>::idxProp &ic = t.idxlist[t.actidx];
  if (++t.actidx >= t.rank) t.actidx = 0;

  ic.type = Tensor<CType>::Pointer;
  ic.data.idx = idx;

  return t;
}
*/

// ----------------------- Set active component -----------------------
/*
template<class CType>
Tensor<CType>& operator, (Tensor<CType>& t, uint pos)
{
  Tensor<CType>::idxProp &ic = t.idxlist[t.actidx];
  if (++t.actidx >= t.rank) t.actidx = 0;

  ic.type = Tensor<CType>::Number;
  if (t.fidx > pos) ic.data.pos = 0;
  else ic.data.pos = pos - t.fidx;

  return t;
}
*/

// ------------------- Set active component index pointers ----------

template<class CType>
Tensor<CType>& Tensor<CType>::Idx(uint dim, uint* idx)
{
  if (dim > 0) dim--;
  if (dim >= rank) dim = rank-1;

  Tensor<CType>::idxProp &ic = idxlist[dim];

  ic.type = Tensor<CType>::Pointer;
  ic.data.idx = idx;
  actidx = 0;

  return this[0];
}


// ----------------------- Set active component -----------------------

template<class CType>
Tensor<CType>& Tensor<CType>::Idx(uint dim, const uint pos)
{
  if (dim > 0) dim--;
  if (dim >= rank) dim = rank-1;
  idxProp &ic = idxlist[dim];

  ic.type = Number;
  if (fidx > pos) ic.data.pos = 0;
  else ic.data.pos = pos - fidx;
  actidx = 0;

  return this[0];
}



// --------------------- Set components to const ---------------------

template<class CType>
Tensor<CType>& Tensor<CType>::operator= (const CType& a)
{

  CType *data = comps.Data();

  InitLoops();

  do
  { data[actpos] = a;
  } while(StepLoops());

  return this[0];
}


// ----------------- Set components to const array ---------------------
// Loops through tensor and increments position in const array

template<class CType>
Tensor<CType>& Tensor<CType>::operator= (const CType* a)
{
  uint i = 0;
  CType *data = comps.Data();

  InitLoops();

  do
  { data[actpos] = a[i];
    i++;
  } while(StepLoops());

  return this[0];
}




// --------------------- Set tensor equal to other tensor --------------

template<class CType>
Tensor<CType>& Tensor<CType>::operator= (const Tensor<CType>& nt)
{
  Tensor<CType> t(nt);
  CType *data = comps.Data();

  InitLoops();

  do
  { data[actpos] = t.ActComp();
  } while(StepLoops());


  return this[0];
}

// --------------------- Returns Copy of Tensor --------------
/*
template<class CType>
Tensor<CType>& operator+ (const Tensor<CType>& nt)
{
  //if (nt.Tcpy) delete nt.Tcpy;
  
  //nt.Tcpy = new Tensor<CType>(nt);

  return nt.Tcpy[0];
}
*/

// --------------------- Negate Components of Tensor --------------

template<class CType>
Tensor<CType> operator- (const Tensor<CType>& nt)
{
  Tensor<CType> t(nt);
  CType *data = t.comps.Data();

  t.InitLoops();

  do
  { data[t.actpos] = -data[t.actpos];
  } while(t.StepLoops());


  return t;
}


// ------------------------ Multiply two tensors -------------------


template<class CType>
Tensor<CType> operator* (const Tensor<CType>& nt1, const Tensor<CType>& nt2)
{
  Tensor<CType> t1(nt1), t2(nt2);
  Tensor<CType> t0;
  Mem<Tensor<CType>::cloopVar> loops;
  uint apos[3];
  int lno;

  lno = InitCLoops(t1, t2, apos, loops, t0);

  CType *data0 = t0.comps.Data();
  CType *data1 = t1.comps.Data();
  CType *data2 = t2.comps.Data();

  if (!lno)
  { data0[apos[0]] = data1[apos[1]] * data2[apos[2]];
    return t0;
  }

  do
  { 
#ifdef _SHOWTIMES_
    printf("\nActPos: %d, %d, %d\n", apos[0], apos[1], apos[2]);
#endif    

    data0[apos[0]] += data1[apos[1]] * data2[apos[2]];  

#ifdef _SHOWTIMES_
    printf("Data0: %le\n", data0[apos[0]]);
    printf("Data1: %le\n", data1[apos[1]]);
    printf("Data2: %le\n", data2[apos[2]]);
#endif    

  } while (StepCMLoops(lno, apos, loops));


  return t0;
}



// ---------- Multiply two tensors and write result into first ------------


template<class CType>
Tensor<CType>& operator*= (Tensor<CType>& nt1, const Tensor<CType>& nt2)
{
  return (nt1 = (nt1 * nt2));
}

// ------------------ Multiply this Tensor with constant ------------------

template<class CType>
Tensor<CType>& operator*= (Tensor<CType>& t, const CType a)
{

  CType *data = t.comps.Data();

  t.InitLoops();

  do
  { data[t.actpos] *= a;
  } while(t.StepLoops());

  return t;
}


// ------------------ Multiply Tensor with constant ------------------

template<class CType>
Tensor<CType> operator* (const Tensor<CType>& nt, const CType a)
{
  Tensor<CType> t(nt);
  CType *data = t.comps.Data();

  t.InitLoops();

  do
  { data[t.actpos] *= a;
  } while(t.StepLoops());

  return t;
}


// ------------------ Multiply Tensor with constant ------------------

template<class CType>
Tensor<CType> operator* (const CType a, const Tensor<CType>& nt)
{
  Tensor<CType> t(nt);
  CType *data = t.comps.Data();

  t.InitLoops();

  do
  { data[t.actpos] *= a;
  } while(t.StepLoops());

  return t;
}



//*********** DIVISION **************



// ------------------------ Divide two tensors -------------------


template<class CType>
Tensor<CType> operator/ (const Tensor<CType>& nt1, const Tensor<CType>& nt2)
{
  Tensor<CType> t1(nt1), t2(nt2);
  Tensor<CType> t0;
  Mem<Tensor<CType>::cloopVar> loops;
  uint apos[3];
  int lno;

  lno = InitCLoops(t1, t2, apos, loops, t0);

  CType *data0 = t0.comps.Data();
  CType *data1 = t1.comps.Data();
  CType *data2 = t2.comps.Data();

  if (!lno)
  { data0[apos[0]] = data1[apos[1]] / data2[apos[2]];
    return t0;
  }

  do
  { 
#ifdef _SHOWDIV_
    printf("\nActPos: %d, %d, %d\n", apos[0], apos[1], apos[2]);
#endif    

    data0[apos[0]] += data1[apos[1]] / data2[apos[2]];  

#ifdef _SHOWDIV_
    printf("Data0: %le\n", data0[apos[0]]);
    printf("Data1: %le\n", data1[apos[1]]);
    printf("Data2: %le\n", data2[apos[2]]);
#endif    

  } while (StepCMLoops(lno, apos, loops));


  return t0;
}



// ---------- Divide two tensors and write result into first ------------


template<class CType>
Tensor<CType>& operator/= (Tensor<CType>& nt1, const Tensor<CType>& nt2)
{
  return (nt1 = (nt1 / nt2));
}

// ------------------ Divide this Tensor with constant ------------------

template<class CType>
Tensor<CType>& operator/= (Tensor<CType>& t, const CType a)
{

  CType *data = t.comps.Data();

  t.InitLoops();

  do
  { data[t.actpos] /= a;
  } while(t.StepLoops());

  return t;
}


// ------------------ Divide Tensor with constant ------------------

template<class CType>
Tensor<CType> operator/ (const Tensor<CType>& nt, const CType a)
{
  Tensor<CType> t(nt);
  CType *data = t.comps.Data();

  t.InitLoops();

  do
  { data[t.actpos] /= a;
  } while(t.StepLoops());

  return t;
}




//***********************************


// ----------------------- Copy Tensor -----------------------

template<class CType>
Tensor<CType>& Tensor<CType>::operator<< (const Tensor<CType>& t)
{
  copyTensor(t);

  return *this;
}
  




// --------------------- Add two Tensors ----------------------------

template<class CType>
Tensor<CType> operator+ (const Tensor<CType>& nt1, const Tensor<CType>& nt2)
{
  Tensor<CType> t1(nt1), t2(nt2);
  Tensor<CType> t0;
  Mem<Tensor<CType>::cloopVar> loops;
  uint apos[3];
  int lno;

  lno = InitMLoops(t1, t2, apos, loops, t0);

  CType *data0 = t0.comps.Data();
  CType *data1 = t1.comps.Data();
  CType *data2 = t2.comps.Data();

  if (!lno)
  { data0[apos[0]] = data1[apos[1]] + data2[apos[2]];
    return t0;
  }

  do
  { 
#ifdef _SHOWADD_
    printf("\nActPos: %d, %d, %d\n", apos[0], apos[1], apos[2]);
#endif    

    data0[apos[0]] = data1[apos[1]] + data2[apos[2]];  

#ifdef _SHOWADD_
    printf("Data0: %le\n", data0[apos[0]]);
    printf("Data1: %le\n", data1[apos[1]]);
    printf("Data2: %le\n", data2[apos[2]]);
#endif    

  } while (StepCMLoops(lno, apos, loops));


  return t0;

}

// --------------------- Add Tensor to this Tensor ------------

template<class CType>
Tensor<CType>& operator+= (Tensor<CType>& nt1, const Tensor<CType>& nt2)
{
  return (nt1 = (nt1 + nt2));
}


// ------------------ Add this Tensor with constant ------------------

template<class CType>
Tensor<CType>& operator+= (Tensor<CType>& t, const CType& a)
{

  CType *data = t.comps.Data();

  t.InitLoops();

  do
  { data[t.actpos] += a;
  } while(t.StepLoops());

  return t;
}


// ------------------ Add Tensor with constant ------------------

template<class CType>
Tensor<CType> operator+ (const Tensor<CType>& nt, const CType& a)
{
  Tensor<CType> t(nt);
  CType *data = t.comps.Data();

  t.InitLoops();

  do
  { data[t.actpos] += a;
  } while(t.StepLoops());

  return t;
}









// --------------------- Subtract two Tensors ----------------------------

template<class CType>
Tensor<CType> operator- (const Tensor<CType>& nt1, const Tensor<CType>& nt2)
{
  Tensor<CType> t1(nt1), t2(nt2);
  Tensor<CType> t0;
  Mem<Tensor<CType>::cloopVar> loops;
  uint apos[3];
  int lno;

  lno = InitMLoops(t1, t2, apos, loops, t0);

  CType *data0 = t0.comps.Data();
  CType *data1 = t1.comps.Data();
  CType *data2 = t2.comps.Data();

  if (!lno)
  { data0[apos[0]] = data1[apos[1]] - data2[apos[2]];
    return t0;
  }

  do
  { 
#ifdef _SHOWSUB_
    printf("\nActPos: %d, %d, %d\n", apos[0], apos[1], apos[2]);
#endif    

    data0[apos[0]] = data1[apos[1]] - data2[apos[2]];  

#ifdef _SHOWSUB_
    printf("Data0: %le\n", data0[apos[0]]);
    printf("Data1: %le\n", data1[apos[1]]);
    printf("Data2: %le\n", data2[apos[2]]);
#endif    

  } while (StepCMLoops(lno, apos, loops));


  return t0;
}

// --------------------- Sub Tensor from this Tensor ------------

template<class CType>
Tensor<CType>& operator-= (Tensor<CType>& nt1, const Tensor<CType>& nt2)
{
  return (nt1 = (nt1 - nt2));
}

// ------------------ Sub this Tensor with constant ------------------

template<class CType>
Tensor<CType>& operator-= (Tensor<CType>& t, const CType& a)
{

  CType *data = t.comps.Data();

  t.InitLoops();

  do
  { data[t.actpos] -= a;
  } while(t.StepLoops());

  return t;
}


// ------------------ Sub Tensor with constant ------------------

template<class CType>
Tensor<CType> operator- (const Tensor<CType>& nt, const CType& a)
{
  Tensor<CType> t(nt);
  CType *data = t.comps.Data();

  t.InitLoops();

  do
  { data[t.actpos] -= a;
  } while(t.StepLoops());

  return t;
}


//*************************************************************************
// Multiply Tensors without contraction ***********************************
//*************************************************************************

template<class CType>
Tensor<CType> operator& (const Tensor<CType>& nt1, const Tensor<CType>& nt2)
{
  Tensor<CType> t1(nt1), t2(nt2);
  Tensor<CType> t0;
  Mem<Tensor<CType>::cloopVar> loops;
  uint apos[3];
  int lno;

  lno = InitMLoops(t1, t2, apos, loops, t0);

  CType *data0 = t0.comps.Data();
  CType *data1 = t1.comps.Data();
  CType *data2 = t2.comps.Data();

  if (!lno)
  { data0[apos[0]] = data1[apos[1]] * data2[apos[2]];
    return t0;
  }

  do
  { 
#ifdef _SHOWADD_
    printf("\nActPos: %d, %d, %d\n", apos[0], apos[1], apos[2]);
#endif    

    data0[apos[0]] = data1[apos[1]] * data2[apos[2]];  

#ifdef _SHOWADD_
    printf("Data0: %le\n", data0[apos[0]]);
    printf("Data1: %le\n", data1[apos[1]]);
    printf("Data2: %le\n", data2[apos[2]]);
#endif    

  } while (StepCMLoops(lno, apos, loops));


  return t0;

}

//*************************************************************************
// Multiply Tensors without contraction and write into first **************
//*************************************************************************

template<class CType>
Tensor<CType>& operator&= (Tensor<CType>& nt1, const Tensor<CType>& nt2)
{
  return (nt1 = (nt1 & nt2));
}


//*************************************************************************
// Divide Tensors without contraction ***********************************
//*************************************************************************

template<class CType>
Tensor<CType> operator% (const Tensor<CType>& nt1, const Tensor<CType>& nt2)
{
  Tensor<CType> t1(nt1), t2(nt2);
  Tensor<CType> t0;
  Mem<Tensor<CType>::cloopVar> loops;
  uint apos[3];
  int lno;

  lno = InitMLoops(t1, t2, apos, loops, t0);

  CType *data0 = t0.comps.Data();
  CType *data1 = t1.comps.Data();
  CType *data2 = t2.comps.Data();

  if (!lno)
  { data0[apos[0]] = data1[apos[1]] / data2[apos[2]];
    return t0;
  }

  do
  { 
#ifdef _SHOWADD_
    printf("\nActPos: %d, %d, %d\n", apos[0], apos[1], apos[2]);
#endif    

    data0[apos[0]] = data1[apos[1]] / data2[apos[2]];  

#ifdef _SHOWADD_
    printf("Data0: %le\n", data0[apos[0]]);
    printf("Data1: %le\n", data1[apos[1]]);
    printf("Data2: %le\n", data2[apos[2]]);
#endif    

  } while (StepCMLoops(lno, apos, loops));


  return t0;

}

//*************************************************************************
// Divide Tensors without contraction and write into first ****************
//*************************************************************************

template<class CType>
Tensor<CType>& operator%= (Tensor<CType>& nt1, const Tensor<CType>& nt2)
{
  return (nt1 = (nt1 % nt2));
}
