
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
    connectionHealthy = true;
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

    connect(m_tcpConnection, &TcpConnection::finished, this, [&](){
        connectionHealthy = false;
    });
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
        // std::cout << "MSG_GEO_POS: " << geo_pos[0] << " " << geo_pos[1] << " " << geo_pos[2] << std::endl;
        return;
    }
    else if (msgID == TCP_MSG_KEY::MSG_UBX_FIXSTATUS)
    {
        quint8 val = 0;
        *(tcpMessage.dStream) >> val;
        // std::cout << "MSG_UBX_FIXSTATUS: " << Gnss::FixType::name[val] << std::endl;
        return;
    }
    else if (msgID == TCP_MSG_KEY::MSG_GPIO_RATE_AVERAGE)
    {
        quint8 whichRate;
        qreal averageValue;
        *(tcpMessage.dStream) >> whichRate >> averageValue;
        gpio_rate[whichRate] = averageValue;
        // std::cout << "Rate " << (whichRate == 0 ? "XOR" : "AND") << " " << averageValue << std::endl;
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

bool MUONPI::isConnected()
{
    return connectionHealthy;
}