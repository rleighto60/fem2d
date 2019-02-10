/*
$Id: eval.c 1.2 1994/09/12 19:11:27 leighton Exp $
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

program     -   eval.c
author      -   Russell Leighton
date        -   11 Aug 1992

input       -   nf      - function number
                n       - element order number
                ip      - integration point (0 if xi is specified)
                xi      - local coordinate (used if ip = 0)
                yi      - local coordinate (used if ip = 0)
                val     - pointer to variable to contain result

description -   Routine to evaluate function at selected points within 
                an element.

*/

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "header.h"

#define DELIMITER   1
#define NUMBER      2
#define VARIABLE    3
#define FUNCTION    4

float x,y,xx,yy,c[10],d[10],a[2];
REAL f[4],detJ,area;
static char *prog;
static char token[80];
static char tok_type;

int eval_exp(float *answer);
int eval_exp2(float *answer);
int eval_exp3(float *answer), eval_exp4(float *answer);
int eval_exp5(float *answer), eval_exp6(float *answer);
int eval_exp7(float *answer);
int atom(float *answer);
void get_token(void);
void serror(int error);
float find_var(char *s);
int isdelim(char c);
float interp(int id, float t, int flag);
float newton(int id, float v, int flag);
int deriv(int n, float *p, float *s, int flag);

extern void shape(float xi, float yi, int id, REAL *psi, REAL *dpdx, REAL *dpdy);
extern int grad();
extern void dshape();
extern void force();
extern void rotmat();
extern void lamina();

extern char *_args[24];

/*

routine     -   eval

input       -   nf - function number
                n  - element number
                ip - point flag
                xi - local x location
                yi - local y location

output      -   val - value

description -   Routine to evaluate function at specifed location.

*/

