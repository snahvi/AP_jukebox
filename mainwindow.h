#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "user.h"
#include "song.h"
#include <QMainWindow>
#include <QtMultimedia/QMediaPlayer>
#include <QtCore>
#include <QtWidgets>
#include <QStyle>

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

    void Updateduration(qint64 duration);

    void durationChanged(qint64 duration);

    void positionChanged(qint64 progress);

    void on_pushButton_soundoff_clicked();

    void on_actionOpen_file_triggered();

    void on_push_button_lastsong_clicked();

    void on_push_button_getback_clicked();

    void on_pushButton_play_clicked();

    void on_push_button_gofront_clicked();

    void on_push_button_nxtsong_clicked();

    void on_horizontalSlider_valueChanged(int value);

    void on_verticalSlider_valueChanged(int value);
    
    // New playlist management slots
    void on_actionNew_Playlist_triggered();
    void on_actionOpen_Folder_triggered();
    void on_actionLoad_Playlist_triggered();

private:
    Ui::MainWindow *ui;
    bool Is_Muted=false;
    bool Is_Playing=false;
    QMediaPlayer *Mplayer;
    QAudioOutput *audioOutput;
    qint64 Mduration;
    qint64 Mprogress;

    // User and playlist functionality
    User *currentUser;
    int currentSongIndex;

    // Helper methods
    void loadSong(int index);
    void initializeUser();
    void updatePlaylistDisplay();
    void playCurrentPlaylist();
    Song getCurrentSong();
};

#endif // MAINWINDOW_H
