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

ArduEyeUI::ArduEyeUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ArduEyeUI),
    comm(new CommWrapper)
{
    ui->setupUi(this);
    comm->Enumerate(ui->PortSelect);

    connect(comm->enumerator, SIGNAL(deviceDiscovered(const QextPortInfo &)),
               this, SLOT(onDeviceDiscovered(const QextPortInfo &)));
    connect(comm->enumerator, SIGNAL(deviceRemoved(const QextPortInfo &)),
               this, SLOT(onDeviceRemoved(const QextPortInfo &)));
   // connect(this, SIGNAL(paintReady(void)), this, SLOT(PaintManager()), Qt::QueuedConnection);

    ui->textEdit->setReadOnly(true);
    ui->CmdListEdit->setReadOnly(true);

    DataBufferSize = StartIdx = BufEndIdx = DataIdx = 0;
    NumDataSets = 0;
    LoadTextFile();
    InitDataSets();

    Timer.start();

    for(int i=0;i<256;++i)
      ColorMap[i] = qRgb(i,i,i);

    QPixmap PMBlack(ui->imagelabel->width(), ui->imagelabel->height());
    PMBlack.fill(Qt::black);
    ImagePixMap = PMBlack;

    ESCReceived = false;
    FileRecordOn = false;
    CmdReceived = false;
}

ArduEyeUI::~ArduEyeUI()
{
    for(int i = 0; i < NumDataSets; i++)
        DS[i].Delete();
    delete DataBuffer;
    delete ui;
}

void ArduEyeUI::on_findButton_clicked()
{
    on_CmdEdit_returnPressed();
}

void ArduEyeUI::onDeviceDiscovered(const QextPortInfo &newport)
{
    ui->PortSelect->addItem(newport.portName);
}

void ArduEyeUI::onDeviceRemoved(const QextPortInfo &newport)
{
    int Idx = ui->PortSelect->findText(newport.portName, Qt::MatchExactly);
    ui->PortSelect->removeItem(Idx);
}

void ArduEyeUI::on_ConnectButton_clicked()
{
    bool success = comm->Open(ui->PortSelect->currentText());
    connect(comm->port, SIGNAL(readyRead()), this, SLOT(onDataAvailable()));
    if(success)
        ui->textEdit->append("Device Connected Successfully");
    else
        ui->textEdit->append("Device Connect Failed");
}

void ArduEyeUI::onDataAvailable()
{
    int numbytes;
    QString num;
    numbytes = comm->port->bytesAvailable();
  //  qDebug() << "recieved bytes:"  << numbytes;

    if(numbytes >= MAX_PACKET_SIZE)
    {
       QByteArray temp = comm->port->readAll();
       qDebug() << "BufferOverflow!";
       //ui->textEdit->append(temp);
        return;
    }

    if(numbytes > DataBufferSize - DataIdx)
    {
        BufEndIdx = DataIdx;
        DataIdx = 0;
    }
    comm->port->read(DataBuffer + DataIdx, numbytes);


    for (int i = 0; i < numbytes; i++)
    {
        if(ESCReceived)
        {
            switch(DataBuffer[DataIdx + i])
            {
            case END_PCKT:
                ParsePacket(StartIdx, DataIdx + i - 1);
                break;
            case GO_CHAR:
            //    qDebug() << "Ack";
                comm->SendAck();
                break;
            case START_PCKT:
                StartIdx = DataIdx + i;
                break;
            case CMD_ACK:
                CmdReceived = true;
                break;
            default:
                break;
            }
            ESCReceived = false;
        }
        else if(DataBuffer[DataIdx + i] == ESC_CHAR)
            ESCReceived = true;
    }
    DataIdx += numbytes;

}

