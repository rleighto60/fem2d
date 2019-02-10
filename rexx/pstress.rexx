/*

Code:       pstress.rexx
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
'problem(struct)'
'set plane stress'

/* lamina(id,E1,E2,nu12,G12) */

'set del=1-a4*a4*a3/a2'
'alias lamina=mat(a1,1,0,a2/del,a3/del,0.0,a5,a4*a3/del,0.0,0.0,0.0,0.0,0.0)'

'set epsx=x1'
'set epsy=y2'
'set epsz=-0.5*c5*(x1+y2)/c4'
'set gamxy=x2+y1'
'set gam12=(0.25*(epsx-epsy)^2+0.25*gamxy^2)^0.5'
'set eps1=0.5*(epsx+epsy)+gam12'
'set eps2=0.5*(epsx+epsy)-gam12'
'set eps3=epsz'
'set edir=0.5*atan(gamxy,(epsx-epsy))'

'set sigx=c1*x1+c5*y2+c10*gamxy-f1'
'set sigy=c5*x1+c2*y2+c9*gamxy-f2'
'set sigz=0.0'
'set tauxy=c10*x1+c9*y2+c4*gamxy'
'set tau12=(0.25*(sigx-sigy)^2+tauxy^2)^0.5'
'set sig1=0.5*(sigx+sigy)+tau12'
'set sig2=0.5*(sigx+sigy)-tau12'
'set sig3=0.0'
'set sigh=(sigx+sigy+sigz)/3'
'set sigd=sig1-sigh'
'set sdir=0.5*atan(2*tauxy,(sigx-sigy))'

'set fun'd2c(kind+65)'=epsx,epsy,epsz,gamxy,eps1,eps2,gam12,sigx,sigy,tauxy,sig1,sig2,tau12,sigh,sigd'
exit
