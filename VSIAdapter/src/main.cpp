#include <QCoreApplication>
#include <QSettings>
#include <QPointer>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include "opendshandler.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //initiating QSettings
    QCoreApplication::setApplicationName("W3CServer");
    QCoreApplication::setOrganizationName("MelcoGOT");
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QPointer<QSettings> settings = new QSettings();
    QSharedPointer<QFileInfo> checkFile(new QFileInfo(settings->fileName()));

    qDebug() << "Try to open settings file: " << settings->fileName();

    //check if settings file exists on local filesystem otherwise copy
    //the default settings file from resources
    if(!checkFile->isFile())
    {
        qDebug() << "No settings file found";

        if(QDir().mkpath(checkFile->absolutePath()))
        {
            qDebug() << "Path exists";

            //could we copy?
            if (QFile::copy(":/W3CServer.ini",settings->fileName()))
            {
                qDebug() << "Default settings file copied";

                //waits for settings file to become available and loads content
                while(!checkFile->isFile());
                settings->sync();
            }
        }
    }

    // Set up connection to Open DS
    const QString vssFile = "/etc/vss_rel_1.json";
    QSharedPointer<VSSSignalInterface> m_vsssInterface = QSharedPointer<VSSSignalInterfaceImpl>(new VSSSignalInterfaceImpl(vssFile));
    QSharedPointer<OpenDSHandler>m_openDSHandler = QSharedPointer<OpenDSHandler>(new OpenDSHandler());
    QObject::connect(m_openDSHandler.data(), &OpenDSHandler::valueChanged, static_cast <VSSSignalInterfaceImpl*>(m_vsssInterface.data()), &VSSSignalInterfaceImpl::updateValue);

    return a.exec(); // start exec loop
}
