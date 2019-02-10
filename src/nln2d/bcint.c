/*
$Id: bcint.c 1.2 1994/09/12 19:11:27 leighton Exp $
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

program     -   bcint.c
author      -   Russell Leighton
date        -   23 Apr 1992

input       -   m  - bc number
                u  - element nodal point solutions

output      -   ke - stiffness matrix
                fe - load vector

description -   Routine to integrate distributed load over element side.
                Performs numerical integration to compute load values using 
                flux values passed and shape function values/derivatives 
                supplied by shape().

*/

#include "header.h"

extern int checkdbc();

float xip[4][4] = { { 0.0000000000,  0.0000000000,  0.0000000000,  0.0000000000 },
                    {-0.5773502692,  0.5773502692,  0.0000000000,  0.0000000000 },
                    {-0.7745966692,  0.0000000000,  0.7745966692,  0.0000000000 },
                    {-0.8611363116, -0.3399810436,  0.3399810436,  0.8611363116 } };
float w[4][4]  =  { { 2.0000000000,  0.0000000000,  0.0000000000,  0.0000000000 },
                    { 1.0000000000,  1.0000000000,  0.0000000000,  0.0000000000 },
                    { 0.5555555556,  0.8888888889,  0.5555555556,  0.0000000000 },
                    { 0.3478548451,  0.6521451549,  0.6521451549,  0.3478548451 } };

/*

program     -   shape.c
author      -   Russell Leighton
date        -   28 Feb 1992

input       -   xi   - value where shape function values/derivs are calculated
                n    - order of the shape function

output      -   psi  - array of shape function values
                dpsi - array of shape function derivatives

description -   Finite element side shape function routine. 

*/

void sshape(float xi, int n, REAL *psi, REAL *dpsi)
{
    REAL f,d,dp;
    int i,j,k;

    f = 2.0/(REAL)n;
    for (i=0; i<=n; i++) {
        psi[i] = d = 1.0; dpsi[i] = 0.0;
        for (j=0; j<=n; j++) {
            if (j != i) {
                psi[i] *= ((REAL)xi - j*f + 1.0);
                dp = 1.0;
                for (k=0; k<=n; k++) {
                    if ((k != i) && (k != j)) dp *= ((REAL)xi - k*f + 1.0);
                }
                dpsi[i] += dp;
                d *= (i-j)*f;
            }
        }
        psi[i] /= d;
        dpsi[i] /= d;
    }
}

void bcint(int m, REAL u[2][9], REAL ke[18][18], REAL fe[18])
{
    float xi;
    REAL x,y,r,dxde,dyde,dval,J,fac,pen;
    REAL psi[4],dpsi[4],p[2],n[2];
    int i,j,k,jj,kk,ip,is,ne,nt,ns,nn,ni,nm,nds[4];
    
    if (m < 0) return;
    if(dbcs[m].type == 0) return;
    ne = enums[dbcs[m].id];
    nt = elems[ne].type;
    ni = shapes[nt]->no;
    ns = shapes[nt]->ns;
    nn = ni + 1;
    nm = ni - 1;

    while (m >= 0) {
        if(!(dbcs[m].type) || (dbcs[m].kind != pkind)) {
            m = dbcs[m].next;
            continue;
        }
        if(!checkdbc("bcint",m)) return;

/* collect nodes on element side */

        is = dbcs[m].side - 1;
        nds[0] = is;
        for (i=0;i<nm;i++) nds[i+1] = ns+is*nm+i;
        if (is < ns-1) nds[ni] = is+1;
        else nds[ni] = 0;

/* loop over integration points */

        for (ip=0; ip<ni; ip++) {
            xi = xip[ni-1][ip];

/* compute gradient and normal to element side */

            sshape(xi,ni,psi,dpsi);
            dxde = dyde = 0.0;
            for (i=0; i<nn; i++) {
                x = (REAL)nodes[nnums[elems[ne].nodes[nds[i]]]].x + u[0][nds[i]];
                y = (REAL)nodes[nnums[elems[ne].nodes[nds[i]]]].y + u[1][nds[i]];
                dxde += dpsi[i]*x;
                dyde += dpsi[i]*y;
            }
            J = (REAL)sqr(dxde*dxde + dyde*dyde);
            p[0] = 1.0;
            p[1] = 0.0;
            if (pndof > 1) {
                n[0] = dyde/J;
                n[1] = -dxde/J;
                switch (dbcs[m].dof) {
                    case 1: p[0] = n[0]; p[1] = n[1]; break;
                    case 2: p[0] = n[1]; p[1] = n[0]; break;
                }
            }
            fac = J*w[ni-1][ip];

/* for axisymmetry... */

            r = 0.0;
            if (ptype) { 
                for (i=0; i<nn; i++) 
                    r += (REAL)psi[i]*(nodes[nnums[elems[ne].nodes[nds[i]]]].x + u[0][nds[i]]);
                fac *= r;
            }

            switch (dbcs[m].type) {

/* apply Dirchlet type boundary condition (using penalty method) */

                case 1:
                    dval = 0.0;
                    pen = ke[0][0]*1.e7;
                    for (i=0; i<nn; i++) dval += psi[i]*dbcs[m].val[i];
                    for (i=0; i<nn; i++) {
                        for (j=0; j<pndof; j++) {
                            jj = pndof*nds[i]+j;
                            fe[jj] += pen*(p[j]*dval - u[j][nds[i]])*psi[i]*fac;
                            for (k=0; k<pndof; k++) {
                                kk = pndof*nds[i]+k;
                                ke[jj][kk] += p[j]*p[k]*pen*psi[i]*fac;
                            }
                        }
                    }
                    break;

/* apply flux type boundary condition */

                case 2:
                    dval = 0.0;
                    for (i=0; i<nn; i++) dval += psi[i]*dbcs[m].val[i];
                    for (i=0; i<nn; i++) {
                        for (j=0; j<pndof; j++) fe[pndof*nds[i]+j] += p[j]*dval*psi[i]*fac;
                    }
                    break;
            }
        }
        m = dbcs[m].next;
    }
}
