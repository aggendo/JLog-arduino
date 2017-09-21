#include "JLog.h"
#include <DS3231.h>

//#define USE_SDCARD
#define LOG_USB
//#define DEBUG_USB
//#define USE_RTC

JLog logger = JLog();

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

#define SD_ID 14

void setup()
{
  // Setup Serial connection
  Serial.begin(9600);
#ifdef USE_RTC
  rtc.begin();
  // Uncomment the next line if you are using an Arduino Leonardo
  //while (!Serial) {}
#ifdef DEBUG_USB
  Serial.println("rtc set to:");
  Serial.print(rtc.getDateStr());
#endif
#endif
#ifdef USE_SDCARD
  //now lets find next availible name
  char* datestr = rtc.getDateStr();
  int i = 0;
  char* fileName = datestr;
  strcat(fileName, String(i));
  strcat(fileName, ".jlog");
  while SD.exists(fileName){
    i++;
    fileName = datestr;
    strcat(fileName, String(i));
    strcat(fileName, ".jlog");
  }
  logger.begin(fileName, SD_ID);
  logger.addToHeader("date",String(rtc.getUnixTime()));
#endif
  logger.begin();
  logger.addSensorLogId("0x01","voltage");
  logger.addSensorLogId("0x02","voltage2");
  logger.writeHeader();
}

int iii = 0;
void loop() {
  if(iii==100){
    logger.writeValue(0x01, analogRead(0));
    logger.writeValue(0x02, analogRead(1));
    delay(100);
    iii = 0;
  } else {
    iii++;
  }
  delay(10);
}
