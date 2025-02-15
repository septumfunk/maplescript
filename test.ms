#const nullptr pointer 0
#const ptri pointer 1

#const H byte 72
#const e byte 101
#const l byte 108
#const o byte 111
#const w byte 119
#const r byte 114
#const d byte 100

#const comma byte 44
#const space byte 32
#const bang byte 33
#const null byte 0
#const lb byte 10

aloc 15
ldct nullptr

ref 0
ref 1
copy 3 2 8
pop 2
ldct H
ref 2
copy 1 3 1
pop 2

ldct ptri
add 1 2
pop 1
ldct e
ref 2
copy 1 3 1
pop 2

ldct ptri
add 1 2
pop 1
ldct l
ref 2
copy 1 3 1
pop 2

ldct ptri
add 1 2
pop 1
ldct l
ref 2
copy 1 3 1
pop 2

ldct ptri
add 1 2
pop 1
ldct o
ref 2
copy 1 3 1
pop 2

ldct ptri
add 1 2
pop 1
ldct comma
ref 2
copy 1 3 1
pop 2

ldct ptri
add 1 2
pop 1
ldct space
ref 2
copy 1 3 1
pop 2

ldct ptri
add 1 2
pop 1
ldct w
ref 2
copy 1 3 1
pop 2

ldct ptri
add 1 2
pop 1
ldct o
ref 2
copy 1 3 1
pop 2

ldct ptri
add 1 2
pop 1
ldct r
ref 2
copy 1 3 1
pop 2

ldct ptri
add 1 2
pop 1
ldct l
ref 2
copy 1 3 1
pop 2

ldct ptri
add 1 2
pop 1
ldct d
ref 2
copy 1 3 1
pop 2

ldct ptri
add 1 2
pop 1
ldct bang
ref 2
copy 1 3 1
pop 2

ldct ptri
add 1 2
pop 1
ldct lb
ref 2
copy 1 3 1
pop 2

pop 1
puts 0
free 0
pop 1
ret

main:
    ldct 0
    prnt 0
    ret