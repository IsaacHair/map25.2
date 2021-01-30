This will expand on the moving box from program set #2.

Basically, I will have an actual rendering of a rocket with rendering of fuel, etc.
The rocket will move around on the screen according to gravity as usual, but, instead
of side thrusters, it will have RCS to rotate the ship and a singular bottom thruster.
I will NOT create land, delta-v limitation, angular delta-v limitation, etc.
Also, I think I'm just going to go for a look-up table when it comes to calculating
the rotated rendering of the ship and the values of sin(x) and cos(x) for the
acceleration from the rotated ship.

Version 0:
Basically just shifting some stuff around to make sure it still works, is essentially
a copy from lunar_2_04.c. The main difference is that I will rename the x and y variables
to actually correspond with what appears on the screen. In other words, "x" shit will
correspond to left-and-right, and "y" shit will correspond to up-and-down. Hopefully the new naming system
will be easier to understand. I will also increase the accelerations a bit to make it faster
and up the frame rate a bit by decreasing the frame delay counter.

Version 1:
I will add actual sprites (for now, it will just be a single sprite for the rocked that
looks the same no matter if the thrusters are on or not). Note that the animation will work
by overriding the previous sprite with black, then painting the new sprite.

Version 2:
I will just speed up the acceleration by a factor of 2.5 because it is painfully slow
right now. I will also add an animation for the bottom thruster.

Version 3: (Not completed yet)
I will add rotation and the ability to render those rotations. The sprite will be stored
as a rom array. The rotation WILL NOT INFLUENCE MOVEMENT. It is purely aesthetic.
In specific, this version serves to test the viability of a ROM array paradigm. I will
also expand the sprite up to 8x8, add color, and try to clean the file up from a
programmer's point of view.

The ROM array works as follows:
There are different functions for printing each rotation of the sprite.
The start addresses of these functions are stored in an array in RAM,
which can be indexed through simply by incrementing or decrementing
the RAM pointer. These functions will all share the same return pointer
address in ram (the second addresses of return are all the same) so as
to make everything easier.

THERE IS STILL NO GENERAL STACK, only the heap and arrays created for shit you want to do.

Note: the libraries are getting just disgusting and the bastardized c is horrible
to program in. I will have to get serious about making an actual assembly language.
