/* simple solid rocket motor */
options results

address fem2d

call '../rexx/init'

pi = 3.141592654
rb = 6.0
ra = 1.0
rs = 4.8
w2 = 0.2
h = 0.05
E = 1500.0
nu = 0.4900
Et = 500.0
nut = 0.4500
alp = 6.0e-6
Ec = 10.0e6
nuc = 0.3000
alpc = 5.0e-5
P = -1000.0
Tref = 140
To = 77
T = 500

a1 = asin(w2/ra)
a2 = pi/4 - a1
da = (a2 - a1)/16
b1 = asin(w2/rs)
b2 = pi/4 - b1
db = (b2 - b1)/16
x1 = ra*cos(a1)
y1 = ra*sin(a1)
x2 = rs
y2 = w2
g = 1.5
i = line(x1,y1,x2,y2,g,1,21,1)
xc = rs
yc = 0.0
i = arc(xc,yc,pi/2,0.0,w2,21,9,1)
x1 = ra*cos(a1 + da)
y1 = ra*sin(a1 + da)
x2 = rs*cos(b1 + db)
y2 = rs*sin(b1 + db)
i = line(x1,y1,x2,y2,g,41,11,2)
i = ltip(xc,yc,x2,y2,0.0,0.25,w2,rb,61,3,2)
x1 = ra*cos(a1 + 2*da)
y1 = ra*sin(a1 + 2*da)
x2 = rs*cos(b1 + 2*db)
y2 = rs*sin(b1 + 2*db)
i = line(x1,y1,x2,y2,g,81,21,1)
i = ltip(xc,yc,x2,y2,0.0,0.5,w2,rb,101,5,1)
x1 = ra*cos(a1 + 3*da)
y1 = ra*sin(a1 + 3*da)
x2 = rs*cos(b1 + 3*db)
y2 = rs*sin(b1 + 3*db)
i = line(x1,y1,x2,y2,g,121,11,2)
i = ltip(xc,yc,x2,y2,0.0,0.75,w2,rb,141,3,2)
x1 = ra*cos(a1 + 4*da)
y1 = ra*sin(a1 + 4*da)
x2 = rs*cos(b1 + 4*db)
y2 = rs*sin(b1 + 4*db)
i = line(x1,y1,x2,y2,g,161,21,1)
i = ltip(xc,yc,x2,y2,0.0,1.0,w2,rb,181,5,1)
i = arc(0.0,0.0,pi/16,0.0,rb + h/2,190,3,2)
i = arc(0.0,0.0,pi/16,0.0,rb + h,195,5,1)

x1 = ra*cos(a1 + 6*da)
y1 = ra*sin(a1 + 6*da)
x2 = rs*cos(3*pi/32)
y2 = rs*sin(3*pi/32)
i = line(x1,y1,x2,y2,g,201,11,2)
x1 = x2
y1 = y2
x2 = rb*cos(3*pi/32)
y2 = rb*sin(3*pi/32)
i = line(x1,y1,x2,y2,1.0,221,3,2)
x1 = x2
y1 = y2
x2 = (rb + h)*cos(3*pi/32)
y2 = (rb + h)*sin(3*pi/32)
i = line(x1,y1,x2,y2,1.0,225,2,2)

x1 = ra*cos(a1 + 8*da)
y1 = ra*sin(a1 + 8*da)
x2 = rs*cos(pi/8)
y2 = rs*sin(pi/8)
i = line(x1,y1,x2,y2,g,241,21,1)
x1 = x2
y1 = y2
x2 = rb*cos(pi/8)
y2 = rb*sin(pi/8)
i = line(x1,y1,x2,y2,1.0,261,5,1)
x1 = x2
y1 = y2
x2 = (rb + h)*cos(pi/8)
y2 = (rb + h)*sin(pi/8)
i = line(x1,y1,x2,y2,1.0,265,3,1)

do j=1 to 14
    n = j*2 - 1
    quad8 j    1 n n+2 n+82 n+80 n+1 n+42 n+81 n+40
