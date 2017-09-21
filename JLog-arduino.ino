#include "JLog.h"
//#include <DS3231.h>
//#include <SPI.h>
#include<Wire.h>

const int MPU_addr=0x68;  // I2C address of the MPU-6050

int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

JLog logger = JLog();

// Init the DS3231 using the hardware interface
//DS3231  rtc(SDA, SCL);

int SD_ID = 10;

void setup()
{
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  // Setup Serial connection
  Serial.begin(9600);
  
  //rtc.begin();
  
  // Uncomment the next line if you are using an Arduino Leonardo
  //while (!Serial) {}
  
  //Serial.println("rtc set to:");
  //Serial.print(rtc.getDateStr());


  
  char* datestr = "test";//rtc.getDateStr();
  logger.begin(datestr, SD_ID);
  //TODO: fix adding date to header
  //logger.addToHeader("date",String(rtc.getTime()));
  logger.addSensorLogId("0x01","AcX");
  logger.addSensorLogId("0x02","AcY");
  logger.addSensorLogId("0x03","AcZ");
  logger.addSensorLogId("0x04","Tmp");
  logger.addSensorLogId("0x05","GyX");
  logger.addSensorLogId("0x06","GyY");
  logger.addSensorLogId("0x07","GyZ");
  logger.writeHeader();
}

void loop() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  Serial.print("AcX = "); Serial.print(AcX);
  logger.writeValue(0x01, AcX);
  logger.writeValue(0x02, AcY);
  logger.writeValue(0x03, AcZ);
  logger.writeValue(0x04, Tmp/340.00+36.53); //equation for temperature in degrees C from datasheet
  logger.writeValue(0x05, GyX);
  logger.writeValue(0x06, GyY);
  logger.writeValue(0x07, GyZ);
  delay(10);
}
