ramtest.a25 will just test what happens when ram is put on the bus and written to at the
same time. It is not written using a C meta-program; it is just written directly in assembly.

After testing, it appears that, if OE and WE are active for the ram chip at the same time,
it will default to outputting. This is because, when charged with 0x6942 and then having
OE and WE activated, the value on the bus remained at 0x6942 instead of being pulled up
to 0xffff, which is what would have happened if the chip defaulted to writing instead of
outputting.
