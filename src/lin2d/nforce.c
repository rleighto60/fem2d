/*
$Id: nforce.c 1.2 1994/09/12 19:11:27 leighton Exp $
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

program     -   nforce.c
author      -   Russell Leighton
date        -   30 Nov 1994

input       -   iprint  - print flag (0 for no print, 1 for print)

description -   Routine to calculate nodal reaction forces.

*/

#include "header.h"

extern void elem();
extern void error();
extern int checkelem();

int nforce(int iprint)
{
    int i,j,k,ii,jj,ig,n,ns,nn,ne;
    REAL u[2][9],ke[18][18],fe[18];
    VECTOR f;

    ne = pndof*nnodes;

    if((f = (VECTOR)calloc((size_t)ne,sizeof(REAL))) == NULL) {
        error("formkf - could not allocate memory for load vector");
        return(0);
    }

    for(n=0;n<nelems;n++) {
        if(elems[n].id == 0) continue;
        if(!checkelem("nforce",n)) return(0);
        elem(n,ke,fe);
        ns = elems[n].type;
        nn = shapes[ns]->nn;

/* retrieve solution values */

        for (i=0; i<nn; i++) {
            u[0][i] = u[1][i] = 0.0;
            for (j=0; j<nsolns; j++) {
                if ((solns[j].kind == pkind) && (solns[j].type == 0)) {
                    k = solns[j].dof - 1;
                    u[k][i] = solns[j].u[nnums[elems[n].nodes[i]]];
                }
            }
        }

/* accumulate nodal reaction force values */

        for(i=0;i<nn;i++) {
            for(ii=0;ii<pndof;ii++) {
                ig = pndof*nnums[elems[n].nodes[i]] + ii;
                for(j=0;j<nn;j++) {
                    for(jj=0;jj<pndof;jj++) 
                        f[ig] += ke[pndof*i+ii][pndof*j+jj]*u[jj][j];
                }
            }
        }
    }

    if(iprint) {
        fprintf(stdout,"\n  n       f\n\n");
        for(i=0;i<nnodes;i++) {
            fprintf(stdout,"%4d",i);
            for(j=0;j<pndof;j++) fprintf(stdout," %12.4e",(float)f[pndof*i+j]);
            fprintf(stdout,"\n");
        }
    }

    for(i=0;i<pndof;i++) {
        for(n=0;n<nsolns;n++) {
            if((solns[n].type == 1) && 
               (solns[n].kind == pkind) && 
               (solns[n].dof == i+1) && 
               (solns[n].step == 0)) break;
        }
        if(n == nsolns) {
            nsolns++;
            solns = (struct soln *)realloc(solns,nsolns*sizeof(struct soln));
            if(solns == NULL) { 
                error("solve - could not allocate memory for solns");
                break; 
            }
            if((solns[n].u = (float *)malloc((size_t)nnodes*sizeof(float))) == NULL) {
                error("solve - could not allocate memory for soln");
                break; 
            }
        }
        for(j=0;j<nnodes;j++) solns[n].u[j] = f[pndof*j+i];
        solns[n].type = 1;
        solns[n].kind = pkind;
        solns[n].dof = i+1;
        solns[n].step = 0;
    }

    free(f);
    return(1);
}
