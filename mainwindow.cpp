#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mediaPlayer = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    videoWidget = new QVideoWidget(this);

    mediaPlayer->setAudioOutput(audioOutput);
    mediaPlayer->setVideoOutput(videoWidget);

    ui->centralwidget->layout()->addWidget(videoWidget);
    videoWidget->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_nextButton_clicked()
{

}


void MainWindow::on_pauseButton_clicked()
{

}


void MainWindow::on_playButton_clicked()
{

}


void MainWindow::on_backButton_clicked()
{

}


void MainWindow::on_volumeSlider_valueChanged(int value)
{

}


void MainWindow::on_filesList_itemClicked(QListWidgetItem *item)
{

}

