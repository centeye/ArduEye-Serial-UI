/*
  ArduEye_UI.h - Event processing functions for the ArduEye UI
 Centeye, Inc
 Created by Alison Leonard. August, 2011

 ===============================================================================
 Copyright (c) 2011, Centeye, Inc.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 * Neither the name of Centeye, Inc. nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL CENTEYE, INC. BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ===============================================================================
*/

#ifndef ARDUEYE_UI_H
#define ARDUEYE_UI_H

#include <QWidget>
#include "commwrapper.h"
#include "Datasets.h"
#include <QElapsedTimer>
#include <QFile>

// text display locations
#define TEXT_XLOC 10
#define TEXT_YLOC 20

// min max functions
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
    // constructor / destructor
    explicit ArduEyeUI(QWidget *parent = 0);
    ~ArduEyeUI();

private slots:
    //Serial device found
    void onDeviceDiscovered(const QextPortInfo &newport);
    //Serial device removed
    void onDeviceRemoved(const QextPortInfo &newport);
    // connect to serial device button
    void on_ConnectButton_clicked();
    // serial data is available (via qextserial library)
    void onDataAvailable();
    // command entered
    void on_CmdEdit_returnPressed();
    // start file record
    void on_FileRecordButton_clicked();
    // check if Cmd Ack has been received (slot driven by a timer)
    void CheckCmdReceived();
    // command entered
    void on_enterButton_clicked();

private:
    // self
    Ui::ArduEyeUI *ui;
    // wrapper for qextserial library
    CommWrapper *comm;
    // list of commands (filled via parameters txt file)
    QStringList CmdList;
    // List of Command IDs (CmdList and CmdIndex together are the key for parsing text entered in the UI)
    QByteArray CmdIndex;
    // Main image display array
    QPixmap ImagePixMap;
    // Datasets structure, keeps dataset arrays and settings
    DataSets *DS;
    // number of possible datasets (defined by parameters txt file)
    int NumDataSets;
    // Buffer for reading in serial bytes
    char * DataBuffer;
    // flags for parsing incoming serial bytes
    int DataBufferSize, StartIdx, BufEndIdx, DataIdx;
    // flag for recording data
    bool FileRecordOn;
    // flag for parsing incoming serial bytes
    bool ESCReceived;
    // file for recording data
    QFile FileSave;
    // flag that cmd was received by Arduino
    bool CmdReceived;

    //FUNCTIONS
    //parse serial packets (process incoming data and commands)
    void ParsePacket(int Start, int End);
    // load parameters txt file and parse
    void LoadTextFile();
    // parse commands in Cmd Text Line and send to ArduEye
    void Parse(QString inText);
    // init datasets using parameters txt file
    void InitDataSets();
    // find index of dataset in the Datasets structure
    int GetActiveDataSet(int inDSID);
    // record active datsets to open file
    void RecordtoFile();

    //PRINT FUNCTIONS
    void PrintImage(uchar * data, int rows, int cols);
    void PrintVectors(char *dataR, char * dataC, int rows, int cols, QPixmap * PM);
    void PrintOFYBoxes(char *data, int rows, int cols);
    void PrintOFXBoxes(char *data, int rows, int cols);
    void PrintText(char * data, int xloc, int yloc, QPixmap * PM);
    void PrintPoints(char *data, int NumPoints, QPixmap * PM);
    // print functions manager, called after End of Frame flag received
    void paintManager();


};

#endif // ArduEyeUI_H
