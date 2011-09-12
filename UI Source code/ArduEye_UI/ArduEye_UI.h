#ifndef ARDUEYE_UI_H
#define ARDUEYE_UI_H

#include <QWidget>
#include "commwrapper.h"
#include "Datasets.h"
#include <QElapsedTimer>
#include <QFile>

#define TEXT_XLOC 10
#define TEXT_YLOC 20

template <class T> const T& max ( const T& a, const T& b ) {
  return (b<a)?a:b;
}
template <class T> const T& min ( const T& a, const T& b ) {
  return (b<a)?a:b;
}

namespace Ui {
    class ArduEyeUI;
}

class ArduEyeUI : public QWidget
{
    Q_OBJECT

public:
    explicit ArduEyeUI(QWidget *parent = 0);
    ~ArduEyeUI();

private slots:
    void on_findButton_clicked();
    void onDeviceDiscovered(const QextPortInfo &newport);
    void onDeviceRemoved(const QextPortInfo &newport);
    void on_ConnectButton_clicked();
    void onDataAvailable();
    void on_CmdEdit_returnPressed();
    void paintManager();
    void on_FileRecordButton_clicked();
    void CheckCmdReceived();

signals:
    void paintReady();

private:
    Ui::ArduEyeUI *ui;
    CommWrapper *comm;
    QElapsedTimer Timer;
    QStringList CmdList;
    QByteArray CmdIndex;
    QPixmap ImagePixMap;
    //char ImageDat[1024];
    char AckCmd[3];
    DataSets *DS;
    int NumDataSets;
    bool PixRefresh;
    char * DataBuffer;
    int DataBufferSize, StartIdx, BufEndIdx, DataIdx;
    bool FileRecordOn;
    bool ESCReceived;
    QFile FileSave;
    bool CmdReceived;

    void ParsePacket(int Start, int End);
    void LoadTextFile();
    void Parse(QString inText);
    void PrintImage(uchar * data, int rows, int cols);
    void PrintVectors(char *dataR, char * dataC, int rows, int cols, QPixmap * PM);
    void PrintOFYBoxes(char *data, int rows, int cols);
    void PrintOFXBoxes(char *data, int rows, int cols);
    void PrintText(char * data, int xloc, int yloc, QPixmap * PM);
    void PrintPoints(char *data, int NumPoints, QPixmap * PM);
    void InitDataSets();
    int GetActiveDataSet(int inDSID);
    void RecordtoFile();


};

#endif // ArduEyeUI_H
