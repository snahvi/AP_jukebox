#ifndef EQUALIZERWIDGET_H
#define EQUALIZERWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QPainter>
#include <QVector>
#include <QColor>

class EqualizerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EqualizerWidget(QWidget *parent = nullptr);
    void setBarCount(int count);
    void setAnimationSpeed(int speed);
    void startAnimation();
    void stopAnimation();
    void setColors(const QColor &primary, const QColor &secondary = QColor());

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateBars();

private:
    QTimer *animationTimer;
    QVector<double> barHeights;
    int barCount;
    int animationSpeed;
    QColor primaryColor;
    QColor secondaryColor;
    bool useGradient;
    
    void generateRandomHeights();
};

#endif // EQUALIZERWIDGET_H 