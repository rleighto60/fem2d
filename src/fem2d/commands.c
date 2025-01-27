/*
$Id: command.c 1.1 1995/01/23 17:40:26 leighton Exp leighton $
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

program     -   command.c

description -   command functions

$Log: command.c $
 * Revision 1.1  1994/01/23  18:20:39  leighton
 * Initial revision
 *
*/

#define COMMANDS

#include "header.h"
#include "commands.h"

void _com_err(int argn, char *argv[])
{
    error("main - unknown command");
}
void _com_plane(int argn, char *argv[])
{
    ptype = 0;
}
void _com_axisym(int argn, char *argv[])
{
    ptype = 1;
}
/* problem(kind,ndof,title)                       */
void _com_problem(int argn, char *argv[])
{
    int i,n;

    n = eval_args(1, MIN(argn,3), argv);
    pkind = 0;
    if (n > 1) pkind = (int)vals[1];
    if (pkind < 0) pkind = 0;

    if (pkind >= nprobs) {
        probs = (struct problem *)realloc(probs,(pkind+1)*sizeof(struct problem));
        if (probs == NULL) { 
            error("main - could not allocate memory for probs");
            return; 
        }
        for(i=nprobs;i<=pkind;i++) {
            probs[i].ndof = 0;
            probs[i].title[0] = 0;
        }
        if (pkind%2) probs[pkind].ndof = 1;
        else probs[pkind].ndof = 2;
        nprobs = pkind+1;
    }
    if (n > 2) probs[pkind].ndof = (int)vals[2];
    pndof = probs[pkind].ndof;
    if (pndof < 1) pndof = 1;
    if (pndof > 2) pndof = 2;
    if (argn > 3) {
        strncpy(probs[pkind].title,argv[3],39);
        probs[pkind].title[39] = 0;
    }
}
/* super(kind,p1,p2,title)                       */
void _com_super(int argn, char *argv[])
{
    int i,j,k,n,ni,nt,nn,n1,n2,p1,p2;
    float val,vmin,vmax;

    n = eval_args(1, MIN(argn,4), argv);
    if (n < 4) {
        error("super - missing arguments");
        return;
    }
    pkind = (int)vals[1];
    p1 = (int)vals[2];
    p2 = (int)vals[3];
    if ((p1 < 0) || (p1 >= nprobs) || (p2 < 0) || (p2 >= nprobs)) {
        error("super - bad problem kind");
        return;
    }
    if (probs[p1].ndof != probs[p2].ndof) {
        error("super - problem ndofs do not match");
        return;
    }
    if (pkind < 0) pkind = nprobs;

    if (pkind >= nprobs) {
        probs = (struct problem *)realloc(probs,(pkind+1)*sizeof(struct problem));
        if (probs == NULL) { 
            error("super - could not allocate memory for probs");
            return; 
        }
        for(i=nprobs;i<=pkind;i++) {
            probs[i].ndof = 0;
            probs[i].title[0] = 0;
        }
        nprobs = pkind+1;
    }
    pndof = probs[pkind].ndof = probs[p1].ndof;

    for (i=0;i<pndof;i++) {
        n = n1 = n2 = -1;
        for (j=0;j<nsolns;j++) {
            if (solns[j].dof == i+1) {
                if (solns[j].kind == pkind) n = j;
                if (solns[j].kind == p1) n1 = j;
                if (solns[j].kind == p2) n2 = j;
            }
        }
        if ((n1 < 0) || (n2 < 0)) continue;
        if(n < 0) {
            n = nsolns++;
            solns = (struct soln *)realloc(solns,nsolns*sizeof(struct soln));
            if(solns == NULL) { 
                error("super - could not allocate memory for solns");
                break; 
            }
            if((solns[n].u = (float *)malloc((size_t)nnodes*sizeof(float))) == NULL) {
                error("super - could not allocate memory for soln");
                break; 
            }
        }
        for(j=0;j<nnodes;j++) solns[n].u[j] = solns[n1].u[j] + solns[n2].u[j];
        solns[n].type = 0;
        solns[n].kind = pkind;
        solns[n].dof = i+1;
        solns[n].step = 0;
    }

    for (i=0;i<nsets;i++) {
        if (sets[i].kind != p1) continue;
        n1 = i; n = n2 = -1;
        for (j=0;j<nsets;j++) {
            if (sets[j].func == sets[i].func) {
                if (sets[j].kind == pkind) n = j;
                if (sets[j].kind == p2) n2 = j;
            }
        }
        if (n2 < 0) continue;
        if (sets[n1].flag != sets[n2].flag) continue;
        if(n < 0) {
            n = nsets++;
            sets = (struct set *)realloc(sets,nsets*sizeof(struct set));
            if(sets == NULL) { 
                error("super - could not allocate memory for sets");
                break; 
            }
            sets[n].kind = pkind;
            sets[n].ndof = pndof;
            sets[n].func = sets[n1].func;
            sets[n].results = 0;
        }
        sets[n].flag = sets[n1].flag;
        if(sets[n].results) free(sets[n].results);
        sets[n].results = (struct result *)malloc(nelems*sizeof(struct result));
        if(sets[n].results == NULL) {
            error("super - could not allocate memory for results");
            break; 
        }

        vmin = 1.e17;
        vmax = -1.e17;

        for(j=0;j<nelems;j++) {
            if(elems[j].id == 0) continue;
            if(elems[j].id < 0) {
                sets[n].results[j].val[0] = 1.e17;
                continue;
            }
            nt = elems[j].type;
            nn = shapes[nt]->nn;
            if(sets[n].flag != 0) ni = nn;
            else ni = shapes[nt]->eintg->ni;
            for(k=0;k<ni;k++) {
                val = sets[n].results[j].val[k] = 
                    sets[n1].results[j].val[k] + sets[n2].results[j].val[k];
                vmin = MIN(vmin,val);
                vmax = MAX(vmax,val);
            }
        }
        sets[n].min = vmin;
        sets[n].max = vmax;
    }
    if (argn > 4) {
        strncpy(probs[pkind].title,argv[4],39);
        probs[pkind].title[39] = 0;
    }
}
/* mat(id,type,C[10],ckind1,f1[2],ref1,ckind2,f2[2],ref2) */
/* type = 0, global; 1, local */
void _com_mat(int argn, char *argv[])
{
    int i,j,m,id;

    argn = eval_args(1, argn, argv);
    id = mmax;
    if (argn > 1) id = (int)vals[1];
    m = 0;
    if (argn > 2) m = (int)vals[2];
    if (id < 1) id = mmax;
    if ((i = newmat(id,pkind)) < 0) return;
    mats[i].type = m;
    for (j=0;j<10;j++) {
        if (argn > j+3) mats[i].C[j] = vals[j+3];
    }
    if (argn > 13) mats[i].ckind[0] = (int)vals[13];
    if (argn > 14) mats[i].f[0] = vals[14];
    if (argn > 15) mats[i].f[1] = vals[15];
    if (argn > 16) mats[i].ref[0] = vals[16];
    if (argn > 17) mats[i].ckind[1] = (int)vals[17];
    if (argn > 18) mats[i].f[2] = vals[18];
    if (argn > 19) mats[i].f[3] = vals[19];
    if (argn > 20) mats[i].ref[1] = vals[20];
}
/* lam(id,mid,frac,mang) */
void _com_lam(int argn, char *argv[])
{
    int i,j,k,l,m,id;

    id = lmax;
    if (argn > 1) {
        if (eval_arg(argv[1],&vals[1])) id = (int)vals[1];
    }
    if (id < 1) id = lmax;
    if (id+1 > lmax) {
        if ((lnums = newobject( id, lmax, lnums )) != NULL) lmax = id+1;
        else return; 
    }
    i = j = lnums[id];
    if (argn > 2) {
        while (i >= 0) {
            j = i;
            i = lams[j].next;
        }
        if (j >= 0) i = j;
        else i = nlams;
        while ((i < nlams) && (lams[i].id != 0)) i++;
        if (i >= nlams) {
            i = nlams++;
            lams = (struct lam *)realloc(lams,nlams*sizeof(struct lam));
            if (lams == NULL) { 
                error("main - could not allocate memory for lams");
                return; 
            }
            if (lnums[id] < 0) lnums[id] = i;
        }
        if (j >= 0) lams[j].next = i;
        lams[i].id = id;
        lams[i].next = -1;
        m = 0;
        if (argn > 2) {
            if (eval_arg(argv[2],&vals[2])) m = (int)vals[2];
        }
        if ((m <= 0) || (m >= mmax)) l = -1;
        else {
            k = l = mnums[m];
            while (k >= 0) {
                l = k;
                if (mats[k].kind == pkind) break;
                else k = mats[l].next;
            }
        }
        if (l < 0) {
            error("main - missing material for lam");
            return; 
        }
        lams[i].mat = l;
        lams[i].frac = 1.0;
        if (argn > 3) {
            if (eval_arg(argv[3],&vals[3])) lams[i].frac = (float)vals[3];
        }
        lams[i].mang = 0.0;
        lams[i].func = -1;
        if (argn > 4) {
            if ((m = getfun(argv[4])) < nfuns) lams[i].func = m;
            else {
                if (eval_arg(argv[4],&vals[4])) lams[i].mang = (float)vals[4];
            }
        }
        if ((j = newmat(lams[i].id,pkind)) < 0) return;
        mats[j].type = mats[l].type;
        mats[j].ckind[0] = mats[l].ckind[0];
        mats[j].ref[0] = mats[l].ref[0];
        mats[j].ckind[1] = mats[l].ckind[1];
        mats[j].ref[1] = mats[l].ref[1];
    }
    else {
        while (i >= 0) {
            j = i;
            i = lams[j].next;
            lams[j].id = 0;
        }
    }
}
/* node(id,x,y)                             */
void _com_node(int argn, char *argv[])
{
    int i,n,id;
    float xi,yi,x,y;

    argn = eval_args(1, argn, argv);
    id = nmax;
    if (argn > 1) id = (int)vals[1];
    if (id < 1) id = nmax;
    if (id < nmax) {
        n = nnums[id];
        if ((n >= 0) && (nodes[n].id != 0)) {
            if (argn < 3) {
                nodes[n].id = 0;
                if ((i = nodes[n].bc) >= 0) {
                    do {
                        pbcs[i].type = 0;
                    } while ((i = pbcs[i].next) >= 0);
                }
                return;
            }
        }
    }
    x = y = xi = yi = 0.0;
    if (argn > 2) x = vals[2];
    if (argn > 3) y = vals[3];
    if ((n = newnode(id)) < 0) return;
    toglobal(x,y,&(nodes[n].x),&(nodes[n].y));
    if (solns) freemem(1,-1);
}
/* elem(id,mat,nn,ni,nodes[9])         */
void _com_elem(int argn, char *argv[])
{
    int i,j,k,l,m,n,id,nid[9];

    argn = eval_args(1, argn, argv);
    id = emax;
    if (argn > 1) id = (int)vals[1];
    if (id < 1) id = emax;
    if (id < emax) {
        n = enums[id];
        if ((n >= 0) && (elems[n].id != 0)) {
            if (argn < 4) {
                if (argn < 3) {
                    elems[n].id = 0;
                    if ((i = elems[n].bc) >= 0) {
                        do {
                            dbcs[i].type = 0;
                        } while ((i = dbcs[i].next) >= 0);
                    }
                }
                else elems[n].mat = (int)vals[2];
                return;
            }
        }
    }
    i = m = 0;
    if (argn > 3) m = (int)vals[3];
    if (argn > 4) i = (int)vals[4];
    for (l=0;l<11;l++) {
        if ((m == shapes[l]->nn) && (i == shapes[l]->cintg->ni)) break;
    }
    if (l > 10) {
        error("main - bad element specification");
        return; 
    }
    for (i=0;i<m;i++) {
        j = i;
        if (argn > i+5) j = (int)vals[i+5];
        nid[i] = ABS(j);
    }
    if (i < m) return; 
    if ((n = newelem(id)) < 0) return;
    elems[n].mat = 0;
    if (argn > 2) elems[n].mat = (int)vals[2];
    elems[n].type = l;
    for (i=0;i<m;i++) {
        elems[n].nodes[i] = nid[i];
        k = nnums[nid[i]];
        if ((k >= 0) && (k < nnodes) && (nid[i] < nmax))
            nodes[k].id = ABS(nodes[k].id);
    }
    for (i=0;i<m;i++) {
        k = nnums[elems[n].nodes[i]];
        for (j=0;j<m;j++) {
            l = nnums[elems[n].nodes[j]] - k;
            l = ABS(l) + 1;
            if (l > bw) bw = l;
        }
    }
}
/* pbc(node,type,dof,val)                                  */
/*      type = 0 - null, 1 - dirchlet, 2 - flux, 3 - constraint */
/*      dof  = 0 - scalar, 1 - x comp, 2 - y comp               */
void _com_pbc(int argn, char *argv[])
{
    int i,j,l,n,id;

    argn = eval_args(1, argn, argv);
    id = 0;
    if (argn > 1) id = (int)vals[1];
    if (id < 1) { 
        error("main - bad node id for pbc");
        return; 
    }
    if ((nnums[id] < 0) || (id >= nmax)) { 
        error("main - node id undefined for pbc");
        return; 
    }
    n = 0;
    if (argn > 3) n = (int)vals[3];
    if (n > pndof) { 
        error("main - bad dof specification for pbc");
        return; 
    }
    i = j = nodes[nnums[id]].bc;
    while (i >= 0) {
        j = i;
        if ((pbcs[i].kind == pkind) && (pbcs[i].dof == n)) break;
        else i = pbcs[j].next;
    }
    if (i < 0) {
        i = npbcs++;
        pbcs = (struct pbc *)realloc(pbcs,npbcs*sizeof(struct pbc));
        if (pbcs == NULL) { 
            error("main - could not allocate memory for pbcs");
            return; 
        }
        if (nodes[nnums[id]].bc < 0) nodes[nnums[id]].bc = i;
        if (j >= 0) pbcs[j].next = i;
        pbcs[i].next = -1;
    }
    pbcs[i].id = pbcs[i].jd = id;
    pbcs[i].type = 0;
    pbcs[i].kind = pkind;
    pbcs[i].dof = n;
    pbcs[i].val = 0.0;
    if (argn > 4) {
        pbcs[i].type = (int)vals[2];
        if(pbcs[i].type == 3) {
            pbcs[i].jd = (int)vals[4];
            l = nnums[pbcs[i].jd] - nnums[pbcs[i].id];
            l = ABS(l) + 1;
            if (l > bw) bw = l;
        }
        else pbcs[i].val = vals[4];
    }
}
/* dbc(elem,type,dof,side,val[3])                  */
/*      type = 0 - null, 1 - dirchlet, 2 - flux         */
/*      dof  = 0 - scalar, 1 - norm comp, 2 - tang comp */
void _com_dbc(int argn, char *argv[])
{
    int i,j,m,n,id,is;
    float p[4];

    argn = eval_args(1, argn, argv);
    id = 0; i = 1;
    if (argn > 1) id = (int)vals[1];
    if (id < 1) { 
        error("main - bad element id for dbc");
        return; 
    }
    if ((enums[id] < 0) || (id >= emax)) { 
        error("main - element id undefined for dbc");
        return; 
    }
    n = 0;
    if (argn > 3) n = (int)vals[3];
    if (n > pndof) { 
        error("main - bad dof specification for dbc");
        return; 
    }
    is = 0;
    if (argn > 4) is = (int)vals[4];
    if ((is > shapes[elems[enums[id]].type]->ns) || (is <= 0)) is = i;
    i = j = elems[enums[id]].bc;
    while (i >= 0) {
        j = i;
        if ((dbcs[i].kind == pkind) && (dbcs[i].dof == n) && (dbcs[i].side == is)) 
            break;
        else i = dbcs[j].next;
    }
    if (i < 0) {
        i = ndbcs++;
        dbcs = (struct dbc *)realloc(dbcs,ndbcs*sizeof(struct dbc));
        if (dbcs == NULL) { 
            error("main - could not allocate memory for dbcs");
            return; 
        }
        if (elems[enums[id]].bc < 0) elems[enums[id]].bc = i;
        if (j >= 0) dbcs[j].next = i;
        dbcs[i].next = -1;
    }
    dbcs[i].id = dbcs[i].jd = id;
    dbcs[i].type = 0;
    dbcs[i].kind = pkind;
    dbcs[i].dof = n;
    dbcs[i].side = is;
    if (argn > 5) {
        dbcs[i].type = (int)vals[2];
        m = shapes[elems[enums[id]].type]->no;
        p[0] = p[m] = vals[5];
        if (argn > 6) p[m] = vals[6];
        for (j=1;j<m;j++) {
            p[j] = (float)j*(p[0] + p[m])/(float)m;
            if (argn > j+6) p[j] = vals[j+6];
        }
        for (j=0;j<=m;j++) dbcs[i].val[j] = p[j];
    }
}
/* check */
void _com_check(int argn, char *argv[])
{
    sprintf(field,"%d materials\n",nmats);
    sprintf(field,"%s%d nodes\n",field,nnodes);
    sprintf(field,"%s%d elements\n",field,nelems);
    sprintf(field,"%s%d pbcs\n",field,npbcs);
    sprintf(field,"%s%d dbcs\n",field,ndbcs);
    sprintf(field,"%s\n half bandwidth = %d",field,pndof*bw);
    check();
}
/* eval(func,elem,n,xi,yi) */
void _com_eval(int argn, char *argv[])
{
    int j,n;
    float xi,yi,f;

    if (argn < 2) return;
    argn = eval_args(2, argn, argv);
    n = -1;
    j = 0;
    xi = yi = 0.0;
    if (argn > 2) {
        n = (int)vals[2];
        if (n < 0) {
            n = -n;
            if (n < emax) n = enums[n];
            else return;
        }
        if (argn > 3) j = (int)vals[3];
        if ((n >= nelems) || (n < 0)) {
            error("main - operation aborted");
            return; 
        }
        if (argn > 4) xi = vals[4];
        if (argn > 5) yi = vals[5];
    }
    if (eval(argv[1],n,j,xi,yi,&f)) sprintf(field,"%12.4e",f);
}
/* select(func,flag) */
void _com_select(int argn, char *argv[])
{
    int i,m,n;

    if (argn < 2) return;
    if ((m = getfun(argv[1])) < nfuns) {
        for (i=0;i<nsets;i++) {
            if ((sets[i].func == m) && (sets[i].kind == pkind)) break;
        }
        if (i == nsets) {
            n = nsets++;
            sets = (struct set *)realloc(sets,nsets*sizeof(struct set));
            if (sets == NULL) { 
                error("main - could not allocate memory for sets");
                return; 
            }
            sets[n].results = 0;
        }
        if (sets[i].results) freemem(2,i);
        sets[i].kind = pkind;
        sets[i].ndof = pndof;
        sets[i].flag = 0;
        sets[i].func = m;
        if (argn > 2) {
            stolower(argv[2]);
            if (!strncmp(argv[2],"n",1)) sets[i].flag = 1;
            if (!strncmp(argv[2],"a",1)) sets[i].flag = 2;
        }
        sets[i].min = sets[i].max = 0.0;
    }
    else error("main - function not defined");
}
/* save(file) */
void _com_save(int argn, char *argv[])
{
    int i;

    if (argn > 1) {
        strcpy(filename,argv[1]);
    }
    i = 0;
    if (argn > 2) {
        stolower(argv[2]);
        if (!strncmp(argv[2],"all",3)) i = 0;
        if (!strncmp(argv[2],"m",1)) i = 1;
        if (!strncmp(argv[2],"d",1)) i = 2;
        if (!strncmp(argv[2],"s",1)) i = 3;
        if (!strncmp(argv[2],"r",1)) i = 4;
        if (!strncmp(argv[2],"f",1)) i = 5;
        if (!strncmp(argv[2],"ali",3)) i = 5;
    }
    if (filename[0] != 0) {
        if (!savedata(i,filename)) {
            error("main - error saving file");
            return;
        }
    }
}
/* read(file) */
void _com_read(int argn, char *argv[])
{
    if (argn > 1) {
        strcpy(filename,argv[1]);
    }
    if (filename[0] != 0) {
        if (!readdata(filename)) { 
            error("main - error reading file");
            return; 
        }
    }
}
/* reset(flag) */
void _com_reset(int argn, char *argv[])
{
    int i = -1, m;

    if (argn > 2) {
        if ((m = getfun(argv[2])) < nfuns) {
            for (i=0;i<nsets;i++) {
                if ((sets[i].func == m) && (sets[i].kind == pkind)) break;
            }
        }
    }
    if (argn > 1) {
        stolower(argv[1]);
        if (!strncmp(argv[1],"all",3)) freemem(0,-1);
        if (!strncmp(argv[1],"s",1)) freemem(1,-1);
        if (!strncmp(argv[1],"r",1)) freemem(2,i);
        if (!strncmp(argv[1],"g",1)) freemem(3,0);
        if (!strncmp(argv[1],"f",1)) freemem(4,0);
        if (!strncmp(argv[1],"ali",3)) freemem(5,0);
        if (!strncmp(argv[1],"d",1)) freemem(6,0);
    }
    else {
        if (hfp) {
            showpage(hfp);
            writeclose(hfp);
            fclose(hfp);
            hfp = NULL;
        }
        freemem(-1,-1);
        for (i=0;i<12;i++) {
            colors[i].Red = defcolors[i].Red;
            colors[i].Green = defcolors[i].Green;
            colors[i].Blue = defcolors[i].Blue;
        }
    }
}
/* stop */
void _com_stop(int argn, char *argv[])
{
    if (hfp) {
        showpage(hfp);
        writeclose(hfp);
        fclose(hfp);
        hfp = NULL;
    }
    freemem(0,-1);
    close_down = TRUE;
}
/* version */
void _com_version(int argn, char *argv[])
{
    sprintf(field,"FEM2D - version: %s - date: %s - time: %s",VERSION,__DATE__,__TIME__);
    sprintf(field,"%s\nCopyright � 1993-2009 - Russell Leighton",field);
}

