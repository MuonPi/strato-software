
#include "muonpi.h"

#include "tcpconnection.h"
#include "tcpmessage.h"
#include "tcpmessage_keys.h"
#include "ublox_structs.h"
#include <QCoreApplication>
#include <QMetaType>
#include <QPointF>
#include <QThread>
#include <iostream>

constexpr unsigned timeout_ms = 1000;

MUONPI::MUONPI(QObject *parent) : QObject(parent), m_tcpThread(nullptr), m_tcpConnection(nullptr)
{
    qRegisterMetaType<TcpMessage>("TcpMessage");
}

MUONPI::~MUONPI()
{
    stop();
}

void MUONPI::start()
{
    setup(); // alles einrichten
    if (m_tcpThread != nullptr)
    {
        m_tcpThread->start(); // Startet Eventloop des TCP-Threads
    }
}

void MUONPI::setup()
{
    // TCP-Thread anlegen
    m_tcpThread = new QThread();

    // TcpConnection **ohne Parent** erstellen
    m_tcpConnection = new TcpConnection("localhost", 51508, 0, timeout_ms);

    // Connection in den Thread verschieben
    m_tcpConnection->moveToThread(m_tcpThread);

    // Signals verbinden
    connect(m_tcpThread, &QThread::started, m_tcpConnection, &TcpConnection::makeConnection);

    connect(m_tcpConnection, &TcpConnection::receivedTcpMessage, this, &MUONPI::receivedTcpMessage);

    connect(m_tcpConnection, &TcpConnection::finished, m_tcpThread, &QThread::quit);

    connect(m_tcpThread, &QThread::finished, m_tcpConnection, &QObject::deleteLater);
    connect(m_tcpThread, &QThread::finished, m_tcpThread, &QObject::deleteLater);
    connect(m_tcpConnection, &TcpConnection::connectionTimeout, this, [this](QString remotePeerAddress, quint16 remotePeerPort, QString localAddress, quint16 localPort,
        quint32 timeoutTime, quint32 connectionDuration){
            m_tcpThread->quit();
        });
    connect(m_tcpThread, &QThread::finished, this, [this](){
        m_tcpConnection = nullptr;
        m_tcpThread = nullptr;
        connectionHealthy = false;
        QTimer::singleShot(timeout_ms, this, [this]() {
            start();
        });
    });
    connect(m_tcpConnection, &TcpConnection::connected, this, [this]() {
        connectionHealthy = true;
    });
}

void MUONPI::stop()
{
    if (m_tcpConnection != nullptr)
    {
        m_tcpConnection->closeThisConnection();
    }

    if (m_tcpThread != nullptr)
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
        std::cout << "TCP QUIT RECEIVED. Restarting TCP Connection..." << std::endl;
        // Restart TCP connection
        connectionHealthy = false;
        stop(); // will automatically restart due to defined signal
        return;
    }
    else if (msgID == TCP_MSG_KEY::MSG_GEO_POS)
    {
        GnssPosStruct pos{};
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

bool MUONPI::getPosition(double *position)
{
    for(uint8_t i = 0; i < 3; i++)
    {
        position[i] = geo_pos[i];
        geo_pos[i] = -1;
    }
    return true;
}

bool MUONPI::getXOR(double &rate)
{
    rate = gpio_rate[0];
    gpio_rate[0] = -1;
    return true;
}

bool MUONPI::getAND(double &rate)
{
    rate = gpio_rate[1];
    gpio_rate[1] = -1;
    return true;
}

bool MUONPI::isConnected()
{
    return connectionHealthy;
}