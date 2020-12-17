This will provide functions to define addition and fixed point multiplication
(1,3,12). It will also provide a macro to read from the keypad and macros to do things with the
lcd. It will provide macros to do things like write the predecessor of a register to itself.
It will also provide compiler functions to actually write instructions in the first place,
manage the next addresses to create conditional loops in slightly less painful ways
than guessing how big ranges are, and compiler functions to allocate and use labels for
rom addresses.

Move the library version into whatever <include> folder you are using for your c programs,
then discard the version extension (eg it should become bl_map25.2.h). This is just done for ease
of use as the library is updated. It will have the version extension in the main repo folder.

Note that both rom address labels and ram address labels are allocated by functions in the
c "metaprogram" library to help avoid collisions.

**Example main() function:**
<start:>
	char _next[5];
	if (argc != 3) {
		printf("need <target> <buffer>\n");
		exit(0x01);
	}
	addr = 0;
	labelcount = 0;
	heapcount = 0;
	fd = fopen(argv[2], "w+");
	makeaddlabel();
	makemfplabel();
	libheap();
	makeheap(&MAIN_dummy); //yes this needs to be the address of the pointer
	makelabel(_next); //it is already the address because its a string
	inst("imm dir0 ffff");
	lcdinit();
<some code>
<some code>
<end:>
	mfpcode();
	addcode();
	replacemfpcall();
	replaceaddcall();
	removex88(argv[1]);
	fclose(fd);

Note that addr, labelcount, heapcount, and fd are included within the library as global variables and should
not be re-instantiated when using the library (obviously).

Only have to initialize stuff for functions or macros you will use in the program; don't do all of the
function stuff for mfp if you won't use it because it will just soak up several pages of rom for
nothing.

Default for functions/macros is for them to operate on singe words (eg "add" adds a word to another word),
and word size is 16 bits because that is the size for everything.

Note that "addr" refers to the program address in rom unless otherwise stated (except for in the
instruction itself, where the addr mnemonic refers to the address register for ram).

Note that, unless otherwise stated, the c functions act as macros, eg "calladd" will write the code
that calls the addition function at that rom address, or "genpred16" will write the code to get the
predecessor of the general register at that rom address.

Version 1_00: doesnt work as of now, or at least it apppears to fail with the add32() macro.

Ensure that your pointers to ram and rom are allocated before you attempt to use them. For variables,
since they are generally global in the compiler function, it is best to just declare them in the very
beginning of the file. For example, ADD_ret is used in the function addcode() but also in the macro
calladd().

Also, in this version, for add32(), you MUST BUFFER the sum. If you try to do something like a = a + b
without buffering the sum, it will work fine on a 16 bit level. However, for 32 bits done with the
current method (which is to add as two 16 bit sets and then see if a carry occurred), you need
to know both the addends' values AND the value of the sum to figure out whether a carry occurred
to adjust the final value. The bufferring just occurs within the add32() macro and the answer is transferred
back into where it is supposed to be afterwards.

So, calladd(), callmfp(), and add32() will give you the correct answer even if the pointers they are
given point to some of the same numbers.