int eval(expr,n,ip,xi,yi,val)
char *expr;
int n,ip;
float xi,yi,*val;
{
    int i,j,k,l,ni,nn,ns,m;
    REAL J[2][2],E[6][6];
    REAL *psi,*dpde,*dpdn,dpdx[9],dpdy[9];
    REAL ps[9],de[9],dn[9];
    float y1,y2;

/* initialize variables */

    x = y = xx = yy = 0.0;
    for(i=0;i<10;i++) c[i]  = d[i] = 0.0;
    for(i=0;i<4;i++) f[i] = 0.0;
    a[0] = a[1] = 0.0;
    for(i=0;i<nsolns;i++) solns[i].v = solns[i].dvdx = solns[i].dvdy = 0.0;

    if(n >= 0) {
        ns = elems[n].type;
        nn = shapes[ns]->nn;

/* find the appropriate material */

        m = l = elems[n].mat;
        if((m > 0) && (m < mmax)) {
            m = mnums[m];
            while(m >= 0) {
                if(mats[m].kind == pkind) break;
                else m = mats[m].next;
            }
        }
        else m = -1;

/* and laminate if one exists */

        if((l > 0) && (l < lmax)) {
            l = lnums[l];
            if (lams[l].id == 0) l = -1;
        }
        else l = -1;

        if(m >= 0) {
            if(mats[m].type != 1) for(i=0;i<10;i++) c[i] = mats[m].C[i];
        }

        ni = shapes[ns]->eintg->ni; 

/* calculate shape function data at evaluation points (if needed) */

        if(!shapes[ns]->flag) {
            for(i=0;i<ni;i++) {
                xi = shapes[ns]->eintg->xip[i];
                yi = shapes[ns]->eintg->yip[i];
                psi = shapes[ns]->psi + i*nn;
                dpde = shapes[ns]->dpde + i*nn;
                dpdn = shapes[ns]->dpdn + i*nn;
                shape(xi,yi,ns,psi,dpde,dpdn);
            }
            shapes[ns]->flag = 1;
        }

/* calculate element area (to normalize output jacobian) */

        area = 0.0;
        for (i=0; i<ni; i++) {
            psi = shapes[ns]->psi + i*nn;
            dpde = shapes[ns]->dpde + i*nn;
            dpdn = shapes[ns]->dpdn + i*nn;
            if (grad(n,dpde,dpdn,J,&detJ)) area += detJ*shapes[ns]->eintg->w[i];
        }

        if(ip < 0) { 
            ip = (-ip - 1)%ni;
            xi = shapes[ns]->eintg->xip[ip];
            yi = shapes[ns]->eintg->yip[ip];
            psi = shapes[ns]->psi + ip*nn;
            dpde = shapes[ns]->dpde + ip*nn;
            dpdn = shapes[ns]->dpdn + ip*nn;
        }
        else {
            if(ip > 0) {
                ip = (ip - 1)%nn;
                xi = shapes[ns]->xi[ip];
                yi = shapes[ns]->yi[ip];
            }
            psi = ps;
            dpde = de;
            dpdn = dn;
            shape(xi,yi,ns,psi,dpde,dpdn);
        }
        xx = (xi - shapes[ns]->xi[0])/(shapes[ns]->xi[1] - shapes[ns]->xi[0]);
        yy = (yi - shapes[ns]->xi[0])/(shapes[ns]->xi[1] - shapes[ns]->xi[0]);

/* calculate gradient */

        if (grad(n,dpde,dpdn,J,&detJ)) {

            dshape(n,J,detJ,dpde,dpdn,dpdx,dpdy);

/* calculate global position and solution values/derivatives */

            for(i=0;i<nn;i++) {
                x += (float)psi[i]*nodes[nnums[elems[n].nodes[i]]].x;
                y += (float)psi[i]*nodes[nnums[elems[n].nodes[i]]].y;
                for(j=0;j<nsolns;j++) {
                    if(solns[j].kind == pkind) {
                        solns[j].v += (float)psi[i]*solns[j].u[nnums[elems[n].nodes[i]]];
                        solns[j].dvdx += (float)dpdx[i]*solns[j].u[nnums[elems[n].nodes[i]]];
                        solns[j].dvdy += (float)dpdy[i]*solns[j].u[nnums[elems[n].nodes[i]]];
                    }
                }
            }
        }

/* retrieve material (and laminate) properties and body forces, if possible */

        if(m >= 0) {
            force(n,m,psi,f);
            if (mats[m].type == 1) {
                rotmat(n,m,J,E);
                a[0] = (float)atan2(J[1][0] - J[0][1],J[0][0] + J[1][1])*180.0/PI;
                k = 0;
                for (j=0; j<4; j++) {
                    for (i=0; i<(4-j); i++) c[k++] = (float)E[i][i+j];
                }
            }
        }
        if(l >= 0) {
            i = l;
            y1 = y2 = 0.0;
            while(i >= 0) {
                y2 += lams[i].frac;
                if((yy >= y1) && (yy <= y2)) break;
                y1 = y2;
                i = lams[i].next;
            }
            if(i >= 0) {
                if(lams[i].func < 0) a[1] = (REAL)lams[i].mang;
                else {
                    prog = funs[lams[i].func].expr;
                    if(eval_exp(val)) a[1] = (REAL)(*val);
                }
                lamina(lams[i].mat,(REAL)a[1]*PI/180.0,E);
                k = 0;
                for (j=0; j<4; j++) {
                    for (i=0; i<(4-j); i++) d[k++] = (float)E[i][i+j];
                }
            }
        }
    }

    prog = expr;
    if(!eval_exp(val)) {
        severity = 5;
        return(0);
    }
    return(1);
}

/*

routine     -   getval

input       -   expr - function expression
                n    - element number
                psi  - element shape function

output      -   val  - value

description -   Routine to evaluate function expression.

*/

