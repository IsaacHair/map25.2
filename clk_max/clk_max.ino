void setup() {
  pinMode(12, OUTPUT);
  int i;
  while (1) {
    PORTB = B11111111;
    delayMicroseconds(2);
    PORTB = B00000000;
    delayMicroseconds(2);
  }

}

void loop() {
  // put your main code here, to run repeatedly:

}
