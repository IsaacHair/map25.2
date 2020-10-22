map25.2 assembly:
This is basically just machine code.
instruction looks like: XXXX <pneumonic> XXXX XXXX
The first set of X's is the address to store the instruction at.
The second set of X's is the immediate value for the instruction.
The third set of X's is the address for the next instruction.
The pneumonic represents the op code without having to write
the op code out in raw binary or hex values.

updated instruction:
XXXX <bus connection pneumonic> <write/jump/no-op pneumonic> XXXX XXXX
X's correspond to the same things as above.
Also, a line with only "<" begins a comment (start the comment
on the next line) and ">" ends the comment (put the ">" on its
own line and start the code after that line).
Note: comment should not contain ">" in the actual content of
the comment.

bus pneumonics (upper nibble):
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

write/jump/no-op pneumonics (lower nibble):
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

Updated pneumonics:
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

Ok basically I'm going to update the pneumonics and then make a
full list of pneumonics with functional descriptions.
A single character is also considered a pneumonic and it just
directly corresponds to the machine code; ie typing "7" is the same
as typing "dir1".
