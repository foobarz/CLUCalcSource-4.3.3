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


// Character string Library Definitions

#include "cstr.h"


// Constructor
cStr::cStr(const char *nstr)
{
  if (nstr)
  { str = strlen(nstr) + 1;
    strcpy(str.Data(), nstr);
  }
  else
  { str = 1;
    str[0] = 0;
  }

  cfill = ' ';
}

// Copy Constructor
cStr::cStr(const cStr& nstr)
{
  str = nstr.str;
  cfill = nstr.cfill;
}

// Destructor
cStr::~cStr()
{
  return;
}


/// Make Upper Case String
cStr& cStr::ToUpperCase()
{
	uint i, len = str.Count();
    char *data = str.Data();

	if (len > 0) len--;

	for(i=0;i<len;i++)
	{
		data[i] = char( toupper(int( data[i] )) );
	}

	return *this;
}

/// Make Lower Case String
cStr& cStr::ToLowerCase()
{
	uint i, len = str.Count();
    char *data = str.Data();

	if (len > 0) len--;

	for(i=0;i<len;i++)
	{
		data[i] = char( tolower(int( data[i] )) );
	}

	return *this;
}


// SubString Extraction
cStr cStr::operator() (uint fpos, uint lpos, uint cut)
{
	cStr s;
	char *data = str.Data();
	char *sdata;
	uint i,j, iLen;

	if (fpos > lpos) return s;

	s |= (iLen = lpos-fpos+1);
	sdata = s.Str();

	strncpy(sdata, &data[fpos], iLen);

//	for(i=fpos,j=0;i<lpos;i++,j++)
//	  sdata[j] = data[i];

	if (cut) str.Del(fpos, lpos-fpos+1);

	return s;
}

// Last no chars extracted
cStr cStr::Last(uint no, uint cut)
{
	cStr s;
	char *sdata;
	char *data = str.Data();
	int i,j;
	int len = Len();

	if (!no || int(no) > len) return s;

	s |= no;
	sdata = s.Str();

	for(i=len-1,j=int(no)-1;j>=0;i--,j--)
		sdata[j] = data[i];

	if (cut) str.Del(len-no, no);

	return s;
}



// First no chars extracted
cStr cStr::First(uint no, uint cut)
{
	cStr s;
	char *sdata;
	char *data = str.Data();
	uint i;
	uint len = Len();

	if (!no || no > len) return s;

	s |= no;
	sdata = s.Str();

	for(i=0;i<no;i++)
		sdata[i] = data[i];

	if (cut) str.Del(0, no);

	return s;
}





// Assignment operator
cStr& cStr::operator= (const cStr& nstr)
{
  str = nstr.str;
  cfill = nstr.cfill;

  return *this;
}

// Assignment operator
cStr& cStr::operator= (const char *nstr)
{
  if (nstr)
  { 
	  if ((str = strlen(nstr) + 1))
		strcpy(str.Data(), nstr);
  }
  else 
  { str = 1;
    str[0]= 0;
  }
   
  return *this;
}


// Assignment operator
cStr& cStr::operator= (const char a)
{
  str = 2;
  str[0] = a;
  str[1] = 0;
   
  return *this;
}


// Assignment operator
cStr& cStr::operator= (const long a)
{
  char hstr[_MAXSTRSIZE_];
  
  sprintf(hstr, "%ld", a);
  *this = hstr;

  return *this;
}

// Assignment operator
cStr& cStr::operator= (const int a)
{
  char hstr[_MAXSTRSIZE_];
  
  sprintf(hstr, "%d", a);
  *this = hstr;

  return *this;
}

// Assignment operator
cStr& cStr::operator= (const unsigned int a)
{
  char hstr[_MAXSTRSIZE_];
  
  sprintf(hstr, "%u", a);
  *this = hstr;

  return *this;
}


// Assignment operator
cStr& cStr::operator= (const double a)
{
  char hstr[_MAXSTRSIZE_];
  
  sprintf(hstr, "%lg", a);
  *this = hstr;

  return *this;
}



