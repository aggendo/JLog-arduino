#include "Arduino.h"
#include "JLog.h"
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

JLog::JLog()
{
}
void JLog::begin(char datestr[], int SD_ID){
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
  String filename = "loggy1.txt";
  dataFile = SD.open(filename);//filename, FILE_WRITE);
  if(!dataFile){
    Serial.print("cannot write to file: ");
    Serial.println(filename);
    return;
  }
  Serial.print("Logging to ");
  Serial.println(filename);
  addToHeader("version", FILE_VERSION);
}
void JLog::begin(){
  Serial.print("<header>\n\r");
  //_headercontents = "<header>\n\r";
  addToHeader("version", FILE_VERSION);
  Serial.println("names:{");
}

void JLog::addSensorLogId(char id[], char Name[]){
  sprintf(_sensorIdNames, "%s%s:%s,\n\r", _sensorIdNames, Name, id);
}
void JLog::addToHeader(char Name[], char Value[]){
  sprintf("%s%s:%s;\n\r", _headercontents, Name, Value);
}
/*void JLog::setIdByteSize(int Size){
  
}*/
void JLog::writeHeader(){
  char* header;
  sprintf(header, "<header>\n\r%s names: {\n\r %s};\n\r</header>", _headercontents, _sensorIdNames);
  dataFile.print(header);
  dataFile.flush(); //make sure everything gets put in the file before we continue.
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
  dataFile.write((const uint8_t *)&myData, sizeof(myData));
  dataFile.flush();
  //Serial.write((uint8_t *)&myData, sizeof(myData));
}

void JLog::writeValue(int sensorId, long sensorValue){
  struct datastore_long myData;
  myData.variable_length = 4;
  myData.end_byte = ENDBYTE;
  myData.Millis = millis();
  myData.sensorId = sensorId;
  myData.value = sensorValue;
  dataFile.write((const uint8_t *)&myData, sizeof(myData));
  dataFile.flush();
  //Serial.write((uint8_t *)&myData, sizeof(myData));
}
void JLog::writeValue(int sensorId, byte sensorValue){
  struct datastore_char myData;
  myData.variable_length = 1;
  myData.end_byte = ENDBYTE;
  myData.Millis = millis();
  myData.sensorId = sensorId;
  myData.value = sensorValue;
  dataFile.write((const uint8_t *)&myData, sizeof(myData));
  dataFile.flush();
  //Serial.write((uint8_t *)&myData, sizeof(myData));
}
void JLog::writeValue(int sensorId, char sensorValue){
  struct datastore_char myData;
  myData.variable_length = 1;
  myData.end_byte = ENDBYTE;
  myData.Millis = millis();
  myData.sensorId = sensorId;
  myData.value = sensorValue;
  dataFile.write((const uint8_t *)&myData, sizeof(myData));
  dataFile.flush();
  //Serial.write((uint8_t *)&myData, sizeof(myData));
}
void JLog::writeValue(int sensorId, short sensorValue){
  struct datastore_short myData;
  myData.variable_length = 2;
  myData.end_byte = ENDBYTE;
  myData.Millis = millis();
  myData.sensorId = sensorId;
  myData.value = sensorValue;
  dataFile.write((const uint8_t *)&myData, sizeof(myData));
  dataFile.flush();
  //Serial.write((uint8_t *)&myData, sizeof(myData));
}
void JLog::writeValue(int sensorId, double sensorValue){
  struct datastore_double myData;
  myData.variable_length = sizeof(sensorValue);
  myData.end_byte = ENDBYTE;
  myData.Millis = millis();
  myData.sensorId = sensorId;
  myData.value = sensorValue;
  dataFile.write((const uint8_t *)&myData, sizeof(myData));
  dataFile.flush();
  //Serial.write((uint8_t *)&myData, sizeof(myData));
}
void JLog::closeFile(){
  dataFile.close();
}

