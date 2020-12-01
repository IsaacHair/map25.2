Version 0:

Simple machine to simulate the map25.2 cpu inside whatever machine
you use to compile the program.

Note: It first converts the upload code into a file to represent the
actual position of things in ROM (meaning that the file is always 65536
instructions long). Note: it defaults to putting 0xff if no data is stated
for that space in rom. If ask for input, it will throw an error because
there is no input for this version. It will put the bus to 0xffff if there
is a dnc instruction, just like on the actual map 25.2.

Converter program:
The rom takes the form
of a single line of 655,560 hex characters. First, the file is filled with "f"
as this is the default for gls29ee010 EEPROMS.
Then, instructions (which are 10 nibbles long
each and thus take up 10 hex characters) are inserted in the corresponding
position.

Simulator program:
Just starts at address zero and reads the instructions as they come. Prints
values of registers and delta clocks continuously to terminal.

Note: I am coverting plot16.up. There is nothing significant about this file
per se, its just an easy example file that I know works. It will also allow
me to create an accurate measurement of the clock speed on the map25.2 because
the program takes 2,830.2 seconds to fully render all 76,800 pixels of the
mandelbrot set. The simulator will tell me the exact number of clock cycles
this took, and from there it is a simple matter of division to get instructions
per second.
