Going to build functions from the ground up and test directly
on the map25.2 instead of simulating on the terminal.

The order of functions/macros created will probably be:
0. Predecessor macro (uses edge detection, predecessor faster than successor)
   This should average 7.5 clocks per execution.
1. Fast adder function (function parameter pointers are immediate, not
   on a ram stack, so the adder function can't be recursively called).
   This will probably involve repeated predecession of one's complement.
   I still need to determine whether repeated predecession or virtual
   logic gates is faster.
2. Fast multiplier function for numbers that are stored as 1 sign bit then
   3 bits for the integer part, then 12 bits for the fraction part.
   An algorythm consisting of 16 bit additions is ideal.
3. Pixel iterate function. It computes z^2+c for ONE iteration.
4. Pixel render function. Calls iterate and prints an output color
   based on how long the pixel took to blow up to (absolute value) > 2.
5. LCD display function. Initializes the LCD, counts the corresponding
   values for the rows and columns, and calls pixel render to print
   the results for those pixels.


predecessor macro = "pm"




WAITWAITWAIT
Using "if"-"else" statements to perform 4-input additions allows you to
multiply two 16 bit numbers with a fraction part in only around 400 clocks.
"iemul" is the if-else multiply protocol.
In this paradigm, the only functions needed to render a mandelbrot set are:
0. 2-input multiply constructed with 4-input additions.
1. 2-input addition.
2. Protocol for if statements. Honestly, just adding a value to the number
   being tested and then testing that number will be a lot easier; you can
   engineer it to go negative or positive past a threshold value and test
   equality or greater than or less than or any combonation. This method
   will be MUCH more readable and understandable than using virtual logic
   gates for complex comparisons. It will also be faster because it requires
   very few pushes and pulls form ram compared to virtual logic gates. Also,
   you need to define and test a macro for if statements instead of explicitly
   defining a bunch of instructions for them and hoping no typos occur.
