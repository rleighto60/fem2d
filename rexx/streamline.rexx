/*

Code:       stream.rexx
Author:     Russell Leighton
Revision:   6 Feb 1995

Comments:

This script sets up FEM2D for streamline flow problems.

*/
options results

address fem2d

'info problem'
parse var result kind ndof nsoln nsets title

'problem 'kind' 1'

'mat(1,0,1.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,1.0)'

/* streamline(nid,value) */

'alias streamline=pbc(a1,cons,1,a2)'

/* velocity(eid,side,value) */

'alias velocity=dbc(a1,flux,1,a2,a3)'

'set psi=u1'
'set vel=(x1*x1+y1*y1)^.5'
'set theta=atan(-x1,y1)'

'set fun'd2c(kind+65)'=psi vel theta'
exit
