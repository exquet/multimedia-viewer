#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentItem(0)
    , duration(0)
    , isFullScreen(false)
{
    ui->setupUi(this);

    //icons
    QIcon logoIcon("C:/Users/dimat/Documents/QT projects/multimdia/icons/logo.png");
    MainWindow::setWindowIcon(logoIcon);
    QIcon fileIcon("C:/Users/dimat/Documents/QT projects/multimdia/icons/file.png");
    ui->actionOpen->setIcon(fileIcon);
    QIcon playIcon("C:/Users/dimat/Documents/QT projects/multimdia/icons/play.png");
    ui->playButton->setIcon(playIcon);
    QIcon pauseIcon("C:/Users/dimat/Documents/QT projects/multimdia/icons/pause.png");
    ui->pauseButton->setIcon(pauseIcon);
    QIcon backIcon("C:/Users/dimat/Documents/QT projects/multimdia/icons/back.png");
    ui->backButton->setIcon(backIcon);
    QIcon nextIcon("C:/Users/dimat/Documents/QT projects/multimdia/icons/next.png");
    ui->nextButton->setIcon(nextIcon);
    QIcon fsIcon("C:/Users/dimat/Documents/QT projects/multimdia/icons/fullscreen.png");
    ui->fullScreenButton->setIcon(fsIcon);

    audioOutput = new QAudioOutput(this); // объект аудиовыхода
    mediaPlayer = new QMediaPlayer(this); // медиаплеер для управления воспроизведением
    videoWidget = new QVideoWidget(this); // виджет для отображения видео
    videoWidget->installEventFilter(this);

    mediaPlayer->setAudioOutput(audioOutput); // устанавливаем аудиовыход для воспроизведения звука
    mediaPlayer->setVideoOutput(videoWidget); // устанавливаем виджет для вывода видео

    // добавляем videoWidget вместо QWidget(vidWidget)
    QVBoxLayout *layout = new QVBoxLayout(ui->vidWidget);
    layout->addWidget(videoWidget);
    ui->vidWidget->setLayout(layout);

    videoWidget->show();

    audioOutput->setVolume(50);

    ui->filesList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->filesList, &QListWidget::customContextMenuRequested, this, &MainWindow::showContextMenu);

    timeLabel = ui->timeLabel;
    timeLabel->setText("0:00 / 0:00");

    positionSlider = ui->timeSlider;
    positionSlider->setRange(0, 0);
    connect(positionSlider, &QSlider::sliderMoved, this, &MainWindow::setPosition);

    // соединение сигнала изменения позиции со слотом updatePosition
    connect(mediaPlayer, &QMediaPlayer::positionChanged, this, &MainWindow::updatePosition);
    // соединение сигнала изменения длительности со слотом updateDuration
    connect(mediaPlayer, &QMediaPlayer::durationChanged, this, &MainWindow::updateDuration);

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
        mediaPlayer->play();
    }
}

void MainWindow::showContextMenu(const QPoint &point) {
    // проверка на выбранный эллемент
    QListWidgetItem *item = ui->filesList->itemAt(point);
    if(!item){return;}

    QMenu contextMenu(tr("Выберете действие"), this);
    QAction *deleteAction = contextMenu.addAction(tr("Удалить"));

    // преобразует эти локальные координаты в глобальные координаты экрана и отображат меню
    QAction *selectedAction = contextMenu.exec(ui->filesList->mapToGlobal(point));

    if (selectedAction == deleteAction) {
        deleteSelectedItem();
    }
}

void MainWindow::deleteSelectedItem() {
    int currentRow = ui->filesList->currentRow();
    if (currentRow >= 0) {
        delete ui->filesList->takeItem(currentRow);

        if (ui->filesList->count() == 0) {
            mediaPlayer->stop();
            ui->fileNameLabel->setText("File name");
        }
        else if (currentRow < ui->filesList->count()) {
            ui->filesList->setCurrentRow(currentRow);
            playCurrentItem();
            QString name = ui->filesList->currentItem()->text();
            ui->fileNameLabel->setText(name);
        }
        else if (currentRow == ui ->filesList->count() && ui->filesList->count() > 0) {
            ui->filesList->setCurrentRow(currentRow-1);
            playCurrentItem();
            QString name = ui->filesList->currentItem()->text();
            ui->fileNameLabel->setText(name);
        }

    }
}

void MainWindow::updatePosition(qint64 position) {
    // если пользователь не перетаскивает Slider то обновляем его позицию
    if (!positionSlider->isSliderDown()) {
        positionSlider->setValue(static_cast<int>(position));
    }

    QTime currentTime(0, 0);
    currentTime = currentTime.addMSecs(static_cast<int>(position)); // текущая позиция
    QTime totalTime(0, 0);
    totalTime = totalTime.addMSecs(static_cast<int>(duration)); // общая длительность

    //если длительность > 1 часа, то "h:mm:ss", иначе "m:ss"
    QString format = (duration > 3600000) ? "h:mm:ss" : "m:ss";
    timeLabel->setText(currentTime.toString(format) + " / " + totalTime.toString(format));
}

void MainWindow::updateDuration(qint64 newDuration) {
    duration = newDuration;
    positionSlider->setRange(0, static_cast<int>(newDuration));
}

void MainWindow::setPosition(int position) {
    mediaPlayer->setPosition(position);
}

void MainWindow::on_fullScreenButton_clicked()
{
    if (!isFullScreen) {
        // сохраняем нормальное состояние окна
        setWindowState(windowState() | Qt::WindowFullScreen);
        // выводим видеовиджет поверх остальных элементов и на всё окно
        videoWidget->setParent(nullptr); // "отсоединение" от  изначального QWidget
        videoWidget->setWindowFlags(Qt::Window); // ::Window - будет ввести себя как самостоятельное окно

        videoWidget->showFullScreen();
        isFullScreen = true;
    } else {
        exitFullScreen();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape && isFullScreen) {
        on_fullScreenButton_clicked();
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::exitFullScreen()
{
    if (isFullScreen) {
        mediaPlayer->pause();
        videoWidget->hide();
        // Удаляем флаги окна и возвращаем виджет в контейнер
        videoWidget->setWindowFlags(Qt::Widget);
        // Важно: удаляем старый лейаут перед созданием нового
        if (ui->vidWidget->layout()) {
            delete ui->vidWidget->layout();
        }
        // Создаем новый лейаут и добавляем в него видеовиджет
        QVBoxLayout *layout = new QVBoxLayout(ui->vidWidget);
        layout->addWidget(videoWidget);
        ui->vidWidget->setLayout(layout);
        videoWidget->show();
        // Восстанавливаем обычное состояние окна
        setWindowState(windowState() & ~Qt::WindowFullScreen);
        mediaPlayer->play();
        isFullScreen = false;
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == videoWidget) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Escape && isFullScreen) {
                exitFullScreen();
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

