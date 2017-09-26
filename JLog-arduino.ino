//#include <DS3231.h>
//#include <SPI.h>
#include<Wire.h>
#include "Arduino.h"
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
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

char * _filename;

int _IdByteSize;
//char* _filename; //TODO: 50 character limit on filename
//    int _sdcardId;
    char* _headercontents;
    char* _sensorIdNames;
    #define ENDBYTE 0xff
    #define FILE_VERSION "0.1"

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
  begin_log("test.jlg", SD_ID);//filename, SD_ID);
  addValueId("0x01", "Latitude");
  addValueId("0x02", "Longitude");
  addValueId("0x05", "Altitude");
  //we are going to try and use the gps to get the time and date
  writeHeader();
}

void loop() {
  while (ss.available() > 0){
    if (gps.encode(ss.read())){
      void displayInfo();
      if (gps.location.isValid())
      {
        writeValue(0x01, gps.location.lat());
        writeValue(0x01, gps.location.lng());
      }
    }
  }
}
#include <SPI.h>
#include <SD.h>

File dataFile;

struct datastore_char {
    long Millis;
    char sensorId;
    char variable_length; //1
    char value;
    char end_byte;
};

struct datastore_int {
    long Millis;
    char sensorId;
    char variable_length; //2 or 4 bytes
    int value;
    char end_byte;
};

struct datastore_double {
    long Millis;
    char sensorId;
    char variable_length; //2 or 4 bytes
    double value;
    char end_byte;
};

struct datastore_short {
    long Millis;
    char sensorId;
    char variable_length; //2 bytes
    short value;
    char end_byte;
};

struct datastore_long {
    long Millis;
    char sensorId;
    char variable_length; //4 bytes
    long value;
    char end_byte;
};
void begin_log(char datestr[], int SD_ID){
  if (!SD.begin(SD_ID)) {
     Serial.println("Card failed, or not present");
     return;
  }
  Serial.println("card initialized");
  //now lets find next availible name
  /*int i = 0;
  char* filename = sprintf("%s_%i.jlog",datestr, i);
  while(SD.exists(filename)){
    i++;
    filename = sprintf("%s-%i.jlog",datestr, i);
  }*/
  char * filename = "loggy1.txt";
  _filename = filename;
  dataFile = SD.open(_filename, FILE_WRITE);//filename, FILE_WRITE);
  if(!dataFile){
    Serial.print("cannot write to file: ");
    Serial.println(_filename);
    return;
  }
  Serial.print("Logging to ");
  Serial.println(_filename);
  addToHeader("version", FILE_VERSION);
  dataFile.close();
}
void begin(){
  Serial.print("<header>\n\r");
  //_headercontents = "<header>\n\r";
  addToHeader("version", FILE_VERSION);
  Serial.println("names:{");
}

void addValueId(char id[], char Name[]){
  sprintf(_sensorIdNames, "%s%s:%s,\n\r", _sensorIdNames, Name, id);
}
void addToHeader(char Name[], char Value[]){
  sprintf("%s%s:%s;\n\r", _headercontents, Name, Value);
}
/*void setIdByteSize(int Size){
  
}*/
void writeHeader(){
  dataFile = SD.open(_filename, FILE_WRITE);//filename, FILE_WRITE);
  if(!dataFile){
    Serial.print("cannot write to file: ");
    Serial.println(_filename);
    return;
  }
  char* header;
  sprintf(header, "<header>\n\r%s names: {\n\r %s};\n\r</header>", _headercontents, _sensorIdNames);
  dataFile.write(header);
  dataFile.flush(); //make sure everything gets put in the file before we continue.
  dataFile.close();
}

//Below is not good programming but it is more efficiant than a proper modular design
//Sadly this is the best way of writing it for speed

