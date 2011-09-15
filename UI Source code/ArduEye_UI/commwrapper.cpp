/*
  commwrapper.cpp - Wrapper for the qextserial library by Michal Policht
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

#include "commwrapper.h"
#include "Datasets.h"

/*---------------------------------------------------
 CommWrapper: Constructor
 ---------------------------------------------------*/
CommWrapper::CommWrapper():
    port(new QextSerialPort),
    enumerator(new QextSerialEnumerator)
{
    enumerator->setUpNotifications();
    // define ACKCmd
    ACKCmd.append(ESC_CHAR);
    ACKCmd.append(ACK_CMD);
}

/*---------------------------------------------------
 ~CommWrapper: Desctructor
 ---------------------------------------------------*/
CommWrapper::~CommWrapper()
{
    // flush remaining bytes in serial buffer
    port->flush();
}

/*---------------------------------------------------
 Enumerate: go through connected devices and add port names to the Serial Port list menu
 Input:  list - name of the Serial port list combo box
 ---------------------------------------------------*/
void CommWrapper::Enumerate(QComboBox * list)
{
    // go through connected devices and add port names to the Serial Port list menu (drop down combo box)
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
    for (int i = 0; i < ports.size(); i++)
        list->addItem(ports.at(i).portName);
}

/*---------------------------------------------------
 Open: Setup Serial port and connect
 Input:  name - Name of serial port to open
 return:  true if port opened successfully, false if not
 ---------------------------------------------------*/
bool CommWrapper::Open(const QString & name)
{
    port = new QextSerialPort(name, QextSerialPort::EventDriven);

    port->setTimeout(0);

    // highest baudrate achievable with all PC platforms
    port->setBaudRate(BAUD115200);
    port->open(QIODevice::ReadWrite | QIODevice::Unbuffered);

    // return true if device opens succsessfully
    if(!port->isOpen())
    {
        port->flush();
        port->readAll();
        return false;
    }
    else
        return true;
}

/*---------------------------------------------------
 SendAck: Send Acknowledge Packet to ArduEye.  Ack is sent
 in response to  a GO_CHAR to alert the arduEye that the
 serial buffer not overflowing and can accept more data
 ---------------------------------------------------*/
void CommWrapper::SendAck()
{
    port->write(ACKCmd);
}

