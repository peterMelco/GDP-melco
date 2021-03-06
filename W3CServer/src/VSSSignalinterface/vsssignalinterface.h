#ifndef VSSSIGNALINTERFACE_H
#define VSSSIGNALINTERFACE_H

#include <QJsonObject>
class VSSSignalInterface : public QObject
{
public:
    virtual QString getSignalValue(const QString& path) = 0;
    virtual qint8   setSignalValue(const QString& path, QVariant value) = 0;
    virtual QJsonObject getVSSNode(const QString& path) = 0;
    virtual QJsonObject getVSSTree(const QString& path) = 0;
};
#endif // VSSSIGNALINTERFACE_H