/* info */
void _com_info(int argn, char *argv[])
{
    int i,j,k,l,m,n,is;
    float xi,yi,x,y,vi,vf,t;

    if (argn > 1) {

        x = y = xi = yi = 0.0;

        stolower(argv[1]);
        if (!strncmp(argv[1],"m",1)) {      /* info mat */
            m = 0;
            if (argn > 2) {
                if (eval_arg(argv[2],&vals[2])) m = (int)vals[2];
                if ((m <= 0) || (m >= mmax)) { 
                    error("main - bad material id");
                    return; 
                }
            }
            if ((m <= 0) || (m >= mmax)) {
                error("main - material does not exist");
                return;
            }
            m = mnums[m];
            while (m >= 0) {
                if (mats[m].kind == pkind) break;
                else m = mats[m].next;
            }
            if ((m < 0) || (m >= nmats)) {
                error("main - material does not exist");
                return;
            }
            sprintf(field,"%5d %5d %5d",
                m,mats[m].id,mats[m].type);
            for (i=0;i<10;i++) 
                sprintf(field,"%s %12.4e",field,mats[m].C[i]);
            sprintf(field,"%s %5d %12.4e %12.4e %12.4e",field,
                mats[m].ckind[0],mats[m].f[0],mats[m].f[1],mats[m].ref[0]);
            sprintf(field,"%s %5d %12.4e %12.4e %12.4e",field,
                mats[m].ckind[1],mats[m].f[2],mats[m].f[3],mats[m].ref[1]);
            return; 
        }
        if (!strncmp(argv[1],"n",1)) {      /* info node */
            n = 0;
            if (argn > 2) {
                if ((n = arg2id(argv[2], nnodes, nmax, nnums)) < 0) {
                    error("main - bad node id");
                    return; 
                }
            }
            if ((n < 0) || (n >= nnodes)) { 
                error("main - no node selected");
                return; 
            }
            if (nodes[n].id == 0) { 
                error("main - node does not exist");
                return; 
            }
            tolocal(nodes[n].x,nodes[n].y,&x,&y);
            sprintf(field,"%5d %5d %12.4e %12.4e",n,ABS(nodes[n].id),x,y);
            return; 
        }
        if (!strncmp(argv[1],"e",1)) {      /* info elem */
            n = 0;
            if (argn > 2) {
                if ((n = arg2id(argv[2], nelems, emax, enums)) < 0) {
                    error("main - bad element id");
                    return; 
                }
            }
            if ((n < 0) || (n >= nelems)) { 
                error("main - no element selected");
                return; 
            }
            if (elems[n].id == 0) { 
                error("main - element does not exist");
                return; 
            }
            m = elems[n].type;
            sprintf(field,"%5d %5d %5d %5d %5d",
                n,ABS(elems[n].id),elems[n].mat,shapes[m]->nn,shapes[m]->cintg->ni);
            for (i=0;i<shapes[m]->nn;i++) 
                sprintf(field,"%s %5d",field,nnums[elems[n].nodes[i]]);
            return; 
        }
        if (!strncmp(argv[1],"la",2)) {      /* info label */
            n = 0;
            if (argn > 2) {
                if ((n = arg2id(argv[2], nlabs, jmax, jnums)) < 0) {
                    error("main - bad label id");
                    return; 
                }
            }
            if ((n < 0) || (n >= nlabs)) { 
                error("main - no label selected");
                return; 
            }
            if (labels[n].type == 0) { 
                error("main - label does not exist");
                return; 
            }
            sprintf(field,"%5d %5d '%s' %5d",n,labels[n].id,labels[n].string,labels[n].pid);
            return; 
        }
        if (!strncmp(argv[1],"li",2)) {      /* info line */
            n = 0;
            if (argn > 2) {
                if ((n = arg2id(argv[2], nlins, imax, inums)) < 0) {
                    error("main - bad line id");
                    return; 
                }
            }
            if ((n < 0) || (n >= nlins)) { 
                error("main - no line selected");
                return; 
            }
            if (lines[n].type == 0) { 
                error("main - line does not exist");
                return; 
            }
            j = 0;
            i = lines[n].pid;
            while (i > 0) {
                j++;
                i = points[pnums[i]].next;
            }
            sprintf(field,"%5d %5d %5d %5d %5d %5d",
                n,lines[n].id,lines[n].type,lines[n].col,lines[n].pid,j);
            return; 
        }
        if (!strncmp(argv[1],"po",2)) {      /* info point */
            n = 0;
            if (argn > 2) {
                if ((n = arg2id(argv[2], npnts, pmax, pnums)) < 0) {
                    error("main - bad point id");
                    return; 
                }
            }
            if ((n < 0) || (n >= npnts)) { 
                error("main - no point selected");
                return; 
            }
            if (points[n].type == 0) { 
                error("main - point does not exist");
                return; 
            }
            tolocal(points[n].x,points[n].y,&x,&y);
            k = l = 0;
            if ((k = getlabel(points[n].id)) >= 0) k = labels[k].id;
            if ((l = get_line(points[n].id)) >= 0) l = lines[l].id;
            t = points[n].ang*180.0/PI;
            sprintf(field,"%5d %5d %5d %5d %5d %5d %12.4e %12.4e %12.4e %5d",
                n,points[n].id,l,k,points[n].type,points[n].col,t,x,y,points[n].next);
            return; 
        }
        if (!strncmp(argv[1],"pb",2)) {      /* info pbc */
            n = 0;
            if (argn > 3) {
                if ((n = arg2id(argv[3], nnodes, nmax, nnums)) < 0) {
                    error("main - bad node id");
                    return; 
                }
            }
            if ((n < 0) || (n >= nnodes)) { 
                error("main - no node selected");
                return; 
            }
            if (nodes[n].id == 0) { 
                error("main - node does not exist");
                return; 
            }
            i = nodes[n].bc;
            if (argn > 2) {
                j = 0;
                if (eval_arg(argv[2],&vals[2])) j = (int)vals[2];
                while (i >= 0) {
                    if ((pbcs[i].kind == pkind) && (pbcs[i].dof == j)) break;
                    else i = pbcs[i].next;
                }
            }
            else {
                while (i >= 0) {
                    if (pbcs[i].kind == pkind) break;
                    else i = pbcs[i].next;
                }
            }
            sprintf(field,"%5d %5d",n,ABS(nodes[n].id));
            if (i >= 0) {
                if (pbcs[i].type == 0) return;
                sprintf(field,"%s %5d %5d %12.4e",field,
                        pbcs[i].type,pbcs[i].dof,pbcs[i].val);
            }
            return; 
        }
        if (!strncmp(argv[1],"d",1)) {      /* info dbc */
            n = i = 0;
            if (argn > 3) {
                if ((n = arg2id(argv[3], nelems, emax, enums)) < 0) {
                    error("main - bad element id");
                    return; 
                }
            }
            if ((n < 0) || (n >= nelems)) { 
                error("main - no element selected");
                return; 
            }
            if (elems[n].id == 0) { 
                error("main - element does not exist");
                return; 
            }
            is = 0;
            if (argn > 4) {
                if (eval_arg(argv[4],&vals[4])) is = (int)vals[4];
            }
            if ((is > shapes[elems[n].type]->ns) || (is <= 0)) is = i;
            i = elems[n].bc;
            if (argn > 2) {
                j = 0;
                if (eval_arg(argv[2],&vals[2])) j = (int)vals[2];
                while (i >= 0) {
                    if ((dbcs[i].kind == pkind) && (dbcs[i].dof == j) && (is == dbcs[i].side)) 
                        break;
                    else i = dbcs[i].next;
                }
            }
            else {
                while (i >= 0) {
                    if ((dbcs[i].kind == pkind) && (is == dbcs[i].side)) 
                        break;
                    else i = dbcs[i].next;
                }
            }
            sprintf(field,"%5d %5d %5d",n,ABS(elems[n].id),is);
            if (i >= 0) {
                if (dbcs[i].type == 0) return;
                sprintf(field,"%s %5d %5d",field,dbcs[i].type,dbcs[i].dof);
                for (j=0;j<=shapes[elems[n].type]->no;j++)
                    sprintf(field,"%s %12.4e",field,dbcs[i].val[j]);
            }
            return; 
        }
        if (!strncmp(argv[1],"s",1)) {      /* info solution */
            n = 0;
            if (argn > 3) {
                if ((n = arg2id(argv[3], nnodes, nmax, nnums)) < 0) {
                    error("main - bad node id");
                    return; 
                }
            }
            if ((n < 0) || (n >= nnodes)) { 
                error("main - no node selected");
                return; 
            }
            if (nodes[n].id == 0) { 
                error("main - node does not exist");
                return; 
            }
            if ((nsolns > 0) && (argn > 2)) {
                if (eval_arg(argv[2],&vals[2])) i = (int)vals[2];
                else return;
                for (j=0;j<nsolns;j++) {
                    if ((solns[j].dof == i) && (solns[j].type == 0) && (solns[j].kind == pkind)) {
                        sprintf(field,"%5d %5d %12.4e",n,ABS(nodes[n].id),solns[j].u[n]);
                        break;
                    }
                }
            }
            return; 
        }
        if (!strncmp(argv[1],"fo",2)) {      /* info force */
            n = 0;
            if (argn > 3) {
                if ((n = arg2id(argv[3], nnodes, nmax, nnums)) < 0) {
                    error("main - bad node id");
                    return; 
                }
            }
            if ((n < 0) || (n >= nnodes)) { 
                error("main - no node selected");
                return; 
            }
            if (nodes[n].id == 0) { 
                error("main - node does not exist");
                return; 
            }
            if ((nsolns > 0) && (argn > 2)) {
                if (eval_arg(argv[2],&vals[2])) i = (int)vals[2];
                else return;
                for (j=0;j<nsolns;j++) {
                    if ((solns[j].dof == i) && (solns[j].type == 1) && (solns[j].kind == pkind)) {
                        sprintf(field,"%5d %5d %12.4e",n,ABS(nodes[n].id),solns[j].u[n]);
                        break;
                    }
                }
            }
            return; 
        }
        if (!strncmp(argv[1],"fu",2)) {      /* info functions */
            for (i=0;i<nfuns;i++) {
                if ((strlen(field)+strlen(funs[i].name)) < 318)
                    sprintf(field,"%s %s",field,funs[i].name);
            }
            return; 
        }
        if (!strncmp(argv[1],"g",1)) {      /* info group */
            if (argn > 2) {
                if ((n = getname(argv[2])) < ngrps) {
                    j = 0;
                    for (k=0;k<nnodes;k++) {
                        i = ABS(nodes[k].id);
                        if (i >= groups[n].nmax) continue;
                        if (groups[n].ng[i>>5] & 1L<<(i%32)) j++;
                    }
                    sprintf(field,"%5d",j);
                }
            }
            else {
                for (i=0;i<ngrps;i++) {
                    if ((strlen(field)+strlen(groups[i].name)) < 318)
                        sprintf(field,"%s %s",field,groups[i].name);
                }
            }
            return; 
        }
        if (!strncmp(argv[1],"r",1)) {      /* info result */
            if (argn > 2) {
                if ((n = getfun(argv[2])) < nfuns) {
                    for (i=0;i<nsets;i++) {
                        if ((sets[i].func == n) && (sets[i].kind == pkind)) break;
                    }
                    if (i == nsets) {
                        error("main - not a selected result function");
                        return; 
                    }
                }
                else {
                    error("main - function not defined");
                    return;
                }
                if (sets[i].results) {
                    vi = sets[i].min;
                    vf = sets[i].max;
                    sprintf(field,"%5d %5d %12.4e %12.4e",sets[i].ndof,sets[i].flag,vi,vf);
                }
                else sprintf(field,"%5d %5d",sets[i].ndof,sets[i].flag);
            }
            else {
                n = MIN(nsets,32);
                for (i=0;i<n;i++) {
                    if (sets[i].kind != pkind) continue;
                    if (sets[i].results) sprintf(field,"%s *%s",field,funs[sets[i].func].name);
                    else sprintf(field,"%s %s",field,funs[sets[i].func].name);
                }
            }
            return; 
        }
        if (!strncmp(argv[1],"pr",2)) {      /* info problem */
            k = pkind;
            if (argn > 2) {
                if (eval_arg(argv[2],&vals[2])) k = (int)vals[2];
            }
            if ((k < 0) || (k >= nprobs)) sprintf(field,"%5d 0 0 0 ''",k);
            else {
                m = n = 0;
                for (j=0;j<nsolns;j++) {
                    if (solns[j].kind == k) m++;
                }
                for (i=0;i<nsets;i++) {
                    if (sets[i].kind == k) n++;
                }
                sprintf(field,"%5d %5d %5d %5d '%s'",
                    k,probs[k].ndof,m,n,probs[k].title);
            }
            return; 
        }
        if (!strncmp(argv[1],"c",1)) {      /* info coord */
            if (ctype) sprintf(field,"polar %12.4e %12.4e",Xorg,Yorg);
            else sprintf(field,"cart %12.4e %12.4e",Xorg,Yorg);
            return; 
        }
        if (!strncmp(argv[1],"w",1)) {      /* info window */
            sprintf(field,"%12.4e %12.4e %12.4e %12.4e",view.xmin,view.xmax,view.ymin,view.ymax);
            return; 
        }
    }
    else {
        if (ptype) 
            sprintf(field,"axisym %5d %5d %5d %5d %5d %5d",
                nnodes,nelems,ngrps,npnts,nlins,nlabs);
        else
            sprintf(field,"plane %5d %5d %5d %5d %5d %5d",
                nnodes,nelems,ngrps,npnts,nlins,nlabs);
    }
}
/* open(left,top,width,height,pubname) */
void _com_open(int argn, char *argv[])
{
}
/* close */
void _com_close(int argn, char *argv[])
{
}
/* plot */
void _com_plot(int argn, char *argv[])
{
    int i,j,m,n;
    float f,vi,vf;

    if (!hfp) {
        error("main - hardcopy file not open");
        return;
    }
    if (argn > 1) {
        stolower(argv[1]);
        /* plot model */
        if (!strncmp(argv[1],"m",1)) {
            plotmodel(hfp);
            return; 
        }
        /* plot draw */
        if (!strncmp(argv[1],"dr",2)) {
            plotdraw(hfp);
            return; 
        }
        /* plot deform scale */
        if (!strncmp(argv[1],"de",2)) {
            f = 1.0;
            if (argn > 2) {
                if (eval_arg(argv[2],&vals[2])) f = vals[2];
            }
            plotdeform(f,hfp);
            return; 
        }
        /* plot result fun comb rmin rmax */
        if (!strncmp(argv[1],"r",1)) {
            if (argn < 3) return;
            if ((m = getfun(argv[2])) < nfuns) {
                for (i=0;i<nsets;i++) {
                    if ((sets[i].func == m) && (sets[i].kind == pkind)) break;
                }
                if (i == nsets) {
                    error("main - not a selected result function");
                    return; 
                }
            }
            else {
                error("main - function not defined");
                return;
            }
            if (sets[i].results) {
                n = 0;
                if (argn > 3) {
                    if (eval_arg(argv[3],&vals[3])) n = (int)vals[3];
                }
                vi = sets[i].min;
                if (argn > 4) {
                    if (eval_arg(argv[4],&vals[4])) vi = vals[4];
                }
                vf = sets[i].max;
                if (argn > 5) {
                    if (eval_arg(argv[5],&vals[5])) vf = vals[5];
                }
                pkind = sets[i].kind;
                pndof = sets[i].ndof;
                fillresult(i,-2,n,vi,vf,hfp);
            }
            else error("main - no results processed");
            return; 
        }
        /* plot vector fun1 fun2 subd rmin rmax */
        if (!strncmp(argv[1],"v",1)) {
            if (argn < 4) return;
            if ((m = getfun(argv[2])) < nfuns) {
                for (i=0;i<nsets;i++) {
                    if ((sets[i].func == m) && (sets[i].kind == pkind)) break;
                }
                if (i == nsets) {
                    error("main - not a selected result function");
                    return; 
                }
            }
            else {
                error("main - function not defined");
                return;
            }
            if ((m = getfun(argv[3])) < nfuns) {
                for (j=0;j<nsets;j++) {
                    if ((sets[j].func == m) && (sets[j].kind == pkind)) break;
                }
                if (j == nsets) {
                    error("main - not a selected result function");
                    return; 
                }
            }
            else {
                error("main - function not defined");
                return;
            }
            if (sets[i].results && sets[j].results) {
                n = 0;
                if (argn > 4) {
                    if (eval_arg(argv[4],&vals[4])) n = (int)vals[4];
                }
                vi = sets[i].min;
                if (argn > 5) {
                    if (eval_arg(argv[5],&vals[5])) vi = vals[5];
                }
                vf = sets[i].max;
                if (argn > 6) {
                    if (eval_arg(argv[6],&vals[6])) vf = vals[6];
                }
                pkind = sets[i].kind;
                pndof = sets[i].ndof;
                plotresult(i,j,n,vi,vf,hfp);
            }
            else error("main - no results processed");
                return; 
        }
        /* plot bcs */
        if (!strncmp(argv[1],"bc",2)) {
            for (i=0;i<npbcs;i++) plotpbc(i,1,hfp);
            for (i=0;i<ndbcs;i++) plotdbc(i,1,hfp);
            return; 
        }
        /* plot bound */
        if (!strncmp(argv[1],"bo",2)) {
            plotbound();
            return; 
        }
    }
    else {
        plotmodel(hfp);
        for (i=0;i<npbcs;i++) plotpbc(i,1,hfp);
        for (i=0;i<ndbcs;i++) plotdbc(i,1,hfp);
        plotdraw(hfp);
    }
}
/* show */
void _com_show(int argn, char *argv[])
{
}
/* fill */
void _com_fill(int argn, char *argv[])
{
    int i,m,n;
    float f,vi,vf;

    if (!hfp) {
        error("main - hardcopy file not open");
        return;
    }
    if (argn > 1) {
        stolower(argv[1]);
        if (!strncmp(argv[1],"m",1)) {
            fillmodel(hfp);
            return; 
        }
        if (!strncmp(argv[1],"d",1)) {
            f = 1.0;
            if (argn > 2) {
                if (eval_arg(argv[2],&vals[2])) f =vals[2];
            }
            filldeform(f,hfp);
            return; 
        }
        if (!strncmp(argv[1],"r",1)) {
            if (argn < 3) return;
            if ((m = getfun(argv[2])) < nfuns) {
                for (i=0;i<nsets;i++) {
                    if ((sets[i].func == m) && (sets[i].kind == pkind)) break;
                }
                if (i == nsets) {
                    error("main - not a selected result function");
                    return; 
                }
            }
            else {
                error("main - function not defined");
                return;
            }
            if (sets[i].results) {
                m = n = 0;
                if (argn > 3) {
                    stolower(argv[3]);
                    m = -1;
                    if (!strncmp(argv[3],"ma",2)) m = 0;
                    if (!strncmp(argv[3],"mi",2)) m = 1;
                    if (!strncmp(argv[3],"a",1)) m = 2;
                    if (m < 0) {
                        if (eval_arg(argv[3],&vals[3])) n = (int)vals[3];
                    }
                }
                vi = sets[i].min;
                if (argn > 4) {
                    if (eval_arg(argv[4],&vals[4])) vi = vals[4];
                }
                vf = sets[i].max;
                if (argn > 5) {
                    if (eval_arg(argv[5],&vals[5])) vf = vals[5];
                }
                pkind = sets[i].kind;
                pndof = sets[i].ndof;
                fillresult(i,m,n,vi,vf,hfp);
            }
            else error("main - no results processed");
            return; 
        }
    }
    else {
        fillmodel(hfp);
    }
}
/* hard */
void _com_hard(int argn, char *argv[])
{
    if (argn > 1) {
        strcpy(filename,argv[1]);
    }
    else {
        if (hfp) {
            showpage(hfp);
            writeclose(hfp);
            fclose(hfp);
            hfp = NULL;
            filename[0] = 0;
        }
    }
    if (filename[0] != 0) {
        hfp = fopen((char *)filename,"w");
        if (hfp) writeprolog(hfp);
    }
}
/* session */
void _com_session(int argn, char *argv[])
{
}
/* window(xmin,xmax,ymin,ymax) */
void _com_window(int argn, char *argv[])
{
    argn = eval_args(1, argn, argv);
    if (argn > 1) view.xmin = vals[1];
    if (argn > 2) view.xmax = vals[2];
    if (argn > 3) view.ymin = vals[3];
    if (argn > 4) view.ymax = vals[4];

    view.xtol = 0.0;
    view.ytol = 0.0;
}
/* coord(type,xorg,yorg) */
void _com_coord(int argn, char *argv[])
{
    argn = eval_args(2, argn, argv);
    if (argn > 1) {
        stolower(argv[1]);
        if (!strncmp(argv[1],"p",1)) ctype = 1;
        else ctype = 0;
    }
    else ctype = 0;
    Xorg = Yorg = 0.0;
    if (argn > 2) Xorg = vals[2];
    if (argn > 3) Yorg = vals[3];
}
/* grid(xinc,yinc) */
void _com_grid(int argn, char *argv[])
{
}
/* find(all) */
void _com_find(int argn, char *argv[])
{
    findwindow(argn);
}
/* zoom(factor) */
void _com_zoom(int argn, char *argv[])
{
    float f,dx,dy;

    f = 0.0;
    if (argn > 1) {
        if (eval_arg(argv[1],&vals[1])) f = vals[1];
        dx = ((1.0+f)*view.xmin + (1.0-f)*view.xmax)/2.0;
        view.xmax = ((1.0+f)*view.xmax + (1.0-f)*view.xmin)/2.0;
        dy = ((1.0+f)*view.ymin + (1.0-f)*view.ymax)/2.0;
        view.ymax = ((1.0+f)*view.ymax + (1.0-f)*view.ymin)/2.0;
        view.xmin = dx; view.ymin = dy;
    }
}
/* move(ax,ay) */
void _com_move(int argn, char *argv[])
{
    float ax,ay,dx,dy;

    argn = eval_args(1, argn, argv);
    ax = ay = 0.0;
    if (argn > 1) {
        ax = vals[1];
        if (argn > 2) ay = vals[2];
        dx = (1.0-ax)*view.xmin + ax*view.xmax;
        view.xmax = (1.0+ax)*view.xmax - ax*view.xmin;
        dy = (1.0-ay)*view.ymin + ay*view.ymax;
        view.ymax = (1.0+ay)*view.ymax - ay*view.ymin;
        view.xmin = dx; view.ymin = dy;
    }
}
/* clear */
void _com_clear(int argn, char *argv[])
{
    clearwindow(hfp);
}
/* group(name,oper,data[]) */
void _com_group(int argn, char *argv[])
{
    int i,j,k,l,m,n;
    float xi,yi,x,y,dx,dy,r,t;

    if (argn > 2) {
        stolower(argv[2]);
        /* group name 'save' filename */
        if (!strncmp(argv[2],"sa",2)) {
            if ((n = getname(argv[1])) == ngrps) return;
            if (argn > 3) {
                strcpy(filename,argv[3]);
            }
            if (filename[0] != 0) {
                if (!savegroup(n,(char *)filename)) 
                    error("main - error saving file");
            }
            return; 
        }
        /* group name 'read' filename */
        if (!strncmp(argv[2],"rea",3)) {
            if (argn > 3) {
                strcpy(filename,argv[3]);
            }
            if (filename[0] != 0) {
                n = getname(argv[1]);
                if (!readgroup(n,(char *)filename)) { 
                    error("main - error reading file");
                    return; 
                }
                strncpy(groups[n].name,argv[1],11);
                groups[n].name[11] = 0;
                if (solns) freemem(1,-1);
            }
            return; 
        }
        argn = eval_args(3, argn, argv);
        /* group name 'add' n1 n2 */
        if (!strncmp(argv[2],"a",1)) {
            if ((n = getname(argv[1])) == ngrps) {
                n = ngrps++;
                groups = (struct group *)realloc(groups,ngrps*sizeof(struct group));
                if (groups == NULL) { 
                    error("main - could not allocate memory for groups");
                    return; 
                }
                strncpy(groups[n].name,argv[1],11);
                groups[n].name[11] = 0;
                groups[n].nmax = 0;
                groups[n].ng = 0;
            }
            if (groups[n].nmax < nmax) {
                m = (nmax + 31)>>5;
                groups[n].ng = (long *)realloc(groups[n].ng,m*sizeof(long));
                if (groups[n].ng == NULL) { 
                    error("main - could not allocate memory for groups");
                    return; 
                }
                l = (groups[n].nmax + 31)>>5;
                for (j=l;j<m;j++) groups[n].ng[j] = 0.0;
                groups[n].nmax = nmax;
            }
            if (argn > 3) {
                j = 1;
                j = (int)vals[3];
                k = j;
                if (argn > 4) k = (int)vals[4];
                if (k == 0) k = nmax-1;
                k = MIN(k,nmax-1);
                for (m=0;m<nelems;m++) {
                    if (elems[m].id <= 0) continue;
                    for (l=0;l<shapes[elems[m].type]->nn;l++) {
                        i = elems[m].nodes[l];
                        if ((i >= j) && (i <= k)) {
                            groups[n].ng[i>>5] |= 1L<<(i%32);
                        }
                    }
                }
                for (i=j;i<=k;i++) {
                    if (nodes[nnums[i]].id >= 0) continue;
                    if (nnums[i] >= 0) {
                        groups[n].ng[i>>5] |= 1L<<(i%32);
                    }
                }
            }
            return; 
        }
        /* group name 'rem' n1 n2 */
        if (!strncmp(argv[2],"rem",3)) {
            if ((n = getname(argv[1])) == ngrps) return;
            if (argn > 3) {
                j = 1;
                j = (int)vals[3];
                k = j;
                if (argn > 4) k = (int)vals[4];
                if (k == 0) k = groups[n].nmax-1;
                k = MIN(k,groups[n].nmax-1);
                for (i=j;i<=k;i++) {
                    if (nodes[nnums[i]].id == 0) continue;
                    if (nnums[i] >= 0) {
                        if (groups[n].ng[i>>5] & 1L<<(i%32)) {
                            groups[n].ng[i>>5] &= ~(1L<<(i%32));
                        }
                    }
                }
            }
            return; 
        }
        /* group name 'tran' x1 y1 x2 y2 */
        if (!strncmp(argv[2],"t",1)) {
            if ((n = getname(argv[1])) != ngrps) {
                xi = yi = x = y = 0.0;
                if (argn > 4) { xi = vals[3]; yi = vals[4]; }
                if (argn > 6) { x = vals[5]; y = vals[6]; }
                for (k=0;k<nnodes;k++) {
                    i = ABS(nodes[k].id);
                    if (i >= groups[n].nmax) continue;
                    if (groups[n].ng[i>>5] & 1L<<(i%32)) {
                        if (ctype == 0) {
                            nodes[k].x += x - xi;
                            nodes[k].y += y - yi;
                        }
                        else {
                            dx = nodes[k].x - Xorg;
                            dy = nodes[k].y - Yorg;
                            r = (float)sqr((double)(dx*dx + dy*dy));
                            t = (float)atan2((double)dy,(double)dx)*180.0/PI;
                            r += x - xi;
                            t += y - yi;
                            t *= PI/180.0;
                            nodes[k].x = r*cos(t) + Xorg;
                            nodes[k].y = r*sin(t) + Yorg;
                        }
                    }
                }
            }
            return; 
        }
        /* group name 'del' */
        if (!strncmp(argv[2],"d",1)) {
            if ((n = getname(argv[1])) != ngrps) {
                for (k=0;k<nelems;k++) {
                    if (elems[k].id == 0) continue;
                    for (j=0;j<shapes[elems[k].type]->nn;j++) {
                        i = elems[k].nodes[j];
                        if (i >= groups[n].nmax) continue;
                        if (groups[n].ng[i>>5] & 1L<<(i%32)) break;
                    }
                    if (j != shapes[elems[k].type]->nn) {
                        elems[k].id = 0;
                        if ((i = elems[k].bc) >= 0) {
                            do {
                        dbcs[i].type = 0;
                            } while ((i = dbcs[i].next) >= 0);
                        }
                    }
                }
                for (k=0;k<nnodes;k++) {
                    i = ABS(nodes[k].id);
                    if (i >= groups[n].nmax) continue;
                    if (groups[n].ng[i>>5] & 1L<<(i%32)) {
                        nodes[k].id = 0;
                        if ((j = nodes[k].bc) >= 0) {
                            do {
                        pbcs[j].type = 0;
                            } while ((j = pbcs[j].next) >= 0);
                        }
                    }
                }
            }
            return; 
        }
        /* group name 'off' */
        if (!strncmp(argv[2],"of",2)) {
            if ((n = getname(argv[1])) != ngrps) {
                for (k=0;k<nelems;k++) {
                    if (elems[k].id == 0) continue;
                    for (j=0;j<shapes[elems[k].type]->nn;j++) {
                        i = elems[k].nodes[j];
                        if (i >= groups[n].nmax) break;
                        if (!(groups[n].ng[i>>5] & 1L<<(i%32))) break;
                    }
                    if (j == shapes[elems[k].type]->nn) {
                        elems[k].id = -ABS(elems[k].id);
                    }
                }
            }
            return; 
        }
        /* group name 'on' */
        if (!strncmp(argv[2],"on",2)) {
            if ((n = getname(argv[1])) != ngrps) {
                for (k=0;k<nelems;k++) {
                    if (elems[k].id == 0) continue;
                    for (j=0;j<shapes[elems[k].type]->nn;j++) {
                        i = elems[k].nodes[j];
                        if (i >= groups[n].nmax) break;
                        if (!(groups[n].ng[i>>5] & 1L<<(i%32))) break;
                    }
                    if (j == shapes[elems[k].type]->nn) {
                        elems[k].id = ABS(elems[k].id);
                    }
                }
            }
            return; 
        }
    }
}
/* set(function=expression) */
void _com_set(int argn, char *argv[])
{
    int m,n;

    if (argn > 1) {
        if ((n = getfun(argv[1])) == nfuns) {
            n = nfuns++;
            funs = (struct fun *)realloc(funs,nfuns*sizeof(struct fun));
            if (funs == NULL) { 
                error("main - could not allocate memory for funs");
                return; 
            }
            strncpy(funs[n].name,argv[1],11);
            funs[n].name[11] = 0;
            funs[n].nchar = 0;
            funs[n].expr = 0;
        }
        if (argn > 2) {
            m = 2*(strlen(argv[2]) >> 1) + 2;
            if (funs[n].nchar != m) {
                funs[n].expr = (char *)realloc(funs[n].expr,m*sizeof(char));
                if (funs[n].expr == NULL) { 
                    error("main - could not allocate memory for expression");
                    return; 
                }
                funs[n].nchar = m;
            }
            strcpy(funs[n].expr,argv[2]);
        }
    }
}
/* get(function) */
void _com_get(int argn, char *argv[])
{
    int n;

    if (argn > 1) {
        if ((n = getfun(argv[1])) != nfuns) strcpy(field,funs[n].expr);
        else if ((n = getalias(argv[1])) != nalias) 
            strcpy(field,aliases[n].expr);
    }
}
/* alias(command=expression) */
void _com_alias(int argn, char *argv[])
{
    int m,n;

    if (argn > 1) {
        if ((n = getalias(argv[1])) == nalias) {
            n = nalias++;
            aliases = (struct fun *)realloc(aliases,nalias*sizeof(struct fun));
            if (aliases == NULL) { 
                error("main - could not allocate memory for aliases");
                return; 
            }
            strncpy(aliases[n].name,argv[1],11);
            aliases[n].name[11] = 0;
            aliases[n].nchar = 0;
            aliases[n].expr = 0;
        }
        if (argn > 2) {
            m = 2*(strlen(argv[2]) >> 1) + 2;
            if (aliases[n].nchar != m) {
                aliases[n].expr = (char *)realloc(aliases[n].expr,m*sizeof(char));
                if (aliases[n].expr == NULL) { 
                    error("main - could not allocate memory for expression");
                    return; 
                }
                aliases[n].nchar = m;
            }
            strcpy(aliases[n].expr,argv[2]);
        }
    }
}
/* cd(dir) */
void _com_cd(int argn, char *argv[])
{
}
/* why */
void _com_why(int argn, char *argv[])
{
    strcpy(field,lasterr);
    lasterr[0] = 0;
}
/* color(index,r,g,b) */
void _com_color(int argn, char *argv[])
{
    int i,j,k,n;

    argn = eval_args(1, argn, argv);
    if (argn > 1) n = (int)vals[1];
    else n = 1;
    if (argn > 2) i = (int)vals[2];
    else i = 15;
    if (argn > 3) j = (int)vals[3];
    else j = 15;
    if (argn > 4) k = (int)vals[4];
    else k = 15;
    if ((n < 1) || (n > 12)) return;
    colors[n-1].Red = i%16;
    colors[n-1].Green = j%16;
    colors[n-1].Blue = k%16;
}
/* view(id,frac) */
void _com_view(int argn, char *argv[])
{
}
/* auto */
void _com_auto(int argn, char *argv[])
{
    if (argn > 1) {
        stolower(argv[1]);
        if (!strncmp(argv[1],"e",1)) {
            if (argn > 2) erase = FALSE;
            else erase = TRUE;
        }
    }
}
/* point(id,type,col,ang,x,y) */
void _com_point(int argn, char *argv[])
{
    int k,l,m,n,id;
    float xi,yi,x,y;

    argn = eval_args(1, argn, argv);
    id = pmax;
    if (argn > 1) id = (int)vals[1];
    if (id < 1) id = pmax;
    k = l = -1;
    if (id < pmax) {
        n = pnums[id];
        if ((n >= 0) && (points[n].type != 0)) {
            k = getlabel(id);
            l = get_line(id);
            if (argn < 3) {
                points[n].type = 0;
                if ((k < 0) && (l < 0)) return;
            }
        }
    }
    if ((n = newpoint(id)) < 0) return;
    x = y = xi = yi = 0.0;
    if (argn > 5) {
        x = vals[5];
        if (argn > 6) y = vals[6];
    }
    if ((argn > 5) || (points[n].type == 0))
        toglobal(x,y,&(points[n].x),&(points[n].y));
    if (argn > 2) points[n].type = (int)vals[2];
    if (points[n].type > 9) points[n].type = 9;
    if (points[n].type == 0) points[n].type = 1;
    if (argn > 3) points[n].col = (int)vals[3];
    if (argn > 4) points[n].ang = vals[4]*PI/180.0;
    if (l >= 0) {
        m = lines[l].pid;
        while (m > 0) {
            if (m != id) {
                points[pnums[m]].type = points[n].type;
                points[pnums[m]].col = points[n].col;
                points[pnums[m]].ang = points[n].ang;
            }
            m = points[pnums[m]].next;
        }
    }
}
/* line(id,type,col,pid) */
void _com_line(int argn, char *argv[])
{
    int i,j,m,n,id;
    float x,y;

    argn = eval_args(1, argn, argv);
    id = imax;
    if (argn > 1) id = (int)vals[1];
    if (id < 1) id = imax;
    if (id < imax) {
        n = inums[id];
        if ((n >= 0) && (lines[n].type != 0)) {
            if (argn < 3) {
                lines[n].type = 0;
                if ((i = lines[n].pid)) {
                    do {
                        points[pnums[i]].type = 0;
                    } while ((i = points[pnums[i]].next) > 0);
                }
                lines[n].pid = 0;
                return;
            }
        }
    }
    if ((n = newline(id)) < 0) return;
    if (argn > 2) lines[n].type = (int)vals[2];
    if (!lines[n].type) return;
    if (argn > 3) lines[n].col = (int)vals[3];
    if (argn > 4) {
        for(j=4;j<argn;j++) {
            m = (int)vals[j];
            if(m <= 0) m = pmax;
            if ((i = newpoint(m)) < 0) return;
            if(!points[i].type) {
                points[i].type = 1;
                points[i].col = lines[n].col;
                x = y = 0.0;
                toglobal(x,y,&(points[i].x),&(points[i].y));
            }
            if ((i = lines[n].pid)) {
                while (points[pnums[i]].next > 0) i = points[pnums[i]].next;
                if (points[pnums[m]].next == 0) points[pnums[i]].next = m;
                else points[pnums[i]].next = -m;
            }
            else lines[n].pid = m;
        }
    }
}

