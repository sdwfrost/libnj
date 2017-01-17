#include <stdio.h>
#include <stdlib.h>

#include "nj.h"

int main(void) {
  double D[] = {2.95898433658294, 2.32786669256166, 2.59011289686896, 1.76088649779558, 1.68078850745223, 1.21574932453223, 2.04346269601956, 2.30418596812524, 2.22408797778189, 1.41234505199827, 1.67306832410395, 1.5929703337606,1.93531452841125, 1.85521653806791, 0.453543569659814};
  int N = 6;
  int M = 2*N-3;
  int *edge1 = (int*)malloc(M*sizeof(int));
  int *edge2 = (int*)malloc(M*sizeof(int));
  double *edge_length = (double*)malloc(M*sizeof(double));
  C_nj(D, &N, edge1, edge2, edge_length);
  for(int i = 0; i < M; i++) {
    printf("%d\t%d\t%f\n", edge1[i],edge2[i],edge_length[i]);
  }
  free(edge1);
  free(edge2);
  free(edge_length);
  return(0);
}
