#include <stdlib.h>
#include "nj.h"

/* The C code (with minor changes is */
/* Copyright 2006-2011 Emmanuel Paradis */
/* https://github.com/cran/ape/blob/master/COPYING */
/* Thanks Emmanuel! */

int give_index(int i, int j, int n)
{
	if (i > j) return(DINDEX(j, i));
	else return(DINDEX(i, j));
}

double sum_dist_to_i(int n, double *D, int i)
/* returns the sum of all distances D_ij between i and j
   with j = 1...n and j != i */
{
/* we use the fact that the distances are arranged sequentially
   in the lower triangle, e.g. with n = 6 the 15 distances are
   stored as (the C indices are indicated):
           i
     1  2  3  4  5
  2  0
  3  1  5
j 4  2  6  9
  5  3  7 10 12
  6  4  8 11 13 14
  so that we sum the values of the ith column--1st loop--and those of
  (i - 1)th row (labelled 'i')--2nd loop */

	double sum = 0;
	int j, start, end;

	if (i < n) {
		/* the expression below CANNOT be factorized
		   because of the integer operations (it took
		   me a while to find out...) */
		start = n*(i - 1) - i*(i - 1)/2;
		end = start + n - i;
		for (j = start; j < end; j++) sum += D[j];
	}

	if (i > 1) {
		start = i - 2;
		for (j = 1; j <= i - 1; j++) {
			sum += D[start];
			start += n - j - 1;
		}
	}

	return(sum);
}

int C_nj(double *D, int *N, int *edge1, int *edge2, double *edge_length)
{
	double *S, Sdist, Ndist, *new_dist, A, B, smallest_S, x, y;
	int n, i, j, k, ij, smallest, OTU1, OTU2, cur_nod, o_l, *otu_label;

	S = &Sdist;
	new_dist = &Ndist;
	otu_label = &o_l;

	n = *N;
	cur_nod = 2*n - 2;

	S = (double*)malloc((n + 1)*sizeof(double));
	new_dist = (double*)malloc((n*(n - 1)/2)*sizeof(double));
	otu_label = (int*)malloc((n + 1)*sizeof(int));

  smallest = 0;
	OTU1 = 1;
	OTU2 = 2;

	for (i = 1; i <= n; i++) otu_label[i] = i; /* otu_label[0] is not used */

	k = 0;

	while (n > 3) {

		for (i = 1; i <= n; i++)
			S[i] = sum_dist_to_i(n, D, i); /* S[0] is not used */

		ij = 0;
		smallest_S = 1e50;
		B = n - 2;
		for (i = 1; i < n; i++) {
			for (j = i + 1; j <= n; j++) {
				A = B*D[ij] - S[i] - S[j];
				if (A < smallest_S) {
					OTU1 = i;
					OTU2 = j;
					smallest_S = A;
					smallest = ij;
				}
				ij++;
			}
		}

		edge2[k] = otu_label[OTU1];
		edge2[k + 1] = otu_label[OTU2];
		edge1[k] = edge1[k + 1] = cur_nod;

		/* get the distances between all OTUs but the 2 selected ones
		   and the latter:
		   a) get the sum for both
		   b) compute the distances for the new OTU */

		A = D[smallest];
		ij = 0;
		for (i = 1; i <= n; i++) {
			if (i == OTU1 || i == OTU2) continue;
			x = D[give_index(i, OTU1, n)]; /* dist between OTU1 and i */
 			y = D[give_index(i, OTU2, n)]; /* dist between OTU2 and i */
			new_dist[ij] = (x + y - A)/2;
			ij++;
		}
		/* compute the branch lengths */
		B = (S[OTU1] - S[OTU2])/B; /* don't need B anymore */
		edge_length[k] = (A + B)/2;
		edge_length[k + 1] = (A - B)/2;

		/* update before the next loop
		   (we are sure that OTU1 < OTU2) */
		if (OTU1 != 1)
			for (i = OTU1; i > 1; i--)
				otu_label[i] = otu_label[i - 1];
		if (OTU2 != n)
			for (i = OTU2; i < n; i++)
				otu_label[i] = otu_label[i + 1];
		otu_label[1] = cur_nod;

		for (i = 1; i < n; i++) {
			if (i == OTU1 || i == OTU2) continue;
			for (j = i + 1; j <= n; j++) {
				if (j == OTU1 || j == OTU2) continue;
				new_dist[ij] = D[DINDEX(i, j)];
				ij++;
			}
		}

		n--;
		for (i = 0; i < n*(n - 1)/2; i++) D[i] = new_dist[i];

		cur_nod--;
		k = k + 2;
	}

	for (i = 0; i < 3; i++) {
		edge1[*N*2 - 4 - i] = cur_nod;
		edge2[*N*2 - 4 - i] = otu_label[i + 1];
	}

	edge_length[*N*2 - 4] = (D[0] + D[1] - D[2])/2;
	edge_length[*N*2 - 5] = (D[0] + D[2] - D[1])/2;
	edge_length[*N*2 - 6] = (D[2] + D[1] - D[0])/2;

	free(S);
	free(new_dist);
	free(otu_label);
	return(0);
}

