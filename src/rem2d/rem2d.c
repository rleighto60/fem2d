/*
$Id: rem2d.c 1.2 1994/09/12 19:11:27 leighton Exp $
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

program     -   rem2d.c
author      -   Russell Leighton
date        -   27 Oct 1993

description -   Program to remove unreferenced nodes.

*/

#define MAIN
#include "header.h"
#include "iff.h"

extern int readdata();
extern void freemem();

int resavedata(char *);

int main( int argc, char *argv[] )
{
    int i;
    char *in=0,*out=0;

    if(argc > 1) {
        for(i=1;i<argc;i++) {
            if(!in) in = argv[i];
            else if(!out) out = argv[i];
        }
        if(!readdata(in)) {
            fprintf(stderr,"main - error reading data file");
            return 1;
        }
        if(out) resavedata(out);
        else resavedata(in);
        freemem(0,-1);
        return 0;
    }
    fprintf(stderr,"Usage: rem2d <input file> [output file]\n");
    return 1;
}

int resavedata(char *fspec)
{
    FILE *fp;
    Chunk header;
    long id;
    long formSize;
    int i,n,nn,nb;
   
    nn = 0;
    nmax = 0;
    for(i=0;i<nnodes;i++) {
        if(nodes[i].id > 0) {
            nmax = MAX(nmax,ABS(nodes[i].id));
            nn++;
        }
        else {
            if((n = nodes[i].bc) >= 0) {
                do {
                    pbcs[n].type = 0;
                } while((n = pbcs[n].next) >= 0);
            }
        }
    }
    nmax++;
    nb = 0;
    for(i=0;i<npbcs;i++) {
        if(pbcs[i].type != 0) nb++;
    }

    if((fp = fopen(fspec,"w")) == NULL) return(1);

    formSize = sizeof(id) + sizeof(header);
    if(nodes) formSize += sizeof(header) + nn*sizeof(struct node);
    if(pbcs) formSize += sizeof(header) + nb*sizeof(struct pbc);

    header.ckID = ID_FORM;
    header.ckSize = formSize;
    SafeWrite(fp,&header,sizeof(header));

    id = ID_FE2D;
    SafeWrite(fp,&id,sizeof(id));

    if(nodes) {
        header.ckID = ID_NODS;
        header.ckSize = nn*sizeof(struct node);
        SafeWrite(fp,&header,sizeof(header));
        for(i=0;i<nnodes;i++) {
            if(nodes[i].id > 0) SafeWrite(fp,&nodes[i],sizeof(struct node));
        }
    }
    if(pbcs) {
        header.ckID = ID_PBCS;
        header.ckSize = nb*sizeof(struct pbc);
        SafeWrite(fp,&header,sizeof(header));
        for(i=0;i<npbcs;i++) {
            if(pbcs[i].type != 0) SafeWrite(fp,&pbcs[i],sizeof(struct pbc));
        }
    }
    header.ckID = ID_ENDD;
    header.ckSize = 0L;
    SafeWrite(fp,&header,sizeof(header));

    fclose(fp);
    return(0);
}
