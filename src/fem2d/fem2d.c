/*
$Id: fem2d.c 1.2 1994/09/12 19:11:27 leighton Exp $
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

program     -   fem2d.c

description -   Main entry for fem2d.

*/
#define MAIN
#include "header.h"
#include "fem2d.h"
#include "commands.h"
#include "hash.h"

int main( int argc, char *argv[] )
{
  short returnCode;
  RXSTRING Result ;

  if(argc > 1) {
    RexxRegisterSubcomExe( hostname, (PFN) fem2dHandler, NULL ) ;
    RexxRegisterFunctionExe( "asin", (PFN) asinHandler ) ;
    RexxRegisterFunctionExe( "atan", (PFN) atanHandler ) ;
    RexxRegisterFunctionExe( "sin", (PFN) sinHandler ) ;
    RexxRegisterFunctionExe( "cos", (PFN) cosHandler ) ;
    RexxRegisterFunctionExe( "sqrt", (PFN) sqrtHandler ) ;
    RexxRegisterFunctionExe( "pow", (PFN) powHandler ) ;

    Result.strlength = 200 ;
    Result.strptr = malloc( 200 ) ;

    RexxStart( 0, NULL, argv[1], 0, hostname, RXCOMMAND,
               NULL, &returnCode, &Result ) ;
    if (hfp) {
      showpage(hfp);
      writeclose(hfp);
      fclose(hfp);
    }
    return 0;
  }
  printf("Usage: fem2d <script>\n");
  return 1;
}

APIRET APIENTRY fem2dHandler( 
   PRXSTRING command, 
   PUSHORT flags,
   PRXSTRING returnstring)
{
  FILE *fopen();
  char *aargs[MAXARG],*args0 = 0,*args1 = 0;
  int i,n,argn;

  if (command->strptr != NULL) {
    for (i=0;i<MAXARG;i++) _args[i] = aargs[i] = NULL;

    args0 = (char *)malloc((size_t)(command->strlength + 1));
    if (args0 != NULL) { 
      strcpy(args0,command->strptr);
      argn = parse(args0,_args,MAXARG);

      if ((n = getalias(_args[0])) != nalias) {
        args1 = (char *)malloc((size_t)(aliases[n].nchar));
        if (args1 == NULL) { 
          error("main - could not allocate memory for alias _args");
          if (args0) free(args0);
          return 0;
        }
        strcpy(args1,aliases[n].expr);
        argn = parse(args1,aargs,MAXARG);
      }
      else {
        for (i=0;i<argn;i++) aargs[i] = _args[i];
      }

      severity = RXSUBCOM_OK;
      field[0] = 0;

      i = hash(aargs[0]);
      (*commands[i])(argn,aargs);
    }
    else {
      severity = RXSUBCOM_ERROR;
      field[0] = 0;
    }
  }
  else {
    severity = RXSUBCOM_ERROR;
    field[0] = 0;
  }
  *flags = severity;
  if (field[0]) {
    MAKERXSTRING(result.shvvalue,field,strlen(field));
    RexxVariablePool(&result);
  }
  returnstring->strptr = NULL;
  returnstring->strlength = 0;
  if (args0) free(args0);
  if (args1) free(args1);
  return 0;
}

APIRET APIENTRY asinHandler( 
   PSZ name,
   ULONG argc,
   PRXSTRING argv, 
   PSZ queuename,
   PRXSTRING returnstring)
{
  float arg, val = 0.0;
  char strval[40];

  if (argc > 0) {
    sscanf(argv[0].strptr, "%g", &arg);
    val = (float)asin((double)arg);
  }
  sprintf(strval, "%g", val);
  strcpy(returnstring->strptr, strval);
  returnstring->strlength = strlen(strval);
  return 0L;
}

APIRET APIENTRY atanHandler( 
   PSZ name,
   ULONG argc,
   PRXSTRING argv, 
   PSZ queuename,
   PRXSTRING returnstring)
{
  float arg1, arg2, val = 0.0;
  char strval[40];

  if (argc > 1) {
    sscanf(argv[0].strptr, "%g", &arg1);
    sscanf(argv[1].strptr, "%g", &arg2);
    val = (float)atan2((double)arg1, (double)arg2);
  }
  sprintf(strval, "%g", val);
  strcpy(returnstring->strptr, strval);
  returnstring->strlength = strlen(strval);
  return 0L;
}

APIRET APIENTRY sinHandler( 
   PSZ name,
   ULONG argc,
   PRXSTRING argv, 
   PSZ queuename,
   PRXSTRING returnstring)
{
  float arg, val = 0.0;
  char strval[40];

  if (argc > 0) {
    sscanf(argv[0].strptr, "%g", &arg);
    val = (float)sin((double)arg);
  }
  sprintf(strval, "%g", val);
  strcpy(returnstring->strptr, strval);
  returnstring->strlength = strlen(strval);
  return 0L;
}

APIRET APIENTRY cosHandler( 
   PSZ name,
   ULONG argc,
   PRXSTRING argv, 
   PSZ queuename,
   PRXSTRING returnstring)
{
  float arg, val = 0.0;
  char strval[40];

  if (argc > 0) {
    sscanf(argv[0].strptr, "%g", &arg);
    val = (float)cos((double)arg);
  }
  sprintf(strval, "%g", val);
  strcpy(returnstring->strptr, strval);
  returnstring->strlength = strlen(strval);
  return 0L;
}

APIRET APIENTRY sqrtHandler( 
   PSZ name,
   ULONG argc,
   PRXSTRING argv, 
   PSZ queuename,
   PRXSTRING returnstring)
{
  extern double sqrt(double arg);
  float arg, val = 0.0;
  char strval[40];

  if (argc > 0) {
    sscanf(argv[0].strptr, "%g", &arg);
    val = (float)sqrt((double)arg);
  }
  sprintf(strval, "%g", val);
  strcpy(returnstring->strptr, strval);
  returnstring->strlength = strlen(strval);
  return 0L;
}

APIRET APIENTRY powHandler( 
   PSZ name,
   ULONG argc,
   PRXSTRING argv, 
   PSZ queuename,
   PRXSTRING returnstring)
{
  float arg1, arg2, val = 0.0;
  char strval[40];

  if (argc > 1) {
    sscanf(argv[0].strptr, "%g", &arg1);
    sscanf(argv[1].strptr, "%g", &arg2);
    val = (float)pow((double)arg1, (double)arg2);
  }
  sprintf(strval, "%g", val);
  strcpy(returnstring->strptr, strval);
  returnstring->strlength = strlen(strval);
  return 0L;
}
