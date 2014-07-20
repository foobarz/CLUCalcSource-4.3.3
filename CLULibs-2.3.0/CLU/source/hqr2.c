#include <stdio.h>
#include <math.h>
#include "eispack.h"

#define min(a,b)	(((a) < (b)) ? (a) : (b))
#define max(a,b)	(((a) > (b)) ? (a) : (b))
#define TRUE 1
#define FALSE 0

/* This subroutine is a translation of the ALGOL procedure HQR2,
 * Num. Math. 14, 219,231(1970) by Martin, Peters, and Wilkinson.
 * Handbook for Automatic Computation, vol. II - Linear Algebra,
 * pp. 357-391 (1971).
 *
 * FORTRAN version from EISPACK (dated 1983) was used as model.
 */

void cdiv(double ar,double ai,double br,double bi,double *cr,double *ci)
{
	double s,ars,ais,brs,bis;
	
    s = fabs(br) + fabs(bi);
	ars = ar / s;
	ais = ai / s;
	brs = br / s;
	bis = bi / s;
	s = brs * brs + bis * bis;
	*cr = (ars * brs + ais * bis) / s;
	*ci = (ais * brs - ars * bis) / s;
}
 
void hqr2(int n,int lo,int hi,double **h,double wr[],double wi[],
	double **z,int *ierr)
{
    double norm,p,q,r,ra,s,sa,t,tst1,tst2,vr,vi,w,x,y,zz,sgn;
	int en,enm2,i,ii,itn,its,j,jj,k,l,ll,m,mm,mp2,nn,na,notlas;
	
	*ierr = 0;
	norm = 0.0;
	k = 0;		/* Used for array indexing. FORTRAN version: k=1 */
	
/* Store roots isolated by balance, and compute matrix norm. */
	for (i = 0; i < n; i++) {
		for (j = k; j < n; j++)
            norm += fabs(h[i][j]);
		k = i;
		if (!((i >= lo) && (i <= hi)))
		{
			wr[i] = h[i][i];
			wi[i] = 0.0;
		}
	}
	en = hi;
	t = 0.0;
	itn = 30 * n;

/* Search for next eigenvalues. */
_60:
	if (en < lo) goto _340;
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
		printf("%20.16g %20.16g %20.16g\n",h[0][0],h[0][1],h[0][2]);
		printf("%20.16g %20.16g %20.16g\n",h[1][0],h[1][1],h[1][2]);
		printf("%20.16g %20.16g %20.16g\n",h[2][0],h[2][1],h[2][2]);
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
        sgn = (p<0)?-1:(p>0);
        s = sgn*sqrt(p*p+q*q+r*r);
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
		for (j = k; j < n; j++) {
			p = h[k][j] + q * h[k+1][j];
			h[k][j] -= (p * x);
			h[k+1][j] -= (p * y);
		}
		j = min(en,k+3);

/* Column modification. */
		for (i = 0; i <= j; i++) {
			p = x * h[i][k] + y * h[i][k+1];
			h[i][k] -= p;
			h[i][k+1] -= (p * q);
		}
/* Accumulate transformations. */
		for (i = lo;i <= hi; i++) {
			p = x * z[i][k] + y * z[i][k+1];
			z[i][k] -= p;
			z[i][k+1] -= (p * q);
		}
		goto _255;
_225:

/* Row modification. */
		for (j = k; j < n; j++) {
			p = h[k][j] + q * h[k+1][j] + r * h[k+2][j];
			h[k][j] -= (p * x);
			h[k+1][j] -= (p * y);
			h[k+2][j] -= (p * zz);
		}
		j = min(en,k+3);
		
/* Column modification. */
		for (i = 0; i <= j; i++) {
			p = x * h[i][k] + y * h[i][k+1] + zz * h[i][k+2];
			h[i][k] -= p;
			h[i][k+1] -= (p * q);
			h[i][k+2] -= (p * r);
		}
/* Accumulate transformations. */
		for (i = lo; i <= hi; i++) {
			p = x * z[i][k] + y * z[i][k+1] + zz * z[i][k+2];
			z[i][k] -= p;
			z[i][k+1] -= (p * q);
			z[i][k+2] -= (p * r);
		}
			
_255:;
_260:;
	}	
	goto _70;

