#include "equalizerwidget.h"
#include <QPainter>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QRandomGenerator>
#include <QtMath>

EqualizerWidget::EqualizerWidget(QWidget *parent)
    : QWidget(parent)
    , animationTimer(new QTimer(this))
    , barCount(50)
    , animationSpeed(50)
    , primaryColor(QColor(255, 165, 0))  // Orange color like the image
    , secondaryColor(QColor(255, 140, 0))
    , useGradient(true)
{
    // Initialize bar heights
    barHeights.resize(barCount);
    generateRandomHeights();
    
    // Setup animation timer
    connect(animationTimer, &QTimer::timeout, this, &EqualizerWidget::updateBars);
    
    // Set minimum size
    setMinimumSize(400, 150);
    
    // Set background to transparent/dark
    setStyleSheet("background-color: rgba(0, 0, 0, 0.8);");
}

void EqualizerWidget::setBarCount(int count)
{
    barCount = qMax(10, qMin(200, count)); // Limit between 10-200 bars
    barHeights.resize(barCount);
    generateRandomHeights();
    update();
}

void EqualizerWidget::setAnimationSpeed(int speed)
{
    animationSpeed = qMax(10, qMin(1000, speed)); // Limit between 10-1000ms
    if (animationTimer->isActive()) {
        animationTimer->stop();
        animationTimer->start(animationSpeed);
    }
}

void EqualizerWidget::startAnimation()
{
    animationTimer->start(animationSpeed);
}

void EqualizerWidget::stopAnimation()
{
    animationTimer->stop();
    // Fade out bars gradually
    for (int i = 0; i < barHeights.size(); ++i) {
        barHeights[i] *= 0.1;
    }
    update();
}

void EqualizerWidget::setColors(const QColor &primary, const QColor &secondary)
{
    primaryColor = primary;
    secondaryColor = secondary.isValid() ? secondary : primary;
    useGradient = secondary.isValid();
    update();
}

void EqualizerWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Calculate bar dimensions
    int availableWidth = width() - 20; // 10px margin on each side
    int barWidth = availableWidth / barCount;
    int maxBarHeight = height() - 20; // 10px margin top and bottom
    
    for (int i = 0; i < barCount; ++i) {
        int x = 10 + i * barWidth;
        int barHeight = static_cast<int>(barHeights[i] * maxBarHeight);
        int y = height() - 10 - barHeight;
        
        // Create gradient effect
        if (useGradient) {
            QLinearGradient gradient(x, y + barHeight, x, y);
            
            // Create a vibrant gradient similar to the image
            gradient.setColorAt(0.0, primaryColor.darker(120));
            gradient.setColorAt(0.3, primaryColor);
            gradient.setColorAt(0.7, secondaryColor);
            gradient.setColorAt(1.0, QColor(255, 200, 100)); // Lighter top
            
            painter.setBrush(QBrush(gradient));
        } else {
            painter.setBrush(QBrush(primaryColor));
        }
        
        painter.setPen(Qt::NoPen);
        
        // Draw bar with slight spacing
        QRect barRect(x + 1, y, barWidth - 2, barHeight);
        painter.drawRect(barRect);
        
        // Add glow effect for higher bars
        if (barHeights[i] > 0.7) {
            painter.setBrush(QBrush(QColor(255, 255, 255, 30)));
            painter.drawRect(barRect);
        }
    }
}

void EqualizerWidget::updateBars()
{
    // Generate new random heights with some smoothing
    for (int i = 0; i < barHeights.size(); ++i) {
        // Create wave-like motion
        double waveComponent = qSin(QRandomGenerator::global()->bounded(100) / 10.0) * 0.3;
        double randomComponent = QRandomGenerator::global()->bounded(100) / 100.0;
        
        // Smooth transition
        double targetHeight = (waveComponent + randomComponent * 0.7) * 0.8 + 0.2;
        barHeights[i] = barHeights[i] * 0.7 + targetHeight * 0.3; // Smooth interpolation
        
        // Ensure values stay within bounds
        barHeights[i] = qMax(0.0, qMin(1.0, barHeights[i]));
    }
    
    update(); // Trigger repaint
}

void EqualizerWidget::generateRandomHeights()
{
    for (int i = 0; i < barHeights.size(); ++i) {
        barHeights[i] = QRandomGenerator::global()->bounded(100) / 100.0;
    }
} 