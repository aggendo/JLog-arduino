#include "JLog.h"

JLog logger = JLog();

#define SD_CS 10
//fake comment

void setup(){
  Serial.begin(9600);
  
  logger.begin("test.jlg", SD_CS, true);
  logger.addToHeader("example","value");
  logger.addToHeader("more stuff","damn I farted");
  logger.addValueId(0x01,"pork");
  logger.addValueId(0x02,"beans");
  logger.writeHeader();
  logger.serialUpload();
}

void loop(){
  
}


