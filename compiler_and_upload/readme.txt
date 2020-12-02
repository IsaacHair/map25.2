The assembler takes an assembly file as input and converts it into upload code.
The romsend program interfaces with the arduino program over a usb interface to
write the code to the gls29ee010's for use on the map25.2.
Note that the arduino program with _highlowprog appended to the name
is used if you want to select the upper most bit of the rom; the other
program just sets this bit to zero. The other version may not interface well with
the romsend program.
