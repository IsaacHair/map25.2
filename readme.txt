**The readme.txt's in this repo will also act as changelog.txt's so that
everything is visible easily**

map25.2 assembly:
This is basically just machine code.
instruction looks like: XXXX <mnemonic> XXXX XXXX
The first set of X's is the address to store the instruction at.
The second set of X's is the immediate value for the instruction.
The third set of X's is the address for the next instruction.
The mnemonic represents the op code without having to write
the op code out in raw binary or hex values.

updated instruction:
XXXX <bus connection mnemonic> <write/jump/no-op mnemonic> XXXX XXXX
X's correspond to the same things as above.
Also, a line with only "<" begins a comment (start the comment
on the next line) and ">" ends the comment (put the ">" on its
own line and start the code after that line).
Note: comment should not contain ">" in the actual content of
the comment.

bus mnemonics (upper nibble):
0x0 imm
0x1 adr
0x2 gen
0x3 rol
0x4 ror
0x5 out
0x6 dir
0x7 inp
0x8 ram
0x9-0xf (not used)

write/jump/no-op mnemonics (lower nibble):
0x0 jzor
0x1 asnx
0x2 out0
0x3 out1
0x4 adr0
0x5 adr1
0x6 dir0
0x7 dir1
0x8 gen0
0x9 gen1
0xa noop
0xb (not used)
0xc (not used)
0xd rlow
0xe rupp
0xf rall

***NOTE: Rom chip zero is the RIGHTMOST chip and rom chip four is the LEFTMOST.

Using *.u25 for upload code for EEPROMS (ie a stream of hex digits with
a page and data for that page).
Using *.a25 for assembly code.

Updated mnemonics:
Upper nibble
0x0 imm
0x1 adr
0x2 gen
0x3 rol
0x4 ror
0x5 out
0x6 dir
0x7 inp
0x8 ram
0x9-0xf d/c (bus is pulled high by pull-up resistors)

lower nibble:
0x0 jzor
0x1 asnx
0x2 out0
0x3 out1
0x4 adr0
0x5 adr1
0x6 dir0
0x7 dir1
0x8 gen0
0x9 gen1
0xa noop
0xb noop
0xc noop
0xd rlow
0xe rupp
0xf rall

So the extra commands are ones that are physically possible, but they
are never implemented unless the user manually specifies them
(for example, the assembly language only needs one code for noop,
not three, so it defaults to 0xa)

There is a command for d/c in the latest version,
which corresponds to 0x9.

Ok basically I'm going to update the mnemonics and then make a
full list of mnemonics with functional descriptions.
A single character is also considered a mnemonic and it just
directly corresponds to the machine code; ie typing "7" is the same
as typing "dir1".

Also, the .a25 file extension is used to denote assembly files
and the .u25 file extension denotes machine code files that have
been organized into the eeprom page data format for upload.
These extensions are not required by any means, it just
makes things more organized when you read over the file contents.


ACTUAL COMPLETE LIST OF OP CODES:
upper 4 bits:
0x0 => "imm", "0" (immediate value is put on bus)
0x1 => "adr", "addr", "1" (address register is put on bus)
0x2 => "gen", "2" (general register is put on bus)
0x3 => "rol", "3" (general register rotated left is put on bus)
0x4 => "ror", "4" (general register rotated right is put on bus)
0x5 => "out", "5" (output register is put on bus)
0x6 => "dir", "6" (direction register is put on bus)
0x7 => "inp", "in", "7" (input is put on bus)
0x8 => "ram", "8" (value of ram at the address register address is put on bus)
0x9 => "dnc", "nc", "9" (bus is not connected, so it is pulled high)
0xa => "a" (same as 0x9)
0xb => "b" (same as 0x9)
0xc => "c" (same as 0x9)
0xd => "d" (same as 0x9)
0xe => "e" (same as 0x9)
0xf => "f" (same as 0x9)
lower 4 bits:
0x0 => "jzor", "jnz", "jumpor", "0" (the bus and immediate are bitwise "and"-ed,
then the result is "or"-ed with itself and this is used as the last bit
of the next program address value)
0x1 => "asnx", "asnnext", "1" (next program address is set equal to bus value)
0x2 => "out0", "2" (output register is set to zero with bus as mask)
0x3 => "out1", "3" (output register is set to one with bus as mask)
0x4 => "adr0", "addr0", "4" (address register is set to zero with bus as mask)
0x5 => "adr1", "addr1", "5" (address register is set to one with bus as mask)
0x6 => "dir0", "6" (direction register is set to zero with bus as mask)
0x7 => "dir1", "7" (direction register is set to one with bus as mask)
0x8 => "gen0", "8" (general register is set to zero with bus as mask)
0x9 => "gen1", "9" (general register is set to one with bus as mask)
0xa => "noop", "a" (no operation is performed)
0xb => "b" (same as 0xa)
0xc => "c" (same as 0xa)
0xd => "rlow", "ramlow", "d" (ram lower byte at address register
address is set to the value of the lower byte of the bus)
0xe => "rupp", "ramup", "rup", "e" (ram upper byte at address register
address is set to the value of the upper byte of the bus)
0xf => "rall", "ramall", "f" (ram upper and lower byte at address register
address is set to the value of the bus)