// Assignment operator
cStr& cStr::operator= (const long double a)
{
  char hstr[_MAXSTRSIZE_];
  
  sprintf(hstr, "%Lg", a);
  *this = hstr;

  return *this;
}

// Assignment operator
cStr& cStr::operator= (const void* a)
{
  char hstr[_MAXSTRSIZE_];
  
  sprintf(hstr, "%p", a);
  *this = hstr;

  return *this;
}


// Test whether char is a format char
int isFormat(char c)
{
  if (!strchr(_CSTR_FORMAT_CHARS_, c)) return 0;
  else return 1;
}



// Comparison Operator
int operator== (const cStr& a, const cStr& b)
{
  return (!strcmp(a.str.Data(), b.str.Data()));
}

// NOT Comparison operator. Same as strcmp
int operator!= (const cStr& a, const cStr& b)
{
  return strcmp(a.str.Data(), b.str.Data());
}

// Comparison Operator
int operator== (const cStr& a, const char* b)
{
  return (!strcmp(a.str.Data(), b));
}

// NOT Comparison operator. Same as strcmp
int operator!= (const cStr& a, const char* b)
{
  return strcmp(a.str.Data(), b);
}

// Comparison Operator
int operator== (const char* b, const cStr& a)
{
  return (!strcmp(a.str.Data(), b));
}

// NOT Comparison operator. Same as strcmp
int operator!= (const char* b, const cStr& a)
{
  return strcmp(a.str.Data(), b);
}



// Searches for a in b from left to right
int operator> (const char a, const cStr& b)
{
  char *pos;
  cStr str;

  str = b;
  
  pos = strchr(str, a);

  if (!pos) return 0;
  else return (((char *) pos - (char *) str.Str()) + 1);
 
}


// Searches for a in b from right to left
int operator< (const char a, const cStr& b)
{
  char *pos;
  cStr str;

  str = b;
  
  pos = strrchr(str, a);

  if (!pos) return 0;
  else return (((char *) pos - (char *) str.Str()) + 1);
 
}




// Capitalize string
cStr operator! (const cStr& a)
{
  cStr b;
  char *s;
  int i = 0;

  b = a;
  s = b.str.Data();

  while(s[i] != 0)
  { s[i] = toupper(s[i]);
    i++;
  }

  return b;
}


// Lower case string
cStr operator- (const cStr& a)
{
  cStr b;
  char *s;
  int i = 0;

  b = a;
  s = b.str.Data();

  while(s[i] != 0)
  { s[i] = tolower(s[i]);
    i++;
  }

  return b;
}


// Reverse String
cStr operator~ (const cStr& a)
{
  cStr b;
  char *s, c;
  int l, n, i, j;

  b = a;
  l = b.Len();
  if (l <= 1) return b;

  n = l >> 1;
  s = b.str.Data();

  for(i=0,j=l-1;i<n;i++,j--)
  { c = s[i];
    s[i] = s[j];
    s[j] = c;
  }

  return b;
}


// Concat Strings
cStr operator+ (const cStr& a, const cStr& b)
{
  cStr c, d;
  c = a;
  d = b;

  if ((c.str += strlen(d.str.Data())))
	  strcat(c.str.Data(), d.str.Data());

  return c;
}

// Concat to this String
cStr& operator+= (cStr& a, const cStr& b)
{
  cStr c;
  c = b;

  if ((a.str += strlen(c.str.Data())))
	  strcat(a.str.Data(), c.str.Data());

  return a;
}


// Concat Strings
cStr operator+ (const cStr& a, const char *b)
{
  cStr c;
  c = a;

  if ((c.str += strlen(b)))
	  strcat(c.str.Data(), b);

  return c;
}


// Concat Strings
cStr operator+ (const char *b, const cStr& a)
{
  cStr c, d;
  c = b;
  d = a;


  if ((c.str += strlen(d.str.Data())))
	  strcat(c.str.Data(), d.str.Data());

  return c;
}


