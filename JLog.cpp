#include "JLog.h"
#include "Arduino.h"
//#include <SPI.h>
#include <SD.h>
//#include <stdio.h>

short header_line; //which line is the last key:value pair in header on (not including those in 'name : {}'
short name_line; //last line with ValueId 'id:name' pair

char * _filename;


//#TODO: make variable length byte actually an byte that denotes variable type not the number of bytes the variable is i.e. (0x01 = char, 0x02 = unsigned char...) that will be in version 0.1.2 of format

struct datastore {
    long Millis;
    char sensorId;
    char variable_length; //1
    byte * value;
    char end_byte;
};

JLog::JLog(){
}

bool JLog::begin(char *filename, int sdcardId){
  boolean result = begin(filename, sdcardId, false);
  return(result); //default will find a new name that  will work.
}

bool JLog::begin(char *filename, int sdcardId, bool overWrite){
  if (!SD.begin(sdcardId)) {
     Serial.println("Card failed, or not present");
     return(false);
  }
  Serial.println("card initialized");
  if(SD.exists(filename)){
    if(!overWrite){ //we need to find the next availible filename
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
    } else {
      SD.remove(filename);//get rid of old file
    }
  }
  _filename = filename;
  if(SD.exists("tmp0")){
    SD.remove("tmp0"); //delete the old tempory file
  }
  File tmpFile = SD.open("tmp0", FILE_WRITE);
  if(!tmpFile){
    Serial.print("cannot write to file: tmp0");
    return(false);
  } else {
    tmpFile.write("<header>\r\nnames : {\r\n};\r\n</header>"); //write skeleton structure to a temporary file
    header_line = 1;
    name_line = 2;//we set these here instead of as default so you can close a logging file and open a different with same class (don't know why you would though)
    tmpFile.close();
    return(true);
  }
}

//copies everything from the temporary file to the real one
void JLog::addToFile(bool finalWrite, int line, char *Entry){
  char *tmpName; //input file
  char *finalFileName; // output file
  if(SD.exists(F("tmp0"))){
     tmpName = "tmp0";
     if(finalWrite){
      finalFileName = _filename;
     }else {
      finalFileName = "tmp1";
     }
  }else{
    if(SD.exists(F("tmp1"))){
      tmpName = "tmp1";
      if(finalWrite){
        finalFileName = _filename;
      } else {
        finalFileName = "tmp0";
      }
    } else {
      return; //the user is probably trying to edit the file after the header has been written prevent this
    }
  }
  File tmpFile = SD.open(tmpName, FILE_READ);
  if(!tmpFile){
    Serial.print("cannot write to file: ");
    Serial.println(tmpName);
    return;
  }
  File finalFile = SD.open(finalFileName, FILE_WRITE);
  if(!finalFile){
    Serial.print("cannot write to file: ");
    Serial.println(finalFileName);
    return;
  }
  // read from the file until there's nothing else in it:
  char newChar = ' ';
  int currentLine = 0;
  while (tmpFile.available()) {
    newChar = tmpFile.read();  //add next read char
    if(newChar == '\n'){        //new line found
      currentLine++;
      //we still need that carriage return, and we want it before what we are inserting
      finalFile.write('\n');
      if(currentLine==line){//we are at the destination line
        finalFile.write(Entry); //add the new shit to the file
      }
    } else {
       finalFile.write(newChar); //add the last read char from input file to output file
    }
  }
  if(line==header_line){//we were passed header_line so we need to add one to name_line also
    name_line++;
    header_line++;
  } else { //hopefully, (unless programming error) we were passed name_line
    name_line++;
  }
  tmpFile.close();
  finalFile.close();
  //remove the old tmp file
  SD.remove(tmpName);
}



//NOTE: once this is run you CANNOT edit the contents of the header
void JLog::writeHeader(){
  addToFile(true, 0, "");
}

void JLog::addToHeader(char *Name, char *ValueFor){
  char * newEntry = new char[50];//TODO: dynamically allocate from sizeof(Name) and sizeof(Value)
  sprintf(newEntry, "%s:%s;\r\n", Name, ValueFor);
  addToFile(false, header_line, newEntry);
}

void JLog::addValueId(char id, char *Name){
  char * newEntry = new char[50]; //TODO: also dynamically alocate this
  sprintf(newEntry, "%2X:%s,\r\n", id, Name); //probably should be %x:%s
  addToFile(false, name_line, newEntry);
}

bool JLog::writeValue(char sensorId, unsigned char *sensorValue){
  File dataFile = SD.open(_filename, FILE_WRITE);
  if(!dataFile){
    Serial.print("cannot write to file: ");
    Serial.println(_filename);
    return(false);
  }
  struct datastore myData;
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
#ifdef DEBUG
void JLog::serialUpload(){
  File myFile = SD.open(_filename, FILE_READ);
  if (myFile) {
    Serial.println("contents:");

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


