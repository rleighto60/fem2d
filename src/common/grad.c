/*
$Id: grad.c 1.2 1994/09/12 19:11:27 leighton Exp $
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


program     -   grad.c
author      -   Russell Leighton
date        -   31 Mar 1993
update      -   29 Jun 1994

input       -   n    - element number
                dpde - array of shape function derivatives
                dpdn - array of shape function derivatives

output      -   J    - Jacobian transformation matrix
                detJ - determinant of the Jacobian

description -   Routine to compute shape function gradient

*/

#include "header.h"

int grad(int n, REAL *dpde, REAL *dpdn, REAL J[2][2], REAL *detJ)
{
    int i,l,ns,nn;

    ns = elems[n].type;
    nn = shapes[ns]->nn;

/* calculate Jacobian */

    J[0][0] = J[0][1] = J[1][0] = J[1][1] = 0.0;
    for (i=0; i<nn; i++) {
        l = nnums[elems[n].nodes[i]];
        J[0][0] += dpde[i]*nodes[l].x;
        J[0][1] += dpdn[i]*nodes[l].x;
        J[1][0] += dpde[i]*nodes[l].y;
        J[1][1] += dpdn[i]*nodes[l].y;
    }

/* calculate determinant of Jacobian */

    *detJ = J[0][0]*J[1][1] - J[0][1]*J[1][0];
    if (*detJ < -1.e-20) {
        sprintf(lasterr,"grad - warning in element %d, negative Jacobian",
                ABS(elems[n].id));
        severity = 5;
    }
    else if (*detJ < 1.e-20) {
        sprintf(lasterr,"grad - error in element %d, ill-conditioned Jacobian",
                ABS(elems[n].id));
        severity = 10;
        return(0);
    }

    return(1);
}

/*

program     -   dshape.c
author      -   Russell Leighton
date        -   29 Jun 1994

input       -   n    - element number
                J    - Jacobian transformation matrix
                detJ - determinant of the Jacobian
                dpde - array of shape function derivatives
                dpdn - array of shape function derivatives

output      -   dpdx - shape function derivatives wrt x
                dpdy - shape function derivatives wrt y

description -   Routine to compute shape function derivatives wrt to global
                coordinate system.

*/

void dshape(n,J,detJ,dpde,dpdn,dpdx,dpdy)
int n;
REAL J[2][2],detJ,*dpde,*dpdn,*dpdx,*dpdy;
{
    REAL JI[2][2];
    int i,ns,nn;

    ns = elems[n].type;
    nn = shapes[ns]->nn;

/* calculate shape function derivatives */

    JI[0][0] =  J[1][1]/detJ; JI[0][1] = -J[0][1]/detJ; 
    JI[1][0] = -J[1][0]/detJ; JI[1][1] =  J[0][0]/detJ;
    for (i=0; i<nn; i++) {
        dpdx[i] = dpde[i]*JI[0][0] + dpdn[i]*JI[1][0];
        dpdy[i] = dpde[i]*JI[0][1] + dpdn[i]*JI[1][1];
    }
}