/* label(id,string,pid) */
void _com_label(int argn, char *argv[])
{
    int i,m,n,id;
    float xi,yi,x,y;

    id = jmax;
    if (argn > 1) {
        if (eval_arg(argv[1],&vals[1])) id = (int)vals[1];
    }
    if (id < 1) id = jmax;
    m = pmax;
    if (id < jmax) {
        n = jnums[id];
        if ((n >= 0) && (labels[n].type != 0)) {
            m = labels[n].pid;
            if (argn < 3) {
                labels[n].type = 0;
                points[pnums[m]].type = 0;
                return;
            }
        }
    }
    if ((n = newlabel(id)) < 0) return;
    if (argn > 3) {
        if (eval_arg(argv[3],&vals[3])) m = (int)vals[3];
    }
    if(m <= 0) m = pmax;
    if ((i = newpoint(m)) < 0) return;
    if(!points[i].type) {
        points[i].type = 1;
        points[i].col = 1;
        x = y = xi = yi = 0.0;
        toglobal(x,y,&(points[i].x),&(points[i].y));
    }
    labels[n].pid = m;
    labels[n].type = 1;
    m = 2*(strlen(argv[2]) >> 1) + 2;
    if (labels[n].nchar != m) {
        labels[n].string = (char *)realloc(labels[n].string,m*sizeof(char));
        if (labels[n].string == NULL) { 
            error("main - could not allocate memory for label string");
            return; 
        }
        labels[n].nchar = m;
    }
    strncpy(labels[n].string,argv[2],m-1);
    labels[n].string[m-1] = 0;
}

