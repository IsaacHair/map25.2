#define ADR 22
#define DATA 40
#define WE 50
#define OE 51
#define CE 52

struct {
  int line[128];
  int data[128];
  int len;
  long adr;
} prog;

void setup() {
  int i, werr, rerr;
  Serial.begin(115200, SERIAL_8N1);
  pinMode(WE, OUTPUT);
  digitalWrite(WE, HIGH);
  pinMode(OE, OUTPUT);
  digitalWrite(OE, HIGH);
  pinMode(CE, OUTPUT);
  digitalWrite(CE, LOW);
  for (i = 0; i < 17; i++)
    pinMode(ADR+i, OUTPUT);
  while (1) {
    werr = 0;
    rerr = 0;
    confirm();
    grabreset();
    while (grabpage()) {
      pagewrite();
      if (i = errors()) {
        werr += i;
        pagewrite();
      }
    }
    grabreset();
    while (grabpage()) {
      if (i = errors()) {
        rerr += i;
      }
    }
    senderr(werr, rerr);
  }
}

void confirm() {
  while (Serial.available() == 0) ;
  Serial.read();
}

void grabreset() {
  Serial.print("r");
  confirm();
}

int grabpage() {
  int holder, i, j;
  char c;
  Serial.print("p");
  while (1) {
    for (holder = 0, i = 0; i < 4; i++) {
      while (!Serial.available()) ;
      if ((c = Serial.read()) >= '0' && c <= '9')
        holder = (holder << 4) + c-'0';
      else
        holder = (holder << 4) + c-'a'+10;
    }
    if (holder == 0xffff)
      return (0);
    prog.adr = holder;
    for (j = 0; j < 128; j++) {
      for (holder = 0, i = 0; i < 2; i++) {
        while (!Serial.available()) ;
        if ((c = Serial.read()) >= '0' && c <= '9')
          holder = (holder << 4) + c-'0';
        else
          holder = (holder << 4) + c-'a'+10;
      }
      if (holder == 0xff) {
        prog.len = j;
        return (1);
      }
      prog.line[j] = holder;
      for (holder = 0, i = 0; i < 2; i++) {
        while (!Serial.available()) ;
        if ((c = Serial.read()) >= '0' && c <= '9')
          holder = (holder << 4) + c-'0';
        else
          holder = (holder << 4) + c-'a'+10;
      }
      prog.data[j] = holder;
    }
  }
}

void senderr(int werr, int rerr)
{
  Serial.print("e");
  confirm();
  Serial.print(werr);
  confirm();
  Serial.print(rerr);
}

void pagewrite() {
  int j, k;
  int i;
  for (i = 0; i < 8; i++)
    pinMode(DATA+i, OUTPUT);
  digitalWrite(WE, HIGH);
  digitalWrite(OE, HIGH);
  digitalWrite(ADR+15, HIGH);
  digitalWrite(ADR+16, HIGH);
  setcomm(0x5555);
  setdata(0xAA);
  digitalWrite(WE, LOW);
  digitalWrite(WE, HIGH);
  setcomm(0x2AAA);
  setdata(0x55);
  digitalWrite(WE, LOW);
  digitalWrite(WE, HIGH);
  setcomm(0x5555);
  setdata(0xA0);
  digitalWrite(WE, LOW);
  digitalWrite(WE, HIGH);
  setpage(prog.adr);
  delay(5);
  for (j = 0; j < prog.len; j++) {
    digitalWrite(WE, HIGH);
    setline(prog.line[j]);
    setdata(prog.data[j]);
    digitalWrite(WE, LOW);
  }
  digitalWrite(WE, HIGH);
  delay(25);
}

void setdata(int data) {
  int k;
  for (k = 0; k < 8; k++)
    if (data & (1 << k))
      digitalWrite(DATA+k, HIGH);
    else
      digitalWrite(DATA+k, LOW);
}

void setpage(long page) {
  int k;
  for (k = 0; k < 17; k++)
    if (page & (1 << k))
      digitalWrite(ADR+k, HIGH);
    else
      digitalWrite(ADR+k, LOW);
}

void setcomm(int comm) {
  int k;
  for (k = 0; k < 15; k++)
    if (comm & (1 << k))
      digitalWrite(ADR+k, HIGH);
    else
      digitalWrite(ADR+k, LOW);
}

void setline(int line) {
  int k;
  for (k = 0; k < 7; k++)
    if (line & (1 << k))
      digitalWrite(ADR+k, HIGH);
    else
      digitalWrite(ADR+k, LOW);
}

int errors() {
  int j;
  int i;
  int err;
  for (i = 0; i < 8; i++)
    pinMode(DATA+i, INPUT);
  digitalWrite(WE, HIGH);
  digitalWrite(OE, LOW);
  setpage(0x0000);
  setpage(0xffff);
  setpage(prog.adr);
  for (j = 0, err = 0; j < prog.len; j++) {
    setline(prog.line[j]);
    if (readdata() != prog.data[j])
      err++;
  }
  return (err);
}

int readdata() {
  int k;
  int ret;
  for (k = 0, ret = 0; k < 8; k++)
    if (digitalRead(DATA+k))
      ret += (1 << k);
  return (ret);
}

void loop() {}
