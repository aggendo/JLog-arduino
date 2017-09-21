#ifndef JLog_h
#define JLog_h

class JLog
{
  public:
    JLog();
    void begin(char filename[], int sdcardId);
    void begin();
    void addSensorLogId(char id[], char Name[]);
    void addToHeader(char Name[], char Value[]);
    //void setIdByteSize(int Size);
    void writeHeader();
    void writeValue(int sensorId, int sensorValue);
    void writeValue(int sensorId, float sensorValue);
    void writeValue(int sensorId, long sensorValue);
    void writeValue(int sensorId, byte sensorValue);
    void writeValue(int sensorId, char sensorValue);
    void writeValue(int sensorId, short sensorValue);
    void closeFile();
  private:
    void writeVal(uint8_t data);
    int _IdByteSize;
    char* _filename; //TODO: 50 character limit on filename
    int _sdcardId;
    char* _headercontents;
    char* _sensorIdNames;
    #define ENDBYTE 0xff
    #define FILE_VERSION "0.1"
};
#endif
