0000 imm dir1 ffff 0001
0001 imm out1 ffff 0002
0002 imm out0 1000 0003
0003 imm gen1 ffff 0005
0005 gen jzor 00ff 0006
0006 gen jzor 0fff 0008
0007 gen jzor 000f 000a
0008 gen jzor 3fff 000c
0009 gen jzor 03ff 000e
000a gen jzor 003f 0010
000b gen jzor 0003 0012
000c gen jzor 7fff 0014
000d gen jzor 1fff 0016
000e gen jzor 07ff 0018
000f gen jzor 01ff 001a
0010 gen jzor 007f 001c
0011 gen jzor 001f 001e
0012 gen jzor 0007 0020
0013 gen jzor 0001 0022
0014 gen jzor ffff 0024
0015 imm gen1 3fff 0027
0016 imm gen1 1fff 0028
0017 imm gen1 0fff 0029
0018 imm gen1 07ff 002a
0019 imm gen1 03ff 002b
001a imm gen1 01ff 002c
001b imm gen1 00ff 002d
001c imm gen1 007f 002e
001d imm gen1 003f 002f
001e imm gen1 001f 0030
001f imm gen1 000f 0031
0020 imm gen1 0007 0032
0021 imm gen1 0003 0033
0022 imm gen1 0001 0034
0023 imm gen0 0001 0035
0024 imm gen1 ffff 0035
0025 imm gen1 7fff 0026
0026 imm gen0 8000 0035
0027 imm gen0 4000 0035
0028 imm gen0 2000 0035
0029 imm gen0 1000 0035
002a imm gen0 0800 0035
002b imm gen0 0400 0035
002c imm gen0 0200 0035
002d imm gen0 0100 0035
002e imm gen0 0080 0035
002f imm gen0 0040 0035
0030 imm gen0 0020 0035
0031 imm gen0 0010 0035
0032 imm gen0 0008 0035
0033 imm gen0 0004 0035
0034 imm gen0 0002 0035
0035 gen jzor ffff 0036
0036 dnc noop 0000 0038
0037 dnc noop 0000 0005
0038 imm out1 1000 0039
0039 imm gen1 ffff 003b
003b gen jzor 00ff 003c
003c gen jzor 0fff 003e
003d gen jzor 000f 0040
003e gen jzor 3fff 0042
003f gen jzor 03ff 0044
0040 gen jzor 003f 0046
0041 gen jzor 0003 0048
0042 gen jzor 7fff 004a
0043 gen jzor 1fff 004c
0044 gen jzor 07ff 004e
0045 gen jzor 01ff 0050
0046 gen jzor 007f 0052
0047 gen jzor 001f 0054
0048 gen jzor 0007 0056
0049 gen jzor 0001 0058
004a gen jzor ffff 005a
004b imm gen1 3fff 005d
004c imm gen1 1fff 005e
004d imm gen1 0fff 005f
004e imm gen1 07ff 0060
004f imm gen1 03ff 0061
0050 imm gen1 01ff 0062
0051 imm gen1 00ff 0063
0052 imm gen1 007f 0064
0053 imm gen1 003f 0065
0054 imm gen1 001f 0066
0055 imm gen1 000f 0067
0056 imm gen1 0007 0068
0057 imm gen1 0003 0069
0058 imm gen1 0001 006a
0059 imm gen0 0001 006b
005a imm gen1 ffff 006b
005b imm gen1 7fff 005c
005c imm gen0 8000 006b
005d imm gen0 4000 006b
005e imm gen0 2000 006b
005f imm gen0 1000 006b
0060 imm gen0 0800 006b
0061 imm gen0 0400 006b
0062 imm gen0 0200 006b
0063 imm gen0 0100 006b
0064 imm gen0 0080 006b
0065 imm gen0 0040 006b
0066 imm gen0 0020 006b
0067 imm gen0 0010 006b
0068 imm gen0 0008 006b
0069 imm gen0 0004 006b
006a imm gen0 0002 006b
006b gen jzor ffff 006c
006c dnc noop 0000 006e
006d dnc noop 0000 003b
006e imm out0 0c00 006f
006f imm out0 00ff 0070
0070 imm out1 0038 0071
0071 imm out0 0200 0072
0072 imm out1 0200 0073
0073 imm out0 00ff 0074
0074 imm out1 0011 0075
0075 imm out0 0200 0076
0076 imm out1 0200 0077
0077 imm out0 00ff 0078
0078 imm out1 0013 0079
0079 imm out0 0200 007a
007a imm out1 0200 007b
007b imm out0 00ff 007c
007c imm out1 0020 007d
007d imm out0 0200 007e
007e imm out1 0200 007f
007f imm out0 00ff 0080
0080 imm out1 0029 0081
0081 imm out0 0200 0082
0082 imm out1 0200 0083
0083 imm out0 0400 0084
0084 imm out0 00ff 0085
0085 imm out1 000c 0086
0086 imm out0 0200 0087
0087 imm out1 0200 0088
0088 imm out1 0400 0089
0089 imm out0 00ff 008a
008a imm out1 00e6 008b
008b imm out0 0200 008c
008c imm out1 0200 008d
008d imm out0 0400 008e
008e imm out0 00ff 008f
008f imm out1 002a 0090
0090 imm out0 0200 0091
0091 imm out1 0200 0092
0092 imm out1 0400 0093
0093 imm out0 00ff 0094
0094 imm out1 0000 0095
0095 imm out0 0200 0096
0096 imm out1 0200 0097
0097 imm out0 00ff 0098
0098 imm out1 0000 0099
0099 imm out0 0200 009a
009a imm out1 0200 009b
009b imm out0 00ff 009c
009c imm out1 0000 009d
009d imm out0 0200 009e
009e imm out1 0200 009f
009f imm out0 00ff 00a0
00a0 imm out1 0040 00a1
00a1 imm out0 0200 00a2
00a2 imm out1 0200 00a3
00a3 imm out0 0400 00a4
00a4 imm out0 00ff 00a5
00a5 imm out1 002b 00a6
00a6 imm out0 0200 00a7
00a7 imm out1 0200 00a8
00a8 imm out1 0400 00a9
00a9 imm out0 00ff 00aa
00aa imm out1 0000 00ab
00ab imm out0 0200 00ac
00ac imm out1 0200 00ad
00ad imm out0 00ff 00ae
00ae imm out1 0000 00af
00af imm out0 0200 00b0
00b0 imm out1 0200 00b1
00b1 imm out0 00ff 00b2
00b2 imm out1 0000 00b3
00b3 imm out0 0200 00b4
00b4 imm out1 0200 00b5
00b5 imm out0 00ff 00b6
00b6 imm out1 003f 00b7
00b7 imm out0 0200 00b8
00b8 imm out1 0200 00b9
00b9 imm out0 0400 00ba
00ba imm out0 00ff 00bb
00bb imm out1 002c 00bc
00bc imm out0 0200 00bd
00bd imm out1 0200 00be
00be imm out1 0400 00bf
00bf imm out0 0400 00c0
00c0 imm out0 00ff 00c1
00c1 imm out1 0000 00c2
00c2 imm out0 0200 00c3
00c3 imm out1 0200 00c4
00c4 imm out0 0400 00c5
00c5 imm out0 00ff 00c6
00c6 imm out1 002a 00c7
00c7 imm out0 0200 00c8
00c8 imm out1 0200 00c9
00c9 imm out1 0400 00ca
00ca imm out0 00ff 00cb
00cb imm out1 0000 00cc
00cc imm out0 0200 00cd
00cd imm out1 0200 00ce
00ce imm out0 00ff 00cf
00cf imm out1 0000 00d0
00d0 imm out0 0200 00d1
00d1 imm out1 0200 00d2
00d2 imm out0 00ff 00d3
00d3 imm out1 0000 00d4
00d4 imm out0 0200 00d5
00d5 imm out1 0200 00d6
00d6 imm out0 00ff 00d7
00d7 imm out1 00ef 00d8
00d8 imm out0 0200 00d9
00d9 imm out1 0200 00da
00da imm out0 0400 00db
00db imm out0 00ff 00dc
00dc imm out1 002b 00dd
00dd imm out0 0200 00de
00de imm out1 0200 00df
00df imm out1 0400 00e0
00e0 imm out0 00ff 00e1
00e1 imm out1 0000 00e2
00e2 imm out0 0200 00e3
00e3 imm out1 0200 00e4
00e4 imm out0 00ff 00e5
00e5 imm out1 0000 00e6
00e6 imm out0 0200 00e7
00e7 imm out1 0200 00e8
00e8 imm out0 00ff 00e9
00e9 imm out1 0001 00ea
00ea imm out0 0200 00eb
00eb imm out1 0200 00ec
00ec imm out0 00ff 00ed
00ed imm out1 003f 00ee
00ee imm out0 0200 00ef
00ef imm out1 0200 00f0
00f0 imm out0 0400 00f1
00f1 imm out0 00ff 00f2
00f2 imm out1 003c 00f3
00f3 imm out0 0200 00f4
00f4 imm out1 0200 00f5
00f5 imm out1 0400 00f6
00f6 imm addr0 ffff 00f7
00f7 imm addr1 9600 00f9
00f9 addr jzor 00ff 00fa
00fa addr jzor 0fff 00fc
00fb addr jzor 000f 00fe
00fc addr jzor 3fff 0100
00fd addr jzor 03ff 0102
00fe addr jzor 003f 0104
00ff addr jzor 0003 0106
0100 addr jzor 7fff 0108
0101 addr jzor 1fff 010a
0102 addr jzor 07ff 010c
0103 addr jzor 01ff 010e
0104 addr jzor 007f 0110
0105 addr jzor 001f 0112
0106 addr jzor 0007 0114
0107 addr jzor 0001 0116
0108 addr jzor ffff 0118
0109 imm addr1 3fff 011b
010a imm addr1 1fff 011c
010b imm addr1 0fff 011d
010c imm addr1 07ff 011e
010d imm addr1 03ff 011f
010e imm addr1 01ff 0120
010f imm addr1 00ff 0121
0110 imm addr1 007f 0122
0111 imm addr1 003f 0123
0112 imm addr1 001f 0124
0113 imm addr1 000f 0125
0114 imm addr1 0007 0126
0115 imm addr1 0003 0127
0116 imm addr1 0001 0128
0117 imm addr0 0001 0129
0118 imm addr1 ffff 0129
0119 imm addr1 7fff 011a
011a imm addr0 8000 0129
011b imm addr0 4000 0129
011c imm addr0 2000 0129
011d imm addr0 1000 0129
011e imm addr0 0800 0129
011f imm addr0 0400 0129
0120 imm addr0 0200 0129
0121 imm addr0 0100 0129
0122 imm addr0 0080 0129
0123 imm addr0 0040 0129
0124 imm addr0 0020 0129
0125 imm addr0 0010 0129
0126 imm addr0 0008 0129
0127 imm addr0 0004 0129
0128 imm addr0 0002 0129
0129 imm out0 00ff 012a
012a imm out1 00fc 012b
012b imm out0 0200 012c
012c imm out1 0200 012d
012d imm out0 00ff 012e
012e imm out1 0000 012f
012f imm out0 0200 0130
0130 imm out1 0200 0131
0131 imm out0 00ff 0132
0132 imm out1 0000 0133
0133 imm out0 0200 0134
0134 imm out1 0200 0135
0135 imm out0 00ff 0136
0136 imm out1 00fc 0137
0137 imm out0 0200 0138
0138 imm out1 0200 0139
0139 imm out0 00ff 013a
013a imm out1 0000 013b
013b imm out0 0200 013c
013c imm out1 0200 013d
013d imm out0 00ff 013e
013e imm out1 0000 013f
013f imm out0 0200 0140
0140 imm out1 0200 0141
0141 addr jzor ffff 0142
0142 dnc noop 0000 0144
0143 dnc noop 0000 00f9
0144 imm out0 0400 0145
0145 imm out0 00ff 0146
0146 imm out1 0000 0147
0147 imm out0 0200 0148
0148 imm out1 0200 0149
0149 imm out0 0400 014a
014a imm out0 00ff 014b
014b imm out1 0000 014c
014c imm out0 0200 014d
014d imm out1 0200 014e
014e imm out0 0400 014f
014f imm out0 00ff 0150
0150 imm out1 002a 0151
0151 imm out0 0200 0152
0152 imm out1 0200 0153
0153 imm out1 0400 0154
0154 imm out0 00ff 0155
0155 imm out1 0000 0156
0156 imm out0 0200 0157
0157 imm out1 0200 0158
0158 imm out0 00ff 0159
0159 imm out1 0021 015a
015a imm out0 0200 015b
015b imm out1 0200 015c
015c imm out0 00ff 015d
015d imm out1 0000 015e
015e imm out0 0200 015f
015f imm out1 0200 0160
0160 imm out0 00ff 0161
0161 imm out1 002c 0162
0162 imm out0 0200 0163
0163 imm out1 0200 0164
0164 imm out0 0400 0165
0165 imm out0 00ff 0166
0166 imm out1 002b 0167
0167 imm out0 0200 0168
0168 imm out1 0200 0169
0169 imm out1 0400 016a
016a imm out0 00ff 016b
016b imm out1 0000 016c
016c imm out0 0200 016d
016d imm out1 0200 016e
016e imm out0 00ff 016f
016f imm out1 0021 0170
0170 imm out0 0200 0171
0171 imm out1 0200 0172
0172 imm out0 00ff 0173
0173 imm out1 0000 0174
0174 imm out0 0200 0175
0175 imm out1 0200 0176
0176 imm out0 00ff 0177
0177 imm out1 002c 0178
0178 imm out0 0200 0179
0179 imm out1 0200 017a
017a imm out0 0400 017b
017b imm out0 00ff 017c
017c imm out1 003c 017d
017d imm out0 0200 017e
017e imm out1 0200 017f
017f imm out1 0400 0180
0180 imm addr0 ffff 0181
0181 imm addr1 0048 0183
0183 addr jzor 00ff 0184
0184 addr jzor 0fff 0186
0185 addr jzor 000f 0188
0186 addr jzor 3fff 018a
0187 addr jzor 03ff 018c
0188 addr jzor 003f 018e
0189 addr jzor 0003 0190
018a addr jzor 7fff 0192
018b addr jzor 1fff 0194
018c addr jzor 07ff 0196
018d addr jzor 01ff 0198
018e addr jzor 007f 019a
018f addr jzor 001f 019c
0190 addr jzor 0007 019e
0191 addr jzor 0001 01a0
0192 addr jzor ffff 01a2
0193 imm addr1 3fff 01a5
0194 imm addr1 1fff 01a6
0195 imm addr1 0fff 01a7
0196 imm addr1 07ff 01a8
0197 imm addr1 03ff 01a9
0198 imm addr1 01ff 01aa
0199 imm addr1 00ff 01ab
019a imm addr1 007f 01ac
019b imm addr1 003f 01ad
019c imm addr1 001f 01ae
019d imm addr1 000f 01af
019e imm addr1 0007 01b0
019f imm addr1 0003 01b1
01a0 imm addr1 0001 01b2
01a1 imm addr0 0001 01b3
01a2 imm addr1 ffff 01b3
01a3 imm addr1 7fff 01a4
01a4 imm addr0 8000 01b3
01a5 imm addr0 4000 01b3
01a6 imm addr0 2000 01b3
01a7 imm addr0 1000 01b3
01a8 imm addr0 0800 01b3
01a9 imm addr0 0400 01b3
01aa imm addr0 0200 01b3
01ab imm addr0 0100 01b3
01ac imm addr0 0080 01b3
01ad imm addr0 0040 01b3
01ae imm addr0 0020 01b3
01af imm addr0 0010 01b3
01b0 imm addr0 0008 01b3
01b1 imm addr0 0004 01b3
01b2 imm addr0 0002 01b3
01b3 imm out0 00ff 01b4
01b4 imm out1 002c 01b5
01b5 imm out0 0200 01b6
01b6 imm out1 0200 01b7
01b7 imm out0 00ff 01b8
01b8 imm out1 002c 01b9
01b9 imm out0 0200 01ba
01ba imm out1 0200 01bb
01bb imm out0 00ff 01bc
01bc imm out1 002c 01bd
01bd imm out0 0200 01be
01be imm out1 0200 01bf
01bf imm out0 00ff 01c0
01c0 imm out1 002c 01c1
01c1 imm out0 0200 01c2
01c2 imm out1 0200 01c3
01c3 imm out0 00ff 01c4
01c4 imm out1 002c 01c5
01c5 imm out0 0200 01c6
01c6 imm out1 0200 01c7
01c7 imm out0 00ff 01c8
01c8 imm out1 002c 01c9
01c9 imm out0 0200 01ca
01ca imm out1 0200 01cb
01cb addr jzor ffff 01cc
01cc dnc noop 0000 01ce
01cd dnc noop 0000 0183
01ce imm out0 0400 01cf
01cf imm out0 00ff 01d0
01d0 imm out1 0000 01d1
01d1 imm out0 0200 01d2
01d2 imm out1 0200 01d3
01d3 imm out0 0400 01d4
01d4 imm out0 00ff 01d5
01d5 imm out1 0000 01d6
01d6 imm out0 0200 01d7
01d7 imm out1 0200 01d8
01d8 imm out0 0400 01d9
01d9 imm out0 00ff 01da
01da imm out1 002a 01db
01db imm out0 0200 01dc
01dc imm out1 0200 01dd
01dd imm out1 0400 01de
01de imm out0 00ff 01df
01df imm out1 0000 01e0
01e0 imm out0 0200 01e1
01e1 imm out1 0200 01e2
01e2 imm out0 00ff 01e3
01e3 imm out1 0001 01e4
01e4 imm out0 0200 01e5
01e5 imm out1 0200 01e6
01e6 imm out0 00ff 01e7
01e7 imm out1 0000 01e8
01e8 imm out0 0200 01e9
01e9 imm out1 0200 01ea
01ea imm out0 00ff 01eb
01eb imm out1 0064 01ec
01ec imm out0 0200 01ed
01ed imm out1 0200 01ee
01ee imm out0 0400 01ef
01ef imm out0 00ff 01f0
01f0 imm out1 002b 01f1
01f1 imm out0 0200 01f2
01f2 imm out1 0200 01f3
01f3 imm out1 0400 01f4
01f4 imm out0 00ff 01f5
01f5 imm out1 0000 01f6
01f6 imm out0 0200 01f7
01f7 imm out1 0200 01f8
01f8 imm out0 00ff 01f9
01f9 imm out1 0032 01fa
01fa imm out0 0200 01fb
01fb imm out1 0200 01fc
01fc imm out0 00ff 01fd
01fd imm out1 0000 01fe
01fe imm out0 0200 01ff
01ff imm out1 0200 0200
0200 imm out0 00ff 0201
0201 imm out1 0046 0202
0202 imm out0 0200 0203
0203 imm out1 0200 0204
0204 imm out0 0400 0205
0205 imm out0 00ff 0206
0206 imm out1 003c 0207
0207 imm out0 0200 0208
0208 imm out1 0200 0209
0209 imm out1 0400 020a
020a imm addr0 ffff 020b
020b imm addr1 041a 020d
020d addr jzor 00ff 020e
020e addr jzor 0fff 0210
020f addr jzor 000f 0212
0210 addr jzor 3fff 0214
0211 addr jzor 03ff 0216
0212 addr jzor 003f 0218
0213 addr jzor 0003 021a
0214 addr jzor 7fff 021c
0215 addr jzor 1fff 021e
0216 addr jzor 07ff 0220
0217 addr jzor 01ff 0222
0218 addr jzor 007f 0224
0219 addr jzor 001f 0226
021a addr jzor 0007 0228
021b addr jzor 0001 022a
021c addr jzor ffff 022c
021d imm addr1 3fff 022f
021e imm addr1 1fff 0230
021f imm addr1 0fff 0231
0220 imm addr1 07ff 0232
0221 imm addr1 03ff 0233
0222 imm addr1 01ff 0234
0223 imm addr1 00ff 0235
0224 imm addr1 007f 0236
0225 imm addr1 003f 0237
0226 imm addr1 001f 0238
0227 imm addr1 000f 0239
0228 imm addr1 0007 023a
0229 imm addr1 0003 023b
022a imm addr1 0001 023c
022b imm addr0 0001 023d
022c imm addr1 ffff 023d
022d imm addr1 7fff 022e
022e imm addr0 8000 023d
022f imm addr0 4000 023d
0230 imm addr0 2000 023d
0231 imm addr0 1000 023d
0232 imm addr0 0800 023d
0233 imm addr0 0400 023d
0234 imm addr0 0200 023d
0235 imm addr0 0100 023d
0236 imm addr0 0080 023d
0237 imm addr0 0040 023d
0238 imm addr0 0020 023d
0239 imm addr0 0010 023d
023a imm addr0 0008 023d
023b imm addr0 0004 023d
023c imm addr0 0002 023d
023d imm out0 00ff 023e
023e imm out1 0000 023f
023f imm out0 0200 0240
0240 imm out1 0200 0241
0241 imm out0 00ff 0242
0242 imm out1 00c8 0243
0243 imm out0 0200 0244
0244 imm out1 0200 0245
0245 imm out0 00ff 0246
0246 imm out1 0000 0247
0247 imm out0 0200 0248
0248 imm out1 0200 0249
0249 imm out0 00ff 024a
024a imm out1 0000 024b
024b imm out0 0200 024c
024c imm out1 0200 024d
024d imm out0 00ff 024e
024e imm out1 00c8 024f
024f imm out0 0200 0250
0250 imm out1 0200 0251
0251 imm out0 00ff 0252
0252 imm out1 0000 0253
0253 imm out0 0200 0254
0254 imm out1 0200 0255
0255 addr jzor ffff 0256
0256 dnc noop 0000 0258
0257 dnc noop 0000 020d
0258 imm out0 0400 0259
0259 imm out0 00ff 025a
025a imm out1 0000 025b
025b imm out0 0200 025c
025c imm out1 0200 025d
025d imm out0 0400 025e
025e imm out0 00ff 025f
025f imm out1 0000 0260
0260 imm out0 0200 0261
0261 imm out1 0200 0262
0262 dnc noop 0000 0262