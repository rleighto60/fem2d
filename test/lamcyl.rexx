/* test of a transformed orthotropic material */

options results

address fem2d

call '../rexx/init'

problem 0 2 '"Pressurized Filament Wound Cylinder"'

call '../rexx/cstrain'

tiso 1 181.0 10.3 0.23 0.70 7.17

lam 2 1 1/2 ' 45.0'
lam 2 1 1/2 '-45.0'

coord 'polar'

do r=10 to 10.25 by 0.125
    do ang=90 to 0 by -9
        node 0 r ang
    end
end

do j=0 to 4
    i = j*2
    quad8 j+1 2 i+1 i+3 i+25 i+23 i+2 i+14 i+24 i+12
    press j+1 1 0-0.1
end
dispx 1 0.0
dispx 12 0.0
dispx 23 0.0
dispy 11 0.0
dispy 22 0.0
dispy 33 0.0

find
hard 'lamcyl.ps'
auto 'erase' 'off'
fill 'model'
plot 'bcs'

save 'lamcyl.dat'

address system 'lin2d lamcyl.dat'

read 'lamcyl.dat'

clear
find
zoom 1.2
fill 'model'
plot 'deform'
plot 'bcs'
exit
