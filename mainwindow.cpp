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
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialog>
#include <QComboBox>

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
    connect(Mplayer, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia) {
            onSongFinished();
        }
    });

    ui->horizontalSlider->setRange(0, Mplayer->duration()/1000);
    ui->label_Value_1->setStyleSheet("color: white;");
    ui->label_Value_2->setStyleSheet("color: white;");
    ui->label->setStyleSheet("color: white; font-size: 14px; font-weight: bold;");
    ui->label_playlist_status->setStyleSheet("color: #B0BEC5; background-color: rgba(255, 255, 255, 10); border-radius: 5px; padding: 5px;");
    ui->label_next_song->setStyleSheet("color: #90A4AE; background-color: rgba(255, 255, 255, 5); border-radius: 3px; padding: 3px;");

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
        updateNextSongDisplay();
    }
}


void MainWindow::on_push_button_lastsong_clicked()
{
    if (currentUser->isQueueMode()) {
        QMessageBox::information(this, tr("Queue Mode Active"), 
                               tr("Previous song navigation is not available in queue mode. Songs are consumed as they play."));
        return;
    }
    
    // Playlist mode
    Playlist* currentPlaylist = currentUser->getCurrentPlaylist();
    if (!currentPlaylist || currentPlaylist->isEmpty()) {
        QMessageBox::information(this, tr("No Songs Available"), 
                               tr("Please add songs to your playlist before navigating."));
        return;
    }

    currentSongIndex--;
    if (currentSongIndex < 0) {
        currentSongIndex = currentPlaylist->getSongCount() - 1; // Loop to last song
    }

    loadSong(currentSongIndex);
    qDebug() << "Previous song loaded, index:" << currentSongIndex;
    updateNextSongDisplay();
}


void MainWindow::on_push_button_getback_clicked()
{
    // Check if we have a current playlist and it's not empty and a song is loaded
    Playlist* currentPlaylist = currentUser->getCurrentPlaylist();
    if (!currentPlaylist || currentPlaylist->isEmpty() || currentSongIndex < 0) {
        QMessageBox::information(this, tr("No Songs Available"), 
                               tr("Please load a song before using seek controls."));
        return;
    }
    
    int currentPos = ui->horizontalSlider->value();
    int newPos = qMax(0, currentPos - 5); // Go back 5 seconds, but not below 0
    ui->horizontalSlider->setValue(newPos);
    Mplayer->setPosition(newPos * 1000);
    qDebug() << "Seeking backward to:" << newPos;
}


void MainWindow::on_pushButton_play_clicked()
{
    qDebug() << "Play button clicked, Is_Playing:" << Is_Playing;
    
    if (currentUser->isQueueMode()) {
        // Queue mode
        Queue* queue = currentUser->getQueue();
        if (queue->isEmpty()) {
            QMessageBox::information(this, tr("No Songs Available"), 
                                   tr("Please add songs to the queue before playing."));
            return;
        }
        
        // If no song is currently loaded in queue mode, load the next song from queue
        if (currentSongIndex < 0) {
            loadNextSongFromQueue();
            return;
        }
    } else {
        // Playlist mode
        Playlist* currentPlaylist = currentUser->getCurrentPlaylist();
        if (!currentPlaylist || currentPlaylist->isEmpty()) {
            QMessageBox::information(this, tr("No Songs Available"), 
                                   tr("Please add songs to your playlist before playing."));
            return;
        }
        
        // If no song is currently loaded, load the first song
        if (currentSongIndex < 0 || currentSongIndex >= currentPlaylist->getSongCount()) {
            currentSongIndex = 0;
            loadSong(currentSongIndex);
            return; // loadSong will handle the playing
        }
    }
    
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
    // Check if we have a current playlist and it's not empty and a song is loaded
    Playlist* currentPlaylist = currentUser->getCurrentPlaylist();
    if (!currentPlaylist || currentPlaylist->isEmpty() || currentSongIndex < 0) {
        QMessageBox::information(this, tr("No Songs Available"), 
                               tr("Please load a song before using seek controls."));
        return;
    }
    
    int currentPos = ui->horizontalSlider->value();
    int newPos = qMin(Mduration, currentPos + 5); // Go forward 5 seconds, but not beyond duration
    ui->horizontalSlider->setValue(newPos);
    Mplayer->setPosition(newPos * 1000);
    qDebug() << "Seeking forward to:" << newPos;
}


