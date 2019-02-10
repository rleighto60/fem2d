/*
$Id: plot.c 1.3 1994/09/14 17:41:05 leighton Exp leighton $
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

program     -   plot.c

description -   plot routines.

$Log: plot.c $
 * Revision 1.3  1994/09/14  17:41:05  leighton
 * made source code ANSI compliant
 *
 * Revision 1.2  1994/09/12  19:54:11  leighton
 * put in support for 'views'.
 *
 * Revision 1.1  1994/09/12  18:33:15  leighton
 * Initial revision
 *
*/
#include "header.h"
#include "plot.h"

/*  getelem - get element coordinates (and test if element is outside window)

    n       - element order number
    x,y     - pointer to arrays to hold coordinates

    returns true if element inside window, false otherwise     */

int getelem(int n,
            float *x,
            float *y,
            float f)
{
    float xmin,xmax,ymin,ymax,dx,dy;
    int i,j,nt,nn,ni;

    nt = elems[n].type;
    nn = shapes[nt]->nn;

    xmin = ymin = 1.e10;
    xmax = ymax = -1.e10;
    for (i=0;i<nn;i++) {
        ni = nnums[elems[n].nodes[i]];
        if ((ni < 0) || (ni >= nnodes) || (elems[n].nodes[i] >= nmax)) return(0);
        dx = dy = 0.0;
        if ((pndof > 1) && (f > 0.0)) {
            for (j=0;j<nsolns;j++) {
                if ((solns[j].kind == pkind) && (solns[j].type == 0)) {
                    switch (solns[j].dof) {
                        case 1: dx = solns[j].u[ni]; break;
                        case 2: dy = solns[j].u[ni]; break;
                    }
                }
            }
        }
        x[i] = nodes[ni].x + f*dx;
        y[i] = nodes[ni].y + f*dy;
        xmin = MIN(x[i],xmin); ymin = MIN(y[i],ymin);
        xmax = MAX(x[i],xmax); ymax = MAX(y[i],ymax);
    }
    if ((xmax < view.xmin) || 
        (xmin > view.xmax) || 
        (ymax < view.ymin) || 
        (ymin > view.ymax)) return(0);
    else return(1);
}

void clearwindow(FILE *hfp)
{

    if (hfp) {
        showpage(hfp);
        fprintf(hfp,"%%%%Page:\ngs\n0 %d 0 %d init\n",WIDTH,HEIGHT);
    }
    lastnode = lastelem = -1;
}

/*  findwindow   - set window bounds to include complete model      */

void findwindow()
{
    float tol=1.e-5;
    float dx,dy;
    int i;

    if (nnodes || npnts) {
        view.xmin = view.ymin = 1.e10;
        view.xmax = view.ymax = -1.e10;
        for (i=0;i<nnodes;i++) {
            if (nodes[i].id == 0) continue;
            view.xmin = MIN(nodes[i].x,view.xmin);
            view.ymin = MIN(nodes[i].y,view.ymin);
            view.xmax = MAX(nodes[i].x,view.xmax);
            view.ymax = MAX(nodes[i].y,view.ymax);
        }
        for (i=0;i<npnts;i++) {
            if ((points[i].type == 0) || (points[i].col == 0)) continue;
            view.xmin = MIN(points[i].x,view.xmin);
            view.ymin = MIN(points[i].y,view.ymin);
            view.xmax = MAX(points[i].x,view.xmax);
            view.ymax = MAX(points[i].y,view.ymax);
        }
    }
    else view.xmin = view.xmax = view.ymin = view.ymax = 0.0;
    view.xtol = 20.0*(view.xmax - view.xmin)/((float)WIDTH);
    view.xtol = MAX(view.xtol,tol);
    view.ytol = 20.0*(view.ymax - view.ymin)/((float)HEIGHT);
    view.ytol = MAX(view.ytol,tol);
    dx = 1.05*view.xmin - 0.05*view.xmax; 
    view.xmax = 1.05*view.xmax - 0.05*view.xmin; 
    dy = 1.05*view.ymin - 0.05*view.ymax; 
    view.ymax = 1.05*view.ymax - 0.05*view.ymin; 
    view.xmin = dx; view.ymin = dy;
    setbound();
}

/*  setbound   - limit window bounds        */

void setbound()
{
    float dx,dy,ax,ay,nx,ny;

    dx = view.xmax - view.xmin;
    dy = view.ymax - view.ymin;
    ax = view.xmax + view.xmin;
    ay = view.ymax + view.ymin;
    if (dx < 0.0) { 
        view.xmin = (ax+dx)/2.0; 
        view.xmax = (ax-dx)/2.0; 
        dx = -dx;
    }
    if (dy < 0.0) { 
        view.ymin = (ay+dy)/2.0; 
        view.ymax = (ay-dy)/2.0; 
        dy = -dy;
    }
    if (dx < view.xtol) { 
        dx = view.xtol; 
        view.xmin = (ax-dx)/2.0; 
        view.xmax = (ax+dx)/2.0; 
    }
    if (dy < view.ytol) { 
        dy = view.ytol; 
        view.ymin = (ay-dy)/2.0; 
        view.ymax = (ay+dy)/2.0; 
    }
    nx = dx/(float)WIDTH;
    ny = dy/(float)HEIGHT;
    if (nx > ny) dy = nx*(float)HEIGHT;
    else dx = ny*(float)WIDTH;
    view.ymin = (ay-dy)/2.0; 
    view.ymax = (ay+dy)/2.0;
    view.xmin = (ax-dx)/2.0;
    view.xmax = (ax+dx)/2.0;
}

/*  adjustfill   - make sure window bounds are legal for fills

    f       - deformation scale                     */

void adjustfill(float f)
{
    float x,y,xmin,xmax,ymin,ymax;
    float dx,dy,dxmax,dymax;
    int i,j,n,nt,nn,ni;

    dxmax = dymax = -1.e10;
    for (n=0;n<nelems;n++) {
        if (elems[n].id == 0) continue;
        nt = elems[n].type;
        nn = shapes[nt]->nn;

        xmin = ymin = 1.e10;
        xmax = ymax = -1.e10;
        for (i=0;i<nn;i++) {
            ni = nnums[elems[n].nodes[i]];
            dx = dy = 0.0;
            if (pndof > 1) {
                for (j=0;j<nsolns;j++) {
                    if (solns[j].kind == pkind) {
                        switch (solns[j].dof) {
                            case 1: dx = solns[j].u[ni]; break;
                            case 2: dy = solns[j].u[ni]; break;
                        }
                    }
                }
            }
            x = nodes[ni].x + f*dx;
            y = nodes[ni].y + f*dy;
            xmin = MIN(x,xmin); ymin = MIN(y,ymin);
            xmax = MAX(x,xmax); ymax = MAX(y,ymax);
        }
        if ((xmax < view.xmin) || (xmin > view.xmax) || (ymax < view.ymin) || (ymin > view.ymax)) continue;
        dx = xmax - xmin;
        dy = ymax - ymin;
        dxmax = MAX(dx,dxmax); dymax = MAX(dy,dymax);
    }
    view.xtol = 1.2*dxmax;
    view.ytol = 1.2*dymax;
    setbound();
}

/*  outside     - does point lie outside window ?

    x,y     - point coordinates in window space

    returns true if point outside window, false otherwise       */

int outside(float x,
            float y)
{
    if ((x < view.xmin) || 
        (x > view.xmax) || 
        (y < view.ymin) || 
        (y > view.ymax)) return(1);
    else return(0);
}