/* One root found. */
_270:
	h[en][en] = x + t;
	wr[en] = h[en][en];
	wi[en] = 0.0;
	en = na;
	goto _60;

/* Two roots found. */
_280:
	p = (y - x) / 2.0;
	q = p * p + w;
    zz = sqrt(fabs(q));
	h[en][en] = x + t;
	x = h[en][en];
	h[na][na] = y + t;
	if ( q < 0.0)
		goto _320;

/* Real pair. */
    sgn = (p<0)?-1:(p>0);
    zz = p + sgn*zz;
	wr[na] = x + zz;
	wr[en] = wr[na];
	if (zz != 0.0)
		wr[en] = x - w / zz;
	wi[na] = 0.0;
	wi[en] = 0.0;
	x = h[en][na];
    s = fabs(x) + fabs(zz);
	p = x / s;
	q = zz / s;
	r = sqrt(p * p + q * q);
	p /= r;
	q /= r;

/* Row modification. */
	for (j = na;j < n; j++) {
		zz = h[na][j];
		h[na][j] = q * zz + p * h[en][j];
		h[en][j] = q * h[en][j] - p * zz;
	}

/* Column modification. */
	for (i = 0; i <= en; i++) {
		zz = h[i][na];
		h[i][na] = q * zz + p * h[i][en];
		h[i][en] = q * h[i][en] - p * zz;
	}
/* Accumulate transformations. */
	for (i = lo; i <= hi; i++) {
		zz = z[i][na];
		z[i][na] = q * zz + p * z[i][en];
		z[i][en] = q * z[i][en] - p * zz;
	}
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

/* All roots found. Backsubstitute to find vectors of upper
 * triangular form. */
