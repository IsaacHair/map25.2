convert01 is convert and vm03 is vm
These bois will be added to the /bin folder so they can be used
from anywhere in the terminal.
Actually, on second thought, this is very dangerous because
I will inevitably forget to update the files and it will take
15 minutes to figure out why the hell vm isn't executing properly
when I "clearly updated it" -- you know how it goes.
I'm just going to put convert01 and vm03 in the /bin folder
and the root folder of this repo as well.

Simple machine to simulate the map25.2 cpu inside whatever machine
you use to compile the program.

Note: It first converts the upload code into a file to represent the
actual position of things in ROM (meaning that the file is always 65536
instructions long). Note: it defaults to putting 0xff if no data is stated
for that space in rom. If ask for input, it will throw an error because
there is no input for this version. It will put the bus to 0xffff if there
is a dnc instruction, just like on the actual map 25.2.

Converter program v0:
The rom takes the form
of a single line of 655,560 hex characters. First, the file is filled with "f"
as this is the default for gls29ee010 EEPROMS.
Then, instructions (which are 10 nibbles long
each and thus take up 10 hex characters) are inserted in the corresponding
position.

Simulator program v0:
Just starts at address zero and reads the instructions as they come. Prints
values of registers and delta clocks continuously to terminal.

Note: I am coverting plot16.up. There is nothing significant about this file
per se, its just an easy example file that I know works. It will also allow
me to create an accurate measurement of the clock speed on the map25.2 because
the program takes 2,830.2 seconds to fully render all 76,800 pixels of the
mandelbrot set. The simulator will tell me the exact number of clock cycles
this took, and from there it is a simple matter of division to get instructions
per second. Note that the measurement of 2,830.2 seconds was recorded with:
	Vpp = 2.825 +/- 0.025 volts
	Temperature = 70. degrees F +/- 2 degrees F

Plot16_copy results (decimal values): Delta_t on the map25.2 is 2830.2 seconds.
delta_clocks on the simulation is exactly 1,290,638,025 instructions (this is
the count of total instructions, starting with curr 0x0000, for the curr
register to equal the address of the "halt" location on the phase where nxt is
written. This means that the speed based on this test is 456.02 kHz average.
However, I suspect that it fluxuates up and down by as much as 1 percent
because the not gate oscillator is not very stable. The measurement of 456 kHz
from the P2 Oscilloscope was suprisingly accurate.

Converter v1:

The .up file has the following transistions in the data it is transmitting:
START -> PAGE ADDRESS
PAGE ADDRESS -> LINE AND DATA
LINE AND DATA -> LINE AND DATA
LINE AND DATA -> NEXT PAGE INDICATOR
NEXT PAGE INDICATOR -> PAGE ADDRESS
NEXT PAGE INDICATOR -> NEXT CHIP INDICATOR
NEXT CHIP INDICATOR -> PAGE ADDRESS
NEXT CHIP INDICATOR -> EOF

Page address to next page indicator since a page address is only provided if
it contains data.

This version will basically create a state machine.
I don't know if it is safe to call functions several thousand layers deep but
whatevs it seems to work.
