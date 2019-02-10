/*
$Id: iff.h 1.2 1994/09/12 19:11:27 leighton Exp $
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

program     -   iff.h

description -   Header file for interchange file format (IFF).

*/
#define MakeID(a,b,c,d) ((long)(d)<<24L | (long)(c)<<16L | (b)<<8 | (a))
#define SafeRead(a,b,c) if (fread(b,1L,c,a) < c) { fclose(a); return(0); }
#define SafeWrite(a,b,c) if(fwrite(b,1L,c,a) < c) { fclose(a); return(0); }
#define SafeSeek(a,b)    if(fseek(a,b,SEEK_SET)!=0) { fclose(a); return(0); }

#define ID_FORM MakeID('F','O','R','M')
#define ID_FE2D MakeID('F','E','2','D')
#define ID_GLBL MakeID('G','L','B','L')
#define ID_PRBS MakeID('P','R','B','S')
#define ID_MATS MakeID('M','A','T','S')
#define ID_LAMS MakeID('L','A','M','S')
#define ID_NODS MakeID('N','O','D','S')
#define ID_ELMS MakeID('E','L','M','S')
#define ID_PBCS MakeID('P','B','C','S')
#define ID_DBCS MakeID('D','B','C','S')
#define ID_NGRP MakeID('N','G','R','P')
#define ID_SOLN MakeID('S','O','L','N')
#define ID_RSET MakeID('R','S','E','T')
#define ID_FUNS MakeID('F','U','N','S')
#define ID_ALAS MakeID('A','L','A','S')
#define ID_COLS MakeID('C','O','L','S')
#define ID_PNTS MakeID('P','N','T','S')
#define ID_LINS MakeID('L','I','N','S')
#define ID_LABS MakeID('L','A','B','S')
#define ID_ENDD MakeID('E','N','D','D')

typedef struct {
  long ckID,ckSize;
} Chunk;
