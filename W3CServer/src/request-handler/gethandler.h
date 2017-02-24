#ifndef GETHANDLER_H
#define GETHANDLER_H
#include "requesthandler.h"
#include "vissrequest.h"
#include <QWebSocket>

class GetHandler : public RequestHandler
{
public:
    explicit GetHandler(QObject *parent = 0,VISSRequest* vissrequest=0,QWebSocket* client=0);
    void processRequest();
};

#endif // GETHANDLER_H