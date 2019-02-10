/*
$Id: fem2d.h 1.2 1994/09/12 19:11:27 leighton Exp $
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

program     -   fem2d.h

description -   Header file for fem2d.

*/
#define INCL_RXSUBCOM
#define INCL_RXSHV
#define INCL_RXFUNC
#define ULONG_TYPEDEFED
#include <rexxsaa.h>

SHVBLOCK result = {
    NULL,
    { 6, (char *)"result" },
    { 0, NULL },
    6,
    320,
    RXSHV_SYSET,
    0
};

APIRET APIENTRY fem2dHandler(PRXSTRING, PUSHORT, PRXSTRING);
APIRET APIENTRY asinHandler(PSZ, ULONG, PRXSTRING, PSZ, PRXSTRING);
APIRET APIENTRY atanHandler(PSZ, ULONG, PRXSTRING, PSZ, PRXSTRING);
APIRET APIENTRY sinHandler(PSZ, ULONG, PRXSTRING, PSZ, PRXSTRING);
APIRET APIENTRY cosHandler(PSZ, ULONG, PRXSTRING, PSZ, PRXSTRING);
APIRET APIENTRY sqrtHandler(PSZ, ULONG, PRXSTRING, PSZ, PRXSTRING);
APIRET APIENTRY powHandler(PSZ, ULONG, PRXSTRING, PSZ, PRXSTRING);
extern void showpage(FILE *);
extern void writeclose(FILE *);

char hostname[] = "FEM2D";
extern FILE *hfp;
