#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QSlider>
#include <QPushButton>
#include <QListWidgetItem>
#include <QWidget>
#include <QLayout>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:


    void on_nextButton_clicked();

    void on_pauseButton_clicked();

    void on_playButton_clicked();

    void on_backButton_clicked();

    void on_volumeSlider_valueChanged(int value);

    void on_filesList_itemClicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;

    QStringList mediaFiles;
    int currentIndex;
    QMediaPlayer *mediaPlayer;
    QAudioOutput *audioOutput;
    QVideoWidget *videoWidget;
};
#endif // MAINWINDOW_H
