#include <stdio.h>
#include <math.h>
//#include "eispack.h"

#define min(a,b)	(((a) < (b)) ? (a) : (b))
#define max(a,b)	(((a) > (b)) ? (a) : (b))
#define TRUE 1
#define FALSE 0

/* This subroutine is a translation of the ALGOL procedure HQR,
 * Num. Math. 14, 219,231(1970) by Martin, Peters, and Wilkinson.
 * Handbook for Automatic Computation, vol. II - Linear Algebra,
 * pp. 357-391 (1971).
 *
 * The FORTRAN version from EISPACK (dated 1983) was used as a model,
 * warts and all.
 */
 
void hqr(int n,int lo,int hi,double **h,double *wr,double *wi,int *ierr)
{
	double norm,p,q,r,s,t,tst1,tst2,w,x,y,zz;
	int en,enm2,i,itn,its,j,k,l,ll,m,mm,mp2,na,notlas;
	
	*ierr = 0;
	norm = 0.0;
	k = 0;		/* Used for array indexing. FORTRAN version: k=1 */
	
/* Store roots isolated by balance. */
	for (i = 0; i < n; i++) {
		for (j = k; j < n; j++)
            norm += fabs(h[i][j]);
		k = i;
		if ((i >= lo) && (i <= hi))			
			goto _50;
		wr[i] = h[i][i];
		wi[i] = 0.0;
_50:;
	}
	en = hi;
	t = 0.0;
	itn = 30 * n;

/* Search for next eigenvalues. */
_60:
	if (en < lo) goto _1001;
	its = 0;
	na = en - 1;
	enm2 = na - 1;

/* Look for single small sub-diagonal element. */
_70:
	for (ll = lo;ll <= en; ll++) {
		l = en + lo - ll;
		if (l == lo) goto _100;
        s = fabs(h[l-1][l-1]) + fabs(h[l][l]);
		if (s == 0.0)
			s = norm;
		tst1 = s;
        tst2 = tst1 + fabs(h[l][l-1]);
		if (tst2 == tst1) goto _100;
	}

/* Form shift. */
_100:
	x = h[en][en];
	if (l == en)
		goto _270;
	y = h[na][na];
	w = h[en][na] * h[na][en];
	if (l == na)
		goto _280;
	if (itn == 0)
		goto _1000;
	if ((its != 10) && (its != 20))
		goto _130;		

/* Form exceptional shift. */
	t += x;
	for (i = lo; i <= en; i++)
		h[i][i] -= x;
    s = fabs(h[en][na]) + fabs(h[na][enm2]);
	x = 0.75 * s;
	y = x;
	w = -0.4375 * s * s;
_130:
	its++;
	itn--;
/* Look for two consecutive small sub-diagonal elements. */
	for (mm = l;mm <= enm2; mm++) {
		m = enm2 + l - mm;
		zz = h[m][m];
		r = x - zz;
		s = y - zz;
		p = (r * s - w) / h[m+1][m] + h[m][m+1];
		q = h[m+1][m+1] - zz - r - s;
		r = h[m+2][m+1];
        s = fabs(p) + fabs(q) + fabs(r);
		p /= s;
		q /= s;
		r /= s;
		if (m == l)
			goto _150;
        tst1 = fabs(p) * (fabs(h[m-1][m-1]) + fabs(zz) +
            fabs(h[m+1][m+1]));
        tst2 = tst1 + fabs(h[m][m-1]) * (fabs(q) + fabs(r));
		if (tst2 == tst1)
			goto _150;
	}
_150:
	mp2 = m + 2;
	for (i = mp2; i <= en; i++) {
		h[i][i-2] = 0.0;
		if (i == mp2)
			goto _160;
		
		h[i][i-3] = 0.0;
_160:;
	}
/* Double QR step involving rows l to en and columns m to en. */
	for (k = m; k <= na; k++) {
		notlas = (k != na);
		if (k == m)
			goto _170;
		p = h[k][k-1];
		q = h[k+1][k-1];
		r = 0.0;
		if (notlas)
			r = h[k+2][k-1];
        x = fabs(p) + fabs(q) + fabs(r);
		if (x == 0.0)
			goto _260;
		p /= x;
		q /= x;
		r /= x;
_170:
        s = sqrt(p*p+q*q+r*r);
        if (p < 0.0) s = -s;
		if (k == m)
			goto _180;
		h[k][k-1] = -s * x;
		goto _190;
_180:
		if (l != m)
			h[k][k-1] = -h[k][k-1];
_190:
		p += s;
		x = p / s;
		y = q / s;
		zz = r / s;
		q /= p;
		r /= p;		
		if (notlas)
			goto _225;
/* Row modification. */
        for (j = k; j <= en; j++) {
			p = h[k][j] + q * h[k+1][j];
			h[k][j] -= (p * x);
			h[k+1][j] -= (p * y);
		}
		j = min(en,k+3);

/* Column modification. */
        for (i = l; i <= j; i++) {
			p = x * h[i][k] + y * h[i][k+1];
			h[i][k] -= p;
			h[i][k+1] -= (p * q);
		}
		goto _255;
_225:

/* Row modification. */
        for (j = k; j <= en; j++) {
			p = h[k][j] + q * h[k+1][j] + r * h[k+2][j];
			h[k][j] -= (p * x);
			h[k+1][j] -= (p * y);
			h[k+2][j] -= (p * zz);
		}
		j = min(en,k+3);
		
/* Column modification. */
        for (i = l; i <= j; i++) {
			p = x * h[i][k] + y * h[i][k+1] + zz * h[i][k+2];
			h[i][k] -= p;
			h[i][k+1] -= (p * q);
			h[i][k+2] -= (p * r);
		}
_255:;
_260:;
	}
		goto _70;
	
/* One root found. */
_270:
		wr[en] = x + t;
		wi[en] = 0.0;
		en = na;
		goto _60;

/* Two roots found. */
_280:
		p = (y - x) / 2.0;
		q = p * p + w;
        zz = sqrt(fabs(q));
		x += t;
		if ( q < 0.0)
			goto _320;

/* Real pair. */
        zz = fabs(zz);
        if (p < 0.0) zz = -zz;
        zz += p;
		wr[na] = x + zz;
		wr[en] = wr[na];
		if (zz != 0.0)
			wr[en] = x - w / zz;
		wi[na] = 0.0;
		wi[en] = 0.0;
		goto _330;

/* Complex pair. */
_320:
		wr[na] = x + p;
		wr[en] = x + p;
		wi[na] = zz;
		wi[en] = -zz;
_330:
		en = enm2;
		goto _60;
	
/* Set error -- all eigenvalues have not converged after 30*n iterations. */
_1000:
	*ierr = en;
_1001:
	return;
}
