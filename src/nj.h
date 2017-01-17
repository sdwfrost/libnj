#ifndef 	__NJSHARED__
#define 	__NJSHARED__

#define DINDEX(i, j) n*(i - 1) - i*(i - 1)/2 + j - i - 1

extern void C_nj(double *D, int *N, int *edge1, int *edge2, double *edge_length);

#endif
