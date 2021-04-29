Note: programming language is now called Suig instead of BiggieCheese
Extention: .su
***Going back to calling it BiggieCheese, extension .bc, for the sake of
simplicity and because BiggieCheese is a dumber name because hes a rapper

Revised features:
	- inline machine code or 1-to-1 machine code
	- if loop, for loop
	- function calls WITHOUT stack
	- goto's
	- absolute and symbolic RAM/ROM addresses that aren't painful to use
	- automatically write all functions to ROM
	- automatic label maker and scheduler for the x88 shit
	- automatically set the ROM address to 0x0000 for the
	  start of the "main" macro
	- automatically reset heap counter
	- automatically reset label counter
	- automatic heap maker, where arrays are created where the heap counter is
	- allow user to move heap counter to the next multiple of a power of two
	  or +/- a value or to an absolute value
	- inline c with the ability to create pre-processor variables, functions,
	  etcetera. should NOT also have the ability to manipulate how assembly code
	  is written as this is extremely messy.
	- ability to use variables from inline c, enabling expressions and complex
	  manipulation and such
	- tabs show nesting like they do in python
	- don't have to specify immediate if not necessary
	- can specify current and next addresses, but the latter requires you to
	  put in a value for immediate
	- "fordef" command to repeat a block if the inside c variable evaluates to nonzero;
	  should be paired with an inline c thing to change the variable;
	  is called fordef because it is a for loop with definitions
	- "fordef" is the equivalent of a for loop written in pre-compiler
	  c; "for" is an actual for loop in machine code that
	  will run on the map25.2
	- macro is initiated with "#"
	- comment is initiated with "//" and will comment the entirety of the line
	  and only the entirety of the line; does not see semicolons
	- either semicolons or newlines represent the end of an instruction
	- sections of "for" loop are separated by :
	- functions are declared by !
	- "continue" and "break"
	- else, else if
	- mark keyword will set a variable to the value of the ROM address at that point
	  and change all instances of that variable ahead and behind of that point
	- there will also be an option to set a c variable to
	  track along with a $variable; changing one will change
	  the other; keyword is "track"
	- tracking is synched before and after every "eval" block;
	  @ variables are the only ones that can be easily dynamically
	  changed, so the $ variable just takes on the value of the @
	  variable
	- pre-compiler variables are all global or just within
	  macros; these are the only two options
	- @ variables are only declared within inline-c ("eval");
	  $ variables are declared as arguments to a macro or
	  with the "void" keyword
	- @ variables must be accessed using @ unless they are within
	  inline c
	- for and fordef loops have their components separated by ::
	- order of "track" arguments shouldn't matter as long
	  as they come in pairs of those to be tracked
	- "+" and "-" are used to show relative ROM address to the
	  current position
	*** Actually, with recursion, if/else, for, and macros,
	  rom address stuff shouldn't be necessary at all
	*** Actually, @ and $ variables should be combined into one;
	  just use the $ symbol, so need to declare all of these
	  variables with either "int" to make it random (also
	  32 bits to give extra space to work with and sign) or "heap"
	  which will assign it to a value on the heap and increment
	  heap by one
	*** there will also be automatic stack creation with
	  automatic pushes and pops for addresses and constant
	  values
	*** the start and end of the stacks defaults to the upper
	  half of ram, starting with 0x8000
	*** there will be a large set of implicit functions and macros
	  in the language
	- again, other variables can be created with inline c, but only
	  variables created using "int" or "void" can be used
	  in the BiggieCheese itself
	- ifdef and elsedef are just pre-compiler conditional statements
	  that feed directly into the corresponding if and else loops
	  in c when compiled to bastardized c
	*** fordef, ifdef, elsedef, int, void, #, ! represent pre-compiler statements,
	  similar to #define in c, that do not produce code directly
	  in the final machine code
	- can use "else if" or "elsedef ifdef" if want
	*** need to use semicolons for some parts and NOT others
	- note that eval is proper inline c, and rep just goes
	  directly into a c for loop. "int" doesnt even do that
	  much besides allocating a linked variable
	- absolute and relative ROM is not necessary because recursion
	  can provide what you need in terms of complex threading
	*** will have both labels/gotos and relative ROM positioning
	  with "+"/"-" because recursion is not strong enough for
	  the double-threaded add macro
	- "goto $x asdf" goes to the $xth instance of asdf relative
	  to the current position; if $x is negative it is backwards,
	  positive is forwards, zero is bi-derectional (relies
	  on their only being one instance of that label either in
	  the macro or globally)
	- again, labels can either be global or within a macro
	  or within a function -> ACTUALLY NO, global and local conflicts
	  are painful, so all are either in a function or macro
	*** the program begins at the main label, which is neither
	  a function or a macro; label can only be used once; follows
	  indentation rules
	*** the reason this is necessary is that positional entropy
	  data storage is extremely "complex", and recursion is not
	  powerful enough to describe it
	- don't overload global and local variables
	*** labels actually don't have to be global or local;
	  they simply exist and can be accessed from anywhare at
	  all times, so be safe
	*** Macros CANNOT BE RECURSIVE, but functions can
	*** fordef uses "," to separate statements and ";" to separate sections,
	  but for uses ";" to separate statements and "::" to separate sections
