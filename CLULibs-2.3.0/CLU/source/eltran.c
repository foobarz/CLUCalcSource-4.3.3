#include <stdio.h>
#include "eispack.h"
#define FALSE 0
#define TRUE 1

void eltran(int n, int lo, int hi, double **a,int intch[],double **z)
{
/* This routine is a translation of the Algol procedure from
 * Handbook for Automatic Computation, vol. II, Linear Algebra,
 * by Wilkinson and Reinsch, Springer-Verlag.
 */
	double x,y;
	int i,j,kl,la,m,mm,mp,mp1;
	
/* Initialize z to identity matrix. */
	for (j = 0; j < n; j++) {
		for (i = 0; i < n; i++)
			z[i][j] = 0.0;
		z[j][j] = 1.0;
	}
	kl = hi - lo - 1;
	if (kl < 1)
		return;

	for (mm = 1; mm <= kl; mm++) {
		mp = hi - mm;
		mp1 = mp + 1;
		for (i = mp1; i <= hi; i++) {
			z[i][mp] = a[i][mp-1];
		}
		i = intch[mp];
		if (i != mp)
		{
			for (j = mp; j <= hi; j++) 
			{
				z[mp][j] = z[i][j];
				z[i][j] = 0.0;
			}
			z[i][mp] = 1.0;
		}
	}
}


