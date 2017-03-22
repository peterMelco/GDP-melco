#include <QtCore/QCoreApplication>
#include "w3ctestclient.h"
#include "w3ctestclienthandler.h"

#include <QDebug>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    qDebug() << "Client: main started";
    QCoreApplication a(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("Melco Gothenburg W3C VISS Reference Implementation - Test Client Application");
    parser.addHelpOption();
    //parser.addVersionOption();

    parser.addPositionalArgument("tests", QCoreApplication::translate("main",
                                 "Test cases to run, [subscribe, subscribeall, authorize, getvss, getset, status]. Each test case can be entered several times.")); // TODO Add list to -help

    QCommandLineOption randomizeOption(QStringList() << "r" << "random", QCoreApplication::translate("main",
                                       "Shuffles the order the test cases are excecuted in. The order will be different on each client."));
    parser.addOption(randomizeOption);

    QCommandLineOption secureOption(QStringList() << "s" << "secure", QCoreApplication::translate("main",
                                    "Use Secure Web Sockets. Does nothing at the moment since we are able to ignore all errors."));
    parser.addOption(secureOption);

    QCommandLineOption clientOption(QStringList() << "c" << "clients", QCoreApplication::translate("main", "Number of clients to use."),
                                    QCoreApplication::translate("main", "nr of clients"));
    parser.addOption(clientOption);

    QCommandLineOption urlOption(QStringList() << "url", QCoreApplication::translate("main", "Full W3CServer target implementation url."),
                                 QCoreApplication::translate("main", "url"));
    parser.addOption(urlOption);

    QCommandLineOption softwareOption(QStringList() << "software", QCoreApplication::translate("main",
                                      "Software version the test is run upon. When run from Bamboo this is the git commit id."),
                                      QCoreApplication::translate("main", "version"));
    parser.addOption(softwareOption);

    QCommandLineOption timestampOption(QStringList() << "timestamp", QCoreApplication::translate("main",
                                       "Timestamp when the test were initialized. When run from bamboo this is the same timestamp found in the image created."),
                                       QCoreApplication::translate("main", "timestamp"));
    parser.addOption(timestampOption);


    parser.process(a);

    int nrOfClients = 2; // Default number of clients
    if(parser.isSet(clientOption))
    {
        nrOfClients = parser.value(clientOption).toInt();    // Needs better validation
    }

    QQueue<TestCase> tests;
    for(auto test : parser.positionalArguments())
    {
        if (test == "subscribe")
        {
            tests << TestCase::SUBSCRIBE_UNSUBSCRIBE;
        }
        else if (test == "subscribeall")
        {
            tests << TestCase::SUBSCRIBEALL_UNSUBSCRIBEALL;
        }
        else if (test == "authorize")
        {
            tests << TestCase::AUTHORIZE_SUCCESS;
        }
        else if (test == "getvss")
        {
            tests << TestCase::GET_VSS;
        }
        else if (test == "setget")
        {
            tests << TestCase::SET_GET;
        }
        else if (test == "set")
        {
            tests << TestCase::SET;
        }
        else if (test == "get")
        {
            tests << TestCase::GET;
        }
        else if (test == "status")
        {
            tests << TestCase::STATUS;
        }

        else if (test == "get")
        {
            tests << TestCase::GET;
        }
        else
        {
            qDebug() << "Unknown argument: " << test;
            QCoreApplication::exit(-1);
        }
    }

    if(tests.length() == 0)
    {
        //tests << TestCase::AUTHORIZE_SUCCESS;


        //tests << TestCase::GET_VSS;

        //tests << TestCase::SUBSCRIBE_UNSUBSCRIBE;

        //tests << TestCase::AUTHORIZE_SUCCESS;

        tests << TestCase::STATUS;
    }
    bool randomize = parser.isSet(randomizeOption);
    bool secure = parser.isSet(secureOption);


    QString url = "wss://127.0.0.1:8080"; // default url
    // Is url set, change url. If not, and secure is set, set to secure url, else use default url.


    if(parser.isSet(urlOption))
    {
        url = parser.value(urlOption);
    }
    else if(secure)
    {
        url = "wss://127.0.0.1:8080"; // default secure url
    }

    qDebug() << "nrOfClients: " << nrOfClients << " randomize: " << randomize << " secure: " << secure << " url: " << url;

    QString swversion = parser.value(softwareOption);
    QString timestamp = parser.value(timestampOption);
    W3cTestClientHandler handler(nrOfClients, tests, url, swversion,timestamp,randomize);


    Q_UNUSED(handler);
    return a.exec();
}