void MainWindow::on_push_button_nxtsong_clicked()
{
    if (currentUser->isQueueMode()) {
        // In queue mode, just load the next song from queue
        loadNextSongFromQueue();
        return;
    }
    
    // Playlist mode
    Playlist* currentPlaylist = currentUser->getCurrentPlaylist();
    if (!currentPlaylist || currentPlaylist->isEmpty()) {
        QMessageBox::information(this, tr("No Songs Available"), 
                              tr("Please add songs to your playlist before navigating."));
        return;
    }

    if (currentUser->isShuffleEnabled()) {
        // Get a random song
        Song randomSong = currentUser->getRandomSongFromCurrentPlaylist();
        if (randomSong.isValid()) {
            // Find the index of this random song
            for (int i = 0; i < currentPlaylist->getSongCount(); ++i) {
                if (currentPlaylist->getSong(i).getFilePath() == randomSong.getFilePath()) {
                    currentSongIndex = i;
                    loadSong(currentSongIndex);
                    qDebug() << "Random song loaded, index:" << currentSongIndex;
                    updateNextSongDisplay();
                    return;
                }
            }
        }
    } else {
        // Normal sequential mode
        currentSongIndex++;
        if (currentSongIndex >= currentPlaylist->getSongCount()) {
            currentSongIndex = 0; // Loop to first song
        }
        loadSong(currentSongIndex);
        qDebug() << "Next song loaded, index:" << currentSongIndex;
        updateNextSongDisplay();
    }
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
            qDebug() << "Default playlist created with" << defaultPlaylist->getSongCount() << "songs";
        } else {
            // Set it as current playlist even if empty, so user sees it in the UI
            currentUser->setCurrentPlaylist("Default Playlist");
            qDebug() << "Default playlist created but no sample songs found";
        }
    }
    
    qDebug() << "User initialized with default playlist";
    
    // Initialize UI state
    ui->actionEnable_Queue->setChecked(false);
    ui->button_shuffle->setChecked(false);
    ui->button_repeat->setChecked(false);
    updatePlaylistDisplay();
    updateNextSongDisplay();
    updateButtonStates();
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
    
    // Add indicators to display
    QString shuffleIndicator = currentUser->isShuffleEnabled() ? " 🔀" : "";
    QString repeatIndicator = currentUser->isRepeatEnabled() ? " 🔁" : "";
    
    ui->label->setText(QString("🎵 Song %1/%2: %3%4%5").arg(index + 1).arg(currentPlaylist->getSongCount()).arg(song.getDisplayName()).arg(shuffleIndicator).arg(repeatIndicator));
    
    // Add error handling
    connect(Mplayer, &QMediaPlayer::errorOccurred, this, [](QMediaPlayer::Error error, const QString &errorString){
        qDebug() << "Media player error:" << error << errorString;
    });

    connect(Mplayer, &QMediaPlayer::mediaStatusChanged, this, [](QMediaPlayer::MediaStatus status){
        qDebug() << "Media status changed:" << status;
    });

    qDebug() << "Song loaded successfully:" << song.getDisplayName();
    updatePlaylistDisplay();
    updateNextSongDisplay();
}

void MainWindow::updatePlaylistDisplay()
{
    QString shuffleIndicator = currentUser->isShuffleEnabled() ? " 🔀" : "";
    QString repeatIndicator = currentUser->isRepeatEnabled() ? " 🔁" : "";
    
    if (currentUser->isQueueMode()) {
        Queue* queue = currentUser->getQueue();
        if (!queue->isEmpty()) {
            ui->label_playlist_status->setText(QString("🎵 Queue Mode: %1 songs in queue%2%3").arg(queue->getSongCount()).arg(shuffleIndicator).arg(repeatIndicator));
        } else {
            ui->label_playlist_status->setText("🎵 Queue Mode: Empty queue - Add songs to queue");
        }
    } else {
        Playlist* currentPlaylist = currentUser->getCurrentPlaylist();
        if (currentPlaylist) {
            if (!currentPlaylist->isEmpty()) {
                ui->label_playlist_status->setText(QString("🎧 %1: %2 songs%3%4").arg(currentPlaylist->getName()).arg(currentPlaylist->getSongCount()).arg(shuffleIndicator).arg(repeatIndicator));
            } else {
                ui->label_playlist_status->setText(QString("📂 %1: Empty playlist - Add songs to start playing").arg(currentPlaylist->getName()));
            }
        } else {
            ui->label_playlist_status->setText("📂 No playlist selected - Create or load a playlist");
        }
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
        
        // Ask user if they want to make this their active playlist
        QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Playlist Created"),
            tr("Playlist '%1' has been created successfully.\nDo you want to make it your active playlist?").arg(playlistName),
            QMessageBox::Yes | QMessageBox::No);
        
                 if (reply == QMessageBox::Yes) {
             currentUser->setCurrentPlaylist(playlistName);
             currentSongIndex = -1; // Reset song index for new playlist
             
             // Clear the main display since there's no song loaded
             ui->label->setText("🎵 Playlist ready - Add songs to start playing");
             
             // Stop any current playback
             if (Is_Playing) {
                 Mplayer->stop();
                 Is_Playing = false;
                 ui->pushButton_play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
             }
         }
        
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
                // Clear queue and disable queue mode when switching to new playlist
                if (currentUser->isQueueMode()) {
                    currentUser->setQueueMode(false);
                    ui->actionEnable_Queue->setChecked(false);
                }
                
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
        // Clear queue and disable queue mode when switching playlists
        if (currentUser->isQueueMode()) {
            currentUser->setQueueMode(false);
            ui->actionEnable_Queue->setChecked(false);
            QMessageBox::information(this, tr("Queue Cleared"), 
                                   tr("Switched to playlist mode. Queue has been cleared."));
        }
        
        // Reset shuffle and repeat when switching playlists
        currentUser->setShuffleEnabled(false);
        currentUser->setRepeatEnabled(false);
        updateButtonStates();
        
        currentUser->setCurrentPlaylist(selectedPlaylist);
        Playlist* playlist = currentUser->getCurrentPlaylist();
        
        if (playlist && !playlist->isEmpty()) {
            QMessageBox::information(this, tr("Playlist Loaded"), 
                                   tr("Playlist '%1' loaded with %2 songs.").arg(selectedPlaylist).arg(playlist->getSongCount()));
            playCurrentPlaylist();
        } else {
            QMessageBox::information(this, tr("Empty Playlist"), 
                               tr("The playlist '%1' is empty. You can add songs to it using 'Playlist → Add Songs to Playlist' or 'Playlist → Create Playlist from Folder'.").arg(selectedPlaylist));
            currentSongIndex = -1; // Reset song index for empty playlist
            
            // Clear the main display and stop playback
            ui->label->setText("🎵 Playlist loaded - Add songs to start playing");
            if (Is_Playing) {
                Mplayer->stop();
                Is_Playing = false;
                ui->pushButton_play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
            }
        }
    }
}

