Goal: provide a way to square a complex number.
Format: Complex number is stored as 16 bits for the
real part and 16 bits for the imaginary part.
The first eight bits are the sign, sixtyfours, thirtytwos,
etc. digits.
The digits after this are the halves, quarters, etc.

(a+bi)^2 = a^2-b^2 + 2abi

Number multiplication follows this procedure:
Keep rotating the original number, and, if there is a
one for the corresponding digit in the number you
are multiplying by, copy the rotated number into
ram; otherwise, copy zero.
Note that the number needs to be truncated when
rotating to prevent digits from wrapping around.
After all of these rotations are stored in ram,
add all of the rotations together.
The easiest way to do this is probably just using
masks to create "logic gates" and add one pair at
a time, re-applying the "logic gates" until all
carries are zero, then moving on to the next number
and continuing until only one number is left.
This can require up to O(x^2+x) or so clocks though
where x is the number of bits in each factor.
This seems to be about as good as it is gonna get
so maybe that's it?
Oh and you also need to subtract by taking the
inverse (1 operation) and then adding one (16
logic gate things in the worst case).

This means that squaring a complex number
with 32 bits of precision total will require
about O(3x^2+4x)+c operations. For 16 bits,
this puts you at around 1500 clocks with the
logic gates and everything. Actually, it might
be faster to use if-else instead of mask logic
gates for the addition part (not the rotation
though).

Note: using 2's complement for negative numbers.
Wait this might be bad for the multiplication
algorithm.
The algorithm kinda sucks though, but it seems
that with the current setup the worst case
will always be O(x^2+x) for factors of x bits.
