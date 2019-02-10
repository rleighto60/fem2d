/*  */
options results

address fem2d

call '../rexx/init'

problem 0 1 '"Heat Transfer"'

call '../rexx/pthermal'

therm 1 100

h = 0.25

do j=0 to 10
   do i=1 to 11
      n = j*20 + i
      node n (i-1)*h j*h
   end
end
do j=0 to 4
   do i=1 to 5
      n = j*5 + i
      m = (j*20 + (i-1))*2 + 1
      quad8 n 1 m m+2 m+42 m+40 m+1 m+22 m+41 m+20
   end
end

tempr 85 1000
tempr 167 0

save 'plate.dat'

address system 'lin2d plate.dat'

read 'plate.dat'

'select' 'temp' 'node'

save 'plate.dat'

address system 'post2d plate.dat'

read 'plate.dat'

window '-0.1' '2.6' '-0.1' '2.6'
hard 'plate.ps'
auto 'erase' 'off'
fill 'result' 'temp' 4
exit