int getfun(char *name)
{
    int i;

    for (i=0;i<nfuns;i++) {
        if (!strncmp(name,funs[i].name,11)) break;
    }
    return(i);
}

int getname(char *name)
{
    int i;

    for (i=0;i<ngrps;i++) {
        if (!strncmp(name,groups[i].name,11)) break;
    }
    return(i);
}

int getalias(char *alias)
{
    int i;

    for (i=0;i<nalias;i++) {
        if (!strncmp(alias,aliases[i].name,11)) break;
    }
    return(i);
}

int newmat(int id,
           int kind)
{
    int i,j;

    if (id+1 > mmax) {
        if ((mnums = newobject( id, mmax, mnums )) != NULL) mmax = id+1;
        else return(-1);
    }
    i = j = mnums[id];
    while (i >= 0) {
        j = i;
        if (kind == mats[i].kind) break;
        else i = mats[j].next;
    }
    if (i < 0) {
        i = nmats++;
        mats = (struct mat *)realloc(mats,nmats*sizeof(struct mat));
        if (mats == NULL) { 
            error("newmat - could not allocate memory for mats");
            return(-1);
        }
        if (mnums[id] < 0) mnums[id] = i;
        if (j >= 0) mats[j].next = i;
        mats[i].next = -1;
    }
    for (j=0;j<10;j++) mats[i].C[j] = 0.0;
    for (j=0;j<4;j++) mats[i].f[j] = 0.0;
    mats[i].ref[0] = mats[i].ref[1] = -1.0;
    mats[i].kind = mats[i].ckind[0] = mats[i].ckind[1] = kind;
    mats[i].id = id;
    return(i);
}

