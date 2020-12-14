Version 0:
Just a sped up version of mandelbrot_3_04; basically, I will switch to the (a-b)*(a+b) method
for calculating the real value. The escape test will be simply testing if the real or
imaginary is >= 2, so the escape zone forms a square instead of a circle. I will also remove some
unneeded additions and such.

Actually, for this version, I have re-written the comparisons and such in MAIN, but I have not
actually adjusted the algorithm yet.

Version 1:
Does what version zero paragraph 1 states. Note that the labels for address replacement
are not served by a function; the programmer just picks them.

Yeah this version just creates some weird artifacts because the algorithm is being weird. Also, 16
as escape value is too low.

Version 2:
Like version 1, but the algorythm is fixed and escape is 32.
The algorithm where ZR(next) = (ZR-ZI)*(ZR+ZI)+ZR0 produces sign dependent artifacts.

Version 3:
This will be a copy of version 2, but I will set the max_iterations to 8. I will also return
to making the escape "square" extend across (-2,2) in the real and imaginary. This setup
will do two things: Create really weird artifacts and render really fast.

Artifacts occur because:
	The escape value is 4, which is way too high to ensure accuracy (overflows occur
		everywhere).
	If the last 4 bits of MAIN_ZR or MAIN_ZI are "1", the number is said to have escaped even if the value
		is not outside of the escape squre. This is accomplished by testing the last bits.
	There are only 8 iterations, which is just stupidly low.
