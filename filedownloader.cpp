#include "filedownloader.h"

FileDownloader::FileDownloader(QObject *parent) : QObject(parent){

    connect(&m_WebCtrl, SIGNAL(finished(QNetworkReply*)),SLOT(fileDownloaded(QNetworkReply*)));

}

FileDownloader::~FileDownloader(){

}


void FileDownloader::fileDownloaded(QNetworkReply* pReply){
    m_DownloadedData = pReply->readAll();
    //emit a signal
    pReply->deleteLater();
    emit downloaded();
}

void FileDownloader::download(QUrl url){
    _url = url;
    QNetworkRequest request(url);
    m_WebCtrl.get(request);
}

QByteArray FileDownloader::downloadedData() const{
    return m_DownloadedData;
}
