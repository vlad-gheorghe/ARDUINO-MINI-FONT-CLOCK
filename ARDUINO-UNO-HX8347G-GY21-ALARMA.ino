// original sketch from http://educ8s.tv/arduino-real-time-clock/
// Nicu FLORICA (niq_ro) from http://www.arduinotehniq.com 
// made some changes for animate display with blinking signs
// eliminate library for clock, use just subroutines
// use 3 buttons for manual adjust the data & clock
// add alarm (now usee eeprom memory for store hour alarm)
// adapted by Vlad Gheorghe for Arduino Mega and 3.95 display with ILI9844
// adapted by Nicu FLORICA for Due and same display

// icon made by Cristi Vasilache
static const PROGMEM uint8_t clopotel[32 * 24 / 8] = {
0X00,0X03,0XC0,0X00,0X00,0X07,0XE0,0X00,0X00,0X0F,0XF0,0X00,0X00,0X0F,0XF0,0X00,
0X00,0X0F,0XF0,0X00,0X20,0X38,0X1C,0X04,0X24,0X41,0X02,0X24,0X44,0X81,0X01,0X22,
0X48,0X81,0X01,0X12,0X49,0X01,0X00,0X92,0X49,0X00,0X80,0X92,0X89,0X00,0X80,0X91,
0X91,0X00,0X80,0X49,0X92,0X1F,0X80,0X49,0X91,0X00,0X00,0X89,0X49,0X00,0X00,0X92,
0X49,0X00,0X00,0X92,0X48,0X80,0X01,0X12,0X48,0X80,0X01,0X12,0X24,0X40,0X02,0X24,
0X20,0X30,0X0C,0X04,0X00,0X7E,0X3E,0X00,0X00,0X79,0XCE,0X00,0X00,0X70,0X0E,0X00};
// alarmOff
static const PROGMEM uint8_t faraclopotel[32 * 24 / 8] = {
0X00,0X03,0XC0,0X00,0X00,0X07,0XE0,0X00,0X80,0X0F,0XF0,0X01,0X60,0X0F,0XF0,0X06,
0X10,0X0F,0XF0,0X08,0X0C,0X38,0X1C,0X30,0X02,0X40,0X82,0X40,0X01,0XC0,0X81,0X80,
0X00,0XC0,0X83,0X00,0X01,0X30,0X8C,0X80,0X01,0X08,0X90,0X80,0X01,0X06,0XE0,0X80,
0X01,0X1F,0X80,0X40,0X01,0X03,0XC0,0X40,0X01,0X04,0X20,0X80,0X01,0X18,0X18,0X80,
0X01,0X20,0X04,0X80,0X00,0XC0,0X03,0X00,0X01,0X40,0X01,0X80,0X06,0X40,0X02,0X60,
0X08,0X30,0X0C,0X10,0X30,0X7E,0X7E,0X0C,0X40,0X79,0X8E,0X02,0X80,0X70,0X0E,0X01,
};


#include <SHT21.h> //https://github.com/markbeee/SHT21
SHT21 sht; 


#include "EEPROM.h"

#include <Adafruit_GFX.h>    // https://github.com/adafruit/Adafruit-GFX-Library
#include <Adafruit_TFTLCD.h> // https://github.com/adafruit/TFTLCD-Library
#include <Wire.h>


#define LCD_CS 0 // Chip Select goes to Analog 3 //connect to GND
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET 0 // Can alternately just connect to Arduino's reset pin

// Assign human-readable names to some common 16-bit color values:
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);


#include <Fonts/FreeSerif9pt7b.h>  //FreeSerifBold12pt7b

#include <Fonts/FreeSerifBold12pt7b.h> 

//float maxTemperature=0;
//float minTemperature=200;
//char charMinTemperature[10];
//char charMaxTemperature[10];
char timeChar[100];
char dateChar[50];
char temperatureChar[10];
char humidityChar[10];

float temperature = 0;
float previousTemperature = 0;
float humidity = 0;
float previousHumidity = 0;

String dateString;
int minuteNow=0;
int minutePrevious=0;


#define DS3231_I2C_ADDRESS 104
byte tMSB, tLSB;
float temp3231;
float temperatura, temperatura0;
byte hh, mm, ss;
byte yy, ll, dd, zz;
//float humidity; // variable to store hemidity


#define meniu A3 
#define minus 11
#define plus  12 
#define alarm 13 
#define buzzer 10

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

int nivel = 0;   // if is 0 - clock
                 // if is 1 - hour adjust
                 // if is 2 - minute adjust

