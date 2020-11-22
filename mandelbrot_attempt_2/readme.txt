Writing a map25.2 program to create a mandelbrot set rendering.
Just starting entirely from scratch for plot01.c and onwards.

32 bit precision, with formatting: 1 sign bit, 4 int bits, 27 fraction bits.
Use two's complement for negative numbers.
Pointers work as follows (the pointer points to the lower word of the
number and the successor to the pointer points to the upper word):
Example: MUL_PROD is a pointer to the product from multiplication
and is set to 0x6900; eg the product is stored starting at address 0x6900
in map25.2 ram. If the value of the product, following the form above,
is 0xf6942000, then MUL_PROD (eg ram address 0x6900) will contain
the value 0x2000, and MUL_PROD+1 (eg ram address 0x6901) will contain
the value 0xf694. For clarity, ensure that the definitions for pointers
point to an even address so that (pointer) is even and
(pointer+1) = (pointer|0x0001) and both are the odd successor to the
even pointer.

Terminology note. Take the following DECIMAL multiplication as an example:
   59
*  22
_____
  118
+1180
_____
 1298

In this example, 59 is MUL_F0 and 22 is MUL_F1. 118 is the zeroth partial
product and 1180 is the first partial product. 1298 is MUL_PROD. Partial
products for the binary multiplication will either be zero or the value
of the number (only rotated after the decimal place; before the decimal,
rotation occurs at the very end. more on this later). The partial products
are stored in a ram array that is indexed between using a ram predecessor
function (because predecessor is faster than successor due to the map25.2
being an OR-logic machine instead of an AND-logic machine).

Binary example (sign bit not included because that is corrected for in
the very beginning before multiplying and then added back at the very end):

  01.10
* 10.01
_______
  01.10 (don't rotate yet)
  00.00 (don't rotate yet)
  00.00 (rotate zero bits up)
+ 11.00 (rotate one bit up)
_______
  11.01

There are several things to note here. First of all, partial products that
resulted from multiplying by a number in MUL_F1 from its FRACTION part are
NOT rotated when they are loaded into the partial product array. Only
partial products from the INTEGER part are rotated (rotation is the same
thing as padding with "0"s when you do decimal multiplication to get
the partial products to line up). When adding, the partial products from
before the decimal are added and then the RUNNING TOTAL IS ROTATED DOWN ONE.
After the decimal, the RUNNING TOTAL IS NOT ROTATED DOWN. This has the effect
of completing a full multiplication of the inputs to an output with TWICE
the number of bits in its precision which is then truncated down to the
original precision. Eg, two 32 bit numbers can be multiplied to a 64 bit
precision result and then it is truncated down to 32 bits. Note, however,
that no 64 bit numbers are actually used; the running total is always 32 bits.
Note that the partial products are added one at a time to the running total,
not all at once.

Addition procedure (this doesnt care about where the decimal is or even
if there is a sign bit or not because two's complement is just that sexy):
Simply start from the lowest bit and EXPLICITLY define if-elses for each
bit to essentially load the value of ADDEND0, ADDEND1, and CARRY into the
position of the machine (so there will be three bits of entropy in the
position of the machine), working your way up the bits. Note that CARRY
is not an actual ram value; rather, it is simply buffered into the ENTROPY
of the system. Eg, there is a thread for if the carry is zero and a thread
for if the carry is one.

Example for a couple bits (note that you start with the zero carry thread):

start:
goto zerobit
zerobit:
if (bitzero(ADDEND0))
	if (bitzero(ADDEND0))
		bitzero(SUM) = 0
		goto onebitcarry
	else
		bitzero(SUM) = 1
		goto onebitnocarry
else
	if (bitzero(ADDEND0))
		bitzero(SUM) = 1
		goto onebitnocarry
	else
		bitzero(SUM) = 0
		goto onebitnocarry

onebitnocarry:
if (bitone(ADDEND0))
	if (bitone(ADDEND0))
		bitone(SUM) = 0
		goto twobitcarry
	else
		bitone(SUM) = 1
		goto twobitnocarry
else
	if (bitone(ADDEND0))
		bitone(SUM) = 1
		goto twobitnocarry
	else
		bitone(SUM) = 0
		goto twobitnocarry

onebitcarry:
if (bitone(ADDEND0))
	if (bitone(ADDEND0))
		bitone(SUM) = 1
		goto twobitcarry
	else
		bitone(SUM) = 0
		goto twobitcarry
else
	if (bitone(ADDEND0))
		bitone(SUM) = 0
		goto twobitcarry
	else
		bitone(SUM) = 1
		goto twobitnocarry

Et cetera for the rest of the bits for however long you want. Remember
to account for the fact that, if you are doing 32 bit arithmatic, each
number will be spread across two ram addresses.


Iterations are calculated as follows.
	repeat until MAX_ITERATIONS:
		Z(next) = (Z(previous))^2+ZC
		if (radiusof(Z) > 2)
			end;
Where Z is complex and ZC is the complex coordinate of the pixel being
rendered.

This can be completed using only 2 real value fraction multiplications.
Note: It looks like this consists of four multiplications, but the
last two multiplications can be extremely low precision as it is simply
checking whether the number has escaped.
	ZR(next) = (ZR(previous)+ZI(previous))*(ZR(previous)-ZI(previous)) + ZR0
	ZI(next) = (ZR(previous)*ZI(previous))<<1 + ZI0
	escape if ((ZR*ZR + ZI*ZI) > 4)

Multiplication takes the following form. Remember to deal with the fact
that each value is split across two ram addresses.
	0. Correct for sign and store the correct sign in the address
	   for product.
	1. Store UNROTATED partial products in ram.
	2. Store ROTATED partial products in ram.
	3. Add partial products that require ROTATION when adding.
	4. Add partial products that require NO ROTATION when adding.
	5. Correct for sign.