Wow, it has been a while since I updated the root readme.txt. Well,
what I have done since then is invent some more complex assembly code
procedures. Also note that the VMA412 is being used as a display
and a peripheral graphics card. I might actually get a gtx1060 or
whatevery the bois are using these days and add that to the output
bus just to make arithmatic operations stupidly fast. A USB card
might also be useful.

The core paradigm is as follows:
	-CPU: MAP 25.2, obviously, with its 2,861 beautiful transistors
	and entirely discrete construction. It includes a clock, reset
	interrupt handler, input bus, tri-state output bus, and internal
	registers and circuitry to do shit.
	-RAM: Cypress Semiconductor CY62126EV30LL-45ZSXI, which is integrated
	-ROM: Greenliant GLS29EE010, which is integrated
Peripherals currently in use:
	-Display: VMA412, which uses the ILI9341 "video card"
	-Keyboard: 4x3 Matrix Array 12 Key Membrane Switch Keyboard Keypad
	-Speaker: Literally just connect output to speaker channel thru
	a resistor and ensure common ground.

Here is a datasheet I have created for the cpu:
*insert here*

Here is a "thesis" I am working on to document some of the important shit I have invented:
*insert here*

Here are all of the notes:
*insert here*

Note: basic_library/<insert latest version> should be a collection of all
functions and drivers written so far, but it is not updated as of now
because I am still writing the keypad drivers in lunar_programs_*

Note: DO NOT PRESS DOWN ON KEYS ON THE KEYPAD UNTIL DIR REGISTER IS SET.

File types are as follows:
	.c is the c file which is used to create the assembly file
	(no extension) is the executable of the c file, which creates the .x88 and .asm files
	.x88 is the assembly file with \x88 identifiers still present
	.asm is the assembly file with just clean map25.2 assembly (maybe comments too)
	.up is the "upload code" which is just the assembly file but with the mnemonics replaced
		and re-ordered into pages for writing to the eeproms. Only actual pages with data
		are included in the write instructions
	.buff is just the buffer file for the upload code, basically it is the .up file but only
		with mnemonics replaced and comments removed (pages have not been re-ordered yet)
	.eeprom is the converted version of the .up file FOR USE WITH THE VIRTUAL MACHINE
		(this file is meant to emulate the contents of the eeproms on the real machine)

Labels need to have the function name and then an _ to prevent collions. See
lunar_programs_0/readme.txt for a full explanation (it will be under version 8).
Labels are actually served using the function makelabel(), again see version 8.

Note about copying files into the /bin folder of the linux computer to use them on the terminal:
when doing this, I will ommit the file version for clarity, so collisioncheck3 becomes
just collisioncheck. However, I will maintain the version number in the main folder of the
repo to ensure that it is updated to the latest version.

Note that, at this point, lunar_0_08.c is the most advanced version of any file; it actually
has proper label allocation, it fully initializes the lcd, and it includes mfp() and add().

Quick sidebar: mfp() can actually be used as regular multiply as long as the factors are shifted
up a total of twelve bits between the two of them. So, you could shift factor0 up by 8 (displacement,
not enumeration) and factor1 up by 4 (again displacement) and get the correct result as long
as the factors don't end up being truncated.

Ok now the most advanced file is lunar_0_11.c. Basically, until I update the "basic_library" folder,
the most advanced file will be the latest lunar* file.
