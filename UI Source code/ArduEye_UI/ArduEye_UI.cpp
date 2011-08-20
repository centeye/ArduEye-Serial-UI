#include "ArduEye_UI.h"
#include "ui_ArduEyeUI.h"
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtDebug>
#include <QPainter>
#include <QColor>

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

    ui->textEdit->setReadOnly(true);
    ui->CmdListEdit->setReadOnly(true);

    NumDataSets = 0;
    LoadTextFile();
    InitDataSets();

    HeadDat = new Header(HEADER_SIZE);
    OFYRec = OFXRec = false;
    Timer.start();

    for(int i=0;i<256;++i)
      ColorMap[i] = qRgb(i,i,i);

    QPixmap PMBlack(ui->imagelabel->width(), ui->imagelabel->height());
    PMBlack.fill(Qt::black);
    ImagePixMap = PMBlack;

}

ArduEyeUI::~ArduEyeUI()
{
    for(int i = 0; i < NumDataSets; i++)
        DS[i].Delete();
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
    int numbytes, SkippedCount;
    QString num, num1, num2, num3;
    char Header[10];//HeadDat->HeaderSize];
    char Poll[1];
    numbytes= comm->port->bytesAvailable();
    ui->textEdit->append("recieved bytes:" + num.setNum(numbytes));

    if(numbytes >= MAX_PACKET_SIZE)
    {
       QByteArray temp =  comm->port->readAll();
       //ui->textEdit->append(temp);
        return;
    }

   if(HeadDat->Process == NULL_PROCESS)
    {
        SkippedCount = 0;
        while((Poll[0] != ESC_CHAR) && (SkippedCount < numbytes))
        {
            SkippedCount++;
            comm->port->read(Poll, 1);
        }

        if(SkippedCount > numbytes-2)
        {
            qDebug() << "trash bytes " << SkippedCount << "numbytes" << numbytes;
            comm->port->readAll();
            return;
        }

        comm->port->read(Poll, 2);
        HeadDat->SetPacketSize(((unsigned char)Poll[0] << 8) + (unsigned char)Poll[1]);

        if(HeadDat->PacketSize > MAX_PACKET_SIZE)
            return;

        HeadDat->Process = RECEIVE_PROCESS;
        if(numbytes < HeadDat->PacketSize)
            return;
    }
    if(numbytes < HeadDat->PacketSize)
    {
    //    qDebug() << "wait for " << HeadDat->PacketSize << "  numbytes: " << numbytes;
        return;
    }
    else
    {
        comm->port->read(Header, HeadDat->HeaderSize);
        HeadDat->Parse(Header);
        int AD = GetActiveDataSet(HeadDat->DSID);

        if((HeadDat->DataSize > 0) && (HeadDat->DSIdx + HeadDat->DataSize <= DS[AD].MaxSize))
            comm->port->read(DS[AD].DataArray + HeadDat->DSIdx, HeadDat->DataSize);
        else
        {
            qDebug() << "Size" << HeadDat->DataSize << "DSIDX" << HeadDat->DSIdx << "MaxSize" << DS[AD].MaxSize;
            QByteArray Temp = comm->port->readAll();
        }

        if(HeadDat->EODS)
            PaintManager(AD);
        HeadDat->Process = NULL_PROCESS;

    }
}

void ArduEyeUI::PaintManager(int ActiveDataSet)
{
    int xID, yID;

    int DSSize = HeadDat->DSRows * HeadDat->DSCols;
    if((DSSize <= DS[ActiveDataSet].MaxSize) && (DSSize > 0))
    {

        switch(HeadDat->DSID)
        {
        case DATA_ID_RAW:
           PrintImage((uchar *)(DS[ActiveDataSet].DataArray), HeadDat->DSRows, HeadDat->DSCols);
           break;
        case DATA_ID_OFX:
          //  ui->OpticX_Label->update();
            PrintOFXBoxes(DS[ActiveDataSet].DataArray, HeadDat->DSRows, HeadDat->DSCols);
            if(!OFYRec)
            {
                yID = GetActiveDataSet(DATA_ID_OFY);
                DS[yID].Clear();
                PrintVectors(DS[yID].DataArray, DS[ActiveDataSet].DataArray, HeadDat->DSRows, HeadDat->DSCols);
            }
            OFYRec = false;
            OFXRec = true;
            break;
        case DATA_ID_OFY:
            PrintOFYBoxes(DS[ActiveDataSet].DataArray, HeadDat->DSRows, HeadDat->DSCols);
            xID = GetActiveDataSet(DATA_ID_OFX);
            if(!OFXRec)
                DS[xID].Clear();
            PrintVectors(DS[ActiveDataSet].DataArray, DS[xID].DataArray, HeadDat->DSRows, HeadDat->DSCols);
            OFYRec = true;
            OFXRec = false;
            break;
        case DATA_ID_FPS:
            PrintFPS(DS[ActiveDataSet].DataArray);
            break;
        }
    }
}

