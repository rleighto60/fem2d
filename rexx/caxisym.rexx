/*

Code:       caxisym.rexx
Author:     Russell Leighton
Revision:   8 Jul 94

Comments:

This script sets up FEM2D for axisym problems with composite materials.  It
sets needed variables and defines useful stress/strain functions for
post-processing.

*/
options results

address fem2d

'info problem'
parse var result kind ndof nsoln nsets title

'problem 'kind' 2'

'axisym'
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
'set epsz=cos(m1)*cos(m1)*x1+sin(m1)*sin(m1)*y2+sin(m1)*cos(m1)*(x2+y1)'
'set epsn=cos(m1)*cos(m1)*y2+sin(m1)*sin(m1)*x1-sin(m1)*cos(m1)*(x2+y1)'
'set epst=t1'
'set gamzn=2*sin(m1)*cos(m1)*(y2-x1)+(cos(m1)*cos(m1)-sin(m1)*sin(m1))*(x2+y1)'

'set sigz=l1*epsz+l5*epsn+l8*t1+l10*gamzn'
'set sign=l5*epsz+l2*epsn+l6*t1+l9*gamzn'
'set sigt=l8*epsz+l6*epsn+l3*t1+l7*gamzn'
'set tauzn=l10*epsz+l9*epsn+l7*t1+l4*gamzn'

'set sigf=cos(m2)*cos(m2)*sigz+sin(m2)*sin(m2)*sigt'

'set fun'd2c(kind+65)'=epsz epsn epst gamzn sigz sign sigt tauzn sigf mang'
exit
