// #include "muonpi.h"

// #include <iostream>
// #include <thread>
// #include <chrono>

// #include "tcpconnection.h"
// #include "tcpmessage.h"
// #include "tcpmessage_keys.h"
// #include "ublox_structs.h"

// #include <QCoreApplication>
// #include <QThread>
// #include <QPointF>


// MUONPI::MUONPI(QObject* parent)
//     : QObject(parent)
// {}


// MUONPI::~MUONPI()
// {
//     stop();
// }


// bool MUONPI::init()
// {
//     stop();
//     start();
//     return true;
// }


// bool MUONPI::start()
// {
//     // Prüfen ob bereits eine Qt-App existiert
//     // if (!QCoreApplication::instance())
//     // {
//     // }

//     // m_qtThread = std::thread([this]()
//     // {
//     //     int argc = 0;
//     //     char** argv = nullptr;
//     //     m_ownedApp = std::make_unique<QCoreApplication>(argc, argv);
//     //     QCoreApplication::setApplicationName("strato-software");
//     //     QCoreApplication::setApplicationVersion("1.0.0");

//     //     setup();
//     //     std::cout << "nach setup" << std::endl;

//     //     // Eventloop starten
//     //     m_ownedApp->exec();
//     // });

//     setup();
//     if (m_tcpThread)
//     {
//         m_tcpThread->start(); // Startet Eventloop des TcpThreads
//         std::cout << "TCP thread started: " << m_tcpThread << std::endl;
//     }
//     // std::cout << "nach thread gestartet m_ownedApp->exec()" << std::endl;
//     return true;
// }


// void MUONPI::setup()
// {
//     // qRegisterMetaType<TcpMessage>("TcpMessage");

//     // // m_tcpThread = new QThread(this);
//     // m_tcpThread = new QThread();
//     // // m_tcpThread->setObjectName("strato-muonpi-tcp");

//     // m_tcpConnection = new TcpConnection("localhost", 51508);
//     // // m_tcpConnection->moveToThread(m_tcpThread);

//     // connect(m_tcpThread, &QThread::started, m_tcpConnection, &TcpConnection::makeConnection);

//     // connect(m_tcpConnection, &TcpConnection::receivedTcpMessage, this, &MUONPI::receivedTcpMessage);

//     // connect(m_tcpConnection, &TcpConnection::finished, m_tcpThread, &QThread::quit);

//     // connect(m_tcpThread, &QThread::finished, m_tcpConnection, &QObject::deleteLater);

//     // connect(m_tcpThread, &QThread::finished, m_tcpThread, &QObject::deleteLater);

//     // // m_tcpThread->start();


//     qRegisterMetaType<TcpMessage>("TcpMessage");

//     m_tcpThread = new QThread();

//     m_tcpConnection = new TcpConnection("localhost", 51508);
//     m_tcpConnection->moveToThread(m_tcpThread);

//     connect(m_tcpThread, &QThread::started,
//             m_tcpConnection, &TcpConnection::makeConnection);

//     connect(m_tcpConnection, &TcpConnection::receivedTcpMessage,
//             this, &MUONPI::receivedTcpMessage);

//     connect(m_tcpConnection, &TcpConnection::finished,
//             m_tcpThread, &QThread::quit);

//     connect(m_tcpThread, &QThread::finished,
//             m_tcpConnection, &QObject::deleteLater);

//         m_tcpConnection->makeConnection();
// }



// void MUONPI::stop()
// {
//     if (m_tcpConnection)
//         m_tcpConnection->closeThisConnection();

//     if (m_tcpThread)
//     {
//         m_tcpThread->quit();
//         m_tcpThread->wait();
//     }

//     // if (m_ownedApp)
//     //     QCoreApplication::quit();
    
//     // if (m_qtThread.joinable())
//     //     m_qtThread.join();
//     // std::this_thread::sleep_for(std::chrono::milliseconds(100));
// }


// void MUONPI::receivedTcpMessage(TcpMessage tcpMessage)
// {
//     std::cout << "receivedTcpMessage" << std::endl;

//     TCP_MSG_KEY msgID = static_cast<TCP_MSG_KEY>(tcpMessage.getMsgID());

//     if (msgID == TCP_MSG_KEY::MSG_QUIT_CONNECTION)
//     {
//         std::cout << "TCP QUIT" << std::endl;
//         return;
//     }

//     else if (msgID == TCP_MSG_KEY::MSG_GEO_POS)
//     {
//         GnssPosStruct pos {};
//         *(tcpMessage.dStream) >> pos.iTOW >> pos.lon >> pos.lat >> pos.height >> pos.hMSL >> pos.hAcc >> pos.vAcc;
//         std::cout << "MSG_GEO_POS: " << static_cast<double>(pos.lon)*1e-7 << " " << static_cast<double>(pos.lat)*1e-7 << " " << static_cast<double>(pos.height) / 1e3 << std::endl;
//         geo_pos[0] = static_cast<double>(pos.lat) * 1e-7;
//         geo_pos[1] = static_cast<double>(pos.lon) * 1e-7;
//         geo_pos[2] = static_cast<double>(pos.height) / 1e3;
//         return;
//     }

//     else if (msgID == TCP_MSG_KEY::MSG_UBX_FIXSTATUS)
//     {
//         quint8 val = 0;
//         *(tcpMessage.dStream) >> val;
//         std::cout << "MSG_UBX_FIXSTATUS: " << Gnss::FixType::name[val] << std::endl;
//         return;
//     }

