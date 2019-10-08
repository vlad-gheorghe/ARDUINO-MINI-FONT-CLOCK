
/*
1	3,3V
2 	Vcc 5v
3	Vcc 5v
3	nc
4 	nc 
5	nc
6 	GND
7 	A3
8 	nc
9 	GND
10	nc
11	A1
12	A2
13	nc
14	GND
15	D8
16 	D9
17	Vcc 5v
18 	D2
19 	D3
20 	GND
21 	D5
22 	D4
23 	D6
24 	D7
25 	GND
26 	A0

*/




#include <SPI.h>
#include <Wire.h>
#include "RTClib.h"
//#include</Fonts/FreeMonoBoldOblique24pt7b.h>
#define tft_CS A3 // Chip Select goes to Analog 3
#define tft_CD A2 // Command/Data goes to Analog 2
#define tft_WR A1 // tft Write goes to Analog 1
#define tft_RD A6 // tft Read goes to Analog 0
#define tft_RESET A0 // Can alternately just connect to Arduino's reset pin

#include "Adafruit_GFX.h"    // Adafruit Grafik-Bibliothek
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <DHT.h>
#define DHTPIN 13         // Zur Messung verwendeter Pin, in unserem Fall also Pin 4
#define DHTTYPE DHT11    // DHT 11
DHT dht(DHTPIN, DHTTYPE);  // Initialisieren des DHTs
RTC_Millis RTC;
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0  
#define WHITE   0xFFFF
#define ORANGE          0xFD20
#define GREENYELLOW     0xAFE5
#define DARKGREEN       0x03E0

byte SW0 = 10;
byte SW1 = 11;
byte SW2 = 12;
int zh, uh, ore;
int zm, um, miniti;
int an, zi,minu,secu;
uint8_t luna = 0;
char daysOfTheWeek[7][12] = {"Duminica", "Luni", "Marti", "Miercuri", "Joi", "Vineri", "Simbata"};
uint8_t bbb = 0, sss=0, mmm=0, hhh=0;
 float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0;    // Saved H, M, S x & y multipliers
float sdeg = 0, mdeg = 0, hdeg = 0;
uint16_t osx = 120, osy = 120, omx = 120, omy = 120, ohx = 120, ohy = 120; // Saved H, M, S x & y coords
int16_t x0 = 0, x1 = 0, yy0 = 0, yy1 = 0;
uint32_t targetTime = 0;                    // for next 1 second timeout
boolean initial = 1;
uint16_t xpos; // x position for clock face
int h, t1, tv;
float te;
byte refresh, citire;
uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}



