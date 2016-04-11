#ifndef IPRECEIVER_H
#define IPRECEIVER_H
#include "ipsender.h"
#include "xmlparse.h"

class ipreceiver : public QObject
{
    Q_OBJECT
public:
    ipreceiver();
    void populate_struct(sys &sys_1);
    QUdpSocket *socket;
    QHostAddress groupAddress;
    int ssvTTL;
    quint16 ssvSocket;

public slots:
    void update_status();
    void pinger();
};

#endif // IPRECEIVER_H
