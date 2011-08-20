#ifndef DATASETS_H
#define DATASETS_H

#define HEADER_SIZE 8
#define DEFAULT_PACKET_SIZE 512
#define MAX_PACKET_SIZE 1010

#define ESC_CHAR    38 //0xFF

#define NULL_PROCESS -1
#define RECEIVE_PROCESS 1

// data set ids
#define DATA_ID_RAW   48
#define DATA_ID_OFX   49
#define DATA_ID_OFY   50
#define DATA_ID_FPS   51

struct DataSets{

public:
    QString name;
    int DSID; // data set ID
    int length; // current length of data set, as opposed to max length
    int width, height;
    int SizeFactor;
    char * DataArray;
    int MaxSize;

    DataSets()
    {
        SizeFactor = 1;
    }

    void InitDataArray(int MaxLength)
    {
        DataArray = new char[MaxLength*SizeFactor];
        for(int i = 0; i < MaxLength; i++)
            DataArray[i] = 0;
        MaxSize = MaxLength*SizeFactor;
        length = MaxLength;
    }
    void Clear()
    {
        for(int i = 0; i < length; i++)
            DataArray[i] = 0;
    }
    void Delete()
    {
        delete DataArray;
    }
};

//Header: 2bytes pckt size, 1 byte DataId, 1 byte row, 1 byte cols,  2Bytes dataIndex, 1 Byte EODS
struct Header {

public:
    int HeaderSize;
    int DSID, Process;
    int PacketSize, DataSize;
    unsigned int DSIdx, DSRows, DSCols;
    bool EODS;

    Header(int Size)
    {
        HeaderSize = Size;
        EODS = false;
        DSIdx = 0;
        PacketSize = DEFAULT_PACKET_SIZE;
        Process = NULL_PROCESS;
    }
    void Parse(char * InDat)
    {
        DSID = (int)InDat[0];
        DSRows = ((unsigned char)InDat[1] << 8) + (unsigned char)InDat[2];
        DSCols = ((unsigned char)InDat[3] << 8) + (unsigned char)InDat[4];
        DSIdx = ((unsigned char)InDat[5] << 8) + (unsigned char)InDat[6];
        EODS = (InDat[7] > 0) ? true : false;

    }
    void SetPacketSize(int Size)
    {
        PacketSize = Size;
        DataSize = PacketSize - HeaderSize;
    }

};

#endif // DATASETS_H
