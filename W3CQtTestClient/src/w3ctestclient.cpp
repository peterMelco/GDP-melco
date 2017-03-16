#include "w3ctestclient.h"
#include "getvisstestdatajson.h"
#include <QtCore/QDebug>
#include <QtWebSockets/QWebSocket>
#include <QCoreApplication>
#include <QJsonParseError>
#include <QTime>
#include <QJsonObject>
#include <QTimer>

QT_USE_NAMESPACE

W3cTestClient::W3cTestClient(const QUrl &url, QObject *parent) :
    QObject(parent), m_url(url)
{
    qRegisterMetaType<TestResult>();
}

W3cTestClient::~W3cTestClient()
{
    if(m_webSocket->isValid()) { m_webSocket->close(); }
    m_webSocket->deleteLater();
}

void W3cTestClient::startClient(QQueue<TestCase> tests)
{
    qDebug() << " Starting Client";
    m_tests = tests;
    m_webSocket = new QWebSocket();

    connect(m_webSocket, &QWebSocket::connected, this, &W3cTestClient::onConnected);
    typedef void (QWebSocket:: *sslErrorsSignal)(const QList<QSslError> &);
    connect(m_webSocket, static_cast<sslErrorsSignal>(&QWebSocket::sslErrors), this, &W3cTestClient::onSslErrors);

    m_webSocket->open(QUrl(m_url));
}

void W3cTestClient::onConnected()
{
    qDebug() << "WebSocket connected";
    connect(m_webSocket, &QWebSocket::textMessageReceived,
            this, &W3cTestClient::onTextMessageReceived);

    runTest();
}

void W3cTestClient::runTest()
{
    m_currentTest = m_tests.dequeue();

    m_currentTestResult = new TestResult();
    m_currentTestResult->testcase = m_currentTest;
    m_currentTestResult->started = QDateTime::currentDateTime(); // Now?

    switch (m_currentTest)
    {
        case TestCase::SUBSCRIBE_UNSUBSCRIBE:
            RunSubscribeUnsubscribeTest();
            break;

        case TestCase::SUBSCRIBEALL_UNSUBSCRIBEALL:
            RunSubscribeUnsubscribeAllTest();
            break;

        case TestCase::GET_VSS:
            RunGetVssTest();
            break;

        case TestCase::SET_GET:
            break;

        case TestCase::AUTHORIZE_SUCCESS:
            RunAuthorizeTest();
            break;

        default:
            break;
    }
}

