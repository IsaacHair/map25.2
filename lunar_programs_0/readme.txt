As you would expect, this will create a version of the lunar lander
game. It is to be used with the vma412 and the arduino number pad.

Note that lunar_0_XX and lunarXX refer to the same thing;
its just that lunar_0_XX is more specific if the file is moved
to a different location so you know that it came from the zeroth set
of programs (eg the index of this lunar folder as a whole).

Version 0:
This was basically where i just copied in most of the code
from previous programs that I think I will need. These include
code to define what instructions are, how to write to the lcd,
variations on the edge-detection predecessor macro, and of course
the multiply and add functions. The order in which addresses is
replaced was also tweaked a bit so that functions can call each other.
I HAVE NOT CREATED A STACK SO RECURSION IS STILL UNSAFE.

Version 1:
More tweaks to check stuff, also used definitions for pinouts.

Version 2:
Very minor tweaks, mostly testing the new create.sh

Version 3:
Just echos the value in the keypad to gen_reg.

Version 4:
Started working on a frame rendering protocol but realized that
I should write more macros instead of doing everything line by
line in assembly (so I bailed half way thru writing the program).

Version 5:
Just cleaned up the functions and everything should be set up better.

Version 6:
Just a white box that only moves up and down. Has gravity and one
thruster that can go up. Uses integer to store position, not fixed point.
Note that the screen in this version is treated as 512x512 even though
visible area is only 240x320.

Version 7: (lunar_0_07* is an exact copy of lunar07*)
Just a white box going up and down, but with speed improvements and general
improvements.

Version 8:
Same as version 7, but I fixed \x88 to work with strings of any length
and made other general improvements.

Switched argument order on transimmimm() to match the other functions
(eg the thing changed is the first argument).

Note that the protocol
now for labels and immediate pointers is to put the name of the parent
function in front to avoid collisions. Ex: If mul(a, b) and add(a, b)
both use a variable (lets call it "buffer"), they will need definitions
for the address of this variable. The "buffer" for mul might be at address
0x6942, so this address will be referred to using the definition MUL_BUFFER.
Similary, the "buffer" for add might be at address 0x0999, so this address
will be referred to as ADD_BUFFER. These functions might also use the
address label "END" to refer to the end of a loop. In order to prevent
replacex88 from replacing the "END" in mul() with a value from add() and vice
versa, the "END" in mul() is called "MUL_END" and the "END" in add() is
called "ADD_END". This naturally prevents collisions because c forbids two
functions with the same name, so you will never have a case where two
labels or variable addresses have the same prefix (note: this is why the "_"
is important; only use this between the function name and variable name
to ensure 0 collisions; otherwise a function with name a() and variable "BC"
would collide with a function named ab() with variable "C" because both
would create variable definitions of "ABC"). Also, use values that are ouside
of the '0' to '9' range or 'a' to 'f' range because these are used for
actual numbers.

*Problem: This method does not work well for labels (variable pointers are fine).
This method requires the length of the file to change dynamically.
It is possible to decide on a fixed length for identifiers, which is
basically what is already happening (the length is 4), or just pad
with a shit load of spaces and then remove when removing the \x88's.

*Better method: Make the names of the identifiers just 4 character arrays
in the c program itself.
Or 5 if you are using sprintf() and need the \0 to go somewhere but you
get the idea. Create a function to allocate unique identifiers
for these arrays, starting from "AAAA" and counting up using the
capital letters. This gives 26^4 possible labels for the assembly
\x88 file and allows you to refer to each of those labels using
an intelligible and unique name. Ex: You create char mul_end[4]
to serve as the identifier for the end of a loop. You send it to
function makelabel(char* str), which allocates it to "AAAF" because
that is the next label based on a global variable that counts up
to keep track of where you are. Now, when using that label, just
type mul_end and it will be used as "AAAF".

Also, with allocation being done this way, you don't have to do the "FX_"
prefix because the string itself can just be declared within the function
and gcc can handle not having collisions when it is compiled.

Note: I could store the address pointers as strings instead of numbers,
which would simplify the writing of code and allow you to not have
to replace the function calls at the end because the value would just
be some placeholder like "AABB" and then it would get replaced with the
actual location like "a09c". However, leaving them as numbers is a good
way to check that you aren't mixing addresses and labels together
because it will throw a warning if you try to use an unsigned short
where the replacex88() function is expecting a pointer or vice versa.
Therefore, I will not replace with strings immediately.

Also added "booster" option (if you press "0" it will multiply the acceleration
by 0x3.744. Note that the current fixed point algorithm will actually
work fine for this; just input the acceleration value and then multiply
by 0x3744. Even though it treats the acceleration as a tiny number instead of an
int, it will still scale by a factor of 0x3.744 which is what you want.
This also serves as a way to test that the mfp function is working.
Note that gravity is also accelerated, so really this is like time warping by
0x3.744.

Note: Mark string variables with "_" in front to easily tell what they are.

Version 9:
This will just move a lunar lander sprite around on a black screen
using arrow key inputs from the arduino number pad.
Arrow keys map as follows: <right> is key "7", <left> is key "9",
<up> is key "8", and <down> is key "0". Acceleration boost is key "#".
Note that the up and down are inverted on purpose because this is how
it is on an airplane/rocket.
They keypad is meant to be used upside down.
Numbers are stored as fixed points where the integer part is the upper 7
bits and the fraction part is the lower 9 bits, and there is no sign.
mfp() still operates on the presumption that the form is 1,3,12 not 7,9.
Only the corner 128 pixels in the x and y direction are used, and,
if the rocket goes off screen, it will simply loop back around.

Version 10:
Same as version 9, except the screen is used fully and it loops around immediately.

Version 11:
Moves a 5x5 spaceship around on the screen, looping around if it goes off screen.
It has thrusters in all directions and it has timescale (eg you can adjust
"timewarp" like in kerbal space program). <up> is "8", <down> is "0", <right> is "7",
<left> is "9", <increase warp> is "#", <decrease warp> is "*". Note that time
warp exists on a logarithmic scale. You can also set the time to 0 if you want and
it can still recover. Note that the data types of (1,9,6) and (1,3,12) and (0,16,0)
are mixed in this program, so it is kind of messy. Also, the values become somewhat
innacurate if you scale down to extremely slow speeds because, for example, an
acceleration of 0x0111 would get cut off to an acceleration of 0x0001 when scaled
down by time warp.

In future versions, it might be better to perform time warps in a different way.
Right now, time warping means scaling the velocity and acceleration scalor
during the warp, and, once it is done, the acceleration is constantly
scaled by the acceleration scalor.
