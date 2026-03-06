#include "muonpi.h"

#include <iostream>
#include <thread>
#include <chrono>

#include "tcpconnection.h"
#include "tcpmessage.h"
#include "tcpmessage_keys.h"
#include "ublox_structs.h"

#include <QCoreApplication>
#include <QThread>
#include <QPointF>


MUONPI::MUONPI(QObject* parent)
    : QObject(parent)
{}


MUONPI::~MUONPI()
{
    stop();
}


bool MUONPI::init()
{
    stop();
    start();
    return true;
}


bool MUONPI::start()
{
    // Prüfen ob bereits eine Qt-App existiert
    // if (!QCoreApplication::instance())
    // {
    // }

    int argc = 0;
    char** argv = nullptr;
    m_ownedApp = std::make_unique<QCoreApplication>(argc, argv);
    QCoreApplication::setApplicationName("strato-software");
    QCoreApplication::setApplicationVersion("1.0.0");

    setup();
    std::cout << "nach setup" << std::endl;

    // Eventloop starten
    if (m_ownedApp)
        // m_ownedApp->exec();
        return m_ownedApp->exec();

    std::cout << "nach m_ownedApp->exec()" << std::endl;

    return true;
}


void MUONPI::setup()
{
    qRegisterMetaType<TcpMessage>("TcpMessage");

    m_tcpThread = new QThread(this);
    // m_tcpThread = new QThread();
    m_tcpThread->setObjectName("strato-muonpi-tcp");

    m_tcpConnection = new TcpConnection("localhost", 51508);
    m_tcpConnection->moveToThread(m_tcpThread);

    connect(m_tcpThread, &QThread::started, m_tcpConnection, &TcpConnection::makeConnection);

    connect(m_tcpConnection, &TcpConnection::receivedTcpMessage, this, &MUONPI::receivedTcpMessage);

    connect(m_tcpConnection, &TcpConnection::finished, m_tcpThread, &QThread::quit);

    connect(m_tcpThread, &QThread::finished, m_tcpConnection, &QObject::deleteLater);

    connect(m_tcpThread, &QThread::finished, m_tcpThread, &QObject::deleteLater);

    m_tcpThread->start();
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

    if (m_ownedApp)
        QCoreApplication::quit();
    
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
        std::cout << "MSG_GEO_POS: " << static_cast<double>(pos.lon)*1e-7 << " " << static_cast<double>(pos.lat)*1e-7 << " " << static_cast<double>(pos.height) / 1e3 << std::endl;
        geo_pos[0] = static_cast<double>(pos.lat) * 1e-7;
        geo_pos[1] = static_cast<double>(pos.lon) * 1e-7;
        geo_pos[2] = static_cast<double>(pos.height) / 1e3;
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
        float rateYValue;
        if (!rate.empty())
        {
            rateYValue = rate.at(rate.size() - 1).y();
        }
        else
        {
            rateYValue = 0.0;
        }
        std::cout << "Rate " << (whichRate == 0 ? "XOR" : "AND") << " " << rateYValue << std::endl;
        gpio_rate[whichRate] = static_cast<double>(rateYValue);
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
