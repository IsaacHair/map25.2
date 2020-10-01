void setup() {
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
  while (1) {
    /*note: this will end up being
    a few tens of ppm slower than 1Hz
    but who cares*/
    /*went to 10Hz*/
    digitalWrite(11, HIGH);
    delay(40);
    digitalWrite(11, LOW);
    delay(10);
    digitalWrite(12, HIGH);
    delay(40);
    digitalWrite(12, LOW);
    delay(10);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
