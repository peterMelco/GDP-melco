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
#include <QThread>
#include "subscriptions.h"
#include "unsubnotifier.h"

//Initialize static members
Subscriptions* Subscriptions::m_instance = nullptr;
int Subscriptions::m_subscriptionIdCounter = 0;
QMutex Subscriptions::m_mutex;

Subscriptions* Subscriptions::getInstance()
{
    QMutexLocker mutexlock(&m_mutex);
    if (m_instance == nullptr)
    {
        m_subscriptionIdCounter = 0;
        m_instance = new Subscriptions();
     }
    return m_instance;
}

QString Subscriptions::addSubcription(SubscribeHandler* handler)
{
    QMutexLocker lock(&m_mutex);

    m_subscriptionIdCounter++;

    QString subidStr = QString::number(m_subscriptionIdCounter);
    QWebSocket* client = handler -> getSocketClient();
    UnsubNotifier* usubNotifier = new UnsubNotifier(nullptr,handler);
    // connect unsubscription
    connect(usubNotifier,&UnsubNotifier::unsubscribe,handler,&SubscribeHandler::unsubscribe,Qt::QueuedConnection);
    // handle unsubscribe
    m_notifiers.insert(subidStr,usubNotifier);
    //handle unsubscribe all
    m_clientsubscriptions.insert(client,subidStr);

    return QString::number(m_subscriptionIdCounter);
}

bool Subscriptions::unsubscribe(QString subscriptionId,QWebSocket* client)
{
    QMutexLocker lock(&m_mutex);



    if (m_notifiers.contains(subscriptionId))
    {
        UnsubNotifier* notifier = m_notifiers.find(subscriptionId).value();

        notifier -> unsubScribe();
    }
    else
    {
        return false;
    }

    m_notifiers.remove(subscriptionId);
    m_clientsubscriptions.remove(client);

    return true;
}

bool Subscriptions::unsubscribeAll(QWebSocket* client)
{
    QMutexLocker lock(&m_mutex);

    QList<QString> ids = m_clientsubscriptions.values(client);

    if (ids.size() == 0)
    {
        return false;
    }

    for (int i = 0; i < ids.size(); ++i)
    {
        UnsubNotifier* notifier = m_notifiers.find(ids.value(i)).value();
        notifier -> unsubScribe();
    }

    for(int i = 0; i < ids.size(); ++i)
    {
        m_notifiers.remove(ids.value(i));
    }

    m_clientsubscriptions.remove(client);

    return true;
}