int newnode(int id)
{
    int n;

    if (id+1 > nmax) {
        if ((nnums = newobject( id, nmax, nnums )) != NULL) nmax = id+1;
        else return(-1);
    }
    n = nnums[id];
    if (n < 0) {
        nnums[id] = n = nnodes++;
        nodes = (struct node *)realloc(nodes,nnodes*sizeof(struct node));
        if (nodes == NULL) { 
            error("newnode - could not allocate memory for nodes");
            return(-1);
        }
        nodes[n].bc = -1;
    }
    nodes[n].id = -id;
    return(n);
}

int newelem(int id)
{
    int n;

    if (id+1 > emax) {
        if ((enums = newobject( id, emax, enums )) != NULL) emax = id+1;
        else return(-1);
    }
    n = enums[id];
    if (n < 0) {
        enums[id] = n = nelems++;
        elems = (struct elem *)realloc(elems,nelems*sizeof(struct elem));
        if (elems == NULL) { 
            error("main - could not allocate memory for elems");
            return(-1);
        }
        elems[n].bc = -1;
    }
    elems[n].id = id;
    return(n);
}

int newpoint(int id)
{
    int n;

    if (id+1 > pmax) {
        if ((pnums = newobject( id, pmax, pnums )) != NULL) pmax = id+1;
        else return(-1);
    }
    n = pnums[id];
    if (n < 0) {
        pnums[id] = n = npnts++;
        points = (struct point *)realloc(points,npnts*sizeof(struct point));
        if (points == NULL) { 
            error("newpoint - could not allocate memory for points");
            return(-1);
        }
        points[n].type = 0;
        points[n].next = 0;
        points[n].col = 1;
        points[n].ang = 0.0;
    }
    points[n].id = id;
    return(n);
}

