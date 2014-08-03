#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFile>
#include <QInputDialog>

//https://github.com/rburchell/groovy
//http://answer.techwikihow.com/66287/playing-h264-video-stream-raspberri-pi-qt.html

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    _settings = new QSettings("settings.ini",QSettings::IniFormat,this);
    this->setWindowTitle("QtInternetRadio");

    ui->MuteCheckBox->setChecked(_settings->value("MuteCheckBox",false).toBool());

    _isPlaying=false;
    _player = new QMediaPlayer(0,QMediaPlayer::StreamPlayback);


    ui->volumeSlider->setRange(0,100);
    ui->volumeSlider->setTickInterval(1);
    ui->volumeSlider->setTracking(true);
    bool ok;
    int vol = _settings->value("volumeSlider",50).toInt(&ok);
    if(!ok){
        vol=50;
    }
    ui->volumeSlider->setValue( vol );





    ui->actionSelect_stations_for_logging->setCheckable(true);
    ui->actionShow_playlist_logging->setCheckable(true);
    ui->actionShow_playlist_logging->setChecked(_settings->value("actionShow_playlist_logging",false).toBool());
    on_actionShow_playlist_logging_toggled(ui->actionShow_playlist_logging->isChecked());




    this->restoreGeometry( _settings->value("geometry",this->geometry()).toByteArray() );
    this->restoreState( _settings->value("windowState").toByteArray() );


    QFile file(qApp->applicationDirPath()+"/presets.xml");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this,"Error","Could not open presets.xml",QMessageBox::Ok);
        return;
    }
    QDomDocument doc("presets");
    if (!doc.setContent(&file)) {
        QMessageBox::critical(this,"Error","Could not set QDomDocument",QMessageBox::Ok);
        return;
    }

    //Get the root element
    QDomElement docElem = doc.documentElement();



    QDomNode n = docElem.firstChild();
    while (!n.isNull()) {
        if (n.isElement()) {
            QDomElement e = n.toElement();
            //qDebug() << "Element name: " << e.tagName();
            QString group = e.attribute("title");
            //qDebug()<<group;
            if(!group.isEmpty() && e.hasAttribute("id")){
                QMenu *menu = new QMenu(group);
                ui->menuPresets->addMenu(menu);
                for(int i=0;i<n.childNodes().size();i++){
                    e=n.childNodes().at(i).toElement();
                    addPresetSubMenus(e,menu);
                }
            }
        }
        n = n.nextSibling();
    }

}


MainWindow::~MainWindow(){
    delete ui;
    delete _player;_player=0;
    delete _settings;_settings=0;
}

void MainWindow::closeEvent(QCloseEvent *e) {

    on_StopButton_clicked();
    _settings->setValue("geometry", saveGeometry());
    _settings->setValue("windowState", saveState());
    _settings->setValue("actionShow_playlist_logging",ui->actionShow_playlist_logging->isChecked());
    _settings->setValue("volumeSlider",ui->volumeSlider->value());
    _settings->setValue("MuteCheckBox",ui->MuteCheckBox->isChecked());

    QMainWindow::closeEvent(e);
}

void MainWindow::on_actionShow_playlist_logging_toggled(bool checked){
    ui->PlayListLoggingGroupBox->setVisible(checked);
}

void MainWindow::on_actionSelect_stations_for_logging_toggled( bool checked ){

    QString str="Stop";
    if(!checked){
        str="Start";
    }
    ui->actionSelect_stations_for_logging->setText(str+" selecting stations for playlist logging");

}


bool MainWindow::isStation(const QDomElement &xml) const{
    return xml.tagName().toLower()=="station";
}

void MainWindow::addPresetSubMenus( QDomElement &xml, QMenu *parent ){



    bool _isStation = isStation(xml);

    if(!_isStation && !xml.isNull()){
        //The child group also has child groups, add those recursively
        QString title = xml.attribute("title");
        //qDebug()<<title;
        QMenu *menu = new QMenu(title);
        parent->addMenu(menu);
        for(int i=0;i<xml.childNodes().size();i++){
            QDomElement childElement = xml.childNodes().at(i).toElement();
            if(childElement.isNull()){
                continue;
            }
            //qDebug()<<childElement.tagName()<<childElement.text();
            if(isStation(childElement)){
                addStation(childElement,menu);
            }else{
                addPresetSubMenus(childElement,menu);
            }
        }
    }else if(_isStation){
        //This child group does not have more child group, add it as as action
        //qDebug()<<xml.name().toString();


        addStation(xml,parent);
        xml=xml.nextSibling().toElement();
        //qDebug()<<xml.tagName();

        addPresetSubMenus(xml,parent);
    }





}

