#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Mplayer=new QMediaPlayer();
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
    ui->pushButton_soundoff->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_soundoff_clicked()
{
    if(Is_Muted==0)
    {
        ui->pushButton_soundoff->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
        Is_Muted =1;
    }
    else if(Is_Muted==1)
    {
        ui->pushButton_soundoff->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
        Is_Muted =0;
    }
}


void MainWindow::on_actionOpen_file_triggered()
{
    QString FileName = QFileDialog::getOpenFileName(this, tr("Select Audio File"), "", tr("MP3 Files (*.mp3)"));
    Mplayer->setSource(QUrl(FileName));
    QFileInfo fileinfo(FileName);
    ui->label->setText(fileinfo.fileName());
}


void MainWindow::on_push_button_lastsong_clicked()
{

}

