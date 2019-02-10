/*
$Id: elem.c 1.2 1994/09/12 19:11:27 leighton Exp $
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

program     -   elem.c
author      -   Russell Leighton
date        -   20 Apr 1992
revision    -   24 Apr 1993

input       -   n  - element number
                u  - element nodal point solutions

output      -   ke - stiffness matrix
                fe - load vector

description -   Routine to compute element stiffness matrix and load vector.

*/

#include "header.h"

extern void shape(float xi, float yi, int id, REAL *psi, REAL *dpdx, REAL *dpdy);
extern int grad(int n, REAL *dpde, REAL *dpdn, REAL J[2][2], REAL *detJ);
extern void dshape();
extern void force();
extern void rotmat();
extern void laminate();

void elem(int n, REAL u[2][9], REAL ke[18][18], REAL fe[18])
{
    float xi,yi;
    REAL x;
    REAL J[2][2],detJ,ei,ej,ek,fac;
    REAL *psi,*dpde,*dpdn,dpdx[9],dpdy[9];
    REAL A[4][5],M[5][5],G[5][2][9],B[4][2][9],E[4][4],s[4],e[4],f[4];
    REAL dudx[2],dudy[2],ur;
    int i,j,k,l,mn,ln,ip,ii,jj,ns,nn,ni,ne;

    ns = elems[n].type;
    nn = shapes[ns]->nn;
    ni = shapes[ns]->cintg->ni;
    ne = pndof*nn;

/* find the appropriate material */

    mn = ln = elems[n].mat;
    if((mn > 0) && (mn < mmax)) {
        mn = mnums[mn];
        while (mn >= 0) {
            if (mats[mn].kind == pkind) break;
            else mn = mats[mn].next;
        }
    }
    else mn = -1;
    if (mn < 0) {
        sprintf(lasterr,"elem - error in element %d, no material defined",ABS(elems[n].id));
        severity = 10;
        return;
    }

/* and laminate if one exists */

    if((ln > 0) && (ln < lmax)) {
        ln = lnums[ln];
        if (lams[ln].id == 0) ln = -1;
    }
    else ln = -1;

/* initialize element stiffness matrix and load vector */

    for (i=0; i<ne; i++) {
        fe[i] = 0.0;
        for (j=0; j<ne; j++) ke[i][j] = 0.0;
    }

/* retrieve material data */

    if(mats[mn].type != 1) {
        k = 0;
        for (j=0; j<4; j++) {
            for (i=0; i<(4-j); i++) E[i][i+j] = E[i+j][i] = (REAL)mats[mn].C[k++];
        }
    }

/* calculate shape function data at integration points (if needed) */

    if (!shapes[ns]->flag) {
        for (ip=0; ip<ni; ip++) {
            xi = shapes[ns]->cintg->xip[ip];
            yi = shapes[ns]->cintg->yip[ip];
            psi = shapes[ns]->psi + ip*nn;
            dpde = shapes[ns]->dpde + ip*nn;
            dpdn = shapes[ns]->dpdn + ip*nn;
            shape(xi,yi,ns,psi,dpde,dpdn);
        }
        shapes[ns]->flag = 1;
    }

/* loop on integration points */

    for (ip=0; ip<ni; ip++) {
        psi = shapes[ns]->psi + ip*nn;
        dpde = shapes[ns]->dpde + ip*nn;
        dpdn = shapes[ns]->dpdn + ip*nn;

/* calculate gradient */

        if(!grad(n,dpde,dpdn,J,&detJ)) return;

/* compute material properties for laminates */

        if(ln >= 0) {
            laminate(ln,n,mn,psi);
            if(mats[mn].type != 1) {
                k = 0;
                for (j=0; j<4; j++) {
                    for (i=0; i<(4-j); i++) E[i][i+j] = E[i+j][i] = (REAL)mats[mn].C[k++];
                }
            }
        }

/* transform material properties to global reference for type 1 materials */

        if (mats[mn].type == 1) rotmat(n,mn,J,E);

/* transform shape function derivatives */

        dshape(n,J,detJ,dpde,dpdn,dpdx,dpdy);

/* calculate body force */

        force(n,mn,psi,f);

/* calculate integration factor */

        fac = detJ*shapes[ns]->cintg->w[ip];

/* calculate solution derivatives */

        dudx[0] = dudx[1] = dudy[0] = dudy[1] = ur = 0.0;
        for (i=0; i<nn; i++) {
            for (ii=0; ii<pndof; ii++) {
                dudx[ii] += dpdx[i]*u[ii][i];
                dudy[ii] += dpdy[i]*u[ii][i];
            }
            ur += psi[i]*u[0][i];
        }

/* for axisymmetry... */

        x = 0.0;
        if (ptype) { 
            for (i=0; i<nn; i++) x += psi[i]*nodes[nnums[elems[n].nodes[i]]].x;
            fac *= x;
            f[2] *= x;
        }

/* assemble A matrix */

        for (i=0; i<4; i++) {
            for (j=0; j<5; j++) A[i][j] = 0.0;
        }
        A[0][0] = A[3][2] = dudx[0];
        A[0][1] = A[3][3] = dudx[1];
        A[1][2] = A[3][0] = dudy[0];
        A[1][3] = A[3][1] = dudy[1];
        if (ptype) A[2][4] = ur/x;

/* assemble G matrix */

        for (i=0; i<nn; i++) {
            G[0][0][i] = G[1][1][i] = dpdx[i];
            G[2][0][i] = G[3][1][i] = dpdy[i];
            if (ptype) G[4][0][i] = psi[i]/x;
            else G[4][0][i] = 0.0;
            G[0][1][i] = G[1][0][i] = G[2][1][i] = G[3][0][i] = G[4][1][i] = 0.0;
        }

/* assemble B matrix */

        for (i=0; i<nn; i++) {
            B[0][0][i] = B[3][1][i] = dpdx[i];
            B[1][1][i] = B[3][0][i] = dpdy[i];
            if (ptype) B[2][0][i] = psi[i]/x;
            else B[2][0][i] = 0.0;
            B[0][1][i] = B[1][0][i] = B[2][1][i] = 0.0;
            for (ii=0; ii<pndof; ii++) {
                for (k=0; k<4; k++) {
                    ek = 0.0;
                    for (l=0; l<5; l++) ek += A[k][l]*G[l][ii][i];
                    B[k][ii][i] += ek;
                }
            }
        }

/* compute strain and stress */

        e[0] = dudx[0] + (dudx[0]*dudx[0] + dudx[1]*dudx[1])/2.0;
        e[1] = dudy[1] + (dudy[0]*dudy[0] + dudy[1]*dudy[1])/2.0;
        if (ptype) e[2] = (1 + ur/x/2.0)*ur/x;
        else e[2] = 0.0;
        e[3] = dudy[0] + dudx[1] + dudx[0]*dudy[0] + dudx[1]*dudy[1];
        for (k=0; k<4; k++) s[k] = 0.0;
        for (k=0; k<4; k++) {
            for (l=0; l<4; l++) s[k] += E[k][l]*e[l];
        }

/* assemble M matrix */

        for (i=0; i<5; i++) {
            for (j=0; j<5; j++) M[i][j] = 0.0;
        }
        M[0][0] = M[1][1] = s[0];
        M[2][2] = M[3][3] = s[1];
        M[0][2] = M[1][3] = M[2][0] = M[3][1] = s[3];
        M[4][4] = s[2];

/* accumulate element stiffness and load */

        for (i=0; i<nn; i++) {
            for (ii=0; ii<pndof; ii++) {
                ei = 0.0;
                for (l=0; l<3; l++) ei += f[ii]*B[l][ii][i];
                for (l=0; l<4; l++) ei -= B[l][ii][i]*s[l];
                fe[pndof*i+ii] += (ei + f[ii+2]*psi[i])*fac;
                for (j=0; j<nn; j++) {
                    for (jj=0; jj<pndof; jj++) {
                        ej = 0.0;
                        for (k=0; k<4; k++) {
                            ek = 0.0;
                            for (l=0; l<4; l++) ek += E[k][l]*B[l][jj][j];
                            ej += B[k][ii][i]*ek;
                        }
                        for (k=0; k<5; k++) {
                            ek = 0.0;
                            for (l=0; l<5; l++) ek += M[k][l]*G[l][jj][j];
                            ej += G[k][ii][i]*ek;
                        }
                        ke[pndof*i+ii][pndof*j+jj] += ej*fac;
                    }
                }
            }
        }
    }
}
