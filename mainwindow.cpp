#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QStyle>
#include <QFileDialog>
#include <QUrl>
#include <QFileInfo>
#include <QTime>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Initialize audio player
    Mplayer=new QMediaPlayer(this);
    audioOutput=new QAudioOutput(this);
    Mplayer->setAudioOutput(audioOutput);

    // Set up button icons and sizes
    ui->pushButton_play->setIconSize(QSize(36, 36));
    ui->push_button_getback->setIconSize(QSize(36, 36));
    ui->push_button_gofront->setIconSize(QSize(36, 36));
    ui->push_button_lastsong->setIconSize(QSize(36, 36));
    ui->push_button_nxtsong->setIconSize(QSize(36, 36));
    ui->pushButton_play ->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->push_button_getback->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
    ui->push_button_gofront->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    ui->push_button_lastsong->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    ui->push_button_nxtsong->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    ui->pushButton_soundoff->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));

    // Set up volume slider
    ui->verticalSlider->setMaximum(100);
    ui->verticalSlider->setMinimum(1);
    ui->verticalSlider->setValue(50);
    audioOutput->setVolume(0.5); // 50%

    // Connect audio player signals
    connect(Mplayer, &QMediaPlayer::durationChanged, this, &MainWindow::durationChanged);
    connect(Mplayer, &QMediaPlayer::positionChanged, this, &MainWindow::positionChanged);

    ui->horizontalSlider->setRange(0, Mplayer->duration()/1000);
    ui->label_Value_1->setStyleSheet("color: white;");
    ui->label_Value_2->setStyleSheet("color: white;");
    ui->label->setStyleSheet("color: white; font-size: 14px; font-weight: bold;");
    ui->label_playlist_status->setStyleSheet("color: #B0BEC5; background-color: rgba(255, 255, 255, 10); border-radius: 5px; padding: 5px;");

    // Test if labels are working
    ui->label_Value_1->setText("00:00");
    ui->label_Value_2->setText("00:00");
    qDebug() << "Labels initialized with test text";

    // Initialize user and load first song
    initializeUser();
    Playlist* currentPlaylist = currentUser->getCurrentPlaylist();
    if (currentPlaylist && !currentPlaylist->isEmpty()) {
        currentSongIndex = 0;
        loadSong(currentSongIndex);
    }

    // Set window title
    setWindowTitle("MuPlayer - Music Player");
}

MainWindow::~MainWindow()
{
    delete currentUser;
    delete ui;
}

void MainWindow::Updateduration(qint64 duration)
{
    QTime CurrentTime((duration / 3600) % 60, (duration / 60) % 60, duration % 60, (duration * 1000) % 1000);
    QTime totalTime((Mduration / 3600) % 60, (Mduration / 60) % 60, Mduration % 60, (Mduration * 1000) % 1000);
    QString format = "mm:ss";
    if (Mduration > 3600)format = "hh:mm:ss";
    ui->label_Value_1->setText(CurrentTime.toString(format));
    ui->label_Value_2->setText(totalTime.toString(format));
}

void MainWindow::on_pushButton_soundoff_clicked()
{
    if(Is_Muted==0)
    {
        ui->pushButton_soundoff->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
        audioOutput->setMuted(true);
        Is_Muted =1;
    }
    else if(Is_Muted==1)
    {
        ui->pushButton_soundoff->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
        audioOutput->setMuted(false);
        Is_Muted =0;
    }
}


void MainWindow::on_actionOpen_file_triggered()
{
    QString FileName = QFileDialog::getOpenFileName(this, tr("Select Audio File"), "",
                                                    tr("Audio Files (*.mp3 *.wav *.flac *.m4a)"));

    if (!FileName.isEmpty()) {
        QFileInfo fileinfo(FileName);
        qDebug() << "Loading audio file:" << FileName;

        // Stop current playback
        if (Is_Playing) {
            Mplayer->stop();
            Is_Playing = false;
            ui->pushButton_play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        }

        Mplayer->setSource(QUrl::fromLocalFile(FileName));
        ui->label->setText(fileinfo.fileName());

        // Add error handling
        connect(Mplayer, &QMediaPlayer::errorOccurred, this, [](QMediaPlayer::Error error, const QString &errorString){
            qDebug() << "Media player error:" << error << errorString;
        });

        connect(Mplayer, &QMediaPlayer::mediaStatusChanged, this, [](QMediaPlayer::MediaStatus status){
            qDebug() << "Media status changed:" << status;
        });

        // Reset current song index to indicate we're not in playlist mode
        currentSongIndex = -1;
        qDebug() << "External file loaded (not in playlist)";
        
        // Update display
        updatePlaylistDisplay();
    }
}


