This version will eliminate the multiplication in favor of 32-bit arithmatic to increase precision
drastically. As far as complex arithmatic goes, there will only be the 16-bit predecessor macros
(obviously), 16-bit addgenram macro, 16-bit addition function, and 32-bit addition function.

Note: I should probably break down the components of these functions and of the multiplication
function from the previous version to smaller chunks (like you are supposed to do in
functional programming instead of being a bitch and having 200 line functions).

Note: This version will not have time warp.

Note: This version will also not have screen wrapping; it will just reject values that go outside
of the screen, instead setting the position to the edge of the screen in that direction (either x
or y) and zeroing the speed in that direction (again either x or y).

Version 0:
This is basically just a copy of lunar_0_11 but without timewarp and with 32 bit precision and MUCH
slower accelerations.

Note that this version CURRENTLY DOES NOT WORK.

Version 1:
Basically trying to get version 1 to work, but it still fails. Should probably just re-write most
of this from scratch. That way, it can be cleaner to begin with.
