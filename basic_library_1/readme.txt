This will provide functions to define addition and fixed point multiplication
(1,3,12). It will also provide a macro to read from the keypad and macros to do things with the
lcd. It will provide macros to do things like write the predecessor of a register to itself.
It will also provide compiler functions to actually write instructions in the first place,
manage the next addresses to create conditional loops in slightly less painful ways
than guessing how big ranges are, and compiler functions to allocate and use labels for
rom addresses.

Move the library version into whatever <include> folder you are using for your c programs,
then discard the version extension (eg it should become bl.h). This is just done for ease
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
	fd = fopen(argv[2], "w+");
	makeaddlabel();
	makemfplabel();
	makeaddvariables();
	makemfpvariables();
	makelabel(_next);
	inst("imm dir0 ffff");
	lcdinit();
<end:>
	mfpcode();
	addcode();
	replacemfpcall();
	replaceaddcall();
	removex88(argv[1]);
	fclose(fd);

Note that addr, labelcount, and fd are included within the library as global variables and should
not be re-defined.
