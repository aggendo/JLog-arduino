#ifndef JLog_h
#define JLog_h

class JLog
{
  public:
    JLog();
    bool begin(char *filename, char sdcardId, bool overWrite);
    bool begin(char *filename, char sdcardId);
    void addSensorLogId(char id, char *Name);
    void addToHeader(char *Name, char *Value);
    //void setIdByteSize(int Size);
    bool writeHeader();
    bool writeValue(char sensorId, int sensorValue);
    bool writeValue(char sensorId, float sensorValue);
    bool writeValue(char sensorId, long sensorValue);
    bool writeValue(char sensorId, char sensorValue);
    bool writeValue(char sensorId, short sensorValue);
    bool writeValue(char sensorId, double sensorValue);
  private:
    int _IdByteSize;
    char* _filename; //TODO: 50 character limit on filename
    int _sdcardId;
    char* _headercontents;
    char* _sensorIdNames;
    #define ENDBYTE 0xff
    #define FILE_VERSION "0.1"
};
#endif
