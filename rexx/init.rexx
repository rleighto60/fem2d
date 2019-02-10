/*

Code:       init.rexx
Author:     Russell Leighton
Revision:   16 Feb 1994

Comments:

This script sets up FEM2D on startup.  It is used by fem2d.rexx, but can be
executed  to  reset to an initial state.  This script sets up FEM2D for use
with structural analysis, plane strain problems.

*/

address fem2d

'reset all'

'set thermal 0'
'set stress 1'
'set strain 2'

'set cons 1'
'set flux 2'

/* iso(id,E,nu,alpha,Tref) */

'set la=a2*a3/((1+a3)*(1-plane*a3))'
'set mu=a2/(2*(1+a3))'
'set et=a4*a2/(1-plane*a3)'
'alias iso=mat(a1,0,la+2*mu,la+2*mu,la+2*mu,mu,la,la,0.0,la,0.0,0.0,thermal,et,et,a5)'

/* tri3(id,mid,n1,n2,n3) */

'alias tri3=elem(a1,a2,3,1,a3,a4,a5)'

/* tri6(id,mid,n1,n2,n3,n4,n5,n6) */

'alias tri6=elem(a1,a2,6,3,a3,a4,a5,a6,a7,a8)'

/* quad4(id,mid,n1,n2,n3,n4) */

'alias quad4=elem(a1,a2,4,4,a3,a4,a5,a6)'

/* quad8(id,mid,n1,n2,n3,n4,n5,n6,n7,n8) */

'alias quad8=elem(a1,a2,8,4,a3,a4,a5,a6,a7,a8,a9,a10)'

/* quad9(id,mid,n1,n2,n3,n4,n5,n6,n7,n8,n9) */

'alias quad9=elem(a1,a2,9,9,a3,a4,a5,a6,a7,a8,a9,a10,a11)'

/* dispx(nid,value) */

'alias dispx=pbc(a1,cons,1,a2)'

/* dispy(nid,value) */

'alias dispy=pbc(a1,cons,2,a2)'

/* forcx(nid,value) */

'alias forcx=pbc(a1,flux,1,a2)'

/* forcy(nid,value) */

'alias forcy=pbc(a1,flux,2,a2)'

/* dispr(nid,value) */

'alias dispr=pbc(a1,cons,1,a2)'

/* dispz(nid,value) */

'alias dispz=pbc(a1,cons,2,a2)'

/* forcr(nid,value) */

'alias forcr=pbc(a1,flux,1,a2)'

/* forcz(nid,value) */

'alias forcz=pbc(a1,flux,2,a2)'

/* slope(eid,side,value) */

'alias slope=dbc(a1,cons,1,a2,a3)'

/* press(eid,side,value) */

'alias press=dbc(a1,flux,1,a2,a3)'

/* tract(eid,side,value) */

'alias tract=dbc(a1,flux,2,a2,a3)'

'set scale 1'
'set material 1'
'set value 0'

'problem 0 2'

exit
