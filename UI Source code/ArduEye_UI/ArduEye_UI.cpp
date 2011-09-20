/*
  ArduEye_UI.cpp - Event processing functions for the ArduEye UI
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

#include "ArduEye_UI.h"
#include "ui_ArduEyeUI.h"
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtDebug>
#include <QPainter>
#include <QColor>
#include <QFileDialog>
#include <QTimer>

QVector<QRgb> ColorMap(256);

/*---------------------------------------------------------------
  ArduEyeUI: constructor
 ---------------------------------------------------------------*/
ArduEyeUI::ArduEyeUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ArduEyeUI),
    comm(new CommWrapper)
{
    ui->setupUi(this);
    // setup comm port
    comm->Enumerate(ui->PortSelect);

    // initialize serial device discovered signals and slots
    connect(comm->enumerator, SIGNAL(deviceDiscovered(const QextPortInfo &)),
               this, SLOT(onDeviceDiscovered(const QextPortInfo &)));
    connect(comm->enumerator, SIGNAL(deviceRemoved(const QextPortInfo &)),
               this, SLOT(onDeviceRemoved(const QextPortInfo &)));
   // connect(this, SIGNAL(paintReady(void)), this, SLOT(PaintManager()), Qt::QueuedConnection);

    // Command list and command history boxes are read only
    ui->textEdit->setReadOnly(true);
    ui->CmdListEdit->setReadOnly(true);

    // initialize data parsing variables
    DataBufferSize = StartIdx = BufEndIdx = DataIdx = 0;
    NumDataSets = 0;
    // load parameters txt file and initialize datasets
    LoadTextFile();
    InitDataSets();

    // init grayscale colormap
    for(int i=0;i<256;++i)
      ColorMap[i] = qRgb(i,i,i);

    // init ImagePixMap to black
    QPixmap PMBlack(ui->imagelabel->width(), ui->imagelabel->height());
    PMBlack.fill(Qt::black);
    ImagePixMap = PMBlack;

    // init serial input parsing flags
    ESCReceived = false;
    FileRecordOn = false;
    CmdReceived = false;
}

/*---------------------------------------------------------------
  ~ArduEyeUI : desctructor
 ---------------------------------------------------------------*/
ArduEyeUI::~ArduEyeUI()
{
    for(int i = 0; i < NumDataSets; i++)
        DS[i].Delete();
    delete DataBuffer;
    delete ui;
}

/*---------------------------------------------------------------
  onDeviceDiscovered: slot is called when a new serial device is found
  the new signal is added to the serial device list combo box
  Input: newport - new serial port name (signal from qextserial library)
 ---------------------------------------------------------------*/
void ArduEyeUI::onDeviceDiscovered(const QextPortInfo &newport)
{
    // add port to combo box
    ui->PortSelect->addItem(newport.portName);
}

/*---------------------------------------------------------------
  onDeviceRemoved: slot is called when a serial device is removed
  the signal is removed from the serial device list combo box
  Input: newport - lost serial port name (signal from qextserial library)
 ---------------------------------------------------------------*/
void ArduEyeUI::onDeviceRemoved(const QextPortInfo &newport)
{
    //get port name
    int Idx = ui->PortSelect->findText(newport.portName, Qt::MatchExactly);
    //remove port name
    ui->PortSelect->removeItem(Idx);
}

/*---------------------------------------------------------------
  on_ConnectButton_clicked: open serial part that is selected by the
  serial port list combo box.  Report whether connection was successful
 ---------------------------------------------------------------*/
void ArduEyeUI::on_ConnectButton_clicked()
{
    // open port
    bool success = comm->Open(ui->PortSelect->currentText());
    // report success or failure
    if(success)
    {
        // connect readyread signal for this port so that incoming data will call onDataAvailable()
        connect(comm->port, SIGNAL(readyRead()), this, SLOT(onDataAvailable()));
        ui->textEdit->append("Device Connected Successfully");
    }
    else
        ui->textEdit->append("Device Connect Failed");
}