int newlabel(int id)
{
    int n;

    if (id+1 > jmax) {
        if ((jnums = newobject( id, jmax, jnums )) != NULL) jmax = id+1;
        else return(-1);
    }
    n = jnums[id];
    if (n < 0) {
        jnums[id] = n = nlabs++;
        labels = (struct label *)realloc(labels,nlabs*sizeof(struct label));
        if (labels == NULL) { 
            error("main - could not allocate memory for labels");
            return(-1);
        }
        labels[n].type = 0;
        labels[n].pid = 0;
        labels[n].string = 0;
        labels[n].nchar = 0;
    }
    labels[n].id = id;
    return(n);
}

int newline(int id)
{
    int n;

    if (id+1 > imax) {
        if ((inums = newobject( id, imax, inums )) != NULL) imax = id+1;
        else return(-1);
    }
    n = inums[id];
    if (n < 0) {
        inums[id] = n = nlins++;
        lines = (struct line *)realloc(lines,nlins*sizeof(struct line));
        if (lines == NULL) { 
            error("main - could not allocate memory for lines");
            return(-1);
        }
        lines[n].type = 0;
        lines[n].pid = 0;
        lines[n].col = 1;
    }
    lines[n].id = id;
    return(n);
}

int *newobject( int id, int omax, int *onums )
{
    int i;

    onums = (int *)realloc(onums,(id+1)*sizeof(int));
    if (onums == NULL)
        error("newobject - could not allocate memory for object nums");
    else {
        for (i=omax;i<=id;i++) onums[i] = -1;
    }
    return(onums);
}