float getval(expr,n,psi)
char *expr;
int n;
REAL *psi;
{
    int i,nn,ns;
    float val;

/* initialize variables */

    x = y = xx = yy = 0.0;
    for(i=0;i<10;i++) c[i]  = d[i] = 0.0;
    for(i=0;i<4;i++) f[i] = 0.0;
    a[0] = a[1] = 0.0;

    if(n >= 0) {
        ns = elems[n].type;
        nn = shapes[ns]->nn;

/* calculate global position */

        for(i=0;i<nn;i++) {
            x += (float)psi[i]*nodes[nnums[elems[n].nodes[i]]].x;
            y += (float)psi[i]*nodes[nnums[elems[n].nodes[i]]].y;
        }
    }

    prog = expr;
    if(!eval_exp(&val)) {
        severity = 5;
        return(0.0);
    }
    return(val);
}

int eval_arg(expr,val)
char *expr;
float *val;
{
    prog = expr;
    if(!eval_exp(val)) return(0);
    return(1);
}

int eval_exp(float *answer)
{
    get_token();
    if(!*token) {
        serror(2);
        return(0);
    }
    return(eval_exp2(answer));
}

int eval_exp2(float *answer)
{
    int err;
    char op;
    float temp;

    err = eval_exp3(answer);
    while((op = *token) == '+' || op == '-') {
        get_token();
        err = eval_exp3(&temp);
        switch(op) {
            case '-':
                *answer = *answer - temp;
                break;
            case '+':
                *answer = *answer + temp;
                break;
        }
    }
    return(err);
}

int eval_exp3(float *answer)
{
    int err;
    char op;
    float temp;

    err = eval_exp4(answer);
    while((op = *token) == '*' || op == '/') {
        get_token();
        err = eval_exp4(&temp);
        switch(op) {
            case '*':
                *answer = *answer * temp;
                break;
            case '/':
                *answer = *answer / temp;
                break;
        }
    }
    return(err);
}

int eval_exp4(float *answer)
{
    int err;
    float f;

    err = eval_exp5(answer);
    if(*token == '^') {
        get_token();
        err = eval_exp4(&f);
        if(f == 0.0) {
            *answer = 1.0;
            return(err);
        }
        *answer = (float)pow((double)*answer,(double)f);
    }
    return(err);
}

int eval_exp5(float *answer)
{
    int err;
    char op;

    op = 0;
    if((tok_type == DELIMITER) && (*token == '+' || *token == '-')) {
        op = *token;
        get_token();
    }
    err = eval_exp6(answer);
    if(op == '-') *answer = -(*answer);
    return(err);
}