void writeValue(int sensorId, int sensorValue){
  dataFile = SD.open(_filename, FILE_WRITE);//filename, FILE_WRITE);
  if(!dataFile){
    Serial.print("cannot write to file: ");
    Serial.println(_filename);
    return;
  }
  struct datastore_int myData;
  myData.variable_length = sizeof(sensorValue);
  myData.end_byte = ENDBYTE;
  myData.Millis = millis();
  myData.sensorId = sensorId;
  myData.value = sensorValue;
  dataFile.write((const uint8_t *)&myData, sizeof(myData));
  dataFile.flush();
  //Serial.write((uint8_t *)&myData, sizeof(myData));
  dataFile.close();
}

void writeValue(int sensorId, long sensorValue){
  dataFile = SD.open(_filename, FILE_WRITE);//filename, FILE_WRITE);
  if(!dataFile){
    Serial.print("cannot write to file: ");
    Serial.println(_filename);
    return;
  }
  struct datastore_long myData;
  myData.variable_length = 4;
  myData.end_byte = ENDBYTE;
  myData.Millis = millis();
  myData.sensorId = sensorId;
  myData.value = sensorValue;
  dataFile.write((const uint8_t *)&myData, sizeof(myData));
  dataFile.flush();
  //Serial.write((uint8_t *)&myData, sizeof(myData));
  dataFile.close();
}
void writeValue(int sensorId, byte sensorValue){
  dataFile = SD.open(_filename, FILE_WRITE);//filename, FILE_WRITE);
  if(!dataFile){
    Serial.print("cannot write to file: ");
    Serial.println(_filename);
    return;
  }
  struct datastore_char myData;
  myData.variable_length = 1;
  myData.end_byte = ENDBYTE;
  myData.Millis = millis();
  myData.sensorId = sensorId;
  myData.value = sensorValue;
  dataFile.write((const uint8_t *)&myData, sizeof(myData));
  dataFile.flush();
  //Serial.write((uint8_t *)&myData, sizeof(myData));
  dataFile.close();
}
void writeValue(int sensorId, char sensorValue){
  dataFile = SD.open(_filename, FILE_WRITE);//filename, FILE_WRITE);
  if(!dataFile){
    Serial.print("cannot write to file: ");
    Serial.println(_filename);
    return;
  }
  struct datastore_char myData;
  myData.variable_length = 1;
  myData.end_byte = ENDBYTE;
  myData.Millis = millis();
  myData.sensorId = sensorId;
  myData.value = sensorValue;
  dataFile.write((const uint8_t *)&myData, sizeof(myData));
  dataFile.flush();
  dataFile.close();
  //Serial.write((uint8_t *)&myData, sizeof(myData));
}
void writeValue(int sensorId, short sensorValue){
  dataFile = SD.open(_filename, FILE_WRITE);//filename, FILE_WRITE);
  if(!dataFile){
    Serial.print("cannot write to file: ");
    Serial.println(_filename);
    return;
  }
  struct datastore_short myData;
  myData.variable_length = 2;
  myData.end_byte = ENDBYTE;
  myData.Millis = millis();
  myData.sensorId = sensorId;
  myData.value = sensorValue;
  dataFile.write((const uint8_t *)&myData, sizeof(myData));
  dataFile.flush();
  dataFile.close();
  //Serial.write((uint8_t *)&myData, sizeof(myData));
}
void writeValue(int sensorId, double sensorValue){
  dataFile = SD.open(_filename, FILE_WRITE);//filename, FILE_WRITE);
  if(!dataFile){
    Serial.print("cannot write to file: ");
    Serial.println(_filename);
    return;
  }
  struct datastore_double myData;
  myData.variable_length = sizeof(sensorValue);
  myData.end_byte = ENDBYTE;
  myData.Millis = millis();
  myData.sensorId = sensorId;
  myData.value = sensorValue;
  dataFile.write((const uint8_t *)&myData, sizeof(myData));
  dataFile.flush();
  dataFile.close();
  //Serial.write((uint8_t *)&myData, sizeof(myData));
}
void closeFile(){
  dataFile.close();
}
