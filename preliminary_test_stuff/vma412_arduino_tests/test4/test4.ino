//
// ILI9341 ID Checker against data sheet http://www.adafruit.com/datasheets/ILI9341.pdf
// Ugly bit basher - R Jennings 11/11/14 (http://www.shaders.co.uk)
//
 #define CS A3   // Chip Select goes to Analog 3
 #define CD A2   // Command/Data goes to Analog 2
 #define WR A1   // LCD Write goes to Analog 1
 #define RD A0   // LCD Read goes to Analog 0
 #define RST A4  // Can alternately just connect to Arduino's reset pin
 #define D0 8    // DO
 #define D1 9    // D1
 #define D2 2    // D2
 #define D3 3    // D3
 #define D4 4    // D4
 #define D5 5    // D5
 #define D6 6    // D6
 #define D7 7    // D7

void setup(void) {
// Set up port directions and default states
  pinMode(CS, OUTPUT);  pinMode(CD, OUTPUT);  pinMode(WR, OUTPUT);  pinMode(RD, OUTPUT);  pinMode(RST, OUTPUT);
  digitalWrite(CS, 1); digitalWrite(CD, 1); digitalWrite(WR, 1); digitalWrite(RD, 1); digitalWrite(RST, 1);
  pinMode(D0, OUTPUT); pinMode(D1, OUTPUT); pinMode(D2, OUTPUT); pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT); pinMode(D5, OUTPUT); pinMode(D6, OUTPUT); pinMode(D7, OUTPUT);

// This just drivers the serial monitor no screen activity 
  Serial.begin(9600);
  Serial.println("TFT LCD test - Read of the ID register");
//HW Reset
  digitalWrite(RST,0); delay(5);
  digitalWrite(RST,1); delay(500); 
// Read the ID Register, D3h
// Start by writing to the command register with the register to read
  digitalWrite(CD, 0);
  digitalWrite(CS, 0);
  digitalWrite(D0, 1); digitalWrite(D1, 1); digitalWrite(D2, 0); digitalWrite(D3, 0);  // 3 - Change for lower nibble as required
  digitalWrite(D4, 1); digitalWrite(D5, 0); digitalWrite(D6, 1); digitalWrite(D7, 1);  // D - Change for upper nibble as required
  digitalWrite(WR, 0); digitalWrite(WR, 1);
  digitalWrite(CS, 1);   
// Switch to reading   
  pinMode(D0, INPUT);    pinMode(D1, INPUT);    pinMode(D2, INPUT);    pinMode(D3, INPUT);
  pinMode(D4, INPUT);    pinMode(D5, INPUT);    pinMode(D6, INPUT);    pinMode(D7, INPUT);
  int x=0;
// Then read the four bytes and print to the serial port
// See section 8.3.23 page 233 for read sequence
// 1st Parameter - Dummy byte could be anything
  digitalWrite(CD, 1);
  digitalWrite(CS, 0);
  digitalWrite(RD, 0);
  x=0;
  digitalWrite(RD, 0);
  x|=digitalRead(D0);    x|=digitalRead(D1)<<1; x|=digitalRead(D2)<<2; x|=digitalRead(D3)<<3;
  x|=digitalRead(D4)<<4; x|=digitalRead(D5)<<5; x|=digitalRead(D6)<<6; x|=digitalRead(D7)<<7;
  Serial.print("Dummny Byte Read =        "); Serial.println(x, HEX);   
  digitalWrite(RD, 1);
 // 2nd Parameter should be 00h
  x=0;
  digitalWrite(RD, 0);
  x|=digitalRead(D0);    x|=digitalRead(D1)<<1; x|=digitalRead(D2)<<2; x|=digitalRead(D3)<<3;
  x|=digitalRead(D4)<<4; x|=digitalRead(D5)<<5; x|=digitalRead(D6)<<6; x|=digitalRead(D7)<<7;
  Serial.print("2nd Parameter Byte Read = "); Serial.println(x, HEX);   
  digitalWrite(RD, 1);
 // 3rd Parameter should be 93h   
  x=0;
  digitalWrite(RD, 0);
  x|=digitalRead(D0);    x|=digitalRead(D1)<<1;  x|=digitalRead(D2)<<2; x|=digitalRead(D3)<<3;
  x|=digitalRead(D4)<<4; x|=digitalRead(D5)<<5;  x|=digitalRead(D6)<<6; x|=digitalRead(D7)<<7;
  Serial.print("3rd Parameter Byte Read = "); Serial.println(x, HEX);   
  digitalWrite(RD, 1);
 // 4th Parameter should be 41h     
  x=0;
  digitalWrite(RD, 0);
  x|=digitalRead(D0);    x|=digitalRead(D1)<<1; x|=digitalRead(D2)<<2; x|=digitalRead(D3)<<3;
  x|=digitalRead(D4)<<4; x|=digitalRead(D5)<<5; x|=digitalRead(D6)<<6; x|=digitalRead(D7)<<7;
  Serial.print("4th Parameter Byte Read = "); Serial.println(x, HEX);   
  digitalWrite(RD, 1);

  digitalWrite(CS, 1);
  
  // Start by writing to the command register with the command cause i think thats how this works - this should be memory write
  digitalWrite(CD, 0);
  digitalWrite(CS, 0);
  digitalWrite(D0, 0); digitalWrite(D1, 0); digitalWrite(D2, 1); digitalWrite(D3, 1);  // C - Change for lower nibble as required
  digitalWrite(D4, 0); digitalWrite(D5, 1); digitalWrite(D6, 0); digitalWrite(D7, 0);  // 2 - Change for upper nibble as required
  digitalWrite(WR, 0); digitalWrite(WR, 1);
  digitalWrite(CS, 1);
  // dump 2 data points
  digitalWrite(CD, 1);
  digitalWrite(CS, 0);
  
  digitalWrite(D0, 0); digitalWrite(D1, 0); digitalWrite(D2, 0); digitalWrite(D3, 0);  // 0 - Change for lower nibble as required
  digitalWrite(D4, 0); digitalWrite(D5, 0); digitalWrite(D6, 1); digitalWrite(D7, 0);  // 4 - Change for upper nibble as required
  digitalWrite(WR, 0); digitalWrite(WR, 1);

  digitalWrite(D0, 0); digitalWrite(D1, 0); digitalWrite(D2, 0); digitalWrite(D3, 0);  // 0 - Change for lower nibble as required
  digitalWrite(D4, 0); digitalWrite(D5, 0); digitalWrite(D6, 1); digitalWrite(D7, 0);  // 4 - Change for upper nibble as required
  digitalWrite(WR, 0); digitalWrite(WR, 1);
  
  digitalWrite(CS, 1);

  //can you at least turn the display on/off????

  digitalWrite(CD, 0);
  digitalWrite(RD, 1);
  digitalWrite(CS, 0);
  digitalWrite(D0, 0); digitalWrite(D1, 0); digitalWrite(D2, 0); digitalWrite(D3, 1);  // 8 - Change for lower nibble as required
  digitalWrite(D4, 0); digitalWrite(D5, 1); digitalWrite(D6, 0); digitalWrite(D7, 0);  // 2 - Change for upper nibble as required
  digitalWrite(WR, 0); digitalWrite(WR, 1);
  digitalWrite(CS, 1);

  Serial.print("so...\n");
  
}
//Nothing to do here except ur mom but already did her so...
void loop(void) {
}