void setup(void) {
  Wire.begin(); 
 dht.begin();  // DHT startn
  RTC.begin(DateTime(F(__DATE__), F(__TIME__)));
//Wire.beginTransmission(0x68);
//Wire.write(0x07); // move pointer to SQW address
//Wire.write(0x10); // sends 0x10 (hex) 00010000 (binary) to control register - turns on square wave
//Wire.endTransmission();

 
  uint16_t g_identifier;
    g_identifier = tft.readID(); //
     if (g_identifier == 0x00D3 || g_identifier == 0xD3D3) g_identifier = 0x9481; // write-only shield
    if (g_identifier == 0xFFFF) g_identifier = 0x9341; // serial
    tft.begin(g_identifier);
    tft.setRotation(1);
   tft.fillScreen(BLACK);
   tft.fillScreen(RED);
   tft.fillScreen(YELLOW);
   tft.fillScreen(BLUE);
  delay(500);
    tft.fillScreen(BLACK);

    pinMode(SW0, INPUT);  // for this use a slide switch
  pinMode(SW1, INPUT);  // N.O. push button switch
  pinMode(SW2, INPUT);  // N.O. push button switch
  digitalWrite(SW0, HIGH); // pull-ups on
  digitalWrite(SW1, HIGH);
  digitalWrite(SW2, HIGH);
  tft.setTextSize(1);
  tft.setTextColor(WHITE, BLACK);
}
void loop () {
   DateTime now = RTC.now();
  // String stringOne = RTC.now();
  int h = dht.readHumidity();
  int t = dht.readTemperature();
luna=(now.month());
an=(now.year());
zi=(now.day());
minu=(now.minute());
secu=(now.second());
ore=(now.hour());


 tft.fillRect (0,40,480,10,BLUE);
tft.setTextColor (WHITE,BLUE);
tft.setTextSize (1);
tft.setCursor (sss*4.3,40);
tft.print (F("  Monitorizare timp si clima")); 
  tft.fillRect (0,0,480,10,CYAN);
tft.setTextColor (BLACK,CYAN);
tft.setTextSize (1);
tft.setCursor (sss*3.3,1);
tft.print (F("Adaptat de Vlad Gheorghe 2016")); 

//week=(now.dayOfWeek()); (daysOfTheWeek[now.dayOfTheWeek()]);
//tft.print(daysOfTheWeek[now.dayOfTheWeek()]);
//String stringOne =RTC.now();
  tft.setTextColor(ORANGE, BLACK);
//tft.setFont(FreeMonoBoldOblique24pt7b);
   tft.setTextSize(12);
   
   tft.setCursor(20,70);
   if ( now.hour() < 10)
   {
     tft.print(" "); 
     tft.print(now.hour(), DEC);
   }
   else
   {
   tft.print(now.hour(), DEC);
   }
   tft.print(":");
   if ( now.minute() < 10)
   {
     tft.print("0"); 
     tft.print(now.minute(), DEC);
   }
   else
   {
   tft.print(now.minute(), DEC);
   }
   tft.setTextSize(5);
  tft.setCursor(380, 90); tft.print(":");
   if ( now.second() < 10)
   {
     tft.print("0"); 
     tft.print(now.second(), DEC);
   }
   else
   {
   tft.print(now.second(), DEC);
   }
     //tft.print(""); 
  
     tft.setTextColor(MAGENTA, BLACK);
   tft.setTextSize(2);
   tft.setCursor(100, 17);
   //tft.print(dayOfWeek(), const);
   tft.print(daysOfTheWeek[now.dayOfTheWeek()]);
    tft.print(" ");
  // tft.print(week);
    if ( now.day() < 10)
   {
     tft.print("0"); 
     tft.print(now.day(), DEC);
   }
   else
   {
   tft.print(now.day(), DEC);
   }
   tft.print(" ");
   if ( now.month() < 10)
   {
    // tft.print("0"); 
    // tft.print(now.month(), DEC);
    printMonth(luna);
   }
   else
   {
   //tft.print(now.month(), DEC);
   printMonth(luna);
   }
   tft.print(" ");
   tft.print(now.year(), DEC);
   tft.print(" "); 
   
/*
int a = t;
   for (int x = 0; x < a; x++)
   { 
   tft.drawLine(200+2*x, 220, 200+2*x, 220-x, BLACK);
}
 for (int x = 0; x < a; x++)
   { 
   tft.drawLine(200+2*x, 220, 200+2*x, 220-x, WHITE);
}
*/

  
  analog();
  clima();
 
   if (!(digitalRead(SW0))) set_time(); // hold the switch to set time

 
   delay(500);
}