int hh1, mm1, zz1, dd1, ll1, yy1;   
int maxday;
boolean initial = 1;
char chartemp[3];
int hha, mma;   // for alarm
byte al;     // for alarm 
byte xa = 320;
byte ya = 240;

#define disk1 0x57    //Address of 24LC256 eeprom chip


void setup () 
{
  // Serial.begin(9600);
   
    //tft.begin();
    tft.begin(0x7575); // SDFP5408
    tft.setRotation(1);
    tft.fillScreen(BLACK);

   tft.setRotation(1);
    tft.fillScreen(BLACK);
   
    Wire.begin();

  temperature = sht.getTemperature();  // get temp from SHT 
  humidity = sht.getHumidity(); // get temp from SHT
    
pinMode(meniu, INPUT); 
pinMode(plus, INPUT); 
pinMode(minus, INPUT); 
pinMode(alarm, INPUT); 
pinMode(buzzer, OUTPUT);
digitalWrite(meniu, HIGH);  // put inputs in high state (when push is to ground -> low state)
digitalWrite(plus, HIGH);
digitalWrite(minus, HIGH);
digitalWrite(alarm, HIGH);
digitalWrite(buzzer, LOW);

 

    printText("Temperatura", GREEN,0,150,1);  // Temperature Static Text
    printText("Alarma setata", MAGENTA,223,150,1);
    printText("Umiditate", GREEN,5,195,1);
   
     printText("@2023 by Vlad Gheorghe", GREENYELLOW,180,230,1); //119
   // printText("MAX", BLUE,10,190,1);
   // printText("MIN", RED,52,190,1);

//hha = readEEPROM(disk1,100), DEC;
//mma = readEEPROM(disk1,101), DEC;
//al = readEEPROM(disk1,102), DEC;

hha = EEPROM.read(100);
mma = EEPROM.read(101);
al = EEPROM.read(102);

if ((hha < 0) || (hha > 23)) hha = 7;
if ((mma < 0) || (mma > 59)) mma = 0;
if ((al < 0) || (al > 1)) al = 0;
}


