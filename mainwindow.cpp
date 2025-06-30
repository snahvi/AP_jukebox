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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Mplayer=new QMediaPlayer(this);
    audioOutput=new QAudioOutput(this);
    Mplayer->setAudioOutput(audioOutput);
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
    ui->verticalSlider->setMaximum(100);
    ui->verticalSlider->setMinimum(1);
    ui->verticalSlider->setValue(50);
    audioOutput->setVolume(50);
    connect(Mplayer, &QMediaPlayer::durationChanged, this, &MainWindow::durationChanged);
    connect(Mplayer, &QMediaPlayer::positionChanged, this, &MainWindow::positionChanged);
    //connect(Mplayer, &QMediaPlayer::Updateduaration, this, &MainWindow::Updateduaration);
    ui->horizontalSlider->setRange(0, Mplayer->duration()/1000);
    ui->label_Value_1->setStyleSheet("color: white;");
    ui->label_Value_2->setStyleSheet("color: white;");
    
    // Test if labels are working
    ui->label_Value_1->setText("00:00");
    ui->label_Value_2->setText("00:00");
    qDebug() << "Labels initialized with test text";
}

MainWindow::~MainWindow()
{
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
    QString FileName = QFileDialog::getOpenFileName(this, tr("Select Audio File"), "", tr("MP3 Files (*.mp3)"));
    if (!FileName.isEmpty()) {
        qDebug() << "Loading file:" << FileName;
        Mplayer->setSource(QUrl::fromLocalFile(FileName));
        QFileInfo fileinfo(FileName);
        ui->label->setText(fileinfo.fileName());
        audioOutput->setVolume(1.0);  // Set volume to 100%
        
        // Add error handling
        connect(Mplayer, &QMediaPlayer::errorOccurred, this, [](QMediaPlayer::Error error, const QString &errorString){
            qDebug() << "Media player error:" << error << errorString;
        });
        
        connect(Mplayer, &QMediaPlayer::mediaStatusChanged, this, [](QMediaPlayer::MediaStatus status){
            qDebug() << "Media status changed:" << status;
        });
    }
}


void MainWindow::on_push_button_lastsong_clicked()
{

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

