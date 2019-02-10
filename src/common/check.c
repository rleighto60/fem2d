/*
$Id: check.c 1.2 1994/09/12 19:11:27 leighton Exp $
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

program     -   check.c

description -   Check model integrity.

*/
#include "header.h"

extern void error(char *mess);

int checkelem(code,n)
char *code;
int n;
{
    int i,nn,node,err=0;
    char errmess[80];

    if((mnums[elems[n].mat] < 0) || (elems[n].mat >= mmax)) {
        sprintf(errmess,"%s - error in element %d, undefined material %d",
                code,ABS(elems[n].id),elems[n].mat);
        error(errmess);
        err = 1;
    }
    if((elems[n].type < 0) || (elems[n].type > 10)) {
        sprintf(errmess,"%s - error in element %d, undefined type %d",
                code,ABS(elems[n].id),elems[n].type);
        error(errmess);
        return(0);
    }
    nn = shapes[elems[n].type]->nn;
    for(i=0;i<nn;i++) {
        node = nnums[elems[n].nodes[i]];
        if((node < 0) || (node >= nnodes) || (elems[n].nodes[i] >= nmax)) {
            sprintf(errmess,"%s - error in element %d, undefined node %d",
                    code,ABS(elems[n].id),elems[n].nodes[i]);
            error(errmess);
            err = 1;
        }
    }

    if(!err) return(1);
    else return(0);
}

int checkpbc(code,n)
char *code;
int n;
{
    int id;
    char errmess[80];

    id = pbcs[n].id;
    if((nnums[id] < 0) || (id >= nmax)) {
        sprintf(errmess,
            "%s - error in boundary condition, undefined node %d",code,id);
        error(errmess);
        return(0);
    }
    return(1);
}

int checkdbc(code,n)
char *code;
int n;
{
    int id;
    char errmess[80];

    id = dbcs[n].id;
    if((enums[id] < 0) || (id >= emax)) {
        sprintf(errmess,
            "%s - error in boundary condition, undefined element %d",code,id);
        error(errmess);
        return(0);
    }
    if(!checkelem(code,enums[id])) return(0);
    return(1);
}

int check()
{
    int n,ier,ier1=1;

    for(n=0;n<nelems;n++) {
        if(elems[n].id == 0) continue;
        if(!(ier = checkelem("check",n))) {
            ier1 &= ier;
            continue;
        }
    }
    return(ier1);
}
