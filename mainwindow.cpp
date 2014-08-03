#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFile>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);

    QFile file(qApp->applicationDirPath()+"/presets.xml");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this,"Error","Could not open presets.xml",QMessageBox::Ok);
        return;
    }

    QXmlStreamReader xml(&file);
    xml.readNextStartElement();
    while (!xml.atEnd()) {
        qDebug()<<xml.name();

        QString group = xml.attributes().value("title").toString();
        qDebug()<<group;
        if(!group.isEmpty() && xml.attributes().hasAttribute("id")){
            QMenu *menu = new QMenu(group);
            ui->menuPresets->addMenu(menu);
            xml.readNextStartElement();
            addPresetSubMenus(xml,menu);
        }else{
            xml.readNextStartElement();
        }

    }
}

bool MainWindow::isStation(const QXmlStreamReader &xml) const{
    return xml.name().toString().toLower()=="station";
}

void MainWindow::addPresetSubMenus( QXmlStreamReader &xml, QMenu *parent ){

    bool isEndElement = xml.isEndElement();
    if(isEndElement){
        if(xml.readNextStartElement()){
            addPresetSubMenus(xml,parent);
        }
    }

    qDebug()<<xml.attributes().value("title").toString()<<xml.name().toString();
    bool _isStation = isStation(xml);

    if(!_isStation && !isEndElement){
        //The child group also has child groups, add those recursively
        QString title = xml.attributes().value("title").toString();
        //qDebug()<<title;
        QMenu *menu = new QMenu(title);
        parent->addMenu(menu);
        xml.readNextStartElement();
        addPresetSubMenus(xml,menu);
    }else if(_isStation){
        //This child group does not have more child group, add it as as action
        //qDebug()<<xml.name().toString();

        QString url = xml.attributes().value("url").toString();
        QString name = xml.attributes().value("title").toString();
        bool ok;
        int id = xml.attributes().value("id").toInt(&ok);
        if(!ok){
            id=-1;
        }
        if(xml.readNextStartElement()){
            QStringList sources;
            QString tmpname = xml.name().toString().toLower();
            while(tmpname=="source"){
                QString source = xml.readElementText();
                sources.append(source);
                xml.readNextStartElement();
                tmpname = xml.name().toString().toLower();
            }
            //xml.readNextStartElement();
            Station station(name,url,sources,id);
            _stations[id]=station;
            QAction *action = new QAction(name,parent);
            action->setData(id);
            connect(action,SIGNAL(triggered()),this,SLOT(presetTriggered()));
            parent->addAction(action);


            addPresetSubMenus(xml,parent);
        }
    }




}

void MainWindow::presetTriggered(){

}

MainWindow::~MainWindow(){
    delete ui;
}


void MainWindow::on_PlayButton_clicked(){

}

void MainWindow::on_StopButton_clicked(){

}

void MainWindow::on_MuteButton_clicked(){

}
