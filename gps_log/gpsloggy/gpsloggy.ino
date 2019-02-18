/*
  SD card datalogger

 This example shows how to log data from three analog sensors
 to an SD card using the SD library.

 The circuit:
 * analog sensors on analog ins 0, 1, and 2
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

 created  24 Nov 2010
 modified 9 Apr 2012
 by Tom Igoe

 This example code is in the public domain.

 */

#include <SPI.h>
#include <SD.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#define debug

const int chipSelect = 10;
static const int RXPin = 7, TXPin = 6;
static const uint32_t GPSBaud = 9600;
const int LED1 = 3;
const int LED2 = 5;
String BFilename;
String AFilename;

//if you want to only keep 1 in gps signal every second. 
//(69 makes sure you always get the first second) since it will never get past 60
uint8_t lastSec = 69;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  /*while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }*/
  ss.begin(GPSBaud);

  #ifdef debug
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
  #endif

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    #ifdef debug
    Serial.println("Card failed, or not present");
    #endif
    // don't do anything more:
    while (1){
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, LOW);
      delay(700);
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, HIGH);
      delay(700);
    }
  }
  #ifdef debug
  Serial.println("card initialized.");
  #endif
  boolean hasDate = false;
  while(!hasDate){
    if (ss.available() > 0){
      if (gps.encode(ss.read())){
        if (gps.time.isValid()){
          hasDate = true; //we now have a valid date.
          String fileBase;
          fileBase += padZ(String(gps.date.month()));
          fileBase += "-";
          fileBase += padZ(String(gps.date.day()));
          BFilename = fileBase + "B.txt";
          AFilename = fileBase + "A.txt";
          if(SD.exists(BFilename)){
            boolean newFile = false;
            int i = 1;
            while (!newFile){
              String newEnd = padZ(String(i)) + ".txt";
              if(!SD.exists(fileBase+"B"+newEnd)){
                BFilename = fileBase+"B"+newEnd;
                AFilename = fileBase+"A"+newEnd;
                newFile = true;
              }
              i++;
            }
          }
          #ifdef debug
          Serial.print("logging to ");
          Serial.print(BFilename);
          Serial.print(" & ");
          Serial.print(AFilename);
          #endif
          String header = "Latitude,Longitude,altitude(ft),speed(mph),num of sats,millis,date in DDMMYY,time in HHMMSS";
          Serial.print("areal\t");
    
          String dataString;
          dataString += "xAccW";
          dataString += ',';
          dataString += "yAccW";
          dataString += ',';
          dataString += "zAccW";
          dataString += ',';
          dataString += "xAcc";
          dataString += ',';
          dataString += "yAcc";
          dataString += ',';
          dataString += "zAcc";
          dataString += ',';
          dataString += "roll";
          dataString += ',';
          dataString += "pitch";
          dataString += ',';
          dataString += "yaw";
          dataString += ',';
          dataString += "millis";
          dataString += ',';
          dataString += "date in DDMMYY";
          dataString += ',';
          dataString += "time in HHMMSS";
          
          writeToFile(BFilename, header);
          writeToFile(AFilename, dataString);
        }
      }
    }
    if (millis() > 5000 && gps.charsProcessed() < 10)
    {
      #ifdef debug
      Serial.println(F("No GPS detected: check wiring."));
      #endif
      while (1){
        digitalWrite(LED1, HIGH);
        digitalWrite(LED2, LOW);
        delay(100);
        digitalWrite(LED1, LOW);
        digitalWrite(LED2, HIGH);
        delay(100);
      }
    }
  }  
}

void loop() {
  while (ss.available() > 0){
    if (gps.encode(ss.read())){
      if (gps.location.isValid()){
        if(gps.time.isValid()){
          uint8_t Nsec = gps.time.second();
          if(lastSec!=Nsec){
            lastSec=Nsec;
            // make a string for assembling the data to log:
            String dataString = "";
          
            /*// read three sensors and append to the string:
            for (int analogPin = 0; analogPin < 3; analogPin++) {
              int sensor = analogRead(analogPin);
              dataString += String(sensor);
              if (analogPin < 2) {
                dataString += ",";
              }
            }*/
            dataString += String(gps.location.lat(), 6);
            dataString += ',';
            dataString += String(gps.location.lng(), 6);
            dataString += ',';
            dataString += String(gps.altitude.feet());
            dataString += ',';
            dataString += String(gps.speed.mph());
            dataString += ',';
            dataString += String(gps.satellites.value());
            dataString += ',';
            dataString += String(millis());
            dataString += ',';
            dataString += String(gps.date.value());
            dataString += ',';
            String timeStr = String(gps.time.value());
            timeStr.remove(6,7);
            dataString += timeStr;
            
            writeToFile(BFilename, dataString);
          }
        }
      }
    }
  }
  #ifdef mpu
  if(0){
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetAccel(&aa, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
    Serial.print("areal\t");
    Serial.print(aaReal.x);
    Serial.print("\t");
    Serial.print(aaReal.y);
    Serial.print("\t");
    Serial.println(aaReal.z);
    // display Euler angles in degrees
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    Serial.print("ypr\t");
    Serial.print(ypr[0] * 180/M_PI);
    Serial.print("\t");
    Serial.print(ypr[1] * 180/M_PI);
    Serial.print("\t");
    Serial.println(ypr[2] * 180/M_PI);
  }
  #endif
}
String padZ(String number){
  if(number.length()==1){
    String returnable;
    returnable += "0";
    returnable += number;
    return(returnable);
  } else{
    return(number);
  }
}
void writeToFile(String fName, String dataString){
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open(fName, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    #ifdef debug
    Serial.println(dataString);
    #endif
  }
  // if the file isn't open, pop up an error:
  else {
    #ifdef debug
    Serial.print("error opening: ");
    Serial.println(fName);
    #endif
    while (1){
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, LOW);
      delay(700);
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, HIGH);
      delay(700);
    }
  }
}

