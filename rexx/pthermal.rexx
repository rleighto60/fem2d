/*

Code:       pthermal.rexx
Author:     Russell Leighton
Revision:   6 Feb 1995

Comments:

This script sets up FEM2D for thermal heat transfer problems for use with
the linear solver (lin2d).

*/
options results

address fem2d

'info problem'
parse var result kind ndof nsoln nsets title

'set thermal 'kind
'problem 'kind' 1'

/* therm(id,k) */

'alias therm=mat(a1,0,a2,0.0,0.0,a2,0.0,0.0,0.0,0.0,0.0,0.0)'

/* tempr(nid,value) */

'alias tempr=pbc(a1,cons,1,a2)'

'set temp=u1'
'set tflux=(x1*x1+y1*y1)^.5'
'set theta=atan(y1,x1)'

'set fun'd2c(kind+65)'=temp,tflux,theta'
