/*
$Id: nln2d.c 1.2 1994/09/12 19:11:27 leighton Exp $
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

program     -   nln2d.c
author      -   Russell Leighton
date        -   1 Nov 1993

description -   Program to compute a finite element solutions for simple
                two dimensional, plane or axisymmetric, problems.

*/

#define MAIN
#include "header.h"
#include "iff.h"

extern int formkf();
extern int solve();
extern int savedata();
extern int readdata();
extern void freemem();

int main( int argc, char *argv[] )
{
  int i;
  float du;
  char *in=0,*out=0;

  if(argc > 1) {
    for(i=1;i<argc;i++) {
      if(!in) in = argv[i];
      else if(!out) out = argv[i];
    }
    if(!readdata(in)) {
      fprintf(stderr,"main - error reading data file\n");
      return 1;
    }
    if(!formkf(0,&du)) return 1;
    fprintf(stdout,"%12.4e\n",du);
    if(!solve(0,&du)) return 1;
    fprintf(stdout,"%12.4e\n",du);
    if(out) savedata(3,out);
    else savedata(0,in);
    freemem(0,-1);
    return 0;
  }
  fprintf(stderr,"Usage: nln2d <file>\n");
  return 1;
}
