#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentItem(0)
    , duration(0)
    , isFullScreen(false)
    , isImage(false)
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

    imageLabel = ui->imageLabel;
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setScaledContents(false); // Меняем на false и используем масштабирование вручную
    imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    imageLabel->hide();

    //start
    ui->vidWidget->hide();
    ui->timeSlider->hide();
    ui->volumeSlider->hide();
    ui->timeLabel->hide();
    ui->backButton->hide();
    ui->nextButton->hide();
    ui->pauseButton->hide();
    ui->playButton->hide();
    ui->fullScreenButton->hide();

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
    QString filePath = item->data(Qt::UserRole).toString();
    if (isImageFile(filePath)) {
        displayFile(filePath);
    }
    else{
        imageLabel->hide();
        ui->vidWidget->show();
        videoWidget->show();
        ui->timeSlider->show();
        ui->volumeSlider->show();
        ui->timeLabel->show();
        ui->backButton->show();
        ui->nextButton->show();
        ui->pauseButton->show();
        ui->playButton->show();
        ui->fullScreenButton->show();

        mediaPlayer->setSource(QUrl::fromLocalFile(filePath));
        mediaPlayer->play();
    }

    QString name = ui->filesList->currentItem()->text();
    ui->fileNameLabel->setText(name);
}


void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Открыть медиа"), "",
                                                    tr("Video (*.mp4 *.avi *.mkv);;All files (*.*);; Images (*.jpg *.jpeg *.png"));

    if (fileName.isEmpty()) {
        return;
    }

    // создание объекта QFileInfo с использованием пути к файлу fileName
    QListWidgetItem *item = new QListWidgetItem(QFileInfo(fileName).fileName(), ui->filesList);
    item->setData(Qt::UserRole, fileName); // установка дополнительных сведений о файле
    ui->filesList->addItem(item);
    ui->filesList->setCurrentItem(item); // Выбираем только что добавленный элемент

    if (isImageFile(fileName)) {
        // Скрываем элементы управления видео
        ui->vidWidget->hide();
        ui->timeSlider->hide();
        ui->volumeSlider->hide();
        ui->timeLabel->hide();
        ui->backButton->hide();
        ui->nextButton->hide();
        ui->pauseButton->hide();
        ui->playButton->hide();
        ui->fullScreenButton->show();

        // Показываем и настраиваем imageLabel
        imageLabel->show();
        currentImagePath = fileName;
        currentPixmap = QPixmap(fileName);
        if (!currentPixmap.isNull()) {
            imageLabel->setPixmap(currentPixmap.scaled(imageLabel->size(),
                                                       Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        mediaPlayer->stop(); // Останавливаем плеер, если он воспроизводил что-то
    }
    else {
        // метод QMediaPlayer устанавливающий источник медиа для воспроизведения.
        mediaPlayer->setSource(QUrl::fromLocalFile(fileName));
        imageLabel->hide();

        ui->vidWidget->show();
        videoWidget->show();
        ui->timeSlider->show();
        ui->volumeSlider->show();
        ui->timeLabel->show();
        ui->backButton->show();
        ui->nextButton->show();
        ui->pauseButton->show();
        ui->playButton->show();
        ui->fullScreenButton->show();

        mediaPlayer->play();
    }

    QString name = QFileInfo(fileName).fileName();
    ui->fileNameLabel->setText(name);
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
        // сохранение текущего воспроизведения
        bool wasPlaying = (mediaPlayer->playbackState() == QMediaPlayer::PlayingState);
        mediaPlayer->pause();
        // сохранение текущего положения
        qint64 currentPosition = mediaPlayer->position();
        mediaPlayer->setVideoOutput(nullptr);
        // отсоединение videowidget
        videoWidget->hide();
        videoWidget->setParent(nullptr);
        videoWidget->setWindowFlags(Qt::Window);
        // пареподключение videowidget
        mediaPlayer->setVideoOutput(videoWidget);
        videoWidget->showFullScreen();
        // возобновляем воспроизведение
        mediaPlayer->setPosition(currentPosition);
        if (wasPlaying) {
            mediaPlayer->play();
        }
        isFullScreen = true;
    }
    else {
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
        bool wasPlaying = (mediaPlayer->playbackState() == QMediaPlayer::PlayingState);
        mediaPlayer->pause();
        qint64 currentPosition = mediaPlayer->position();
        mediaPlayer->setVideoOutput(nullptr);
        videoWidget->hide();
        videoWidget->setWindowFlags(Qt::Widget);
        // удаление и создание videowidget
        delete videoWidget;
        videoWidget = new QVideoWidget(this);
        videoWidget->installEventFilter(this);
        // удаление старого слоя
        QLayout* oldLayout = ui->vidWidget->layout();
        if (oldLayout) {
            delete oldLayout;
        }
        // создание нового слоя
        QVBoxLayout* layout = new QVBoxLayout(ui->vidWidget);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(videoWidget);
        mediaPlayer->setVideoOutput(videoWidget);
        videoWidget->show();
        mediaPlayer->setPosition(currentPosition);
        if (wasPlaying) {
            mediaPlayer->play();
        }
        setWindowState(windowState() & ~Qt::WindowFullScreen);
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

bool MainWindow::isImageFile(const QString &filePath) {
    QFileInfo fileInfo(filePath);
    QString ext = fileInfo.suffix().toLower();

    // true если jpg или jpeg или png
    return (ext == "jpg" || ext == "jpeg" || ext == "png");
}

void MainWindow::displayFile(const QString &filePath) {
    isImage = isImageFile(filePath);

    if (isImage) {
        // Останавливаем видеоплеер и скрываем видеовиджет
        mediaPlayer->stop();
        videoWidget->hide();
        ui->vidWidget->hide();

        // Скрываем элементы управления для видео
        ui->timeSlider->hide();
        ui->volumeSlider->hide();
        ui->timeLabel->hide();
        ui->backButton->hide();
        ui->nextButton->hide();
        ui->pauseButton->hide();
        ui->playButton->hide();
        ui->fullScreenButton->show();

        // Загружаем и отображаем изображение
        currentImagePath = filePath;
        currentPixmap = QPixmap(filePath);

        if (!currentPixmap.isNull()) {
            imageLabel->setPixmap(currentPixmap.scaled(imageLabel->size(),
                                                       Qt::KeepAspectRatio, Qt::SmoothTransformation));
            imageLabel->show();
        } else {
            qDebug() << "Ошибка загрузки изображения:" << filePath;
        }
    }
    else {
        // Скрываем изображение
        imageLabel->hide();

        // Показываем видеовиджет и элементы управления
        ui->vidWidget->show();
        videoWidget->show();
        ui->timeSlider->show();
        ui->volumeSlider->show();
        ui->timeLabel->show();
        ui->backButton->show();
        ui->nextButton->show();
        ui->pauseButton->show();
        ui->playButton->show();
        ui->fullScreenButton->show();

        // Устанавливаем источник и начинаем воспроизведение
        mediaPlayer->setSource(QUrl::fromLocalFile(filePath));
        mediaPlayer->play();
    }

    // Обновляем имя файла
    QString name = QFileInfo(filePath).fileName();
    ui->fileNameLabel->setText(name);
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    if (isImage && !currentImagePath.isEmpty()) {
        QPixmap pixmap(currentImagePath);
        if (!pixmap.isNull()) {
            imageLabel->setPixmap(pixmap.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
    QMainWindow::resizeEvent(event);
}

void MainWindow::updateImageDisplay() {
    if (!currentPixmap.isNull()) {
        // Используем актуальный размер imageLabel для масштабирования
        imageLabel->setPixmap(currentPixmap.scaled(imageLabel->size(),
                                                   Qt::KeepAspectRatio,
                                                   Qt::SmoothTransformation));
    }
}