int getlabel( int id )
{
    int i,n;

    for (i=0;i<nlabs;i++) {
        n = labels[i].pid;
        if (n == id) return(i);
    }
    return(-1);
}

int get_line( int id )
{
    int i,n;

    for (i=0;i<nlins;i++) {
        n = lines[i].pid;
        while (n > 0) {
            if (n == id) return(i);
            n = points[pnums[n]].next;
        }
    }
    return(-1);
}

void toglobal( float x,
               float y,
               float *xx,
               float *yy )
{
    float r,t;

    if (ctype == 0) {
        *xx = x + Xorg;
        *yy = y + Yorg;
    }
    else {
        r = x; t = PI*y/180.0;
        *xx = r*cos(t) + Xorg;
        *yy = r*sin(t) + Yorg;
    }
}

void tolocal( float x,
              float y,
              float *xx,
              float *yy )
{
    x -= Xorg; y -= Yorg;
    if (ctype != 0) {
        *xx = (float)sqr((double)(x*x + y*y));
        *yy = (float)atan2((double)y,(double)x)*180.0/PI;
    }
    else {
        *xx = x;
        *yy = y;
    }
}

int arg2id(char *arg, int nn, int nm, int *nums)
{
    int n = 0;
    float f;

    stolower(arg);
    if (!strncmp(arg,"m",1)) n = 1 - nm;
    else {
        if (!strncmp(arg,"l",1)) n = nn - 1;
        else if (eval_arg(arg,&f)) n = (int)f;
    }
    if (n < 0) {
        n = -n;
        if (n < nm) n = nums[n];
        else return (-1);
    }
    return (n);
}

void stolower(char *icom)
{
    while (*icom != 0) { *icom = (char)tolower((int)(*icom)); icom++; }
}

int eval_args(int argi, int argc, char *argv[])
{
    int i;

    for (i=argi;i<argc;i++) {
        if (!eval_arg(argv[i],&vals[i])) break;
    }
    return(i);
}