Compilation process:
	- use struct pointers for each line to allow easy shuffling
	- the inline c is evaluated first to create an interum ram buffer where
	  all the assembly is written as dictated by the c code, meaning lines
	  are repeated or omitted as specified and variables are inserted
	- eval lines outside of the macros happen first, then it goes inside the
	- note that this will first create a c file as an output, which
	  is basically the type of bastardized c i am using right now,
	  which is then further compiled
	  macros as they are called up; lastly, it is applied to functions
	  when they are copied down at the end
	- all addresses are resolved and inserted into the ram buffer. this
	  includes resolving the symbolic RAM/ROM addresses and resolving for/if
	  loops and gotos. this also means the insertion of macros
	*** Previous line is not necessary because
	  compiling to bastardized c
	- convert the file to machine code (actually upload code)
Notes:
	- RAM addresses are declared before use, but rom addresses are declared
	  on-the-fly (these are function calls or gotos or loops)
	- variable names cannot be overloaded on any level between
	  parent and child levels. same with macros. However, variable name collissions
	  for macros that will never be nested are fine because the variable
	  is set to the correct value each time the function is called.
	- variables eventually just get resolved to an absolute number, and
	  the "void" keyword just associates a number with a variable,
	  so calling a macro and using various variables within the macro
	  to represent values it was called with is fine because it will just
	  get resolved anyways regardless of nesting depth.
	- as a result of the previous point, you can use variables and absolute
	  numbers independently
	- All numbers are in hex for everything EXCEPT WITHIN INLINE C.
	- the conditional part for an "if" or "for" needs to be of the form: <source> <mask>
	  or <number> where the number can be a plain hex number, c variable, or
	  a $ variable (but this doesnt make much functional sense)
	- inline c is after keyword evaluate and lasts one line only
	- c variables are put in place with @
	- both functions and macros are invoked by simply stating their name
	- all $ variables are the value of the pointer
	- evaluate is marked as "eval"
	- "main" is neither a function or a macro; it doesn't even need a key word;
	  rather, it is everything in the document that is not within a macro or function
	  block. Again, this is similar to python
	- $'s don't have to be initiated before use; they just simply exist and are added
	  to the look up table if haven't been used before. The point of "void" operating
	  on the variables is to assign them to a heap pointer.
	*** Previous line is incorrect
Purposes:
	- schedule all of the dumbass low-level shit for you
	- greatly reduce the number of stupid errors
	- obfuscate some stuff

OK this is a lot more complex than I expected, and it still looks like this isn't providing
the best functionality. Particularly:
	- pre-compiler variables are still difficult to manipulate
	- inner part of for/if loops are painful
	*** Note that variables declared anywhere
	  can be used anywhere because everything
	  is compiled to bastardized c
	*** VARIABLES UPDATE: "heap" is used
	  to simutaneously declare a 32-bit signed
	  variable and assign it to a number on
	  the heap; to declare variables
	  without this, just use the "eval" thing,
	  not "int" directly
	*** to move the heap pointer head, use
	  "mheap" command with a number after
	  in hex of course, written out for absolute
	  or with +/- for relative movement
	*** don't give user any control to make
	  absolute statement for ROM addresses;
	  compiler handles all of this
	- Functions are just macros for the function
	  call procedure.
	- variables and such can be done globally

