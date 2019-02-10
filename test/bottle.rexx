/* internally pressurized filament wound bottle */

options results

address fem2d

call '../rexx/init'

problem 0 2 '"Pressurized Filament Wound Bottle"'

call '../rexx/caxisym'

tiso 1 26.25e6 1.50e6 0.23 0.70 1.0e6

set 'fang=asin(1.7365/r0)'
lam 2 1 1 'fang'
lam 3 1 2/3 'fang'
lam 3 1 1/3 90.0

do i=0 to 2
    coord polar 0 10
    r = i*0.125 + 10
    do ang=80 to 0 by -8
        node 0 r ang
    end
    coord cart
    x = i*0.1875 + 10
    do y=9 to 0 by -1
        node 0 x y
    end
end

do j=0 to 4
    i = j*2
    quad8 j+1 2 i+1 i+3 i+45 i+43 i+2 i+24 i+44 i+22
    press j+1 1 '-2000'
end
do j=5 to 9
    i = j*2
    quad8 j+1 3 i+1 i+3 i+45 i+43 i+2 i+24 i+44 i+22
    press j+1 1 '-2000'
end

dispz 21 '0.0'
dispz 42 '0.0'
dispz 63 '0.0'

find
hard 'bottle.ps'
auto 'erase' 'off'
fill 'model'
plot 'bcs'

save 'bottle.dat'

address system 'lin2d bottle.dat'

read 'bottle.dat'

clear
find
zoom 1.2
fill 'model'
plot 'deform'
plot 'bcs'
exit