int eval_exp6(float *answer)
{
    float temp1,temp2;
    char *prog0;
    char *functions[15] = { "pi","sin","cos","tan","asin","acos","atan","abs",
                            "intx","inty","indx","indy","inxt","inyt",0 };
    int nfun = 14;
    int i,nf,err;


    if(tok_type == FUNCTION) {
        for(nf=0;nf<nfun;nf++) {
            if(!strcmp(token,functions[nf])) break;
        }
        switch(nf) {
            case 0:     /* pi */
                get_token();
                *answer = (float)PI;
                return(1);
            case 1:     /* sin */
                get_token();
                err = eval_exp7(&temp1);
                *answer = (float)sin((double)(temp1*PI/180.0));
                return(err);
            case 2:     /* cos */
                get_token();
                err = eval_exp7(&temp1);
                *answer = (float)cos((double)(temp1*PI/180.0));
                return(err);
            case 3:     /* tan */
                get_token();
                err = eval_exp7(&temp1);
                *answer = (float)tan((double)(temp1*PI/180.0));
                return(err);
            case 4:     /* asin */
                get_token();
                err = eval_exp7(&temp1);
                if(temp1 > 1.0) temp1 = 1.0;
                if(temp1 < -1.0) temp1 = -1.0;
                *answer = (float)asin((double)temp1)*180.0/PI;
                return(err);
            case 5:     /* acos */
                get_token();
                err = eval_exp7(&temp1);
                if(temp1 > 1.0) temp1 = 1.0;
                if(temp1 < -1.0) temp1 = -1.0;
                *answer = (float)acos((double)temp1)*180.0/PI;
                return(err);
            case 6:     /* atan */
                get_token();
                err = eval_exp7(&temp1);
                err *= eval_exp7(&temp2);
                *answer = (float)atan2((double)temp1,(double)temp2)*180.0/PI;
                return(err);
            case 7:     /* abs */
                get_token();
                err = eval_exp7(&temp1);
                *answer = (float)ABS(temp1);
                return(err);
            case 8:     /* intx */
                get_token();
                err = eval_exp7(&temp1);
                err *= eval_exp7(&temp2);
                *answer = interp((int)temp1,temp2,1);
                return(err);
            case 9:     /* inty */
                get_token();
                err = eval_exp7(&temp1);
                err *= eval_exp7(&temp2);
                *answer = interp((int)temp1,temp2,-1);
                return(err);
            case 10:    /* indx */
                get_token();
                err = eval_exp7(&temp1);
                err *= eval_exp7(&temp2);
                *answer = interp((int)temp1,temp2,2);
                return(err);
            case 11:    /* indy */
                get_token();
                err = eval_exp7(&temp1);
                err *= eval_exp7(&temp2);
                *answer = interp((int)temp1,temp2,-2);
                return(err);
            case 12:    /* inxt */
                get_token();
                err = eval_exp7(&temp1);
                err *= eval_exp7(&temp2);
                *answer = newton((int)temp1,temp2,1);
                return(err);
            case 13:     /* inyt */
                get_token();
                err = eval_exp7(&temp1);
                err *= eval_exp7(&temp2);
                *answer = newton((int)temp1,temp2,-1);
                return(err);
            default:
                prog0 = prog;
                if(((token[0] | ' ') == 'a') && (isdigit(token[1]))) {
                    sscanf(token,"%*c%d",&i);
                    if (_args[i] == NULL) { serror(2); return(0); }
                    prog = _args[i];
                }
                else {
                    for(i=0;i<nfuns;i++) {
                        if(!strcmp(token,funs[i].name)) break;
                    }
                    if(i == nfuns) { serror(3); return(0); }
                    prog = funs[i].expr;
                }
                get_token();
                err = eval_exp2(answer);
                prog = prog0;
                get_token();
                return(err);
        }
    }
    return(eval_exp7(answer));
}

int eval_exp7(float *answer)
{
    int err;

    if((*token == '(') || (*token == ',')) {
        get_token();
        err = eval_exp2(answer);
        if((*token != ')') && (*token != ',')) { serror(1); err = 0; }
        if(*token != ',') get_token();
        return(err);
    }
    else return(atom(answer));
}

int atom(float *answer)
{
    switch(tok_type) {
        case VARIABLE:
            *answer = find_var(token);
            get_token();
            return(1);
        case NUMBER:
            *answer = atof(token);
            get_token();
            return(1);
        default:
            serror(0);
            return(0);
    }
}

void serror(int error)
{
    static char *e[] = {
        "syntax error",
        "unbalanced parentheses",
        "no expression present",
        "unknown function"
    };
    sprintf(lasterr,"eval - %s",e[error]);
}

void get_token(void)
{
    char *temp;

    tok_type = 0;
    temp = token;
    *temp = '\0';

    if(!*prog) return;
    while(isspace(*prog)) ++prog;

    if(strchr("+-*/^=(),",*prog)) {
        tok_type = DELIMITER;
        *temp++ = *prog++;
    }
    else if(isalpha(*prog)) {
        tok_type = FUNCTION;
        switch(*prog | ' ') {
            case 'c': 
            case 'l': 
            case 'f': 
            case 'm': 
            case 'p': 
            case 'j': 
            case 'n': 
            case 'u': 
            case 'x': 
            case 'y': 
            case 'r': 
            case 'z': 
            case 't': 
                *temp++ = *prog++;
                if(isdigit(*prog)) tok_type = VARIABLE; 
                break;
        }
        while(!isdelim(*prog)) *temp++ = *prog++;
    }
    else if((isdigit(*prog)) || (*prog == '.')) {
        while(!isdelim(*prog)) {
            if((*prog | ' ') == 'e') *temp++ = *prog++;
            *temp++ = *prog++;
        }
        tok_type = NUMBER;
    }

    *temp = '\0';
}

