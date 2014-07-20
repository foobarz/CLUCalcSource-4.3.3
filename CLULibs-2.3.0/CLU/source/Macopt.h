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


//////////////////////////////////////////////////////////////////////////////////
/// Header File for MacOpt
//////////////////////////////////////////////////////////////////////////////////


#ifndef MACOPT_H
#define MACOPT_H

#include "xutlib.h"

#include "MinFuncBase.h"

//////////////////////////////////////////////////////////////////////////////////
/// Error Messages

#define	ERR_NONE			0	// No Error
#define ERR_MEM				1	// Not enough memory
#define ERR_WRONGFUNC		2	// Function cannot be used in minimiser
#define ERR_FUNCEVAL		3	// Error in function evaluation

#define ERR_STR_NONE			"No Error."
#define ERR_STR_MEM				"Not enough memory."
#define ERR_STR_WRONGFUNC		"Function cannot be used by MacOpt."
#define ERR_STR_FUNCEVAL		"Error in function evaluation."

//////////////////////////////////////////////////////////////////////////////////
/// Notification Messages

#define MSG_NONE			0	// No Message
#define MSG_VB1				1	// Verbose 1
#define MSG_VB2				2	// Verbose 2
#define MSG_VB3				3	// Verbose 3
#define MSG_VB4				4	// Verbose 4
#define MSG_WARN			10	// Warning Messages
#define MSG_CHECK			11	// Checking Messages


//////////////////////////////////////////////////////////////////////////////////


// MacOpt Parameters
struct TMacOptAttribs
{
	uint verbose;
	double tolerance;
	uint itmax;
	bool rich;

	// The following influence stepping behaviour of LinMin
	double linmin_g[3];
	// Maximum Stepsize
	double stepmax;

	TMacOptAttribs()
	{ verbose = 0; tolerance = 1e-4; itmax = 100; rich = 1; 
	  linmin_g[0] = 1.5; linmin_g[1] = 1.25; linmin_g[2] = 0.5; 
	  stepmax = 0.5;
	}
};

class MacOpt 
{
public:

	MacOpt();
	MacOpt(MinFuncBase& _minFunc, const TMacOptAttribs &sAttrib);

	~MacOpt();

	bool Init(MinFuncBase& _minFunc, const TMacOptAttribs &sAttrib);

	bool MacOptII(double *p); 
	bool MacCheckGrad(double *p,
		    double epsilon, 
		    int stopat);	

	void EnableRecordSteps(bool b = true) { bRecordSteps = b; }
	double AdjustStepSize(double x);

	Mem<double>& GetStepRecord() { return mStepRec; }
	StrMessageList& GetMsgList() { return msgList; }

	void ResetErrors() { errList.ResetList(); }
	void ResetMessages() { msgList.ResetList(); }

protected:
	void SetFuncEvalError(uint deriv, double *p);

private:
	double MacLinMinII(double *p);
	double MacProdII (double * , double * , double ) ;
	void MacOptRestart ( int ) ;

protected:
	StrMessageList msgList, errList;

	Mem<double> mStepRec;
	bool bRecordSteps;

private:
	
	MinFuncBase *minFunc;

	int a_n;     /* dimension of parameter space */
	
	double a_tol ;    /* convergence declared when the gradient vector is smaller
					   in magnitude than this, or when the mean absolute 
					 step is less than this (see above) */
	double a_grad_tol_tiny ; /* if gradient is less than this, we definitely 
										  stop, even if we are not using a gradient 
							 tolerance */
	double a_step_tol_tiny ; /* if step is less than this, we stop, even if 
								 we are not using a step tolerance */
	int a_end_if_small_step ; /* defines the role of tol -- alternative is
							 end_on_small_grad */
	int a_its ;               /* number of its */
	int a_itmax ;             /* max */
	bool a_rich ; /* whether to do the extra gradient evaluation at the beginning 
				   of each new line min */
	uint a_verbose ; 
	double a_stepmax ;        /* largest step permitted (not used in macopt) */
										   
	int a_linmin_maxits ;     /* in maclinmin */
	double a_linmin_g1 ;      /* factors for growing and shrinking the interval */
	double a_linmin_g2 ;
	double a_linmin_g3 ;
	double a_lastx     ;      /* keeps track of typical step length */
	double a_lastx_default ;  /* if maclinmin is reset, lastx is set to this */
									   
	/* These should not be touched by the user. They are handy pointers for macopt
	 to use  */
	double a_gtyp ; /* stores the rms gradient for linmin */
	double *a_pt , *a_gx , *a_gy , *a_gunused ;
	double *a_xi , *a_g , *a_h ; 
	int a_restart ;           /* whether to restart macopt - fresh cg directions */
									   
	/* a_lastx :--- 1.0 might make general sense, (cf N.R.)
	 but the best setting of all is to have 
	 a prior idea of the eigenvalues. If 
	 the objective function is equal to sum of N
	 terms then set this to 1/N, for example 
	 Err on the small side to be conservative. */

};



#endif

