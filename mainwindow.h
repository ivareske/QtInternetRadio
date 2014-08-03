#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QXmlStreamReader>
#include <QMultimedia>
#include <QMediaPlayer>
#include <QStringList>

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

private slots:
    void on_PlayButton_clicked();
    void on_StopButton_clicked();
    void on_MuteButton_clicked();

    void presetTriggered();

private:
    bool isStation(const QXmlStreamReader &xml) const;
    void addPresetSubMenus(QXmlStreamReader &xml, QMenu *parent);


    Ui::MainWindow *ui;
    QHash<int,Station> _stations;

};

#endif // MAINWINDOW_H