void set_time()   {
  byte minutes1 = 0;
  byte hours1 = 0;
  byte minutes = 0;
  byte hours = 0;

  while (!digitalRead(SW0))  // set time switch must be released to exit
  {
    minutes1=minutes;
    hours1=hours;
    
     
    while (!digitalRead(SW1)) // set minutes
    { 
     minutes++;  
   // converting hexa in zecimal:
    zh = hours / 16;
    uh = hours - 16 * zh ;
    ore = 10 * zh + uh; 
    zm = minutes / 16;
    um = minutes - 16 * zm ;
    miniti = 10 * zm + um; 
  
  
   tft.setCursor(20, 70);
   tft.setTextColor(YELLOW, BLACK);
   tft.setTextSize(12);
   if ( ore < 10)
   {
     tft.print(" "); 
     tft.print(ore);
   }
   else
   {
   tft.print(ore);
   }
   tft.print(":");
   if ( miniti < 10)
   {
     tft.print("0"); 
     tft.print(miniti);
   }
   else
   {
   tft.print(miniti);
   }
    tft.setTextSize(5);tft.setCursor(380, 90);
   tft.print(":");
   tft.print("00"); 
      
      if ((minutes & 0x0f) > 9) minutes = minutes + 6;
      if (minutes > 0x59) minutes = 0;
      Serial.print("Minutes = ");
      if (minutes >= 9) Serial.print("0");
      Serial.println(minutes, HEX);
    delay(150);    
    }

    while (!digitalRead(SW2)) // set hours
    { 
     hours++;          
     
   // converting hexa in zecimal:
   zh = hours / 16;
    uh = hours - 16 * zh ;
    ore = 10 * zh + uh; 
    zm = minutes / 16;
    um = minutes - 16 * zm ;
    miniti = 10 * zm + um; 
    
  
   tft.setCursor(20, 70);
   tft.setTextColor(YELLOW, BLACK);
   tft.setTextSize(12);
   if ( ore < 10)
   {
     tft.print(" "); 
     tft.print(ore);
   }
   else
   {
   tft.print(ore);
   }
   tft.print(":");
   if ( miniti < 10)
   {
     tft.print("0"); 
     tft.print(miniti);
   }
   else
   {
   tft.print(miniti);
   }
    tft.setTextSize(5);tft.setCursor(380, 90);
   tft.print(":");
   tft.print("00");
      
      if ((hours & 0x0f) > 9) hours =  hours + 6;
      if (hours > 0x23) hours = 0;
      Serial.print("Hours = ");
      if (hours <= 9) Serial.print("0");
      Serial.println(hours, HEX);
    delay(150);
    }

  
  
    // converting hexa in zecimal:
    zh = hours / 16;
    uh = hours - 16 * zh ;
    ore = 10 * zh + uh; 
    zm = minutes / 16;
    um = minutes - 16 * zm ;
    miniti = 10 * zm + um; 
    
   /*  for(int i = 20 ; i >0  ; i--) {
     displayNumber01(ore*100+miniti); 
     }
 //  delay(150);
    */

   tft.setCursor(20, 70);
   tft.setTextColor(YELLOW, BLACK);
   tft.setTextSize(12);
   if ( ore < 10)
   {
     tft.print(" "); 
     tft.print(ore);
   }
   else
   {
   tft.print(ore);
   }
   tft.print(":");
   if ( miniti < 10)
   {
     tft.print("0"); 
     tft.print(miniti);
   }
   else
   {
   tft.print(miniti);
   }
   tft.setTextSize(5);tft.setCursor(380, 90);
   tft.print(":");
   tft.print("00");
/*
  Wire.beginTransmission(0x68); // activate DS1307
    Wire.write(0); // where to begin
    Wire.write(0x00);          //seconds
    Wire.write(minutes);          //minutes
    Wire.write(0x80 | hours);    //hours (24hr time)
    Wire.write(0x06);  // Day 01-07
    Wire.write(0x01);  // Date 0-31
    Wire.write(0x05);  // month 0-12
    Wire.write(0x09);  // Year 00-99
    Wire.write(0x10); // Control 0x10 produces a 1 HZ square wave on pin 7. 
    Wire.endTransmission();
*/
  //  RTC.adjust(DateTime(2016, 9, 19, (hours, HEX), (minutes, HEX), 0));
    RTC.adjust(DateTime(an, luna, zi, hours, minutes, 0));
   
}}