void loop () 
{

if (nivel == 0)
{

if (digitalRead(meniu) == LOW)

  {
  //nivel = nivel+1;
  nivel =99;
  hh1=hh;
  mm1=mm;
  zz1=zz;
  yy1=yy;
  ll1=ll;
  dd1=dd;
  delay(500);
  tft.fillScreen(BLACK);
  tft.fillRect(0,0,128,160,BLACK);
  }
 
  readDS3231time(&ss, &mm, &hh, &zz, &dd, &ll,&yy);

if ((ss == 15) || (ss == 45 || initial))
{

 temperature = sht.getTemperature();
 humidity = sht.getHumidity();
 delay(300);

temperature = sht.getTemperature();
humidity = sht.getHumidity();

}


    String halarma = "";
    if(hha<10)
    {
         halarma = halarma+" "+String(hha);
    }else  
    halarma = halarma+ String(hha);
    if(mma<10)
    {
        halarma = halarma+":0"+String(mma);
    }else
    {
        halarma = halarma+":"+String(mma);
    }
    halarma.toCharArray(timeChar,200);

if (digitalRead(alarm) == LOW)

  {
  al = al + 1;

 // writeEEPROM(disk1, 102, al%2);  
  EEPROM.write(102, al%2);
  delay(500);
  tft.drawBitmap(140, 169, faraclopotel, 32, 24, BLACK);  
  tft.drawBitmap(140, 169, clopotel, 32, 24, BLACK);  
  }
if (mma == mm & hha == hh & al%2)
{
//digitalWrite(buzzer, HIGH);
tone(buzzer, 1000, 50);
  delay(200);
  noTone(buzzer);
delay(200);
tft.drawBitmap(140, 169, clopotel, 32, 24, GREEN);
//tft.setFont(&FreeSerif9pt7b);
   printText(timeChar, GREEN,195,170,4); //tft.setFont();
}
else
{
if (al%2 == 0)
{
tft.drawBitmap(140, 169, faraclopotel, 32, 24, RED);
    digitalWrite(buzzer, LOW);
   // tft.setFont(&FreeSerif9pt7b);
    printText(timeChar, RED,195,170,4); //tft.setFont();
 
}
if (al%2 == 1)
{
   
tft.drawBitmap(140, 169, clopotel, 32, 24, WHITE);  
    digitalWrite(buzzer, LOW);
   // tft.setFont(&FreeSerif9pt7b);
    printText(timeChar, WHITE,195,170,4);// tft.setFont();
}
}
 
  minuteNow = mm;
  if(minuteNow!=minutePrevious || initial)
  {
    initial = 0;
    dateString = getDayOfWeek(zz)+", ";
    dateString = dateString+String(dd)+"-"+month(ll);
    dateString= dateString+"-20"+ String(yy); 
    minutePrevious = minuteNow;
    String hours = "";
    if(hh<10)
    {
         hours = hours+"0"+String(hh);
    }else  
    hours = hours+ String(hh);
    if(mm<10)
    {
        hours = hours+" 0"+String(mm);
    }else
    {
        hours = hours+" "+String(mm);
    }
    
    hours.toCharArray(timeChar,100);
    tft.fillRect(0,0,320,135,BLACK);
  
   tft.setFont(&FreeSerifBold12pt7b);
   //tft.setFont(&FreeSerif9pt7b);
  printText(timeChar, ORANGE,17,122,5); tft.setFont();
    dateString.toCharArray(dateChar,50);
  
    tft.setFont(&FreeSerif9pt7b);
    printText(dateChar, CYAN,70,24,1); tft.setFont();
  }

if(humidity !=previousHumidity)
{previousHumidity = humidity;
    String humidityString = String(humidity,1);
    humidityString.toCharArray(humidityChar,10);
    tft.fillRect(0,213,52,20,BLACK);
    printText(humidityChar, WHITE,0,214,2);
    printText("%", WHITE,61,214,2);
}
  
  if(temperature != previousTemperature)
  {
    previousTemperature = temperature;
    String temperatureString = String(temperature,1);
    temperatureString.toCharArray(temperatureChar,10);
    tft.fillRect(0,165,52,20,BLACK);
    printText(temperatureChar, WHITE,0,169,2);
    printText("o", WHITE,51,168,1);
    printText("C", WHITE,59,169,2);

  /*  if(temperature>maxTemperature)
    {
      maxTemperature = temperature;
    }
      dtostrf(maxTemperature,5, 1, charMaxTemperature); 
      tft.fillRect(0,210,30,15,BLACK);
      printText(charMaxTemperature, WHITE,0,215,1);
      printText("o", WHITE,31,213,1);
      printText("C", WHITE,37,215,1);
    //}
    if(temperature<minTemperature)
    {
      minTemperature = temperature;
    }
      dtostrf(minTemperature,5, 1, charMinTemperature); 
      tft.fillRect(47,210,26,15,BLACK);
      printText(charMinTemperature, WHITE,43,215,1);
      printText("o", WHITE,75,213,1);
      printText("C", WHITE,82,215,1);
    //}
    */
  }

delay(500);

     tft.fillCircle(148,69,7,ORANGE);
     tft.fillCircle(148,105,7,ORANGE);
  

delay(500);
    
    tft.fillCircle(148,69,5,BLACK);
     tft.fillCircle(148,105,5,BLACK);
} // end usual case (clock)

if (nivel == 99)   // clear display
{
  tft.fillScreen(BLACK);

nivel = 1;
  }    

if (nivel == 1)   // change hours
{

if (digitalRead(meniu) == LOW)
  {
  nivel = nivel+1;
   delay(500);
  tft.fillScreen(BLACK);
  tft.fillRect(0,0,160,240,BLACK);
  }    


printText("HOUR:", GREEN,10,36,2);  
dtostrf(hh1,3, 0, chartemp); 
      //tft.fillRect(50,50,70,18,BLACK);
      tft.fillRect(50,50,70,38,BLACK);
      delay(50);
      printText(chartemp, WHITE,60,50,2);
      delay(50);
      
if (digitalRead(plus) == LOW)

    {
    hh1 = hh1+1;
    delay(150);
    }
if (digitalRead(minus) == LOW)

    {
    hh1 = hh1-1;
    delay(150);
    }    
if (hh1 > 23) hh1 = 0;
if (hh1 < 0) hh1 = 23;       
     
}  // end loop nivel = 1 (change the hours)


if (nivel == 2)   // change minutes
{

if (digitalRead(meniu) == LOW)
  {
  nivel = nivel+1;
  delay(500);
  tft.fillScreen(BLACK);
  tft.fillRect(0,0,128,160,BLACK);
  }    

printText("SETTING", GREEN,10,18,2);  // Temperature Static Text
tft.fillRect(10,35,70,18,BLACK);
printText("MINUTE:", GREEN,10,35,2);  // Temperature Static Text
Serial.println(mm1);
dtostrf(mm1,3, 0, chartemp); 
      tft.fillRect(50,50,70,18,BLACK);
      delay(50);
      printText(chartemp, WHITE,60,50,2);
      delay(50);
     
if (digitalRead(plus) == LOW)

    {
    mm1 = mm1+1;
    delay(150);
    }
if (digitalRead(minus) == LOW)

    {
    mm1 = mm1-1;
    delay(150);
    }    
if (mm1 > 59) mm1 = 0;
if (mm1 < 0) mm1 = 59;       
     
}  // end loop nivel = 2 (change the minutes)

if (nivel == 3)   // change day in week
{

if (digitalRead(meniu) == LOW)
  {
  nivel = nivel+1;
  delay(500);
  tft.fillScreen(BLACK);
  tft.fillRect(0,0,128,160,BLACK);
  }    

printText("SETTING", GREEN,10,18,2);  // Temperature Static Text
tft.fillRect(0,35,128,18,BLACK);
printText("Day in Week:", GREEN,10,40,1);  // Temperature Static Text

dtostrf(zz1,3, 0, chartemp); 
      tft.fillRect(50,50,70,18,BLACK);
      delay(50);
      printText(chartemp, WHITE,60,50,2);
tft.fillRect(10,80,100,18,BLACK);      
if (zz1 == 1) printText("1 - Luni", RED,10,80,1);  // Temperature Static Text
if (zz1 == 2) printText("2 - Marti", RED,10,80,1);  // Temperature Static Text
if (zz1 == 3) printText("3 - Miercuri", RED,10,80,1);  // Temperature Static Text
if (zz1 == 4) printText("4 - Joi", RED,10,80,1);  // Temperature Static Text
if (zz1 == 5) printText("5 - Vineri", RED,10,80,1);  // Temperature Static Text
if (zz1 == 6) printText("6 - Simbata", RED,10,80,1);  // Temperature Static Text
if (zz1 == 0) printText("0 - Duminica", RED,10,80,1);  // Temperature Static Text  
    delay(50);
     
if (digitalRead(plus) == LOW)

    {
    zz1 = zz1+1;
    delay(150);
    }
if (digitalRead(minus) == LOW)

    {
    zz1 = zz1-1;
    delay(150);
    }    
if (zz1 > 6) zz1 = 0;
if (zz1 < 0) zz1 = 6;       
     
}  // end loop nivel = 3 (change the day of the week)

if (nivel == 4)   // change year
{

if (digitalRead(meniu) == LOW)
  {
  nivel = nivel+1;
  delay(500);
  tft.fillScreen(BLACK);
  tft.fillRect(0,0,128,160,BLACK);
  tft.fillRect(10,80,100,18,BLACK);    
  }    

printText("SETTING", GREEN,10,18,2);  // Temperature Static Text
tft.fillRect(10,35,70,18,BLACK);
printText("YEAR:", GREEN,10,35,2);  // Temperature Static Text
dtostrf(yy1,3, 0, chartemp); 
      tft.fillRect(40,50,80,18,BLACK);
      tft.fillRect(10,80,100,18,BLACK);  // erase last explication...
      delay(50);
      printText("20", WHITE,45,50,2);
      printText(chartemp, WHITE,60,50,2);
      delay(50);

     
if (digitalRead(plus) == LOW)

    {
    yy1 = yy1+1;
    delay(150);
    }
if (digitalRead(minus) == LOW)

    {
    yy1 = yy1-1;
    delay(150);
    }    
if (yy1 > 80) yy1 = 80;
if (yy1 < 23) yy1 = 23;       
     
}  // end loop stare = 4 (change the year)


if (nivel == 5)   // change mounth
{

if (digitalRead(meniu) == LOW)
  {
  nivel = nivel+1;
  delay(500);
  tft.fillScreen(BLACK);
   tft.fillRect(10,80,100,18,BLACK);    
  }    
printText("SETTING", GREEN,10,18,2);  // Temperature Static Text
tft.fillRect(10,35,70,18,BLACK);
printText("MONTH:", GREEN,10,35,2);  // Temperature Static Text
dtostrf(ll1,3, 0, chartemp); 
      tft.fillRect(40,50,80,18,BLACK);
      tft.fillRect(10,80,100,18,BLACK);  // erase last explication...
      delay(50);
      printText(chartemp, WHITE,60,50,2);
      delay(50);
     
if (digitalRead(plus) == LOW)

    {
    ll1 = ll1+1;
    delay(150);
    }
if (digitalRead(minus) == LOW)

    {
    ll1 = ll1-1;
    delay(150);
    }    
   
if (ll1 > 12) ll1 = 1;
if (ll1 < 1) ll1 = 12;       
     
}  // end loop stare = 5 (change the day as data)

if (nivel == 6)   // change day as data
{

if (digitalRead(meniu) == LOW)
  {
  nivel = nivel+1;
  delay(500);
  tft.fillScreen(BLACK);
 tft.fillRect(10,80,100,18,BLACK);      
  }    

printText("SETTING", GREEN,10,18,2);  // Temperature Static Text
tft.fillRect(10,35,70,18,BLACK);
printText("DAY:", GREEN,10,35,2);  // Temperature Static Text
dtostrf(dd1,3, 0, chartemp); 
      tft.fillRect(50,50,70,18,BLACK);
      tft.fillRect(10,80,100,18,BLACK);  // erase last explication...
      delay(50);
      printText(chartemp, WHITE,60,50,2);
      delay(50);
 
if (digitalRead(plus) == LOW)

    {
    dd1 = dd1+1;
    delay(150);
    }
if (digitalRead(minus) == LOW)

    {
    dd1 = dd1-1;
    delay(150);
    
    }    
    
 // Dawn & Dusk controller. http://andydoz.blogspot.ro/2014_08_01_archive.html
 if (ll == 4 || ll == 5 || ll == 9 || ll == 11) { //30 days hath September, April June and November
    maxday = 30;
  }
  else {
  maxday = 31; //... all the others have 31
  }
  if (ll ==2 && yy % 4 ==0) { //... Except February alone, and that has 28 days clear, and 29 in a leap year.
    maxday = 29;
  }
  if (ll ==2 && ll % 4 !=0) {
    maxday = 28;
  }

if (dd1 > maxday) dd1 = 1;
if (dd1 < 1) dd1 = maxday;       
     
}  // end loop nivel = 6 (change the day as data)

if (nivel == 7)   // change hours alarm
{

if (digitalRead(meniu) == LOW)
  {
  nivel = nivel+1;
  delay(500);
  tft.fillScreen(BLACK);
  tft.fillRect(0,0,128,160,BLACK);
  }    
tft.fillRect(10,15,100,38,BLACK);




printText("ALARM", GREEN,10,18,2);  // Temperature Static Text
printText("HOUR:", GREEN,10,36,2);  // Temperature Static Text
dtostrf(hha,3, 0, chartemp); 
      //tft.fillRect(50,50,70,18,BLACK);
      tft.fillRect(50,50,70,38,BLACK);
      delay(50);
      printText(chartemp, WHITE,60,50,2);
      delay(50);
      
if (digitalRead(plus) == LOW)

    {
    hha = hha+1;
    delay(150);
    }
if (digitalRead(minus) == LOW)

    {
    hha = hha-1;
    delay(150);
    }    
if (hha > 23) hha = 0;
if (hha < 0) hha = 23;       
     
}  // end loop nivel = 7 (change the hours)

if (nivel == 8)   // change minutes for alarm
{

if (digitalRead(meniu) == LOW)
  {
  nivel = nivel+1;
  delay(500);
  tft.fillScreen(BLACK);
  tft.fillRect(0,0,128,160,BLACK);
  }    
tft.fillRect(10,15,70,18,BLACK);
printText("ALARM", GREEN,10,18,2);  // Temperature Static Text
tft.fillRect(10,35,70,18,BLACK);
printText("MINUTE:", GREEN,10,35,2);  // Temperature Static Text
Serial.println(mm1);
dtostrf(mma,3, 0, chartemp); 
      tft.fillRect(50,50,70,18,BLACK);
      delay(50);
      printText(chartemp, WHITE,60,50,2);
      delay(50);
     
if (digitalRead(plus) == LOW)

    {
    mma = mma+1;
    delay(150);
    }
if (digitalRead(minus) == LOW)

    {
    mma = mma-1;
    delay(150);
    }    
if (mma > 59) mma = 0;
if (mma < 0) mma = 59;       
  tft.fillScreen(BLACK);   
}  // end loop nivel = 8 (change the minutes)


 if (nivel >=9)  // readfy to return to main loop
 {
  setDS3231time(0, mm1, hh1, zz1, dd1, ll1, yy1);
/*
// for Mega
  EEPROM.write(100, hha);
  EEPROM.write(101, mma);
  EEPROM.write(102, al%2);
*/  
// for Due
//writeEEPROM(disk1, 100, hha);
//writeEEPROM(disk1, 101, mma);
//writeEEPROM(disk1, 102, al%2);
 EEPROM.write(100, hha);
  EEPROM.write(101, mma);
  EEPROM.write(102, al%2);
  nivel = 0;  
  previousTemperature= 0.0;
  initial = 1;


    printText("Temperatura", GREEN,0,150,1);  // Temperature Static Text
    printText("Alarma setata", MAGENTA,223,150,1);
    printText("Umiditate", GREEN,5,195,1);
   
     printText("@2023 by Vlad Gheorghe", GREENYELLOW,180,230,1); //119
   
   temperature = sht.getTemperature();  // get temp from SHT 
  float temperature = sht.getTemperature(); 
 humidity = sht.getHumidity();
 float humidity = sht.getHumidity();
Serial.println(temperature);

 }
}  // end main loop


