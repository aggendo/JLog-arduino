//#include <DS3231.h>
//#include <SPI.h>
#include<Wire.h>
#include "Arduino.h"
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include "JLog.h"
/*
   This sample sketch demonstrates the normal use of a TinyGPS++ (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/
static const int RXPin = 7, TXPin = 6;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

const int MPU_addr=0x68;  // I2C address of the MPU-6050

// Init the DS3231 using the hardware interface
//DS3231  rtc(SDA, SCL);

int SD_ID = 10;

int _IdByteSize;
//char* _filename; //TODO: 50 character limit on filename
//    int _sdcardId;
    char* _headercontents;
    char* _sensorIdNames;
    #define ENDBYTE 0xff
    #define FILE_VERSION "0.1"

JLog logger =  JLog();

char dateStr[5]="";
char timeStr[11]="";

void setup()
{
  // Setup Serial connection
  Serial.begin(9600);
  ss.begin(GPSBaud);
  
  bool gotdate = false;
  pinMode(5, OUTPUT);
  pinMode(3, OUTPUT);
  /*while (millis() < 5000){ //try for 5 seconds to get gps for date for filename
    while (ss.available() > 0){
      if (gps.encode(ss.read())){
        if(gps.date.isValid() && gps.time.isValid()){
          char *dateStr=new char(6);//this one is a guess
          //char *timeStr=new char(11);//this one is correct
          sprintf(dateStr, "%02d-%02d",gps.date.month(),gps.date.day());
          //sprintf(timeStr, "%02d:%02d:%02d.%02d", gps.time.hour(), gps.time.minute(), gps.time.second(), gps.time.centisecond());
          gotdate = true;
          
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
    Serial.println(F("No GPS detected: check wiring."));
    while(true){
      digitalWrite(3, HIGH);
      delay(1000);
      digitalWrite(3, LOW);
      delay(1000);
    }
  }*/
  //char * filename = new char[9];
  //sprintf(filename, "%s.jlg",dateStr);
  logger.begin("test.jlg", SD_ID);//filename, SD_ID);
  logger.addValueId(0x01, "Latitude");
  logger.addValueId(0x02, "Longitude");
  logger.addValueId(0x05, "Altitude");
  //we are going to try and use the gps to get the time and date
  logger.writeHeader();
}

void loop() {
  while (ss.available() > 0){
    if (gps.encode(ss.read())){
      void displayInfo();
      if (gps.location.isValid())
      {
        logger.writeValue(0x01, byte(gps.location.lat()));
        logger.writeValue(0x01, byte(gps.location.lng()));
      }
    }
  }
}
