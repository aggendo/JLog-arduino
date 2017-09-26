#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SD.h>

char * filename = new char[11+3+4];
char * _filename = new char[11+3+4];

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(7, 6);

char * datestr= new char[11];

void setup() {
  ss.begin(9600);
  Serial.begin(9600);

  if (!SD.begin(10)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }

  //lets get a file name from the date and time
  bool gotdate = false;
  pinMode(5, OUTPUT);
  pinMode(3, OUTPUT);
  while (millis() < 5000){ //try for 5 seconds to get gps for date for filename
    while (ss.available() > 0){
      if (gps.encode(ss.read())){
        if(gps.date.isValid() && gps.time.isValid()){
          yearend = gps.date.year() - 2000; //get the two digit year
          sprintf(datestr, "%02d-%02d-%02d", yearend, gps.date.month(), gps.date.day());//gps.date.year()
          gotdate = true;
          int ie = 0;
          Serial.println(datestr);
          sprintf(filename,"%s_%d.jlg",datestr, ie);
          while(SD.exists(filename)){
            ie++;
            sprintf(filename, "%s_%d.jlg",datestr, ie);
          }
          _filename = filename;
          Serial.println(filename);
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
  }
  if(SD.exists(filename)){
    SD.remove(filename);
  }
  writeValue(0x01, 109);
}

void loop()
{
  delay(10);
  // This sketch displays information every time a new sentence is correctly encoded.
  while (ss.available() > 0){
    if (gps.encode(ss.read())){
      if (gps.location.isValid())
      {
        digitalWrite(5, HIGH);
        writeValue(gps.location.lng(), gps.location.lat());
        //writeValue(0x02, );
        digitalWrite(5, LOW);
      }
    }
  }
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
}

char * strValLG = new char[11];
char * strValLT = new char[11];
char * Entry = new char[26];

void writeValue(double Long, double Lat){
  File dataFile;
  
  dataFile = SD.open(_filename, FILE_WRITE);//filename, FILE_WRITE);
  if(!dataFile){
    Serial.print("cannot write to file: ");
    Serial.println(_filename);
    return;
  }
  dtostrf(Long, 7, 6, strValLT);
  dtostrf(Lat,  7, 6, strValLG);
  char s[20];
  sprintf(s, "%u", millis());
  dataFile.write(s);
  dataFile.write(":{");
  dataFile.write(strValLG);
  dataFile.write(",");
  dataFile.write(strValLT);
  dataFile.write("}\r\n");
  dataFile.flush();
  dataFile.close();
}