/*---------------------------------------------------------------
  onDataAvailable: slot called when new data is available on the serial port
  data is read into a buffer and scanned for communications flag bytes (ESC_CHAR, START_PCKT etc)
 ---------------------------------------------------------------*/
void ArduEyeUI::onDataAvailable()
{
    int numbytes;
    QString num;
    numbytes = comm->port->bytesAvailable();

    // clear incoming bytes if the bytes number is greater than the maximum packet buffer
    if(numbytes >= MAX_PACKET_SIZE)
    {
       QByteArray temp = comm->port->readAll();
        return;
    }

    // reset DataBuffer index to zero if the incoming bytes do
    // fit into the end of buffer
    if(numbytes > DataBufferSize - DataIdx)
    {
        //last valid data index in buffer recorded for later processing
        BufEndIdx = DataIdx;
        //clear DataIdx
        DataIdx = 0;
    }
    // read incoming bytes into buffer
    comm->port->read(DataBuffer + DataIdx, numbytes);

    //scan bytes for ESC_CHAR commands
    for (int i = 0; i < numbytes; i++)
    {
        // if ESC_CHAR was received, check if the next byte is a command
        // ESC_CHARs that are not part of commands are duplicated
        if(ESCReceived)
        {
            switch(DataBuffer[DataIdx + i])
            {
            // when END_PCKT is received, parse data packet
            case END_PCKT:
                ParsePacket(StartIdx, DataIdx + i - 1);
                break;
             // GO_CHAR is the flow control byte
             // an Ack is sent in response to indicate that the serial buffer is not full
            case GO_CHAR:
                comm->SendAck();
                break;
            // record location of start packet
            case START_PCKT:
                StartIdx = DataIdx + i;
                break;
            // set CmdRecieved flag when CMD_ACK is recieved
            // if CMD_ACK is not received, command will be resent
            case CMD_ACK:
                CmdReceived = true;
                break;
            default:
                break;
            }
            ESCReceived = false;
        }
        // Set ESCRecieved flag when ESC_CHAR is received
        else if(DataBuffer[DataIdx + i] == ESC_CHAR)
            ESCReceived = true;
    }
    DataIdx += numbytes;

}

/*---------------------------------------------------------------
  ParsePacket: Process Data Packets, Store data in DataSet structures
  Input:  Start: index of START_PCKT
          End: index of END_PCKT
 ---------------------------------------------------------------*/
