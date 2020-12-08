void setup() {
  pinMode(50, OUTPUT);
  digitalWrite(50, HIGH);
  pinMode(51, OUTPUT);
  digitalWrite(51, LOW);
  pinMode(52, OUTPUT);
  digitalWrite(52, LOW);

  /*for (int i = 24; i < 39; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  for (int i = 22; i < 24; i++) {
    pinMode(i, INPUT);
  }*/
  for (int i = 22; i < 29; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  for (int i = 31; i < 39; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  for (int i = 29; i < 31; i++) {
    pinMode(i, INPUT);
  }
  
  for (int i = 40; i < 48; i++) {
    pinMode(i, INPUT);
  }
}

void loop() {}