void fillresult(int is,
                int flag,
                int nr,
                float vi,
                float vf,
                FILE *hfp)
{
    extern void shape(float xi, float yi, int id, REAL *psi, REAL *dpdx, REAL *dpdy);
    int i,j,k,ix,iy,dx,dy,ip,n,ni,nt,nn,no,nv,col;
    float x[9],y[9],xx[3],yy[3],vv[3];
    float xi,yi;
    float dv,val=0.0;
    REAL psi[9],dpde[9],dpdn[9];
    struct result *results;

    adjustfill(0.0);
    if (erase) clearwindow(hfp);

    nv = 12;
    dv = (vf-vi)/(float)nv;

    results = sets[is].results;

    for (n=0;n<nelems;n++) {
        if (elems[n].id < 0) plotelem(n,0.0,0,3,hfp);
        if (elems[n].id <= 0) continue;
        if (results[n].val[0] > sets[is].max) {
            plotelem(n,0.0,0,1,hfp);
            continue;
        }

        if (getelem(n,x,y,0.0)) {
            nt = elems[n].type;
            nn = shapes[nt]->nn;
            if ((sets[is].flag != 0) && (nr > 0)) {
                no = nr*shapes[nt]->no;
                if (nt > 2) {
                    for (j=0;j<no;j++) {
                        for (i=0;i<no;i++) {
                            xi = 2.0*(float)i/no - 1.0;
                            yi = 2.0*(float)j/no - 1.0;
                            shape(xi,yi,nt,psi,dpde,dpdn);
                            xx[0] = yy[0] = vv[0] = 0.0;
                            for (k=0;k<nn;k++) {
                                xx[0] += (float)psi[k]*x[k];
                                yy[0] += (float)psi[k]*y[k];
                                vv[0] += (float)psi[k]*results[n].val[k];
                            }
                            xi = 2.0*(float)(i+1)/no - 1.0;
                            shape(xi,yi,nt,psi,dpde,dpdn);
                            xx[1] = yy[1] = vv[1] = 0.0;
                            for (k=0;k<nn;k++) {
                                xx[1] += (float)psi[k]*x[k];
                                yy[1] += (float)psi[k]*y[k];
                                vv[1] += (float)psi[k]*results[n].val[k];
                            }
                            yi = 2.0*(float)(j+1)/no - 1.0;
                            shape(xi,yi,nt,psi,dpde,dpdn);
                            xx[2] = yy[2] = vv[2] = 0.0;
                            for (k=0;k<nn;k++) {
                                xx[2] += (float)psi[k]*x[k];
                                yy[2] += (float)psi[k]*y[k];
                                vv[2] += (float)psi[k]*results[n].val[k];
                            }

                            trigrad(flag,xx,yy,vv,vi,dv,hfp);

                            xi = 2.0*(float)i/no - 1.0;
                            shape(xi,yi,nt,psi,dpde,dpdn);
                            xx[1] = yy[1] = vv[1] = 0.0;
                            for (k=0;k<nn;k++) {
                                xx[1] += (float)psi[k]*x[k];
                                yy[1] += (float)psi[k]*y[k];
                                vv[1] += (float)psi[k]*results[n].val[k];
                            }

                            trigrad(flag,xx,yy,vv,vi,dv,hfp);
                        }
                    }
                }
                else {
                    for (j=0;j<no;j++) {
                        for (i=0;i<(no-j);i++) {
                            xi = (float)i/no;
                            yi = (float)j/no;
                            shape(xi,yi,nt,psi,dpde,dpdn);
                            xx[0] = yy[0] = vv[0] = 0.0;
                            for (k=0;k<nn;k++) {
                                xx[0] += (float)psi[k]*x[k];
                                yy[0] += (float)psi[k]*y[k];
                                vv[0] += (float)psi[k]*results[n].val[k];
                            }
                            xi = (float)(i+1)/no;
                            shape(xi,yi,nt,psi,dpde,dpdn);
                            xx[1] = yy[1] = vv[1] = 0.0;
                            for (k=0;k<nn;k++) {
                                xx[1] += (float)psi[k]*x[k];
                                yy[1] += (float)psi[k]*y[k];
                                vv[1] += (float)psi[k]*results[n].val[k];
                            }
                            xi = (float)i/no;
                            yi = (float)(j+1)/no;
                            shape(xi,yi,nt,psi,dpde,dpdn);
                            xx[2] = yy[2] = vv[2] = 0.0;
                            for (k=0;k<nn;k++) {
                                xx[2] += (float)psi[k]*x[k];
                                yy[2] += (float)psi[k]*y[k];
                                vv[2] += (float)psi[k]*results[n].val[k];
                            }

                            trigrad(flag,xx,yy,vv,vi,dv,hfp);

                            if (i == (no-j-1)) continue;

                            xi = (float)(i+1)/no;
                            yi = (float)(j+1)/no;
                            shape(xi,yi,nt,psi,dpde,dpdn);
                            xx[0] = yy[0] = vv[0] = 0.0;
                            for (k=0;k<nn;k++) {
                                xx[0] += (float)psi[k]*x[k];
                                yy[0] += (float)psi[k]*y[k];
                                vv[0] += (float)psi[k]*results[n].val[k];
                            }

                            trigrad(flag,xx,yy,vv,vi,dv,hfp);
                        }
                    }
                }
            }
            else { 
                if (sets[is].flag != 0) ni = nn;
                else ni = shapes[nt]->eintg->ni;
                switch (flag) {
                    case 0:
                        val = -1.e20;
                        for (ip=0;ip<ni;ip++) val = MAX(val,results[n].val[ip]);
                        break;
                    case 1:
                        val = 1.e20;
                        for (ip=0;ip<ni;ip++) val = MIN(val,results[n].val[ip]);
                        break;
                    case 2:
                        val = 0.0;
                        for (ip=0;ip<ni;ip++) val += results[n].val[ip];
                        val /= ni;
                        break;
                }
                col = (int)((val-vi)/dv) + 4; col = MAX(col,4); 
                if (col > 15) col = 15;
                ifillelem(nt,col,x,y,hfp);
            }
            plotelem(n,0.0,0,1,hfp);
        }
    }

    switch (flag) {
        case 0: sprintf((char *)itext,"maximum %s over",funs[sets[is].func].name);
                break;
        case 1: sprintf((char *)itext,"minimum %s over",funs[sets[is].func].name);
                break;
        case 2: sprintf((char *)itext,"average %s over",funs[sets[is].func].name);
                break;
        default:sprintf((char *)itext,"%s over",funs[sets[is].func].name);
                break;
    }
    switch (sets[is].flag) {
        case 0: sprintf((char *)itext,"%s quadrature points",itext);
                break;
        case 1: sprintf((char *)itext,"%s nodes",itext);
                break;
        case 2: sprintf((char *)itext,"%s nodes (averaged)",itext);
                break;
    }
    ix = WIDTH - 12*strlen(itext) - 4;
    if (hfp) {
        fprintf(hfp,"%d %d m\n", ix+4, HEIGHT-12);
        fprintf(hfp,"(%s) tt\n",itext);
    }
    if (pkind < nprobs) {
        if (probs[pkind].title[0] != 0) {
            sprintf((char *)itext,"%s",probs[pkind].title);
            if (hfp) {
                fprintf(hfp,"%d %d m\n", 12, HEIGHT-12);
                fprintf(hfp,"(%s) tt\n",itext);
            }
        }
    }
    dx = WIDTH/40;
    dy = (HEIGHT)/14;
    ix = WIDTH - dx - 5;
    iy = HEIGHT - dy;
    if (flag == -2) val = vi-dv/2.0;
    else {
        val = vi;
        if (hfp) {
            fprintf(hfp,"%d %d m\n", ix-80, HEIGHT-iy-4);
            fprintf(hfp,"(%10.2e) tt\n",val);
        }
        sprintf((char *)itext,"%10.2e",val);
    }
    for (n=1;n<=nv;n++) {
        val += dv;
        if (hfp) {
            fprintf(hfp,"np\n%d %d m\n", ix, HEIGHT-iy);
            fprintf(hfp,"%d %d l\n", ix+dx, HEIGHT-iy);
            fprintf(hfp,"%d %d l\n", ix+dx, HEIGHT-iy+dy);
            fprintf(hfp,"%d %d l\n", ix, HEIGHT-iy+dy);
            fprintf(hfp,"cp ");
            hardcol(n+3,1,hfp);
            fprintf(hfp,"0 gd\n");
            if (flag == -2) fprintf(hfp,"%d %d m\n", ix-80, HEIGHT-iy+dy/2-4);
            else fprintf(hfp,"%d %d m\n", ix-80, HEIGHT-iy+dy-4);
            fprintf(hfp,"(%10.2e) tt\n",val);
        }
        sprintf((char *)itext,"%10.2e",val);
        iy -= dy;
    }
}