void ArduEyeUI::ParsePacket(int Start, int End)
{
    int DataID = DataBuffer[Start + 1];
    int PacketType, DIdx, MaxSize, Idx;
    char *Array;

    // if END_FRAME packet is received, display datasets and record data if requested
    if(DataID == END_FRAME)
    {
        RecordtoFile();
        paintManager();
        return;
    }

    // ToDo Fix to work in (super rare) case where Start Packt equal to BufEndIdx
    if(Start == BufEndIdx)
        qDebug() << "START == BUFEND!!!!!!!!";

    // odd dataIDs are headers
    // check for header ID and process header
    if(DataID % 2  != 0)
    {
        PacketType = PCKT_HEADER;
        DIdx = GetActiveDataSet(DataID-1);
        Array = DS[0].DataArray;
        MaxSize = DS[0].MaxSize;
    }
    else
    {
        PacketType = PCKT_DATA;
        DIdx = GetActiveDataSet(DataID);
        Array = DS[DIdx].DataArray;
        MaxSize = DS[DIdx].MaxSize;
    }

    // Fill Data Array, checking for escape characters (escape characters are repeated in normal data
    // and duplicates must be removed)
    if(Start  > End)
    {
        Idx = 0;
        // if start > end, the packet wraps around the end of the buffer and must be read out accordingly
        for(int i = Start + 2; i < BufEndIdx; i++)
        {
            // remove duplicates if ESC_CHAR is found
            if(DataBuffer[i]  == ESC_CHAR)
            {
               i++;
               // GO_CHAR or CMD_ACK could also be after ESC_CHAR, skip these, they are not data
               while((DataBuffer[i] == GO_CHAR) || (DataBuffer[i] == CMD_ACK))
                {
                    i++;
                    // GO_CHAR can be repeated a few times and all should be skipped
                    if(DataBuffer[i] == ESC_CHAR)
                        i++;
                    else
                        break;
                 }
            }
            Array[Idx++] = DataBuffer[i];
            // make sure we are not writing over the end of the array
            if(Idx > MaxSize)
            {
                BufEndIdx = 0;
                return;
            }
        }
        for(int i = 0; i < End; i++)
        {
            // remove duplicates if ESC_CHAR is found
            if(DataBuffer[i]  == ESC_CHAR)
            {
               i++;
               // GO_CHAR or CMD_ACK could also be after ESC_CHAR, skip these, they are not data
               while((DataBuffer[i] == GO_CHAR) || (DataBuffer[i] == CMD_ACK))
                {
                    i++;
                    // GO_CHAR can be repeated a few times and all should be skipped
                    if(DataBuffer[i] == ESC_CHAR)
                        i++;
                    else
                        break;
                 }
            }
            Array[Idx++] = DataBuffer[i];
            // make sure we are not writing over the end of the array
            if(Idx > MaxSize)
            {
                BufEndIdx = 0;
                return;
            }
        }
    }
    // start < end, data read out in direct sequence
    else
    {
        Idx = 0;
        for(int i = Start + 2; i < End; i++)
        {
            // remove duplicates if ESC_CHAR is found
            if(DataBuffer[i]  == ESC_CHAR)
            {
               i++;
               // GO_CHAR or CMD_ACK could also be after ESC_CHAR, skip these, they are not data
               while((DataBuffer[i] == GO_CHAR) || (DataBuffer[i] == CMD_ACK))
                {
                    i++;
                    // GO_CHAR can be repeated a few times and all should be skipped
                    if(DataBuffer[i] == ESC_CHAR)
                        i++;
                    else
                        break;
                 }
            }
            Array[Idx++] = DataBuffer[i];
            // make sure we are not writing over the end of the array
            if(Idx > MaxSize)
                return;
        }
    }
    // store header data info
    if(PacketType == PCKT_HEADER)
    {
        // check checksum
        char CheckSum = DataID;
        for(int k = 0; k < 4; k++)
            CheckSum += DS[0].DataArray[k];
        // process header data if Checksum matches
        if(CheckSum == DS[0].DataArray[4])
        {
            DS[DIdx].height = (DS[0].DataArray[0] << 8) + DS[0].DataArray[1];
            DS[DIdx].width = (DS[0].DataArray[2] << 8) + DS[0].DataArray[3];
            DS[DIdx].DisplayType = DS[0].DataArray[5];
            DS[DIdx].length = DS[DIdx].height * DS[DIdx].width;
        }
    }
    // set data received flag for non-header data
    else
        DS[DIdx].DataReceived = true;
}

/*---------------------------------------------------------------
  GetActiveDataSet: Get index of dataset in the DataSets structure
  Input:    inDSID - Dataset ID (received from data or header packet)
 ---------------------------------------------------------------*/
int ArduEyeUI::GetActiveDataSet(int inDSID)
{
   int Idx = 0;

   //cycle through Datasets structure and look for DSID match
   while((DS[Idx].DSID != inDSID) && (Idx < NumDataSets))
       Idx++;

   // return index of match
   if(Idx < NumDataSets)
       return Idx;
   else
       return 0;
}

/*---------------------------------------------------------------
  RecordtoFile: Record active datasets if Record is set active
 ---------------------------------------------------------------*/
void ArduEyeUI::RecordtoFile()
{
    if(FileRecordOn)
    {
        for(int n = 0; n < NumDataSets; n++)
        {
            // if DataReceived set to true, record data
            if(DS[n].DataReceived)
            {
                // write data info to file
                char DataInfo[] = {DS[n].DSID, DS[n].width, DS[n].height};
                FileSave.write(DataInfo, 3);

                //write data to file
                FileSave.write(DS[n].DataArray, DS[n].length);
                qDebug() << FileSave.pos() << DS[n].length;
            }
        }
    }
}

