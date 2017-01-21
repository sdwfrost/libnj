#ifndef 	__NJSHARED__
#define 	__NJSHARED__

#define DINDEX(i, j) n*(i - 1) - i*(i - 1)/2 + j - i - 1

int choosePair(double* D, int n, double* R, int* s, int* sw, int* x, int* y, int fS);
double cnxy(int x, int y, int n, double* D);
int mxy(int x,int y, int n, double* D);
double nxy(int x, int y, int n, double* D);
int cxy(int x, int y, int n, double* D);

extern int C_nj(double *D, int *N, int *edge1, int *edge2, double *edge_length);
extern int C_njs(double *D, int *N, int *edge1, int *edge2, double *edge_length, int *fsS);

#endif
