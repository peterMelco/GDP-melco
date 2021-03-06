#ifndef GETVISSTESTDATAJSON_H
#define GETVISSTESTDATAJSON_H
#include <QString>

enum requesttype {GET=1,SET,SUBSCRIBE,UNSUBSCRIBE,UNSUBSCRIBEALL,GETVSS,AUTHORIZE,STATUS};

class GetVissTestDataJson
{
public:
    GetVissTestDataJson();
    static QString getTestDataString(requesttype type,QString subId = "1");

    // return a JSON {"action":"authorize...}
    static QString getAuthJson();
    static QString getSubscriptionJson();
    static QString getUnsubscribe(QString subscriptionId);
    static QString getUnsubscribeAll();
    static QString getGetJson();
    static QString getSetJson();
    static QString getGetVssJson();
    static QString getSetValue();
    static QString getStatusJson();
    static int m_requestId;
private:
    static QString m_setValue;
};

#endif // GETVISSTESTDATAJSON_H