/*---------------------------------------------------------------
  paintManager:  manages calling display functions
 ---------------------------------------------------------------*/
void ArduEyeUI::paintManager()
{
    int xID, yID;
    bool ImageProcessed = false;
    int yloc = TEXT_YLOC;

    // Fill Base Image
    for (int n = 0; n < NumDataSets; n++)
    {
        if((DS[n].DisplayType == DISPLAY_GRAYSCALE_IMAGE) && (DS[n].DataReceived))
        {
            PrintImage((uchar *)(DS[n].DataArray), DS[n].height, DS[n].width);
            ImageProcessed = true;
            DS[n].DataReceived = false;
        }
    }
    // if no rawImage, make a full black pixmap
    if(!ImageProcessed)
    {
            QPixmap PMBlack(ui->imagelabel->width(), ui->imagelabel->height());
            PMBlack.fill(Qt::black);
            ImagePixMap = PMBlack;
    }

    // fill chart images
    for (int n = 0; n < NumDataSets; n++)
    {
        if((DS[n].DisplayType == DISPLAY_CHARTS) && (DS[n].DataReceived))
        {
            int cols = DS[n].width/2;
            int size = DS[n].height * cols;
            PrintOFXBoxes(DS[n].DataArray, DS[n].height, cols);
            PrintOFYBoxes(DS[n].DataArray + size, DS[n].height, cols);
            PrintVectors(DS[n].DataArray+size, DS[n].DataArray, DS[n].height, cols, &ImagePixMap);
            DS[n].DataReceived = false;
        }
    }
    // print text images
    for (int n = 0; n < NumDataSets; n++)
    {
        if((DS[n].DisplayType == DISPLAY_TEXT) && (DS[n].DataReceived))
        {
            PrintText(DS[n].DataArray, TEXT_XLOC, yloc, &ImagePixMap);
            DS[n].DataReceived = false;
            yloc += 20;
        }
    }
    // print text to Data Dump /Cmd History Text Box
    for (int n = 0; n < NumDataSets; n++)
    {
        if((DS[n].DisplayType == DISPLAY_DUMP) && (DS[n].DataReceived))
        {
            QString num;
            QString cmd = CmdList[CmdIndex.indexOf(DS[n].DataArray)] ;
            ui->textEdit->append("cmd" + cmd + ": "  + num.setNum(DS[n].DataArray[1]));
            DS[n].DataReceived = false;
        }
    }
    // print feature point data
    for (int n = 0; n < NumDataSets; n++)
    {
        if((DS[n].DisplayType == DISPLAY_POINTS) && (DS[n].DataReceived))
        {
            PrintPoints(DS[n].DataArray, DS[n].length, &ImagePixMap);
            DS[n].DataReceived = false;
        }
    }
    // print pixmap to image box
    ui->imagelabel->setPixmap(ImagePixMap);
}

/*---------------------------------------------------------------
  PrintText
  Input:  data (format data[0] = length, data[1:length-1] data, null terminated title string
          xloc : text location in x pixels
          yloc : text location in y pixels
          PM  Pixmap that text will be printed on
 ---------------------------------------------------------------*/
void ArduEyeUI::PrintText(char * data, int xloc, int yloc, QPixmap * PM)
{
    int size = data[0];
    int text = 0, idx = 1;
    for(int i = size-1; i >= 0; i--)
    {
        text += ((unsigned char)data[idx] << (i * 8));
        idx++;
    }
    QString fpsString, num;
    fpsString.append(data + size + 1);
    fpsString.append(num.setNum(text));
    QPoint TopLeft(xloc, yloc);

    QPainter painter(PM);
    QPen pen(Qt::red, 2);
    painter.setPen(pen);
    painter.drawText(TopLeft, fpsString);
}

/*---------------------------------------------------------------
  PrintImage print grayscale image
  Input:  data imageData
          rows pixel rows
          cols pixel cols
 ---------------------------------------------------------------*/