void MainWindow::on_actionAdd_Songs_triggered()
{
    // First check if there are any playlists
    QStringList playlistNames = currentUser->getPlaylistNames();
    if (playlistNames.isEmpty()) {
        QMessageBox::information(this, tr("No Playlists"), 
                               tr("You need to create a playlist first before adding songs to it."));
        return;
    }
    
    // Let user select a folder to browse
    QString folderPath = QFileDialog::getExistingDirectory(this, tr("Select Folder with Songs"));
    if (folderPath.isEmpty()) {
        return;
    }
    
    // Scan the folder for songs
    QList<Song> availableSongs = currentUser->scanFolderForSongs(folderPath);
    if (availableSongs.isEmpty()) {
        QMessageBox::information(this, tr("No Songs Found"), 
                               tr("No supported audio files found in the selected folder."));
        return;
    }
    
    // Create a custom dialog for song selection
    QDialog songSelectionDialog(this);
    songSelectionDialog.setWindowTitle(tr("Select Songs to Add"));
    songSelectionDialog.setMinimumSize(500, 400);
    
    QVBoxLayout *layout = new QVBoxLayout(&songSelectionDialog);
    
    // Add instruction label
    QLabel *instructionLabel = new QLabel(tr("Select songs to add to playlist:"));
    layout->addWidget(instructionLabel);
    
    // Create list widget for songs with checkboxes
    QListWidget *songListWidget = new QListWidget();
    for (int i = 0; i < availableSongs.size(); ++i) {
        const Song &song = availableSongs[i];
        QListWidgetItem *item = new QListWidgetItem(song.getDisplayName());
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        item->setData(Qt::UserRole, i); // Store song index
        songListWidget->addItem(item);
    }
    layout->addWidget(songListWidget);
    
    // Add playlist selection
    QHBoxLayout *playlistLayout = new QHBoxLayout();
    QLabel *playlistLabel = new QLabel(tr("Add to playlist:"));
    QComboBox *playlistCombo = new QComboBox();
    playlistCombo->addItems(playlistNames);
    
    // Set current playlist as default if available
    QString currentPlaylistName = currentUser->getCurrentPlaylistName();
    if (!currentPlaylistName.isEmpty()) {
        int index = playlistCombo->findText(currentPlaylistName);
        if (index >= 0) {
            playlistCombo->setCurrentIndex(index);
        }
    }
    
    playlistLayout->addWidget(playlistLabel);
    playlistLayout->addWidget(playlistCombo);
    playlistLayout->addStretch();
    layout->addLayout(playlistLayout);
    
    // Add buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *selectAllButton = new QPushButton(tr("Select All"));
    QPushButton *deselectAllButton = new QPushButton(tr("Deselect All"));
    QPushButton *addButton = new QPushButton(tr("Add Selected"));
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    
    buttonLayout->addWidget(selectAllButton);
    buttonLayout->addWidget(deselectAllButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(cancelButton);
    layout->addLayout(buttonLayout);
    
    // Connect button signals
    connect(selectAllButton, &QPushButton::clicked, [songListWidget]() {
        for (int i = 0; i < songListWidget->count(); ++i) {
            songListWidget->item(i)->setCheckState(Qt::Checked);
        }
    });
    
    connect(deselectAllButton, &QPushButton::clicked, [songListWidget]() {
        for (int i = 0; i < songListWidget->count(); ++i) {
            songListWidget->item(i)->setCheckState(Qt::Unchecked);
        }
    });
    
    connect(cancelButton, &QPushButton::clicked, &songSelectionDialog, &QDialog::reject);
    
    connect(addButton, &QPushButton::clicked, [&]() {
        // Get selected songs
        QList<Song> selectedSongs;
        for (int i = 0; i < songListWidget->count(); ++i) {
            QListWidgetItem *item = songListWidget->item(i);
            if (item->checkState() == Qt::Checked) {
                int songIndex = item->data(Qt::UserRole).toInt();
                selectedSongs.append(availableSongs[songIndex]);
            }
        }
        
        if (selectedSongs.isEmpty()) {
            QMessageBox::warning(&songSelectionDialog, tr("No Songs Selected"), 
                               tr("Please select at least one song to add."));
            return;
        }
        
        // Get selected playlist
        QString selectedPlaylistName = playlistCombo->currentText();
        Playlist* targetPlaylist = currentUser->getPlaylist(selectedPlaylistName);
        
        if (!targetPlaylist) {
            QMessageBox::critical(&songSelectionDialog, tr("Error"), 
                                tr("Selected playlist not found."));
            return;
        }
        
        // Add songs to playlist
        int addedCount = 0;
        int duplicateCount = 0;
        for (const Song &song : selectedSongs) {
            if (targetPlaylist->containsSong(song)) {
                duplicateCount++;
            } else {
                targetPlaylist->addSong(song);
                addedCount++;
            }
        }
        
        songSelectionDialog.accept();
        
        // Show success message with details
        QString message;
        if (duplicateCount > 0) {
            message = tr("Successfully added %1 songs to playlist '%2'.\n%3 songs were already in the playlist and were skipped.")
                     .arg(addedCount).arg(selectedPlaylistName).arg(duplicateCount);
        } else {
            message = tr("Successfully added %1 songs to playlist '%2'.").arg(addedCount).arg(selectedPlaylistName);
        }
        QMessageBox::information(this, tr("Songs Added"), message);
        
                 // Update display and switch to the playlist if it's not current
         if (currentUser->getCurrentPlaylistName() != selectedPlaylistName) {
             QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Switch Playlist"),
                 tr("Do you want to switch to the playlist '%1' you just added songs to?").arg(selectedPlaylistName),
                 QMessageBox::Yes | QMessageBox::No);
             
             if (reply == QMessageBox::Yes) {
                 // Clear queue and disable queue mode when switching playlists
                 if (currentUser->isQueueMode()) {
                     currentUser->setQueueMode(false);
                     ui->actionEnable_Queue->setChecked(false);
                 }
                 
                 // Reset shuffle and repeat when switching playlists
                 currentUser->setShuffleEnabled(false);
                 currentUser->setRepeatEnabled(false);
                 updateButtonStates();
                 
                 currentUser->setCurrentPlaylist(selectedPlaylistName);
                 playCurrentPlaylist();
             }
         }
        
        updatePlaylistDisplay();
    });
    
    // Show the dialog
    songSelectionDialog.exec();
}

