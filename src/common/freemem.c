/*
$Id: freemem.c 1.2 1994/09/12 19:11:27 leighton Exp $
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


program     -   freemem.c
author      -   Russell Leighton
date        -   31 Mar 1993

input       -   flag
                n

description -   Routine to free memory

*/

#include "header.h"

void freemem(flag,n)
int flag,n;
{
    int i;

    if(flag <= 0) {
        if(mnums) { free(mnums); mnums = 0; }
        if(lnums) { free(lnums); lnums = 0; }
        if(nnums) { free(nnums); nnums = 0; }
        if(enums) { free(enums); enums = 0; }
        if(nodes) { free(nodes); nodes = 0; }
        if(elems) { free(elems); elems = 0; }
        if(probs) { free(probs); probs = 0; }
        if(mats) { free(mats); mats = 0; }
        if(lams) { free(lams); lams = 0; }
        if(pbcs) { free(pbcs); pbcs = 0; }
        if(dbcs) { free(dbcs); dbcs = 0; }
        nprobs = nmats = nlams = nnodes = nelems = npbcs = ndbcs = bw = nw = 0;
        mmax = lmax = nmax = emax = 1;
    }
    if((flag <= 0) || (flag == 1)) {
        for(i=0;i<nsolns;i++) free(solns[i].u);
        if(solns) { free(solns); solns = 0; }
        nsolns = 0;
    }
    if((flag <= 0) || (flag == 1) || (flag == 2)) {
        if(n < 0) {
            for(i=0;i<nsets;i++) {
                if(sets[i].results) { free(sets[i].results); sets[i].results = 0; }
            }
            if(sets) { free(sets); sets = 0; }
            nsets = 0;
        }
        else if(n != nsets) {
            if(sets[n].results) { free(sets[n].results); sets[n].results = 0; }
        }
    }
    if((flag <= 0) || (flag == 3)) {
        for(i=0;i<ngrps;i++) free(groups[i].ng);
        if(groups) { free(groups); groups = 0; }
        ngrps = 0;
    }
    if((flag == 0) || (flag == 4)) {
        for(i=0;i<nfuns;i++) free(funs[i].expr);
        if(funs) { free(funs); funs = 0; }
        nfuns = 0;
    }
    if((flag == 0) || (flag == 5)) {
        for(i=0;i<nalias;i++) free(aliases[i].expr);
        if(aliases) { free(aliases); aliases = 0; }
        nalias = 0;
    }
    if((flag <= 0) || (flag == 6)) {
        if(points) { free(points); points = 0; }
        if(lines) { free(lines); lines = 0; }
        for(i=0;i<nlabs;i++) free(labels[i].string);
        if(labels) { free(labels); labels = 0; }
        if(pnums) { free(pnums); pnums = 0; }
        if(inums) { free(inums); inums = 0; }
        if(jnums) { free(jnums); jnums = 0; }
        npnts = nlins = nlabs = 0;
        pmax = imax = jmax = 1;
    }
}