void trigrad(int flag,
             float *xx, 
             float *yy, 
             float *vv, 
             float vi,
             float dv,
             FILE *hfp )
{
    int i,ci,cf;
    int nn[3];
    float a,b,c,e1,e2,e3,f,v,vmin,vmax;
    float x[4],y[4];

    if(vv[0] > vv[1]) {
        if (vv[0] > vv[2]) {
            nn[0] = 0; 
            if(vv[1] > vv[2]) { nn[1] = 1; nn[2] = 2; }
            else { nn[1] = 2; nn[2] = 1; }
        }
        else {
            nn[0] = 2; nn[1] = 0; nn[2] = 1;
        }
    }
    else {
        if (vv[1] > vv[2]) {
            nn[0] = 1; 
            if(vv[0] > vv[2]) { nn[1] = 0; nn[2] = 2; }
            else { nn[1] = 2; nn[2] = 0; }
        }
        else {
            nn[0] = 2; nn[1] = 1; nn[2] = 0;
        }
    }

    a = vv[nn[2]]*(yy[nn[0]]-yy[nn[1]])-vv[nn[1]]*(yy[nn[0]]-yy[nn[2]])+vv[nn[0]]*(yy[nn[1]]-yy[nn[2]]);
    b = vv[nn[2]]*(xx[nn[0]]-xx[nn[1]])-vv[nn[1]]*(xx[nn[0]]-xx[nn[2]])+vv[nn[0]]*(xx[nn[1]]-xx[nn[2]]);
    c = (yy[nn[0]]-yy[nn[2]])*(xx[nn[1]]-xx[nn[2]])-(xx[nn[0]]-xx[nn[2]])*(yy[nn[1]]-yy[nn[2]]);
    e1 = b*(yy[nn[0]]-yy[nn[2]])-a*(xx[nn[0]]-xx[nn[2]]);
    e2 = b*(yy[nn[1]]-yy[nn[2]])-a*(xx[nn[1]]-xx[nn[2]]);
    e3 = b*(yy[nn[0]]-yy[nn[1]])-a*(xx[nn[0]]-xx[nn[1]]);

    if (flag == -2) {
        vmin = ((int)((vv[nn[2]]-vi)/dv-0.5))*dv + vi + dv/2.0;
        vmax = ((int)((vv[nn[0]]-vi)/dv-0.5))*dv + vi + dv/2.0;
        if (vmin <= vv[nn[2]]) vmin += dv;
        if (vmax >= vv[nn[0]]) vmax -= dv;
        ci = (int)((vmax-vi)/dv) + 4; 
        ci = MAX(ci,4); ci = MIN(ci,15);
        cf = (int)((vmin-vi)/dv) + 3; 
        cf = MAX(cf,3); cf = MIN(cf,15);
    }
    else {
        vmin = ((int)((vv[nn[2]]-vi)/dv))*dv + vi;
        vmax = ((int)((vv[nn[0]]-vi)/dv))*dv + vi;
        if (vmax >= vv[nn[0]]) vmax -= dv;
        ci = (int)((vv[nn[0]]-vi)/dv) + 4; 
        ci = MAX(ci,4); ci = MIN(ci,15);
        cf = (int)((vv[nn[2]]-vi)/dv) + 4; 
        cf = MAX(cf,4); cf = MIN(cf,15);
        fillpoly(3,xx,yy,ci,hfp);
    }

    for(v = vmax; v > vi; v -= dv) {
        if (ci <= cf) break;
        i = 0;
        x[i] = xx[nn[2]]; 
        y[i] = yy[nn[2]];
        i++;
        if (e1 != 0) {
            f = c*(v-vv[nn[2]])/e1;
            x[i] = f*(xx[nn[0]]-xx[nn[2]]) + xx[nn[2]];
            y[i] = f*(yy[nn[0]]-yy[nn[2]]) + yy[nn[2]];
            i++;
        }
        if (e2 != 0) {
            f = c*(v-vv[nn[2]])/e2;
            if ((f >= 0) && (f <= 1)) {
                x[i] = f*(xx[nn[1]]-xx[nn[2]]) + xx[nn[2]];
                y[i] = f*(yy[nn[1]]-yy[nn[2]]) + yy[nn[2]];
                i++;
            }
        }
        if (i < 3) {
            if (e3 != 0) {
                f = c*(v-vv[nn[2]])/e3;
                x[i] = f*(xx[nn[0]]-xx[nn[1]]) + b*c/e3 + xx[nn[2]];
                y[i] = f*(yy[nn[0]]-yy[nn[1]]) + a*c/e3 + yy[nn[2]];
                i++;
            }
            x[i] = xx[nn[1]]; 
            y[i] = yy[nn[1]];
            i++;
        }
        if (i > 2) {
            if (flag != -2) fillpoly(i,x,y,--ci,hfp);
            else {
                plot(x[1],y[1],0,hfp);
                plot(x[2],y[2],1,hfp);
                if (hfp) hardcol(ci--,0,hfp);
            }
        }
        else break;
    }
}

void fillpoly(int n, 
              float *xx, 
              float *yy, 
              int col, 
              FILE *hfp)
{
    long ix[9],iy[9];
    int i;

    for (i=0;i<n;i++) {
        ix[i] = xtoix(xx[i]);
        iy[i] = ytoiy(yy[i]);
    }

    ifillpoly(ix,iy,n,hfp);
    if (hfp) {
        fprintf(hfp,"cp ");
        hardcol(col,1,hfp);
    }
}

