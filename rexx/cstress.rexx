/*

Code:       cstress.rexx
Author:     Russell Leighton
Revision:   16 Feb 1994

Comments:

This script sets up FEM2D for plane stress problems for use with the linear
solver  (lin2d).  It sets needed variables and defines useful stress/strain
functions for post-processing.

*/
options results

address fem2d

'info problem'
parse var result kind ndof nsoln nsets title

'problem 'kind' 2'

'plane'
'set plane stress'

/* lamina(id,E1,E2,nu12,G12) */

'set del=1-a4*a4*a3/a2'
'alias lamina=mat(a1,1,a2/del,a3/del,0.0,a5,a4*a3/del,0.0,0.0,0.0,0.0,0.0)'

'set ta=m1*pi/180'
'set epst=cos(ta)*cos(ta)*x1+sin(ta)*sin(ta)*y2+sin(ta)*cos(ta)*(x2+y1)'
'set epsn=cos(ta)*cos(ta)*y2+sin(ta)*sin(ta)*x1-sin(ta)*cos(ta)*(x2+y1)'

exit
