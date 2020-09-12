<
Writing alternating high and low values to registers
in different patterns, then switching quickly between
them to	see if there are short circuits during the
transition period.
>
0000 imm adr0 ffff 0001
0001 imm adr1 5555 0002
0002 imm gen0 ffff 0003
0003 imm gen1 aaaa 0004
0004 imm out0 ffff 0005
0005 imm out1 5555 0006
0006 imm dir0 ffff 0007
0007 imm dir1 aaaa 0008
0008 imm rall aaaa 0010
0010 adr noop 0000 0011
0011 gen noop 0000 0012
0012 rol noop 0000 0013
0013 dir noop 0000 0014
0014 ror noop 0000 0015
0015 imm noop aaaa 0016
0016 out noop 0000 0017
0017 ram noop 0000 0010
