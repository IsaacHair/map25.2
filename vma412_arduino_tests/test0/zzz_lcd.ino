//Technical support:support@openplatform.cc
// Breakout/Arduino UNO pin usage:
// LCD Data Bit :   7   6   5   4   3   2   1   0
// Uno dig. pin :   7   6   5   4   3   2   9   8
// Uno port/pin : PD7 PD6 PD5 PD4 PD3 PD2 PB1 PB0
// Mega dig. pin:  29  28  27  26  25  24  23  22
#define LCD_RD   A0
#define LCD_WR   A1     
#define LCD_RS   A2        
#define LCD_CS   A3       
#define LCD_REST A4

void Lcd_Writ_Bus(unsigned char d)
{
 PORTD = (PORTD & B00000011) | ((d) & B11111100); 
 PORTB = (PORTB & B11111100) | ((d) & B00000011); 
 *(portOutputRegister(digitalPinToPort(LCD_WR))) &=  ~digitalPinToBitMask(LCD_WR);
 *(portOutputRegister(digitalPinToPort(LCD_WR)))|=  digitalPinToBitMask(LCD_WR);
}


void Lcd_Write_Com(unsigned char VH)  
{   
  *(portOutputRegister(digitalPinToPort(LCD_RS))) &=  ~digitalPinToBitMask(LCD_RS);//LCD_RS=0;
  Lcd_Writ_Bus(VH);
}

void Lcd_Write_Data(unsigned char VH)
{
  *(portOutputRegister(digitalPinToPort(LCD_RS)))|=  digitalPinToBitMask(LCD_RS);//LCD_RS=1;
  Lcd_Writ_Bus(VH);
}
void Lcd_Write_Data_16(unsigned int dat)
{
  *(portOutputRegister(digitalPinToPort(LCD_RS)))|=  digitalPinToBitMask(LCD_RS);//LCD_RS=1;
  Lcd_Writ_Bus(dat>>8);
  Lcd_Writ_Bus(dat);
}

void Lcd_Write_Com_Data(unsigned char com,unsigned char dat)
{
  Lcd_Write_Com(com);
  Lcd_Write_Data(dat);
}
void LCD_WriteReg(unsigned char com,unsigned int dat)
{
  Lcd_Write_Com(com);
  Lcd_Write_Data_16(dat);
 }
void Address_set(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2)
{
  Lcd_Write_Com(0x50);
	Lcd_Write_Data_16(x1);
  
  Lcd_Write_Com(0x51);
	Lcd_Write_Data_16(x2);
        
  Lcd_Write_Com(0x52);
	Lcd_Write_Data_16(y1);
  
	Lcd_Write_Com(0x53);
	Lcd_Write_Data_16(y2);

   Lcd_Write_Com(0x20);
  Lcd_Write_Data_16(x1);
  
  Lcd_Write_Com(0x21);
  Lcd_Write_Data_16(y1);
	Lcd_Write_Com(0x22); 							 
}

void Lcd_Init(void)
{
  digitalWrite(LCD_REST,HIGH);
  delay(50); 
  digitalWrite(LCD_REST,LOW);
  delay(150);
  digitalWrite(LCD_REST,HIGH);
  delay(150);

  digitalWrite(LCD_CS,HIGH);
  digitalWrite(LCD_WR,HIGH);
  digitalWrite(LCD_CS,LOW);  //CS

  LCD_WriteReg(0x0000,0x0001);
  delay(100); //at least 100ms
  LCD_WriteReg( 0x0001,0x0100); // set SS and SM bit
  LCD_WriteReg(0x0010,0x1790);
  LCD_WriteReg(0x0060,0xA700);
  LCD_WriteReg(0x0061,0x0001);
  LCD_WriteReg( 0x0046,0x0002);
  LCD_WriteReg(0x0013,0x8010);
  LCD_WriteReg(0x0012,0x80fe);
  LCD_WriteReg(0x0002,0x0500);
  LCD_WriteReg(0x0003,0x1030);
  
  LCD_WriteReg(0x0030,0x0303);
  LCD_WriteReg(0x0031,0x0303);
  LCD_WriteReg(0x0032,0x0303);
  LCD_WriteReg(0x0033,0x0300);
  LCD_WriteReg(0x0034,0x0003);
  LCD_WriteReg(0x0035,0x0303);
  LCD_WriteReg(0x0036,0x0014);
  LCD_WriteReg(0x0037,0x0303);
  LCD_WriteReg(0x0038,0x0303);
  LCD_WriteReg(0x0039,0x0303);
  LCD_WriteReg(0x003a,0x0300);
  LCD_WriteReg(0x003b,0x0003);
  LCD_WriteReg(0x003c,0x0303);
  LCD_WriteReg(0x003d,0x1400);
  
  LCD_WriteReg(0x0092,0x0200);
  LCD_WriteReg(0x0093,0x0303);
  LCD_WriteReg(0x0090,0x080d);
  
  
  LCD_WriteReg(0x0001, 0x0100); // set SS and SM bit 0100
  LCD_WriteReg( 0x0002, 0x0700); // set 1 line inversion
  LCD_WriteReg(0x0003, 0x1030); // set GRAM write direction and BGR=1.
  LCD_WriteReg(0x0004, 0x0000); // Resize register
  LCD_WriteReg(0x0008, 0x0302); // set the back porch and front porch
  LCD_WriteReg(0x0009, 0x0000); // set non-display area refresh cycle ISC[3:0]
  LCD_WriteReg(0x000A, 0x0000);// FMARK function
  LCD_WriteReg(0x000C, 0x0000); // RGB interface setting
  LCD_WriteReg(0x000D, 0x0000); // Frame marker Position
  LCD_WriteReg(0x000F, 0x0000); // RGB interface polarity
  delay(120);
  
  LCD_WriteReg(0x0030,0x0303);
  LCD_WriteReg(0x0031,0x0303);
  LCD_WriteReg(0x0032,0x0303);
  LCD_WriteReg( 0x0033,0x0300);
  LCD_WriteReg(0x0034,0x0003);
  LCD_WriteReg(0x0035,0x0303);
  LCD_WriteReg(0x0036,0x0014);
  LCD_WriteReg(0x0037,0x0303);
  LCD_WriteReg(0x0038,0x0303);
  LCD_WriteReg(0x0039,0x0303);
  LCD_WriteReg(0x003a,0x0300);
  LCD_WriteReg(0x003b,0x0003);
  LCD_WriteReg(0x003c,0x0303);
  LCD_WriteReg(0x003d,0x1400);
  
  LCD_WriteReg(0x0060, 0xA700); // Gate Scan 
  LCD_WriteReg(0x0061, 0x0001); // NDL,VLE, REV
  LCD_WriteReg( 0x006A, 0x0000); // set scrolling line
  LCD_WriteReg(0x0090, 0x0033);
  LCD_WriteReg(0x0095, 0x0110);
  
  LCD_WriteReg(0x00FF, 0x0001);
  LCD_WriteReg(0x00FF, 0x000C);
  LCD_WriteReg(0x00FF, 0x0000);
  delay(100);
  LCD_WriteReg(0x0003,0x1030); // set GRAM write direction and BGR=1.
  LCD_WriteReg(0x0007,0x0173);
  
  delay(50);         
  
}

