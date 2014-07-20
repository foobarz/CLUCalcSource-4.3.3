#include <stdio.h>
#include "eispack.h"
#define FALSE 0
#define TRUE 1

void elmbak(int lo, int hi, double **a,int intch[],int m,double **z)
{
/* This routine is a translation of the Algol procedure from
 * Handbook for Automatic Computation, vol. II, Linear Algebra,
 * by Wilkinson and Reinsch, Springer-Verlag.
 */
	double x;
	int i,j,kp1,la,mm,mp,mp1;
	
	if (m == 0)
		return;

	la = hi - 1;
	kp1 = lo + 1;
	if (la < kp1)
		return;

	for (mm = kp1; mm <= hi; mm++) 
	{
		mp = lo + hi - mm;
		mp1 = mp + 1;
		for (i = mp1;i <= hi; i++) 
		{
			x = a[i][mp-1];
			if (x != 0.0)
			{
				for (j = 0; j <= m; j++)
					z[i][j] += (x * z[mp][j]);
			}
		}
		i = intch[mp];
		if (i != mp)
		{
			for (j = 0; j <= m; j++) 
			{
				x = z[i][j];
				z[i][j] = z[mp][j];
				z[mp][j] = x;
			}
		}
	}
}


