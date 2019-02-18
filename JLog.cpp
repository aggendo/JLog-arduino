#include "JLog.h"
#include "Arduino.h"
#include <SPI.h>
#include <SD.h>

short header_line; //which line is the last key:value pair in header on (not including those in 'name : {}'
short name_line; //last line with ValueId 'id:name' pair

//#TODO: make variable length byte actually an byte that denotes variable type not the number of bytes the variable is i.e. (0x01 = char, 0x02 = unsigned char...) that will be in version 0.1.2 of format

PROGMEM struct datastore {
    unsigned long Millis;
    char sensorId;
    char variable_length; //1
};

#define FLOAT_  0x02;
#define DOUBLE_ 0x03;
#define INT_    0x04;
#define SHORT_  0x05;
#define CHAR_   0x06;
#define UNCHAR_ 0x07;
#define LONG_   0x08;
#define UNLONG_ 0x09;

char * _filename = new char[20]; //TODO: 50 character limit on filename

JLog::JLog(){
}

bool JLog::begin(char *filename, int sdcardId){
  boolean result = begin(filename, sdcardId, false);
  return(result); //default will find a new name that  will work.
}

bool JLog::begin(char filename[], int sdcardId, bool overWrite){
  if (!SD.begin(sdcardId)) {
     Serial.println(F("Card failed, or not present"));
     return(false);
  }
  Serial.println(F("card initialized"));
  if(SD.exists(filename)){
    if(!overWrite){ //we need to find the next availible filename
      //now lets find next availible name
      char * actualName = strtok_r(filename, ".", &filename);
      Serial.print(F("finding next availible name for file of name: "));
      int file = 1;
      sprintf(filename, "%s_%02d.jlg", actualName, file);
      do
        {
         file = file + 1;
         sprintf(filename, "%s_%02d.jlg", actualName, file);
        } while (SD.exists(filename));
    } else {
      SD.remove(filename);//get rid of old file
    }
  }
  strcpy(_filename, filename);
  Serial.print(F("Logging to file: "));
  Serial.println(_filename);
  return(true);
}

//Below is not good programming but it is more efficiant than a proper modular design
//Sadly this is the best way of writing it for speed

bool JLog::writeValue(char const sensorId, int sensorValue){
  File dataFile = SD.open(_filename, FILE_WRITE);
  if(!dataFile){
    Serial.print(F("cannot write int to file: "));
    Serial.println(_filename);
    return;
  }
  struct datastore myData;
  myData.variable_length = INT_;
  myData.Millis = millis();
  myData.sensorId = sensorId;
  dataFile.write((const uint8_t *)&myData, sizeof(myData));
  dataFile.write((const uint8_t *)&sensorValue, sizeof(sensorValue));
  dataFile.write(0xff); //TODO: use ENDBYTE
  dataFile.flush();
  dataFile.close();
  return(true);
}

bool JLog::writeValue(char const sensorId, long sensorValue){
  File dataFile = SD.open(_filename, FILE_WRITE);
  if(!dataFile){
    Serial.print(F("cannot write long to file: "));
    Serial.println(_filename);
    return;
  }
  struct datastore myData;
  myData.variable_length = LONG_;
  myData.Millis = millis();
  myData.sensorId = sensorId;
  dataFile.write((const uint8_t *)&myData, sizeof(myData));
  dataFile.write((const uint8_t *)&sensorValue, sizeof(sensorValue));
  dataFile.write(0xff); //TODO: use ENDBYTE
  dataFile.flush();
  dataFile.close();
  return(true);
}

bool JLog::writeValue(char const sensorId, char sensorValue){
  File dataFile = SD.open(_filename, FILE_WRITE);
  if(!dataFile){
    Serial.print(F("cannot write char to file: "));
    Serial.println(_filename);
    return;
  }
  struct datastore myData;
  myData.variable_length = CHAR_;
  myData.Millis = millis();
  myData.sensorId = sensorId;
  dataFile.write((const uint8_t *)&myData, sizeof(myData));
  dataFile.write((const uint8_t *)&sensorValue, sizeof(sensorValue));
  dataFile.write(0xff); //TODO: use ENDBYTE
  dataFile.flush();
  dataFile.close();
  return(true);
}
bool JLog::writeValue(char const sensorId, short sensorValue){
  File dataFile = SD.open(_filename, FILE_WRITE);
  if(!dataFile){
    Serial.print(F("cannot write short to file: "));
    Serial.println(_filename);
    return;
  }
  struct datastore myData;
  myData.variable_length = SHORT_;
  myData.Millis = millis();
  myData.sensorId = sensorId;
  dataFile.write((const uint8_t *)&myData, sizeof(myData));
  dataFile.write((const uint8_t *)&sensorValue, sizeof(sensorValue));
  dataFile.write(0xff); //TODO: use ENDBYTE
  dataFile.flush();
  dataFile.close();
  return(true);
}

bool JLog::writeValue(char const sensorId, unsigned long sensorValue){
  File dataFile = SD.open(_filename, FILE_WRITE);
  if(!dataFile){
    Serial.print(F("cannot write short to file: "));
    Serial.println(_filename);
    return;
  }
  struct datastore myData;
  myData.variable_length = UNLONG_;
  myData.Millis = millis();
  myData.sensorId = sensorId;
  dataFile.write((const uint8_t *)&myData, sizeof(myData));
  dataFile.write((const uint8_t *)&sensorValue, sizeof(sensorValue));
  dataFile.write(0xff); //TODO: use ENDBYTE
  dataFile.flush();
  dataFile.close();
  return(true);
}

bool JLog::writeValue(char const sensorId, double sensorValue){
  File dataFile = SD.open(_filename, FILE_WRITE);
  if(!dataFile){
    Serial.print(F("cannot write double to file: "));
    Serial.println(_filename);
    return;
  }
  struct datastore myData;
  myData.variable_length = DOUBLE_;
  myData.Millis = millis();
  myData.sensorId = sensorId;
  dataFile.write((const uint8_t *)&myData, sizeof(myData));
  dataFile.write((const uint8_t *)&sensorValue, sizeof(sensorValue));
  dataFile.write(0xff); //TODO: use ENDBYTE
  dataFile.flush();
  dataFile.close();
  return(true);
}
#ifdef DEBUG
void JLog::serialUpload(char const * FileName){
  File myFile = SD.open(FileName, FILE_READ);
  if (myFile) {
    Serial.println(F("contents:"));

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.print("error opening ");
    Serial.println(_filename);
  }
}
#endif


