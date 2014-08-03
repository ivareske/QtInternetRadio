#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMultimedia>
#include <QMediaPlayer>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
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
