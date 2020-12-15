unsigned short addr;
FILE* fd;
long labelcount;

//note: _KEY* pinouts can be changed to anything you want
#define _KEY_R1 0x0008 //input (with 1k pull up)
#define _KEY_R2 0x0004 //input (with 1k pull up)
#define _KEY_R3 0x0002 //input (with 1k pull up)
#define _KEY_R4 0x0001 //input (with 1k pull up)
#define _KEY_C1 0x8000 //output (thru a 100ohm resistor)
#define _KEY_C2 0x4000 //output (thru a 100ohm resistor)
#define _KEY_C3 0x2000 //output (thru a 100ohm resistor)

//note: _LCD* pinouts can be changed to anything you want as well
#define _LCD_RST 0x1000 //output (thru a 100ohm resistor)
#define _LCD_CS 0x0800 //output (thru a 100ohm resistor)
#define _LCD_RS 0x0400 //output (thru a 100ohm resistor)
#define _LCD_WR 0x0200 //output (thru a 100ohm resistor)
#define _LCD_RD 0x0100 //output (thru a 100ohm resistor)

/*
 * Data bus: (cannot be changed easily; must be lower byte of output bus)
 * _LCD_Dn corresponds to output 0xn (thru a 100ohm resistor)
 * where "n" is the number data bit.
 * Note that output 0xn refers to the position of the bit
 * on the output bus using little endian enumeration.
 * This is not defined because it is easier to just remember
 * that the lower half of the output bus is LCD data.
 * The use of 100ohm resistors is just to protect the map25.2
 */

#define ADD_ADDEND0 0x0000
#define ADD_ADDEND1 0x0001
#define ADD_SUM 0x0002
#define ADD_RET 0x0003
unsigned short add_loc;
char _add_loc_label[5];

#define MFP_ARRAY 0x0010
#define MFP_F0 0x0004
#define MFP_F1 0x0005
#define MFP_PROD 0x0006
#define MFP_RET 0x0007
unsigned short mfp_loc;
char _mfp_loc_label[5];
