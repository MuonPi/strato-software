#ifndef MUONPI_H
#define MUONPI_H

#include <QObject>
#include <memory>
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
    bool start();     // startet alles
    void stop();      // optional sauber beenden

    bool getPosition(double* position);
    bool getXOR(double& rate);
    bool getAND(double& rate);

private slots:
    void receivedTcpMessage(TcpMessage tcpMessage);

private:
    std::unique_ptr<QCoreApplication> m_ownedApp;
    QThread* m_tcpThread = nullptr;
    TcpConnection* m_tcpConnection = nullptr;
    void setup();

    double geo_pos[3] = {0};
    double gpio_rate[2] = {0};
};

#endif