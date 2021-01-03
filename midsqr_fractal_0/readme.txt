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
Update: it works perfectly (or at least seems to).

Version 0_01:
I am going to use the program setup as described above, except
the x and y axes will correspond to real and imaginary respectively,
and the array will be indexed and set using the imaginary part.
Array is also tested using the imaginary part.

The number is stored as one word, with the upper 8 bits for real and the
lower 8 bits for imaginary. For multiplication, the number is split in two
(real and imaginary), then the parts are shifted to the upper part of the
word. Bottom part is padded with zeros as this will make multiplication
using mfp() faster. Each is then squared, etc, you know the usual
(a+bi)^2 = (a^2-b^2)+2abi bullshit. The upper 8 bits of each are then re-inserted
into the number, it is tested with the array, and the process is repeated.
This is not the most efficient process but who cares.

Since mfp() expands with a center between the upper nibbles, this is going
to actually create a "middle split" random number function instead of
one where the split occurs higher.

Result will also still be shifted up 2 spaces.

INCORRECT MULTIPLICATION.

Version 0_02:
Same as 0_01, except the 8-bit real value and 8-bit imaginary value are both
used to check off values in the table (that means if the real and imaginary are
equal, it is automatically killed off). Actually, the parenthesized section
directly before this sentence is false; it is checked in a slightly different
way (see program; it is self-evident).
The flag array is set by the real AND imaginary, and it is tested by the real
AND imaginary.

INCORRECT MULTIPLICATION.

Version 0_03:
Same as 0_01, but tests only the real values. Also only sets the real values.

INCORRECT MULTIPLICATION.

Version 0_04:
Exact copy of version 2, except the real part of the complex square is evalueated
properly (in 0_02, 0_01, and 0_03, the real part ends up being a^2+b^2+1 instead
of a^2-b^2 because of an error in two's complement evaluation).
