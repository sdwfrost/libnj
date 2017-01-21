from libc.stdlib cimport malloc, free

cdef extern from "../src/nj.h":
  int C_nj (double *D, int *N, int *edge1, int *edge2, double *edge_length)
  int C_njs(double *D, int *N, int *edge1, int *edge2, double *edge_length, int *fsS);

def nj(list d, int n):
  cdef double *D =  <double*>malloc(len(d)*sizeof(double))
  if D is NULL:
    raise MemoryError()
  for i in xrange(len(d)):
    D[i]=d[i]
  cdef int N = n
  cdef int M = 2*N-3
  cdef int *edge1 = <int*>malloc(M*sizeof(int))
  cdef int *edge2 = <int*>malloc(M*sizeof(int))
  cdef double *edge_length = <double*>malloc(M*sizeof(double))
  cdef int err = C_nj(D, &N, edge1, edge2, edge_length)
  cdef list result = [[edge1[i],edge2[i],edge_length[i]] for i in xrange(M)]
  free(edge1)
  free(edge2)
  free(edge_length)
  return(result)

def njs(list d, int n, int fs):
  cdef double *D =  <double*>malloc(len(d)*sizeof(double))
  if D is NULL:
    raise MemoryError()
  for i in xrange(len(d)):
    D[i]=d[i]
  cdef int N = n
  cdef int M = 2*N-3
  cdef int *edge1 = <int*>malloc(M*sizeof(int))
  cdef int *edge2 = <int*>malloc(M*sizeof(int))
  cdef double *edge_length = <double*>malloc(M*sizeof(double))
  cdef int err = C_njs(D, &N, edge1, edge2, edge_length, &fs)
  cdef list result = [[edge1[i],edge2[i],edge_length[i]] for i in xrange(M)]
  free(edge1)
  free(edge2)
  free(edge_length)
  return(result)


