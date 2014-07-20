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


#include"makestr.h"
#include "cstr.h"

char* StrType(int a) { return "d"; }
char* StrType(unsigned int a) { return "u"; }
char* StrType(long int a) { return "ld"; }
char* StrType(unsigned long int a) { return "lu"; }
char* StrType(float a) { return "g"; }
char* StrType(double a) { return "lg"; }
char* StrType(long double a) { return "Lg"; }


cStr MakeStr(const int &a) 
{ 
	cStr str,fs("d");
	str = (a%fs);
	return str; 
}

cStr MakeStr(const unsigned int &a) 
{ 
	cStr str, fs("u");
	str = (a%fs);
  return str; 
}

cStr MakeStr(const long &a) 
{ 
	cStr str, fs("ld");
	str = (a%fs);
  return str; 
}

cStr MakeStr(const unsigned long &a) 
{ 
	cStr str, fs("lu");
	str = (a%fs);
  return str; 
}

cStr MakeStr(const float &a) 
{ 
  int len, i,j;
  float b;
	cStr str, exp, fs1 = "e", fs2 = "f";


  b = ((a < 0.0) ? (-a) : (a));
  if( (b < 1e-3 && a != 0.0) || b > 1e3) 
  { str = (a%fs1);
    len = str.Len();
    exp = &str[len-4];
		str |= -4;

    for(i=len-5,j=0;i>=0;i--,j++)
      if(str[i] != '0' && str[i] != '.') break;
 
    if (str[i] == '.') j++;

		str |= -j;
    str << exp;    
  }
  else
  { str = (a%fs2);
    len = str.Len();

    for(i=len-1,j=0;i>=0;i--,j++)
      if (str[i] != '0') break;

    if (str[i] == '.') j++;
		str |= -j;
  }

  return str; 
}

cStr MakeStr(const double &a) 
{ 
  int len, i,j;
  double b;
	cStr str, exp, fs1 = "le", fs2 = "lf";

  b = ((a < 0.0) ? (-a) : (a));
  if((b < 1e-3 && a != 0.0) || b > 1e3) 
  { str = (a%fs1);
    len = str.Len();
    exp = &str[len-4];
    str |= -4;

    for(i=len-5,j=0;i>=0;i--,j++)
      if(str[i] != '0' && str[i] != '.') break;
 
    if (str[i] == '.') j++;
		str |= -j;
    str << exp;    
  }
  else
  { str = (a%fs2);
    len = str.Len();

    for(i=len-1,j=0;i>=0;i--,j++)
      if (str[i] != '0') break;

		if (str[i] == '.') j++;
		str |= -j;
  }

  return str; 
}

