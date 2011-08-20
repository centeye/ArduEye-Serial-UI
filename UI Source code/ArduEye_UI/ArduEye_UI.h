#ifndef ARDUEYE_UI_H
#define ARDUEYE_UI_H

#include <QWidget>
#include "commwrapper.h"
#include "Datasets.h"
#include <QElapsedTimer>

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
    Header *HeadDat;
    bool OFYRec, OFXRec, PixRefresh;

    void LoadTextFile();
    void Parse(QString inText);
    void PaintManager(int ActiveDataSet);
    void PrintImage(uchar * data, int rows, int cols);
    void PrintVectors(char *dataR, char * dataC, int rows, int cols);
    void PrintOFYBoxes(char *data, int rows, int cols);
    void PrintOFXBoxes(char *data, int rows, int cols);
    void PrintFPS(char * data);
    void PrintFPSUI(int data);
    void InitDataSets();
    int GetActiveDataSet(int inDSID);

};

#endif // ArduEyeUI_H
