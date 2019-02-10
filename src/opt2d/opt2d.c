/*
$Id: opt2d.c 1.2 1994/09/12 19:11:27 leighton Exp $
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

program     -   opt2d.c
author      -   Russell Leighton
date        -   17 Apr 1992

description -   Program to determine an optimum node order to reduce
                the bandwidth for a two dimensional finite element mesh.

*/

#define MAIN
#include "header.h"
#include "iff.h"

extern int readdata();
extern void freemem();

int optimize(int, char *);
void addnodes(int *, int *, int *, int *);
int resavedata(char *, int *);

int main( int argc, char *argv[] )
{
    int i,n=1;
    char *in=0,*out=0;

    if(argc > 1) {
        for(i=1;i<argc;i++) {
            if(argv[i][0] == '-') {
                if(argv[i][1] == 'n') sscanf(&argv[i][2],"%d",&n);
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
        if(out) optimize(n,out);
        else optimize(n,in);
        freemem(0,-1);
        return 0;
    }
    fprintf(stderr,"Usage: opt2d <input file> [output file]\n");
    return 1;
}

int optimize(int n, char *datafile)
{
    int *nlist,*elist,*plist,slist[2];

    if((nlist = (int *)calloc((size_t)nnodes,sizeof(int))) == NULL) {
        fprintf(stderr,"optimize - could not allocate memory for node list\n");
        return(1);
    }
    if((elist = (int *)calloc((size_t)nelems,sizeof(int))) == NULL) {
        fprintf(stderr,"optimize - could not allocate memory for element list\n");
        free(nlist);
        return(1);
    }
    if((plist = (int *)calloc((size_t)(nnodes+1),sizeof(int))) == NULL) {
        fprintf(stderr,"optimize - could not allocate memory for optimized node list\n");
        free(nlist); free(elist);
        return(1);
    }
    slist[0] = 1;
    slist[1] = n;
    plist[0] = 0;
    addnodes(nlist,elist,slist,plist);
    resavedata(datafile,plist);
    free(nlist); free(elist); free(plist);
    return(0);
}

void addnodes(int *nlist, int *elist, int *slist, int *plist)
{
    int i,j,n,m,nn,nm,ns,pnode,test;
    int *tlist;

    if((tlist = (int *)calloc(4L*slist[0]+1L,sizeof(int))) == NULL) return;

    for(j=1;j<=slist[0];j++) {
        ns = slist[j];
        nn = shapes[elems[ns].type]->nn;
        for(i=0;i<nn;i++) {
            pnode = nnums[elems[ns].nodes[i]];
            if(!nlist[pnode]) {
                nlist[pnode] = 1;
                plist[++(plist[0])] = pnode;
            }
        }
        elist[ns] = 1;
        nn = shapes[elems[ns].type]->ns;
        for(i=0;i<nelems;i++) {
            if(elems[i].id == 0) continue;
            if(!elist[i]) {
                nm = shapes[elems[i].type]->ns;
                test = 0;
                for(n=0;n<nn;n++) {
                    for(m=0;m<nm;m++) {
                        if(elems[ns].nodes[n] == elems[i].nodes[m]) test++;
                    }
                }
                if(test > 1) {
                    (tlist[0])++;
                    tlist[tlist[0]] = i;
                    elist[i] = 1;
                }
            }
        }
    }
    if(tlist[0]) addnodes(nlist,elist,tlist,plist);
    free(tlist);
}

int resavedata(char *fspec, int *plist)
{
    FILE *fp;
    Chunk header;
    long id;
    long formSize;
    int i;
   
    if((fp = fopen(fspec,"w")) == NULL) return(1);

    formSize = sizeof(id) + sizeof(header);
    if(nodes) formSize += sizeof(header) + plist[0]*sizeof(struct node);
    if(pbcs) formSize += sizeof(header) + npbcs*sizeof(struct pbc);

    header.ckID = ID_FORM;
    header.ckSize = formSize;
    SafeWrite(fp,&header,sizeof(header));

    id = ID_FE2D;
    SafeWrite(fp,&id,sizeof(id));

    if(nodes) {
        header.ckID = ID_NODS;
        header.ckSize = plist[0]*sizeof(struct node);
        SafeWrite(fp,&header,sizeof(header));
        for(i=1;i<=plist[0];i++)
            SafeWrite(fp,&nodes[plist[i]],sizeof(struct node));
    }
    if(pbcs) {
        header.ckID = ID_PBCS;
        header.ckSize = npbcs*sizeof(struct pbc);
        SafeWrite(fp,&header,sizeof(header));
        SafeWrite(fp,pbcs,(size_t)header.ckSize);
    }
    header.ckID = ID_ENDD;
    header.ckSize = 0L;
    SafeWrite(fp,&header,sizeof(header));

    fclose(fp);
    return(0);
}
