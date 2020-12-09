if [ "$#" -ne 1 ]; then
	echo "need file stem";
else
	gcc "$1.c" -o "$1";
	./"$1" "$1.asm" "$1.x88";
	assemble "$1.asm" "$1.up" "$1.buff";
	romsend "$1.up";
fi