void MainWindow::addStation(QDomElement &xml,QMenu *parent){

    QString url = xml.attribute("url");
    QString name = xml.attribute("title");
    bool ok;
    int id = xml.attribute("id").toInt(&ok);
    if(!ok){
        id=-1;
    }

    QDomNodeList childs = xml.childNodes();
    QStringList sources;QString tmpname;
    for(int i=0;i<childs.size();i++){
        QDomElement se = childs.at(i).toElement();
        if(se.isNull()){
            continue;
        }
        tmpname = se.tagName().toLower();
        if(tmpname=="source"){
            QString source = se.text();
            sources.append(source);
        }
    }

    Station station(name,url,sources,id);
    _stations[id]=station;
    QAction *action = new QAction(name,parent);
    action->setData(id);
    connect(action,SIGNAL(triggered()),this,SLOT(presetTriggered()));
    parent->addAction(action);
}



void MainWindow::presetTriggered(){
    QAction *action = qobject_cast<QAction*>(QObject::sender());
    if(action==0){
        return;
    }
    bool ok;
    int id = action->data().toInt(&ok);
    if(!ok){
        return;
    }
    Station s = _stations[id];
    if(!s.isValid()){
        return;
    }
    qDebug()<<"Radio channel "<<s.name()<<"selected";

    if(ui->actionSelect_stations_for_logging->isChecked()){
        //add channel to list to log and start logging if logging is started
    }else{
        //change channel / play...
        _currentStation = s;
        changeStation();
    }

}

void MainWindow::changeStation(){
    if(_isPlaying){
        //stop and change channel and play if already playing
        on_StopButton_clicked();
        on_PlayButton_clicked();
    }else{
        updateInformation();
    }
}

void MainWindow::on_actionOpen_URL_triggered(){

    bool ok;
    QString url = QInputDialog::getText(this,"Open URL","URL:",QLineEdit::Normal,_settings->value("lastUsedURL","").toString(),&ok);
    if(!ok){
        return;
    }
    _settings->setValue("lastUsedURL",url);

    Station station("",url,QStringList(url),0);
    _currentStation = station;
    changeStation();

}

void MainWindow::on_volumeSlider_valueChanged(){
    _player->setVolume(ui->volumeSlider->value());
    QString str = "Volume = "+QString::number(ui->volumeSlider->value());
    statusBar()->showMessage(str,4000);
    ui->volumeSlider->setToolTip(str);
}



void MainWindow::on_PlayButton_clicked(){

    _isPlaying=true;

    QStringList sources = _currentStation.sources();
    for(int i=0;i<sources.size();i++){
        _player->setMedia(QUrl(sources[i]));
        _player->play();
        if( _player->state()==QMediaPlayer::PlayingState ){
            break;
        }
    }
    if(_player->state()!=QMediaPlayer::PlayingState){
        QMessageBox::critical(this,"Error","Could not start playing any of the specified sources for channel "+_currentStation.name());
    }
    _player->setVolume(ui->volumeSlider->value());
    updateInformation();
}

void MainWindow::on_StopButton_clicked(){

    _isPlaying=false;
    _player->stop();
    updateInformation();
}

void MainWindow::on_MuteCheckBox_toggled(bool checked){
    _player->setMuted(checked);
}


void MainWindow::updateInformation(){
    //Should be called after play/stop has been called and _currentStation set

    QString str=_currentStation.name();
    str.append("\n");
    str.append(_currentStation.url());
    str.append("\n\n");

    if( _isPlaying ){
        QString artist = _player->metaData("AlbumArtist").toString();
        QString album = _player->metaData("AlbumTitle").toString();
        QString title = _player->metaData("Title").toString();
        str.append(artist+" - "+album+" - "+title);

        bool ok;
        int bitRate = _player->metaData("AudioBitRate").toInt(&ok);
        QString bitRateStr = "Unknown bitrate";
        if(ok){
            bitRateStr=QString::number(bitRate/1000)+" kbps";
        }
        QString codec = _player->metaData("AudioCodec").toString();
        QString songInfo = bitRateStr;
        if(!codec.simplified().isEmpty()){
            songInfo.append(", codec: "+codec);
        }
        str.append(songInfo);
    }else{
        str.append("Not playing");
        str.append("\n");
    }

    ui->InfoLabel->setText(str);


}
