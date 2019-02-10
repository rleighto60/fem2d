/*
$Id: iff.c 1.2 1994/09/12 19:11:27 leighton Exp $
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

program     -   iff.c

description -   Save and read data in interchange file format (IFF).

*/
#include "header.h"
#include "iff.h"

extern void freemem();

int savedata(int flag, char *fspec)
{
    FILE *fp;
    Chunk header;
    long id;
    long formSize, size = 0;
    int i,j,nn,ne,nb,nd,np,nl,ns;

    nn = ne = nb = nd = np = nl = ns = 0;
    formSize = sizeof(id) + sizeof(header);
    if (flag == 0) {
        formSize += sizeof(header) + 3L*sizeof(int);
        formSize += sizeof(header) + 13*sizeof(struct color);
        if(probs) formSize += sizeof(header) + nprobs*sizeof(struct problem);
    }
    if ((flag == 0) || (flag == 1)) {
        nn = 0;
        nmax = 0;
        for(i=0;i<nnodes;i++) {
            if(nodes[i].id != 0) {
                nmax = MAX(nmax,ABS(nodes[i].id));
                nn++;
            }
        }
        nmax++;
        ne = 0;
        emax = 0;
        for(i=0;i<nelems;i++) {
            if(elems[i].id == 0) continue;
            for(j=0;j<shapes[elems[i].type]->nn;j++) {
                if(nodes[nnums[elems[i].nodes[j]]].id == 0) break;
            }
            if(j == shapes[elems[i].type]->nn) {
                emax = MAX(emax,ABS(elems[i].id));
                ne++;
            }
        }
        emax++;
        nb = 0;
        for(i=0;i<npbcs;i++) {
            if(pbcs[i].type != 0) nb++;
        }
        nd = 0;
        for(i=0;i<ndbcs;i++) {
            if(dbcs[i].type != 0) nd++;
        }
        if(mats) formSize += sizeof(header) + nmats*sizeof(struct mat);
        if(lams) formSize += sizeof(header) + nlams*sizeof(struct lam);
        if(nodes) formSize += sizeof(header) + nn*sizeof(struct node);
        if(elems) formSize += sizeof(header) + ne*sizeof(struct elem);
        if(pbcs) formSize += sizeof(header) + nb*sizeof(struct pbc);
        if(dbcs) formSize += sizeof(header) + nd*sizeof(struct dbc);
        size = (nmax + 31)>>5;
        if(groups) formSize += ngrps*(sizeof(header) + sizeof(struct group) + size*sizeof(long));
    }
    if ((flag == 0) || (flag == 2)) {
        np = 0;
        pmax = 0;
        for(i=0;i<npnts;i++) {
            if(points[i].type != 0) {
                pmax = MAX(pmax,points[i].id);
                np++;
            }
        }
        pmax++;
        nl = 0;
        imax = 0;
        for(i=0;i<nlins;i++) {
            if(lines[i].type != 0) {
                imax = MAX(imax,lines[i].id);
                nl++;
            }
        }
        imax++;
        ns = 0;
        jmax = 0;
        for(i=0;i<nlabs;i++) {
            if(labels[i].type != 0) {
                jmax = MAX(jmax,labels[i].id);
                ns++;
            }
        }
        jmax++;
        if(points) formSize += sizeof(header) + np*sizeof(struct point);
        if(lines) formSize += sizeof(header) + nl*sizeof(struct line);
        if(labels) {
            formSize += sizeof(header) + ns*sizeof(struct label);
            for(i=0;i<nlabs;i++) {
                if(labels[i].type) formSize += labels[i].nchar*sizeof(char);
            }
        }
    }
    if ((flag == 0) || (flag == 3) || (flag == 4)) {
        nn = 0;
        for(i=0;i<nnodes;i++) {
            if(nodes[i].id != 0) nn++;
        }
        if(solns) formSize += nsolns*(sizeof(header) + sizeof(struct soln) + nn*sizeof(float));
    }
    if ((flag == 0) || (flag == 4)) {
        ne = 0;
        for(i=0;i<nelems;i++) {
            if(elems[i].id == 0) continue;
            for(j=0;j<shapes[elems[i].type]->nn;j++) {
                if(nodes[nnums[elems[i].nodes[j]]].id == 0) break;
            }
            if(j == shapes[elems[i].type]->nn) ne++;
        }
        for(i=0;i<nsets;i++) {
            if(sets[i].results) { 
                formSize += sizeof(header) + sizeof(struct set);
                if(sets[i].results) formSize += ne*sizeof(struct result);
            }
        }
    }
    if ((flag == 0) || (flag == 1) || (flag == 5)) {
        if(funs) {
            formSize += sizeof(header) + nfuns*sizeof(struct fun);
            for(i=0;i<nfuns;i++) formSize += funs[i].nchar*sizeof(char);
        }
        if(aliases) {
            formSize += sizeof(header) + nalias*sizeof(struct fun);
            for(i=0;i<nalias;i++) formSize += aliases[i].nchar*sizeof(char);
        }
    }

    if((fp = fopen(fspec,"w")) == NULL) return(0);

    header.ckID = ID_FORM;
    header.ckSize = formSize;
    SafeWrite(fp,&header,sizeof(header));

    id = ID_FE2D;
    SafeWrite(fp,&id,sizeof(id));

    if (flag == 0) {
        header.ckID = ID_GLBL;
        header.ckSize = 3L*sizeof(int);
        SafeWrite(fp,&header,sizeof(header));
        SafeWrite(fp,&ptype,sizeof(int));
        SafeWrite(fp,&pkind,sizeof(int));
        SafeWrite(fp,&pndof,sizeof(int));
        header.ckID = ID_COLS;
        header.ckSize = 13*sizeof(struct color);
        SafeWrite(fp,&header,sizeof(header));
        SafeWrite(fp,colors,(size_t)header.ckSize);
        if(probs) {
            header.ckID = ID_PRBS;
            header.ckSize = nprobs*sizeof(struct problem);
            SafeWrite(fp,&header,sizeof(header));
            SafeWrite(fp,probs,(size_t)header.ckSize);
        }
    }
    if ((flag == 0) || (flag == 1)) {
        if(mats) {
            header.ckID = ID_MATS;
            header.ckSize = nmats*sizeof(struct mat);
            SafeWrite(fp,&header,sizeof(header));
            SafeWrite(fp,mats,(size_t)header.ckSize);
        }
        if(lams) {
            header.ckID = ID_LAMS;
            header.ckSize = nlams*sizeof(struct lam);
            SafeWrite(fp,&header,sizeof(header));
            SafeWrite(fp,lams,(size_t)header.ckSize);
        }
        if(nodes) {
            header.ckID = ID_NODS;
            header.ckSize = nn*sizeof(struct node);
            SafeWrite(fp,&header,sizeof(header));
            for(i=0;i<nnodes;i++) {
                if(nodes[i].id != 0) SafeWrite(fp,&nodes[i],sizeof(struct node));
            }
        }
        if(elems) {
            header.ckID = ID_ELMS;
            header.ckSize = ne*sizeof(struct elem);
            SafeWrite(fp,&header,sizeof(header));
            for(i=0;i<nelems;i++) {
                if(elems[i].id == 0) continue;
                for(j=0;j<shapes[elems[i].type]->nn;j++) {
                    if(nodes[nnums[elems[i].nodes[j]]].id == 0) break;
                }
                if(j == shapes[elems[i].type]->nn) SafeWrite(fp,&elems[i],sizeof(struct elem));
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
        if(dbcs) {
            header.ckID = ID_DBCS;
            header.ckSize = nd*sizeof(struct dbc);
            SafeWrite(fp,&header,sizeof(header));
            for(i=0;i<ndbcs;i++) {
                if(dbcs[i].type != 0) SafeWrite(fp,&dbcs[i],sizeof(struct dbc));
            }
        }
        for(i=0;i<ngrps;i++) {
            header.ckID = ID_NGRP;
            header.ckSize = sizeof(struct group) + size*sizeof(long);
            SafeWrite(fp,&header,sizeof(header));
            SafeWrite(fp,&groups[i],sizeof(struct group));
            SafeWrite(fp,groups[i].ng,(size_t)size*sizeof(float));
        }
    }
    if ((flag == 0) || (flag == 2)) {
        if(points) {
            header.ckID = ID_PNTS;
            header.ckSize = np*sizeof(struct point);
            SafeWrite(fp,&header,sizeof(header));
            for(i=0;i<npnts;i++) {
                if(points[i].type != 0) SafeWrite(fp,&points[i],sizeof(struct point));
            }
        }
        if(lines) {
            header.ckID = ID_LINS;
            header.ckSize = nl*sizeof(struct line);
            SafeWrite(fp,&header,sizeof(header));
            for(i=0;i<nlins;i++) {
                if(lines[i].type != 0) SafeWrite(fp,&lines[i],sizeof(struct line));
            }
        }
        if(labels) {
            header.ckID = ID_LABS;
            header.ckSize = ns*sizeof(struct label);
            for(i=0;i<nlabs;i++) {
                if(labels[i].type) header.ckSize += labels[i].nchar*sizeof(char);
            }
            SafeWrite(fp,&header,sizeof(header));
            for(i=0;i<nlabs;i++) {
                if(labels[i].type) {
                    SafeWrite(fp,&labels[i],sizeof(struct label));
                    SafeWrite(fp,labels[i].string,labels[i].nchar*sizeof(char));
                }
            }
        }
    }
    if ((flag == 0) || (flag == 3) || (flag == 4)) {
        for(i=0;i<nsolns;i++) {
            header.ckID = ID_SOLN;
            header.ckSize = sizeof(struct soln) + nn*sizeof(float);
            SafeWrite(fp,&header,sizeof(header));
            SafeWrite(fp,&solns[i],sizeof(struct soln));
            for(j=0;j<nnodes;j++) {
                if(nodes[j].id != 0) SafeWrite(fp,&solns[i].u[j],sizeof(float));
            }
        }
    }
    if ((flag == 0) || (flag == 4)) {
        for(i=0;i<nsets;i++) {
            header.ckID = ID_RSET;
            header.ckSize = sizeof(struct set);
            if(sets[i].results) header.ckSize += ne*sizeof(struct result);
            SafeWrite(fp,&header,sizeof(header));
            SafeWrite(fp,&sets[i],sizeof(struct set));
            if(sets[i].results) { 
                for(j=0;j<nelems;j++) {
                    if(elems[j].id != 0) 
                        SafeWrite(fp,&sets[i].results[j],sizeof(struct result));
                }
            }
        }
    }
    if ((flag == 0) || (flag == 1) || (flag == 5)) {
        if(funs) {
            header.ckID = ID_FUNS;
            header.ckSize = nfuns*sizeof(struct fun);
            for(i=0;i<nfuns;i++) header.ckSize += funs[i].nchar*sizeof(char);
            SafeWrite(fp,&header,sizeof(header));
            for(i=0;i<nfuns;i++) {
                SafeWrite(fp,&funs[i],sizeof(struct fun));
                SafeWrite(fp,funs[i].expr,funs[i].nchar*sizeof(char));
            }
        }
        if(aliases) {
            header.ckID = ID_ALAS;
            header.ckSize = nalias*sizeof(struct fun);
            for(i=0;i<nalias;i++) header.ckSize += aliases[i].nchar*sizeof(char);
            SafeWrite(fp,&header,sizeof(header));
            for(i=0;i<nalias;i++) {
                SafeWrite(fp,&aliases[i],sizeof(struct fun));
                SafeWrite(fp,aliases[i].expr,aliases[i].nchar*sizeof(char));
            }
        }
    }
    header.ckID = ID_ENDD;
    header.ckSize = 0L;
    SafeWrite(fp,&header,sizeof(header));

    fclose(fp);
    return(1);
}

void calcbw()
{
    int i,j,k,n,m,l;

    bw = 0;
    for(n=0;n<nelems;n++) {
        m = shapes[elems[n].type]->nn;
        for(i=0;i<m;i++) {
            k = nnums[elems[n].nodes[i]];
            if((k >= 0) && (k < nnodes) && (elems[n].nodes[i] < nmax))
                nodes[k].id = ABS(nodes[k].id);
            for(j=0;j<m;j++) {
                l = nnums[elems[n].nodes[j]] - k;
                l = ABS(l) + 1;
                if(l > bw) bw = l;
            }
        }
    }
}

int readdata(fspec)
char *fspec;
{
    FILE *fp;
    Chunk header;
    long id;
    int i,l,n,*glbl[10];
    size_t size;

    for(i=0;i<10;i++) glbl[i] = &n;

    for(i=0;i<nsolns;i++) free(solns[i].u);
    if(solns) { free(solns); solns = 0; }
    nsolns = 0;
    for(i=0;i<nsets;i++) {
        if(sets[i].results) { free(sets[i].results); sets[i].results = 0; }
    }
    if(sets) { free(sets); sets = 0; }
    nsets = 0;

    if((fp = fopen(fspec,"r")) == NULL) return(0);

    SafeRead(fp,&header,sizeof(header));
    if (header.ckID!=ID_FORM) { fclose(fp); return(0); }

    SafeRead(fp,&id,sizeof(id));
    if (id!=ID_FE2D) { fclose(fp); return(0); }

    for (;;) {
        SafeRead(fp,&header,sizeof(header));
        if(header.ckID == ID_ENDD) break;

        switch(header.ckID) {
            case ID_GLBL: glbl[0] = &ptype; glbl[1] = &pkind; glbl[2] = &pndof;
                          i = 0;
                          while(header.ckSize > 0) { 
                            SafeRead(fp,glbl[i++],sizeof(int));
                            header.ckSize -= sizeof(int);
                          }
                          freemem(0,-1);
                          break;

            case ID_PRBS: if(probs) { free(probs); probs = 0; nprobs = 0 ; }
                          if((probs = (struct problem *)malloc((size_t)header.ckSize)) == NULL) {
                              fclose(fp);
                              return(0);
                          }
                          SafeRead(fp,probs,(size_t)header.ckSize);
                          nprobs = header.ckSize/sizeof(struct problem);
                          break;

            case ID_MATS: if(mats) { free(mats); mats = 0; nmats = 0; }
                          if((mats = (struct mat *)malloc((size_t)header.ckSize)) == NULL) {
                              fclose(fp);
                              return(0);
                          }
                          SafeRead(fp,mats,(size_t)header.ckSize);
                          nmats = header.ckSize/sizeof(struct mat);
                          mmax = 0;
                          for(i=0;i<nmats;i++) {
                              if(mats[i].id != 0) 
                                  mmax = MAX(mmax,mats[i].id);
                          }
                          mmax++;
                          if(mnums) { free(mnums); mnums = 0; }
                          if((mnums = (int *)malloc(mmax*sizeof(int))) == NULL) {
                              fclose(fp);
                              return(0);
                          }
                          for(i=0;i<mmax;i++) mnums[i] = -1;
                          for(i=0;i<nmats;i++) {
                              if(mnums[mats[i].id] < 0) mnums[mats[i].id] = i;
                          }
                          break;

            case ID_LAMS: if(lams) { free(lams); lams = 0; nlams = 0; }
                          if((lams = (struct lam *)malloc((size_t)header.ckSize)) == NULL) {
                              fclose(fp);
                              return(0);
                          }
                          SafeRead(fp,lams,(size_t)header.ckSize);
                          nlams = header.ckSize/sizeof(struct lam);
                          lmax = 0;
                          for(i=0;i<nlams;i++) {
                              if(lams[i].id != 0) 
                                  lmax = MAX(lmax,lams[i].id);
                          }
                          lmax++;
                          if(lnums) { free(lnums); lnums = 0; }
                          if((lnums = (int *)malloc(lmax*sizeof(int))) == NULL) {
                              fclose(fp);
                              return(0);
                          }
                          for(i=0;i<lmax;i++) lnums[i] = -1;
                          for(i=0;i<nlams;i++) {
                              if(lnums[lams[i].id] < 0) lnums[lams[i].id] = i;
                          }
                          break;

            case ID_NODS: if(nodes) { free(nodes); nodes = 0; nnodes = 0; }
                          if(pbcs) { free(pbcs); pbcs = 0; npbcs = 0; }
                          if((nodes = (struct node *)malloc((size_t)header.ckSize)) == NULL) {
                              fclose(fp);
                              return(0);
                          }
                          SafeRead(fp,nodes,(size_t)header.ckSize);
                          nnodes = header.ckSize/sizeof(struct node);
                          nmax = 0;
                          for(i=0;i<nnodes;i++) {
                              if(nodes[i].id != 0) 
                                  nmax = MAX(nmax,ABS(nodes[i].id));
                          }
                          nmax++;
                          if(nnums) { free(nnums); nnums = 0; }
                          if((nnums = (int *)malloc(nmax*sizeof(int))) == NULL) {
                              fclose(fp);
                              return(0);
                          }
                          for(i=0;i<nmax;i++) nnums[i] = -1;
                          for(i=0;i<nnodes;i++) {
                              n = ABS(nodes[i].id);
                              nnums[n] = i;
                              nodes[i].id = -n;
                          }
                          break;

            case ID_ELMS: if(elems) { free(elems); elems = 0; nelems = 0; }
                          if(dbcs) { free(dbcs); dbcs = 0; ndbcs = 0; }
                          if((elems = (struct elem *)malloc((size_t)header.ckSize)) == NULL) {
                              fclose(fp);
                              return(0);
                          }
                          SafeRead(fp,elems,(size_t)header.ckSize);
                          nelems = header.ckSize/sizeof(struct elem);
                          emax = 0;
                          for(i=0;i<nelems;i++) {
                              if(elems[i].id != 0) 
                                  emax = MAX(emax,ABS(elems[i].id));
                          }
                          emax++;
                          if(enums) { free(enums); enums = 0; }
                          if((enums = (int *)malloc(emax*sizeof(int))) == NULL) {
                              fclose(fp);
                              return(0);
                          }
                          for(i=0;i<emax;i++) enums[i] = -1;
                          for(i=0;i<nelems;i++) enums[ABS(elems[i].id)] = i;
                          break;

            case ID_PBCS: if(pbcs) { free(pbcs); pbcs = 0; npbcs = 0 ; }
                          if((pbcs = (struct pbc *)malloc((size_t)header.ckSize)) == NULL) {
                              fclose(fp);
                              return(0);
                          }
                          SafeRead(fp,pbcs,(size_t)header.ckSize);
                          npbcs = header.ckSize/sizeof(struct pbc);
                          for(i=0;i<npbcs;i++) {
                              if(pbcs[i].type == 3) {
                                  l = nnums[pbcs[i].jd] - nnums[pbcs[i].id];
                                  l = ABS(l) + 1;
                                  if (l > bw) bw = l;
                              }
                          }
                          break;

            case ID_DBCS: if(dbcs) { free(dbcs); dbcs = 0; ndbcs = 0; }
                          if((dbcs = (struct dbc *)malloc((size_t)header.ckSize)) == NULL) {
                              fclose(fp);
                              return(0);
                          }
                          SafeRead(fp,dbcs,(size_t)header.ckSize);
                          ndbcs = header.ckSize/sizeof(struct dbc);
                          break;

            case ID_NGRP: n = ngrps++;
                          groups = (struct group *)realloc(groups,ngrps*sizeof(struct group));
                          if(groups == NULL) { 
                              fclose(fp);
                              return(0); 
                          }
                          SafeRead(fp,&groups[n],sizeof(struct group));
                          size = (size_t)header.ckSize-sizeof(struct group);
                          if((groups[n].ng = (long *)malloc(size)) == NULL) {
                              fclose(fp);
                              return(0);
                          }
                          SafeRead(fp,groups[n].ng,size);
                          break;

            case ID_SOLN: n = nsolns++;
                          solns = (struct soln *)realloc(solns,nsolns*sizeof(struct soln));
                          if(solns == NULL) { 
                              fclose(fp);
                              return(0); 
                          }
                          SafeRead(fp,&solns[n],sizeof(struct soln));
                          size = (size_t)header.ckSize-sizeof(struct soln);
                          if((solns[n].u = (float *)malloc(size)) == NULL) {
                              fclose(fp);
                              return(0);
                          }
                          SafeRead(fp,solns[n].u,size);
                          break;

            case ID_RSET: n = nsets++;
                          sets = (struct set *)realloc(sets,nsets*sizeof(struct set));
                          if(sets == NULL) { 
                              fclose(fp);
                              return(0); 
                          }
                          SafeRead(fp,&sets[n],sizeof(struct set));
                          size = (size_t)header.ckSize-sizeof(struct set);
                          if(size == 0) break;
                          if((sets[n].results = (struct result *)malloc(size)) == NULL) {
                              fclose(fp);
                              return(0);
                          }
                          SafeRead(fp,sets[n].results,size);
                          break;

            case ID_FUNS: for(i=0;i<nfuns;i++) free(funs[i].expr);
                          if(funs) { free(funs); funs = 0; }
                          nfuns = 0;
                          size = (size_t)header.ckSize;
                          while(size > 0) {
                              i = nfuns++;
                              funs = (struct fun *)realloc(funs,nfuns*sizeof(struct fun));
                              if(funs == NULL) { 
                                  fclose(fp);
                                  return(0);
                              }
                              SafeRead(fp,&funs[i],sizeof(struct fun));
                              size -= sizeof(struct fun);
                              funs[i].expr = (char *)malloc(funs[i].nchar*sizeof(char));
                              if(funs[i].expr == NULL) { 
                                  fclose(fp);
                                  return(0);
                              }
                              SafeRead(fp,funs[i].expr,funs[i].nchar*sizeof(char));
                              size -= funs[i].nchar*sizeof(char);
                          }
                          break;

            case ID_ALAS: for(i=0;i<nalias;i++) free(aliases[i].expr);
                          if(aliases) { free(aliases); aliases = 0; }
                          nalias = 0;
                          size = (size_t)header.ckSize;
                          while(size > 0) {
                              i = nalias++;
                              aliases = (struct fun *)realloc(aliases,nalias*sizeof(struct fun));
                              if(aliases == NULL) { 
                                  fclose(fp);
                                  return(0);
                              }
                              SafeRead(fp,&aliases[i],sizeof(struct fun));
                              size -= sizeof(struct fun);
                              aliases[i].expr = (char *)malloc(aliases[i].nchar*sizeof(char));
                              if(aliases[i].expr == NULL) { 
                                  fclose(fp);
                                  return(0);
                              }
                              SafeRead(fp,aliases[i].expr,aliases[i].nchar*sizeof(char));
                              size -= aliases[i].nchar*sizeof(char);
                          }
                          break;

            case ID_COLS: SafeRead(fp,colors,(size_t)header.ckSize);
                          break;

            case ID_PNTS: if(points) { free(points); points = 0; npnts = 0; }
                          if((points = (struct point *)malloc((size_t)header.ckSize)) == NULL) {
                              fclose(fp);
                              return(0);
                          }
                          SafeRead(fp,points,(size_t)header.ckSize);
                          npnts = header.ckSize/sizeof(struct point);
                          pmax = 0;
                          for(i=0;i<npnts;i++) {
                              if(points[i].id != 0) 
                                  pmax = MAX(pmax,points[i].id);
                          }
                          pmax++;
                          if(pnums) { free(pnums); pnums = 0; }
                          if((pnums = (int *)malloc(pmax*sizeof(int))) == NULL) {
                              fclose(fp);
                              return(0);
                          }
                          for(i=0;i<pmax;i++) pnums[i] = -1;
                          for(i=0;i<npnts;i++) pnums[points[i].id] = i;
                          break;

            case ID_LINS: if(lines) { free(lines); lines = 0; nlins = 0; }
                          if((lines = (struct line *)malloc((size_t)header.ckSize)) == NULL) {
                              fclose(fp);
                              return(0);
                          }
                          SafeRead(fp,lines,(size_t)header.ckSize);
                          nlins = header.ckSize/sizeof(struct line);
                          imax = 0;
                          for(i=0;i<nlins;i++) {
                              if(lines[i].id != 0) 
                                  imax = MAX(imax,lines[i].id);
                          }
                          imax++;
                          if(inums) { free(inums); inums = 0; }
                          if((inums = (int *)malloc(imax*sizeof(int))) == NULL) {
                              fclose(fp);
                              return(0);
                          }
                          for(i=0;i<imax;i++) inums[i] = -1;
                          for(i=0;i<nlins;i++) inums[lines[i].id] = i;
                          break;

            case ID_LABS: for(i=0;i<nlabs;i++) free(labels[i].string);
                          if(labels) { free(labels); labels = 0; }
                          nlabs = 0;
                          size = (size_t)header.ckSize;
                          while(size > 0) {
                              i = nlabs++;
                              labels = (struct label *)realloc(labels,nlabs*sizeof(struct label));
                              if(labels == NULL) { 
                                  fclose(fp);
                                  return(0);
                              }
                              SafeRead(fp,&labels[i],sizeof(struct label));
                              size -= sizeof(struct label);
                              labels[i].string = (char *)malloc(labels[i].nchar*sizeof(char));
                              if(labels[i].string == NULL) { 
                                  fclose(fp);
                                  return(0);
                              }
                              SafeRead(fp,labels[i].string,labels[i].nchar*sizeof(char));
                              size -= labels[i].nchar*sizeof(char);
                          }
                          jmax = 0;
                          for(i=0;i<nlabs;i++) {
                              if(labels[i].id != 0) 
                                  jmax = MAX(jmax,labels[i].id);
                          }
                          jmax++;
                          if(jnums) { free(jnums); jnums = 0; }
                          if((jnums = (int *)malloc(jmax*sizeof(int))) == NULL) {
                              fclose(fp);
                              return(0);
                          }
                          for(i=0;i<jmax;i++) jnums[i] = -1;
                          for(i=0;i<nlabs;i++) jnums[labels[i].id] = i;
                          break;

            default:      fseek(fp,(long)header.ckSize,SEEK_CUR);
        }
    }
    fclose(fp);
    calcbw();
    return(1);
}