void ArduEyeUI::PrintFPS(char * data)
{
    int fps = (data[0] << 8) + data[1];
    QString fpsString, num;
    fpsString = "FPS Sensor: " + num.setNum(fps);
    QPoint TopLeft(10,20);

    QPixmap * PM = (QPixmap *)(ui->imagelabel->pixmap());
    if(PM == NULL)
    {
        QPixmap PMBlack(ui->imagelabel->width(), ui->imagelabel->height());
        PMBlack.fill(Qt::black);
        ui->imagelabel->setPixmap(PMBlack);
        PM = (QPixmap *)(ui->imagelabel->pixmap());
    }
    QPainter painter(PM);
    QPen pen(Qt::red, 2);
    painter.setPen(pen);
    painter.drawText(TopLeft, fpsString);
}

void ArduEyeUI::PrintFPSUI(int data)
{
    QString fpsString, num;
    fpsString = "FPS UI: " + num.setNum(data);
    QPoint TopLeft(10,20);

    QPixmap * PM = (QPixmap *)(ui->imagelabel->pixmap());
    if(PM == NULL)
    {
        QPixmap PMBlack(ui->imagelabel->width(), ui->imagelabel->height());
        PMBlack.fill(Qt::black);
        ui->imagelabel->setPixmap(PMBlack);
        PM = (QPixmap *)(ui->imagelabel->pixmap());
    }
    QPainter painter(PM);
    QPen pen(Qt::red, 2);
    painter.setPen(pen);
    painter.drawText(TopLeft, fpsString);
}

void ArduEyeUI::PrintImage(uchar * data, int rows, int cols)
{
    //QImage image(data, 13, 13, QImage::Format_Indexed8);
    QImage image(data, cols, rows, cols, QImage::Format_Indexed8);
    image.setColorTable(ColorMap);
  //  QImage image2 = image.convertToFormat(QImage::Format_RGB888);
    QImage imFit = image.scaled(ui->imagelabel->width(), ui->imagelabel->height());
    ImagePixMap = QPixmap::fromImage(imFit);
    if(PixRefresh)
    {
        ui->imagelabel->setPixmap(QPixmap::fromImage(imFit));//ImagePixMap);
     //   int ms = Timer.elapsed();
      //  int fps = (ms > 0) ? 1000/ms : 0;
      //  PrintFPSUI(fps);
      //  Timer.start();
    }
    PixRefresh = true;
}

void ArduEyeUI::PrintVectors(char *dataR, char * dataC, int rows, int cols)
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
            QPointF E(S.x() - (int)dataC[iIdx + j] *BoxHeight / 256, S.y() + (int)dataR[iIdx + j]*BoxHeight / 256);
            PointList[iIdx*2 + 2*j+1] = E;
        }
    }

    QPixmap PM = ImagePixMap; //(QPixmap *)(ui->imagelabel->pixmap());
    if(PixRefresh == false)
    {
        QPixmap PMBlack(ui->imagelabel->width(), ui->imagelabel->height());
        PMBlack.fill(Qt::black);
        //ui->imagelabel->setPixmap(PMBlack);
        PM = PMBlack;//(QPixmap *)(ui->imagelabel->pixmap());
    }
    QPainter painter(&PM);
    QPen pen(Qt::green, 2);
    painter.setPen(pen);
    painter.drawLines(PointList, rows*cols);

    //painter->end();
    ui->imagelabel->setPixmap(PM);
    ImagePixMap = PM;

 //   int ms = Timer.elapsed();
 //   int fps = (ms > 0) ? 1000/ms : 0;
 //   PrintFPSUI(fps);
 //   Timer.start();

    PixRefresh = false;
    delete PointList;
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
    Dat.append((char)(inList.count()));

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

    for(int i = 0; i < Dat.size(); i++)
        qDebug() << static_cast<int>(Dat[i]);
    // write data to serial device
    comm->port->write(Dat);
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
    int DSIdx = 0;

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
        DSIdx++;
    }
}
