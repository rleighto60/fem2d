/*
$Id: shapes.h 1.2 1994/09/12 19:11:27 leighton Exp $
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

program     -   shapes.h
author      -   Russell Leighton
date        -   28 Feb 1992

description -   Header file containing shape function and integration rule data

*/

/* 1 point integration rule for triangular elements */

float t1_xip[1] = { 0.333333333 };
float t1_yip[1] = { 0.333333333 };
float t1_w[1]   = { 0.5 };

struct intg t1_intg = { 1, &t1_xip[0], &t1_yip[0], &t1_w[0] };

/* 3 point integration rule for triangular elements */

float t3_xip[3] = { 0.0,0.5,0.5 };
float t3_yip[3] = { 0.5,0.0,0.5 };
float t3_w[3]   = { 0.166666667,0.166666667,0.166666667 };

struct intg t3_intg = { 3, &t3_xip[0], &t3_yip[0], &t3_w[0] };

/* 1 point integration rule for quadralateral elements */

float q1_xip[1] = { 0.0 };
float q1_yip[1] = { 0.0 };
float q1_w[1]   = { 4.0 };

struct intg q1_intg = { 1, &q1_xip[0], &q1_yip[0], &q1_w[0] };

/* 4 point integration rule for quadralateral elements */

float q4_xip[4] = { -0.577350269,0.577350269,0.577350269,-0.577350269 };
float q4_yip[4] = { -0.577350269,-0.577350269,0.577350269,0.577350269 };
float q4_w[4]   = { 1.0,1.0,1.0,1.0 };

struct intg q4_intg = { 4, &q4_xip[0], &q4_yip[0], &q4_w[0] };

/* 9 point integration rule for quadralateral elements */

float q9_xip[9] = { -0.774596669,0.0,0.774596669,
                    -0.774596669,0.0,0.774596669,
                    -0.774596669,0.0,0.774596669 };
float q9_yip[9] = { -0.774596669,-0.774596669,-0.774596669,
                     0.0,0.0,0.0,
                     0.774596669,0.774596669,0.774596669 };
float q9_w[9]   = {  0.308641975,0.493827161,0.308641975,
                     0.493827161,0.790123457,0.493827161,
                     0.308641975,0.493827161,0.308641975 };

struct intg q9_intg = { 9, &q9_xip[0], &q9_yip[0], &q9_w[0] };

/* 3 node linear triangular element with 1 point integration rule */

UWORD t3_ln[3]  = { 0x02,0x04,0x01 };
int t3_i[3]     = { 0,1,2 };
int t3_j[3]     = { 1,2,0 };
float t3_xi[3]  = { 0.0,1.0,0.0 };
float t3_yi[3]  = { 0.0,0.0,1.0 };

REAL t3_1_psi[3]  = { 0.0 };
REAL t3_1_dpde[3] = { 0.0 };
REAL t3_1_dpdn[3] = { 0.0 };

struct shape tri3_1 = {
    3,3,3,1,
    &t3_ln[0], &t3_i[0], &t3_j[0], &t3_xi[0], &t3_yi[0], &t1_intg, &t1_intg,
    0,
    &t3_1_psi[0], &t3_1_dpde[0], &t3_1_dpdn[0]
};

/* 6 node quadratic triangular element with 1 point integration rule */

UWORD t6_ln[6]  = { 0x0a,0x14,0x21,0x06,0x05,0x03 };
int t6_i[6]     = { 0,1,2,3,4,5 };
int t6_j[6]     = { 1,2,0,5,3,4 };
float t6_xi[6]  = { 0.0,1.0,0.0,0.5,0.5,0.0 };
float t6_yi[6]  = { 0.0,0.0,1.0,0.0,0.5,0.5 };

REAL t6_1_psi[18] = { 0.0 };
REAL t6_1_dpde[18]= { 0.0 };
REAL t6_1_dpdn[18]= { 0.0 };

struct shape tri6_1 = {
    6,3,6,2,
    &t6_ln[0], &t6_i[0], &t6_j[0], &t6_xi[0], &t6_yi[0], &t1_intg, &t3_intg,
    0,
    &t6_1_psi[0], &t6_1_dpde[0], &t6_1_dpdn[0]
};

/* 6 node quadratic triangular element with 3 point integration rule */

REAL t6_3_psi[18] = { 0.0 };
REAL t6_3_dpde[18]= { 0.0 };
REAL t6_3_dpdn[18]= { 0.0 };

struct shape tri6_3 = {
    6,3,6,2,
    &t6_ln[0], &t6_i[0], &t6_j[0], &t6_xi[0], &t6_yi[0], &t3_intg, &t3_intg,
    0,
    &t6_3_psi[0], &t6_3_dpde[0], &t6_3_dpdn[0]
};

/* 4 node linear quadralateral element with 1 point integration rule */

UWORD q4_ln[4]  = { 0x06,0x0c,0x09,0x03 };
int q4_i[4]     = { 0,1,2,3 };
int q4_j[4]     = { 1,2,3,0 };
float q4_xi[4]  = { -1.0,1.0,1.0,-1.0 };
float q4_yi[4]  = { -1.0,-1.0,1.0,1.0 };

REAL q4_1_psi[4] = { 0.0 };
REAL q4_1_dpde[4]= { 0.0 };
REAL q4_1_dpdn[4]= { 0.0 };