void plotresult(int is,
                int iv,
                int nr,
                float vi,
                float vf,
                FILE *hfp)
{
    extern void shape(float xi, float yi, int id, REAL *psi, REAL *dpdx, REAL *dpdy);
    int i,j,k,ix,iy,dx,dy,ip,n,ni,nt,nn,no,nv,col;
    float x[9],y[9];
    float xi,yi,x1,y1;
    float dv,val,dir;
    REAL *ps,*de,*dn;
    REAL psi[9],dpde[9],dpdn[9];
    struct result *results,*vectors;

    if (erase) clearwindow(hfp);

    nv = 12;
    dv = (vf-vi)/(float)nv;

    results = sets[is].results;
    vectors = sets[iv].results;

    for (n=0;n<nelems;n++) {
        if (elems[n].id < 0) plotelem(n,0.0,0,3,hfp);
        if (elems[n].id <= 0) continue;

        if (getelem(n,x,y,0.0)) {
            nt = elems[n].type;
            nn = shapes[nt]->nn;
            if ((sets[is].flag != 0) && (sets[iv].flag != 0) && (nr > 0)) {
                no = nr*shapes[nt]->no;
                if (nt > 2) {
                    for (j=0;j<no;j++) {
                        for (i=0;i<no;i++) {
                            xi = (float)(2*i + 1)/no - 1.0;
                            yi = (float)(2*j + 1)/no - 1.0;
                            shape(xi,yi,nt,psi,dpde,dpdn);
                            val = 0.0;
                            x1 = y1 = 0.0;
                            xi = yi = 0.0;
                            for (k=0;k<nn;k++) {
                                dir = vectors[n].val[k]*PI/180.0;
                                xi += (float)psi[k]*cos((double)dir);
                                yi += (float)psi[k]*sin((double)dir);
                                val += (float)psi[k]*results[n].val[k];
                                x1 += (float)psi[k]*x[k];
                                y1 += (float)psi[k]*y[k];
                            }
                            dir = (float)atan2((double)yi,(double)xi);
                            col = (int)((val-vi)/dv) + 4; col = MAX(col,4); 
                            col = MIN(col,15);
                            ix = xtoix(x1);
                            iy = ytoiy(y1);
                            plotsym(ix,iy,6L,3L,-6L,0L,3L,25L,col,dir,hfp);
                        }
                    }
                }
                else {
                    for (j=0;j<no;j++) {
                        for (i=0;i<(no-j);i++) {
                            xi = ((float)i + 0.3333333)/no;
                            yi = ((float)j + 0.3333333)/no;
                            shape(xi,yi,nt,psi,dpde,dpdn);
                            val = 0.0;
                            x1 = y1 = 0.0;
                            xi = yi = 0.0;
                            for (k=0;k<nn;k++) {
                                dir = vectors[n].val[k]*PI/180.0;
                                xi += (float)psi[k]*cos((double)dir);
                                yi += (float)psi[k]*sin((double)dir);
                                val += (float)psi[k]*results[n].val[k];
                                x1 += (float)psi[k]*x[k];
                                y1 += (float)psi[k]*y[k];
                            }
                            dir = (float)atan2((double)yi,(double)xi);
                            col = (int)((val-vi)/dv) + 4; col = MAX(col,4); 
                            col = MIN(col,15);
                            ix = xtoix(x1);
                            iy = ytoiy(y1);
                            plotsym(ix,iy,6L,3L,-6L,0L,3L,25L,col,dir,hfp);

                            if (i == (no-j-1)) continue;

                            xi = ((float)i + 0.6666667)/no;
                            yi = ((float)j + 0.6666667)/no;
                            shape(xi,yi,nt,psi,dpde,dpdn);
                            val = 0.0;
                            x1 = y1 = 0.0;
                            xi = yi = 0.0;
                            for (k=0;k<nn;k++) {
                                dir = vectors[n].val[k]*PI/180.0;
                                xi += (float)psi[k]*cos((double)dir);
                                yi += (float)psi[k]*sin((double)dir);
                                val += (float)psi[k]*results[n].val[k];
                                x1 += (float)psi[k]*x[k];
                                y1 += (float)psi[k]*y[k];
                            }
                            dir = (float)atan2((double)yi,(double)xi);
                            col = (int)((val-vi)/dv) + 4; col = MAX(col,4); 
                            col = MIN(col,15);
                            ix = xtoix(x1);
                            iy = ytoiy(y1);
                            plotsym(ix,iy,6L,3L,-6L,0L,3L,25L,col,dir,hfp);
                        }
                    }
                }
            }
            else { 
                if (sets[is].flag != 0) {
                    for (i=0;i<nn;i++) {
                        val = results[n].val[i];
                        dir = vectors[n].val[i]*PI/180.0;
                        col = (int)((val-vi)/dv) + 4; col = MAX(col,4); 
                        col = MIN(col,15);
                        ix = xtoix(x[i]);
                        iy = ytoiy(y[i]);
                        plotsym(ix,iy,6L,3L,-6L,0L,3L,25L,col,dir,hfp);
                    }
                }
                else {
                    ni = shapes[nt]->eintg->ni; 
                    if (!shapes[nt]->flag) {
                        for (i=0;i<ni;i++) {
                            xi = shapes[nt]->eintg->xip[i];
                            yi = shapes[nt]->eintg->yip[i];
                            ps = shapes[nt]->psi + i*nn;
                            de = shapes[nt]->dpde + i*nn;
                            dn = shapes[nt]->dpdn + i*nn;
                            shape(xi,yi,nt,ps,de,dn);
                        }
                        shapes[nt]->flag = 1;
                    }
                    for (ip=0;ip<ni;ip++) {
                        ps = shapes[nt]->psi + ip*nn;
                        val = results[n].val[ip];
                        dir = vectors[n].val[ip]*PI/180.0;
                        x1 = y1 = 0.0;
                        for (k=0;k<nn;k++) {
                            x1 += (float)ps[k]*x[k];
                            y1 += (float)ps[k]*y[k];
                        }
                        col = (int)((val-vi)/dv) + 4; col = MAX(col,4); 
                        col = MIN(col,15);
                        ix = xtoix(x1);
                        iy = ytoiy(y1);
                        plotsym(ix,iy,6L,3L,-6L,0L,3L,25L,col,dir,hfp);
                    }
                }
            }
            plotelem(n,0.0,0,1,hfp);
        }
    }

    sprintf((char *)itext,"%s over",funs[sets[is].func].name);
    switch (sets[is].flag) {
        case 0: sprintf((char *)itext,"%s quadrature points",itext);
                break;
        case 1: sprintf((char *)itext,"%s nodes",itext);
                break;
        case 2: sprintf((char *)itext,"%s nodes (averaged)",itext);
                break;
    }
    ix = WIDTH - 12*strlen(itext) - 4;
    if (hfp) {
        fprintf(hfp,"%d %d m\n", ix+4, HEIGHT-12);
        fprintf(hfp,"(%s) tt\n",itext);
    }
    if (pkind < nprobs) {
        if (probs[pkind].title[0] != 0) {
            sprintf((char *)itext,"%s",probs[pkind].title);
            if (hfp) {
                fprintf(hfp,"%d %d m\n", 12, HEIGHT-12);
                fprintf(hfp,"(%s) tt\n",itext);
            }
        }
    }
    dx = WIDTH/40;
    dy = (HEIGHT)/14;
    ix = WIDTH - dx - 5;
    iy = HEIGHT - dy;
    val = vi;
    if (hfp) {
        fprintf(hfp,"%d %d m\n", ix-80, HEIGHT-iy-4);
        fprintf(hfp,"(%10.2e) tt\n",val);
    }
    sprintf((char *)itext,"%10.2e",val);
    for (n=1;n<=nv;n++) {
        val += dv;
        if (hfp) {
            fprintf(hfp,"np\n%d %d m\n", ix, HEIGHT-iy);
            fprintf(hfp,"%d %d l\n", ix+dx, HEIGHT-iy);
            fprintf(hfp,"%d %d l\n", ix+dx, HEIGHT-iy+dy);
            fprintf(hfp,"%d %d l\n", ix, HEIGHT-iy+dy);
            fprintf(hfp,"cp ");
            hardcol(n+3,1,hfp);
            fprintf(hfp,"0 gd\n");
            fprintf(hfp,"%d %d m\n", ix-80, HEIGHT-iy+dy-4);
            fprintf(hfp,"(%10.2e) tt\n",val);
        }
        sprintf((char *)itext,"%10.2e",val);
        iy -= dy;
    }
}

void writeclose(FILE *hfp)
{
    fprintf(hfp,"%%%%EOF\n");
}