int isdelim(char c)
{
    if(strchr(" +-/*^=(),",c) || c==9 || c=='\r' || c=='\n' || c==0) return(1);
    return(0);
}

float find_var(char *s)
{
    int i,k;

    i = 1;
    sscanf(s,"%*c%d",&i);
    switch(s[0] | ' ') {
        case 'c':
            if((i > 0) && (i < 11)) return(c[i-1]);
            else return(0.0);
        case 'l':
            if((i > 0) && (i < 11)) return(d[i-1]);
            else return(0.0);
        case 'f':
            if((i > 0) && (i < 5)) return((float)f[i-1]);
            else return(0.0);
        case 'm':
            if((i > 0) && (i < 3)) return((float)a[i-1]);
            else return(0.0);
        case 'p':
            if(i == 1) return(xx);
            if(i == 2) return(yy);
            return(0.0);
        case 'j':
            if(i == 0) return((float)(detJ/area));
            else return(0.0);
        case 'u':
            for(k=0;k<nsolns;k++) {
                if((solns[k].dof == i) && (solns[k].type == 0) && (solns[k].kind == pkind)) 
                    return(solns[k].v);
            }
            return(0.0);
        case 'n':
            for(k=0;k<nsolns;k++) {
                if((solns[k].dof == i) && (solns[k].type == 1) && (solns[k].kind == pkind)) 
                    return(solns[k].v);
            }
            return(0.0);
        case 'x':
        case 'r':
            if(i == 0) return(x);
            for(k=0;k<nsolns;k++) {
                if((solns[k].dof == i) && (solns[k].type == 0) && (solns[k].kind == pkind)) 
                    return(solns[k].dvdx);
            }
            return(0.0);
        case 'y':
        case 'z':
            if(i == 0) return(y);
            for(k=0;k<nsolns;k++) {
                if((solns[k].dof == i) && (solns[k].type == 0) && (solns[k].kind == pkind)) 
                    return(solns[k].dvdy);
            }
            return(0.0);
        case 't':
            if(ptype) {
                for(k=0;k<nsolns;k++) {
                    if((solns[k].dof == i) && (solns[k].type == 0) && (solns[k].kind == pkind)) 
                        return(solns[k].v/x);
                }
            }
            return(0.0);
        default : return(0.0);
    }
}

float interp(int id, float t, int flag)
{
    float *xi,*ui;
    float dt;
    float x0, ax, bx, cx, ex;
    int j,k,m,n,p,s;

    if ((id <= 0) || (id >= imax)) return(0.0);
    k = 0;
    n = inums[id];
    if (n < 0) return(0.0);
    m = lines[n].pid;
    while (m > 0) {
        k++;
        m = points[pnums[m]].next;
    }
    if (m < 0) { k++; s = 1; }
    else s = 0;
    if ((k < 2) || (t > (float)(k-1))) return(0.0);
    if (!(xi = (float *)malloc(k*sizeof(float)))) return(0.0);
    m = lines[n].pid;
    for (j=0;j<k;j++) {
        if (flag < 0) xi[j] = points[pnums[m]].y;
        else xi[j] = points[pnums[m]].x;
        m = ABS(points[pnums[m]].next);
    }

    p = lines[n].type;

    x0 = 0.0;
    j = (int)t;
    if ((p > 0) || (k < 3)) {
        cx = xi[j+1] - xi[j];
        ex = xi[j];

        dt = t - (float)j;
        switch (ABS(flag)) {
            case 1: x0 = cx*dt + ex; break;
            case 2: x0 = cx; break;
            default: x0 = 0.0; break;
        }
    }
    else {
        if (!(ui = (float *)malloc(k*sizeof(float)))) {
            free(xi);
            return(0.0);
        }

        if(!deriv(k, xi, ui, s)) {
            free(xi);
            free(ui);
            return(0.0);
        }

        ax = (ui[j+1] - ui[j])/6;
        bx = ui[j]/2;
        cx = xi[j+1] - xi[j] - (ui[j+1] + 2*ui[j])/6;
        ex = xi[j];

        dt = t - (float)j;
        switch (ABS(flag)) {
            case 1: x0 = ax*dt*dt*dt + bx*dt*dt + cx*dt + ex; break;
            case 2: x0 = 3*ax*dt*dt + 2*bx*dt + cx; break;
            case 3: x0 = 6*ax*dt + 2*bx; break;
            case 4: x0 = 6*ax; break;
            default: x0 = 0.0; break;
        }

        free(ui);
    }
    free(xi);
    return(x0);
}

