#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentItem(0)
{
    ui->setupUi(this);

    audioOutput = new QAudioOutput(this); // объект аудиовыхода
    mediaPlayer = new QMediaPlayer(this); // медиаплеер для управления воспроизведением
    videoWidget = new QVideoWidget(this); // виджет для отображения видео

    mediaPlayer->setAudioOutput(audioOutput); // устанавливаем аудиовыход для воспроизведения звука
    mediaPlayer->setVideoOutput(videoWidget); // устанавливаем виджет для вывода видео

    // добавляем videoWidget вместо QWidget(vidWidget)
    QVBoxLayout *layout = new QVBoxLayout(ui->vidWidget);
    layout->addWidget(videoWidget);
    ui->vidWidget->setLayout(layout);

    videoWidget->show();

    audioOutput->setVolume(50);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_nextButton_clicked()
{
    currentItem = ui->filesList->currentRow();
    if (currentItem < ui->filesList->count() - 1) {
        ui->filesList->setCurrentRow(currentItem + 1);
        playCurrentItem();

        QString name = ui->filesList->currentItem()->text();
        ui->fileNameLabel->setText(name);
    }
}


void MainWindow::on_pauseButton_clicked()
{
    mediaPlayer->pause();
}


void MainWindow::on_playButton_clicked()
{
    if (ui->filesList->currentItem() == nullptr) {
        return;
    }
    else {
        mediaPlayer->play();

        QString name = ui->filesList->currentItem()->text();
        ui->fileNameLabel->setText(name);
    }
}


void MainWindow::on_backButton_clicked()
{
    int currentRow = ui->filesList->currentRow();
    if (currentRow > 0) {
        ui->filesList->setCurrentRow(currentRow - 1);
        playCurrentItem();

        QString name = ui->filesList->currentItem()->text();
        ui->fileNameLabel->setText(name);
    }
}


void MainWindow::on_volumeSlider_valueChanged(int value)
{
    audioOutput->setVolume(value / 100.0);
}


void MainWindow::on_filesList_itemClicked(QListWidgetItem *item)
{
    QString filePath = item->data(Qt::UserRole).toString(); // код извлекает путь к файлу
    mediaPlayer->setSource(QUrl::fromLocalFile(filePath));

    QString name = ui->filesList->currentItem()->text();
    ui->fileNameLabel->setText(name);
}


void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Открыть медиа"), "",
                                                    tr("Файлы (*.mp4 *.avi *.mkv);;Все файлы (*.*)"));

    // создание объекта QFileInfo с использованием пути к файлу fileName
    QListWidgetItem *item = new QListWidgetItem(QFileInfo(fileName).fileName(), ui->filesList);
    item->setData(Qt::UserRole, fileName); // установка дополнительных сведений о файле
    ui->filesList->addItem(item);

    // метод QMediaPlayer устанавливающий источник медиа для воспроизведения.
    mediaPlayer->setSource(QUrl::fromLocalFile(fileName));
}

void MainWindow::playCurrentItem() {
    QListWidgetItem *currentItem = ui->filesList->currentItem();

    if (currentItem) {
        QString filePath = currentItem->data(Qt::UserRole).toString();
        mediaPlayer->setSource(QUrl::fromLocalFile(filePath));
    }
}