void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c)                   
{	
  unsigned int i,j;
  Lcd_Write_Com(0x02c); //write_memory_start
  digitalWrite(LCD_RS,HIGH);
  digitalWrite(LCD_CS,LOW);
  l=l+x;
  Address_set(x,y,l,y);
  j=l*2;
  for(i=1;i<=j;i++)
  {
    Lcd_Write_Data(c);
  }
  digitalWrite(LCD_CS,HIGH);   
}

void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c)                   
{	
  unsigned int i,j;
  Lcd_Write_Com(0x02c); //write_memory_start
  digitalWrite(LCD_RS,HIGH);
  digitalWrite(LCD_CS,LOW);
  l=l+y;
  Address_set(x,y,x,l);
  j=l*2;
  for(i=1;i<=j;i++)
  { 
    Lcd_Write_Data(c);
  }
  digitalWrite(LCD_CS,HIGH);   
}

void Rect(unsigned int x,unsigned int y,unsigned int w,unsigned int h,unsigned int c)
{
  H_line(x  , y  , w, c);
  H_line(x  , y+h, w, c);
  V_line(x  , y  , h, c);
  V_line(x+w, y  , h, c);
}

void Rectf(unsigned int x,unsigned int y,unsigned int w,unsigned int h,unsigned int c)
{
  unsigned int i;
  for(i=0;i<h;i++)
  {
    H_line(x  , y  , w, c);
    H_line(x  , y+i, w, c);
  }
}
int RGB(int r,int g,int b)
{return r << 16 | g << 8 | b;
}
void LCD_Clear(unsigned int j)                   
{	
  unsigned int i,m;
 Address_set(0,0,240-1,320-1);
  //Lcd_Write_Com(0x02c); //write_memory_start //
  //digitalWrite(LCD_RS,HIGH); //
  digitalWrite(LCD_CS,LOW);

  for(i=0;i<320;i++)
    for(m=0;m<240;m++)
    {
      Lcd_Write_Data(j>>8);
      Lcd_Write_Data(j);

    }
  digitalWrite(LCD_CS,HIGH);   
}

void setup()
{
  Serial.begin(9600);
  for(int p=2;p<10;p++)
  {
    pinMode(p,OUTPUT);
  }
  pinMode(A0,OUTPUT);
  pinMode(A1,OUTPUT);
  pinMode(A2,OUTPUT);
  pinMode(A3,OUTPUT);
  pinMode(A4,OUTPUT);
  digitalWrite(A0, HIGH);
  digitalWrite(A1, HIGH);
  digitalWrite(A2, HIGH);
  digitalWrite(A3, HIGH);
  digitalWrite(A4, HIGH);
  Lcd_Init();
 LCD_Clear(0xf800);
}

void loop()
{  
   LCD_Clear(0xffff);
   LCD_Clear(0x0000);
   LCD_Clear(0xf800);
   LCD_Clear(0x07E0);
   LCD_Clear(0x001F);
     
  for(int i=0;i<1000;i++)
  {
    Rect(random(300),random(300),random(300),random(300),random(65535)); // rectangle at x, y, with, hight, color
    delay(1000);
  }
  
//  LCD_Clear(0xf800);
}