Compiler (actual process):
	- the compiler outputs a bastardized c file.
	  Which the user then executes to create an
	  assembly file, which is then assembled
	  into upload code (again by the user).
	- the c file is buffered in ram as a struct*
	  kinda thing where each line has a pointer
	  to the nested content (which can be left
	  null) and to the next line after the nesting (which
	  can also be null to end that nest level)
	- variable names don't have to be put into
	  any fancy array; they are literally
	  just typed as declarations in the output
	  file because the output file is c
	- macros just end up corresponding to
	  functions in the c file that advance
	  addr, etc in the expected ways
	- functions in BiggieCheese become
	  c functions with that extra pizzazz added
	  like putting things on a function stack
	  and actually creating a copy of the code
	  in rom
	- fordef -> c for loop
	- ifdef/elsedef -> goes directly
	  into what it would correspond to in c
	- goto's -> translated into the bastardized
	  versions currently used
	- if/else/if else -> this is the most difficult of all,
	  the compiler actually has to think a bit and
	  make the corresponding gotos
	- for -> same deal as "if" but with some
	  extra added pizzazz
	- variables -> literally just write it
	  in c
	- assembly -> just convert into the
	  bastardized "inst()" function family stuff
	- copy over most of the lower level setup functions from the basic library
	  (this is EXCLUDING lcd drivers, multiplication, etc;
	  the only actual machine code functions are
	  the functions to create a map25.2 function stack)
	- some basic parsing to remove comments
	  and separate statements at ";" or "::"
	- needs to handle local variable names and
	  global variable names -> i think this
	  naturally occurs because the variables
	  are only declared in functions or globally,
	  actually, if anything, need to ensure global
	  for the global variables and that it isn't encapsulated
	  into main() in the output file.
	- goto with number for direction needs
	  to be written in an intelligent way
	- heap head manipulation obviously
	*** Actually, all numbers are just passed
	  into the bastardized c as is, so hex
	  numbers must have the preceding "0x"
	- everything just ends up closely corresponding
	  with the resulting bastardized c file

Notes
	- basically everything should be translated
	  into bastardized c so that things like
	  the displacement parameter on goto
	  can be a c expression
	- removed need for $; these types of
	  areas just default to being a c expression
	- to reiterate, there is NO ABSOLUTE control
	  over ROM addresses and only relative goto
	  control; this is because obfuscation is
	  helpful
	- tab depth must be TABS ONLY
	*** There can be global variables but not
	  global code
	- ROM won't be 100% packed because of
	  if/else bodies that are just gotos
	  and because it is less messy to not
	  fully pack if-else stuff
	- ***Actually, ROM can be packed with if-else
	  if you just keep track of where the else
	  body should start and then skip over the if
	  body to
	*** goto's should look back and see if they
	  need to insert a no-operation instruction
	  or if they can just be appended to the last
	  instruction


Compiler
	- It uses a singly linked list, where
	  you can have lines with no content to help
	  with formatting (ex: the program
	  starts with such a line)
	- Note that the compiler just pisses unused ram everywhere as well
	*** Actually, you don't have to insert the macros; just force gcc
	  to do it for you, right?
	- NEED TO DO WEIRD STUFF for global
	  "heap"; define the variable globally
	  and then set it using the heap function
	- ***OR wait; heap can just be definition?
	- ***can't be definition because those are all
	  global in the C file
	- Use recursive functions in the compiler to do if-else
	  stuff
	- need to provide a few native macros like genpred
	*** 1000 character limit per line for the sake
	  of simplicity in compiler
	*** Can get absolute ROM addresses by
	  just using labels and gotos



VERSIONS:
v0_00: Just feeling things out

v0_01: Scrapped 0_00 and am going to store lines in a doubly linked list and 
	do the insertion of macros on the gcc side.

v0_02: Going to just parse each line, then convert the line into bastardized c
	based on the first keyword. The compiler will have to parse each line,
	but, for the most part, the heavy lifting is left to gcc when it
	compiles the bastardized file.
	- note that this version no longer has a singly linked list
	*** It has a doubly linked list instead; still have
	to be able to dynamically add or remove lines;
	the difference is that this list doesn't
	have nested lists within it
	*** depth is an absolute measure of line
	depth as no macros need to be inserted, so
	you don't have to worry about this value
	being wrong; gcc will do the heavy lifting
