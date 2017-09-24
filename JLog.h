#ifndef JLog_h
#define JLog_h

#define DEBUG

class JLog
{
  public:
    JLog();
    bool begin(char *filename, int sdcardId, bool overWrite);
    bool begin(char *filename, int sdcardId);
    void writeHeader();
    void addValueId(char id, char *Name);
    void addToHeader(char *Name, char *ValueFor);
    //void setIdByteSize(int Size);
    bool writeValue(char sensorId, unsigned char *sensorValue);
    //bool writeValue(char sensorId, float sensorValue);
    //bool writeValue(char sensorId, long sensorValue);
    //bool writeValue(char sensorId, char sensorValue);
    //bool writeValue(char sensorId, short sensorValue);
    //bool writeValue(char sensorId, double sensorValue);
    #ifdef DEBUG
    void serialUpload(); //for debugging uploads file through serial
    #endif
  private:
    //in the below method finalWrite denotes that the output file is _filename
    void addToFile(bool finalWrite, int line, char* Entry);
    #define ENDBYTE 0xff
    #define FILE_VERSION "0.1"
};
#endif