void W3cTestClient::onTextMessageReceived(QString message)
{
    //qDebug() << "Message received:" << message << "\n";
    //parse message
    QJsonParseError parseError;
    QJsonDocument  jsonDocument;
    QJsonObject    jsonObject;

    jsonDocument = QJsonDocument::fromJson(message.toUtf8(),&parseError);
    jsonObject = jsonDocument.object();
    if (parseError.error == QJsonParseError::NoError)
    {
        QString actionString =jsonObject["action"].toString();

        if (actionString == "subscribe")
        {
            if(m_currentTest != TestCase::SUBSCRIBE_UNSUBSCRIBE && m_currentTest != TestCase::SUBSCRIBEALL_UNSUBSCRIBEALL)
            {
                qDebug() << "" << "Received Subcribe action when not requested";

                failTestRun();
                return;
            };

            QString path = jsonObject["path"].toString();

            //cache last received subscribeId
            m_unsubscribeCachedSubscriptionId = jsonObject["subscriptionId"].toString();

            QString requestid = jsonObject["requestId"].toString();
            qDebug() << " Succesfully subscribed to " << path  << ", Sub ID: "<< m_unsubscribeCachedSubscriptionId ;
            //qDebug() << " requestId is : " + requestid << " \n";
        }
        else if (actionString == "unsubscribe")
        {
            if(m_currentTest != TestCase::SUBSCRIBE_UNSUBSCRIBE)
            {
                qDebug() << " Received Unsubscribe action when not requested";

                failTestRun();
                return;
            };

            QString  subscriptionId = jsonObject["subscriptionId"].toString();
            QJsonObject errorObject = jsonObject["error"].toObject();
            if (!errorObject.empty())
            {
                QString errorMessage = errorObject["message"].toString();
                qDebug() << errorMessage + " SubID: " << subscriptionId;

                failTestRun();
            }
            else
            {
                qDebug() << " Succesfully unsuscribed to Sub ID: " << subscriptionId;

                passTestRun();
            }
        }
        else if (actionString == "unsubscribeAll")
        {
            if(m_currentTest != TestCase::SUBSCRIBEALL_UNSUBSCRIBEALL)
            {
                qDebug() << " Received Unsubscribe All action when not requested";

                failTestRun();
                return;
            };


            QString requestId = jsonObject["requestId"].toString();
            QJsonObject errorObject = jsonObject["error"].toObject();
            if (!errorObject.empty())
            {
                QString errorMessage = errorObject["message"].toString();
                qDebug() << errorMessage + " request was :" << requestId;

                failTestRun();
            }
            else
            {
                qDebug() << " Succesfully performed unsubscribe all ";

                passTestRun();
            }

        }
        else if (actionString == "subscribing")
        {
            if(m_currentTest != TestCase::SUBSCRIBE_UNSUBSCRIBE && m_currentTest != TestCase::SUBSCRIBEALL_UNSUBSCRIBEALL)
            {
                qDebug() << " Received Subscribing action when not requested";

                failTestRun();
                return;
            };

            QString subId = jsonObject["subscriptionId"].toString();
            QString valueStr  =  jsonObject["value"].toString();
            int value = valueStr.toInt();
            QString timeString  = jsonObject["timestamp"].toString();
            QDateTime time_t = QDateTime::fromTime_t(timeString.toInt());

            //qDebug() << " subscriptionId is : " + subId << " \n";
            //qDebug() << " currentTime is : " + time_t.toString() << " \n";
            //qDebug() << " The value is :   " << value << " \n";

            qDebug() << " Received Subscribing message";
            //passTestRun(); //Add a counter of some sort before we pass?
        }
        else if (actionString == "getVSS")
        {
            if(m_currentTest != TestCase::GET_VSS)
            {
                qDebug() << " Received Get VSS action when not requested";

                failTestRun();
                return;
            };

            QString requestId = jsonObject["requestId"].toString();
            QJsonObject errorObject = jsonObject["error"].toObject();
            if (!errorObject.empty())
            {
                QString errorMessage = errorObject["message"].toString();
                qDebug() << errorMessage + " request was :" << requestId;

                failTestRun();
            }
            else
            {
                //QJsonObject vssObject = jsonObject["vss"].toObject();
                qDebug() << " VSS data received";// << vssObject;
                passTestRun();
            }
        }
        else if (actionString == "authorize")
        {
            if(m_currentTest != TestCase::AUTHORIZE_SUCCESS)
            {
                qDebug() << " Received Authorize action when not requested";

                failTestRun();
                return;
            };

            QString requestId = jsonObject["requestId"].toString();
            QJsonObject errorObject = jsonObject["error"].toObject();
            if (!errorObject.empty())
            {
                qDebug() << " Failed to authorize";
                QString errorMessage = errorObject["message"].toString();
                qDebug() << errorMessage + " request was :" << requestId;

                failTestRun();
            }
            else
            {
                qDebug() << " Successfully authorized";
                //qDebug() << jsonObject;

                passTestRun();
            }
        }
        else if (actionString == "get")
        {
            if(m_currentTest != TestCase::SET_GET)
            {
                qDebug() << " Received Get action when not requested";

                failTestRun();
                return;
            };

            // Check if error
            passTestRun();
        }
        else if (actionString == "set")
        {
            if(m_currentTest != TestCase::SET_GET)
            {
                qDebug() << " Received Set action when not requested";

                failTestRun();
                return;
            };

            // Check error
            // Send Get Request
        }
        else
        {
            qDebug() << " Received unknown Request " << actionString;
            failTestRun();
        }
    }
    else
    {
        qDebug() << " JSON Parse Error " << parseError.errorString();
        failTestRun();
    }
}


