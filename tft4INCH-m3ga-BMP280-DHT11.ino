// original sketch from http://educ8s.tv/arduino-real-time-clock/
// Nicu FLORICA (niq_ro) from http://www.arduinotehniq.com 
// made some chenages for animate display with blinking signs
// eliminate library for clock, use just subroutines
// use 3 buttons for manual adjust the data & clock
// add alarm (now usee eeprom memory for store hour alarm)

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
//#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <SPI.h>
#include "SparkFunBME280.h"
#include "EEPROM.h"
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <DHT.h>
#define DHTPIN A11     // what pin we're connected DHT11/22
#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22

DHT dht(DHTPIN, DHTTYPE);

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
BME280 capteur;

//#define TFT_CS     9
//#define TFT_RST    7                      
//#define TFT_DC     8
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

// Option 2: use any pins but a little slower!
//#define TFT_SCLK 13   // set these to be whatever pins you like!
//#define TFT_MOSI 11   // set these to be whatever pins you like!

float maxTemperature=0;
float minTemperature=200;
char charMinTemperature[10];
char charMaxTemperature[10];
char timeChar[100];
char dateChar[50];
char temperatureChar[10];
char presiuneChar[10];
char umiditateChar[10];

float temperature = 0;
float previousTemperature = 0;
float presiune = 0;
float previousPresiune = 0;
float umiditate = 0;
float previousUmiditate = 0;

String dateString;
int minuteNow=0;
int minutePrevious=0;


#define DS3231_I2C_ADDRESS 104
byte tMSB, tLSB;
float temp3231;
float temperatura, temperatura0;
byte hh, mm, ss;
byte yy, ll, dd, zz;

#define meniu A8 
#define minus A9
#define plus A10
#define alarm 22
#define buzzer 23

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
byte xa = 250;
byte ya = 250;

void setup () 
{
   Serial.begin(9600);
uint16_t g_identifier;  
 uint32_t when = millis();
    
    if (!Serial) delay(5000);           //allow some time for Leonardo
    Serial.println("Serial took " + String((millis() - when)) + "ms to start");
    static uint16_t identifier;
   
    g_identifier = tft.readID(); //
    Serial.print("ID = 0x");
    Serial.println(g_identifier, HEX);
    if (g_identifier == 0x00D3 || g_identifier == 0xD3D3) g_identifier = 0x9481; // write-only shield
    if (g_identifier == 0xFFFF) g_identifier = 0x9341; // serial

    tft.begin(0x9488); 
  //tft.initR(INITR_BLACKTAB);
   tft.setRotation(3);
    tft.fillScreen(BLACK);
   
    Wire.begin();
    capteur.settings.commInterface = I2C_MODE; 
  capteur.settings.I2CAddress = 0x76;
  capteur.settings.runMode = 3; 
  capteur.settings.tStandby = 0;
  capteur.settings.filter = 0;
  capteur.settings.tempOverSample = 1 ;
  capteur.settings.pressOverSample = 1;
  capteur.settings.humidOverSample = 1;
  delay(10); 
  capteur.begin();
   dht.begin(); 
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
    printText("Temperatura", GREEN,25,195,2);  // Temperature Static Text
   printText("Alarma setata", MAGENTA,235,195,2);
    printText("Presiune", YELLOW,360,270,2);
     printText("Umiditate", YELLOW,215,270,2);
  //  printText("  Ceas cu alarma si termometru V1.2@2017", DARKGREEN,235,295,1);
   // printText("cu Arduino Mega, DS3231 si 3,95", DARKGREEN,235,305,1);
    printText("MAX", RED,28,260,2);
    printText("MIN", BLUE,125,260,2);

// setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year);
//setDS3231time(0, 45, 13, 0, 25, 11, 2016;

//  hha = 7;
//  mma = 00;
//  al = 1;  // 0 = alarm is off (must put in 1 foar active)

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
  nivel = nivel+1;
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


  //float temperature = get3231Temp();
  float temperature = capteur.readTempC();
 float presiune = (capteur.readFloatPressure()/133.322368);
 float umiditate = dht.readHumidity();
 // float temperature = capteur.readTempC(); 
  readDS3231time(&ss, &mm, &hh, &zz, &dd, &ll,&yy);

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
  EEPROM.write(102, al%2);
  delay(500);
  tft.drawBitmap(xa-20, ya-15, faraclopotel, 32, 24, BLACK);  
  tft.drawBitmap(xa-20, ya-15, clopotel, 32, 24, BLACK);  
  }
