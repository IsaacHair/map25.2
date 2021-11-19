<
this architecture is the same as for the map25.2 except that
   *there is an aux register
   *there is an add function between that aux register and the gen register:
    note that this function may be a bit slow; charge the value of gen
    and aux well before using add so that its value resolves in time;
    only MAYBE NECESSARY, DONT KNOW YET
   *rotation has a selector now, so any degree of rotation is possible
   *jzand and jzor are now things
   *system is 24 bits instead of 16 (this applies to everything)
   *gen is the only register with weird writes (eg the bus acts as a mask for 1's or 0's
   *all other registers are just directly written the value of the bus

mneumonics:

upper: (bus ops)
imm
ram
gen
aux
out
dir
in
rol
dnc
addr
add

Lower: (write ops)
ram
out
gen0
gen1
dir
addr
aux
asnx
jzand
jzor
noop

function procedure:
*location of stack pointer is at 7fffff (points to next UNUSED slot, no overflow check)
*stack is between 000000 and 7ffffe, inclusive
*heap is the rest if you want or maybe not
*note that you kinda have to memorize the order of variables in memory
*also, you have to manually calculate where the return address will fall
*this procedure allows for functions to call themselves
*it also allows for functions to use conflicting portions of the heap
*the only conficts you have to worry about are within functions,
 within global, or between global variables and functions

caller procedure:
1. put 1 current fx variable onto stack using macro
2. increment stack pointer using macro
3. repeat steps 1 and 2 for all variables
4. put the return program address onto stack using macro
5. increment stack pointer using macro
6. put 1 function pass values onto the stack using macro
7. increment stack pointer using macro
8. repeat steps 6 and 7 for all function pass values
9. jump to the program address for the new function start

callee procedure:
1. decrement stack pointer using macro
2. copy 1 variable from the stack into a heap memory location using macro
3. repeat steps 1 and 2 for all variables supposed to be passed to function

returner procecure:
1. decrement stack pointer using macro
2. jump to the value on stack at that point

returnee procedure:
1. decrement stack pointer using macro
2. copy 1 variable from the stack into heap location using macro
3. repeat steps 1 and 2 for all variables

NOTE: all of this can be encapsulated as "push" and "pop" pretty much


Programming Language features:
*can make program address of next instruction odd or even; do
 odd
 inst in; jzand; 005000;
 
*can make a label for program address using :
*can define; do

 #def stuff
 this is a long string
 #enddef
 #def num
 100+stuff
 #enddef
 stuff

 to make the compiler write
 100+this is a long string
 in place of "stuff"

*write all numbers in decimal always unless use preceding "0x"
*can goto by simply writing this at the end of an instruction
*can use #def and #enddef for macros as well; literally just
 tells compiler to copy and paste, and it supports nesting
*note that add #def statements must resolve before the
 compiler continues, and it does all of these first in
 the order that they appear from top to bottom on the page
*note that macros are extremely powerful, literally placing
 themselves into any block of text separated by non-(letters or numbers)
*compiler handles the program addresses and next addresses
*next addresses default to +1 of the current address unless
 otherwise specified; the end of an instruction can consist
 of any constant to be added to the current address if it
 starts with a "+" or "-"; otherwise, it is treated as an
 absolute value
*instruction syntax is: inst <bus mnemonic> <write mnemonic> <immediate value>; <optional next statement>;
*if not using the next statement, leave blank but use semicolon
*note that <immediate> and <next> section of instruction can be anything that printf() in c would resolve;
 literally treat it as what you would put inside the printf() brackets
*if you want the section to be fed into printf() as described above, MARK WITH A . AT START
*otherwise, don't mark with a dot and it will be treated as a plain number or variable name
*can do inline compiler c with "inline"
*after everything is resolved, the program file is used to print to a resultant
 file, which is the assembly; this means that inline c will take effect as you progress
*comments with "<" to initiate on its own line and ">" to end on its own line
*there are reserved compiler variable names; each one is to say "compiler"
 before the rest of the variable name to differentiate them
>

#def stackpoint
7fffff
#enddef
<
zeroing stackpoint value to point to start of stack
>
inst imm addr 0x7fffff;;
inst imm ram 0x000000;;

<
make heapaddr
>
inline int heapaddr;

<
push places the value in ram at compiler variable "heapaddr" onto the stack
*expects the user to set heapaddr to point to the correct value before
 pasting in the macro
>
#def push
inst imm addr heapaddr;;
inst ram aux 0x000000;;
inst imm addr stackpoint;;
inst imm gen0 0xffffff;;
inst ram gen1 0x000000;;
inst gen addr 0x000000;;
inst aux ram 0x000000;;
inst imm aux 0x000001;;
inst imm addr stackpoint;;
inst add ram 0x000000;;
#enddef

<
pop gets the next value from the stack, then places it at the address stored in "heapaddr"
*again, expects the user to set heapaddr to point to the correct value before
 pasting in the macro
>
#def pop
inst imm addr stackpoint;;
inst imm gen0 0xffffff;;
inst ram gen1 0x000000;;
inst imm aux 0xffffff;;
inst add ram 0x000000;;
inst ram aux 0x000000;;
inst aux addr 0x000000;;
inst ram aux 0x000000;;
inst imm addr heapaddr;;
inst aux ram 0x000000;;
#enddef

<
example function which grabs user input:
>