// Concat to this String
cStr& operator+= (cStr& a, const char *b)
{
  if ((a.str += strlen(b)))
	  strcat(a.str.Data(), b);

  return a;
}


// add char
cStr operator+ (const cStr& a, const char b)
{
  cStr c;
  c = a;

  uint no = c.str.Count();

  c.str += 1;
  c.str[no-1] = b;
  c.str[no] = 0;

  return c;
}


// add char
cStr operator+ (const char b, const cStr& a)
{
  cStr c;

  c.str += 1;
  c.str[0] = b;
  c.str[1] = 0;

  c += a;	

  return c;
}


// Add char to this String
cStr& operator+= (cStr& a, const char b)
{
	uint no = a.str.Count();

	a.str += 1;
	a.str[no-1] = b;
	a.str[no] = 0;
  
	return a;
}


// Concat uint to string
cStr operator+ (const cStr& a, const uint b)
{
  cStr c;
  char hstr[_MAXSTRSIZE_];

  c = a;

  sprintf(hstr, "%u", b);
  c += hstr;

  return c;
}

// Concat string to uint
cStr operator+ (const uint b, const cStr& a)
{
  cStr c;
  char hstr[_MAXSTRSIZE_];

  c = a;

  sprintf(hstr, "%u", b);
  c = hstr + c;

  return c;
}

// Concat int to this string
cStr& operator+= (cStr& a, const uint b)
{
  char hstr[_MAXSTRSIZE_];

  sprintf(hstr, "%u", b);
  a += hstr;

  return a;
}


// Concat unsigned long to string
cStr operator+ (const cStr& a, const unsigned long b)
{
  cStr c;
  char hstr[_MAXSTRSIZE_];

  c = a;

  sprintf(hstr, "%lu", b);
  c += hstr;

  return c;
}

// Concat string to unsigned long
cStr operator+ (const unsigned long b, const cStr& a)
{
  cStr c;
  char hstr[_MAXSTRSIZE_];

  c = a;

  sprintf(hstr, "%lu", b);
  c = hstr + c;

  return c;
}

// Concat unsigned long to this string
cStr& operator+= (cStr& a, const unsigned long b)
{
  char hstr[_MAXSTRSIZE_];

  sprintf(hstr, "%lu", b);
  a += hstr;

  return a;
}

// Concat int to string
cStr operator+ (const cStr& a, const int b)
{
  cStr c;
  char hstr[_MAXSTRSIZE_];

  c = a;

  sprintf(hstr, "%d", b);
  c += hstr;

  return c;
}

// Concat string to int
cStr operator+ (const int b, const cStr& a)
{
  cStr c;
  char hstr[_MAXSTRSIZE_];

  c = a;

  sprintf(hstr, "%d", b);
  c = hstr + c;

  return c;
}

// Concat int to this string
cStr& operator+= (cStr& a, const int b)
{
  char hstr[_MAXSTRSIZE_];

  sprintf(hstr, "%d", b);
  a += hstr;

  return a;
}


// Concat int to string
cStr operator+ (const cStr& a, const long b)
{
  cStr c;
  char hstr[_MAXSTRSIZE_];

  c = a;

  sprintf(hstr, "%ld", b);
  c += hstr;

  return c;
}

// Concat string to int
cStr operator+ (const long b, const cStr& a)
{
  cStr c;
  char hstr[_MAXSTRSIZE_];

  c = a;

  sprintf(hstr, "%ld", b);
  c = hstr + c;

  return c;
}

// Concat int to this string
cStr& operator+= (cStr& a, const long b)
{
  char hstr[_MAXSTRSIZE_];

  sprintf(hstr, "%ld", b);
  a += hstr;

  return a;
}




// Concat double to string
cStr operator+ (const cStr& a, const double b)
{
  cStr c;
  char hstr[_MAXSTRSIZE_];

  c = a;

  sprintf(hstr, "%lg", b);
  c += hstr;

  return c;
}

