#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class FileDownloader : public QObject
{
    Q_OBJECT
public:
    explicit FileDownloader(QObject *parent = 0);

    virtual ~FileDownloader();

    QByteArray downloadedData() const;

    void download(QUrl url);

    inline QUrl url() const{
        return _url;
    }
signals:
        void downloaded();

private slots:

    void fileDownloaded(QNetworkReply* pReply);

private:

    QNetworkAccessManager m_WebCtrl;
    QUrl _url;
    QByteArray m_DownloadedData;

};

#endif // FILEDOWNLOADER_H
