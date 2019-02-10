/*
$Id: formfk.c 1.2 1994/09/12 19:11:27 leighton Exp $
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

program     -   formkf.c
author      -   Russell Leighton
date        -   21 Apr 1992

input       -   iprint  - print flag (0 for no print, 1 for print)

description -   Routine to assemble global stiffness matrix and load vector.
                Also applies boundary conditions.

*/

#include "header.h"

extern void elem();
extern void bcint();
extern void error();
extern int checkelem();
extern int checkpbc();
extern int checkdbc();

void dirchlet(int n)
{
    int i,j,k,m,ne,nb;
    float val;

    val = pbcs[n].val;

/* retrieve previous solution value */

    for (j=0; j<nsolns; j++) {
        if ((solns[j].kind == pkind) && (solns[j].type == 0) && (solns[j].dof == pbcs[n].dof)) {
            val -= solns[j].u[nnums[pbcs[n].id]];
            break;
        }
    }

    m = pndof*nnums[pbcs[n].id] + pbcs[n].dof - 1;

    ne = pndof*nnodes;
    nb = pndof*bw;

    k = MIN(nb,m+1);
    for (i=1; i<k; i++) {
        j = dpnts[i] + m - i;
        fg[m-i] -= kg[j]*(REAL)val;
        kg[j] = 0.0;
    }
    k = MIN(nb,ne-m);
    for (i=1; i<k; i++) {
        j = dpnts[i] + m;
        fg[m+i] -= kg[j]*(REAL)val;
        kg[j] = 0.0;
    }
    kg[m] = 1.0;
    fg[m] = (REAL)val;
}

void constraint(int n)
{
    REAL pen;
    float val;
    int i,j,k;

    i = pndof*nnums[pbcs[n].id] + pbcs[n].dof - 1;
    j = pndof*nnums[pbcs[n].jd] + pbcs[n].dof - 1;
    val = pbcs[n].val;

    pen = (REAL)kg[i]*1.e7;

    if(j < i) k = dpnts[i-j] + j;
    else k = dpnts[j-i] + i;

    kg[i] += pen;
    kg[j] += pen;
    kg[k] -= pen;
    fg[i] += pen*val;
    fg[j] += pen*val;
}

void flux(int n)
{
    int m;

    m = pndof*nnums[pbcs[n].id] + pbcs[n].dof - 1;

    switch (ptype) {
        case 0: fg[m] += (REAL)pbcs[n].val; break;
        case 1: fg[m] += nodes[nnums[pbcs[n].id]].x*(REAL)pbcs[n].val; break;
    }
}

int formkf(int iprint, float *dr)
{
    int i,j,k,ii,jj,ig,jg,n,m,ns,nn,ne,nb;
    REAL u[2][9],ke[18][18],fe[18];
    size_t size;

    if (kg) { free(kg); kg = 0; }
    if (fg) { free(fg); fg = 0; }
    if (dpnts) { free(dpnts); dpnts = 0; }

    if (pndof != 2) {
        error("formkf - number of degrees of freedom for problem must be 2");
        return(0);
    }

    ne = pndof*nnodes;
    nb = pndof*bw;
    size = (size_t)(ne*nb-((nb*(nb-1)) >> 1));
    if ((kg = (MATRIX)calloc(size,sizeof(REAL))) == NULL) {
        error("formkf - could not allocate memory for stiffness matrix");
        return(0);
    }
    if ((fg = (VECTOR)calloc((size_t)ne,sizeof(REAL))) == NULL) {
        error("formkf - could not allocate memory for load vector");
        return(0);
    }
    if ((dpnts = (long *)malloc((size_t)nb*sizeof(long))) == NULL) {
        error("formkf - could not allocate memory for diagonal pointers");
        return(0);
    }
    mallcd = (size + ne)*sizeof(REAL) + nb*sizeof(int);

    for (i=0; i<nb; i++) dpnts[i] = (long)(ne*i - ((i*(i-1)) >> 1));

    for (i=0; i<nnodes; i++) {
        if (nodes[i].id <= 0) {
            for (ii=0; ii<pndof; ii++) {
                ig = pndof*i + ii;
                kg[ig] = 1.0;
            }
        }
    }

    for (n=0; n<nelems; n++) {
        if (elems[n].id == 0) continue;
        if (!checkelem("formkf",n)) return(0);
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

/* compute element stiffness and load, assemble into global */

        elem(n,u,ke,fe);
        bcint(elems[n].bc,u,ke,fe);
        for (i=0; i<nn; i++) {
            for (ii=0; ii<pndof; ii++) {
                ig = pndof*nnums[elems[n].nodes[i]] + ii;
                fg[ig] += fe[pndof*i+ii];
                for (j=0; j<nn; j++) {
                    for (jj=0; jj<pndof; jj++) {
                        jg = pndof*nnums[elems[n].nodes[j]] + jj;
                        if (jg < ig) continue;
                        m = dpnts[jg-ig] + ig;
                        kg[m] += ke[pndof*i+ii][pndof*j+jj];
                    }
                }
            }
        }
    }

    if (iprint) {
        for (i=0; i<ne; i++) {
            k = MIN(nb,i);
            for (j=0; j<i-k; j++) fprintf(stdout,"%12.4e ",0.0);
            for (j=i-k; j<i; j++) {
                m = dpnts[i-j] + j;
                fprintf(stdout,"%12.4e ",(float)kg[m]);
            }
            k = MIN(nb,ne-i);
            for (j=0; j<k; j++) {
                m = dpnts[j] + i;
                fprintf(stdout,"%12.4e ",(float)kg[m]);
            }
            for (j=0; j<(ne-i-k); j++) fprintf(stdout,"%12.4e ",0.0);
            fprintf(stdout,"\n");
        }
    }

/* apply point boundary conditions */

    for (n=0; n<npbcs; n++) {
        if(pbcs[n].type == 0) continue;
        if (pbcs[n].kind != pkind) continue;
        if (!checkpbc("formkf",n)) return(0);
        switch (pbcs[n].type) {
            case 1: dirchlet(n);
                    break;
            case 2: flux(n);
                    break;
            case 3: constraint(n);
                    break;
        }
    }

    if(iprint == 2) {
        for(i=0;i<ne;i++) {
            k = MIN(nb-1,i);
            for(j=0;j<i-k;j++) fprintf(stdout,"%12.4e ",0.0);
            for(j=i-k;j<i;j++) {
                m = dpnts[i-j] + j;
                fprintf(stdout,"%12.4e ",(float)kg[m]);
            }
            k = MIN(nb,ne-i);
            for(j=0;j<k;j++) {
                m = dpnts[j] + i;
                fprintf(stdout,"%12.4e ",(float)kg[m]);
            }
            for(j=0;j<(ne-i-k);j++) fprintf(stdout,"%12.4e ",0.0);
            fprintf(stdout,"      %12.4e\n",(float)fg[i]);
        }
    }

    *dr = 0.0;
    for (i=0; i<ne; i++) *dr = MAX(*dr,(float)ABS(fg[i]));

    return(1);
}
