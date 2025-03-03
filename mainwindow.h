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
#include <QVBoxLayout>
#include <QFileDialog>
#include <QFileInfo>
#include <QUrl>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QKeyEvent>
#include <Qicon>

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

protected:
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:


    void on_nextButton_clicked();
    void on_pauseButton_clicked();
    void on_playButton_clicked();
    void on_backButton_clicked();
    void on_volumeSlider_valueChanged(int value);
    void on_filesList_itemClicked(QListWidgetItem *item);
    void on_actionOpen_triggered();
    void playCurrentItem();
    void showContextMenu(const QPoint &point);
    void deleteSelectedItem();
    void updatePosition(qint64 position);
    void updateDuration(qint64 duration);
    void setPosition(int position);
    void on_fullScreenButton_clicked();
    void exitFullScreen();

private:
    Ui::MainWindow *ui;

    QStringList mediaFiles;
    int currentIndex;
    QMediaPlayer *mediaPlayer;
    QAudioOutput *audioOutput;
    QVideoWidget *videoWidget;

    int currentItem;
    QLabel *timeLabel;
    QSlider *positionSlider;
    qint64 duration; // длительность медиа
    bool isFullScreen;
};
#endif // MAINWINDOW_H
