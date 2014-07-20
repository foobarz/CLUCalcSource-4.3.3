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


//#include "tensor.h"

// included from xutlib. Set path appropriately
#include "mem.cxx"
/*
template class Mem<Tensor<int>::idxProp>;
template class Mem<Tensor<int>::loopVar>;
template class Mem<Tensor<int>::cloopVar>;
template class Mem<Tensor<int>::rangeType>;

template class Mem<Tensor<float>::idxProp>;
template class Mem<Tensor<float>::loopVar>;
template class Mem<Tensor<float>::cloopVar>;
template class Mem<Tensor<float>::rangeType>;

template class Mem<Tensor<double>::idxProp>;
template class Mem<Tensor<double>::loopVar>;
template class Mem<Tensor<double>::cloopVar>;
template class Mem<Tensor<double>::rangeType>;
*/

#include"matrix.cxx"
//#include"tensor.cxx"

////////////////////////////////////////////////////////////////////////////////////
// int instantiation

Instantiate_Matrix(int);
//Instantiate_Tensor(int);


////////////////////////////////////////////////////////////////////////////////////
// float instantiation

Instantiate_Matrix(float);
//Instantiate_Tensor(float);

////////////////////////////////////////////////////////////////////////////////////
// double instantiation

Instantiate_Matrix(double);
//Instantiate_Tensor(double);

#include "TensorData.cxx"
#include "TensorIdx.cxx"
#include "TensorSingleLoop.cxx"
#include "TensorDoubleLoop.cxx"
#include "TensorContractLoop.cxx"
#include "TensorPointLoop.cxx"
#include "TensorOperators.cxx"

template class CTensorData<double>;
template class CTensorIdx<double>;
template class CTensorSingleLoop<double>;
template class CTensorDoubleLoop<double>;
template class CTensorContractLoop<double>;
template class CTensorPointLoop<double>;

InstantiateTensorOperators(double);

////////////////////////////////////////////////////////////////////////////////////

// The first two number give major and minor version.
// The last three numbers give the date in the format day,month,year.
unsigned _xmalib_version[5] = {1,2,23,4,1999};

char *_xmalib_license_text = 
"This program uses the CLU Library\n"
"Version 1.2, April 1999\n\n"
//
"The library implements classes for Matrix and Tensor operations\n"
"and more.\n\n"
//
"Copyright (C) 1997-2001  Christian B.U. Perwass\n\n"
//
"This library is free software; you can redistribute it and/or\n"
"modify it under the terms of the GNU Library General Public\n"
"License as published by the Free Software Foundation; either\n"
"version 2 of the License, or (at your option) any later version.\n\n"
//
"This library is distributed in the hope that it will be useful,\n"
"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n"
"Library General Public License for more details.\n\n"
//
"You should have received a copy of the GNU Library General Public\n"
"License along with this library; if not, write to the\n"
"Free Software Foundation, Inc., 59 Temple Place - Suite 330,\n"
"Boston, MA  02111-1307, USA.\n\n"
//
"The GNU Library General Public License can be found in the file\n"
"license.txt distributed with this library.\n\n"
//
"Please send all queries and comments regarding the XMA Library to\n\n"
//
"email: public@perwass.com\n"
"mail: Institut fuer Informatik, Preusserstr. 1-9, 24105 Kiel, Germany.\n\n";