end
do j=1 to 14
    n = j*2 + 79
    quad8 j+20 1 n n+2 n+82 n+80 n+1 n+42 n+81 n+40
end
quad8 35 2 187 189 199 197 188 194 198 192
quad8 36 2 185 187 197 195 186 192 196 190
do j=1 to 12
    n = j*2 + 159
    quad8 j+40 1 n n+2 n+82 n+80 n+1 n+42 n+81 n+40
end
quad8 53 2 265 185 195 267 225 190 227 266

problem 0 1 '"Heat Transfer"'

call '../rexx/pthermal'

therm 1 0.5
therm 2 1.0

do j=1 to 29
    tempr j T
end
do j=195 to 199
    tempr j To
end
tempr 227 To
tempr 267 To
do j=41 to 241 by 40
    tempr j T
end

save 'srm4seg.dat'

address system 'lin2d srm4seg.dat'

read 'srm4seg.dat'

'select temp node'
'select tflux ave'
'select theta ave'

save 'srm4seg.dat'

address system 'post2d srm4seg.dat'

read 'srm4seg.dat'

find
zoom 1.2
auto 'erase' 'off'
hard 'srm4seg.ps'
plot 'result' 'temp' 4
plot 'bcs'

problem 1 2 '"Thermal Load"'

call '../rexx/pstrain'

iso 1 Et nut alp Tref
iso 2 Ec nuc alpc Tref
do j=41 to 52
    slope j 3 0.0
end
slope 53 4 0.0
do j=29 to 189 by 40
    dispy j 0.0
end
dispy 194 0.0
dispy 199 0.0

save 'srm4seg.dat'

address system 'lin2d srm4seg.dat'

read 'srm4seg.dat'

'select eps1 node'

save 'srm4seg.dat'

address system 'post2d srm4seg.dat'

read 'srm4seg.dat'

clear
find
zoom 1.2
fill 'result' 'eps1' 4
plot 'deform'
plot 'bcs'

problem 2 2 '"Internal Pressure"'

call '../rexx/pstrain'

iso 1 E nu
iso 2 Ec nuc
do j=1 to 14
    press j 1 P
end
do j=1 to 41 by 20
    press j 4 P
end
do j=41 to 52
    slope j 3 0.0
end
slope 53 4 0.0
do j=29 to 189 by 40
    dispy j 0.0
end
dispy 194 0.0
dispy 199 0.0

save 'srm4seg.dat'

address system 'lin2d srm4seg.dat'

read 'srm4seg.dat'

'select eps1 node'

save 'srm4seg.dat'

address system 'post2d srm4seg.dat'

read 'srm4seg.dat'

check
say result

clear
find
zoom 1.2
fill 'result' 'eps1' 4
plot 'deform'
plot 'bcs'
exit

line:
arg xa,ya,xb,yb,gg,i1,n,di

n1 = di*(n - 1)
node i1 xa ya
do i=di to n1  by di
    j = i1 + i
    xn = xa + (xb-xa)*pow(i/n1,gg)
    yn = ya + (yb-ya)*pow(i/n1,gg)
    node j xn yn
end
return j

arc:
arg cx,cy,aa,ab,r1,i1,n,di

n1 = di*(n - 1)
do i=0 to n1  by di
    j = i1 + i
    an = aa + (ab-aa)*i/n1
    xn = r1*cos(an) + cx
    yn = r1*sin(an) + cy
    node j xn yn
end
return j

ltip:
arg xc,yc,x1,y1,ab,fn,w2,rb,i1,n,di

xa = w2*cos(ab + pi/4) + xc
ya = w2*sin(ab + pi/4) + yc
xb = rb*cos(ab + pi/16)
yb = rb*sin(ab + pi/16)
xx = xa + (xb-xa)*fn
yy = ya + (yb-ya)*fn
i = line(x1,y1,xx,yy,1.0,i1,n,di)
rr = sqrt(xx*xx + yy*yy)
aa = atan(yy,xx)
i = arc(0.0,0.0,aa,ab,rr,i,n,di)
return i