int H(double t)
{
	if (t >= 0 - 1e-10) return 1;
	return 0;
}

int choosePair(double* D,int n,double* R,int* s, int* sw, int* x, int* y, int fS)
{
    int i=0,j=0,k=0;
    int sww=0;
    double cFS[fS];
    int iFS[fS];
    int jFS[fS];
    for(k=0;k<fS;k++)
              {cFS[k]=-1e50;
               iFS[k]=0;
               jFS[k]=0;
              }
    double max=-1e50;
    for (i = 1; i < n; i++)
      {
       for (j = i + 1; j <= n; j++)
         {if(D[give_index(i,j,n)]==-1){sww=1;continue;}
	   if(s[give_index(i,j,n)]<=2)continue;
           //printf("R[%i,%i]=%f\n",i,j,R[give_index(i,j,n)]);
           //printf("s[%i,%i]=%i\n",i,j,s[give_index(i,j,n)]);
           //printf("D[%i,%i]=%f\n",i,j,D[give_index(i,j,n)]);
           int tr=0;
            double numb=((R[give_index(i,j,n)])/(s[give_index(i,j,n)]-2))-D[give_index(i,j,n)];
           //printf("numb(%i,%i)=%f\n",i,j,numb);
           for(k=0;k<fS && cFS[k]>numb;k++);
           //printf("k=%i ",k);
           for(tr=fS-1;tr>k;tr--)
             {cFS[tr]=cFS[tr-1];
              iFS[tr]=iFS[tr-1];
              jFS[tr]=jFS[tr-1];
             }

            if(k<fS){cFS[k]=numb;
                     iFS[k]=i;
                     jFS[k]=j;
                    }
          }
      }

    //no missing distances, just return the one with maximal Q-criterion
   if(sww==0){*x=iFS[0];*y=jFS[0];*sw=0;return(0);
             }
   /*for(k=0;k<fS;k++)
              {printf("value[%i]=%f ",k,cFS[k]);
               printf("i=%i ",iFS[k]);
               printf("j=%i ",jFS[k]);
               printf("\n");
              }*/
  //calculate N*(x,y)
  for(i=0;i<fS;i++)
   {
    if(iFS[i]==0 || jFS[i]==0)continue;
    double nb=nxy(iFS[i],jFS[i],n,D);
    if(nb>max){max=nb;}
    cFS[i]=nb;
   }
  /*printf("all N*(x,y)\n");
  for(k=0;k<fS;k++)
              {printf("value[%i]=%f ",k,cFS[k]);
               printf("i=%i ",iFS[k]);
               printf("j=%i ",jFS[k]);
               printf("\n");
              }*/
  int dk=0;
  //shift the max N*xy to the front of the array
  for(i=0;i<fS;i++)
   {
      if(cFS[i]==max)
        {
          cFS[dk]=cFS[i];
          iFS[dk]=iFS[i];
          jFS[dk]=jFS[i];
          dk++;
        }
   }
  //if just one pair realises max N*xy, return it
  if(dk==1){*x=iFS[0];*y=jFS[0];return(0);}
  fS=dk;
 /*printf("max n*xy values\n");
 for(k=0;k<fS;k++)
              {printf("value[%i]=%f ",k,cFS[k]);
               printf("i=%i ",iFS[k]);
               printf("j=%i ",jFS[k]);
               printf("\n");
              }*/
 max=-1e50;
 //on the front of the array containing max N*xy values compute cxy
 for(i=0;i<fS;i++)
  {
    if(iFS[i]==0 || jFS[i]==0)continue;
    double nb=cxy(iFS[i],jFS[i],n,D);
    if(nb>max){max=nb;}
    cFS[i]=nb;
  }
 /*printf("all c*xy values\n");
 for(k=0;k<fS;k++)
              {printf("value[%i]=%f ",k,cFS[k]);
               printf("i=%i ",iFS[k]);
               printf("j=%i ",jFS[k]);
               printf("\n");
              }*/
  //and again shift maximal C*xy values at the fron of the array
  dk=0;
  for(i=0;i<fS;i++)
   {
      if(cFS[i]==max)
        {
          cFS[dk]=cFS[i];
          iFS[dk]=iFS[i];
          jFS[dk]=jFS[i];
          dk++;
        }
   }
 //if just one C*xy with maximal value, return pair realising it
 if(dk==1){*x=iFS[0];*y=jFS[0];return(0);}
 fS=dk;
 /*printf("max c*xy values\n");
 for(k=0;k<fS;k++)
              {printf("value[%i]=%f ",k,cFS[k]);
               printf("i=%i ",iFS[k]);
               printf("j=%i ",jFS[k]);
               printf("\n");
              }*/
 max=-1e50;
 //on the front of the array containing max C*xy compute m*xy
 for(i=0;i<fS;i++)
  {
    if(iFS[i]==0 || jFS[i]==0)continue;
    double nb=mxy(iFS[i],jFS[i],n,D);
    if(nb>max){max=nb;}
    cFS[i]=nb;
  }
 /*printf("all m*xy values\n");
 for(k=0;k<fS;k++)
              {printf("value[%i]=%f ",k,cFS[k]);
               printf("i=%i ",iFS[k]);
               printf("j=%i ",jFS[k]);
               printf("\n");
              }*/
  //again shift maximal m*xy values to the fron of the array
  dk=0;
  for(i=0;i<fS;i++)
   {
      if(cFS[i]==max)
        {
          cFS[dk]=cFS[i];
          iFS[dk]=iFS[i];
          jFS[dk]=jFS[i];
          dk++;
        }
   }
 //if just one maximal value for m*xy return the pair realising it, found at 0
 if(dk==1){*x=iFS[0];*y=jFS[0];return(0);}
 fS=dk;
 /*printf("max m*xy values\n");
 for(k=0;k<fS;k++)
              {printf("value[%i]=%f ",k,cFS[k]);
               printf("i=%i ",iFS[k]);
               printf("j=%i ",jFS[k]);
               printf("\n");
              }*/
 //and calculate cnxy on these values, but this time we do not shift, but simply
 //return the pair realising the maximum, stored at iPos
 max=-1e50;
 int iPos=0;
 for(i=0;i<fS;i++)
  {
    if(iFS[i]==0 || jFS[i]==0)continue;
    double nb=cnxy(iFS[i],jFS[i],n,D);
    if(nb>max){max=nb;iPos=i;}
    cFS[i]=nb;
  }
 /*printf("cN*xy\n");
 printf("value[%i]=%f ",0,cFS[0]);
 printf("i=%i ",iFS[0]);
 printf("j=%i ",jFS[0]);
 printf("\n");*/
 if(iFS[iPos]==0 || jFS[iPos]==0)
   {
     return(1);
   }
 *x=iFS[iPos];*y=jFS[iPos];
 return(0);
}

