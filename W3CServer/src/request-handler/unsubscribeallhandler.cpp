/**************************************************************************************************************
* Copyright (C) *   2017, peterwinzell
*
*
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.*
*
*
***************************************************************************************************************/
#include <QJsonObject>
#include <QJsonDocument>
#include "unsubscribeallhandler.h"
#include "subscriptions.h"

UnsubscribeAllHandler::UnsubscribeAllHandler(QObject* parent, QSharedPointer<VSSSignalInterface> signalInterface, QSharedPointer<VISSRequest> vissrequest,
        WebSocketWrapper *client): RequestHandler(parent, signalInterface, vissrequest,client)
{
    TRACE("Server", "< UnsubscribeAllHandler > created.");
}

QString UnsubscribeAllHandler::responseBuilder(bool valid)
{
    QJsonObject jsonresponse;
    jsonresponse.insert("action","unsubscribeAll");
    jsonresponse.insert("subscriptionId","null");
    jsonresponse.insert("requestId",m_pVissrequest -> getRequestId());

    if (!valid)
    {
        QJsonObject errorObject;
        errorObject.insert("number",404);
        errorObject.insert("reason","invalid_unsubscription"); // should probably collect error info here, adding failed unsub/subId
        errorObject.insert("message","The specified subscription was not found. ");
        jsonresponse.insert("error",errorObject);
    }

    jsonresponse.insert("timestamp",(int)QDateTime::currentDateTime().toTime_t() );

    QJsonDocument jsonDoc(jsonresponse);
    return jsonDoc.toJson();
}

void UnsubscribeAllHandler::processRequest()
{
    DEBUG("Server","Processing < UnsubscribeAll > request.");

    Subscriptions* subs = Subscriptions::getInstance();
    bool valid = true;
    if (subs)
    {
        valid = subs -> unsubscribeAll(m_pClient); // should kill subscription thread
    }

    m_pClient -> sendTextMessage( responseBuilder(valid) );
}