void MainWindow::loadNextSongFromQueue()
{
    Queue* queue = currentUser->getQueue();
    if (queue->isEmpty()) {
        qDebug() << "Queue is empty, stopping playback";
        if (Is_Playing) {
            Mplayer->stop();
            Is_Playing = false;
            ui->pushButton_play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        }
        ui->label->setText("🎵 Queue empty - Add songs to continue");
        currentSongIndex = -1;
        updatePlaylistDisplay();
        return;
    }
    
    // Take song from queue (respecting shuffle mode)
    Song nextSong;
    if (currentUser->isShuffleEnabled()) {
        // Get random song from queue
        Song randomSong = currentUser->getRandomSongFromCurrentPlaylist();
        if (randomSong.isValid()) {
            // Find and remove this song from queue
            for (int i = 0; i < queue->getSongCount(); ++i) {
                if (queue->getSong(i).getFilePath() == randomSong.getFilePath()) {
                    nextSong = randomSong;
                    queue->removeSong(i);
                    break;
                }
            }
        }
    } else {
        nextSong = queue->takeFirstSong();
    }
    
    if (!nextSong.isValid()) {
        qDebug() << "Invalid song from queue";
        return;
    }
    
    // Stop current playback
    if (Is_Playing) {
        Mplayer->stop();
        Is_Playing = false;
        ui->pushButton_play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    }
    
    // Load the song
    Mplayer->setSource(QUrl::fromLocalFile(nextSong.getFilePath()));
    QString shuffleIndicator = currentUser->isShuffleEnabled() ? " 🔀" : "";
    QString repeatIndicator = currentUser->isRepeatEnabled() ? " 🔁" : "";
    ui->label->setText(QString("🎵 From Queue: %1%2%3").arg(nextSong.getDisplayName()).arg(shuffleIndicator).arg(repeatIndicator));
    
    qDebug() << "Loaded song from queue:" << nextSong.getDisplayName();
    currentSongIndex = 0; // Use 0 to indicate we have a loaded song
    
    // Start playing automatically
    Mplayer->play();
    Is_Playing = true;
    ui->pushButton_play->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    
    updatePlaylistDisplay();
    updateNextSongDisplay();
}

