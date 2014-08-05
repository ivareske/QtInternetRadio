#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QXmlStreamReader>
#include <QMultimedia>
#include <QMediaPlayer>
#include <QStringList>
#include <QDomDocument>
#include <QDomNode>
#include <QSettings>
#include "filedownloader.h"
#include <QPair>
#include <QString>


namespace Ui {
class MainWindow;
}

class StationId : public QPair<int,QString>{
public:
    StationId(int id=-1,const QString &name="") : QPair<int,QString>(id,name){

    }
    inline bool isValid() const{
        return this->first!=-1 && !this->second.isEmpty();
    }
    operator QVariant() const{
        return QVariant::fromValue(*this);
    }
};

class Station{

public:

    explicit Station(const QString &name="", const QString &url="",const QStringList &sources=QStringList(),const int &id=-1){
        _name=name;
        _url=url;
        _sources=sources;
        _id=id;
    }

    inline QString name() const{
        return _name;
    }
    inline QString url() const{
        return _url;
    }
    inline QStringList sources() const{
        return _sources;
    }
    inline int id() const{
        return _id;
    }
    inline bool isValid() const{
        return _sources.count()>0;
    }

    StationId stationId() const{
        return StationId(_id,_name);
    }

private:
    QString _name,_url;
    QStringList _sources;
    int _id;

};



class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


protected:
    void closeEvent(QCloseEvent *e);

private slots:
    void on_PlayButton_clicked();
    void on_StopButton_clicked();
    void presetTriggered();
    void on_actionSelect_stations_for_logging_toggled(bool checked);
    void on_actionShow_playlist_logging_toggled(bool checked);
    void on_volumeSlider_valueChanged();
    void on_MuteCheckBox_toggled(bool checked);
    void updateInformation();
    void on_actionOpen_URL_triggered();
    void playListDownloaded();
private:
    void readScreamerRadioPresets();
    bool isStation(const QDomElement &xml) const;
    void addScreamerRadioPresetSubMenus(QDomElement &xml, QMenu *parent);
    void addScreamerRadioStation(QDomElement &xml, QMenu *parent);
    void changeStation();
    bool isPlayList(const QString &urlString);
    bool playUrl(const QString &url);



    Ui::MainWindow *ui;
    QHash<StationId,Station> _stations;
    QSettings *_settings;
    Station _currentStation;
    QMediaPlayer *_player;
    QList<Station> _favourites;
    bool _isPlaying;    
    FileDownloader *_fileDownloader;    
    QMediaPlaylist *_playList;



};

Q_DECLARE_METATYPE( StationId )

#endif // MAINWINDOW_H
