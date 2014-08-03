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


namespace Ui {
class MainWindow;
}



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
private:
    bool isStation(const QDomElement &xml) const;
    void addPresetSubMenus(QDomElement &xml, QMenu *parent);
    void addStation(QDomElement &xml, QMenu *parent);
    void changeStation();



    Ui::MainWindow *ui;
    QHash<int,Station> _stations;
    QSettings *_settings;
    Station _currentStation;
    QMediaPlayer *_player;
    QList<Station> _favourites;
    bool _isPlaying;




};

#endif // MAINWINDOW_H