void printMonth(int luna)
{
  switch(luna)
  {
    case 1: tft.print("Ianuarie");break;
    case 2: tft.print("Februarie");break;
    case 3: tft.print("Martie");break;
    case 4: tft.print("Aprilie");break;
    case 5: tft.print("Mai");break;
    case 6: tft.print("Iunie.");break;
    case 7: tft.print("Iulie");break;
    case 8: tft.print("August");break;
    case 9: tft.print("Septembrie");break;
    case 10: tft.print("Octombrie");break;
    case 11: tft.print("Noiembrie");break;
    case 12: tft.print("Decembrie");break;
    default: tft.print("***");break;
  } 
}
void analog(){
hhh=ore;
sss=secu;
xpos = 400; // Work out the x coord for the centre of the screen
  tft.drawCircle(xpos, 240, 76, GREEN);
 // tft.fillCircle(xpos, 240, 50, BLACK);
 // Draw 12 lines
  for (int i = 0; i < 360; i += 30) {
    sx = cos((i - 90) * 0.0174532925);
    sy = sin((i - 90) * 0.0174532925);
    x0 = sx * 78 + xpos;
    yy0 = sy * 78 + 240;
    x1 = sx * 74 + xpos;
    yy1 = sy * 74 + 240;

    tft.drawLine(x0, yy0, x1, yy1, GREEN);
  }

 // Draw 60 dots
  for (int i = 0; i < 360; i += 6) {
    sx = cos((i - 54) * 0.0174532925);
    sy = sin((i - 54) * 0.0174532925);
    x0 = sx * 72 + xpos;
    yy0 = sy * 72 + 240;
    // Draw minute markers
    tft.drawPixel(x0, yy0, WHITE);

    // Draw main quadrant dots
   // if (i == 0 || i == 180) tft.fillCircle(x0, yy0, 2, TFT_WHITE);
   // if (i == 90 || i == 270) tft.fillCircle(x0, yy0, 2, TFT_WHITE);
  }
tft.fillCircle(xpos, 241, 3, WHITE);
mmm=minu;
 sdeg = sss * 6;                     // 0-59 -> 0-354
    mdeg = mmm * 6 + sdeg * 0.01666667; // 0-59 -> 0-360 - includes seconds, but these increments are not used
    hdeg = hhh * 30 + mdeg * 0.0833333; // 0-11 -> 0-360 - includes minutes and seconds, but these increments are not used
    hx = cos((hdeg - 90) * 0.0174532925);
    hy = sin((hdeg - 90) * 0.0174532925);
    mx = cos((mdeg - 90) * 0.0174532925);
    my = sin((mdeg - 90) * 0.0174532925);
    sx = cos((sdeg - 90) * 0.0174532925);
    sy = sin((sdeg - 90) * 0.0174532925);
/*
if (targetTime < millis()) {
    targetTime = millis() + 1000;
    ss++;              // Advance second
    if (ss == 60) {
      ss = 0;
      mm++;            // Advance minute
      if (mm > 59) {
        mm = 0;
        hh++;          // Advance hour
        if (hh > 12) {
          //hh = 0;
          hh=hh-12;
        }



        
        
      }
   }
}
*/
if (sss == 0 || initial) {
      initial = 0;
      // Erase hour and minute hand positions every minute
      tft.drawLine(ohx, ohy, xpos, 241, BLACK);
      ohx = hx * 36 + xpos + 1;
      ohy = hy * 36 + 241;
      tft.drawLine(omx, omy, xpos, 241, BLACK);
      omx = mx * 58 + xpos;
      omy = my * 58 + 241;
    }
    // Redraw new hand positions, hour and minute hands not erased here to avoid flicker
    tft.drawLine(osx, osy, xpos, 241, BLACK);
    osx = sx * 70 + xpos + 1;
    osy = sy * 70 + 241;
    tft.drawLine(osx, osy, xpos, 241, RED);
    tft.drawLine(ohx, ohy, xpos, 241, WHITE);
    tft.drawLine(omx, omy, xpos, 241, WHITE);
    tft.drawLine(osx, osy, xpos, 241, RED);

    tft.fillCircle(xpos, 241, 3, RED);
    tft.fillCircle(326, 241, 2, WHITE);
     tft.fillCircle(474, 241, 2, WHITE);
     tft.fillCircle(xpos, 315, 2, WHITE);
     tft.fillCircle(xpos, 167, 2, WHITE);
     tft.setTextSize (1); tft.setCursor(xpos-5, 172);tft.setTextColor (WHITE);tft.print("12");
     tft.setTextSize (1); tft.setCursor(xpos-2, 303);tft.setTextColor (WHITE);tft.print("6");
     tft.setTextSize (1); tft.setCursor(xpos-68, 239);tft.setTextColor (WHITE);tft.print("9");
     tft.setTextSize (1); tft.setCursor(xpos+63, 239);tft.setTextColor (WHITE);tft.print("3");
     tft.setTextSize (1); tft.setCursor(xpos+56, 200);tft.setTextColor (WHITE);tft.print("2");
     tft.setTextSize (1); tft.setCursor(xpos-60, 200);tft.setTextColor (WHITE);tft.print("10");
     tft.setTextSize (1); tft.setCursor(xpos+55, 273);tft.setTextColor (WHITE);tft.print("4");
     tft.setTextSize (1); tft.setCursor(xpos-60, 273);tft.setTextColor (WHITE);tft.print("8");
tft.setTextSize (1); tft.setCursor(xpos+37, 295);tft.setTextColor (WHITE);tft.print("5");
     tft.setTextSize (1); tft.setCursor(xpos-33, 295);tft.setTextColor (WHITE);tft.print("7");
     tft.setTextSize (1); tft.setCursor(xpos+30, 175);tft.setTextColor (WHITE);tft.print("1");
      tft.setTextSize (1); tft.setCursor(xpos-36, 175);tft.setTextColor (WHITE);tft.print("11");
     
     

    




}
void clima(){

  char tempF[6];
int t;
//unsigned long now = millis();
//struct ts t; 
 t = dht.readTemperature();
int t1 = 10*te;
h = dht.readHumidity();

citire = 0;
if ((sss == 0) && (citire == 0)) 
{
  citire = 1;
t = dht.readTemperature();
int t1 = 10*te;
h = dht.readHumidity();
//delay(50);
//Serial.println(t.sec);
}
if ((sss == 15) && (citire == 0))
{
  citire = 1;
t = dht.readTemperature();
int t1 = 10*te;
h = dht.readHumidity();
//delay(50);
//Serial.println(t.sec);
}
if ((sss == 30) && (citire == 0)) 
{
  citire = 1;
t = dht.readTemperature();
int t1 = 10*te;
h = dht.readHumidity();
//delay(50);
//Serial.println(t.sec);
}
if ((sss == 45) && (citire == 0)) 
{
  citire = 1;
t = dht.readTemperature();
int t1 = 10*te;
h = dht.readHumidity();
//delay(50);
//Serial.println(t.sec);
}
if ((sss == 1) || (sss == 16)) citire = 0;
if ((sss == 31) || (sss == 46)) citire = 0;

  te = t;
  tv = 10*t - 10*t1; 

   // need for display time 
   int zs = sss/10;
int us = sss - zs*10;

tft.setTextColor(GREEN, BLACK);
   tft.setTextSize(5);
   tft.setCursor(30, 280);
  // tft.print("H=");
  // tft.print(h);
    if ( h < 10)
   {
     tft.print(" "); 
    
 // tft.setTextColor(GREEN, BLACK);
  // tft.setTextSize(4);
 //  tft.setCursor(30, 280);
     tft.print(h);
   }
   else
   {
   tft.print(h);
   }
   tft.setCursor(90, 280);tft.print("%");
    tft.setTextColor(GREEN);
     tft.setCursor(50,260); tft.setTextSize(1);
  tft.print("Umiditate:");
  // tft.print(an); tft.print(luna); tft.print(zi);
  // tft.printNumF(2);
tft.setTextColor(RED, BLACK);
  tft.setCursor(50,190); tft.setTextSize(1);
  tft.print("Temperatura:");
   tft.setTextSize(5);
  tft.setCursor(35, 210);
  // tft.print("t=");
    if ( t < 10)
   {
     tft.print(" "); 
     tft.print(t);
   }
   else
   {
   tft.print(t);
   }
   //tft.print(",0");
  tft.setTextSize(2);
   tft.write(247); tft.setTextSize(5);
   tft.print("C");
  
}


