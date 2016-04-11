#include "ipreceiver.h"
#include "ipsender.h"
#include "xmlparse.h"


sys *ssv_2;

ipreceiver::ipreceiver()
{
    groupAddress = QHostAddress("239.255.43.21");
    ssvSocket = 45454;
    socket = new QUdpSocket(this);
    socket->bind(QHostAddress::AnyIPv4, ssvSocket, QUdpSocket::ShareAddress);
    socket->joinMulticastGroup(groupAddress);

    connect(socket, SIGNAL(readyRead()), this, SLOT(update_status()));

    QTimer* timer2 = new QTimer(this);

    connect(timer2, SIGNAL(timeout()), this, SLOT(pinger()));
    timer2->start(2000);
}

void ipreceiver::populate_struct(sys &sys_1)
{
    ssv_2 = &sys_1;
}

void ipreceiver::update_status()
{
    while (socket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());
        socket->readDatagram(datagram.data(),datagram.size());
        QString data = datagram;

        qDebug() << "Received: " << data;

        QStringList split_data = data.split(";");

        if (split_data[2].toInt() == -1)
        {
            if (!split_data[3].toInt())
            {
                qDebug() << "Received Host Status";
                ssv_2->subsystems[split_data[0].toInt()].hosts[split_data[1].toInt()].status = split_data[4];
            }
            else if (split_data[3].toInt())
            {
                qDebug() << "Received Host Interface Status";
                ssv_2->subsystems[split_data[0].toInt()].hosts[split_data[1].toInt()].hostInterface.status = split_data[4];
            }
        }

        if (split_data[2].toInt() > -1)
        {
            if(!split_data[3].toInt())
            {
                qDebug() << "Received Process Status";
                ssv_2->subsystems[split_data[0].toInt()].hosts[split_data[1].toInt()].processes[split_data[2].toInt()].status = split_data[4];
            }
            else if (split_data[3].toInt())
            {
                qDebug() << "Received Process Interface Status";
                ssv_2->subsystems[split_data[0].toInt()].hosts[split_data[1].toInt()].processes[split_data[2].toInt()].procInterface.status = split_data[4];
            }
        }

        if (split_data[1].toInt() == -1 && split_data[2].toInt() == -1)
        {
            qDebug() << "Received Subsystem Interface Status";
            ssv_2->subsystems[split_data[0].toInt()].subInterface.status = split_data[4];
        }

    }
}

void ipreceiver::pinger()
{
    for(int i = 0; i < ssv_2->index.size(); i++)
    {
        if (QString::compare(ssv_2->index[i].hostdns,"",Qt::CaseSensitive))
        {
            qDebug() << "Pinging host " << ssv_2 ->index[i].hostdns;
            int exitCode = QProcess::execute("ping", QStringList() << "-c1" << ssv_2->index[i].hostdns);
            if (!exitCode)
            {
                qDebug() << "Host Reachable!";
            }
            else
            {
                qDebug() << "Host Unreachable...";
                ssv_2->subsystems[ssv_2->index[i].subindex].hosts[ssv_2->index[i].hostindex].status = "UNREACHABLE";
                qDebug() << ssv_2->index[i].hostdns << " status set to " << ssv_2->subsystems[ssv_2->index[i].subindex].hosts[ssv_2->index[i].hostindex].status;
            }
        }
    }
}

