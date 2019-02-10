/*
$Id: hash.c 1.2 1994/09/12 19:11:27 leighton Exp $
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

program     -   hash.c

description -   Hash utility routine.

*/
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

void readln(FILE *f, char *line)
{
  if (fgets(line, 80, f) != NULL) {
    if (line[strlen(line)-1]=='\n')
      line[strlen(line)-1]='\0';
  }
}

int hash(char *s, int size, int fact, int mask)
{
  register int i;
  register char *sp;
  register int c;

  i = strlen(s);

  for (sp = s; *sp; sp++ ) {
    c = toupper(*sp);
    i = ((i * fact + c ) & mask);
  }
  return (i % size);
}

int main(int argc, char *argv[])
{
  FILE *fp,*fopen();
  char *hashtable = 0;
  char line[80];
  int i,j,col=0,size=0,mask=0xfff,fact,mins=2,maxs=1024,minf=2,maxf=1024;

  if (argc > 2) sscanf(argv[2],"%x",&mask);
  if (argc > 3) sscanf(argv[3],"%d",&mins);
  if (argc > 4) sscanf(argv[4],"%d",&maxs);
  if (argc > 5) sscanf(argv[5],"%d",&minf);
  if (argc > 6) sscanf(argv[6],"%d",&maxf);

  for (size = mins;size<=maxs;size++) {
    hashtable = (char *)malloc(size);
    if (hashtable == NULL) return 1;

    for (fact=minf;fact<=maxf;fact++) {
      for (i=0;i<size;i++) hashtable[i] = 0;
      fp = fopen(argv[1],"r");
      if (fp == NULL) {
        fprintf(stderr,"error - could not open file - %s\n",argv[1]);
        return 1;
      }
      col = 0;
      i = 0;
      readln(fp,line);
      while (!feof(fp)) {
        j = hash((char *)line,size,fact,mask);
        if (hashtable[j] == 0) hashtable[j] = (char)++i;
        else col++;
        readln(fp,line);
      }
      fclose(fp);
      if (col == 0) break;
    }

    if (col == 0) {
      fprintf(stdout,"/* hash table generated for file - %s\n",argv[1]);
      fprintf(stdout,"   with parameters:\n");
      fprintf(stdout,"       size = %d\n",size);
      fprintf(stdout,"       factor = %d\n",fact);
      fprintf(stdout,"       mask = %x                                 */\n\n",mask);
      fprintf(stdout,"char hashtable[%d] = {",size);
      j = 0;
      while(j < size-1) {
        fprintf(stdout,"\n");
        for(i=0;i<20;i++) {
          if(j == (size-1)) break;
          fprintf(stdout,"%2d,",hashtable[j++]);
        }
      }
      fprintf(stdout,"%2d\n};\n\n",hashtable[j++]);
      fprintf(stdout,"int hash(char *s)\n");
      fprintf(stdout,"{\n");
      fprintf(stdout,"    register int i;\n");
      fprintf(stdout,"    register char *sp;\n");
      fprintf(stdout,"    register int c;\n");

      fprintf(stdout,"    i = strlen(s);\n");

      fprintf(stdout,"    for (sp = s; *sp; sp++ ) {\n");
      fprintf(stdout,"        c = toupper(*sp);\n");
      fprintf(stdout,"        i = ((i * %d + c ) & 0x%x);\n",fact,mask);
      fprintf(stdout,"    }\n");
      fprintf(stdout,"    return ((int)hashtable[i %% %d]);\n",size);
      fprintf(stdout,"}\n");
    }
    free(hashtable);
    if (col == 0) break;
  }
  if (col != 0) {
    fprintf(stderr,"Failed to generate hash - collisions = %d\n",col);
    return 1;
  }
  return 0;
}