void ArduEyeUI::PrintImage(uchar * data, int rows, int cols)
{
    QImage image(data, cols, rows, cols, QImage::Format_Indexed8);
    image.setColorTable(ColorMap);
    QImage imFit = image.scaled(ui->imagelabel->width(), ui->imagelabel->height());
    ImagePixMap = QPixmap::fromImage(imFit);
}

/*---------------------------------------------------------------
  PrintVectors prints vector overlay of X and Y data
  Input     dataR:  row dataset
            dataC:  col dataset
            rows : number of rows of vectors
            cols:  number of cols of vectors
            PM: PixelMap that vectors overlay onto
 ----------------------------------------------------------------*/
void ArduEyeUI::PrintVectors(char *dataR, char * dataC, int rows, int cols, QPixmap *PM)
{
    int iIdx;
    float yPlace;
    QPointF * PointList = new QPointF[rows * cols *2];
    // find vector max dimmensions
    float BoxHeight = ui->imagelabel->height()/rows;
    float BoxWidth = ui->imagelabel->width()/cols;
    // cycle through vector points and store end points in list
    for (int i = 0; i < rows; i++)
    {
        iIdx = i * cols;
        yPlace = (BoxHeight * (i+0.5f));
        for(int j = 0; j < cols; j++)
        {
            QPointF S(BoxWidth* (j+0.5f), yPlace);
            PointList[iIdx*2 + 2*j] = S;
            QPointF E(S.x() - (int)dataC[iIdx + j] *BoxWidth / 256.0f, S.y() + (int)dataR[iIdx + j]*BoxHeight / 256.0f);
            PointList[iIdx*2 + 2*j+1] = E;
        }
    }

    //display vectors
    QPainter painter(PM);
    QPen pen(Qt::red, 2);
    painter.setPen(pen);
    painter.drawLines(PointList, rows*cols);

    delete PointList;
}

/*---------------------------------------------------------------
  PrintPoints: Print single points at the given locations
  Input:   data: PointData  (format data[0] = rows in underlying image, data[1]
                                cols in underlying image, 2pix: Y,X for all points)
           DataSize: number of points including 2 dimmension bytes
           PM: pixel map that points overlay onto
 ---------------------------------------------------------------*/
void ArduEyeUI::PrintPoints(char *data, int DataSize, QPixmap * PM)
{
    int iIdx;
    // compute resolution on display
    float PixHeight = ui->imagelabel->height()/(float)data[0];
    float PixWidth = ui->imagelabel->width()/(float)data[1];

    // set up painter
    QPainter painter(PM);
    QPen pen(Qt::green, 2);
    pen.setWidth(PixWidth - 2);
    painter.setPen(pen);

    // draw points
    for (int i = 2; i < DataSize; i+=2)
        painter.drawPoint((data[i+1] + 0.5) * PixWidth, (data[i] + 0.5) * PixHeight);
}

/*---------------------------------------------------------------
  PrintOFXBoxes  Paints Box Graph view of horizontal optic flow.
  Boxes are red for positive values and blue for negative
  Input:    data - values to be charted, rows*cols in length
            rows : number of rows
            cols:  number of cols
 ---------------------------------------------------------------*/
