
/* balance matrix */
void balanc(double **a,int n,int *low, int *high,double scale[]);

/* forms eigenvectors of matrix by back transformation  */
void balbak(int n,int low, int hi,double scale[],int m,double **z);

/* convert matrix to upper Hessenberg form */
void elmhes(double **a,int n,int lo, int hi,int intch[]);

/* accumulates stabilized similarity transformations */
void eltran(int n,int lo,int hi,double **a,int intch[],double **z);

/* forms eigenvectors by back transformation from elmhes */
void elmbak(int lo,int hi,double **a,int intch[],int m,double **z);

/* solve upper Hessenberg matrix for eigenvalues */
void hqr(int n,int lo,int hi,double **h,double *wr,double *wi,int *ierr);

/* solve upper Hessenberg matrix for eigenvalues and eigenvectors */
void hqr2(int n,int lo,int hi,double **h,double *wr,
	double *wi,double **z,int *ierr);

void pnormwr(int n,double **z,double *wr,double *wi);

