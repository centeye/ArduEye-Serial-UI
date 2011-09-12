#include "commwrapper.h"
#include "Datasets.h"

CommWrapper::CommWrapper():
    port(new QextSerialPort),
    enumerator(new QextSerialEnumerator)
{
    enumerator->setUpNotifications();
    ACKCmd.append(ESC_CHAR);
    ACKCmd.append(ACK_CMD);
}
CommWrapper::~CommWrapper()
{
    port->flush();
}

void CommWrapper::Enumerate(QComboBox * list)
{
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
    for (int i = 0; i < ports.size(); i++)
        list->addItem(ports.at(i).portName);
}
bool CommWrapper::Open(const QString & name)
{
    port = new QextSerialPort(name, QextSerialPort::EventDriven);

    port->setTimeout(0);

    port->setBaudRate(BAUD115200);
    port->open(QIODevice::ReadWrite | QIODevice::Unbuffered);



    if(!port->isOpen())
    {
        port->flush();
        port->readAll();
        return false;
    }
    else
        return true;
}
void CommWrapper::SendAck()
{
    port->write(ACKCmd);
}