void ArduEyeUI::PrintOFXBoxes(char *data, int rows, int cols)
{
    int i, j, iIdx, X1;
    int * BoxCols = new int[cols];

    // if more than one row, average into one row
    for(i = 0; i < cols; i++)
        BoxCols[i] = 0;
    for(i = 0; i < rows; i++)
    {
        iIdx = i*cols;
        for(j = 0; j < cols; j++)
            BoxCols[j] += (int)data[iIdx + j];
    }
    for(j = 0; j < cols; j++)
        BoxCols[j] *= ui->OpticX_Label->height() / (128.0f*rows);

    // paint boxes //
    // setup box width
    int BoxWidth = ui->OpticX_Label->width() / cols;

    //setup painter
    QPixmap PMBlack(ui->OpticX_Label->width(), ui->OpticX_Label->height());
    PMBlack.fill(Qt::black);
    ui->OpticX_Label->setPixmap(PMBlack);
    QPixmap *PM = (QPixmap *)(ui->OpticX_Label->pixmap());

    //paint
    QPainter painter(PM);
    QBrush brush(Qt::blue, Qt::SolidPattern);
    X1 = 0;
    for(j = 0; j < cols; j++)
    {
        if(BoxCols[j] <=0)
        {
            BoxCols[j] *= -1;
            brush.setColor(Qt::red);
        }
        else
            brush.setColor(Qt::blue);
        painter.fillRect(X1, 0, BoxWidth, BoxCols[j], brush);
        X1 += BoxWidth;
    }
    delete BoxCols;
}
/*---------------------------------------------------------------
  PrintOFYBoxes  Paints Box Graph view of vertical optic flow.
  Boxes are red for positive values and blue for negative
  Input:    data - values to be charted, rows*cols in length
            rows : number of rows
            cols:  number of cols
 ---------------------------------------------------------------*/
void ArduEyeUI::PrintOFYBoxes(char *data, int rows, int cols)
{
    int i, j, Y1, iIdx;
    int * BoxRows = new int[rows];

    // if more than one column, avgerage into one column
    for(i = 0; i < rows; i++)
        BoxRows[i] = 0;
    for(i = 0; i < rows; i++)
    {
        iIdx = i * cols;
        for(j = 0; j < cols; j++)
            BoxRows[i] += (int)data[iIdx + j];
        BoxRows[i] *= ui->OpticY_Label->width() / (128.0f * cols);
    }

    // initiallize block size
    int BoxHeight = ui->OpticY_Label->height() / rows;

    // setup painter
    QPixmap PMBlack(ui->OpticY_Label->width(), ui->OpticY_Label->height());
    PMBlack.fill(Qt::black);
    ui->OpticY_Label->setPixmap(PMBlack);
    QPixmap *PM = (QPixmap *)(ui->OpticY_Label->pixmap());

    // paint
    QPainter painter(PM);
    QBrush brush(Qt::blue, Qt::SolidPattern);
    Y1 = 0;
    for(j = 0; j < rows; j++)
    {
        if(BoxRows[j] <=0)
        {
            BoxRows[j] *= -1;
            brush.setColor(Qt::blue);
        }
        else         
            brush.setColor(Qt::red);

        painter.fillRect(0, Y1, BoxRows[j], BoxHeight, brush);
        Y1 += BoxHeight;
    }
    delete BoxRows;
}

/*---------------------------------------------------------------
  Parse  parse commands entered into Command Edit Line.  english commands
  are translated to their byte values using the CmdList and CmdIndex
  Arrays, which are setup in the constructor using the parameters txt file.
  input  inText : text string in the Command Edit Box
 ---------------------------------------------------------------*/
void ArduEyeUI::Parse(QString inText)
{
    QStringList inList = inText.split(" ");
    QByteArray Dat;
    char dat;
    int Idx;

    // fill data array:
    // start packet esc seqence
    Dat.append(ESC_CHAR);
    Dat.append(START_PCKT);

    // set write/display/stop prefix
    Idx = CmdList.indexOf(inList.at(0));
    if(Idx >= 0)
        Dat.append(CmdIndex[Idx]);
    else
    {
        ui->textEdit->append("invalid command: 1 (case sensitive)");
        return;
    }

    // set command
    Idx = CmdList.indexOf(inList.at(1));
    if(Idx >= 0)
        Dat.append(CmdIndex[Idx]);
    else
    {
        ui->textEdit->append("invalid command: 2 (case sensitive)");
        return;
    }

    // set value parameters
    for(int i = 0; i < inList.size()-2; i++)
    {
        dat = static_cast<char>(inList.at(i+2).toShort() & 0xFF);
        Dat.append(dat);
    }
    // end packet esc sequence
    Dat.append(ESC_CHAR);
    Dat.append(END_PCKT);

    // debug mode: print command to debug window
    for(int i = 0; i < Dat.size(); i++)
        qDebug() << static_cast<int>(Dat[i]);

    // write data to serial device
    comm->port->write(Dat);

    // set timer to check for CmdRecieved acknowledge from the ArduEye
    QTimer::singleShot(200, this, SLOT(CheckCmdReceived()));
}

