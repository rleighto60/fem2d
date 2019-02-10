/*
$Id: parse.c 1.2 1994/09/12 19:11:27 leighton Exp $
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

program     -   parse.c
author      -   Russell Leighton
date        -   4 Mar 1994
revision    -   9 Jan 1995 (completely rewritten)

input       -   input - input string to parse

output      -   args - array of string pointers to hold tokens

description -   Routine to parse input command lines

*/

#include <ctype.h>
#include <stdio.h>
#include <string.h>

int parse(char *input, char *args[], int narg)
{
    int i,j=0,flag=0,n;
    char *arg;

    arg = input;

    for(i=0;i<narg;i++) {
        while(*arg == ' ') ++arg;
        if(flag != 2) {
            switch(*arg) {
                case '\"':
                    args[i] = ++arg;
                    arg += strcspn(arg,(char *)"\"");
                    if(*arg != 0) *arg++ = 0;
                    break;
                case '\'':
                    args[i] = ++arg;
                    arg += strcspn(arg,(char *)"\'");
                    if(*arg != 0) *arg++ = 0;
                    break;
                default: 
                    args[i] = arg; 
                    break;
            }
        }
        else args[i] = arg;
        switch(flag) {
            case 0: arg += strcspn(arg,(char *)" =(");
                    break;
            case 1: arg += strlen(arg);
                    flag = 0;
                    break;
            case 2: arg += strcspn(arg,(char *)",");
                    if(*arg == 0) {
                        while(*(--arg) == ' ');
                        if(*arg == ')') {
                            *arg = 0;
                            flag = 0;
                        }
                    }
                    else *arg++ = 0;
                    break;
        }
        if(*arg == 0) break;
        while(*arg == ' ') *arg++ = 0;
        if(!flag) {
            switch(*arg) {
                case '=': flag = 1; *arg++ = 0; break;
                case '(': flag = 2; *arg++ = 0; break;
            }
        }
    }
    if(flag) return(0);

    if(i < narg) i++;
    arg = args[0];
    while(*arg != 0) {
        *arg = (char)tolower((int)(*arg));
        arg++;
    }
    for(n=0;n<i;n++) if((args[n] != NULL) && (args[n][0] != 0)) j = n;
    return(++j);
}