void ArduEyeUI::ParsePacket(int Start, int End)
{
    char Header[HEADER_SIZE*2];
    int DataID = DataBuffer[Start + 1];
    int PacketType, DIdx, MaxSize, Idx;
    char *Array;

    if(DataID == END_FRAME)
    {
        RecordtoFile();
        paintManager();
        return;
    }
    else if (DataID == GO_CHAR)
        return;

    if(Start == BufEndIdx)
        qDebug() << "START == BUFEND!!!!!!!!";

    // odd dataIDs are headers
    // check for header ID and process header
    if(DataID % 2  != 0)
    {
        PacketType = PCKT_HEADER;
        DIdx = GetActiveDataSet(DataID-1);
        Array = Header;
        MaxSize = HEADER_SIZE;
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
        for(int i = Start + 2; i < BufEndIdx; i++)
        {
            if(DataBuffer[i]  == ESC_CHAR)
            {
               i++;
               while((DataBuffer[i] == GO_CHAR) || (DataBuffer[i] == CMD_ACK))
                {
                    i++;
                    if(DataBuffer[i] == ESC_CHAR)
                        i++;
                    else
                        break;
                 }
            }
            Array[Idx++] = DataBuffer[i];
            if(Idx > MaxSize)
            {
                BufEndIdx = 0;
                return;
            }
        }
        for(int i = 0; i < End; i++)
        {
            if(DataBuffer[i]  == ESC_CHAR)
            {
               i++;
                while((DataBuffer[i] == GO_CHAR) || (DataBuffer[i] == CMD_ACK))
                {
                    i++;
                    if(DataBuffer[i] == ESC_CHAR)
                        i++;
                    else
                        break;
                 }
            }
            Array[Idx++] = DataBuffer[i];
            if(Idx > MaxSize)
            {
                BufEndIdx = 0;
                return;
            }
        }
    }
    else
    {
        Idx = 0;
        for(int i = Start + 2; i < End; i++)
        {
            if(DataBuffer[i]  == ESC_CHAR)
            {
               i++;
                while((DataBuffer[i] == GO_CHAR) || (DataBuffer[i] == CMD_ACK))
                {
                    i++;
                    if(DataBuffer[i] == ESC_CHAR)
                        i++;
                    else
                        break;
                }
            }

            Array[Idx++] = DataBuffer[i];
            if(Idx > MaxSize)
                return;
        }
    }
    if(PacketType == PCKT_HEADER)
    {
        DS[DIdx].height = (Header[0] << 8) + Header[1];
        DS[DIdx].width = (Header[2] << 8) + Header[3];
        DS[DIdx].DisplayType = Header[4];
        DS[DIdx].length = DS[DIdx].height * DS[DIdx].width;
    }
    else
        DS[DIdx].DataReceived = true;

    if(Start > End)
        BufEndIdx = 0;
}

void ArduEyeUI::RecordtoFile()
{
    if(FileRecordOn)
    {
        for(int n = 0; n < NumDataSets; n++)
        {
            if(DS[n].DataReceived)
            {
                char DataInfo[] = {DS[n].DSID, DS[n].width, DS[n].height};
                FileSave.write(DataInfo, 3);

                FileSave.write(DS[n].DataArray, DS[n].length);
                qDebug() << FileSave.pos() << DS[n].length;
            }
        }
    }
}

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
    if(!ImageProcessed)
    {
      //  QPixmap PM = ImagePixMap; //(QPixmap *)(ui->imagelabel->pixmap());
      //  if(PixRefresh == false)
      //  {
            QPixmap PMBlack(ui->imagelabel->width(), ui->imagelabel->height());
            PMBlack.fill(Qt::black);
            //ui->imagelabel->setPixmap(PMBlack);
            ImagePixMap = PMBlack;//(QPixmap *)(ui->imagelabel->pixmap());
       // }
    }

    // fill chart images
    for (int n = 0; n < NumDataSets; n++)
    {
        if((DS[n].DisplayType == DISPLAY_CHARTX) && (DS[n].DataReceived))
        {
            PrintOFXBoxes(DS[n].DataArray, DS[n].height, DS[n].width);
            yID = GetActiveDataSet(DATA_ID_OFY);
            if(!DS[yID].DataReceived)
            {
                DS[yID].Clear();
                PrintVectors(DS[yID].DataArray, DS[n].DataArray, DS[n].height, DS[n].width, &ImagePixMap);
                DS[n].DataReceived = false;
            }

        }
        if((DS[n].DisplayType == DISPLAY_CHARTY) && (DS[n].DataReceived))
        {
            PrintOFYBoxes(DS[n].DataArray, DS[n].height, DS[n].width);
            xID = GetActiveDataSet(DATA_ID_OFX);
            if(!DS[xID].DataReceived)
                DS[xID].Clear();
            PrintVectors(DS[n].DataArray, DS[xID].DataArray, DS[n].height, DS[n].width, &ImagePixMap);
            DS[n].DataReceived = false;
            DS[xID].DataReceived = false;
        }
    }
    for (int n = 0; n < NumDataSets; n++)
    {
        if((DS[n].DisplayType == DISPLAY_TEXT) && (DS[n].DataReceived))
        {
            PrintText(DS[n].DataArray, TEXT_XLOC, yloc, &ImagePixMap);
            DS[n].DataReceived = false;
            yloc += 20;
        }
    }
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
    for (int n = 0; n < NumDataSets; n++)
    {
        if((DS[n].DisplayType == DISPLAY_POINTS) && (DS[n].DataReceived))
        {
            PrintPoints(DS[n].DataArray, DS[n].length, &ImagePixMap);
            DS[n].DataReceived = false;
        }
    }
    ui->imagelabel->setPixmap(ImagePixMap);
}

