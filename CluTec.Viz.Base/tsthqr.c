/* tsthqr.c -- test path through elmhes, elmtrans, hqr
 * Solves for eigenvalues and eigenvectors of transition
 * matrix:
 *
 *            0 1 0 0
 *      M =   1 0 1 0
 *            1 0 0 1
 *            1 0 0 0
 */
#include <stdio.h>
#include <malloc.h>
#include "eispack.h"

void main()
{
    double **a,**v,**z,*wr,*wi;
    int *inter,i,n,ierr;

    n = 4;

    a = (double **)calloc(n,sizeof(double *));
    v = (double **)calloc(n,sizeof(double *));
    z = (double **)calloc(n,sizeof(double *));
    for (i=0;i<n;i++) {
        a[i] = (double *)calloc(n,sizeof(double));
        v[i] = (double *)calloc(n,sizeof(double));
        z[i] = (double *)calloc(2*n,sizeof(double));
    }
    wr = (double *)calloc(n,sizeof(double));
    wi = (double *)calloc(n,sizeof(double));
    inter = (int *)calloc(n,sizeof(int));

    a[0][0] = 4.0;
    a[1][1] = 2.0;
    a[2][2] = 3.0;
    a[0][3] = 1.0;
    a[1][3] = 2.0;
    a[3][0] = 1.0;
    v[0][0] = 0.0;
    v[1][1] = 0.0;
    v[2][2] = 0.0;
    v[3][3] = 0.0;

    //printf("Input matrix:\n");
    //for (i=0;i<n;i++) {
    //    printf("%lf, %lf, %lf, %lf\n",a[i][0],a[i][1],a[i][2],a[i][3]);
    //}

    elmhes(a,n,0,3,inter);

    //printf("\nResult of ELMHES:\n");
    //for (i=0;i<n;i++) {
    //    printf("%lf, %lf, %lf, %lf\n",a[i][0],a[i][1],a[i][2],a[i][3]);
    //}
/*
    eltran(n,0,3,a,inter,v);
*/
    hqr(n,0,3,a,wr,wi,&ierr);

    //printf("\nEigenvalues: re, im\n");
    //for (i=0;i<n;i++) {
    //    printf("%15.15lf, %15.15lf\n",wr[i],wi[i]);
    //}


    for (i=0;i<n;i++) {
        free(a[i]);
        free(v[i]);
    }
    free(a);
    free(v);
    free(inter);
    free(wr);
    free(wi);
}