// Concat string to double
cStr operator+ (const double b, const cStr& a)
{
  cStr c;
  char hstr[_MAXSTRSIZE_];

  c = a;

  sprintf(hstr, "%lg", b);
  c = hstr + c;

  return c;
}

// Concat double to this string
cStr& operator+= (cStr& a, const double b)
{
  char hstr[_MAXSTRSIZE_];

  sprintf(hstr, "%lg", b);
  a += hstr;

  return a;
}



// Concat long double to string
cStr operator+ (const cStr& a, const long double b)
{
  cStr c;
  char hstr[_MAXSTRSIZE_];

  c = a;

  sprintf(hstr, "%Lg", b);
  c += hstr;

  return c;
}

// Concat string to long double
cStr operator+ (const long double b, const cStr& a)
{
  cStr c;
  char hstr[_MAXSTRSIZE_];

  c = a;

  sprintf(hstr, "%Lg", b);
  c = hstr + c;

  return c;
}

// Concat double to this string
cStr& operator+= (cStr& a, const long double b)
{
  char hstr[_MAXSTRSIZE_];

  sprintf(hstr, "%Lg", b);
  a += hstr;

  return a;
}



// Stream cStr
cStr& operator<< (cStr& a, const cStr& b)
{
  a += b;

  return a;
}

// Stream char*
cStr& operator<< (cStr& a, const char* b)
{
  a += b;

  return a;
}

// Stream char
cStr& operator<< (cStr& a, const char b)
{
  a += b;

  return a;
}

// Stream uint
cStr& operator<< (cStr& a, const uint b)
{
  a += b;

  return a;
}

// Stream ulong
cStr& operator<< (cStr& a, const unsigned long b)
{
  a += b;

  return a;
}

// Stream int
cStr& operator<< (cStr& a, const int b)
{
  a += b;

  return a;
}

// Stream long
cStr& operator<< (cStr& a, const long b)
{
  a += b;

  return a;
}

// Stream double
cStr& operator<< (cStr& a, const double b)
{
  a += b;

  return a;
}

// Stream long double
cStr& operator<< (cStr& a, const long double b)
{
  a += b;

  return a;
}


//********************************************************************
// % operator ********************************************************
//********************************************************************

cStr& cStr::MakeFormatStr(char *ts)
{
	uint l = Len();

	if (!l) (*this = "%") + ts;
  else if (!isFormat(str[l-1])) *this = "%" + *this + ts;
  else *this = "%" + *this;

	return *this;
}

// Format variable
cStr operator% (const int a, const cStr& b)
{
  cStr str(b);
  char hstr[_MAXSTRSIZE_];

  sprintf(hstr, (char*) str.MakeFormatStr(StrType(a)), a);

  return (str = hstr);
}

// Format variable
cStr operator% (const long a, const cStr& b)
{
  cStr str(b);
  char hstr[_MAXSTRSIZE_];

  sprintf(hstr, (char*) str.MakeFormatStr(StrType(a)), a);

  return (str = hstr);
}


// Format double
cStr operator% (const double a, const cStr& b)
{
  cStr str(b);
  char hstr[_MAXSTRSIZE_];

  sprintf(hstr, (char*) str.MakeFormatStr(StrType(a)), a);

  return (str = hstr);
}


// Format unsigned int
cStr operator% (const long double a, const cStr& b)
{
  cStr str(b);
  char hstr[_MAXSTRSIZE_];

  sprintf(hstr, (char*) str.MakeFormatStr(StrType(a)), a);

  return (str = hstr);
}

// Format variable
cStr operator% (const uint a, const cStr& b)
{
  cStr str(b);
  char hstr[_MAXSTRSIZE_];

  sprintf(hstr, (char*) str.MakeFormatStr(StrType(a)), a);

  return (str = hstr);
}