void ArduEyeUI::PrintText(char * data, int xloc, int yloc, QPixmap * PM)
{
    int fps = ((unsigned char)data[0] << 8) + (unsigned char)data[1];
    QString fpsString, num;
    fpsString = "FPS Sensor: " + num.setNum(fps);
    QPoint TopLeft(xloc, yloc);

 /*   QPixmap PM = ImagePixMap; //(QPixmap *)(ui->imagelabel->pixmap());
    if(PixRefresh == false)
    {
        QPixmap PMBlack(ui->imagelabel->width(), ui->imagelabel->height());
        PMBlack.fill(Qt::black);
        //ui->imagelabel->setPixmap(PMBlack);
        PM = PMBlack;//(QPixmap *)(ui->imagelabel->pixmap());
    }*/
    QPainter painter(PM);
    QPen pen(Qt::red, 2);
    painter.setPen(pen);
    painter.drawText(TopLeft, fpsString);
}

void ArduEyeUI::PrintImage(uchar * data, int rows, int cols)
{
    QImage image(data, cols, rows, cols, QImage::Format_Indexed8);
    image.setColorTable(ColorMap);
    QImage imFit = image.scaled(ui->imagelabel->width(), ui->imagelabel->height());
    ImagePixMap = QPixmap::fromImage(imFit);
}

void ArduEyeUI::PrintVectors(char *dataR, char * dataC, int rows, int cols, QPixmap *PM)
{
    int iIdx;
    float yPlace;
    QPointF * PointList = new QPointF[rows * cols *2];
    int BoxHeight = min(ui->imagelabel->width()/cols, ui->imagelabel->height()/rows);
    for (int i = 0; i < rows; i++)
    {
        iIdx = i * cols;
        yPlace = (ui->imagelabel->height() * (i+0.5f)) / (float)rows;
        for(int j = 0; j < cols; j++)
        {
            QPointF S(ui->imagelabel->width()* (j+0.5f) / cols , yPlace);
            PointList[iIdx*2 + 2*j] = S;
            QPointF E(S.x() - (int)dataC[iIdx + j] *BoxHeight / 256.0f, S.y() + (int)dataR[iIdx + j]*BoxHeight / 256.0f);
            PointList[iIdx*2 + 2*j+1] = E;
        }
    }

    QPainter painter(PM);
    QPen pen(Qt::green, 2);
    painter.setPen(pen);
    painter.drawLines(PointList, rows*cols);

    delete PointList;
}

void ArduEyeUI::PrintPoints(char *data, int DataSize, QPixmap * PM)
{
    int iIdx;
    float PixHeight = ui->imagelabel->height()/(float)data[0];
    float PixWidth = ui->imagelabel->width()/(float)data[1];

    QPainter painter(PM);
    QPen pen(Qt::green, 2);
    pen.setWidth(PixWidth - 2);
    painter.setPen(pen);

    for (int i = 2; i < DataSize; i+=2)
        painter.drawPoint((data[i+1] + 0.5) * PixWidth, (data[i] + 0.5) * PixHeight);
}

