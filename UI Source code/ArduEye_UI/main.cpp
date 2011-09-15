/* Main.cpp - main event loop.
  This function is a generic Qt UI event loop
  */


#include <QtGui/QApplication>
#include "ArduEye_UI.h"
#include <QtDebug>

int main(int argc, char *argv[])
{

    QByteArray Dat, OutDat;
    QApplication a(argc, argv);
    ArduEyeUI w;
    w.show();

    return a.exec();
}
