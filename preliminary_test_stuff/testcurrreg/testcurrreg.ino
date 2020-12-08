void setup() {
  /*d8-d11 are lowest curr bits; d8 is curr eights bit and d11 is curr ones bit
   * d12 is clk
   */

  int i, j;
  for (i = 8; i <= 12; i++)
    pinMode(i, OUTPUT);
  for (i = 0; 1; (i < 6 ? i++ : (i < 8 ? i = 8 : i = 0))) {
    digitalWrite(8, HIGH);
    digitalWrite(9, HIGH);
    digitalWrite(10, HIGH);
    digitalWrite(11, HIGH);
    if (!(i&1))
      digitalWrite(11, LOW);
    if (!(i&2))
      digitalWrite(10, LOW);
    if (!(i&4))
      digitalWrite(9, LOW);
    if (!(i&8))
      digitalWrite(8, LOW);
    delay(10);
    digitalWrite(12, HIGH);
    delay(10);
    digitalWrite(12, LOW);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
