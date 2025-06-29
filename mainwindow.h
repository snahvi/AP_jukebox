#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
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
    void on_pushButton_soundoff_clicked();

    void on_actionOpen_file_triggered();

private:
    Ui::MainWindow *ui;
    bool Is_Muted=false;
    QMediaPlayer *Mplayer;
};

#endif // MAINWINDOW_H
