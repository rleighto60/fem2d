/*

Code:       pstrain.rexx
Author:     Russell Leighton
Revision:   16 Feb 1994

Comments:

This script sets up FEM2D for plane strain problems for use with the linear
solver  (lin2d).  It sets needed variables and defines useful stress/strain
functions for post-processing.

*/
options results

address fem2d

'info problem'
parse var result kind ndof nsoln nsets title

'problem 'kind' 2'

'plane'
'set plane strain'

'set epsbar=0.0'
'set epsx=x1'
'set epsy=y2'
'set epsz=epsbar'
'set gamxy=x2+y1'
'set gam12=(0.25*(epsx-epsy)^2+0.25*gamxy^2)^0.5'
'set eps1=0.5*(epsx+epsy)+gam12'
'set eps2=0.5*(epsx+epsy)-gam12'
'set eps3=epsbar'
'set edir=0.5*atan(gamxy,(epsx-epsy))'

'set sigx=c1*x1+c5*y2+c8*epsbar+c10*gamxy-f1'
'set sigy=c5*x1+c2*y2+c6*epsbar+c9*gamxy-f2'
'set sigz=c8*x1+c6*y2+c3*epsbar+c7*gamxy-f1'
'set tauxy=c10*x1+c9*y2+c7*epsbar+c4*gamxy'
'set tau12=(0.25*(sigx-sigy)^2+tauxy^2)^0.5'
'set sig1=0.5*(sigx+sigy)+tau12'
'set sig2=0.5*(sigx+sigy)-tau12'
'set sig3=sigz'
'set sigh=(sigx+sigy+sigz)/3'
'set sigd=sig1-sigh'
'set sigv=((sigx*sigy+sigy*sigz+sigz*sigx)/3)^0.5'
'set siga=sigv/abs(sigh)'
'set sdir=0.5*atan(2*tauxy,(sigx-sigy))'

'set fun'd2c(kind+65)'=epsx,epsy,gamxy,eps1,eps2,gam12,sigx,sigy,sigz,tauxy,sig1,sig2,tau12,sigh,sigd,sigv,siga'
exit
