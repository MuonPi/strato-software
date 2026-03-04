#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <iomanip>

#include "tcpconnection.h"
#include "tcpmessage.h"
#include "tcpmessage_keys.h"

#include "ublox_structs.h"

#include <QObject>
#include <QPointF>
#include <QThread>
#include <QCoreApplication>



void receivedTcpMessage(TcpMessage tcpMessage)
{
    TCP_MSG_KEY msgID = static_cast<TCP_MSG_KEY>(tcpMessage.getMsgID());
    if (msgID == TCP_MSG_KEY::MSG_GPIO_RATE) {
        quint8 whichRate;
        QVector<QPointF> rate;
        *(tcpMessage.dStream) >> whichRate >> rate;
        float rateYValue;
        if (!rate.empty()) {
            rateYValue = rate.at(rate.size() - 1).y();
        } else {
            rateYValue = 0.0;
        }
        std::cout << "Rate " << (whichRate == 0 ? "XOR" : "AND") << " " << rateYValue << std::endl;
    } else if (msgID == TCP_MSG_KEY::MSG_QUIT_CONNECTION) {
        std::cout << "TCP QUIT" << std::endl;
        return;
    } else if (msgID == TCP_MSG_KEY::MSG_GEO_POS) {
        GnssPosStruct pos {};
        *(tcpMessage.dStream) >> pos.iTOW >> pos.lon >> pos.lat
            >> pos.height >> pos.hMSL >> pos.hAcc >> pos.vAcc;
        std::cout << "MSG_GEO_POS: " << static_cast<double>(pos.lon)*1e-7 << " " << static_cast<double>(pos.lat)*1e-7 << " " << static_cast<int>(pos.height) << std::endl;
        return;
    } else if (msgID == TCP_MSG_KEY::MSG_UBX_FIXSTATUS) {
        quint8 val = 0;
        *(tcpMessage.dStream) >> val;
        std::cout << "MSG_UBX_FIXSTATUS: " << Gnss::FixType::name[val] << std::endl;
        return;
    }
}

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("strato-software");
    QCoreApplication::setApplicationVersion("1.0.0");

    QThread* tcpThread = new QThread();
    tcpThread->setObjectName("strato-muonpi-tcp");
    TcpConnection* tcpConnection = new TcpConnection("localhost", 51508);
    tcpConnection->moveToThread(tcpThread);
    QObject::connect(tcpThread, &QThread::started, tcpConnection, &TcpConnection::makeConnection);
    QObject::connect(tcpThread, &QThread::finished, tcpThread, &QThread::deleteLater);
    QObject::connect(tcpThread, &QThread::finished, tcpConnection, &TcpConnection::deleteLater);
    QObject::connect(tcpConnection, &TcpConnection::connected, [](){
        std::cout << "Connection success!" << std::endl;
    });
    QObject::connect(tcpConnection, &TcpConnection::error, [](int socketError, const QString message){
        std::cout << "Error connecting to tcp socket (" << socketError << ") " << message.toStdString() << std::endl; 
    });
    QObject::connect(tcpConnection, &TcpConnection::receivedTcpMessage, &receivedTcpMessage);
    QObject::connect(tcpConnection, &TcpConnection::finished, tcpThread, &QThread::quit);
    tcpThread->start();
    
    auto result = a.exec();
    tcpConnection->closeThisConnection();
    return result;
}
