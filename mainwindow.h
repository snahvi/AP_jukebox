#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "user.h"
#include "song.h"
#include "equalizerwidget.h"
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
    void on_actionAdd_Songs_triggered();
    
    // Queue management slots
    void on_actionEnable_Queue_triggered();
    void on_actionAdd_to_Queue_triggered();
    void on_actionView_Queue_triggered();
    void on_actionClear_Queue_triggered();
    
    // Button slots
    void on_button_shuffle_clicked();
    void on_button_repeat_clicked();
    
    // Theme slots
    void on_actionTheme_Dark_Purple_triggered();
    void on_actionTheme_Dark_Blue_triggered();
    void on_actionTheme_Dark_Green_triggered();
    void on_actionTheme_Dark_Red_triggered();
    void on_actionTheme_Black_triggered();

    void on_radioButton_clicked();

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
    
    // Equalizer widget
    EqualizerWidget *equalizerWidget;

    // Helper methods
    void loadSong(int index);
    void loadNextSongFromQueue();
    void initializeUser();
    void updatePlaylistDisplay();
    void updateNextSongDisplay();
    void updateButtonStates();
    void updateThemeSelection();
    void changeBackgroundColor(const QString &color);
    void playCurrentPlaylist();
    Song getCurrentSong();
    Song getNextSong();
    void onSongFinished();
    void setupEqualizer();
    void updateEqualizerColorForTheme(const QString &bgColor);
};

#endif // MAINWINDOW_H
