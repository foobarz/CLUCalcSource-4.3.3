#include <stdio.h>
#include <math.h>
#include "eispack.h"
#define FALSE 0
#define TRUE 1

void elmhes(double **a,int n, int lo, int hi, int *intch)
{
/* This routine is a translation of the Algol procedure from
 * Handbook for Automatic Computation, vol. II, Linear Algebra,
 * by Wilkinson and Reinsch, Springer-Verlag.
 */
    double x,y,dtmp;
	int i,j,kp1,la,m,mm1,mp1;
	
	la = hi - 1;
	kp1 = lo + 1;
	if (la < kp1)
		return;

	for (m = kp1; m <= la; m++) 
	{
		mm1 = m - 1;
		x = 0;
		i = m;
		for (j = m; j <= hi; j++) 
		{
            if (!(fabs(a[j][mm1]) <= fabs(x)))
			{
				x = a[j][mm1];
				i = j;
			}
		}
		intch[m] = i;
		if (i != m)
		{
/* Interchange rows and columns of array a. */
	        for (j = mm1; j < n; j++) 
			{
	            dtmp = a[i][j];
				a[i][j] = a[m][j];
				a[m][j] = dtmp;
			}
			for (j = 0; j <= hi; j++) 
			{
	            dtmp = a[j][i];
				a[j][i] = a[j][m];
				a[j][m] = dtmp;
			}
		}

		if (x != 0.0)
		{
			mp1 = m + 1;
			for (i = mp1; i <= hi;i++) 
			{
				y = a[i][mm1];
				if (y != 0)
				{
					y /= x;
					a[i][mm1] = y;
					for (j = m; j < n; j++)
						a[i][j] -= y * a[m][j];
					for (j = 0; j <=hi; j++)
						a[j][m] += y * a[j][i];
				}
			} /* endfor  i */ 
		}
	} /* endfor m */
}


