#include <SD.h>
#include <Arduino.h>
#include <U8x8lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
const int chipSelect = 4;

File file;



#include "DHT.h"
#define DHT11Pin 7
#define DHTType DHT11
DHT HT(DHT11Pin,DHTType);

int i =0;
int y=0;
float humi;
float tempC;
float tempF;
int fanpin =2;
int currentflipstate;
int lastflipstate;
int pirPin = 6;
int val = LOW;
int currentstate =0;
int laststate = 0;
const int xInput = A3;
const int yInput = A2;
const int zInput = A1;
int RawMin = 0;
int RawMax = 1023;
const int sampleSize = 10;


U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   
void setup() {
  Serial.begin(9600);
   analogReference(EXTERNAL);
  Serial.begin(9600);
  //For DHT11
  HT.begin();
  

  pinMode (pirPin, INPUT);
  pinMode(fanpin, OUTPUT); 
  currentstate =digitalRead(pirPin);
   u8x8.begin();
  u8x8.setPowerSave(0);
  pinMode(chipSelect, OUTPUT); // chip select pin must be set to OUTPUT mode
  if (!SD.begin(chipSelect)) { // Initialize SD card
    Serial.println("Could not initialize SD card."); // if return value is false, something went wrong.
  }
  
  if (SD.exists("file.txt")) { // if "file.txt" exists, fill will be deleted
    Serial.println("File exists.");
    if (SD.remove("file.txt") == true) {
      Serial.println("Successfully removed file.");
    } else {
      Serial.println("Could not removed file.");
    }
  }
  
  currentflipstate=ReadAxis(zInput);
}

void loop() {
  
 int xRaw = ReadAxis(xInput);
  int yRaw = ReadAxis(yInput);
  int zRaw = ReadAxis(zInput);

  // Convert raw values to 'milli-Gs"
  long xScaled = map(xRaw, RawMin, RawMax, -3000, 3000);
  long yScaled = map(yRaw, RawMin, RawMax, -3000, 3000);
  long zScaled = map(zRaw, RawMin, RawMax, -3000, 3000);

  // re-scale to fractional Gs
int xAccel = xScaled / 1000.0-2;
  int yAccel = yScaled / 1000.0-2;
  int zAccel = zScaled / 1000.0-2;

  Serial.print("X, Y, Z  :: ");
  Serial.print(xRaw);
  Serial.print(", ");
  Serial.print(yRaw);
  Serial.print(", ");
  Serial.print(zRaw);
  Serial.print(" :: ");
  Serial.print(xAccel);
  Serial.print("G, ");
  Serial.print(yAccel);
  Serial.print("G, ");
  Serial.print(zAccel);
  Serial.println("G");

  delay(200);
  
  
 humi = HT.readHumidity();
 tempC = HT.readTemperature();
 tempF = HT.readTemperature(true);

 Serial.print("Humidity:");
 Serial.print(humi,0);
 Serial.print("%");
 Serial.print(" Temperature:");
 Serial.print(tempC,1);
 Serial.print("C ~ ");
 Serial.print(tempF,1);
 Serial.println("F");


 //display.display(); 
 u8x8.setFont(u8x8_font_courB18_2x3_r);
  u8x8.drawString(0,1,"Temp");
  u8x8.setCursor(0, 5);
  u8x8.print(tempC);
   u8x8.setFont(u8x8_font_8x13_1x2_r);
   u8x8.drawString(11,2,"Humid");
   u8x8.setCursor(11, 5);
   u8x8.print(humi);
  
  u8x8.refreshDisplay();   // only required for SSD1606/7  
  delay(2000);
 
 
 laststate=currentstate;
 currentstate=digitalRead(pirPin);
 




if (tempC>=28){

  digitalWrite(fanpin,HIGH);
   delay(1000);
}
 if (tempC<28){
  
  digitalWrite(fanpin,LOW);

  

}
lastflipstate=currentflipstate;
currentflipstate=ReadAxis(zInput);

 file = SD.open("file.txt", FILE_WRITE); // open "file.txt" to write data
  if (lastflipstate>=750&&currentflipstate<=650||lastflipstate<=650&&currentflipstate>=750)
{i++;
 if (file) {
    file.println("counter of flips:");
    file.println(i); // write number to file
    file.close(); // close file
    Serial.print("Wrote number: "); // debug output: show written number in serial monitor
    Serial.println(i);
  } else {
    Serial.println("Could not open file (writing).");
  }
}
if (laststate==HIGH&& currentstate == LOW||laststate==LOW&& currentstate == HIGH)
{y++;
if (file) {
   file.println("counter of waking up:");
  file.println(y); // write number to file
    file.close(); // close file
    Serial.print("Wrote number: "); // debug output: show written number in serial monitor
    Serial.println(i);
 } else {
    Serial.println("Could not open file (writing).");
  }
}
  file = SD.open("file.txt", FILE_READ); // open "file.txt" to read data
  if (file) {
    Serial.println("- – Reading start – -");
    char character;
    while ((character = file.read()) != -1) { // this while loop reads data stored in "file.txt" and prints it to serial monitor
      Serial.print(character);
    }
    file.close();
    Serial.println("- – Reading end – -");
  } else {
    Serial.println("Could not open file (reading).");
  }
  delay(5000); // wait for 5000ms
  


}

int ReadAxis(int axisPin)
{
  long reading = 0;
  analogRead(axisPin);
  delay(1);
  for (int i = 0; i < sampleSize; i++)
  {
  reading += analogRead(axisPin);
  }
  return reading/sampleSize;
}