void MainWindow::on_push_button_lastsong_clicked()
{
    Playlist* currentPlaylist = currentUser->getCurrentPlaylist();
    if (!currentPlaylist || currentPlaylist->isEmpty()) return;

    currentSongIndex--;
    if (currentSongIndex < 0) {
        currentSongIndex = currentPlaylist->getSongCount() - 1; // Loop to last song
    }

    loadSong(currentSongIndex);
    qDebug() << "Previous song loaded, index:" << currentSongIndex;
}


void MainWindow::on_push_button_getback_clicked()
{
    int currentPos = ui->horizontalSlider->value();
    int newPos = qMax(0, currentPos - 5); // Go back 5 seconds, but not below 0
    ui->horizontalSlider->setValue(newPos);
    Mplayer->setPosition(newPos * 1000);
    qDebug() << "Seeking backward to:" << newPos;
}


void MainWindow::on_pushButton_play_clicked()
{
    qDebug() << "Play button clicked, Is_Playing:" << Is_Playing;
    if(Is_Playing==1)
    {
        ui->pushButton_play ->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        Mplayer->pause();
        Is_Playing=0;
        qDebug() << "Pausing playback";
    }
    else if(Is_Playing==0)
    {
        ui->pushButton_play ->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
        Mplayer->play();
        Is_Playing=1;
        qDebug() << "Starting playback";
    }
}


void MainWindow::on_push_button_gofront_clicked()
{
    int currentPos = ui->horizontalSlider->value();
    int newPos = qMin(Mduration, currentPos + 5); // Go forward 5 seconds, but not beyond duration
    ui->horizontalSlider->setValue(newPos);
    Mplayer->setPosition(newPos * 1000);
    qDebug() << "Seeking forward to:" << newPos;
}


void MainWindow::on_push_button_nxtsong_clicked()
{
    Playlist* currentPlaylist = currentUser->getCurrentPlaylist();
    if (!currentPlaylist || currentPlaylist->isEmpty()) return;

    currentSongIndex++;
    if (currentSongIndex >= currentPlaylist->getSongCount()) {
        currentSongIndex = 0; // Loop to first song
    }

    loadSong(currentSongIndex);
    qDebug() << "Next song loaded, index:" << currentSongIndex;
}


void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    // Only seek if user is dragging the slider (not when we update it programmatically)
    if (ui->horizontalSlider->isSliderDown()) {
        qDebug() << "Slider manually changed to:" << value;
        Mplayer->setPosition(value * 1000); // Convert seconds to milliseconds

        // Update labels immediately for visual feedback
        if (Mduration > 0) {
            QTime CurrentTime((value / 3600) % 60, (value / 60) % 60, value % 60, 0);
            QTime totalTime((Mduration / 3600) % 60, (Mduration / 60) % 60, Mduration % 60, 0);

            QString format = "mm:ss";
            if (Mduration > 3600) format = "hh:mm:ss";

            ui->label_Value_1->setText(CurrentTime.toString(format));
            ui->label_Value_2->setText(totalTime.toString(format));
        }
    }
}

void MainWindow::durationChanged(qint64 duration)
{
    qDebug() << "Duration changed:" << duration;
    Mduration = duration / 1000; // Convert to seconds
    ui->horizontalSlider->setRange(0, Mduration);
    qDebug() << "Duration in seconds:" << Mduration;
}

