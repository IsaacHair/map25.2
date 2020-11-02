//
// ILI9341 ID Checker against data sheet http://www.adafruit.com/datasheets/ILI9341.pdf
// Ugly bit basher - R Jennings 11/11/14 (http://www.shaders.co.uk)
// ACTUALLY YA BOI ISAAC COPIED THIS GUYS WORK, thanks man!
// This guy is using inverted outputs because it is going thru the sheild
// meant for use with map25.2 to test it
// I am also going to modify the pinout to interface with the adapter to map25.2.
// only data is inverted, not control pins
//
 #define CS A2   // Chip Select goes to Analog 3
 #define CD A3   // Command/Data goes to Analog 2
 #define WR A4   // LCD Write goes to Analog 1
 #define RD A5   // LCD Read goes to Analog 0
 #define RST A1  // Can alternately just connect to Arduino's reset pin
 #define D0 2    // DO
 #define D1 3    // D1
 #define D2 4    // D2
 #define D3 5    // D3
 #define D4 6    // D4
 #define D5 7    // D5
 #define D6 8    // D6
 #define D7 9    // D7

void setup(void) {
  int x;
// Set up port directions and default states
  pinMode(CS, OUTPUT);  pinMode(CD, OUTPUT);  pinMode(WR, OUTPUT);  pinMode(RD, OUTPUT);  pinMode(RST, OUTPUT);
  digitalWrite(CS, 1); digitalWrite(CD, 1); digitalWrite(WR, 1); digitalWrite(RD, 1); digitalWrite(RST, 1);
  pinMode(D0, OUTPUT); pinMode(D1, OUTPUT); pinMode(D2, OUTPUT); pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT); pinMode(D5, OUTPUT); pinMode(D6, OUTPUT); pinMode(D7, OUTPUT);
  
