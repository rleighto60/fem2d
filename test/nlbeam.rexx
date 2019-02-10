/* simple cantilever beam with large deformation.
   note: this script should be executed standalone, 
   i.e. type 'rx nlbeam' at the CLI prompt */

options results

address fem2d

call '../rexx/init'

problem 0 2 '"End-Loaded Cantilever Beam"'

call '../rexx/plstress'

h = 0.1
L = 10.0
E1 = 1.0e8
nu1 = 0.000

iso 1 E1 nu1

do i=0 to 20
    xx = L*i/20
    node i+1 xx 0.0
    if i//2 = 0 then node i+31 xx h/2
    node i+61 xx h
end
do j=0 to 9
    i = j*2
    quad8 j+1 1 i+1 i+3 i+63 i+61 i+2 i+33 i+62 i+31
end

dispx 1 0.0
dispy 1 0.0
dispx 31 0.0
dispy 31 0.0
dispx 61 0.0
dispy 61 0.0

window '-2' '10' '-10' '2'
auto 'erase' 'off'
hard 'nlbeam.ps'

plot

/* call open('dxout','ram:dx.out','W') */
/* call open('dyout','ram:dy.out','W') */

I = h*h*h/12

do f=50 to 800 by 50
    say 'appling load of 'f' to end of beam'
    nload = f*L*L/(E1*I)

    forcy 81 0-f

    save 'nlbeam.dat'

    du=1.0
    do while du > 0.0001
        address system 'nln2d nlbeam.dat' with output stem out.
        du = out.2
    end
    read 'nlbeam.dat'
    plot 'deform'

/*
    info soln 1 '-21'
    parse var result nd id dx
    info soln 2 '-21'
    parse var result nd id dy
    call writeln('dxout',1+dx/L' 'nload)
    call writeln('dyout',0-dy/L' 'nload)
*/
end

plot 'bcs'
fill 'deform'

/*
call close('dxout')
call close('dyout')
*/
exit