void printText(char *text, uint16_t color, int x, int y,int textSize)
{
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextSize(textSize);
  tft.setTextWrap(true);
  tft.print(text);
}

String getDayOfWeek(int i)
{
  switch(i)
  {
    case 0: return "Duminica";break;
    case 1: return "Luni";break;
    case 2: return "Marti";break;
    case 3: return "Miercuri";break;
    case 4: return "Joi";break;
    case 5: return "Vineri";break;
    case 6: return "Simbata";break;
    default: return "Monday";break;
  }
}

float get3231Temp() // from http://www.goodliffe.org.uk/arduino/rtc_ds3231.php
{
  //temp registers (11h-12h) get updated automatically every 64s
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0x11);
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 2);
 
  if(Wire.available()) {
    tMSB = Wire.read(); //2's complement int portion
    tLSB = Wire.read(); //fraction portion
   
    temp3231 = (tMSB & B01111111); //do 2's math on Tmsb
    temp3231 += ( (tLSB >> 6) * 0.25 ); //only care about bits 7 & 8
  }
  else {
    //oh noes, no data!
  }
   
  return temp3231;
}

// http://forum.arduino.cc/index.php?topic=398891.0
void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte
                   dayOfMonth, byte month, byte year)
{
    // sets time and date data to DS3231
    Wire.beginTransmission(DS3231_I2C_ADDRESS);
    Wire.write(0); // set next input to start at the seconds register
    Wire.write(decToBcd(second)); // set seconds
    Wire.write(decToBcd(minute)); // set minutes
    Wire.write(decToBcd(hour)); // set hours
    Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
    Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
    Wire.write(decToBcd(month)); // set month
    Wire.write(decToBcd(year)); // set year (0 to 99)
    Wire.endTransmission();
}

// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
    return ( (val / 16 * 10) + (val % 16) );
}

// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
    return ( (val / 10 * 16) + (val % 10) );
}

void readDS3231time(byte *second,
                    byte *minute,
                    byte *hour,
                    byte *dayOfWeek,
                    byte *dayOfMonth,
                    byte *month,
                    byte *year)
{
    Wire.beginTransmission(DS3231_I2C_ADDRESS);
    Wire.write(0); // set DS3231 register pointer to 00h
    Wire.endTransmission();
    Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
    // request seven bytes of data from DS3231 starting from register 00h
    *second = bcdToDec(Wire.read() & 0x7f);
    *minute = bcdToDec(Wire.read());
    *hour = bcdToDec(Wire.read() & 0x3f);
    *dayOfWeek = bcdToDec(Wire.read());
    *dayOfMonth = bcdToDec(Wire.read());
    *month = bcdToDec(Wire.read());
    *year = bcdToDec(Wire.read());
}
String month(int i)
{
  switch(i)
  {
    case 1: return "IAN";break;
    case 2: return "FEB";break;
    case 3: return "MAR";break;
    case 4: return "APR";break;
    case 5: return "MAI";break;
    case 6: return "IUN";break;
    case 7: return "IUL";break;
    case 8: return "AUG";break;
    case 9: return "SEP";break;
    case 10: return "OCT";break;
    case 11: return "NOV";break;
    case 12: return "DEC";break;
    default: return "***";break;
  }
}

void writeEEPROM(int deviceaddress, unsigned int eeaddress, byte data ) 
{
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.write(data);
  Wire.endTransmission();
 
  delay(5);
}
 
byte readEEPROM(int deviceaddress, unsigned int eeaddress ) 
{
  byte rdata = 0xFF;
 
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();
 
  Wire.requestFrom(deviceaddress,1);
 
  if (Wire.available()) rdata = Wire.read();
 
  return rdata;
}
