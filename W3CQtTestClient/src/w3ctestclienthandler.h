#ifndef W3CTESTCLIENTHANDLER_H
#define W3CTESTCLIENTHANDLER_H

#include <QThread>
#include <QXmlStreamWriter>
#include <QFile>
#include <QMutex>

#include "w3ctestclient.h"
#include "clientreport.h"

Q_DECLARE_METATYPE(QQueue<TestCase>)

class W3cTestClientHandler: public QObject
{
    Q_OBJECT
public:
    W3cTestClientHandler(int nrOfClients, QQueue<TestCase> tests, QString url, QString swversion, QString timestamp, bool randomize, const QString &reportDir);
    ~W3cTestClientHandler();

public slots:
    void handleTestClientCompletion(ClientReport* report);

signals:
    void startClients();

private:

    void writeXMLReport(QString filename);
    bool writeHTMLReport(const QString &filename);


    QQueue<TestCase> m_testCases;
    QVector<QThread*> m_clients;
    QList<ClientReport*> m_finishedClients;

    QString m_url;
    QString m_swversion;
    QString m_timestamp;
    bool m_randomize;
    QString m_reportDir;
    //int m_finishedClients = 0;

    QMutex m_mutex;
};

#endif // W3CTESTCLIENTHANDLER_H
