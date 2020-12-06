The assembler takes an assembly file as input and converts it into upload code.
The romsend program interfaces with the arduino program over a usb interface to
write the code to the gls29ee010's for use on the map25.2.
Note that the arduino program with _highlowprog appended to the name
is used if you want to select the upper most bit of the rom; the other
program just sets this bit to zero. The other version may not interface well
with the romsend program.

To quickly assemble and send an assembly file defined using c
(eg a twice removed assembly file), execute the script create.sh
Note that this script expects the c file to take two arguments, the first
being the path for the assembly file and the second being the path for the
assembly file with the \x88 stuff.
Example:
	compile.sh plot16
Is equivalent to:
	gcc plot16.c -o plot16
	./plot16 plot16.asm plot16.x88
	assemble plot16.asm plot16.up plot16.buff
	romsend plot16.up
If you don't want to send the file, just do ^C once the romsend program
begins.

Note: make sure that "romsend", "assemble", and "create.sh" are in the
scripts folder for the bash terminal. Probably have to do chmod stuff for
create.sh