/*---------------------------------------------------------------
  CheckCmdReceived  slot triggered byt timer after a command is sent.
  The ArduEye sends a Command Acknowledge when it receives a command
  successfully.  If the Ackowledge is not recieved within the timer period(200ms)
  the command is sent again.  This feature is important as the serial link
  seems to loose data on occasion
 ---------------------------------------------------------------*/
void ArduEyeUI::CheckCmdReceived()
{
    if(CmdReceived)
    {
        CmdReceived = false;
        ui->textEdit->append(ui->CmdEdit->text());
    }
    else
       Parse(ui->CmdEdit->text());

    qDebug() << "CheckCmdReceived";
}

/*---------------------------------------------------------------
  on_CmdEdit_returnPressed Slot response to UI: parse Cmd Edit Text
and send command to ArduEye.  Pressing return in the Cmd Edit Box and
  pressing the "enter" button have the same effect.
 ---------------------------------------------------------------*/
void ArduEyeUI::on_CmdEdit_returnPressed()
{
        Parse(ui->CmdEdit->text());
}

/*---------------------------------------------------------------
  LoadTextFile  Parse the paramaters txt file and store command / dataset
  information in local arrays.  This function is called in the constructor
 ---------------------------------------------------------------*/
void ArduEyeUI::LoadTextFile()
{
    QFile inputFile(":/ArduEyeCmdListStonyman.txt");
    inputFile.open(QIODevice::ReadOnly);
    QTextStream in (&inputFile);
    char dat;
    QString line;

    // Command categories are printed in bold
    // text file format is predetermined
    ui->CmdListEdit->setFontWeight(75);
    ui->CmdListEdit->append("Prefixes");
    ui->CmdListEdit->setFontWeight(25);
    while((line=in.readLine()) != "/basic_commands"){};
    while((line=in.readLine())!="/end_basic_commands")
    {
        // add command names to CMD list and command values to CmdIndex
        CmdList.append(line.section(" ",0,0));
        dat = static_cast<char>(line.section(" ", 1,1).toShort() & 0xFF);
        CmdIndex.append(dat);
        // print to command reference box
        ui->CmdListEdit->append(line.section(" ",0,0));
    }
    ui->CmdListEdit->append(" ");

    ui->CmdListEdit->setFontWeight(75);
    ui->CmdListEdit->append("Commands");
    ui->CmdListEdit->setFontWeight(25);
    while((line=in.readLine()) != "/VC_commands"){};
    while((line=in.readLine())!= "/end_VC_commands")
    {
        // add command names to CMD list and command values to CmdIndex
        CmdList.append(line.section(" ",0,0));
        dat = static_cast<char>(line.section(" ", 1,1).toShort() & 0xFF);
        CmdIndex.append(dat);
        // print to command reference box
        ui->CmdListEdit->append(line.section(" ",0,0));
    }

    ui->CmdListEdit->append(" ");
    while((line=in.readLine())!="/advanced_VC_commands"){};
    ui->CmdListEdit->setFontWeight(75);
    ui->CmdListEdit->append("Adv. Commands");
    ui->CmdListEdit->setFontWeight(25);
    while((line=in.readLine())!="/end_advanced_VC_commands")
    {
        // add command names to CMD list and command values to CmdIndex
        CmdList.append(line.section(" ",0,0));
        dat = static_cast<char>(line.section(" ", 1,1).toShort() & 0xFF);
        CmdIndex.append(dat);
        // print to command reference box
        ui->CmdListEdit->append(line.section(" ",0,0));
    }

    // generic header is datasetZero, datasets added starting with index 1
    NumDataSets = 1;

    ui->CmdListEdit->append(" ");
    while((line=in.readLine())!= "/display"){};
    ui->CmdListEdit->setFontWeight(75);
    ui->CmdListEdit->append("Display Options");
    ui->CmdListEdit->setFontWeight(25);
    while((line=in.readLine())!= "/end_display")
    {
        // add command names to CMD list and command values to CmdIndex
        CmdList.append(line.section(" ",0,0));
        dat = static_cast<char>(line.section(" ", 1,1).toShort() & 0xFF);
        CmdIndex.append(dat);
        // print to command reference box
        ui->CmdListEdit->append(line.section(" ",0,0));
        NumDataSets++;
    }

    // debug mode : print commands to debug screen
    for (int i=0; i < CmdIndex.size(); i++)
    {
        qDebug() << "Index " << i << ": " << static_cast<int>(CmdIndex[i]);
        qDebug() << "Cmd: "  << CmdList.at(i);
    }
}

