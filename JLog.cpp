#include "JLog.h"
#include "Arduino.h"
#include <SPI.h>
#include <SD.h>

short header_line; //which line is the last key:value pair in header on (not including those in 'name : {}'
short name_line; //last line with ValueId 'id:name' pair

//#TODO: make variable length byte actually an byte that denotes variable type not the number of bytes the variable is i.e. (0x01 = char, 0x02 = unsigned char...) that will be in version 0.1.2 of format

PROGMEM struct datastore {
    long Millis;
    char sensorId;
    char variable_length; //1
    uint8_t *value;
    char end_byte;
};

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
      Serial.println(actualName);
      int file = 1;
      sprintf(filename, "%s_%02d.jlg", actualName, file);
      do
        {
         file = file + 1;
         sprintf(filename, "%s_%02d.jlg", actualName, file);
         Serial.println(filename);
        } while (SD.exists(filename));
    } else {
      SD.remove(filename);//get rid of old file
    }
  }
  strcpy(_filename, filename);
  Serial.print(F("Logging to file: "));
  Serial.println(_filename);
  if(SD.exists("tmp0")){
    SD.remove("tmp0"); //delete the old tempory file
  }
  File tmpFile = SD.open("tmp0", FILE_WRITE);
  if(!tmpFile){
    Serial.print(F("cannot write to file: tmp0"));
    return(false);
  } else {
    tmpFile.write("<header>\r\nnames : {\r\n};\r\n</header>");//skeleton); //write skeleton structure to a temporary file
    header_line = 1;
    name_line = 2;//we set these here instead of as default so you can close a logging file and open a different with same class (don't know why you would though)
  }
  tmpFile.close();
  return(true);
}

//copies everything from the temporary file to the real one
void JLog::addToFile(int line, char *Entry){
  File tmpFile = SD.open("tmp0", FILE_READ);
  if(!tmpFile){
    Serial.print(F("cannot read file: "));
    Serial.println("tmp0");
    return;
  }
  File finalFile  = SD.open("tmp1", FILE_WRITE);
  if(!finalFile){
    Serial.print(F("cannot stash to file: "));
    Serial.println("tmp1");
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
  delay(20); //let the sd catch up
  //remove the old tmp file
  SD.remove("tmp0");
  delay(20); //let the sd catch up
  //NOW because of a lack of fucking memory or copy function we need to copy it back ourselves
  copytmp1("tmp1","tmp0");
}

//our own copy function cause fuck you SD lib
void JLog::copytmp1(char* source, char* destination){
  Serial.print("source ");
  Serial.println(source);
  Serial.print("destination ");
  Serial.println(destination);
  File tmpFile = SD.open(source, FILE_READ);
  if(!tmpFile){
    Serial.print(F("cannot read filed: "));
    Serial.println(source);
    return;
  }
  File finalFile  = SD.open(destination, FILE_WRITE);
  if(!finalFile){
    Serial.print(F("cannot copy to file: "));
    Serial.println(destination);
    return;
  }
  //copy over the file one byte at a time
  while (tmpFile.available()) {
    finalFile.write(tmpFile.read());  
  }
  tmpFile.close();
  finalFile.close();
  delay(20); //let the sd catch up
  SD.remove(source);
}



//NOTE: once this is run you CANNOT edit the contents of the header
void JLog::writeHeader(){
  copytmp1("tmp0",_filename);
}


void JLog::addToHeader(char const *Name, char const *ValueFor){
  char * newEntry = new char[20];//TODO: dynamically allocate from sizeof(Name) and sizeof(Value)
  sprintf(newEntry, "%s:%s;\r\n", Name, ValueFor);
  addToFile(header_line, newEntry);
  delay(800);
}

void JLog::addValueId(char const id, char const *Name){
  char * newEntry = new char[20]; //TODO: also dynamically alocate this
  sprintf(newEntry, "%2X:%s,\r\n", id, Name); //probably should be %x:%s
  addToFile(name_line, newEntry);
  delay(800);
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
  myData.variable_length = sizeof(sensorValue);
  myData.end_byte = ENDBYTE;
  myData.Millis = millis();
  myData.sensorId = sensorId;
  myData.value = (const byte *) sensorValue;
  dataFile.write((const uint8_t *)&myData, sizeof(myData));
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
  myData.variable_length = 4;
  myData.end_byte = ENDBYTE;
  myData.Millis = millis();
  myData.sensorId = sensorId;
  myData.value = (const byte *) sensorValue;
  dataFile.write((const uint8_t *)&myData, sizeof(myData));
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
  myData.variable_length = 1;
  myData.end_byte = ENDBYTE;
  myData.Millis = millis();
  myData.sensorId = sensorId;
  myData.value = (const byte) sensorValue;
  dataFile.write((const uint8_t *)&myData, sizeof(myData));
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
  myData.variable_length = 2;
  myData.end_byte = ENDBYTE;
  myData.Millis = millis();
  myData.sensorId = sensorId;
  myData.value = (const byte *) sensorValue;
  dataFile.write((const uint8_t *)&myData, sizeof(myData));
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
  myData.variable_length = sizeof(sensorValue);
  myData.end_byte = ENDBYTE;
  myData.Millis = millis();
  myData.sensorId = sensorId;
  myData.value = (const uint8_t) sensorValue;
  dataFile.write((const uint8_t *)&myData, sizeof(myData));
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