void writeprolog(FILE *hfp)
{
    int h;

    h = (468*HEIGHT)/WIDTH + 72;

    fprintf(hfp,"%%!PS-Adobe-2.0 EPSF-1.2\n");
    fprintf(hfp,"%%%%BoundingBox: 72 72 540 %d\n",h);
    fprintf(hfp,"%%%%Creator: FEM2D\n");
    fprintf(hfp,"%%%%Title: \n");
    fprintf(hfp,"%%%%CreationDate: %s\n",__DATE__);
    fprintf(hfp,"%%%%DocumentFonts: \n");
    fprintf(hfp,"%%%%EndComments\n");
    fprintf(hfp,"\n");
    fprintf(hfp,"/bdef {bind def} def\n");
    fprintf(hfp,"/np {newpath} bdef\n");
    fprintf(hfp,"/cp {closepath} bdef\n");
    fprintf(hfp,"/gs {gsave} bdef\n");
    fprintf(hfp,"/gr {grestore} bdef\n");
    fprintf(hfp,"/cf {setrgbcolor gsave fill grestore} bdef\n");
    fprintf(hfp,"/cd {setrgbcolor gsave stroke grestore} bdef\n");
    fprintf(hfp,"/gf {setgray gsave fill grestore} bdef\n");
    fprintf(hfp,"/gd {setgray stroke} bdef\n");
    fprintf(hfp,"/nl {0 setlinewidth} bdef\n");
    fprintf(hfp,"/tl {1 setlinewidth} bdef\n");
    fprintf(hfp,"/w {1 setgray} bdef\n");
    fprintf(hfp,"/b {0 setgray} bdef\n");
    fprintf(hfp,"/m {moveto} bdef\n");
    fprintf(hfp,"/r {rmoveto} bdef\n");
    fprintf(hfp,"/l {lineto} bdef\n");
    fprintf(hfp,"/cl {np xmin ymin m xmax ymin l xmax ymax l xmin ymax l cp} bdef\n");
    fprintf(hfp,"/grp {/Courier findfont [12 0 0 14 0 0] makefont} bdef\n");
    fprintf(hfp,"/t {gs currentpoint translate grp setfont show gr} bdef\n");
    fprintf(hfp,"/tt {dup -1 -1 r w t dup 2 0 r t dup 0 2 r t dup -2 0 r t 1 -1 r b t} bdef\n");
    fprintf(hfp,"\n");
    fprintf(hfp,"/init {gr gs\n");
    fprintf(hfp,"   /ymax exch def\n");
    fprintf(hfp,"   /ymin exch def\n");
    fprintf(hfp,"   /xmax exch def\n");
    fprintf(hfp,"   /xmin exch def\n");
    fprintf(hfp,"   /sc 468 xmax xmin sub div def\n");
    fprintf(hfp,"   72 dup translate\n");
    fprintf(hfp,"   sc dup scale\n");
    fprintf(hfp,"   xmin neg ymin neg translate\n");
    fprintf(hfp,"   tl\n");
    fprintf(hfp,"   cl clip\n");
    fprintf(hfp,"} bdef\n");
    fprintf(hfp,"\n");
    fprintf(hfp,"%%%%Page:\ngs\n0 %d 0 %d init\n",WIDTH,HEIGHT);
}

void hardcol(int col, int flag, FILE *hfp)
{
    if (flag) {
        if (col > 3) fprintf(hfp,"%f %f %f cf\n",
            (float)colors[col-4].Red/15.0,
            (float)colors[col-4].Green/15.0,
            (float)colors[col-4].Blue/15.0);
        else fprintf(hfp,"%f gf\n",(1-(col%2))*.5);
    }
    else {
        if (col > 3) fprintf(hfp,"%f %f %f cd\n",
            (float)colors[col-4].Red/15.0,
            (float)colors[col-4].Green/15.0,
            (float)colors[col-4].Blue/15.0);
        else fprintf(hfp,"%f gd\n",(1-(col%2))*.5);
    }
}

void showpage(FILE *hfp)
{
    fprintf(hfp,"showpage\n");
}

/*  plotdraw - plot draw annotations */

void plotdraw(FILE *hfp)
{
    int n;

    setbound();

    for (n=0;n<nlins;n++) { 
        if (lines[n].type != 0) plotline(n,1,hfp);
    }

    for (n=0;n<npnts;n++) { 
        if (points[n].type != 0) plotpoint(n,points[n].col,hfp);
    }

    for (n=0;n<nlabs;n++) { 
        if (labels[n].type != 0) plotlabel(n,1,hfp);
    }
}

/*  plotmodel   - line plot model   */

void plotmodel(FILE *hfp)
{
    int i,n;

    if (erase) clearwindow(hfp);
    setbound();

    for (n=0;n<nelems;n++) { 
        if (elems[n].id > 0) {
            for (i=0;i<shapes[elems[n].type]->nn;i++)
                plotnode(nnums[elems[n].nodes[i]],0,2,hfp);
            plotelem(n,0.0,0,1,hfp);
        }
        else if (elems[n].id < 0) plotelem(n,0.0,0,3,hfp);
    }
    for (n=0;n<nnodes;n++) {
        if (nodes[n].id < 0) plotnode(n,0,2,hfp);
    }
}

/*  fillmodel   - fill plot model   */

void fillmodel(FILE *hfp)
{
    int n;

    if (erase) clearwindow(hfp);
    adjustfill(0.0);

    for (n=0;n<nelems;n++) {
        if (elems[n].id > 0) fillelem(n,0.0,1,hfp);
        else if (elems[n].id < 0) fillelem(n,0.0,3,hfp);
    }
}

/*  plotdeform   - line plot deformed model   */

void plotdeform(float f,
                FILE *hfp)
{
    int n;

    setbound();

    if (hfp) fprintf(hfp,"tl\n");
    for (n=0;n<nelems;n++) { 
        if (elems[n].id != 0) plotelem(n,f,0,2,hfp);
    }
    if (hfp) fprintf(hfp,"nl\n");
}

/*  filldeform   - fill plot deformed model   */

void filldeform(float f,
                FILE *hfp)
{
    int n;

    adjustfill(f);

    for (n=0;n<nelems;n++) { 
        if (elems[n].id != 0) fillelem(n,f,1,hfp);
    }
}

/*  plotbound   - plot model with boundary  */

