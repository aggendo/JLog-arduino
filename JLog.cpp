#include "JLog.h"
#include "Arduino.h"
#include <SPI.h>
#include <SD.h>

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

JLog::JLog(){ 
}
bool JLog::begin(char *filename, char sdcardId){
  boolean result = begin(filename, sdcardId, false);
  return(result); //default will find a new name that  will work.
}

bool JLog::begin(char *filename, char sdcardId, bool overWrite){
  if (!SD.begin(sdcardId)) {
     Serial.println("Card failed, or not present");
     return(false);
  }
  Serial.println("card initialized");
  if(!overWrite && SD.exists(filename)){ //we need to find the next availible filename
    //now lets find next availible name
    char * actualName = strtok_r(filename, ".", &filename);
    Serial.print("finding next availible name for file of name: ");
    Serial.println(actualName);
    int file = 1;
    sprintf(filename, "%s_%02d.jlg", actualName, file);
    do
      {
       file = file + 1;
       sprintf(filename, "%s_%02d.jlg", actualName, file);
      } while (SD.exists(filename));
  }
  _filename = filename;
  File dataFile = SD.open(filename, FILE_WRITE);
  if(!dataFile){
    Serial.print("cannot write to file: ");
    Serial.println(filename);
    return(false);
  } else {
    dataFile.close();
    return(true);
  }
}

//NOTE: once the header is written, this method (and addSensorLogId will no longer function)
void JLog::addToHeader(char Name[], char Value[]){
  sprintf(header, "%s\n\r%s:%s;", header, Name, Value);
}

void JLog::addSensorLogId(char id, char * Name){
  sprintf(sensorNames, "%s,\n\r%02X:%s", sensorNames, Name, id);
  Serial.println(sensorNames);
}
//NOTE: once this is written it is FINAL do not run again, the header cannot be modified again in this version of the code.
bool JLog::writeHeader(){
  char* headerfinal;
  File dataFile = SD.open(_filename, FILE_WRITE);
  if(!dataFile){
    Serial.print("cannot write to file: ");
    Serial.println(_filename);
    return false;
  }
  sprintf(headerfinal, "<header>\n\r%s;\n\r names: {\n\r %s};\n\r</header>", header, sensorNames);
  dataFile.print(headerfinal);
  dataFile.close();
  return true;
}

//Below is not good programming but it is more efficiant than a proper modular design
//Sadly this is the best way of writing it for speed

bool JLog::writeValue(char sensorId, int sensorValue){
  File dataFile = SD.open(_filename, FILE_WRITE);
  if(!dataFile){
    Serial.print("cannot write to file: ");
    Serial.println(_filename);
    return(false);
  }
  struct datastore_int myData;
  myData.variable_length = sizeof(sensorValue);
  myData.end_byte = ENDBYTE;
  myData.Millis = millis();
  myData.sensorId = sensorId;
  myData.value = sensorValue;
  dataFile.write((const uint8_t *)&myData, sizeof(myData));
  dataFile.flush();
  dataFile.close();
  return(true);
}

bool JLog::writeValue(char sensorId, long sensorValue){
  File dataFile = SD.open(_filename, FILE_WRITE);
  if(!dataFile){
    Serial.print("cannot write to file: ");
    Serial.println(_filename);
    return(false);
  }
  struct datastore_long myData;
  myData.variable_length = 4;
  myData.end_byte = ENDBYTE;
  myData.Millis = millis();
  myData.sensorId = sensorId;
  myData.value = sensorValue;
  dataFile.write((const uint8_t *)&myData, sizeof(myData));
  dataFile.flush();
  dataFile.close();
  return(true);
}

bool JLog::writeValue(char sensorId, char sensorValue){
  File dataFile = SD.open(_filename, FILE_WRITE);
  if(!dataFile){
    Serial.print("cannot write to file: ");
    Serial.println(_filename);
    return(false);
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
  return(true);
}
bool JLog::writeValue(char sensorId, short sensorValue){
  File dataFile = SD.open(_filename, FILE_WRITE);
  if(!dataFile){
    Serial.print("cannot write to file: ");
    Serial.println(_filename);
    return(false);
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
  return(true);
}
bool JLog::writeValue(char sensorId, double sensorValue){
  File dataFile = SD.open(_filename, FILE_WRITE);
  if(!dataFile){
    Serial.print("cannot write to file: ");
    Serial.println(_filename);
    return(false);
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
  return(true);
}
