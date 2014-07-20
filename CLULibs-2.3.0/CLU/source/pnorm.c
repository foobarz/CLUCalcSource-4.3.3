#include <stdio.h>
#include <math.h>
#include "eispack.h"

#define max(a,b)    (((a) > (b)) ? (a) : (b))

void pnormwr(int n,double **z,double *wr,double *wi)
{
    double emax,ei,t,xi,xr,yi,yr,zi,zr;
    int i,j,k;
	
    for (i=0;i < n;i++) {
        if (wi[i] == 0.0) {  /* eigenvector is real */
/* find max value in current column (eigenvector is column vector) */
            emax = 0.0;
            for (j = 0;j < n; j++) {
                    emax = max(fabs(z[j][i]),emax);
            } /* endfor j */
            if (emax != 0.0) {
/* rescale eigenvector for max amplitude = unity */
                emax = 1.0/emax;
                for (j = 0;j < n;j++) {
                    z[j][i] *= emax;
                } /* endfor j */
            } /* endif emax */
        } /* endif wi[i] = 0.0 (real entry) */
        else {  /* eigenvector is complex */
/* find max modulus and note location of corresponding element */
            emax = 0.0;
            k = 0;
            for (j = 0;j < n; j++) {
                xr = z[j][i];
                yr = z[j][i+1];
                xr = sqrt(xr*xr + yr*yr);
                if (xr > emax) {
                    emax = xr;
                    k = j;
                } /* endif xr */
            } /* endfor j */
            emax = z[k][i]; /* get re and im of element with max modulus */
            ei = z[k][i+1];

            for (j = 0;j < n; j++) {
/* rotate and scale each entry */
                yr = emax;
                yi = ei;
                xr = z[j][i];
                xi = z[j][i+1];
                if (fabs(yr) > fabs(yi)) {
                    t = yi / yr;
                    yr += t * yi;
                    zr = (xr + t * xi) / yr;
                    zi = (xi - t * xr) / yr;
                } /* endif */
                else {
                    t = yr / yi;
                    yi += t * yr;
                    zr = (t * xr + xi) / yi;
                    zi = (t * xi - xr) / yi;
                } /* endelse */
                z[j][i] = zr;
                z[j][i+1] = zi;
            } /* endfor j */
            i++;  /* complex value uses two columns */
        } /* endelse wi[i] != 0.0 (complex entry) */
    } /* endfor i */
    //printmat(z,"Normalized eigenvectors",n);
} 
				
			
