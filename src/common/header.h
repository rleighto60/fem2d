/*
$Id: header.h 1.2 1994/09/12 19:11:27 leighton Exp $
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

program     -   header.h

description -   Header file for FE routines.

$Log: header.h $
 * Revision 1.3  1995/01/17  19:11:27  leighton
 * added support for point, line, label, and symbol objects.
 *
 * Revision 1.2  1994/09/12  19:11:27  leighton
 * added 'struct view' and pointer to 'views' which replaces 'pubwindow'
 * and associated variables.
 *
 * Revision 1.1  1994/09/12  19:02:14  leighton
 * Initial revision
 *
*/


#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <string.h>

#define MAXARG 24
#define FALSE 0
#define TRUE 1

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
#define ABS(x) ((x<0)?(-(x)):(x))
#define sqr(x) pow(x,0.5)

#undef NULL
#define NULL        ((void *)0L)

#ifndef PI
#define PI	  ((float) 3.141592653589793)
#endif

typedef unsigned char UBYTE;
typedef unsigned short UWORD;
typedef unsigned long ULONG;
typedef short WORD;
typedef char *STRPTR;
typedef double REAL;

typedef REAL *MATRIX;
typedef REAL *VECTOR;

struct problem {
    int ndof;
    char title[40];
};
struct mat {
    int id,next,type,kind,ckind[2];
    float C[10],f[4],ref[2];
};
struct lam {
    int id,next,mat,func;
    float mang,frac;
};
struct node {
    int id,bc;
    float x,y;
};
struct elem {
    int id,bc,type,mat,nodes[9];
};
struct pbc {
    int id,jd,next,type,kind,dof;
    float val;
};
struct dbc {
    int id,jd,next,type,kind,dof,side;
    float val[3];
};
struct soln {
    int type,kind,dof,step;
    float *u,v,dvdx,dvdy;
};
struct result {
    float val[9];
};
struct set {
    int kind,ndof,flag,func;
    float min,max;
    struct result *results;
};
struct fun {
    char name[12];
    char *expr;
    int nchar;
};
struct group {
    char name[12];
    int nmax;
    long *ng;
};
struct point {
    int id,type,col,next;
    float x,y,ang;
};
struct line {
    int id,type,col,pid;
};
struct label {
    int id,type,pid;
    char *string;
    int nchar;
};
struct color {
    WORD    ColorIndex;
    UWORD   Red;
    UWORD   Green;
    UWORD   Blue;
};
struct intg {
    int ni;
    float *xip,*yip,*w;
};
struct shape {
    int nn,ns,nl,no;
    UWORD *ln;
    int *i,*j;
    float *xi,*yi;
    struct intg *cintg,*eintg;
    int flag;
    REAL *psi,*dpde,*dpdn;
};

#ifdef MAIN
#include "shapes.h"

#ifdef LIN2D

#define VERSION "2.1"

MATRIX kg = 0;
VECTOR fg = 0;
long *dpnts=0;
long mallcd;
int mprint=FALSE;
char *_args[MAXARG];
char hostname[] = "LIN2D";
#endif

#ifdef NLN2D

#define VERSION "1.3"

MATRIX kg = 0;
VECTOR fg = 0;
long *dpnts=0;
long mallcd;
int mprint=FALSE;
char *_args[MAXARG];
char hostname[] = "NLN2D";
#endif

#ifdef POST2D

#define VERSION "1.2"

char *_args[MAXARG];
char hostname[] = "POST2D";
#endif

struct problem  *probs = 0;
struct mat      *mats = 0;
struct lam      *lams = 0;
struct node     *nodes = 0;
struct elem     *elems = 0;
struct pbc      *pbcs = 0;
struct dbc      *dbcs = 0;
struct group    *groups = 0;
struct soln     *solns = 0;
struct fun      *funs = 0;
struct fun      *aliases = 0;
struct set      *sets = 0;
struct point    *points = 0;
struct line     *lines = 0;
struct label    *labels = 0;
int nprobs=0,nmats=0,nlams=0,nnodes=0,nelems=0,npbcs=0,ndbcs=0;
int ngrps=0,nsolns=0,nfuns=0,nsets=0,nalias=0;
int npnts=0,nlins=0,nlabs=0;
int mmax=1,lmax=1,nmax=1,emax=1;
int pmax=1,imax=1,jmax=1;
int *mnums=0,*lnums=0,*nnums=0,*enums=0;
int *pnums=0,*inums=0,*jnums=0;
int ptype=0,pkind=0,pndof=2,ctype=0,print=FALSE,tprint=FALSE;
int bw=0,nw=0;
long severity=0L,errorcode=0L;
char lasterr[80] = {0};
struct color colors[13] = {
     { 4, 0, 0,15 },
     { 5, 0, 9,15 },
     { 6, 0,13,15 },
     { 7, 0,15,15 },
     { 8, 0,15, 8 },
     { 9, 0,15, 0 },
     {10,12,15, 0 },
     {11,15,15, 0 },
     {12,15,13, 0 },
     {13,15, 9, 0 },
     {14,15, 5, 0 },
     {15,15, 0, 0 },
     {-1, 0, 0, 0 } 
};

int close_down = FALSE;
int outstanding = 0;

#else

#ifdef LIN2D
extern MATRIX kg;
extern VECTOR fg;
extern long *dpnts;
extern long mallcd;
extern int mprint;
#endif

#ifdef NLN2D
extern MATRIX kg;
extern VECTOR fg;
extern long *dpnts;
extern long mallcd;
extern int mprint;
#endif

extern struct problem   *probs;
extern struct mat       *mats;
extern struct lam       *lams;
extern struct node      *nodes;
extern struct elem      *elems;
extern struct pbc       *pbcs;
extern struct dbc       *dbcs;
extern struct group     *groups;
extern struct soln      *solns;
extern struct fun       *funs;
extern struct fun       *aliases;
extern struct set       *sets;
extern struct point     *points;
extern struct line      *lines;
extern struct label     *labels;
extern struct shape     *shapes[11];
extern int nprobs,nmats,nlams,nnodes,nelems,npbcs,ndbcs;
extern int ngrps,nsolns,nfuns,nsets,nalias;
extern int npnts,nlins,nlabs;
extern int mmax,lmax,nmax,emax;
extern int pmax,imax,jmax;
extern int *mnums,*lnums,*nnums,*enums;
extern int *pnums,*inums,*jnums;
extern int ptype,pkind,pndof,ctype,print,tprint;
extern int bw,nw;
extern long severity,errorcode;
extern char lasterr[80];
extern struct color colors[13];
#endif

void *malloc();
void *realloc();
void *calloc();
double pow(),sin(),cos(),tan(),asin(),acos(),atan2();
