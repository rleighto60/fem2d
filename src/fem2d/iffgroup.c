/*
$Id: iffgroup.c 1.2 1994/09/14 17:42:03 leighton Exp leighton $
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

program     -   iffgroup.c

description -   Routines to support group file IO.

$Log: iffgroup.c $
 * Revision 1.2  1994/09/14  17:42:03  leighton
 * made source code ANSI compliant
 *
 * Revision 1.1  1994/09/12  18:32:14  leighton
 * Initial revision
 *
*/

#include "header.h"
#include "iff.h"

int savegroup(int n,
              char *fspec)
{
    FILE *fp;
    Chunk header;
    long id;
    long formSize;
    int i,j,k,bc,nn,ne,nmin,emin;
   
    nn = 0;
    nmin = nmax;
    for (j=0;j<nnodes;j++) {
        i = ABS(nodes[j].id);
        if (i >= groups[n].nmax) continue;
        if (groups[n].ng[i>>5] & 1L<<(i%32)) {
            nn++;
            nmin = MIN(nmin,i);
        }
    }
    ne = 0;
    emin = emax;
    for (k=0;k<nelems;k++) {
        if (elems[k].id == 0) continue;
        for (j=0;j<shapes[elems[k].type]->nn;j++) {
            i = elems[k].nodes[j];
            if (i >= groups[n].nmax) break;
            if (!(groups[n].ng[i>>5] & 1L<<(i%32))) break;
        }
        if (j == shapes[elems[k].type]->nn) {
            ne++;
            emin = MIN(emin,ABS(elems[k].id));
        }
    }

    if ((fp = fopen(fspec,"w")) == NULL) return(0);

    formSize = sizeof(id) + sizeof(header);
    if (nodes) formSize += sizeof(header) + nn*sizeof(struct node);
    if (elems) formSize += sizeof(header) + ne*sizeof(struct elem);

    header.ckID = ID_FORM;
    header.ckSize = formSize;
    SafeWrite(fp,&header,sizeof(header));

    id = ID_FE2D;
    SafeWrite(fp,&id,sizeof(id));

    bc = -1;
    if (nodes) {
        header.ckID = ID_NODS;
        header.ckSize = nn*sizeof(struct node);
        SafeWrite(fp,&header,sizeof(header));
        for (j=0;j<nnodes;j++) {
            i = ABS(nodes[j].id);
            if (i >= groups[n].nmax) continue;
            if (groups[n].ng[i>>5] & 1L<<(i%32)) {
                id = ABS(nodes[j].id) - nmin + 1;
                SafeWrite(fp,&id,sizeof(int));
                SafeWrite(fp,&bc,sizeof(int));
                SafeWrite(fp,&nodes[j].x,sizeof(float));
                SafeWrite(fp,&nodes[j].y,sizeof(float));
            }
        }
    }
    if (elems) {
        header.ckID = ID_ELMS;
        header.ckSize = ne*sizeof(struct elem);
        SafeWrite(fp,&header,sizeof(header));
        for (k=0;k<nelems;k++) {
            if (elems[k].id == 0) continue;
            for (j=0;j<shapes[elems[k].type]->nn;j++) {
                i = elems[k].nodes[j];
                if (i >= groups[n].nmax) break;
                if (!(groups[n].ng[i>>5] & 1L<<(i%32))) break;
            }
            if (j == shapes[elems[k].type]->nn) {
                id = ABS(elems[k].id) - emin + 1;
                SafeWrite(fp,&id,sizeof(int));
                SafeWrite(fp,&bc,sizeof(int));
                SafeWrite(fp,&elems[k].type,sizeof(int));
                SafeWrite(fp,&elems[k].mat,sizeof(int));
                for (i=0;i<9;i++) {
                    if (i<shapes[elems[k].type]->nn) id = elems[k].nodes[i] - nmin + 1;
                    else id = 0;
                    SafeWrite(fp,&id,sizeof(int));
                }
            }
        }
    }

    header.ckID = ID_ENDD;
    header.ckSize = 0L;
    SafeWrite(fp,&header,sizeof(header));

    fclose(fp);
    return(1);
}

