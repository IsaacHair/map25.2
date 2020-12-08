As you would expect, this will create a version of the lunar lander
game. It is to be used with the vma412 and the arduino number pad.

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

Version 7 (have not created this yet but plan to create it):
This will just move a lunar lander sprite around on a black screen
using arrow key inputs from the arduino number pad. Arrow keys map as
follows: -> is key "4", <- is key "6", v is key "5", and ^ is key "8".
They keypad is meant to be used upside down.
