#ifndef QXMLSETTINGS_H
#define QXMLSETTINGS_H

#include <QtGui>

class QXmlSettings : public QSettings{
    Q_OBJECT
public:    
    explicit QXmlSettings(const QString &fileName, QObject *parent=0);

signals:

public slots:

private:
};

bool writeXmlFile(QIODevice &device, const QSettings::SettingsMap &map);
bool readXmlFile(QIODevice &device, QSettings::SettingsMap &map);

#endif // QXMLSETTINGS_H