double cnxy(int x, int y, int n,double* D)
{
    int i=0;
    int j=0;
    double nMeanXY=0;
    //printf("cN[%i,%i]\n",x,y);
    for(i=1;i<=n;i++)
     {
      for(j=1;j<=n;j++)
      {if(i==j)continue;
       if((i==x && j==y) || (j==x && i==y))continue;
       double n1=0;
       double n2=0;
       if(i!=x)n1=D[give_index(i,x,n)];
       if(j!=y)n2=D[give_index(j,y,n)];
       if(n1==-1 || n2==-1 || D[give_index(i,j,n)]==-1)continue;
       nMeanXY+=(n1+n2-D[give_index(x,y,n)]-D[give_index(i,j,n)]);
       //printf("cnMeanXY after (%i,%i)=%f\n",i,j,nMeanXY);
      }
     }
    return nMeanXY;
}

int mxy(int x,int y,int n,double* D)
{
    int i=0;
    int mx[n+1];
    int my[n+1];
    for(i=1;i<=n;i++)
      {
        mx[i]=0;my[i]=0;
      }
    for(i=1;i<=n;i++)
      {
        if(i!=x && D[give_index(x,i,n)]==-1)
          {
            mx[i]=1;
          }
        if(i!=y && D[give_index(y,i,n)]==-1)
          {
            my[i]=1;
          }
      }
    /*for(i=1;i<=n;i++)
      {
        printf("mx[%i]=%i ",i,mx[i]);
      }
    printf("\n");

    for(i=1;i<=n;i++)
      {
        printf("my[%i]=%i ",i,my[i]);
      }
    printf("\n");*/

    int xmy=0;
    int ymx=0;
    for(i=1;i<=n;i++)
      {
        if(i!=x && mx[i]==1 && my[i]==0)
          {
            xmy++;
          }
        if(i!=y && my[i]==1 && mx[i]==0)
          {
            ymx++;
          }
      }
    //printf("xmy=%i, ymx=%i, xmy+ymx=%i\n",xmy,ymx,xmy+ymx);
    return xmy+ymx;
}

