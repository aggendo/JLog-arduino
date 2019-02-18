//#include <DS3231.h>
//#include <SPI.h>
#include<Wire.h>
#include <SPI.h>
#include <SD.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include "JLog.h"

const int MPU_addr=0x68;  // I2C address of the MPU-6050

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(7, 6);

// Init the DS3231 using the hardware interface
//DS3231  rtc(SDA, SCL);

int SD_ID = 10;

//char dateStr[5]="";
//char timeStr[11]="";

PROGMEM const char LAT=0x01;
PROGMEM const char LOG=0x02;
PROGMEM const char HOURS = 0x06;
PROGMEM const char MINUTES = 0x07;
PROGMEM const char SECONDS = 0x08;
PROGMEM const char ALT  = 0x05;
PROGMEM const char SPEED = 0x03;
PROGMEM const char COURSE = 0x04;

static char *filename = new char(12);


JLog logger = JLog();

void setup()
{
  pinMode(5, OUTPUT);
  pinMode(3, OUTPUT);
  digitalWrite(5, HIGH);
  digitalWrite(3, HIGH);
  // Setup Serial connection
  Serial.begin(9600);
  ss.begin(9600);
  
  bool gotdate = false;
  while (millis() < 5000){ //try for 5 seconds to get gps for date for filename
    while (ss.available() > 0){
      if (gps.encode(ss.read())){
        if(gps.date.isValid() && gps.time.isValid()){
          char *dateStr=new char(6);//this one is a guess
          //char *filename = new char(11);
          //char *timeStr=new char(11);//this one is correct
          sprintf(dateStr, "%02d-%02d",gps.date.month(),gps.date.day());
          //sprintf(timeStr, "%02d:%02d:%02d.%02d", gps.time.hour(), gps.time.minute(), gps.time.second(), gps.time.centisecond());
          
          gotdate = true;
          sprintf(filename, "%s.jlg",dateStr);
          logger.begin(filename, SD_ID);//filename, SD_ID,true);
          break;
        }
      }
    }
    if(gotdate){
      break;
    }
  }
  if (!gotdate) //if we have not gotten gps go into a loop where we show that something is wrong
  {
    digitalWrite(5, LOW);
    Serial.println(F("No GPS detected: check wiring."));
    while(true){
      digitalWrite(3, HIGH);
      delay(1000);
      digitalWrite(3, LOW);
      delay(1000);
    }
  }
  digitalWrite(5, LOW);
  digitalWrite(3, LOW);
}

void loop() {
 while (ss.available() > 0){
    if (gps.encode(ss.read())){
      if (gps.location.isValid() && gps.location.isUpdated())
      {
        logger.writeValue(LOG,gps.location.lng());
        logger.writeValue(LAT, gps.location.lat());
        logger.writeValue(ALT, gps.altitude.meters());
        logger.writeValue(SPEED, gps.speed.mps());
        logger.writeValue(COURSE, gps.course.deg()); //angle in degrees
        //Serial.println("loop");
      }
      if(gps.time.isValid()){
        digitalWrite(5, HIGH);
        logger.writeValue(HOURS, gps.time.hour());
        logger.writeValue(MINUTES, gps.time.minute());
        logger.writeValue(SECONDS, gps.time.second());
        digitalWrite(5, LOW);
      }
    }
  }
}
