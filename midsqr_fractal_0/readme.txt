This program will test an idea for a fractal I have.

The middle-square method can be used to generate (bad) sequences of pseudo-random
numbers. The method works by squaring the previous "random" number (or the seed),
then taking the middle bits of the result. For each seed, cycles of various
lengths will be created.

This program will use the vma412 and ardino 12 key soft keypad as peripherals,
with the bl_1_00.h (referred to as <bl_map25.2.h>) as the library.

The start row is considered 0xff and the start column is considered
0xef, and each pixel results in either (subtracting 1 from column)
or (subtracting 1 from row and resetting column to 0xef).
The second option occurs if the column value is 0x00. Note that these values
are tested AFTER calculation and BEFORE doing the decrementaion (obviously).
For this program, the column value will be the lower 8 bits of the seed
and the page value will be the upper 8 bits. Program will stop AFTER the
row value reaches 0x00 and column is 0x00. The seed will be squared, except, instead of
taking the middle section of the square (which is 32 bits), the result
will be shifted down by 12 (displacement, not enumeration) and then those
lower 16 bits will be taken as the next number. All of this is to say that
I am being lazy and simply using the mfp() function to square and shift
the number at the same time.

The length of the cycle is determined by having a counter and then having a
256 bit array of flags. The flags are set to all zeros, then flags are set
to one at the address of the last 8 bits of the number in the sequence. This
means that the upper 15 bits are wasted. The counter is decremented, starting
at a value of 0x00ff, for every time that the sequence encounters a flag
that is not already set to "1". This means that the flag must be tested
before being written to. The final value of the flag is simply printed
directly as the value of the pixel.

The exact details of what counts as zero and one for the counter, if the VERY
first number is written to the flags will kind of just be glossed over
because it shouldn't really affect the end result very much.

Basically, the color of the pixel is the cycle size at that point.

Version 0_00:
I am just gonna do everything described above and hope for the best.
