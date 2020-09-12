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