_340:
 	if (norm == 0.0)
		goto _1001;
	for (nn = 0; nn < n; nn++) {
		en = n - nn - 1;
		p = wr[en];
		q = wi[en];
		na = en - 1;
		if (q < 0.0) goto _710;
		if (q > 0.0) goto _800;
		
/* Real vector. */
		m = en;
		h[en][en] = 1.0;
		if (na < 0)
			goto _800;
		for (ii = 0; ii <= na; ii++) {
			i = en - ii - 1;
			w = h[i][i] - p;
			r = 0.0;
			for (j = m; j <= en; j++)
				r += (h[i][j] * h[j][en]);
			if (wi[i] >= 0.0)
				goto _630;
			zz = w;
			s = r;
			goto _700;
_630:
			m = i;
			if (wi[i] != 0.0)
				goto _640;
			t = w;
			if (t != 0.0)
				goto _635;
			tst1 = norm;
			t = tst1;
_632:
			t *= 0.01;
			tst2 = norm + t;
			if (tst2 > tst1)
				goto _632;	  			
_635:
			h[i][en] = -r / t;
			goto _680;

/* Solve real equations. */
_640:
			x = h[i][i+1];
			y = h[i+1][i];
			q = (wr[i] - p) * (wr[i] - p) + wi[i] * wi[i];
			t = (x * s - zz * r) / q;
			h[i][en] = t;
            if (fabs(x) <= fabs(zz))
				goto _650;
			h[i+1][en] = (-r - w * t) / x;
			goto _680;
_650:
			h[i+1][en] = (-s - y * t) / zz;

/* Overflow control. */
_680:
            t = fabs(h[i][en]);
			if (t == 0.0)
				goto _700;
			tst1 = t;
			tst2 = tst1 + 1.0 / tst1;
			if (tst2 > tst1)
				goto _700;
			for (j = i; j <= en; j++)
				h[j][en] /= t;
_700:;
		}
/* End real vector. */
		goto _800;

/* Complex vector. */
_710:
		m = na;

/* Last vector component chosen imaginary so that eigenvector matrix
 * is triangular. */
        if (fabs(h[en][na]) <= fabs(h[na][en]))
 			goto _720;
	 	h[na][na] = q / h[en][na];
 		h[na][en] = -(h[en][en] - p) / h[en][na];
 		goto _730;
_720:
		cdiv(0.0,-h[na][en],h[na][na]-p,q,&h[na][na],&h[na][en]);
_730:
		h[en][na] = 0.0;
		h[en][en] = 1.0;
		enm2 = na - 1;
		if (enm2 < 0)
			goto _800;
		for (ii = 0; ii <= enm2; ii++) {
			i = na - ii - 1;
			w = h[i][i] - p;
			ra = 0.0;
			sa = 0.0;
			for (j = m; j <= en; j++) {
				ra += (h[i][j] * h[j][na]);
				sa += (h[i][j] * h[j][en]);
			}
			if (wi[i] >= 0.0)
				goto _770;
			zz = w;
			r = ra;
			s = sa;
			goto _795;
_770:
			m = i;
			if (wi[i] != 0.0)
				goto _780;
			cdiv(-ra,-sa,w,q,&h[i][na],&h[i][en]);
			goto _790;

/* Solve complex equations. */
_780:
			x = h[i][i+1];
			y = h[i+1][i];
			vr = (wr[i] - p) * (wr[i] - p) + wi[i] * wi[i] - q * q;
			vi = (wr[i] - p) * 2.0 * q;
			if ((vr != 0.0) || (vi != 0.0))
				goto _784;
            tst1 = norm * (fabs(w) + fabs(q) + fabs(x) +
                fabs(y) + fabs(zz));
			vr = tst1;
_783:
			vr *= 0.01;
			tst2 = tst1 + vr;
			if (tst2 > tst1)
				goto _783;
_784:
			cdiv(x*r-zz*ra+q*sa,x*s-zz*sa-q*ra,vr,vi,&h[i][na],&h[i][en]);
            if (fabs(x) <= (fabs(zz) + fabs(q)))
				goto _785;
			h[i+1][na] = (-ra - w * h[i][na] + q * h[i][en]) / x;
			h[i+1][en] = (-sa - w * h[i][en] - q * h[i][na]) / x;
			goto _790;
_785:		
			cdiv(-r-y*h[i][na],-s-y*h[i][en],zz,q,
				&h[i+1][na],&h[i+1][en]);

/* Overflow control. */
_790:
            t = max(fabs(h[i][na]),fabs(h[i][en]));
			if (t == 0.0)
					goto _795;
			tst1 = t;
			tst2 = tst1 + 1.0 / tst1;
			if (tst2 > tst1)
				goto _795;
			for (j = i; j <= en; j++) {
				h[j][na] /= t;
				h[j][en] /= t;
			}
_795:;
		}		
/* End complex vector. */
_800:;
	}
/* End back substitution. Vectors of isolated roots. */
	for (i = 0; i < n; i++) {
		if ((i >= lo) && (i <= hi))
			goto _840;
		for (j = i; j < n; j++)
			z[i][j] = h[i][j];
_840:;
	}
	
/* Multiply by transformation matrix to give vectors of original
 * full matrix. */
 	for (jj = lo; jj < n; jj++) {
		j = n + lo - jj - 1;
		m = min(j,hi);
		for (i = lo; i <= hi; i++) {
			zz = 0.0;
			for (k = lo; k <= m; k++)
				zz += (z[i][k] * h[k][j]);
			z[i][j] = zz;
 		}
 	}
 	goto _1001;
/* Set error -- all eigenvalues have not converged after 30*n iterations. */
_1000:
	*ierr = en;
_1001:
	return;
}
