/*
$Id: plot.h 1.3 1994/09/14 17:41:05 leighton Exp leighton $
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

program     -   plot.h

description -   Header file for plot routines.

*/

#define WIDTH 1200
#define HEIGHT 1024

struct view {
    int id;
    float xmin,xmax,ymin,ymax,xtol,ytol;
};

extern FILE *hfp;

extern struct view view;
extern int erase;
extern int lastnode,lastelem;
extern float Xorg,Yorg;

char itext[80];

float xip[4][4] = {{ 0.0000000000,  0.0000000000,  0.0000000000,  0.0000000000},
                   {-0.5773502692,  0.5773502692,  0.0000000000,  0.0000000000},
                   {-0.7745966692,  0.0000000000,  0.7745966692,  0.0000000000},
                   {-0.8611363116, -0.3399810436,  0.3399810436,  0.8611363116}};
float w[4][4]   = {{ 2.0000000000,  0.0000000000,  0.0000000000,  0.0000000000},
                   { 1.0000000000,  1.0000000000,  0.0000000000,  0.0000000000},
                   { 0.5555555556,  0.8888888889,  0.5555555556,  0.0000000000},
                   { 0.3478548451,  0.6521451549,  0.6521451549,  0.3478548451}};

int ifill(long,long,long,long,int,FILE *);
int outside(float,float);

long xtoix(float);
long ytoiy(float);

void adjustfill(float);
void clearwindow(FILE *);
void filldeform(float,FILE *);
void fillelem(int,float,int,FILE *);
void fillmodel(FILE *);
void fillpoly(int,float *,float *,int,FILE *);
void fillresult(int,int,int,float,float,FILE *);
void findwindow(int);
void hardcol(int,int,FILE *);
void ifillelem(int,int,float *,float *,FILE *);
void ifillplot(long,long,int,FILE *);
void ifillpoly(long *, long *,int,FILE *);
void plot(float,float,int,FILE *);
void plotbound(void);
void plotcross(long,long,int,FILE *);
void plotdbc(int,int,FILE *);
void plotdeform(float,FILE *);
void plotdistr(int,int,FILE *);
void plotdot(long,long,FILE *);
void plotdraw(FILE *);
void plotelem(int,float,int,int,FILE *);
void plotlabel(int,int,FILE *);
void plotline(int,int,FILE *);
void plotmodel(FILE *);
void plotnode(int,int,int,FILE *);
void plotnumber(int,long,long,FILE *);
void plotpbc(int,int,FILE *);
void plotpoint(int,int,FILE *);
void plotresult(int,int,int,float,float,FILE *);
void plotspline(int,int,float *,float *,float *,float *,FILE *);
void plotsym(long,long,long,long,long,long,long,long,long,float,FILE *);
void setbound(void);
void showpage(FILE *);
void sshape(float,int,REAL *,REAL *);
void trigrad(int,float *,float *,float *,float,float,FILE *);
void writeclose(FILE *);
void writeprolog(FILE *);