void ArduEyeUI::PrintOFXBoxes(char *data, int rows, int cols)
{
    int i, j, iIdx, X1;
    int * BoxCols = new int[cols];

    // if more than one column, sum data to be 1D
    for(i = 0; i < cols; i++)
        BoxCols[i] = 0;
    for(i = 0; i < rows; i++)
    {
        iIdx = i*cols;
        for(j = 0; j < cols; j++)
            BoxCols[j] += int(data[iIdx + j]);
    }
    for(j = 0; j < cols; j++)
        BoxCols[j] *= ui->OpticX_Label->height() / (128.0f*rows);

    // paint boxes
    int BoxWidth = ui->OpticX_Label->width() / cols;

    QPixmap PMBlack(ui->OpticX_Label->width(), ui->OpticX_Label->height());
    PMBlack.fill(Qt::black);
    ui->OpticX_Label->setPixmap(PMBlack);
    QPixmap *PM = (QPixmap *)(ui->OpticX_Label->pixmap());

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
void ArduEyeUI::PrintOFYBoxes(char *data, int rows, int cols)
{
    int i, j, Y1, iIdx;
    int * BoxRows = new int[rows];

    // if more than one column, sum data to be 1D
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

    // paint boxes
    QPixmap PMBlack(ui->OpticY_Label->width(), ui->OpticY_Label->height());
    PMBlack.fill(Qt::black);
    ui->OpticY_Label->setPixmap(PMBlack);
    QPixmap *PM = (QPixmap *)(ui->OpticY_Label->pixmap());

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

int ArduEyeUI::GetActiveDataSet(int inDSID)
{
   int Idx = 0;

   while((DS[Idx].DSID != inDSID) && (Idx < NumDataSets))
       Idx++;

   if(Idx < NumDataSets)
       return Idx;
   else
       return 0;
}

void ArduEyeUI::Parse(QString inText)
{
    QStringList inList = inText.split(" ");
    QByteArray Dat;
    char dat;
    int Idx;

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
    Dat.append(ESC_CHAR);
    Dat.append(END_PCKT);

    for(int i = 0; i < Dat.size(); i++)
        qDebug() << static_cast<int>(Dat[i]);
    // write data to serial device
    comm->port->write(Dat);

    QTimer::singleShot(200, this, SLOT(CheckCmdReceived()));
}

void ArduEyeUI::CheckCmdReceived()
{
    if(CmdReceived)
        CmdReceived = false;
    else
       Parse(ui->CmdEdit->text());

    qDebug() << "CheckCmdReceived";
}

void ArduEyeUI::on_CmdEdit_returnPressed()
{
        Parse(ui->CmdEdit->text());
        ui->textEdit->append(ui->CmdEdit->text());
}
void ArduEyeUI::LoadTextFile()
{
    QFile inputFile(":/ArduEyeCmdListStonyman.txt");
    inputFile.open(QIODevice::ReadOnly);
    QTextStream in (&inputFile);
    char dat;
    QString line;

    ui->CmdListEdit->append("Prefixes");
    while((line=in.readLine()) != "/basic_commands"){};
    while((line=in.readLine())!="/end_basic_commands")
    {
        CmdList.append(line.section(" ",0,0));
        dat = static_cast<char>(line.section(" ", 1,1).toShort() & 0xFF);
        CmdIndex.append(dat);
        ui->CmdListEdit->append(line.section(" ",0,0));
    }
    ui->CmdListEdit->append(" ");

    ui->CmdListEdit->append("Commands");
    while((line=in.readLine()) != "/VC_commands"){};
    while((line=in.readLine())!= "/end_VC_commands")
    {
        CmdList.append(line.section(" ",0,0));
        dat = static_cast<char>(line.section(" ", 1,1).toShort() & 0xFF);
        CmdIndex.append(dat);
        ui->CmdListEdit->append(line.section(" ",0,0));
    }

    ui->CmdListEdit->append(" ");
    while((line=in.readLine())!="/advanced_VC_commands"){};
    ui->CmdListEdit->append("Adv. Commands");
    while((line=in.readLine())!="/end_advanced_VC_commands")
    {
        CmdList.append(line.section(" ",0,0));
        dat = static_cast<char>(line.section(" ", 1,1).toShort() & 0xFF);
        CmdIndex.append(dat);
        ui->CmdListEdit->append(line.section(" ",0,0));
    }

    ui->CmdListEdit->append(" ");
    while((line=in.readLine())!= "/display"){};
    ui->CmdListEdit->append("Display Options");
    while((line=in.readLine())!= "/end_display")
    {
        CmdList.append(line.section(" ",0,0));
        dat = static_cast<char>(line.section(" ", 1,1).toShort() & 0xFF);
        CmdIndex.append(dat);
        ui->CmdListEdit->append(line.section(" ",0,0));
        NumDataSets++;
    }

    for (int i=0; i < CmdIndex.size(); i++)
    {
        qDebug() << "Index " << i << ": " << static_cast<int>(CmdIndex[i]);
        qDebug() << "Cmd: "  << CmdList.at(i);
    }
}
void ArduEyeUI::InitDataSets()
{
    QFile inputFile(":/ArduEyeCmdListStonyman.txt");
    inputFile.open(QIODevice::ReadOnly);
    QTextStream in (&inputFile);
    QString line, DatType;
    int DSIdx = 0, MaxDataSize = 0;

    DS = new DataSets[NumDataSets];
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

    DataBuffer = new char[MaxDataSize*2];
    DataBufferSize = MaxDataSize * 2;
}

void ArduEyeUI::on_FileRecordButton_clicked()
{
    if(!FileRecordOn)
    {
        QString filename = QFileDialog::getSaveFileName(this, tr("Save File"), "", "");

        FileSave.setFileName(filename);
        FileSave.open(QIODevice::WriteOnly);
        FileRecordOn = true;
    }
    else
    {
        FileSave.close();
        FileRecordOn = false;
    }

}
