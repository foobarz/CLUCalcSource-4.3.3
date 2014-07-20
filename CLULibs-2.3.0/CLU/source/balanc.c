#include <stdio.h>
#include <math.h>
#include "eispack.h"
#define FALSE 0
#define TRUE 1

void balanc(double **a,int n, int *lo, int *hi, double scale[])
{
/* This routine is a translation of the Algol procedure from
 * Handbook for Automatic Computation, vol. II, Linear Algebra,
 * by Wilkinson and Reinsch, Springer-Verlag.
 */
    double c,f,g,r,s,radix,b2,dtmp;
	int i,j,k,l,m,noconv;
	
	radix = 2.0;
	b2 = radix * radix;
	l = 0;
	k = n-1;
	
/* Search for rows isolating an eigenvalue and push them down. */
L1:
	if (k == 0) {
		*lo = l;
		*hi = k;
		return;
	}
	for (j = k; j >= 0; j--) {
		r = 0;
		for (i = 0; i <= k; i++) {
			if (i == j) continue;
			if (a[j][i] != 0.0) r = 1;
		}
		if (r == 0) {
			scale[k] = j;
			if (j != k) {
                for (i = 0; i <= k; i++) {
                    dtmp = a[i][j];
                    a[i][j] = a[i][k];
                    a[i][k] = dtmp;
                }
                for (i = l;i < n; i++) {
                    dtmp = a[j][i];
                    a[j][i] = a[k][i];
                    a[k][i] = dtmp;
                }
			}	
			k--;
			goto L1;
		}
	}
/* Search for columns isolating an eigenvalue and push them left. */
L2:
	for (j = l; j <= k; j++) {
		c = 0;
		for (i = l; i <= k; i++) {
			if (i == j) continue;
			if (a[i][j] != 0.0) c = 1;
		}
		if (c == 0) {
			scale[l] = j;
			if (j != l) {
                for (i = 0; i <= k; i++) {
                    dtmp = a[i][j];
                    a[i][j] = a[i][l];
                    a[i][l] = dtmp;
                }
                for (i = l;i < n; i++) {
                    dtmp = a[j][i];
                    a[j][i] = a[l][i];
                    a[l][i] = dtmp;
                }
			}
			l++;
			goto L2;
		}
	}
/* Now balance the submatrix in rows l through k. */
	*lo = l;
	*hi = k;
	for (i = l; i <= k; i++)
		scale[i] = 1.0;
iteration:
	noconv = FALSE;
	for (i = l; i <= k; i++) {
		c = 0.0;
		r = 0.0;
		for (j = l; j <= k; j++) {
			if (j == i) continue;
            c += fabs(a[j][i]);
            r += fabs(a[i][j]);
		}
/* Guard against zero c or r due to underflow. (In FORTRAN version only!) */
		if ((c == 0.0) || (r == 0.0)) continue;
		g = r / radix;
		f = 1.0;
		s = c + r;
L3:
		while (c < g) {
			f *= radix;
			c *= b2;
		}
		g = r * radix;
L4:
		while (c >= g) {
			f /= radix;
			c /= b2;
		}
/* The preceding lines may be replaced by a machine language procedure
 * computing the exponent sig such that 
 *	sqrt(r/(c * radix)) <= radix^sig < sqrt(r*radix)/c).
 * Now balance.
 */
 		if (((c + r)/f) < (0.95 * s)) {
 			g = 1.0 / f;
 			scale[i] *= f;
 			noconv = TRUE;
 			for (j = l; j < n; j++) 
 				a[i][j] *= g;
 			for (j = 0; j <= k; j++)
 				a[j][i] *= f;
 		}
/* The j loops may be done by exponent modification in machine language. */
	}
	if (noconv) goto iteration;
}