int readgroup(int n,
              char *fspec)
{
    extern void calcbw();
    FILE *fp;
    Chunk header;
    long id;
    int i,j,k,l,m,nn,ne,nmin,emin;

    if ((fp = fopen(fspec,"r")) == NULL) return(0);

    SafeRead(fp,&header,(long)sizeof(header));
    if (header.ckID!=ID_FORM) { fclose(fp); return(0); }

    SafeRead(fp,&id,(long)sizeof(id));
    if (id!=ID_FE2D) { fclose(fp); return(0); }

    nmin = nmax - 1;
    emin = emax - 1;
    for (;;) {
        SafeRead(fp,&header,sizeof(header));
        if (header.ckID == ID_ENDD) break;

        switch (header.ckID) {
            case ID_NODS: nn = header.ckSize/sizeof(struct node);
                          for (j=0;j<nn;j++) {
                            i = nnodes++;
                            nodes = (struct node *)realloc(nodes,nnodes*sizeof(struct node));
                            if (nodes == NULL) { 
                                fclose(fp);
                                return(0); 
                            }
                            nodes[i].bc = -1;
                            SafeRead(fp,&nodes[i],sizeof(struct node));
                            id = ABS(nodes[i].id) + nmin;
                            if (id+1 > nmax) {
                                nnums = (int *)realloc(nnums,(id+1)*sizeof(int));
                                if (nnums == NULL) { 
                                    fclose(fp);
                                    return(0); 
                                }
                                for (k=nmax;k<=id;k++) nnums[k] = -1;
                                nmax = id+1;
                            }
                            nnums[id] = i;
                            nodes[i].id = -id;
                          }
                          if (n == ngrps) {
                            ngrps++;
                            groups = (struct group *)realloc(groups,ngrps*sizeof(struct group));
                            if (groups == NULL) { 
                                fclose(fp);
                                return(0); 
                            }
                            groups[n].nmax = 0;
                            groups[n].ng = 0;
                          }
                          if (groups[n].nmax < nmax) {
                            m = (nmax + 31)>>5;
                            groups[n].ng = (long *)realloc(groups[n].ng,m*sizeof(long));
                            if (groups[n].ng == NULL) { 
                                fclose(fp);
                                return(0); 
                            }
                            l = (groups[n].nmax + 31)>>5;
                            for (k=l;k<m;k++) groups[n].ng[k] = 0.0;
                            groups[n].nmax = nmax;
                          }
                          for (i=nmin+1;i<nmax;i++) {
                            if (nnums[i] >= 0) groups[n].ng[i>>5] |= 1L<<(i%32);
                          }
                          break;

            case ID_ELMS: ne = header.ckSize/sizeof(struct elem);
                          for (j=0;j<ne;j++) {
                            i = nelems++;
                            elems = (struct elem *)realloc(elems,nelems*sizeof(struct elem));
                            if (elems == NULL) { 
                                fclose(fp);
                                return(0); 
                            }
                            elems[i].bc = -1;
                            SafeRead(fp,&elems[i],sizeof(struct elem));
                            id = ABS(elems[i].id) + emin;
                            if (id+1 > emax) {
                                enums = (int *)realloc(enums,(id+1)*sizeof(int));
                                if (enums == NULL) { 
                                    fclose(fp);
                                    return(0); 
                                }
                                for (k=emax;k<=id;k++) enums[k] = -1;
                                emax = id+1;
                            }
                            enums[id] = i;
                            elems[i].id = id;
                            for (k=0;k<shapes[elems[i].type]->nn;k++) {
                                elems[i].nodes[k] += nmin;
                                l = nnums[elems[i].nodes[k]];
                                if ((l >= 0) && (l < nnodes) && (elems[i].nodes[k] < nmax))
                                    nodes[l].id = ABS(nodes[l].id);
                            }
                          }
                          calcbw();
                          break;

            default:      fseek(fp,(long)header.ckSize,SEEK_CUR);
        }
    }
    fclose(fp);
    return(1);
}
