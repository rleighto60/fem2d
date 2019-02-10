/*
$Id: post2d.c 1.2 1994/09/12 19:11:27 leighton Exp $
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

program     -   post2d.c
author      -   Russell Leighton
date        -   10 Dec 1993

description -   Finite element results postprocessor for simple
                two dimensional, plane or axisymmetric, problems.
                Utilizes the REXX macro language as the preprocessor.

*/

#define MAIN
#include "header.h"
#include "iff.h"

extern int savedata();
extern int readdata();
extern int post();
extern void error();
extern void freemem();

int main(int argc, char *argv[])
{
  int i;
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
    for(i=0;i<nsets;i++) {
      if((sets[i].results) || (sets[i].kind != pkind)) continue;
      sets[i].results = (struct result *)malloc(nelems*sizeof(struct result));
      if(sets[i].results == NULL) {
        fprintf(stderr,"main - could not allocate memory for results\n");
        return 1;
      }
      post(i);
    }
    if(out) savedata(4,out);
    else savedata(0,in);
    freemem(0,-1);
    return 0;
  }
  fprintf(stderr,"Usage: post2d <file>\n");
  return 1;
}
