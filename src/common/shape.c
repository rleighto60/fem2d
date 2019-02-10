/*
$Id: shape.c 1.2 1994/09/12 19:11:27 leighton Exp $
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

program     -   shape.c
author      -   Russell Leighton
date        -   28 Feb 1992

input       -   xi,yi- location where shape function values/derivs are calculated
                id   - shape function id

output      -   psi  - array of shape function values
                dpdx - array of shape function derivatives wrt xi
                dpdy - array of shape function derivatives wrt yi

description -   Finite element shape function routine.

*/

#include "header.h"

REAL calc_psi(float xi, float yi, int id, int l, int n)
{
    int i,j;
    REAL d,dx,dy,ds;

    i = shapes[id]->i[l];
    j = shapes[id]->j[l];
    dx = (REAL)(shapes[id]->xi[j] - shapes[id]->xi[i]);
    dy = (REAL)(shapes[id]->yi[j] - shapes[id]->yi[i]);
    ds = dy*shapes[id]->xi[i] - dx*shapes[id]->yi[i];
    d  = dx*shapes[id]->yi[n] - dy*shapes[id]->xi[n] + ds;
    return((dx*yi - dy*xi + ds)/d);
}

REAL calc_dpx(int id, int l, int n)
{
    int i,j;
    REAL d,dx,dy,ds;

    i = shapes[id]->i[l];
    j = shapes[id]->j[l];
    dx = (REAL)(shapes[id]->xi[j] - shapes[id]->xi[i]);
    dy = (REAL)(shapes[id]->yi[j] - shapes[id]->yi[i]);
    ds = dy*shapes[id]->xi[i] - dx*shapes[id]->yi[i];
    d  = dx*shapes[id]->yi[n] - dy*shapes[id]->xi[n] + ds;
    return(-dy/d);
}

REAL calc_dpy(int id, int l, int n)
{
    int i,j;
    REAL d,dx,dy,ds;

    i = shapes[id]->i[l];
    j = shapes[id]->j[l];
    dx = (REAL)(shapes[id]->xi[j] - shapes[id]->xi[i]);
    dy = (REAL)(shapes[id]->yi[j] - shapes[id]->yi[i]);
    ds = dy*shapes[id]->xi[i] - dx*shapes[id]->yi[i];
    d  = dx*shapes[id]->yi[n] - dy*shapes[id]->xi[n] + ds;
    return(dx/d);
}

void shape(float xi, float yi, int id, REAL *psi, REAL *dpdx, REAL *dpdy)
{
    REAL dp;
    int i,j,k,nn,nl;

    nn = shapes[id]->nn;
    nl = shapes[id]->nl;

    for(i=0;i<nn;i++) {
        psi[i] = 1.0; dpdx[i] = dpdy[i] = 0.0;
        for(j=0;j<nl;j++) {
            if((1<<j) & shapes[id]->ln[i]) {
                psi[i] *= calc_psi(xi,yi,id,j,i);
                dp = 1.0;
                for(k=0;k<nl;k++) {
                    if((k != j) && ((1<<k) & shapes[id]->ln[i]))
                        dp *= calc_psi(xi,yi,id,k,i);
                }
                dpdx[i] += dp*calc_dpx(id,j,i);
                dpdy[i] += dp*calc_dpy(id,j,i);
            }
        }
    }
}
