/*
$Id: solve.c 1.2 1994/09/12 19:11:27 leighton Exp $
    Copyright (C) 1991-2013 Russell Leighton <rleighto@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

program     -   solve.c
author      -   Russell Leighton
date        -   7 Mar 1992
revision    -   10 May 1993

input       -   iprint  - print flag (0 for no print, 1 for print)

description -   Routine to solve system of equations contained in the global 
                stiffness matrix and load vector.

*/

#include "header.h"

extern void error();

int tri(MATRIX A, int n, int bw)
{
    int i,j,k,l,n1,m1,m2,j1;
    REAL f;

    n1 = n-1;
    for (i=0; i<n1; i++) {
        if (ABS(A[i]) < 1.0e-10) {
            error("tri - reduction failed due to small pivot");
            return(0);
        }
        j1 = MIN(bw,n-i);
        for (j=1; j<j1; j++) {
            m1 = dpnts[j] + i;
            if (ABS(A[m1]) < 1.0e-10) continue;
            f = A[m1]/A[i];
            for (k=j; k<j1; k++) {
                l = k - j;
                m1 = dpnts[l] + j + i;
                m2 = dpnts[k] + i;
                A[m1] -= A[m2]*f;
            }
        }
    }
    return(1);
}

void rhsub(MATRIX A, VECTOR x, VECTOR b, int n, int bw)
{
    int i,j,n1,j1;

    n1 = n-1;
    for (i=0; i<n1; i++) {
        j1 = MIN(bw,n-i);
        for (j=1; j<j1; j++) b[i+j] -= b[i]*A[dpnts[j]+i]/A[i];
    }
    for (i=n1; i>=0; i--) {
        j1 = MIN(bw,n-i) - 1;
        for (j=j1; j>0; j--) b[i] -= A[dpnts[j]+i]*x[i+j];
        x[i] = b[i]/A[i];
    }
}

int solve(int iprint, float *du)
{
    int i,j,n,ne,nb;
    VECTOR un;

    if (kg == 0) {
        error("solve - no stiffness matrix");
        return(0);
    }

    ne = pndof*nnodes;
    nb = pndof*bw;

    if ((un = (VECTOR)calloc((size_t)ne,sizeof(REAL))) == NULL) {
        error("solve - could not allocate memory for solution vector");
        return(0);
    }

/* tridiagonalize stiffness matrix */

    if (!tri(kg,ne,nb)) return(0);

    rhsub(kg,un,fg,ne,nb);

    if (iprint) {
        for (i=0; i<ne; i++) fprintf(stdout,"%12.4e\n",(float)un[i]);
    }

    *du = 0.0;
    for (i=0; i<ne; i++) *du = MAX(*du,(float)ABS(un[i]));

    for (i=0; i<pndof; i++) {
        for (n=0; n<nsolns; n++) {
            if((solns[n].type == 0) && 
               (solns[n].kind == pkind) && 
               (solns[n].dof == i+1) && 
               (solns[n].step == 0)) break;
        }
        if (n == nsolns) {
            nsolns++;
            solns = (struct soln *)realloc(solns,nsolns*sizeof(struct soln));
            if (solns == NULL) { 
                error("solve - could not allocate memory for solns");
                break; 
            }
            if ((solns[n].u = (float *)calloc((size_t)nnodes,sizeof(float))) == NULL) {
                error("solve - could not allocate memory for soln");
                break; 
            }
            solns[n].type = 0;
            solns[n].kind = pkind;
            solns[n].dof = i+1;
            solns[n].step = 0;
        }
        for (j=0; j<nnodes; j++) solns[n].u[j] += un[pndof*j+i];
    }

    free(un);

    return(1);
}
