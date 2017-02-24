#ifndef JSONREQUESTPARSER_H
#define JSONREQUESTPARSER_H


#include <QString>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDebug>
#include <QRegExp>

#include "vissrequest.h"

class JSONRequestParser : public QObject
{
public:
    JSONRequestParser(QString json, bool debug = false, QObject *parent = Q_NULLPTR);
    ~JSONRequestParser();

    VISSRequest* getRequest();

private:
    bool m_debug;
    VISSRequest* m_request;

    void parseJson();
    bool isValidJson();

    bool validateGetRequest();
    bool validateSetRequest();
    bool validateSubscribeRequest();
    bool validateUnsubscribeRequest();
    bool validateUnsubscribeAllRequest();
    bool validateAuthorizeRequest();
    bool validateGetVSSRequest();

    bool validateAction();
    bool validatePath();
    bool validateId();

    bool validateValue();
    bool validateSubscriptionId();
    bool validateFilters();
    bool validateTokens();

    QJsonParseError m_parseError;
    QJsonDocument m_jsonDocument;
    QJsonObject m_jsonObject;

};
#endif // JSONREQUESTPARSER_H