void MainWindow::positionChanged(qint64 position)
{
    if (Mduration == 0) return; // Don't update if no duration yet

    int currentSeconds = position / 1000; // Convert to seconds

    // Create time objects for formatting
    QTime CurrentTime((currentSeconds / 3600) % 60, (currentSeconds / 60) % 60, currentSeconds % 60, 0);
    QTime totalTime((Mduration / 3600) % 60, (Mduration / 60) % 60, Mduration % 60, 0);

    QString format = "mm:ss";
    if (Mduration > 3600) format = "hh:mm:ss";

    ui->label_Value_1->setText(CurrentTime.toString(format));
    ui->label_Value_2->setText(totalTime.toString(format));

    // Update slider position without triggering valueChanged signal
    ui->horizontalSlider->blockSignals(true);
    ui->horizontalSlider->setValue(currentSeconds);
    ui->horizontalSlider->blockSignals(false);
}

void MainWindow::on_verticalSlider_valueChanged(int value)
{
    audioOutput->setVolume(value / 100.0); // Convert 0-100 to 0.0-1.0
}

void MainWindow::initializeUser()
{
    // Initialize the user
    currentUser = new User("DefaultUser");
    
    // Create a default playlist with some sample songs (if they exist)
    currentUser->createPlaylist("Default Playlist");
    Playlist* defaultPlaylist = currentUser->getPlaylist("Default Playlist");
    
    if (defaultPlaylist) {
        // Add hardcoded song paths if they exist
        QStringList samplePaths = {
            "C:\\Users\\Y.S\\Desktop\\New folder (3)\\1_1 - Age Ye Rooz - Faramarz Aslani (320).mp3",
            "C:\\Users\\Y.S\\Desktop\\New folder (3)\\1_1 - Gonjeshkak - Farhad Mehrad (320).mp3",
            "C:\\Users\\Y.S\\Desktop\\New folder (3)\\1979.mp3"
        };
        
        for (const QString &path : samplePaths) {
            Song song(path);
            if (song.isValid()) {
                defaultPlaylist->addSong(song);
            }
        }
        
        if (!defaultPlaylist->isEmpty()) {
            currentUser->setCurrentPlaylist("Default Playlist");
        }
    }
    
    qDebug() << "User initialized with default playlist";
    updatePlaylistDisplay();
}

void MainWindow::loadSong(int index)
{
    Playlist* currentPlaylist = currentUser->getCurrentPlaylist();
    if (!currentPlaylist || index < 0 || index >= currentPlaylist->getSongCount()) {
        qDebug() << "Invalid song index or no current playlist:" << index;
        return;
    }

    Song song = currentPlaylist->getSong(index);
    QString filePath = song.getFilePath();
    qDebug() << "Loading song:" << filePath;

    // Check if file exists
    if (!song.isValid()) {
        qDebug() << "Invalid song:" << filePath;
        ui->label->setText("File not found: " + song.getFileName());
        return;
    }

    // Stop current playback
    if (Is_Playing) {
        Mplayer->stop();
        Is_Playing = false;
        ui->pushButton_play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    }

    // Load the new song
    Mplayer->setSource(QUrl::fromLocalFile(filePath));
    ui->label->setText(QString("🎵 Song %1/%2: %3").arg(index + 1).arg(currentPlaylist->getSongCount()).arg(song.getDisplayName()));
    
    // Add error handling
    connect(Mplayer, &QMediaPlayer::errorOccurred, this, [](QMediaPlayer::Error error, const QString &errorString){
        qDebug() << "Media player error:" << error << errorString;
    });

    connect(Mplayer, &QMediaPlayer::mediaStatusChanged, this, [](QMediaPlayer::MediaStatus status){
        qDebug() << "Media status changed:" << status;
    });

    qDebug() << "Song loaded successfully:" << song.getDisplayName();
    updatePlaylistDisplay();
}

void MainWindow::updatePlaylistDisplay()
{
    Playlist* currentPlaylist = currentUser->getCurrentPlaylist();
    if (currentPlaylist && !currentPlaylist->isEmpty()) {
        ui->label_playlist_status->setText(QString("🎧 %1: %2 songs").arg(currentPlaylist->getName()).arg(currentPlaylist->getSongCount()));
    } else {
        ui->label_playlist_status->setText("📂 No playlist selected");
    }
}

