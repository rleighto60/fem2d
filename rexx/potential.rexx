/*

Code:       potential.rexx
Author:     Russell Leighton
Revision:   6 Feb 1995

Comments:

This script sets up FEM2D for potential flow problems.

*/
options results

address fem2d

'info problem'
parse var result kind ndof nsoln nsets title

'problem 'kind' 1'

'mat(1,0,1.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,1.0)'

/* source(nid,value) */

'alias source=pbc(a1,cons,1,-a2)'

/* sink(nid,value) */

'alias sink=pbc(a1,cons,1,a2)'

/* velocity(eid,side,value) */

'alias velocity=dbc(a1,flux,1,a2,-a3)'

'set phi=u1'
'set vel=(x1*x1+y1*y1)^.5'
'set theta=atan(y1,x1)'

'set fun'd2c(kind+65)'=phi vel theta'
exit

