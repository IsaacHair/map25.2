This set will use the basic library (bl_1_00.h).

Version 0:
Basically just a copy of lunar_1_01.c except using bl_1_00.h
Will do things using 32 bit arithmatic.

Currently doesnt work.

Ok now, after scheduling the heap allocation to occur in the very beginning,
it works (but I got the acceleration all wrong).

Version 1:
Just version 0 but with acceleration fixed (hopefully).

Note that neither v1 or v0 actually calculates addition properly.

Version 2:
Just tests the add32() macro and dumps various values to output (the direction register
is set to zero and the lcd should just be left off).
Note that the code in this version has been destroyed and it will be very difficult to fix;
when you find the error, just copy v1 into v3 and then fix v3.

Version 3:
I have realized that the code is just disgusting, so I am going to rewrite the whole thing,
starting by just testing whether the add32() works.

Version 4:
It appears that the mistake was to use the int datatype in the 32 bit macros;
it needs to be an unsigned int.

Yeah now it is working after fixing the library.
