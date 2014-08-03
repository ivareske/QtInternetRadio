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
            qDebug() << "Element name: " << e.tagName();
            QString group = e.attribute("title");
            qDebug()<<group;
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
            if(xml.isNull()){
                continue;
            }
            qDebug()<<childElement.tagName()<<childElement.text();
            addPresetSubMenus(childElement,menu);
        }
    }else if(_isStation){
        //This child group does not have more child group, add it as as action
        //qDebug()<<xml.name().toString();

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
        xml=xml.nextSibling().toElement();
        qDebug()<<xml.tagName();
        Station station(name,url,sources,id);
        _stations[id]=station;
        QAction *action = new QAction(name,parent);
        action->setData(id);
        connect(action,SIGNAL(triggered()),this,SLOT(presetTriggered()));
        parent->addAction(action);


        addPresetSubMenus(xml,parent);
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
