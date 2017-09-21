#include "Arduino.h"
#include "JLog.h"
//#ifdef USE_SDCARD
#include <SPI.h>
#include <SD.h>

File dataFile;
//#endif

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

JLog::JLog()
{
}
void JLog::begin(char filename[], int sdcardId){
  _filename = filename;
  _sdcardId = sdcardId;
  #ifdef USE_SDCARD
  if (!SD.begin(sdcardId)) {
        Serial.println("Card failed, or not present");
        return;
  }
  dataFile = SD.open(filename, FILE_WRITE);
  #endif
  _headercontents = "<header>\n\r";
  addToHeader("version", FILE_VERSION);
}
void JLog::begin(){
  Serial.print("<header>\n\r");
  //_headercontents = "<header>\n\r";
  addToHeader("version", FILE_VERSION);
  Serial.println("names:{");
}

void JLog::addSensorLogId(char id[], char Name[]){
  //sprintf(_sensorIdNames, "%s%s:%s,\n\r", _sensorIdNames, Name, id);
  Serial.print(Name);
  Serial.print(":");
  Serial.print(id);
  Serial.println(",");
}
void JLog::addToHeader(char Name[], char Value[]){
  Serial.print(Name);
  Serial.print(":");
  Serial.print(String(Value));
  Serial.print(";\n\r");
  /*strcat(_headercontents, Name);
  strcat(_headercontents, ":");
  strcat(_headercontents, Value);
  strcat(_headercontents, ";\n\r");*/
}
/*void JLog::setIdByteSize(int Size){
  
}*/
void JLog::writeHeader(){
  char* header = "";
  //strcat(header, _headercontents);
  sprintf(header, "names: {\n\r %s};\n\r</header>",_sensorIdNames);
  #ifdef USE_SDCARD
  dataFile.print(header);
  Serial.print("oohs");
  dataFile.flush(); //make sure everything gets put in the file before we continue.
  #endif
  //#ifdef LOG_USB
  Serial.print("}</header>");
  Serial.flush();
  //#endif
}

//Below is not good programming but it is more efficiant than a proper modular design
//Sadly this is the best way of writing it for speed

void JLog::writeValue(int sensorId, int sensorValue){
  struct datastore_int myData;
  myData.variable_length = sizeof(sensorValue);
  myData.end_byte = ENDBYTE;
  myData.Millis = millis();
  myData.sensorId = sensorId;
  myData.value = sensorValue;
  //dataFile.write((const uint8_t *)&myData, sizeof(myData));
  //dataFile.flush();
  Serial.write((uint8_t *)&myData, sizeof(myData));
}

void JLog::writeValue(int sensorId, long sensorValue){
  struct datastore_long myData;
  myData.variable_length = 4;
  myData.end_byte = ENDBYTE;
  myData.Millis = millis();
  myData.sensorId = sensorId;
  myData.value = sensorValue;
  //dataFile.write((const uint8_t *)&myData, sizeof(myData));
  //dataFile.flush();
  Serial.write((uint8_t *)&myData, sizeof(myData));
}
void JLog::writeValue(int sensorId, byte sensorValue){
  struct datastore_char myData;
  myData.variable_length = 1;
  myData.end_byte = ENDBYTE;
  myData.Millis = millis();
  myData.sensorId = sensorId;
  myData.value = sensorValue;
  //dataFile.write((const uint8_t *)&myData, sizeof(myData));
  //dataFile.flush();
  Serial.write((uint8_t *)&myData, sizeof(myData));
}
void JLog::writeValue(int sensorId, char sensorValue){
  struct datastore_char myData;
  myData.variable_length = 1;
  myData.end_byte = ENDBYTE;
  myData.Millis = millis();
  myData.sensorId = sensorId;
  myData.value = sensorValue;
  //dataFile.write((const uint8_t *)&myData, sizeof(myData));
  //dataFile.flush();
  Serial.write((uint8_t *)&myData, sizeof(myData));
}
void JLog::writeValue(int sensorId, short sensorValue){
  struct datastore_short myData;
  myData.variable_length = 2;
  myData.end_byte = ENDBYTE;
  myData.Millis = millis();
  myData.sensorId = sensorId;
  myData.value = sensorValue;
  //dataFile.write((const uint8_t *)&myData, sizeof(myData));
  //dataFile.flush();
  Serial.write((uint8_t *)&myData, sizeof(myData));
}
void JLog::closeFile(){
  dataFile.close();
}