double nxy(int x, int y, int n,double* D)
{
    int sCXY=0;
    int i=0;
    int j=0;
    double nMeanXY=0;
    //printf("N[%i,%i]\n",x,y);
    for(i=1;i<=n;i++)
     {
      for(j=1;j<=n;j++)
      {if(i==j)continue;
       if((i==x && j==y) || (j==x && i==y))continue;
       double n1=0;
       double n2=0;
       if(i!=x)n1=D[give_index(i,x,n)];
       if(j!=y)n2=D[give_index(j,y,n)];
       if(n1==-1 || n2==-1 || D[give_index(i,j,n)]==-1)continue;
       sCXY++;
       //printf("considered pair (%i,%i)\n",i,j);
       nMeanXY+=H(n1+n2-D[give_index(x,y,n)]-D[give_index(i,j,n)]);
       //printf("nMeanXY after (%i,%i)=%f\n",i,j,nMeanXY);
      }
     }
    //printf("sCXY=%i",sCXY);
    if(sCXY==0) return 0;
    return nMeanXY/sCXY;
}

int cxy(int x, int y, int n,double* D)
{
    int sCXY=0;
    int i=0;
    int j=0;

    for(i=1;i<=n;i++)
     {
      for(j=1;j<=n;j++)
      {if(i==j)continue;
       if((i==x && j==y) || (j==x && i==y))continue;
       double n1=0;
       double n2=0;
       if(i!=x)n1=D[give_index(i,x,n)];
       if(j!=y)n2=D[give_index(j,y,n)];
       if(n1==-1 || n2==-1 || D[give_index(i,j,n)]==-1)continue;
       sCXY++;
      }
     }
    return sCXY;
}

