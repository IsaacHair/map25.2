This file will just establish some dynamics and basic movement.
I am not expecting to get a full game; rather, this will establish
the needed graphics and mathematical functions. I might or might
not create an actual function stack, we will have to see.

pacman00.c should establish automatic jump scheduling for if-elses
so that crude estimates don't have to be used. This is all it will do.
Honestly, maybe I should make a library with these basic functions
like inst() and ifelse() and add() and multiply().


What really needs to happen is there needs to be support for inst()
and its derivatives to to the weird \88XX\88 stuff so that anything
you want can be overwritten with the address you want or identify
later in the code. This will help with asnx commands and commands
related to it.