void plotbound()
{
    float x[9],y[9];
    int i,j,k,l,i1,i2,j1,j2,k1,k2,n,nt,nn,ns,m,mt,mn,ms,p;

    if (erase) clearwindow(NULL);
    setbound();

    for (n=0;n<nnodes;n++) {
        if (nodes[n].id != 0) plotnode(n,0,2,NULL);
    }

    for (n=0;n<nelems;n++) { 
        if (elems[n].id != 0) {
            if (getelem(n,x,y,0.0)) {
                nt = elems[n].type;
                nn = shapes[nt]->nn;
                ns = shapes[nt]->ns;
                p = 0;
                for (i=0;i<ns;i++) {
                    plot(x[i],y[i],p,NULL);
                    p = 1;
                    if (nn > ns) plot(x[i+ns],y[i+ns],p,NULL);
                }
                plot(x[0],y[0],p,NULL);
                for (m=0;m<nelems;m++) { 
                    if ((elems[m].id != 0) && (m != n)) {
                        mt = elems[m].type;
                        mn = shapes[mt]->nn;
                        ms = shapes[mt]->ns;
                        for (i=0;i<ns;i++) {
                            k = shapes[nt]->i[i];
                            l = shapes[nt]->j[i];
                            i1 = elems[n].nodes[k];
                            j1 = elems[n].nodes[l];
                            if (nn > ns) k1 = elems[n].nodes[k+ns];
                            else k1 = -1;
                            for (j=0;j<ms;j++) {
                                i2 = elems[m].nodes[shapes[mt]->i[j]];
                                j2 = elems[m].nodes[shapes[mt]->j[j]];
                                if (mn > ms) 
                                    k2 = elems[m].nodes[shapes[mt]->i[j]+ms];
                                else k2 = -1;
                                if (((i1==i2)||(i1==j2))&&((j1==i2)||(j1==j2))&&(k1==k2)) {
                                    plot(x[k],y[k],0,NULL);
                                    if (nn > ns) plot(x[k+ns],y[k+ns],1,NULL);
                                    plot(x[l],y[l],1,NULL);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

/*  plotpbc     - plot a point boundary condition  */

void plotpbc(int n,
             int p,
             FILE *hfp)
{
    int pcol;
    float x,y,ai;
    long ix,iy;

    x = nodes[nnums[pbcs[n].id]].x;
    y = nodes[nnums[pbcs[n].id]].y;

    if (outside(x,y)) return;
    ix = xtoix(x);
    iy = ytoiy(y);

    if (pbcs[n].kind != pkind) return;
    pcol = 0;
    switch (pbcs[n].type) {
        case 1: switch (pndof) {
                    case 1: if (p) pcol = 11;
                            plotsym(ix,iy,3L,3L,0L,0L,4L,0L,pcol,0.0,hfp);
                            break;

                    default:switch (pbcs[n].dof) {
                                case 1: if (pbcs[n].val < 0.0) ai = PI;
                                        else ai = 0.0;
                                        if (p) pcol = 9;
                                        plotsym(ix,iy,6L,3L,-6L,0L,3L,0,pcol,ai,hfp);
                                        break;

                                case 2: if (pbcs[n].val < 0.0) ai = -PI/2.0;
                                        else ai = PI/2.0;
                                        if (p) pcol = 9;
                                        plotsym(ix,iy,6L,3L,-6L,0L,3L,0,pcol,ai,hfp);
                                        break;
                                default:if (p) pcol = 11;
                                        plotsym(ix,iy,3L,3L,0L,0L,4L,0L,pcol,0.0,hfp);
                                        break;

                            }
                            break;
                }
                break;
        case 2: switch (pndof) {
                    case 1: if (p) pcol = 15;
                            plotcross(ix,iy,pcol,hfp);
                            break;

                    default:switch (pbcs[n].dof) {
                                case 1: if (pbcs[n].val < 0.0) ai = PI;
                                        else ai = 0.0;
                                        if (p) pcol = 4;
                                        plotsym(ix,iy,6L,3L,-6L,0L,3L,25,pcol,ai,hfp);
                                        break;

                                case 2: if (pbcs[n].val < 0.0) ai = -PI/2.0;
                                        else ai = PI/2.0;
                                        if (p) pcol = 4;
                                        plotsym(ix,iy,6L,3L,-6L,0L,3L,25,pcol,ai,hfp);
                                        break;
                                default:if (p) pcol = 15;
                                        plotcross(ix,iy,pcol,hfp);
                                        break;

                            }
                            break;
                }
                break;
        case 3: switch (pndof) {
                    case 1: if (p) pcol = 4;
                            plotcross(ix,iy,pcol,hfp);
                            break;

                    default:switch (pbcs[n].dof) {
                                case 1: ai = 0.0;
                                        if (p) pcol = 9;
                                        plotsym(ix,iy,6L,0L,0L,0L,2L,0,pcol,ai,hfp);
                                        break;

                                case 2: ai = PI/2.0;
                                        if (p) pcol = 9;
                                        plotsym(ix,iy,6L,0L,0L,0L,2L,0,pcol,ai,hfp);
                                        break;
                                default:if (p) pcol = 4;
                                        plotcross(ix,iy,pcol,hfp);
                                        break;

                            }
                            break;
                }
                break;
    }
}


/*  plotdbc     - plot a distributed boundary condition  */

void plotdbc(int n,
             int p,
             FILE *hfp)
{
    int pcol;

    if (dbcs[n].kind != pkind) return;
    pcol = 0;
    switch (dbcs[n].type) {
        case 1: switch (pndof) {
                    case 1: if (p) pcol = 11;
                            break;

                    default:switch (dbcs[n].dof) {
                                case 1:
                                case 2: if (p) pcol = 9;
                                        break;
                                default:if (p) pcol = 11;
                                        break;

                            }
                            break;
                }
                plotdistr(n,pcol,hfp);
                break;

        case 2: switch (pndof) {
                    case 1: if (p) pcol = 15;
                            break;

                    default:switch (dbcs[n].dof) {
                                case 1:
                                case 2: if (p) pcol = 4;
                                        break;
                                default:if (p) pcol = 15;
                                        break;
                            }
                            break;
                }
                plotdistr(n,pcol,hfp);
                break;
    }
}

/*  plotdistr   - plot a distributed load boundary condition

    n           - distributed boundary condition number         */

void plotdistr(int n,
               int pcol,
               FILE *hfp)
{
    float xi,x,y,ai;
    REAL r,dxde,dyde,dval;
    REAL psi[4],dpsi[4];
    long ix,iy;
    int i,ip,is,ne,nt,ns,nn,ni,nm,nds[4];
    
    if (n < 0) return;
    ne = enums[dbcs[n].id];
    nt = elems[ne].type;
    ni = shapes[nt]->no;
    ns = shapes[nt]->ns;
    nn = ni + 1;
    nm = ni - 1;
    nds[0] = is = dbcs[n].side - 1;
    for (i=0;i<nm;i++) nds[i+1] = ns+is*nm+i;
    if (is < ns-1) nds[ni] = is+1;
    else nds[ni] = 0;
    for (ip=0;ip<ni;ip++) {
        xi = xip[ni-1][ip];
        sshape(xi,ni,psi,dpsi);
        dxde = dyde = dval = 0.0;
        x = y = 0.0;
        for (i=0;i<nn;i++) {
            if (ptype == 1) r = (REAL)nodes[nnums[elems[ne].nodes[nds[i]]]].x;
            else r = 1.0;
            x += (float)psi[i]*nodes[nnums[elems[ne].nodes[nds[i]]]].x;
            y += (float)psi[i]*nodes[nnums[elems[ne].nodes[nds[i]]]].y;
            dxde += dpsi[i]*nodes[nnums[elems[ne].nodes[nds[i]]]].x;
            dyde += dpsi[i]*nodes[nnums[elems[ne].nodes[nds[i]]]].y;
            dval += psi[i]*r*dbcs[n].val[i];
        }

        ix = xtoix(x);
        iy = ytoiy(y);

        switch (pndof) {
            case 1: plotcross(ix,iy,pcol,hfp); break;
            default:switch (dbcs[n].dof) {
                        case 1: if (dval < 0.0) ai = (float)atan2((double)dxde,-(double)dyde);
                                else ai = (float)atan2(-(double)dxde,(double)dyde);
                                switch (dbcs[n].type) {
                                    case 1: plotsym(ix,iy,6L,3L,-6L,0L,3L,0L,pcol,ai,hfp);
                                            break;
                                    default:plotsym(ix,iy,6L,3L,-6L,0L,3L,25L,pcol,ai,hfp);
                                            break;
                                }
                                break;
                        case 2: if (dval < 0.0) ai = (float)atan2(-(double)dyde,(double)dxde);
                                else ai = (float)atan2((double)dyde,-(double)dxde);
                                switch (dbcs[n].type) {
                                    case 1: plotsym(ix,iy,6L,3L,0L,6L,3L,0L,pcol,ai,hfp);
                                            break;
                                    default:plotsym(ix,iy,6L,3L,0L,6L,3L,25L,pcol,ai,hfp);
                                            break;
                                }
                                break;
                        default:plotcross(ix,iy,pcol,hfp); break;
                    }
                    break;
        }
    }
}

/*  sshape  - Finite element shape function routine based on actual shape 
              function definition.  Consequently, any order shape function 
              may be specified.

    xi      - value where shape function values/derivs are calculated
    n       - order of the shape function
    psi     - array of shape function values
    dpsi    - array of shape function derivatives               */

void sshape(float xi,
            int n,
            REAL *psi,
            REAL *dpsi)
{
    REAL f,d,dp;
    int i,j,k;

    f = 2.0/(REAL)n;
    for (i=0;i<=n;i++) {
        psi[i] = d = 1.0; dpsi[i] = 0.0;
        for (j=0;j<=n;j++) {
            if (j != i) {
                psi[i] *= ((REAL)xi - j*f + 1.0);
                dp = 1.0;
                for (k=0;k<=n;k++) {
                    if ((k != i) && (k != j)) dp *= ((REAL)xi - k*f + 1.0);
                }
                dpsi[i] += dp;
                d *= (i-j)*f;
            }
        }
        psi[i] /= d;
        dpsi[i] /= d;
    }
}

/*  plotdot - plots a dot !!!

    ix,iy   - coordinates in screen space           */

void plotdot(long ix,
             long iy,
             FILE *hfp)
{
    if (hfp) {
        fprintf(hfp,"np\n%ld %ld m\n", ix+1, HEIGHT - iy-1);
        fprintf(hfp,"%ld %ld l\n", ix+1, HEIGHT - iy+1);
        fprintf(hfp,"%ld %ld l\n", ix-1, HEIGHT - iy+1);
        fprintf(hfp,"%ld %ld l cp 0 gd\n", ix-1, HEIGHT - iy-1);
    }
}

/*  plotcross - plots cross hair

    ix,iy   - coordinates in screen space           */

void plotcross(long ix,
               long iy,
               int pcol,
               FILE *hfp)
{
    if (hfp) {
        fprintf(hfp,"np\n%ld %ld m\n", ix+5, HEIGHT - iy);
        fprintf(hfp,"%ld %ld l ", ix-5, HEIGHT - iy);
        hardcol(pcol,0,hfp);
        fprintf(hfp,"np\n%ld %ld m\n", ix, HEIGHT - iy-5);
        fprintf(hfp,"%ld %ld l ", ix, HEIGHT - iy+5);
        hardcol(pcol,0,hfp);
    }
}

/*  plot number routine

    n       - number
    ix,iy   - pixel coordinates             */

void plotnumber(int n,
                long ix,
                long iy,
                FILE *hfp)
{
    if (hfp) {
        fprintf(hfp,"%ld %ld m\n", ix, HEIGHT - iy - 4);
        fprintf(hfp,"(%d) t\n",n);
    }
    sprintf((char *)itext,"%d ",n);
}

/*  plot symbol routine

    ix,iy   - pixel coordinates of head of symbol
    ia,ib   - major and minor radii
    ox      - offset (in unrotated x-direction)
    oy      - offset (in unrotated y-direction)
    n       - number of vertices for symbol
    l       - length of trailing line
    ai      - define normal direction (symbol will be rotated to coincide) */

void plotsym(long ix,
             long iy,
             long ia,
             long ib,
             long ox,
             long oy,
             long n,
             long l,
             long pcol,
             float ai,
             FILE *hfp)
{
    int i,p;
    long nx,ny;
    double x,y,ang;

    if (l) {
        x = (double)ox;
        y = (double)oy;
        nx = (long)(x*cos((double)ai) + y*sin((double)ai)) + ix;
        ny = (long)(y*cos((double)ai) - x*sin((double)ai)) + iy;
        if (hfp) fprintf(hfp,"np\n%ld %ld m\n", nx, HEIGHT - ny);
        x = -(double)l + (double)ox;
        y = (double)oy;
        nx = (long)(x*cos((double)ai) + y*sin((double)ai)) + ix;
        ny = (long)(y*cos((double)ai) - x*sin((double)ai)) + iy;
        if (hfp) {
            fprintf(hfp,"%ld %ld l ", nx, HEIGHT - ny);
            hardcol(pcol,0,hfp);
        }
    }
    if (n) {
        if (n > 1) {
            p = 0;
            for (i=0;i<n;i++) {
                ang = (double)i*PI*2/n;
                x = (double)ia*cos(ang) + (double)ox;
                y = (double)ib*sin(ang) + (double)oy;
                nx = (long)(x*cos((double)ai) + y*sin((double)ai)) + ix;
                ny = (long)(y*cos((double)ai) - x*sin((double)ai)) + iy;
                if (hfp) {
                    if (p) fprintf(hfp,"%ld %ld l\n", nx, HEIGHT - ny);
                    else fprintf(hfp,"np\n%ld %ld m\n", nx, HEIGHT - ny);
                }
                p = 1;
            }
            if (hfp) {
                fprintf(hfp,"cp ");
                hardcol(pcol,1,hfp);
            }
        }
        else {
            if (hfp) {
                fprintf(hfp,"np\n%ld %ld m\n", ix, HEIGHT - iy);
                fprintf(hfp,"%ld %ld l ", ix, HEIGHT - iy);
                hardcol(pcol,0,hfp);
            }
        }
    }
}

/*  plotnode    - plot a node

    n           - node order number
    is          - symbol size             */

void plotnode(int n,
              int is,
              int col,
              FILE *hfp)
{
    float x,y;
    long ix,iy;

    x = nodes[n].x;
    y = nodes[n].y;

    if (!outside(x,y)) {
        ix = xtoix(x);
        iy = ytoiy(y);
        plotdot(ix,iy,hfp);
        if (is) plotnumber(ABS(nodes[n].id),ix+2L,iy+2L,hfp);
    }
}

/*  plotelem    - plot an element

    n           - element order number
    f           - deformation scale                 */

void plotelem(int n,
              float f,
              int is,
              int col,
              FILE *hfp)
{
    float x[9],y[9],xa,ya;
    int i,nt,nn,ns,p;
    long ix,iy;

    if (getelem(n,x,y,f)) {
        nt = elems[n].type;
        nn = shapes[nt]->nn;
        ns = shapes[nt]->ns;
        xa = ya = 0.0;
        p = 0;
        for (i=0;i<ns;i++) {
            xa += x[i]; ya += y[i];
            plot(x[i],y[i],p,hfp);
            p = 1;
            if (nn > ns) plot(x[i+ns],y[i+ns],p,hfp);
        }
        if (hfp) {
            fprintf(hfp,"cp ");
            hardcol(col,0,hfp);
        }
        plot(x[0],y[0],p,NULL);
        if (is) {
            xa /= ns; ya /= ns;
            ix = xtoix(xa);
            iy = ytoiy(ya);
            plotnumber(ABS(elems[n].id),ix,iy,hfp);
        }
    }
}

/*  fillelem    - fill plot an element

    n           - element order number
    f           - deformation scale                 */

void fillelem(int n,
              float f,
              int col,
              FILE *hfp)
{
    float x[9],y[9];
    int i,nt,nm,nn,ns,p;

    if (getelem(n,x,y,f)) {
        nt = elems[n].type;
        nn = shapes[nt]->nn;
        ns = shapes[nt]->ns;
        if (mmax > 1) {
            if (f > 0.0) nm = 16 - ((float)elems[n].mat/(mmax-1))*12;
            else nm = ((float)elems[n].mat/(mmax-1))*12 + 3;
        }
        else nm = (elems[n].mat%12) + 3;

        ifillelem(nt,nm,x,y,hfp);

        p = 0;
        for (i=0;i<ns;i++) {
            plot(x[i],y[i],p,hfp);
            p = 1;
            if (nn > ns) plot(x[i+ns],y[i+ns],p,hfp);
        }
        if (hfp) {
            fprintf(hfp,"cp ");
            hardcol(col,0,hfp);
        }
        plot(x[0],y[0],p,NULL);
    }
}

void ifillelem(int nt, int col, float *x, float *y, FILE *hfp)
{
    int i,nn,ns;
    long ix[9],iy[9];

    ns = shapes[nt]->ns;
    nn = ns*shapes[nt]->no;
    for (i=0;i<ns;i++) {
        if (nn > ns) {
            ix[i<<1] = xtoix(x[i]);
            iy[i<<1] = ytoiy(y[i]);
            ix[(i<<1)+1] = xtoix(x[i+ns]);
            iy[(i<<1)+1] = ytoiy(y[i+ns]);
        }
        else {
            ix[i] = xtoix(x[i]);
            iy[i] = ytoiy(y[i]);
        }
    }

    ifillpoly(ix,iy,nn,hfp);
    if (hfp) {
        fprintf(hfp,"cp ");
        hardcol(col,1,hfp);
        fprintf(hfp,"0 gd\n");
    }
}

void ifillpoly(long *ix, long *iy, int n, FILE *hfp)
{
    int i,p;

    p = ifill(ix[n-1],iy[n-1],ix[0],iy[0],0,hfp);
    for(i=1;i<n;i++) {
        p = ifill(ix[i-1],iy[i-1],ix[i],iy[i],p,hfp);
    }
}

int ifill(long ixo,long iyo,long ixe,long iye,int p,FILE *hfp)
{
    if (ixe < 0) {
        if (ixo > 0) {
            iyo = (iyo*ixe - ixo*iye)/(ixe - ixo);
            ifillplot(0,iyo,p,hfp);
            return(1);
        }
    }
    else {
        if (ixo < 0) {
            iyo = (iyo*ixe - ixo*iye)/(ixe - ixo);
            ifillplot(0,iyo,p,hfp);
            p = 1;
        }
        ifillplot(ixe,iye,p,hfp);
        return(1);
    }
    return(p);
}

/*  plot    - general plot routine

    x,y     - coordinates in window space
    p       - pen flag (0 for penup, 1 for pen down)        */

void plot(float x,
          float y,
          int p,
          FILE *hfp)
{
    long fx,fy;

    fx = xtoix(x);
    fy = ytoiy(y);

    if (hfp) {
        if (p) fprintf(hfp,"%ld %ld l\n", fx, HEIGHT - fy);
        else fprintf(hfp,"np\n%ld %ld m\n", fx, HEIGHT - fy);
    }
}

/*  ifillplot    - general fill plot routine

    fx,fy   - coordinates in pixels
    p       - pen flag (0 for penup, 1 for pen down)        */

void ifillplot(long fx,
               long fy,
               int p,
               FILE *hfp)
{
    if (hfp) {
        if (p) fprintf(hfp,"%ld %ld l\n", fx, HEIGHT - fy - 1);
        else fprintf(hfp,"np\n%ld %ld m\n", fx, HEIGHT - fy - 1);
    }
}

/*  plotpoint   - plot a point

    n           - point order number        */

void plotpoint(int n,
               int col,
               FILE *hfp)
{
    float x,y;
    long ix,iy;

    if ((points[n].col == 0) || (points[n].type == 1)) return;
    x = points[n].x;
    y = points[n].y;

    if (!outside(x,y)) {
        ix = xtoix(x);
        iy = ytoiy(y);
        if (points[n].type < 0)
            plotsym(ix,iy,6L,3L,-6L,0L,3L,0L,col,points[n].ang,hfp);
        else if (points[n].type > 0)
            plotsym(ix,iy,6L,6L,0L,0L,(long)points[n].type,0L,col,points[n].ang,hfp);
    }
}

void plotline(int n,
              int cflag,
              FILE *hfp)
{
    float *x,*y,a1,a2;
    long ix,iy;
    int j,k,m,p,c,s;

    if (cflag) c = lines[n].col;
    else c = 0;
    if (lines[n].col != 0) {
        k = 0;
        m = lines[n].pid;
        while (m > 0) {
            k++;
            m = points[pnums[m]].next;
        }
        if (m < 0) { k++; s = 1; }
        else s = 0;
        if (k == 0) return;
        if (!(x = (float *)malloc(k*sizeof(float)))) return;
        if (!(y = (float *)malloc(k*sizeof(float)))) {
            free(x);
            return;
        }
        m = lines[n].pid;
        for (j=0;j<k;j++) {
            x[j] = points[pnums[m]].x;
            y[j] = points[pnums[m]].y;
            m = ABS(points[pnums[m]].next);
        }

        p = lines[n].type;

        if ((p > 0) || (k < 3)) {
            for (j=0;j<k;j++) plot(x[j],y[j],j,hfp);
            a1 = PI-(float)atan2((double)(y[0]-y[1]),(double)(x[1]-x[0]));
            a2 = PI-(float)atan2((double)(y[k-1]-y[k-2]),(double)(x[k-2]-x[k-1]));
        }
        else plotspline(k,s,x,y,&a1,&a2,hfp);
        if (hfp && cflag) hardcol(c,0,hfp);
        if ((ABS(p) > 1) && (k > 1)) {
            ix = xtoix(x[0]);
            iy = ytoiy(y[0]);

            plotsym((long)ix,(long)iy,6L,3L,-6L,0L,3L,0L,c,a1,hfp);

            if (ABS(p) > 2) {
                ix = xtoix(x[k-1]);
                iy = ytoiy(y[k-1]);
                plotsym((long)ix,(long)iy,6L,3L,-6L,0L,3L,0L,c,a2,hfp);
            }
        }
        free(x);
        free(y);
    }
    m = lines[n].pid;
    while (m > 0) {
        if (cflag) plotpoint(pnums[m],points[pnums[m]].col,hfp);
        else plotpoint(pnums[m],0,hfp);
        m = points[pnums[m]].next;
    }
}

void plotlabel(int n,
               int col,
               FILE *hfp)
{
    float x,y;
    long ix,iy;

    x = points[pnums[labels[n].pid]].x;
    y = points[pnums[labels[n].pid]].y;

    ix = xtoix(x);
    iy = ytoiy(y);

    if (hfp) {
        fprintf(hfp,"%ld %ld m\n", ix, HEIGHT - iy - 8);
        fprintf(hfp,"(%s) tt\n",labels[n].string);
    }
    sprintf((char *)itext,"%s",labels[n].string);
}

void plotspline(int n,
                int s,
                float *x,
                float *y,
                float *a1,
                float *a2,
                FILE *hfp)
{
    extern int deriv(int n, float *p, float *s, int flag);
    float *u,*v;
    float xmin, xmax, ymin, ymax;
    float sinc;
    float t, dt, dx, dy, rx, ry;
    float t0, s1, x0, y0, ax, bx, cx, ex, ay, by, cy, ey, xx, yy;
    int i,j;

    xmin = 1.e20;
    ymin = 1.e20;
    xmax = -xmin;
    ymax = -ymin;

    if (!(u = (float *)malloc(n*sizeof(float)))) return;
    if (!(v = (float *)malloc(n*sizeof(float)))) {
        free(u);
        return;
    }

    if(!deriv(n, x, u, s)) {
        free(u);
        free(v);
        return;
    }

    if(!deriv(n, y, v, s)) {
        free(u);
        free(v);
        return;
    }

    for (i=0;i<n;i++) {
        xmin = MIN(xmin,x[i]);
        xmax = MAX(xmax,x[i]);
        ymin = MIN(ymin,y[i]);
        ymax = MAX(ymax,y[i]);
    }

    dx = xmax - xmin;
    dy = ymax - ymin;

    sinc = 0.05;

/* =============================================== */
/* | Begin loop which computes polynomial        | */
/* | coefficients and plots curve.               | */
/* =============================================== */

    ax = bx = cx = ay = by = cy = 0.0;

    plot(x[0],y[0],0,hfp);
    for (j=0; j<n-1; ++j) {
        t0 = (float)j;
        ax = (u[j+1] - u[j])/6;
        bx = u[j]/2;
        cx = x[j+1] - x[j] - (u[j+1] + 2*u[j])/6;
        ex = xx = x[j];
        ay = (v[j+1] - v[j])/6;
        by = v[j]/2;
        cy = y[j+1] - y[j] - (v[j+1] + 2*v[j])/6;
        ey = yy = y[j];

        if (j == 0) *a1 = PI-(float)atan2((double)(-cy),(double)cx);

        while(t0 < (float)(j+1)) {
            plot(xx,yy,1,hfp);
            s1 = 2.0;
            while(s1 > 1.4) {
                t = t0 + sinc;
                dt = t - (float)j;

                x0 = ax*dt*dt*dt + bx*dt*dt + cx*dt + ex;
                y0 = ay*dt*dt*dt + by*dt*dt + cy*dt + ey;

/* =============================================== */
/* | These next statements compute the arclength | */
/* | of the subsequent step, the adjusted        | */
/* | increment step size and determines whether  | */
/* | an adjustment is necessary before           | */
/* | trudging on.                                | */
/* =============================================== */

                rx = (x0 - xx)/dx;
                ry = (y0 - yy)/dy;
                s1 = (float)sqr((double)(rx*rx + ry*ry))/0.0125;
                sinc /= s1;
            }
            xx = x0;
            yy = y0;
            t0 = t;
        }
    }
    *a2 = PI-(float)atan2((double)(3*ay+2*by+cy),(double)(-3*ax-2*bx-cx));
    plot(x[n-1],y[n-1],1,hfp);
    free(u);
    free(v);
}

long xtoix(float x)
{
    return(((x - view.xmin)/(view.xmax - view.xmin))*(float)WIDTH);
}

long ytoiy(float y)
{
    return(((view.ymax - y)/(view.ymax - view.ymin))*(float)HEIGHT);
}
