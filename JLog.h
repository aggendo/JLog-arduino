#ifndef JLog_h
#define JLog_h

#define DEBUG

class JLog
{
  public:
    JLog();
    bool begin(char filename[], int sdcardId, bool overWrite);
    bool begin(char filename[], int sdcardId);
    void writeHeader();
    //void setIdByteSize(int Size);
    bool writeValue(char const sensorId, int sensorValue);
    bool writeValue(char const sensorId, float sensorValue);
    bool writeValue(char const sensorId, long sensorValue);
    bool writeValue(char const sensorId, char sensorValue);
    bool writeValue(char const sensorId, short sensorValue);
    bool writeValue(char const sensorId, double sensorValue);
    bool writeValue(char const sensorId, unsigned long sensorValue);
    #ifdef DEBUG
    void serialUpload(char const * fileName); //for debugging uploads file through serial
    #endif
  private:
    int _IdByteSize;
    //in the below method finalWrite denotes that the output file is _filename
    void addToFile(int line, char* Entry);
    void copytmp1(char* source, char* destination);
    #define ENDBYTE 0xff
    #define FILE_VERSION "0.1"
};
#endif