//HW Reset
  digitalWrite(RST,0); delay(5);
  digitalWrite(RST,1); delay(500);

  //set all parameters and get ready to write  
  pinMode(D0, OUTPUT);    pinMode(D1, OUTPUT);    pinMode(D2, OUTPUT);    pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);    pinMode(D5, OUTPUT);    pinMode(D6, OUTPUT);    pinMode(D7, OUTPUT);
  digitalWrite(RD, 1);
  digitalWrite(CS, 1);
  digitalWrite(WR, 1);
  digitalWrite(CD, 1);

  //out of IDLE mode
  digitalWrite(CS, 0);
  digitalWrite(CD, 0);
  digitalWrite(D0, 1); digitalWrite(D1, 1); digitalWrite(D2, 1); digitalWrite(D3, 0);  // 8 - Change for lower nibble as required
  digitalWrite(D4, 0); digitalWrite(D5, 0); digitalWrite(D6, 1); digitalWrite(D7, 1);  // 3 - Change for upper nibble as required
  digitalWrite(WR, 0); digitalWrite(WR, 1);

  //out of SLEEP mode
  digitalWrite(D0, 0); digitalWrite(D1, 1); digitalWrite(D2, 1); digitalWrite(D3, 1);  // 1 - Change for lower nibble as required
  digitalWrite(D4, 0); digitalWrite(D5, 1); digitalWrite(D6, 1); digitalWrite(D7, 1);  // 1 - Change for upper nibble as required
  digitalWrite(WR, 0); digitalWrite(WR, 1);
  
  //display on mode
  digitalWrite(D0, 0); digitalWrite(D1, 0); digitalWrite(D2, 1); digitalWrite(D3, 1);  // 3 - Change for lower nibble as required
  digitalWrite(D4, 0); digitalWrite(D5, 1); digitalWrite(D6, 1); digitalWrite(D7, 1);  // 1 - Change for upper nibble as required
  digitalWrite(WR, 0); digitalWrite(WR, 1);

  //set COLMOD
  digitalWrite(D0, 1); digitalWrite(D1, 1); digitalWrite(D2, 0); digitalWrite(D3, 0);  // C - Change for lower nibble as required
  digitalWrite(D4, 1); digitalWrite(D5, 1); digitalWrite(D6, 1); digitalWrite(D7, 1);  // 0 - Change for upper nibble as required
  digitalWrite(WR, 0); digitalWrite(WR, 1);
  digitalWrite(CD, 1);
  digitalWrite(D0, 1); digitalWrite(D1, 0); digitalWrite(D2, 0); digitalWrite(D3, 1);  // 6 - Change for lower nibble as required
  digitalWrite(D4, 1); digitalWrite(D5, 0); digitalWrite(D6, 0); digitalWrite(D7, 0);  // e - Change for upper nibble as required
  digitalWrite(WR, 0); digitalWrite(WR, 1);

  //invsersion off
  digitalWrite(CD, 0);
  digitalWrite(D0, 1); digitalWrite(D1, 1); digitalWrite(D2, 1); digitalWrite(D3, 1);  // 0 - Change for lower nibble as required
  digitalWrite(D4, 1); digitalWrite(D5, 0); digitalWrite(D6, 1); digitalWrite(D7, 1);  // 2 - Change for upper nibble as required
  digitalWrite(WR, 0); digitalWrite(WR, 1);

  //turn memory to screen on
  digitalWrite(D0, 0); digitalWrite(D1, 1); digitalWrite(D2, 1); digitalWrite(D3, 0);  // 9 - Change for lower nibble as required
  digitalWrite(D4, 1); digitalWrite(D5, 0); digitalWrite(D6, 1); digitalWrite(D7, 1);  // 2 - Change for upper nibble as required
  digitalWrite(WR, 0); digitalWrite(WR, 1);

  //write that square to green
  digitalWrite(CD, 0);
  digitalWrite(D0, 1); digitalWrite(D1, 1); digitalWrite(D2, 0); digitalWrite(D3, 0);  // C - Change for lower nibble as required
  digitalWrite(D4, 1); digitalWrite(D5, 0); digitalWrite(D6, 1); digitalWrite(D7, 1);  // 2 - Change for upper nibble as required
  digitalWrite(WR, 0); digitalWrite(WR, 1);
  digitalWrite(CD, 1);
  for (int i = 0; i < 320; i++)
    for (int j = 0; j < 240; j++) {
      digitalWrite(D0, 1); digitalWrite(D1, 1); digitalWrite(D2, 1); digitalWrite(D3, 1);  // 0 - Change for lower nibble as required
      digitalWrite(D4, 1); digitalWrite(D5, 1); digitalWrite(D6, 1); digitalWrite(D7, 1);  // 0 - Change for upper nibble as required
      digitalWrite(WR, 0); digitalWrite(WR, 1);
      digitalWrite(D0, 1); digitalWrite(D1, 1); digitalWrite(D2, 1); digitalWrite(D3, 1);  // 0 - Change for lower nibble as required
      digitalWrite(D4, 1); digitalWrite(D5, 1); digitalWrite(D6, 0); digitalWrite(D7, 1);  // 4 - Change for upper nibble as required
      digitalWrite(WR, 0); digitalWrite(WR, 1);
      digitalWrite(D0, 1); digitalWrite(D1, 1); digitalWrite(D2, 1); digitalWrite(D3, 1);  // 0 - Change for lower nibble as required
      digitalWrite(D4, 1); digitalWrite(D5, 1); digitalWrite(D6, 1); digitalWrite(D7, 0);  // 8 - Change for upper nibble as required
      digitalWrite(WR, 0); digitalWrite(WR, 1);
    }
  digitalWrite(CD, 0);
  digitalWrite(D0, 1); digitalWrite(D1, 1); digitalWrite(D2, 1); digitalWrite(D3, 1);  // 0 - Change for lower nibble as required
  digitalWrite(D4, 1); digitalWrite(D5, 1); digitalWrite(D6, 1); digitalWrite(D7, 1);  // 0 - Change for upper nibble as required
  digitalWrite(WR, 0); digitalWrite(WR, 1);
  digitalWrite(CD, 1);
  
}
//Nothing to do here except ur mom but already did her so...
void loop(void) {
}
