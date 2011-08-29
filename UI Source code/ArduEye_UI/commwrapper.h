#ifndef COMMWRAPPER_H
#define COMMWRAPPER_H

#include "qextserialport.h"
#include "qextserialenumerator.h"
#include "QComboBox"

class CommWrapper
{
public:
    CommWrapper();
    ~CommWrapper();

    QextSerialPort * port;
    QextSerialEnumerator * enumerator;

    void Enumerate(QComboBox * list);
    bool Open(const QString & name);
    void SendAck();

    QByteArray ACKCmd;

};

#endif // COMMWRAPPER_H
