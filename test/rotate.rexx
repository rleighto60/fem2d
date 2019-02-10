/* rotating beam */
options results

address fem2d

call '../rexx/init'

problem 0 2 '"Rotating Beam"'

call '../rexx/pstress'

h = 0.5
L = 10.0
E1 = 1000
nu1 = 0.3000

iso 1 E1 nu1
do i=0 to 20
    xx = L*i/20
    node i+1 xx 0.0
    node i+31 xx h
end
do i=0 to 19
    quad4 i+1 1 i+1 i+2 i+32 i+31
end

dispx 1 0
dispy 1 0
alias 'rotx=pbc a1 1 1 "10*cos(a2)-0.5*sin(a2)-10"'
alias 'roty=pbc a1 1 2 "10*sin(a2)+0.5*cos(a2)-0.5"'
rotx 51 45
roty 51 45

save 'rotate.dat'

window '-2' '12' '-2' '12'
auto 'erase' 'off'
hard 'rotate.ps'

plot

du=1.0
do while du > 0.01
   address system 'nln2d rotate.dat' with output stem out.
   du = out.2
   read 'rotate.dat'
   plot 'deform'
end

fill 'deform'

check
say result

exit