/*---------------------------------------------------------------
  InitDataSets  parse parameters txt file and stor dataset information,
  in the DataSets structure.  Each dataset has a data array that is initialized
  to the max size reported in the parameters txt file.  This function is called
  in the constructor.
 ---------------------------------------------------------------*/
void ArduEyeUI::InitDataSets()
{
    QFile inputFile(":/ArduEyeCmdListStonyman.txt");
    inputFile.open(QIODevice::ReadOnly);
    QTextStream in (&inputFile);
    QString line, DatType;
    int DSIdx = 0, MaxDataSize = 0;

    DS = new DataSets[NumDataSets];

    // record header size
    while((line=in.readLine())!= "/header_info"){};
    while((line=in.readLine())!= "/end_header_info")
    {
        DS[DSIdx].name = line.section(" ",0,0);
        DS[DSIdx].InitDataArray(line.section(" ", 1, 1).toInt());
        DSIdx++;
    }

    // record dataset sizes
    while((line=in.readLine())!= "/display"){};
    while((line=in.readLine())!= "/end_display")
    {
        DS[DSIdx].name = line.section(" ",0,0);
        DS[DSIdx].DSID = line.section(" ", 1,1).toInt();
        DatType = line.section(" ", 3,3);
        if(DatType == "char")
            DS[DSIdx].SizeFactor = 1;
        else if(DatType == "short")
            DS[DSIdx].SizeFactor = 2;
        else if(DatType == "int" )
            DS[DSIdx].SizeFactor = 4;
        DS[DSIdx].InitDataArray(line.section(" ", 2, 2).toInt());
        if(DS[DSIdx].MaxSize > MaxDataSize)
            MaxDataSize = DS[DSIdx].MaxSize;
        DSIdx++;
    }

    // serial input data buffer can hold two copies of the largest dataset
    DataBuffer = new char[MaxDataSize*2];
    DataBufferSize = MaxDataSize * 2;
}

/*---------------------------------------------------------------
  on_FileRecordButton_clicked
 ---------------------------------------------------------------*/
void ArduEyeUI::on_FileRecordButton_clicked()
{
    // if FileRecord not active, activate and open file
    if(!FileRecordOn)
    {
        // call dialog to prompt user for filename / directory
        QString filename = QFileDialog::getSaveFileName(this, tr("Save File"), "", "");

        // open file
        FileSave.setFileName(filename);
        FileSave.open(QIODevice::WriteOnly);
        // activate FileRecord
        FileRecordOn = true;

        // set button text
        ui->FileRecordButton->setText("Stop Record");
    }
    // if FileRecord active, de-activate and close file
    else
    {
        // close file
        FileSave.close();
        //de-active FileRecord
        FileRecordOn = false;

        ui->FileRecordButton->setText("Record to File");
    }
}

/*---------------------------------------------------------------
  on_enterButton_clicked: Slot response to UI: parse Cmd Edit Text
and send command to ArduEye.  Pressing return in the Cmd Edit Box and
  pressing the "enter" button have the same effect.
 ---------------------------------------------------------------*/
void ArduEyeUI::on_enterButton_clicked()
{
    on_CmdEdit_returnPressed();
}
