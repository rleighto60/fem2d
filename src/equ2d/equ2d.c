/*
$Id: equ2d.c 1.2 1994/09/12 19:11:27 leighton Exp $
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

program     -   equ2d.c
author      -   Russell Leighton
date        -   17 Apr 1992

description -   Program to equivilence nodes at same locations.

*/

#define MAIN
#include "header.h"
#include "iff.h"

extern int readdata(),savedata();
extern void freemem();

int getname(char *);
void equivilence(float, int *);

int main( int argc, char *argv[] )
{
    int getname();
    float tol=1.e-5;
    int i,k,n;
    int *nlist;
    char *in=0,*out=0,*igroup=0,*xgroup=0;

    if(argc > 1) {
        for(i=1;i<argc;i++) {
            if(argv[i][0] == '-') {
                if(argv[i][1] == 'i') igroup = &argv[i][2];
                if(argv[i][1] == 'x') xgroup = &argv[i][2];
                if(argv[i][1] == 't') sscanf(&argv[i][2],"%g",&tol);
            }
            else {
                if(!in) in = argv[i];
                else if(!out) out = argv[i];
            }
        }
        if(!readdata(in)) {
            fprintf(stderr,"main - error reading data file");
            return 1;
        }
        if((nlist = (int *)calloc((size_t)nnodes,sizeof(int))) == NULL) {
            fprintf(stderr,"main - could not allocate memory for node list");
            return 1;
        }
        if(igroup) {
            if((n = getname(igroup)) < ngrps) {
                for(k=0;k<nnodes;k++) {
                    i = nodes[k].id;
                    if((i <= 0) || (i >= groups[n].nmax)) continue;
                    if(groups[n].ng[i>>5] & 1L<<(i%32)) nlist[k] = 0;
                    else nlist[k] = -1;
                }
            }
        }
        if(xgroup) {
            if((n = getname(xgroup)) < ngrps) {
                for(k=0;k<nnodes;k++) {
                    i = nodes[k].id;
                    if((i <= 0) || (i >= groups[n].nmax)) continue;
                    if(groups[n].ng[i>>5] & 1L<<(i%32)) nlist[k] = -1;
                }
            }
        }
        for(i=0;i<nelems;i++) {
            if(elems[i].id > 0) continue;
            n = shapes[elems[i].type]->nn;
            for(k=0;k<n;k++) nlist[nnums[elems[i].nodes[k]]] = -1;
        }

        equivilence(tol,nlist);
        free(nlist);
        if(out) savedata(0,out);
        else savedata(0,in);
        freemem(0,-1);
        return 0;
    }
    fprintf(stderr,"Usage: equ2d  <input file> [output file]\n");
    return 1;
}

int getname(char *name)
{
    int i;

    for(i=0;i<ngrps;i++) {
        if (!strncmp(name,groups[i].name,11)) break;
    }
    return(i);
}

void equivilence(float tol, int *nlist)
{
    int i,j,ni,nn;
    float dx,dy;

    for(i=0;i<nnodes-1;i++) {
        if((nlist[i] < 0) || (nodes[i].id <= 0)) continue;
        for(j=i+1;j<nnodes;j++) {
            if((nlist[j] != 0) || (nodes[j].id <= 0)) continue;
            dx = ABS(nodes[i].x - nodes[j].x);
            dy = ABS(nodes[i].y - nodes[j].y);
            if((dx < tol) && (dy < tol)) nlist[j] = nodes[i].id;
        }
    }

    for(i=0;i<nelems;i++) {
        if(elems[i].id == 0) continue;
        nn = shapes[elems[i].type]->nn;
        for(j=0;j<nn;j++) {
            ni = nnums[elems[i].nodes[j]];
            if(nlist[ni] > 0) elems[i].nodes[j] = nlist[ni];
        }
    }
}