void MainWindow::onSongFinished()
{
    qDebug() << "Song finished";
    
    // Check if repeat is enabled
    if (currentUser->isRepeatEnabled() && !currentUser->isQueueMode()) {
        // Repeat the current song
        if (currentSongIndex >= 0) {
            loadSong(currentSongIndex);
            qDebug() << "Repeating current song, index:" << currentSongIndex;
            updateNextSongDisplay();
            updatePlaylistDisplay();
            return;
        }
    }
    
    if (currentUser->isQueueMode()) {
        // In queue mode, automatically play the next song from queue
        loadNextSongFromQueue();
    } else {
        // In playlist mode, go to next song in playlist
        on_push_button_nxtsong_clicked();
    }
    
    // Update displays after song change
    updateNextSongDisplay();
    updatePlaylistDisplay();
}

void MainWindow::on_actionEnable_Queue_triggered()
{
    bool queueMode = ui->actionEnable_Queue->isChecked();
    currentUser->setQueueMode(queueMode);
    
    if (queueMode) {
        QMessageBox::information(this, tr("Queue Mode Enabled"), 
                               tr("Queue mode is now active. Songs will be consumed from the queue as they play.\nUse 'Queue → Add Songs to Queue' to add songs."));
        
        // Stop current playback and reset
        if (Is_Playing) {
            Mplayer->stop();
            Is_Playing = false;
            ui->pushButton_play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        }
        currentSongIndex = -1;
        ui->label->setText("🎵 Queue Mode - Add songs to queue to start playing");
    } else {
        QMessageBox::information(this, tr("Queue Mode Disabled"), 
                               tr("Switched back to playlist mode. The queue has been cleared."));
        
        // Stop current playback and reset
        if (Is_Playing) {
            Mplayer->stop();
            Is_Playing = false;
            ui->pushButton_play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        }
        currentSongIndex = -1;
        ui->label->setText("🎵 Playlist Mode - Load a playlist to start playing");
    }
    
    updatePlaylistDisplay();
    updateNextSongDisplay();
    updateButtonStates();
}