void MainWindow::playCurrentPlaylist()
{
    Playlist* currentPlaylist = currentUser->getCurrentPlaylist();
    if (currentPlaylist && !currentPlaylist->isEmpty()) {
        currentSongIndex = 0;
        loadSong(currentSongIndex);
    }
}

Song MainWindow::getCurrentSong()
{
    Playlist* currentPlaylist = currentUser->getCurrentPlaylist();
    if (currentPlaylist && currentSongIndex >= 0 && currentSongIndex < currentPlaylist->getSongCount()) {
        return currentPlaylist->getSong(currentSongIndex);
    }
    return Song(); // Return invalid song
}

void MainWindow::on_actionNew_Playlist_triggered()
{
    bool ok;
    QString playlistName = QInputDialog::getText(this, tr("New Playlist"),
                                                tr("Playlist name:"), QLineEdit::Normal,
                                                tr("My Playlist"), &ok);
    
    if (ok && !playlistName.isEmpty()) {
        // Check if playlist already exists
        if (currentUser->getPlaylist(playlistName)) {
            QMessageBox::warning(this, tr("Playlist Exists"), 
                                tr("A playlist with this name already exists."));
            return;
        }
        
        currentUser->createPlaylist(playlistName);
        QMessageBox::information(this, tr("Playlist Created"), 
                               tr("Playlist '%1' has been created successfully.").arg(playlistName));
        updatePlaylistDisplay();
    }
}

void MainWindow::on_actionOpen_Folder_triggered()
{
    QString folderPath = QFileDialog::getExistingDirectory(this, tr("Select Music Folder"));
    
    if (!folderPath.isEmpty()) {
        bool ok;
        QString playlistName = QInputDialog::getText(this, tr("Create Playlist from Folder"),
                                                    tr("Playlist name:"), QLineEdit::Normal,
                                                    tr("Folder Playlist"), &ok);
        
        if (ok && !playlistName.isEmpty()) {
            // Check if playlist already exists
            if (currentUser->getPlaylist(playlistName)) {
                QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Playlist Exists"),
                    tr("A playlist with this name already exists. Do you want to replace it?"),
                    QMessageBox::Yes | QMessageBox::No);
                
                if (reply == QMessageBox::Yes) {
                    currentUser->deletePlaylist(playlistName);
                } else {
                    return;
                }
            }
            
            // Create playlist from folder
            currentUser->createPlaylistFromFolder(playlistName, folderPath);
            Playlist* newPlaylist = currentUser->getPlaylist(playlistName);
            
            if (newPlaylist && !newPlaylist->isEmpty()) {
                currentUser->setCurrentPlaylist(playlistName);
                QMessageBox::information(this, tr("Playlist Created"), 
                                       tr("Playlist '%1' created with %2 songs.").arg(playlistName).arg(newPlaylist->getSongCount()));
                playCurrentPlaylist();
            } else {
                QMessageBox::warning(this, tr("No Songs Found"), 
                                   tr("No supported audio files found in the selected folder."));
            }
        }
    }
}

void MainWindow::on_actionLoad_Playlist_triggered()
{
    QStringList playlistNames = currentUser->getPlaylistNames();
    
    if (playlistNames.isEmpty()) {
        QMessageBox::information(this, tr("No Playlists"), 
                               tr("You don't have any playlists yet. Create one first!"));
        return;
    }
    
    bool ok;
    QString selectedPlaylist = QInputDialog::getItem(this, tr("Load Playlist"),
                                                    tr("Select playlist to load:"), 
                                                    playlistNames, 0, false, &ok);
    
    if (ok && !selectedPlaylist.isEmpty()) {
        currentUser->setCurrentPlaylist(selectedPlaylist);
        Playlist* playlist = currentUser->getCurrentPlaylist();
        
        if (playlist && !playlist->isEmpty()) {
            QMessageBox::information(this, tr("Playlist Loaded"), 
                                   tr("Playlist '%1' loaded with %2 songs.").arg(selectedPlaylist).arg(playlist->getSongCount()));
            playCurrentPlaylist();
        } else {
            QMessageBox::warning(this, tr("Empty Playlist"), 
                               tr("The selected playlist is empty."));
        }
    }
}