/**
 * @brief W3cTestClient::RunSubscribeUnsubscribeTest
 * Testing subscribe and unsubscribe and unsubscribeall scenario
 */
void W3cTestClient::RunSubscribeUnsubscribeTest()
{
    qDebug() << " Running Subscribe + Unsubscribe Test \n";
    m_currentTestResult->description = "Subscribe + Unsubscribe ... add description...";

    QString subMess = GetVissTestDataJson::getTestDataString(requesttype::SUBSCRIBE);
    m_webSocket->sendTextMessage(subMess);
    QTimer::singleShot(10000,this,SLOT(unsubscribe()));
}

void W3cTestClient::RunSubscribeUnsubscribeAllTest()
{
    qDebug() << " Running Subscribe + Unsubscribe All Test \n";
    m_currentTestResult->description = "Subscribe + Unsubscribe All ... add description...";

    QString subMess1 = GetVissTestDataJson::getTestDataString(requesttype::SUBSCRIBE);
    QString subMess2 = GetVissTestDataJson::getTestDataString(requesttype::SUBSCRIBE);
    QString subMess3 = GetVissTestDataJson::getTestDataString(requesttype::SUBSCRIBE);

    //do 3 subscriptions
    m_webSocket->sendTextMessage(subMess1);
    m_webSocket->sendTextMessage(subMess2);
    m_webSocket->sendTextMessage(subMess3);

    QTimer::singleShot(10000,this,SLOT(unsubscribeAll()));
}

void W3cTestClient::RunGetVssTest()
{
    qDebug() << " Running Get VSS Test \n";
    m_currentTestResult->description = "Get VSS ... add description...";

    QString subMess = GetVissTestDataJson::getTestDataString(requesttype::GETVSS);
    m_webSocket->sendTextMessage(subMess);
}

void W3cTestClient::RunAuthorizeTest()
{
    qDebug() << " Running Authorize Test \n";
    m_currentTestResult->description = "Authorize ... add description...";

    QString dataJson = GetVissTestDataJson::getTestDataString(requesttype::AUTHORIZE);
    m_webSocket->sendTextMessage(dataJson);
}


void W3cTestClient::passTestRun()
{
    m_currentTestResult->outcome = true;
    m_currentTestResult->ended = QDateTime::currentDateTime();

    // Handle special cases here also, if needed

    emit testresult(m_currentTestResult);

    if(m_tests.length() > 0) { runTest(); }
    else { emit testsfinished(); }
}
void W3cTestClient::failTestRun()
{
    m_currentTestResult->outcome = false;
    m_currentTestResult->ended = QDateTime::currentDateTime();

    // Handle special cases here also, if needed.

    emit testresult(m_currentTestResult);

    if(m_tests.length() > 0) { runTest(); }
    else { emit testsfinished(); }
}


void W3cTestClient::unsubscribe()
{
    qDebug() << " Unsubscribing" << m_unsubscribeCachedSubscriptionId << "\n";
    QString unsubMess = GetVissTestDataJson::getTestDataString(requesttype::UNSUBSCRIBE,m_unsubscribeCachedSubscriptionId);
    m_webSocket->sendTextMessage(unsubMess);
}

void W3cTestClient::unsubscribeAll()
{
    qDebug() << " Unsubscribing All\n";
    QString usubscribeAllMess = GetVissTestDataJson::getTestDataString(requesttype::UNSUBSCRIBEALL);
    m_webSocket->sendTextMessage(usubscribeAllMess);
}

void W3cTestClient::onSslErrors(const QList<QSslError> &errors)
{
    Q_UNUSED(errors);

    // WARNING: Never ignore SSL errors in production code.
    // The proper way to handle self-signed certificates is to add a custom root
    // to the CA store.

    foreach( const QSslError &error, errors )
    {
        //qDebug() << "SSL Error: " << error.errorString();
    }


    m_webSocket->ignoreSslErrors();
}