// Format variable
cStr operator% (const unsigned long a, const cStr& b)
{
  cStr str(b);
  char hstr[_MAXSTRSIZE_];

  sprintf(hstr, (char*) str.MakeFormatStr(StrType(a)), a);

  return (str = hstr);
}



//********************************************************************
// * operator ********************************************************
//********************************************************************


// Multiply string n times
cStr operator* (const cStr& a, int n)
{
  cStr f, b;

  if (!n) return f;

  if (n < 0)
  { b = ~a;
    n = -n;
  }
  else b = a;

  for(int i=0;i<n;i++) 
    f += b;

  return f;
}


// Multiply string n times
cStr operator* (int n, const cStr& a)
{
  cStr f, b;

  if (!n) return f;

  if (n < 0)
  { b = ~a;
    n = -n;
  }
  else b = a;

  for(int i=0;i<n;i++) 
    f += b;

  return f;
}


// Multiply this string n times
cStr& operator*= (cStr& a, int n)
{
  cStr f, b;

  if (!n) return a;

  if (n < 0)
  { b = ~a;
    n = -n;
  }
  else b = a;

  for(int i=0;i<n;i++) 
    f += b;

  a = f;
  return a;
}


// Add or sub n fill chars to right of string
cStr operator| (const cStr& a, int n)
{
  cStr b;
  char *s;
  int f;

  b = a;
  f = b.Len();
  
  if (!n) return b;

  if (n > 0)
  { 
	  if ((b.str += n))
	  {
		s = &b.str.Data()[f];
		memset(s, b.cfill, n);
		s[n] = 0;
	  }
  }
  else if (n = -n, n < f)
    b.str.Del(f-n, n);
  else
    b = "";

  return b;
}


// Add or sub n fill chars to left of string
cStr operator| (int n, const cStr& a)
{
  cStr b;
  char *s;
  int f;

  b = a;
  f = b.Len();
  
  if (!n) return b;

  if (n > 0)
  { 
	  if ((b.str.Insert(0, n)))
		{
		s = b.str.Data();
		memset(s, b.cfill, n);
		}
  }
  else if (n = -n, n < f)
    b.str.Del(0, n);
  else
    b = "";

  return b;
}


// Add n fill chars to right of this string
cStr& operator|= (cStr& a, int n)
{
  char *s;
  int f;

  f = a.Len();
  
  if (!n) return a;

  if (n > 0)
  { 
	  if ((a.str += n))
	  {	
		  s = &a.str.Data()[f];
			memset(s, a.cfill, n);
			s[n] = 0;
	  }
  }
  else if (n = -n, n < f)
    a.str.Del(f-n, n);
  else
    a = "";

  return a;
}


// Conversion of string to uint
int operator>> (const cStr& a, uint& n)
{
  cStr b;

  b = a;
  return sscanf(b.Str(), "%u", &n);
}

// Conversion of string to ulong
int operator>> (const cStr& a, ulong& n)
{
  cStr b;

  b = a;
  return sscanf(b.Str(), "%lu", &n);
}

// Conversion of string to int
int operator>> (const cStr& a, int& n)
{
  cStr b;

  b = a;
  return sscanf(b.Str(), "%d", &n);
}

// Conversion of string to long
int operator>> (const cStr& a, long& n)
{
  cStr b;

  b = a;
  return sscanf(b.Str(), "%ld", &n);
}

// Conversion of string to float
int operator>> (const cStr& a, float& n)
{
  cStr b;

  b = a;
  return sscanf(b.Str(), "%g", &n);
}

// Conversion of string to double
int operator>> (const cStr& a, double& n)
{
  cStr b;

  b = a;
  return sscanf(b.Str(), "%lg", &n);
}

// Conversion of string to long double
int operator>> (const cStr& a, long double& n)
{
  cStr b;

  b = a;
  return sscanf(b.Str(), "%Lg", &n);
}

// Conversion of string to void pointer
int operator>> (const cStr& a, void*& n)
{
  cStr b;

  b = a;
  return sscanf(b.Str(), "%p", &n);
}