float newton(int id, float v, int flag)
{
    float *xi,*ui;
    float dt = 0.0;
    float ax, bx, cx, dx, ex, fx;
    float xtol=1.0e-4,ftol=1.0e-5;
    int maxit=50;
    int i,j,k,m,n,p,s;

    if ((id <= 0) || (id >= imax)) return(0.0);
    k = 0;
    n = inums[id];
    if (n < 0) return(0.0);
    m = lines[n].pid;
    while (m > 0) {
        k++;
        m = points[pnums[m]].next;
    }
    if (m < 0) { k++; s = 1; }
    else s = 0;
    if (k < 2) return(0.0);
    if (!(xi = (float *)malloc(k*sizeof(float)))) return(0.0);
    m = lines[n].pid;
    for (i=0;i<k;i++) {
        if (flag < 0) xi[i] = points[pnums[m]].y;
        else xi[i] = points[pnums[m]].x;
        m = ABS(points[pnums[m]].next);
    }

    p = lines[n].type;

    for (j=0;j<k-1;j++) {
        dt = (v-xi[j])/(xi[j+1]-xi[j]);
        if ((dt >= 0.0) && (dt <= 1.0)) break;
    }

    if ((p < 0) && (k > 2)) {
        if (!(ui = (float *)malloc(k*sizeof(float)))) {
            free(xi);
            return(0.0);
        }

        if(!deriv(k, xi, ui, s)) {
            free(xi);
            free(ui);
            return(0.0);
        }

        ax = (ui[j+1] - ui[j])/6;
        bx = ui[j]/2;
        cx = xi[j+1] - xi[j] - (ui[j+1] + 2*ui[j])/6;
        ex = xi[j];

        fx = ax*dt*dt*dt + bx*dt*dt + cx*dt + ex - v;
        for(i=0;i<maxit;i++) {
            dx = fx/(3*ax*dt*dt + 2*bx*dt + cx);
            dt -= dx;
            fx = ax*dt*dt*dt + bx*dt*dt + cx*dt + ex - v;
            if((ABS(dx) <= xtol) || (ABS(fx) <= ftol)) break;
        }
        free(ui);
    }
    free(xi);

    return((float)j + dt);
}

int deriv(int n, float *p, float *s, int flag)
{
    float *b;
    int j;

    if(!(b = (float *)malloc((n+1)*sizeof(float)))) return 0;

    for(j=1; j<n-1; ++j) {
        s[j] = (p[j+1]-2*p[j]+p[j-1])*6;
        b[j] = 4;
    }
    for(j=2; j<n-1; ++j) {
        b[j] -= 1/b[j-1];
        s[j] -= s[j-1]/b[j-1];
	}
	s[n-2] /= b[n-2];
    for(j=n-3; j>0; --j) s[j] = (s[j]-s[j+1])/b[j];
    if (flag) s[0] = s[n-1] = (s[1]+s[n-2])/2.0;
    else s[0] = s[n-1] = 0.0;
	free(b);
    return 1;
}