int C_njs(double *D, int *N, int *edge1, int *edge2, double *edge_length, int *fsS)
{       //assume missing values are denoted by -1
	double *S,*R, Sdist, Ndist, *new_dist, A, B, smallest_S;
	int n, i, j, k, ij, OTU1, OTU2, cur_nod, o_l, *otu_label;
        /*for(i=0;i<n*(n-1)/2;i++)
          {if(isNA(D[i])){D[i]=-1;}
          }*/
        int *s;//s contains |Sxy|, which is all we need for agglomeration
        double *newR;
        int *newS;
        int fS=*fsS;

	R = &Sdist;
	new_dist = &Ndist;
	otu_label = &o_l;
        n = *N;
	cur_nod = 2*n - 2;

	R = (double*)malloc((n*(n - 1)/2)*sizeof(double));
        S = (double*)malloc((n + 1)*sizeof(double));
        newR = (double*)malloc((n*(n - 1)/2)*sizeof(double));
	new_dist = (double*)malloc((n*(n - 1)/2)*sizeof(double));
	otu_label = (int*)malloc((n + 1)*sizeof(int));
        s = (int*)malloc((n*(n - 1)/2)*sizeof(int));
        newS = (int*)malloc((n*(n - 1)/2)*sizeof(int));

	for (i = 1; i <= n; i++) otu_label[i] = i; /* otu_label[0] is not used */

	k = 0;

        //compute Sxy and Rxy
        for(i=0;i<n*(n-1)/2;i++)
          {newR[i]=0;
           newS[i]=0;
           s[i]=0;
           R[i]=0;
          }

        for(i=1;i<n;i++)
         for(j=i+1;j<=n;j++)
         {//algorithm assumes i,j /in Sij, so skip pair if it is not known
          if(D[give_index(i,j,n)]==-1)
            {
              continue;
            }
          //printf("for %i and %i :\n",i,j);
          for(k=1;k<=n;k++)
           {//ij is the pair for which we compute
            //skip k if we do not know the distances between it and i AND j
              if(k==i || k==j)
               {
                  s[give_index(i,j,n)]++;
		  if(i!=k)R[give_index(i,j,n)]+=D[give_index(i,k,n)];
		  if(j!=k)R[give_index(i,j,n)]+=D[give_index(j,k,n)];
                  continue;
               }
              if(D[give_index(i,k,n)]==-1 || D[give_index(j,k,n)]==-1)continue;
              //printf("%i\n",k);
              s[give_index(i,j,n)]++;
              R[give_index(i,j,n)]+=D[give_index(i,k,n)];
              R[give_index(i,j,n)]+=D[give_index(j,k,n)];
           }
         }

        /*for(i=1;i<n;i++)
                  {
                    for(j=i+1;j<=n;j++)
                      {
                        printf("R[%i,%i]=%f ",i,j,R[give_index(i,j,n)]);
                      }
                    printf("\n");
                  }

                for(i=1;i<n;i++)
                  {
                    for(j=i+1;j<=n;j++)
                      {
                        printf("s[%i,%i]=%i ",i,j,s[give_index(i,j,n)]);
                      }
                    printf("\n");
                  }*/

        k=0;
        int sw=1;//if 1 then incomplete
	while (n > 3) {
		ij = 0;
                for(i=1;i<n;i++)
                 for(j=i+1;j<=n;j++)
                  {newR[give_index(i,j,n)]=0;
                   newS[give_index(i,j,n)]=0;
                  }
		smallest_S = -1e50;
                if(sw==0)
                    for(i=1;i<=n;i++)
                       {S[i]=0;
                       }
		B=n-2;
                if(sw==1)
                     {
                      int err = choosePair(D,n,R,s,&sw,&OTU1,&OTU2,fS);
											if(err==1){
												return(1);
											}
                     }
                 else{ //printf("distance matrix is now complete\n");
                        for (i=1;i<=n;i++)
                         for(j=1;j<=n;j++)
                           {if(i==j)continue;
                             //printf("give_index(%i,%i)=%i\n",i,j,give_index(i,j,n));
                             //printf("D[%i,%i]=%f\n",i,j,D[give_index(i,j,n)]);
                             S[i]+=D[give_index(i,j,n)];
                           }
                        B=n-2;
                        //printf("n=%i,B=%f",n,B);
		        for (i = 1; i < n; i++) {
			 for (j = i + 1; j <= n; j++) {
                             //printf("S[%i]=%f, S[%i]=%f, D[%i,%i]=%f, B=%f",i,S[i],j,S[j],i,j,D[give_index(i,j,n)],B);
                                A=S[i]+S[j]-B*D[give_index(i,j,n)];
                                //printf("Q[%i,%i]=%f\n",i,j,A);
				if (A > smallest_S) {
					OTU1 = i;
					OTU2 = j;
					smallest_S = A;
					/* smallest = ij; */
				}
				ij++;
			}
		        }
                     }

                /*printf("agglomerating %i and %i, Q=%f \n",OTU1,OTU2,smallest_S);

                for(i=1;i<n;i++)
                  {
                    for(j=i+1;j<=n;j++)
                      {
                        printf("R[%i,%i]=%f ",i,j,R[give_index(i,j,n)]);
                      }
                    printf("\n");
                  }

                for(i=1;i<n;i++)
                  {
                    for(j=i+1;j<=n;j++)
                      {
                        printf("s[%i,%i]=%i ",i,j,s[give_index(i,j,n)]);
                      }
                    printf("\n");
                  }

                for(i=1;i<n;i++)
                  {
                    for(j=i+1;j<=n;j++)
                      {
                        printf("d[%i,%i]=%f ",i,j,D[give_index(i,j,n)]);
                      }
                    printf("\n");
                  }*/
                //update Rxy and Sxy, only if matrix still incomplete
                if(sw==1)
                for(i=1;i<n;i++)
                {if(i==OTU1 || i==OTU2)continue;
                 for(j=i+1;j<=n;j++)
                  {if(j==OTU1 || j==OTU2)continue;
                    if(D[give_index(i,j,n)]==-1)continue;
                     if(D[give_index(i,OTU1,n)]!=-1 && D[give_index(j,OTU1,n)]!=-1)
                      {//OTU1 was considered for Rij, so now subtract
                       R[give_index(i,j,n)]-=(D[give_index(i,OTU1,n)]+D[give_index(j,OTU1,n)]);
                       s[give_index(i,j,n)]--;
                      }
                     if(D[give_index(i,OTU2,n)]!=-1 && D[give_index(j,OTU2,n)]!=-1)
                      {//OTU2 was considered for Rij, so now subtract
                       R[give_index(i,j,n)]-=(D[give_index(i,OTU2,n)]+D[give_index(j,OTU2,n)]);
                       s[give_index(i,j,n)]--;
                      }
                  }
                }

		edge2[k] = otu_label[OTU1];
		edge2[k + 1] = otu_label[OTU2];
		edge1[k] = edge1[k + 1] = cur_nod;

		/* get the distances between all OTUs but the 2 selected ones
		   and the latter:
		   a) get the sum for both
		   b) compute the distances for the new OTU */

                double sum=0;

                for(i=1;i<=n;i++)
                 {if(i==OTU1 || i==OTU2)continue;
                 if(D[give_index(OTU1,i,n)]==-1 || D[give_index(OTU2,i,n)]==-1)continue;
                 sum+=(D[give_index(OTU1,i,n)]-D[give_index(OTU2,i,n)]);
                 }
                //although s was updated above, s[otu1,otu2] has remained unchanged
                //so it is safe to use it here
                //if complete distanes, use N-2, else use S
                int down=B;
                if(sw==1){down=s[give_index(OTU1,OTU2,n)]-2;}
                if(down<=0)
                  {return(1);
                  }
                //printf("down=%i\n",down);
                sum*=(1.0/(2*(down)));
                //printf("sum=%f\n",sum);
                double dxy=D[give_index(OTU1,OTU2,n)]/2;

                //printf("R[%i,%i]:%f \n",OTU1,OTU2,sum);
		edge_length[k] = dxy+sum;//OTU1
                //printf("l1:%f \n",edge_length[k]);
		edge_length[k + 1] = dxy-sum;//OTU2
                //printf("l2:%f \n",edge_length[k+1]);
               //no need to change distance matrix update for complete distance
               //case, as pairs will automatically fall in the right cathegory
		A = D[give_index(OTU1,OTU2,n)];
		ij = 0;
		for (i = 1; i <= n; i++) {
			if (i == OTU1 || i == OTU2) continue;
                        if(D[give_index(OTU1,i,n)]!=-1 && D[give_index(OTU2,i,n)]!=-1)
                         {
                            new_dist[ij]=0.5*(D[give_index(OTU1,i,n)]-edge_length[k]+D[give_index(OTU2,i,n)]-edge_length[k+1]);
                         }else{
                         if(D[give_index(OTU1,i,n)]!=-1)
                                {
                                 new_dist[ij]=D[give_index(OTU1,i,n)]-edge_length[k];
                                }else{
                                      if(D[give_index(OTU2,i,n)]!=-1)
                                        {
                                            new_dist[ij]=D[give_index(OTU2,i,n)]-edge_length[k+1];
                                        }else{new_dist[ij]=-1;}
                                     }
                              }

			ij++;
		}

                for (i = 1; i < n; i++) {
			if (i == OTU1 || i == OTU2) continue;
			for (j = i + 1; j <= n; j++) {
				if (j == OTU1 || j == OTU2) continue;
				new_dist[ij] = D[DINDEX(i, j)];
				ij++;
			}
		}

                /*for(i=1;i<n-1;i++)
                {
                  for(j=i+1;j<=n-1;j++)
                   {printf("%f ",new_dist[give_index(i,j,n-1)]);
                   }
                  printf("\n");
                }*/
                //compute Rui, only if distance matrix is still incomplete
                ij=0;
                if(sw==1)
                for(i=2;i<n;i++)
                  {
                   ij++;
                   if(new_dist[give_index(i,1,n-1)]==-1)continue;

                   for(j=1;j<n;j++)
                     { if(j==1 || j==i)
                       {
			 if(i!=j)newR[give_index(1,i,n-1)]+=new_dist[give_index(i,j,n-1)];
			 if(j!=1)newR[give_index(1,i,n-1)]+=new_dist[give_index(1,j,n-1)];
                         newS[give_index(1,i,n-1)]++;
                         continue;
                       }
                       if(new_dist[give_index(i,j,n-1)]!=-1 && new_dist[give_index(1,j,n-1)]!=-1)
                        {
                          newS[give_index(1,i,n-1)]++;
                          newR[give_index(1,i,n-1)]+=new_dist[give_index(i,j,n-1)];
                          newR[give_index(1,i,n-1)]+=new_dist[give_index(1,j,n-1)];
                        }
                     }
                  }
                //fill in the rest of R and S, again only if distance matrix still
                //incomplete
		if(sw==1) /* added 2012-04-02 */
                for(i=1;i<n;i++)
                {if(i==OTU1 || i==OTU2)continue;
                 for(j=i+1;j<=n;j++)
                  {if(j==OTU1 || j==OTU2)continue;
                   newR[ij]=R[give_index(i,j,n)];
                   newS[ij]=s[give_index(i,j,n)];
                   ij++;
                  }
                }
                //update newR and newS with the new taxa, again only if distance
                //matrix is still incomplete
                if(sw==1)
                for(i=2;i<n-1;i++)
                {if(new_dist[give_index(1,i,n-1)]==-1)continue;
                 for(j=i+1;j<=n-1;j++)
                  {if(new_dist[give_index(1,j,n-1)]==-1)continue;
                    if(new_dist[give_index(i,j,n-1)]==-1)continue;
                   newR[give_index(i,j,n-1)]+=(new_dist[give_index(1,i,n-1)]+new_dist[give_index(1,j,n-1)]);
                   newS[give_index(i,j,n-1)]++;
                  }
                }

		/* update before the next loop
		   (we are sure that OTU1 < OTU2) */
		if (OTU1 != 1)
			for (i = OTU1; i > 1; i--)
				otu_label[i] = otu_label[i - 1];
		if (OTU2 != n)
			for (i = OTU2; i < n; i++)
				otu_label[i] = otu_label[i + 1];
		otu_label[1] = cur_nod;

		n--;
		for (i = 0; i < n*(n - 1)/2; i++)
                  {
                    D[i] = new_dist[i];
                    if(sw==1)
                       {
                        R[i] = newR[i];
                        s[i] = newS[i];
                       }
                  }
		cur_nod--;
		k = k + 2;
	}
        int dK=0;//number of known distances in final distance matrix
        int iUK=-1;//index of unkown distance, if we have one missing distance
        int iK=-1;//index of only known distance, only needed if dK==1
        for (i = 0; i < 3; i++) {
		edge1[*N*2 - 4 - i] = cur_nod;
		edge2[*N*2 - 4 - i] = otu_label[i + 1];
                if(D[i]!=-1){dK++;iK=i;}else{iUK=i;}
	}
        if(dK==2)
         {//if two distances are known: assume our leaves are x,y,z, d(x,z) unknown
          //and edge weights of three edges are a,b,c, then any b,c>0 that
          //satisfy c-b=d(y,z)-d(x,y) a+c=d(y,z) are good edge weights, but for
          //simplicity we assume a=c if d(yz)<d(xy) a=b otherwise, and after some
          //algebra we get that we can set the missing distance equal to the
          //maximum of the already present distances
            double max=-1e50;
          for(i=0;i<3;i++)
            {if(i==iUK)continue;
             if(D[i]>max)max=D[i];
            }
          D[iUK]=max;
         }
        if(dK==1)
         {//through similar motivation as above, if we have just one known distance
          //we set the other two distances equal to it
          for(i=0;i<3;i++)
            {if(i==iK)continue;
             D[i]=D[iK];
            }
         }
        if(dK==0)
         {//no distances are known, we just set them to 1
          for(i=0;i<3;i++)
           {D[i]=1;
           }
         }
        edge_length[*N*2 - 4] = (D[0] + D[1] - D[2])/2;
	edge_length[*N*2 - 5] = (D[0] + D[2] - D[1])/2;
	edge_length[*N*2 - 6] = (D[2] + D[1] - D[0])/2;
	return(0);
}
