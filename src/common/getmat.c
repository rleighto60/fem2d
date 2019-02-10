/*
$Id: rotmat.c 1.2 1994/09/12 19:11:27 leighton Exp $
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


program     -   rotmat.c
author      -   Russell Leighton
date        -   29 Jun 1994

input       -   n - element number
                m - material number
                J - Jacobian transformation matrix

output      -   E   - pointer to material properties

description -   Routines to compute transformed/effective material properties
                for composite materials

*/

#include "header.h"

void rotmat(n,m,J,E)
int n,m;
REAL J[2][2],E[4][4];
{
    int i,j,k;
    REAL T[4][4],C[4][4];
    REAL st,ct,ang;

    st = J[1][0] - J[0][1];
    ct = J[0][0] + J[1][1];
    ang = (REAL)atan2(st,ct);
    st = (REAL)sin(ang);
    ct = (REAL)cos(ang);

    T[0][0] = T[1][1] = ct*ct;
    T[0][1] = T[1][0] = st*st;
    T[3][0] = ct*st;
    T[3][1] = -ct*st;
    T[1][3] = 2*ct*st;
    T[0][3] = -2*ct*st;
    T[3][3] = ct*ct - st*st;
    for(i=0;i<4;i++) {
        if(i != 2) T[i][2] = T[2][i] = 0.0;
    }
    T[2][2] = 1.0;

    k = 0;
    for (j=0; j<4; j++) {
        for (i=0; i<(4-j); i++) E[i][i+j] = E[i+j][i] = (REAL)mats[m].C[k++];
    }
    for (i=0; i<4; i++) {
        for (j=0; j<4; j++) {
            C[i][j] = 0.0;
            for (k=0; k<4; k++) C[i][j] += T[i][k]*E[k][j];
        }
    }
    for (i=0; i<4; i++) {
        for (j=0; j<4; j++) {
            E[i][j] = 0.0;
            for (k=0; k<4; k++) E[i][j] += C[i][k]*T[j][k];
        }
    }
}

void laminate(l,ne,m,psi)
int l,ne,m;
REAL *psi;
{
    void lamina();
    extern float getval();
    int i,j,k,n;
    REAL ang,E[6][6];

    for(j=0;j<10;j++) mats[m].C[j] = 0.0;

    n = l;
    while(n >= 0) {
        if(lams[n].func < 0) ang = (REAL)lams[n].mang*PI/180.0;
        else ang = (REAL)getval(funs[lams[n].func].expr,ne,psi)*PI/180.0;
        lamina(lams[n].mat,ang,E);
        k = 0;
        for (j=0; j<4; j++) {
            for (i=0; i<(4-j); i++) mats[m].C[k++] += (float)(lams[n].frac*E[i][i+j]);
        }
        n = lams[n].next;
    }
}

void lamina(m,ang,E)
int m;
REAL ang,E[6][6];
{
    int i,j,k;
    REAL C[6][6],T[6][6];
    REAL st,ct;

    st = (REAL)sin(ang);
    ct = (REAL)cos(ang);

    for(j=0;j<6;j++) {
        for(k=0;k<6;k++) E[j][k] = T[j][k] = 0.0;
    }
    T[0][0] = T[2][2] = ct*ct;
    T[1][1] = 1.0;
    T[3][3] = T[5][5] = ct;
    T[0][2] = T[2][0] = st*st;
    T[4][0] = ct*st;
    T[4][2] = -ct*st;
    T[0][4] = -2*ct*st;
    T[2][4] = 2*ct*st;
    T[4][4] = ct*ct - st*st;
    T[3][5] = -st;
    T[5][3] = st;

    k = 0;
    for (j=0; j<4; j++) {
        for (i=0; i<(4-j); i++) E[i][i+j] = E[i+j][i] = (REAL)mats[m].C[k++];
    }
    E[4][4] = E[3][3];
    E[5][5] = (E[1][1] - E[1][2])/2.0;

    for (i=0; i<6; i++) {
        for (j=0; j<6; j++) {
            C[i][j] = 0.0;
            for (k=0; k<6; k++) C[i][j] += T[i][k]*E[k][j];
        }
    }
    for (i=0; i<6; i++) {
        for (j=0; j<6; j++) {
            E[i][j] = 0.0;
            for (k=0; k<6; k++) E[i][j] += C[i][k]*T[j][k];
        }
    }
}
