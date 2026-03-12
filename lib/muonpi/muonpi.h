#ifndef MUONPI_H
#define MUONPI_H

#include <QObject>
#include <memory>
#include <thread>
#include "tcpmessage.h"

class QThread;
class QCoreApplication;
class TcpConnection;
class TcpMessage;

class MUONPI : public QObject
{
    Q_OBJECT

public:
    explicit MUONPI(QObject* parent = nullptr);
    ~MUONPI();

    bool init();
    bool start();
    void stop();

    bool getPosition(double* position);
    bool getXOR(double& rate);
    bool getAND(double& rate);
    bool isConnected();

private slots:
    void receivedTcpMessage(TcpMessage tcpMessage);

private:
    // std::unique_ptr<QCoreApplication> m_ownedApp;
    QThread* m_tcpThread = nullptr;
    TcpConnection* m_tcpConnection = nullptr;
    void setup();
    std::atomic<bool> connectionHealthy = false;

    // std::thread m_qtThread;

    std::atomic<double> geo_pos[3] = {0};
    std::atomic<double> gpio_rate[2] = {0};
};

#endif