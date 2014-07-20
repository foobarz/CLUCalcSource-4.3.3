#include <stdio.h>
#include "eispack.h"
#define FALSE 0
#define TRUE 1

void balbak(int n,int lo, int hi, double *scale,int m,double **z)
{
/* This routine is a translation of the Algol procedure from
 * Handbook for Automatic Computation, vol. II, Linear Algebra,
 * by Wilkinson and Reinsch, Springer-Verlag.
 */
	double s;
	int i,j,k,ii;

	m--;		/* m is handled as a FORTRAN index variable */
	if (m == 0)
		return;

	if (hi != lo)
	{
		for(i = lo; i <= hi; i++) 
		{
			s = scale[i];

			/* Left hand eigenvectors are back transformed if the foregoing
			* statement is replaced by s = 1.0 / scale[i] ...
			*/
			for (j = 0; j <= m; j++)
				z[i][j] *= s;
		}
	}

 	for (ii = 0; ii < n; ii++) 
	{
 		i = ii;
 		if (!((i >= lo) && ( i <= hi)))
		{
 			if (i < lo)
	 			i = lo - ii;
 			k = scale[i];
 			if (k != i)
			{
	 			for (j = 0;  j <= m; j++) 
				{
	 				s = z[i][j];
 					z[i][j] = z[k][j];
 					z[k][j] = s;
 				}
			}
		}
	}
}


