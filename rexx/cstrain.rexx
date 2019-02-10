/*

Code:       cstrain.rexx
Author:     Russell Leighton
Revision:   7 Jul 94

Comments:

This script sets up FEM2D for plane strain problems with composite
materials.  It sets needed variables and defines useful stress/strain
functions for post-processing.

*/
options results

address fem2d

'info problem'
parse var result kind ndof nsoln nsets title

'problem 'kind' 2'

'plane'
'set plane strain'

/* tiso(id,E1,E2,nu12,nu23,G12) */

'set cmu=a4*a4*a3/a2'
'set del1=1-a5-2*cmu'
'set del2=(1+a5)*del1'
'set ca=a2*(1-a5)/del1'
'set cb=a3*(1-cmu)/del2'
'set cc=a3*a4/del1'
'set cd=a3*(a5-cmu)/del2'
'alias tiso=mat(a1,1,ca,cb,cb,a6,cc,cd,0.0,cc,0.0,0.0)'

'set mang=m1'
'set epst=cos(m1)*cos(m1)*x1+sin(m1)*sin(m1)*y2+sin(m1)*cos(m1)*(x2+y1)'
'set epsn=cos(m1)*cos(m1)*y2+sin(m1)*sin(m1)*x1-sin(m1)*cos(m1)*(x2+y1)'
'set epsz=0.0'
'set gamtn=2*sin(m1)*cos(m1)*(y2-x1)+(cos(m1)*cos(m1)-sin(m1)*sin(m1))*(x2+y1)'

'set sigt=l1*epst+l5*epsn+l10*gamtn'
'set sign=l5*epst+l2*epsn+l9*gamtn'
'set sigz=l8*epst+l6*epsn+l7*gamtn'
'set tautn=l10*epst+l9*epsn+l4*gamtn'

'set sigf=cos(m2)*cos(m2)*sigt+sin(m2)*sin(m2)*sigz'

'set fun'd2c(kind+65)'=epst epsn gamtn sigt sign sigz tautn sigf mang'
exit