void MainWindow::on_actionAdd_to_Queue_triggered()
{
    if (!currentUser->isQueueMode()) {
        QMessageBox::information(this, tr("Queue Mode Not Active"), 
                               tr("Please enable queue mode first using 'Queue → Enable Queue Mode'."));
        return;
    }
    
    // Let user select a folder to browse
    QString folderPath = QFileDialog::getExistingDirectory(this, tr("Select Folder with Songs"));
    if (folderPath.isEmpty()) {
        return;
    }
    
    // Scan the folder for songs
    QList<Song> availableSongs = currentUser->scanFolderForSongs(folderPath);
    if (availableSongs.isEmpty()) {
        QMessageBox::information(this, tr("No Songs Found"), 
                               tr("No supported audio files found in the selected folder."));
        return;
    }
    
    // Create a custom dialog for song selection
    QDialog songSelectionDialog(this);
    songSelectionDialog.setWindowTitle(tr("Add Songs to Queue"));
    songSelectionDialog.setMinimumSize(500, 400);
    
    QVBoxLayout *layout = new QVBoxLayout(&songSelectionDialog);
    
    // Add instruction label
    QLabel *instructionLabel = new QLabel(tr("Select songs to add to queue:"));
    layout->addWidget(instructionLabel);
    
    // Create list widget for songs with checkboxes
    QListWidget *songListWidget = new QListWidget();
    for (int i = 0; i < availableSongs.size(); ++i) {
        const Song &song = availableSongs[i];
        QListWidgetItem *item = new QListWidgetItem(song.getDisplayName());
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        item->setData(Qt::UserRole, i); // Store song index
        songListWidget->addItem(item);
    }
    layout->addWidget(songListWidget);
    
    // Add queue position selection
    QHBoxLayout *positionLayout = new QHBoxLayout();
    QLabel *positionLabel = new QLabel(tr("Add to queue:"));
    QComboBox *positionCombo = new QComboBox();
    positionCombo->addItem(tr("At the end"), -1);
    positionCombo->addItem(tr("At the beginning"), 0);
    
    Queue* queue = currentUser->getQueue();
    for (int i = 1; i <= queue->getSongCount(); ++i) {
        positionCombo->addItem(tr("After song %1").arg(i), i);
    }
    
    positionLayout->addWidget(positionLabel);
    positionLayout->addWidget(positionCombo);
    positionLayout->addStretch();
    layout->addLayout(positionLayout);
    
    // Add buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *selectAllButton = new QPushButton(tr("Select All"));
    QPushButton *deselectAllButton = new QPushButton(tr("Deselect All"));
    QPushButton *addButton = new QPushButton(tr("Add Selected"));
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    
    buttonLayout->addWidget(selectAllButton);
    buttonLayout->addWidget(deselectAllButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(cancelButton);
    layout->addLayout(buttonLayout);
    
    // Connect button signals
    connect(selectAllButton, &QPushButton::clicked, [songListWidget]() {
        for (int i = 0; i < songListWidget->count(); ++i) {
            songListWidget->item(i)->setCheckState(Qt::Checked);
        }
    });
    
    connect(deselectAllButton, &QPushButton::clicked, [songListWidget]() {
        for (int i = 0; i < songListWidget->count(); ++i) {
            songListWidget->item(i)->setCheckState(Qt::Unchecked);
        }
    });
    
    connect(cancelButton, &QPushButton::clicked, &songSelectionDialog, &QDialog::reject);
    
    connect(addButton, &QPushButton::clicked, [&]() {
        // Get selected songs
        QList<Song> selectedSongs;
        for (int i = 0; i < songListWidget->count(); ++i) {
            QListWidgetItem *item = songListWidget->item(i);
            if (item->checkState() == Qt::Checked) {
                int songIndex = item->data(Qt::UserRole).toInt();
                selectedSongs.append(availableSongs[songIndex]);
            }
        }
        
        if (selectedSongs.isEmpty()) {
            QMessageBox::warning(&songSelectionDialog, tr("No Songs Selected"), 
                               tr("Please select at least one song to add."));
            return;
        }
        
        // Get insert position
        int insertPosition = positionCombo->currentData().toInt();
        
        // Add songs to queue
        Queue* queue = currentUser->getQueue();
        int addedCount = 0;
        
        if (insertPosition == -1) {
            // Add to end
            for (const Song &song : selectedSongs) {
                queue->addSong(song);
                addedCount++;
            }
        } else {
            // Insert at specific position
            for (int i = 0; i < selectedSongs.size(); ++i) {
                queue->insertSong(insertPosition + i, selectedSongs[i]);
                addedCount++;
            }
        }
        
        songSelectionDialog.accept();
        
        // Show success message
        QMessageBox::information(this, tr("Songs Added to Queue"), 
                               tr("Successfully added %1 songs to the queue.").arg(addedCount));
        
        updatePlaylistDisplay();
    });
    
    // Show the dialog
    songSelectionDialog.exec();
}

void MainWindow::on_actionView_Queue_triggered()
{
    if (!currentUser->isQueueMode()) {
        QMessageBox::information(this, tr("Queue Mode Not Active"), 
                               tr("Please enable queue mode first using 'Queue → Enable Queue Mode'."));
        return;
    }
    
    Queue* queue = currentUser->getQueue();
    
    // Create a dialog to show and edit the queue
    QDialog queueDialog(this);
    queueDialog.setWindowTitle(tr("View/Edit Queue"));
    queueDialog.setMinimumSize(600, 400);
    
    QVBoxLayout *layout = new QVBoxLayout(&queueDialog);
    
    QLabel *titleLabel = new QLabel(QString(tr("Current Queue (%1 songs):")).arg(queue->getSongCount()));
    layout->addWidget(titleLabel);
    
    QListWidget *queueListWidget = new QListWidget();
    QList<Song> songs = queue->getSongs();
    for (int i = 0; i < songs.size(); ++i) {
        QListWidgetItem *item = new QListWidgetItem(QString("%1. %2").arg(i + 1).arg(songs[i].getDisplayName()));
        queueListWidget->addItem(item);
    }
    layout->addWidget(queueListWidget);
    
    // Add control buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *moveUpButton = new QPushButton(tr("Move Up"));
    QPushButton *moveDownButton = new QPushButton(tr("Move Down"));
    QPushButton *removeButton = new QPushButton(tr("Remove Selected"));
    QPushButton *closeButton = new QPushButton(tr("Close"));
    
    buttonLayout->addWidget(moveUpButton);
    buttonLayout->addWidget(moveDownButton);
    buttonLayout->addWidget(removeButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);
    layout->addLayout(buttonLayout);
    
    // Connect button signals
    connect(moveUpButton, &QPushButton::clicked, [&]() {
        int currentRow = queueListWidget->currentRow();
        if (currentRow > 0) {
            queue->moveUp(currentRow);
            // Refresh the list
            queueListWidget->clear();
            QList<Song> updatedSongs = queue->getSongs();
            for (int i = 0; i < updatedSongs.size(); ++i) {
                QListWidgetItem *item = new QListWidgetItem(QString("%1. %2").arg(i + 1).arg(updatedSongs[i].getDisplayName()));
                queueListWidget->addItem(item);
            }
            queueListWidget->setCurrentRow(currentRow - 1);
                         updatePlaylistDisplay();
             updateNextSongDisplay();
         }
     });
     
     connect(moveDownButton, &QPushButton::clicked, [&]() {
        int currentRow = queueListWidget->currentRow();
        if (currentRow >= 0 && currentRow < queueListWidget->count() - 1) {
            queue->moveDown(currentRow);
            // Refresh the list
            queueListWidget->clear();
            QList<Song> updatedSongs = queue->getSongs();
            for (int i = 0; i < updatedSongs.size(); ++i) {
                QListWidgetItem *item = new QListWidgetItem(QString("%1. %2").arg(i + 1).arg(updatedSongs[i].getDisplayName()));
                queueListWidget->addItem(item);
            }
            queueListWidget->setCurrentRow(currentRow + 1);
                         updatePlaylistDisplay();
             updateNextSongDisplay();
         }
     });
     
     connect(removeButton, &QPushButton::clicked, [&]() {
        int currentRow = queueListWidget->currentRow();
        if (currentRow >= 0) {
            queue->removeSong(currentRow);
            // Refresh the list
            queueListWidget->clear();
            QList<Song> updatedSongs = queue->getSongs();
            for (int i = 0; i < updatedSongs.size(); ++i) {
                QListWidgetItem *item = new QListWidgetItem(QString("%1. %2").arg(i + 1).arg(updatedSongs[i].getDisplayName()));
                queueListWidget->addItem(item);
            }
                         titleLabel->setText(QString(tr("Current Queue (%1 songs):")).arg(queue->getSongCount()));
             updatePlaylistDisplay();
             updateNextSongDisplay();
         }
     });
    
    connect(closeButton, &QPushButton::clicked, &queueDialog, &QDialog::accept);
    
    queueDialog.exec();
}

void MainWindow::on_actionClear_Queue_triggered()
{
    if (!currentUser->isQueueMode()) {
        QMessageBox::information(this, tr("Queue Mode Not Active"), 
                               tr("Please enable queue mode first using 'Queue → Enable Queue Mode'."));
        return;
    }
    
    Queue* queue = currentUser->getQueue();
    if (queue->isEmpty()) {
        QMessageBox::information(this, tr("Queue Already Empty"), 
                               tr("The queue is already empty."));
        return;
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Clear Queue"),
        tr("Are you sure you want to clear all %1 songs from the queue?").arg(queue->getSongCount()),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        currentUser->clearQueue();
        
        // Stop current playback if in queue mode
        if (Is_Playing) {
            Mplayer->stop();
            Is_Playing = false;
            ui->pushButton_play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        }
        currentSongIndex = -1;
        ui->label->setText("🎵 Queue cleared - Add songs to queue to start playing");
        
        updatePlaylistDisplay();
        updateNextSongDisplay();
        QMessageBox::information(this, tr("Queue Cleared"), 
                               tr("The queue has been cleared."));
    }
}

void MainWindow::updateNextSongDisplay()
{
    Song nextSong = getNextSong();
    bool shuffleEnabled = currentUser->isShuffleEnabled();
    bool repeatEnabled = currentUser->isRepeatEnabled();
    
    if (nextSong.isValid()) {
        if (currentUser->isQueueMode()) {
            Queue* queue = currentUser->getQueue();
            int remainingAfter = queue->getSongCount() - 1;
            if (remainingAfter > 0) {
                QString shuffleText = shuffleEnabled ? " 🔀" : "";
                ui->label_next_song->setText(QString("⏭️ Next in Queue: %1 (%2 more after)%3")
                                           .arg(nextSong.getDisplayName())
                                           .arg(remainingAfter)
                                           .arg(shuffleText));
            } else {
                ui->label_next_song->setText(QString("⏭️ Last song in Queue: %1")
                                           .arg(nextSong.getDisplayName()));
            }
        } else {
            Playlist* currentPlaylist = currentUser->getCurrentPlaylist();
            if (currentPlaylist) {
                if (repeatEnabled) {
                    ui->label_next_song->setText(QString("⏭️ Repeat: Current song will repeat 🔁"));
                } else if (currentSongIndex >= 0) {
                    int nextIndex = (currentSongIndex + 1) % currentPlaylist->getSongCount();
                    QString shuffleText = shuffleEnabled ? " 🔀" : "";
                    ui->label_next_song->setText(QString("⏭️ Next in Playlist: %1 (%2/%3)%4")
                                               .arg(nextSong.getDisplayName())
                                               .arg(nextIndex + 1)
                                               .arg(currentPlaylist->getSongCount())
                                               .arg(shuffleText));
                } else {
                    ui->label_next_song->setText(QString("⏭️ First song ready: %1")
                                               .arg(nextSong.getDisplayName()));
                }
            }
        }
    } else {
        // Handle cases where next song is not predictable or available
        if (currentUser->isQueueMode()) {
            Queue* queue = currentUser->getQueue();
            if (queue->isEmpty()) {
                ui->label_next_song->setText("⏭️ Queue is empty - Add songs to queue");
            } else if (shuffleEnabled) {
                ui->label_next_song->setText(QString("⏭️ Random song from queue (%1 songs) 🔀").arg(queue->getSongCount()));
            } else {
                ui->label_next_song->setText("⏭️ Queue will be empty after current song");
            }
        } else {
            Playlist* currentPlaylist = currentUser->getCurrentPlaylist();
            if (!currentPlaylist || currentPlaylist->isEmpty()) {
                ui->label_next_song->setText("⏭️ No songs available - Add songs to play");
            } else if (repeatEnabled) {
                ui->label_next_song->setText(QString("⏭️ Repeat: Current song will repeat 🔁"));
            } else if (shuffleEnabled) {
                ui->label_next_song->setText(QString("⏭️ Random song from playlist (%1 songs) 🔀").arg(currentPlaylist->getSongCount()));
            } else if (currentSongIndex < 0) {
                ui->label_next_song->setText("⏭️ Press play to start playlist");
            } else {
                ui->label_next_song->setText("⏭️ End of playlist reached");
            }
        }
    }
}

Song MainWindow::getNextSong()
{
    if (currentUser->isQueueMode()) {
        Queue* queue = currentUser->getQueue();
        if (!queue->isEmpty()) {
            if (currentUser->isShuffleEnabled()) {
                // For shuffle mode, we can't predict the next song (it's random)
                return Song(); // This will show appropriate message in next song display
            }
            return queue->getSong(0); // First song in queue is next
        }
    } else {
        Playlist* currentPlaylist = currentUser->getCurrentPlaylist();
        if (currentPlaylist && !currentPlaylist->isEmpty()) {
            if (currentUser->isShuffleEnabled()) {
                // For shuffle mode, we can't predict the next song (it's random)
                return Song(); // This will show appropriate message in next song display
            }
            
            if (currentSongIndex >= 0) {
                // Get next song in playlist (with looping)
                int nextIndex = (currentSongIndex + 1) % currentPlaylist->getSongCount();
                return currentPlaylist->getSong(nextIndex);
            } else {
                // No song currently playing, next would be the first song
                return currentPlaylist->getSong(0);
            }
        }
    }
    return Song(); // Return invalid song if no next song
}

void MainWindow::updateButtonStates()
{
    // Update button visual states based on current settings
    ui->button_shuffle->setChecked(currentUser->isShuffleEnabled());
    ui->button_repeat->setChecked(currentUser->isRepeatEnabled());
    
    // Update other displays that depend on these states
    updatePlaylistDisplay();
    updateNextSongDisplay();
}

void MainWindow::on_button_shuffle_clicked()
{
    bool enabled = ui->button_shuffle->isChecked();
    currentUser->setShuffleEnabled(enabled);
    
    qDebug() << "Shuffle button clicked, enabled:" << enabled;
    
    // Update displays
    updateButtonStates();
    
    // Show user feedback
    if (enabled) {
        QMessageBox::information(this, tr("Shuffle Enabled"), 
                               tr("Shuffle is now ON. Next songs will be chosen randomly."));
    } else {
        QMessageBox::information(this, tr("Shuffle Disabled"), 
                               tr("Shuffle is now OFF. Songs will play in playlist order."));
    }
}

void MainWindow::on_button_repeat_clicked()
{
    bool enabled = ui->button_repeat->isChecked();
    currentUser->setRepeatEnabled(enabled);
    
    qDebug() << "Repeat button clicked, enabled:" << enabled;
    
    // Update displays
    updateButtonStates();
    
    // Show user feedback
    if (enabled) {
        QMessageBox::information(this, tr("Repeat Enabled"), 
                               tr("Repeat is now ON. Current song will repeat after finishing."));
    } else {
        QMessageBox::information(this, tr("Repeat Disabled"), 
                               tr("Repeat is now OFF. Songs will advance normally."));
    }
}
