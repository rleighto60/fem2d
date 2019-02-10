/*

Code:       axisym.rexx
Author:     Russell Leighton
Revision:   16 Feb 1994

Comments:

This  script  sets  up FEM2D for axisym problems.  It sets needed variables
and defines useful stress/strain functions for post-processing.

*/
options results

address fem2d

'info problem'
parse var result kind ndof nsoln nsets title

'problem 'kind' 2'

'axisym'
'set plane strain'

'set epsr=r1'
'set epsz=z2'
'set epst=t1'
'set gamrz=r2+z1'
'set gam12=(0.25*(epsr-epsz)^2+0.25*gamrz^2)^0.5'
'set eps1=0.5*(epsr+epsz)+gam12'
'set eps2=0.5*(epsr+epsz)-gam12'
'set eps3=t1'

'set sigr=c1*r1+c5*z2+c8*t1+c10*gamrz-f1'
'set sigz=c5*r1+c2*z2+c6*t1+c9*gamrz-f2'
'set sigt=c8*r1+c6*z2+c3*t1+c7*gamrz-f1'
'set taurz=c10*r1+c9*z2+c7*t1+c4*gamrz'
'set tau12=(0.25*(sigr-sigz)^2+taurz^2)^0.5'
'set sig1=0.5*(sigr+sigz)+tau12'
'set sig2=0.5*(sigr+sigz)-tau12'
'set sig3=sigt'
'set sigh=(sigr+sigz+sigt)/3'
'set sigd=sig1-sigh'
'set sigv=((sigr*sigz+sigz*sigt+sigt*sigr)/3)^0.5'
'set siga=sigv/abs(sigh)'

'set fun'd2c(kind+65)'=epsr,epsz,epst,gamrz,eps1,eps2,eps3,gam12,sigr,sigz,sigt,taurz,sig1,sig2,sig3,tau12,sigh,sigd,sigv,siga'
exit
