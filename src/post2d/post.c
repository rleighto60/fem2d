/*
$Id: post.c 1.2 1994/09/12 19:11:27 leighton Exp $
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

program     -   post.c
author      -   Russell Leighton
date        -   11 Aug 1992

input       -   n - result set number

description -   Routine to evaluate function at selected points throughout
                the model.

*/

#include "header.h"

extern int eval();
extern void error();

struct nresult {
    int n;
    float val;
};

int post(int n)
{
    struct nresult *nresults;
    int i,j,k,m,ni,nt,nn,ip;
    float val,vmin,vmax;

    vmin = 1.e17;
    vmax = -1.e17;

    for(i=0;i<nelems;i++) {
        if(elems[i].id == 0) continue;
        if(elems[i].id < 0) {
            sets[n].results[i].val[0] = vmin;
            continue;
        }
        nt = elems[i].type;
        nn = shapes[nt]->nn;
        if(sets[n].flag != 0) ni = nn;
        else ni = shapes[nt]->eintg->ni;
        for(j=0;j<ni;j++) {
            if(sets[n].flag != 0) ip = j+1;
            else ip = -j-1;
            if(!eval(funs[sets[n].func].expr,i,ip,0.0,0.0,&val)) break;
            sets[n].results[i].val[j] = val;
        }
    }

    if(sets[n].flag == 2) {
        if((nresults = (struct nresult *)malloc((size_t)nnodes*sizeof(struct nresult))) == NULL) {
            error("average - could not allocate memory for node results");
            return(0);
        }
        for(m=1;m<mmax;m++) {
            for(i=0;i<nnodes;i++) nresults[i].n = 0;
            for(i=0;i<nelems;i++) {
                if(elems[i].id <= 0) continue;
                if(elems[i].mat == m) {
                    nn = shapes[elems[i].type]->nn;
                    for(j=0;j<nn;j++) {
                        k = nnums[elems[i].nodes[j]];
                        if(nresults[k].n == 0) nresults[k].val = sets[n].results[i].val[j];
                        else nresults[k].val += sets[n].results[i].val[j];
                        ++(nresults[k].n);
                    }
                }
            }
            for(i=0;i<nelems;i++) {
                if(elems[i].id <= 0) continue;
                if(elems[i].mat == m) {
                    nn = shapes[elems[i].type]->nn;
                    for(j=0;j<nn;j++) {
                        k = nnums[elems[i].nodes[j]];
                        if(nresults[k].n != 0) {
                            nresults[k].val /= nresults[k].n;
                            nresults[k].n = 1;
                            sets[n].results[i].val[j] = nresults[k].val;
                        }
                    }
                }
            }
        }
        free(nresults);
    }

    for(i=0;i<nelems;i++) {
        if(elems[i].id <= 0) continue;
        nt = elems[i].type;
        nn = shapes[nt]->nn;
        if(sets[n].flag != 0) ni = nn;
        else ni = shapes[nt]->eintg->ni;
        for(j=0;j<ni;j++) {
            val = sets[n].results[i].val[j];
            vmin = MIN(vmin,val);
            vmax = MAX(vmax,val);
        }
    }
    sets[n].min = vmin;
    sets[n].max = vmax;

    return(1);
}