if (mma == mm & hha == hh & al%2)
{
//digitalWrite(buzzer, HIGH);
tone(buzzer, 1000, 50);
  delay(200);
  noTone(buzzer);
delay(200);
tft.drawBitmap(xa-20, ya-15, clopotel, 32, 24, GREEN);
tft.setFont(&FreeSans12pt7b);
   printText(timeChar, RED,xa+7,ya,2);tft.setFont();
}
else
{
if (al%2 == 0)
{
   tft.drawBitmap(xa-20, ya-15, faraclopotel, 32, 24, RED);
    digitalWrite(buzzer, LOW);tft.setFont(&FreeSans12pt7b);
    printText(timeChar, BLUE,xa+7,ya,2);tft.setFont();
}
if (al%2 == 1)
{
    tft.drawBitmap(xa-20, ya-15, clopotel, 32, 24, WHITE);  
    digitalWrite(buzzer, LOW);tft.setFont(&FreeSans12pt7b);
    printText(timeChar, WHITE,xa+7,ya,2);tft.setFont();
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
    tft.fillRect(0,0,480,185,BLACK);
  //  printText(timeChar, WHITE,20,25,3);lcd.setFreeFont(FSB24);
   tft.setFont(&FreeSansBold24pt7b);printText(timeChar, CYAN,5,175,4);tft.setFont();
    dateString.toCharArray(dateChar,50);tft.setFont(&FreeMonoBold18pt7b);
    printText(dateChar, ORANGE,38,25,1);tft.setFont();
  }
  
  if(temperature != previousTemperature)
  {
    previousTemperature = temperature;
    String temperatureString = String(temperature,1);
    temperatureString.toCharArray(temperatureChar,10);
    tft.fillRect(0,220,228,40,BLACK);tft.setFont(&FreeSans12pt7b);
    printText(temperatureChar, WHITE,20,254,2);
   
    printText("C", WHITE,142,254,2);tft.setFont();
 printText("o", WHITE,120,215,3);
    

    if(temperature>maxTemperature)
    {
      maxTemperature = temperature;
    }
      dtostrf(maxTemperature,5, 1, charMaxTemperature); 
      tft.fillRect(3,282,66,20,BLACK);tft.setFont(&FreeSans12pt7b);
      printText(charMaxTemperature, RED,3,299,1);
     
      printText("C", RED,70,299,1);tft.setFont();
       printText("o", RED,57,280,2);
    //}
    if(temperature<minTemperature)
    {
      minTemperature = temperature;
    }
      dtostrf(minTemperature,5, 1, charMinTemperature); 
      tft.fillRect(100,282,66,20,BLACK);tft.setFont(&FreeSans12pt7b);
      printText(charMinTemperature, BLUE,100,299,1);
     // printText("o", WHITE,165,280,1);
      printText("C", BLUE,170,299,1);tft.setFont();
       printText("o", BLUE,155,280,2);
     
  {
    if(previousPresiune = presiune);
    String presiuneString = String(presiune,1);
    presiuneString.toCharArray(presiuneChar,10);
    tft.fillRect(340,290,300,40,BLACK);tft.setFont(&FreeSans12pt7b);
    printText(presiuneChar, GREENYELLOW,345,310,1);
    printText("mmHg", GREENYELLOW,415,310,1);tft.setFont();
  //  printText("C", WHITE,142,220,4);
  }
   {
    if(previousUmiditate = umiditate);
    String umiditateString = String(umiditate,1);
    umiditateString.toCharArray(umiditateChar,10);
    tft.fillRect(200,290,100,40,BLACK);tft.setFont(&FreeSans12pt7b);
    printText(umiditateChar, WHITE,210,310,1);
    printText("%RH", WHITE,265,310,1);tft.setFont();
  //  printText("C", WHITE,142,220,4);
  }
    //}
    
  }

delay(500);
//     printText("o", WHITE,62,28,1);
//     printText("o", WHITE,62,35,1);
   //  printText("o", ORANGE,225,75,4);
     tft.fillCircle(240,75,10,CYAN);
     tft.fillCircle(240,145,10,CYAN);
   //  printText("o", ORANGE,225,105,4);

delay(500);
    // printText("o", BLACK,225,75,4);
    // printText("o", BLACK,225,105,4);
    tft.fillCircle(240,75,9,BLACK);
     tft.fillCircle(240,145,9,BLACK);
} // end usual case (clock)


if (nivel == 1)   // change hours
{
if (digitalRead(meniu) == LOW)
  {
  nivel = nivel+1;
   delay(500);
  tft.fillScreen(BLACK);
  tft.fillRect(0,0,128,160,BLACK);
  }    

printText("SETTING", GREEN,10,18,2);  // Temperature Static Text
printText("HOUR:", GREEN,10,36,2);  // Temperature Static Text
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
//Serial.println(mm1);
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
if (yy1 > 49) yy1 = 49;
if (yy1 < 16) yy1 = 16;       
     
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
     
}  // end loop nivel = 8 (change the minutes)


 if (nivel >=9)  // readfy to return to main loop
 {
  setDS3231time(0, mm1, hh1, zz1, dd1, ll1, yy1);
  EEPROM.write(100, hha);
  EEPROM.write(101, mma);
  EEPROM.write(102, al%2);
  
  nivel = 0;  
  previousTemperature= 0.0;
  initial = 1;
     printText("Temperatura", GREEN,25,195,2);  // Temperature Static Text
    printText("Alarma setata", MAGENTA,235,195,2);
    printText("Presiune", YELLOW,360,270,2);
    printText("Umiditate", YELLOW,215,270,2);
   //  printText("  Ceas cu alarma si termometru V1.2@2017", DARKGREEN,235,295,1);
    //printText("cu Arduino Mega, DS3231 si 3,95", DARKGREEN,235,305,1);
    printText("MAX", RED,28,260,2);
    printText("MIN", BLUE,125,260,2);
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
    default: return "****";break;
  }
}