struct shape quad4_1 = {
    4,4,4,1,
    &q4_ln[0], &q4_i[0], &q4_j[0], &q4_xi[0], &q4_yi[0], &q1_intg, &q1_intg,
    0,
    &q4_1_psi[0], &q4_1_dpde[0], &q4_1_dpdn[0]
};

/* 4 node linear quadralateral element with 4 point integration rule */

REAL q4_4_psi[16] = { 0.0 };
REAL q4_4_dpde[16]= { 0.0 };
REAL q4_4_dpdn[16]= { 0.0 };

struct shape quad4_4 = {
    4,4,4,1,
    &q4_ln[0], &q4_i[0], &q4_j[0], &q4_xi[0], &q4_yi[0], &q4_intg, &q1_intg,
    0,
    &q4_4_psi[0], &q4_4_dpde[0], &q4_4_dpdn[0]
};

/* 8 node quadratic quadralateral element with 1 point integration rule */

UWORD q8_ln[8]  = { 0x86,0x1c,0x29,0x43,0x0e,0x0d,0x0b,0x07 };
int q8_i[8]     = { 0,1,2,3,4,5,6,7 };
int q8_j[8]     = { 1,2,3,0,5,6,7,4 };
float q8_xi[8]  = { -1.0,1.0,1.0,-1.0,0.0,1.0,0.0,-1.0 };
float q8_yi[8]  = { -1.0,-1.0,1.0,1.0,-1.0,0.0,1.0,0.0 };

REAL q8_1_psi[32] = { 0.0 };
REAL q8_1_dpde[32]= { 0.0 };
REAL q8_1_dpdn[32]= { 0.0 };

struct shape quad8_1 = {
    8,4,8,2,
    &q8_ln[0], &q8_i[0], &q8_j[0], &q8_xi[0], &q8_yi[0], &q1_intg, &q4_intg,
    0,
    &q8_1_psi[0], &q8_1_dpde[0], &q8_1_dpdn[0]
};

/* 8 node quadratic quadralateral element with 4 point integration rule */

REAL q8_4_psi[32] = { 0.0 };
REAL q8_4_dpde[32]= { 0.0 };
REAL q8_4_dpdn[32]= { 0.0 };

struct shape quad8_4 = {
    8,4,8,2,
    &q8_ln[0], &q8_i[0], &q8_j[0], &q8_xi[0], &q8_yi[0], &q4_intg, &q4_intg,
    0,
    &q8_4_psi[0], &q8_4_dpde[0], &q8_4_dpdn[0]
};

/* 8 node quadratic quadralateral element with 9 point integration rule */

REAL q8_9_psi[72] = { 0.0 };
REAL q8_9_dpde[72]= { 0.0 };
REAL q8_9_dpdn[72]= { 0.0 };

struct shape quad8_9 = {
    8,4,8,2,
    &q8_ln[0], &q8_i[0], &q8_j[0], &q8_xi[0], &q8_yi[0], &q9_intg, &q4_intg,
    0,
    &q8_9_psi[0], &q8_9_dpde[0], &q8_9_dpdn[0]
};

/* 9 node quadratic quadralateral element with 1 point integration rule */

UWORD q9_ln[9]  = { 0x36,0x3c,0x39,0x33,0x1e,0x2d,0x1b,0x27,0x0f };
int q9_i[6]     = { 0,1,2,3,5,6 };
int q9_j[6]     = { 1,2,3,0,7,4 };
float q9_xi[9]  = { -1.0,1.0,1.0,-1.0,0.0,1.0,0.0,-1.0,0.0 };
float q9_yi[9]  = { -1.0,-1.0,1.0,1.0,-1.0,0.0,1.0,0.0,0.0 };

REAL q9_1_psi[36] = { 0.0 };
REAL q9_1_dpde[36]= { 0.0 };
REAL q9_1_dpdn[36]= { 0.0 };

struct shape quad9_1 = {
    9,4,6,2,
    &q9_ln[0], &q9_i[0], &q9_j[0], &q9_xi[0], &q9_yi[0], &q1_intg, &q4_intg,
    0,
    &q9_1_psi[0], &q9_1_dpde[0], &q9_1_dpdn[0]
};

/* 9 node quadratic quadralateral element with 4 point integration rule */

REAL q9_4_psi[36] = { 0.0 };
REAL q9_4_dpde[36]= { 0.0 };
REAL q9_4_dpdn[36]= { 0.0 };

struct shape quad9_4 = {
    9,4,6,2,
    &q9_ln[0], &q9_i[0], &q9_j[0], &q9_xi[0], &q9_yi[0], &q4_intg, &q4_intg,
    0,
    &q9_4_psi[0], &q9_4_dpde[0], &q9_4_dpdn[0]
};

/* 9 node quadratic quadralateral element with 9 point integration rule */

REAL q9_9_psi[81] = { 0.0 };
REAL q9_9_dpde[81]= { 0.0 };
REAL q9_9_dpdn[81]= { 0.0 };

struct shape quad9_9 = {
    9,4,6,2,
    &q9_ln[0], &q9_i[0], &q9_j[0], &q9_xi[0], &q9_yi[0], &q9_intg, &q4_intg,
    0,
    &q9_9_psi[0], &q9_9_dpde[0], &q9_9_dpdn[0]
};

struct shape *shapes[11] = { &tri3_1,&tri6_1,&tri6_3,&quad4_1,&quad4_4,&quad8_1,&quad8_4,&quad8_9,&quad9_1,&quad9_4,&quad9_9 };
