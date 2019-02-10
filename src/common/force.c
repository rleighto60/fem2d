/*
$Id: force.c 1.2 1994/09/12 19:11:27 leighton Exp $
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


program     -   force.c
author      -   Russell Leighton
date        -   31 Mar 1993

input       -   n   - element number
                m   - material number
                psi - array of shape function values

output      -   f   - array of body force components

description -   Routine to compute body force components

*/

#include "header.h"

void force(n,m,psi,f)
int n,m;
REAL *psi,*f;
{
    REAL T[2];
    int i,j,l,ns,nn;

    ns = elems[n].type;
    nn = shapes[ns]->nn;

/* calculate coupled solution values */

    T[0] = T[1] = 0.0;
    for (i=0; i<nn; i++) {
        l = nnums[elems[n].nodes[i]];
        if (mats[m].ckind[0] != pkind) {
            for (j=0; j<nsolns; j++) {
                if ((solns[j].kind == mats[m].ckind[0]) && (solns[j].type == 0)) {
                    T[0] += psi[i]*solns[j].u[l];
                    break;
                }
            }
        }
        if (mats[m].ckind[1] != pkind) {
            for (j=0; j<nsolns; j++) {
                if ((solns[j].kind == mats[m].ckind[1]) && (solns[j].type == 0)) {
                    T[1] += psi[i]*solns[j].u[l];
                    break;
                }
            }
        }
    }

/* calculate body force */

    for (j=0; j<4; j++) f[j] = (REAL)mats[m].f[j];
    if (mats[m].ckind[0] != pkind) {
        f[0] *= T[0]-(REAL)mats[m].ref[0];
        f[1] *= T[0]-(REAL)mats[m].ref[0];
    }
    if (mats[m].ckind[1] != pkind) {
        f[2] *= T[1]-(REAL)mats[m].ref[1];
        f[3] *= T[1]-(REAL)mats[m].ref[1];
    }
}