//     else if (msgID == TCP_MSG_KEY::MSG_GPIO_RATE)
//     {
//         quint8 whichRate;
//         QVector<QPointF> rate;
//         *(tcpMessage.dStream) >> whichRate >> rate;
//         float rateYValue;
//         if (!rate.empty())
//         {
//             rateYValue = rate.at(rate.size() - 1).y();
//         }
//         else
//         {
//             rateYValue = 0.0;
//         }
//         std::cout << "Rate " << (whichRate == 0 ? "XOR" : "AND") << " " << rateYValue << std::endl;
//         gpio_rate[whichRate] = static_cast<double>(rateYValue);
//     }
// }


// bool MUONPI::getPosition(double* position)
// {
//     position[0] = geo_pos[0];
//     position[1] = geo_pos[1];
//     position[2] = geo_pos[2];
//     return true;
// }


// bool MUONPI::getXOR(double& rate)
// {
//     rate = gpio_rate[0];
//     return true;
// }


// bool MUONPI::getAND(double& rate)
// {
//     rate = gpio_rate[1];
//     return true;
// }





























#include "muonpi.h"

#include <iostream>
#include <QThread>
#include <QCoreApplication>
#include <QMetaType>
#include "tcpconnection.h"
#include "tcpmessage.h"
#include "tcpmessage_keys.h"
#include "ublox_structs.h"
#include <QPointF>

MUONPI::MUONPI(QObject* parent)
    : QObject(parent),
      m_tcpThread(nullptr),
      m_tcpConnection(nullptr)
{}


MUONPI::~MUONPI()
{
    stop();
}


bool MUONPI::init()
{
    stop();  // sauber vorher stoppen
    return start();
}


bool MUONPI::start()
{
    setup(); // alles einrichten
    if (m_tcpThread)
    {
        m_tcpThread->start(); // Startet Eventloop des TCP-Threads
        std::cout << "TCP thread started: " << m_tcpThread << std::endl;
    }
    return true;
}


void MUONPI::setup()
{
    qRegisterMetaType<TcpMessage>("TcpMessage");

    // TCP-Thread anlegen
    m_tcpThread = new QThread();

    // TcpConnection **ohne Parent** erstellen
    m_tcpConnection = new TcpConnection("localhost", 51508);

    // Connection in den Thread verschieben
    m_tcpConnection->moveToThread(m_tcpThread);

    // Signals verbinden
    connect(m_tcpThread, &QThread::started,
            m_tcpConnection, &TcpConnection::makeConnection);

    connect(m_tcpConnection, &TcpConnection::receivedTcpMessage,
            this, &MUONPI::receivedTcpMessage);

    connect(m_tcpConnection, &TcpConnection::finished,
            m_tcpThread, &QThread::quit);

    connect(m_tcpThread, &QThread::finished,
            m_tcpConnection, &QObject::deleteLater);

    connect(m_tcpThread, &QThread::finished,
            m_tcpThread, &QObject::deleteLater);
}


void MUONPI::stop()
{
    if (m_tcpConnection)
        m_tcpConnection->closeThisConnection();

    if (m_tcpThread)
    {
        m_tcpThread->quit();
        m_tcpThread->wait();
    }

    m_tcpThread = nullptr;
    m_tcpConnection = nullptr;
}


void MUONPI::receivedTcpMessage(TcpMessage tcpMessage)
{
    std::cout << "receivedTcpMessage" << std::endl;
    TCP_MSG_KEY msgID = static_cast<TCP_MSG_KEY>(tcpMessage.getMsgID());

    if (msgID == TCP_MSG_KEY::MSG_QUIT_CONNECTION)
    {
        std::cout << "TCP QUIT" << std::endl;
        return;
    }
    else if (msgID == TCP_MSG_KEY::MSG_GEO_POS)
    {
        GnssPosStruct pos {};
        *(tcpMessage.dStream) >> pos.iTOW >> pos.lon >> pos.lat >> pos.height >> pos.hMSL >> pos.hAcc >> pos.vAcc;
        geo_pos[0] = static_cast<double>(pos.lat) * 1e-7;
        geo_pos[1] = static_cast<double>(pos.lon) * 1e-7;
        geo_pos[2] = static_cast<double>(pos.height) / 1e3;
        std::cout << "MSG_GEO_POS: " << geo_pos[0] << " " << geo_pos[1] << " " << geo_pos[2] << std::endl;
        return;
    }
    else if (msgID == TCP_MSG_KEY::MSG_UBX_FIXSTATUS)
    {
        quint8 val = 0;
        *(tcpMessage.dStream) >> val;
        std::cout << "MSG_UBX_FIXSTATUS: " << Gnss::FixType::name[val] << std::endl;
        return;
    }
    else if (msgID == TCP_MSG_KEY::MSG_GPIO_RATE)
    {
        quint8 whichRate;
        QVector<QPointF> rate;
        *(tcpMessage.dStream) >> whichRate >> rate;
        double rateYValue = !rate.empty() ? rate.last().y() : 0.0;
        gpio_rate[whichRate] = rateYValue;
        std::cout << "Rate " << (whichRate == 0 ? "XOR" : "AND") << " " << rateYValue << std::endl;
    }
}


bool MUONPI::getPosition(double* position)
{
    position[0] = geo_pos[0];
    position[1] = geo_pos[1];
    position[2] = geo_pos[2];
    return true;
}


bool MUONPI::getXOR(double& rate)
{
    rate = gpio_rate[0];
    return true;
}


bool MUONPI::getAND(double& rate)
{
    rate = gpio_rate[1];
    return true;
}
