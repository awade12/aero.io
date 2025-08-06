#include "windwidget.h"
#include <QPaintEvent>
#include <QPen>
#include <QBrush>
#include <QTimer>
#include <QSizePolicy>
#include <qmath.h>

WindCompass::WindCompass(QWidget *parent)
    : QWidget(parent)
    , m_windSpeed(0.0)
    , m_windDirection(0.0)
    , m_windGust(0.0)
    , m_animationTimer(new QTimer(this))
    , m_animationFrame(0)
{
    setMinimumSize(180, 180);
    setMaximumSize(180, 180);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    connect(m_animationTimer, &QTimer::timeout, [this]() {
        m_animationFrame = (m_animationFrame + 1) % 360;
        update();
    });
    m_animationTimer->start(100);
}

void WindCompass::setWindData(double speed, double direction, double gust)
{
    m_windSpeed = speed;
    m_windDirection = direction;
    m_windGust = gust;
    update();
}

void WindCompass::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    int side = qMin(width(), height());
    painter.setViewport((width() - side) / 2, (height() - side) / 2, side, side);
    painter.setWindow(-90, -90, 180, 180);
    
    QRadialGradient bgGradient(0, 0, 85);
    bgGradient.setColorAt(0, QColor(58, 58, 58, 200));  // #3a3a3a
    bgGradient.setColorAt(1, QColor(43, 43, 43, 200));  // #2b2b2b
    painter.fillRect(-90, -90, 180, 180, QBrush(bgGradient));
    
    QPen borderPen(QColor(85, 85, 85, 150), 1); // #555555
    painter.setPen(borderPen);
    painter.drawEllipse(-80, -80, 160, 160);
    
    painter.setPen(QPen(QColor(255, 255, 255, 120), 1));
    
    for (int i = 0; i < 360; i += 90) {
        painter.save();
        painter.rotate(i);
        painter.drawLine(0, -80, 0, -70);
        painter.restore();
    }
    
    QFont font = painter.font();
    font.setPointSize(12);
    font.setWeight(QFont::Medium);
    painter.setFont(font);
    painter.setPen(QColor(255, 255, 255, 200));
    
    painter.drawText(-6, -60, "N");
    painter.drawText(65, 6, "E");
    painter.drawText(-6, 75, "S");
    painter.drawText(-75, 6, "W");
    
    if (m_windSpeed > 0.1) {
        QColor arrowColor;
        if (m_windSpeed < 6.0) {
            arrowColor = QColor(76, 175, 80);       // Green
        } else if (m_windSpeed < 12.0) {
            arrowColor = QColor(255, 193, 7);      // Amber
        } else if (m_windSpeed < 20.0) {
            arrowColor = QColor(255, 152, 0);      // Orange
        } else {
            arrowColor = QColor(244, 67, 54);      // Red
        }
        
        painter.save();
        painter.rotate(m_windDirection);
        
        QPen arrowPen(arrowColor, 3);
        painter.setPen(arrowPen);
        painter.setBrush(QBrush(arrowColor));
        
        int arrowLength = qMin(60, (int)(m_windSpeed * 2 + 20));
        
        painter.drawLine(0, 0, 0, -arrowLength);

        // arrow head
        QPolygon arrowHead;
        arrowHead << QPoint(0, -arrowLength) 
                  << QPoint(-6, -arrowLength + 12) 
                  << QPoint(6, -arrowLength + 12);
        painter.drawPolygon(arrowHead);
        
        painter.restore();
        

        if (m_windGust > m_windSpeed + 2.0) {
            painter.save();
            painter.rotate(m_windDirection);
            
            QPen gustPen(QColor(255, 193, 7, 120), 2, Qt::DashLine);
            painter.setPen(gustPen);
            
            int gustLength = qMin(70, (int)(m_windGust * 2 + 20));
            if (gustLength > arrowLength) {
                painter.drawLine(0, -arrowLength - 3, 0, -gustLength);
            }
            
            painter.restore();
        }
    }
    

}

WindWidget::WindWidget(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_windDataGroup(nullptr)
    , m_windHistoryGroup(nullptr)
    , m_windCompass(nullptr)
{
    setupUI();
}

void WindWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(12);
    m_mainLayout->setContentsMargins(12, 12, 12, 12);
    
    m_windDataGroup = new QGroupBox("", this);
    m_windDataGroup->setStyleSheet(
        "QGroupBox {"
        "    border: 1px solid #555555;"
        "    border-radius: 6px;"
        "    background-color: #3a3a3a;"
        "    margin: 0px;"
        "    padding: 0px;"
        "}"
    );
    
    auto *mainLayout = new QHBoxLayout(m_windDataGroup);
    mainLayout->setSpacing(25);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    m_windCompass = new WindCompass(this);
    m_windCompass->setMinimumSize(180, 180);
    m_windCompass->setMaximumSize(180, 180);
    mainLayout->addWidget(m_windCompass, 0, Qt::AlignCenter);
    
    auto *dataLayout = new QVBoxLayout();
    dataLayout->setSpacing(15);
    
    m_windSpeedLabel = new QLabel("-- kts");
    m_windSpeedLabel->setStyleSheet(
        "QLabel {"
        "    color: #ffffff;"
        "    font-size: 32px;"
        "    font-weight: 300;"
        "    border: none;"
        "    padding: 0px;"
        "}"
    );
    m_windSpeedLabel->setAlignment(Qt::AlignCenter);
    dataLayout->addWidget(m_windSpeedLabel);
    
    m_windStrengthLabel = new QLabel("--");
    m_windStrengthLabel->setStyleSheet(
        "QLabel {"
        "    color: #4CAF50;"
        "    font-size: 14px;"
        "    font-weight: 500;"
        "    padding: 0px;"
        "}"
    );
    m_windStrengthLabel->setAlignment(Qt::AlignCenter);
    dataLayout->addWidget(m_windStrengthLabel);
    
    m_windDirectionLabel = new QLabel("--");
    m_windDirectionLabel->setStyleSheet(
        "QLabel {"
        "    color: #ffffff;"
        "    font-size: 16px;"
        "    font-weight: 400;"
        "    padding: 8px 0px;"
        "    border-top: 1px solid #555555;"
        "    margin-top: 8px;"
        "}"
    );
    m_windDirectionLabel->setAlignment(Qt::AlignCenter);
    dataLayout->addWidget(m_windDirectionLabel);
    
    m_windGustLabel = new QLabel("");
    m_windGustLabel->setStyleSheet(
        "QLabel {"
        "    color: #fbbf24;"
        "    font-size: 14px;"
        "    font-weight: 500;"
        "    padding: 4px 0px;"
        "}"
    );
    m_windGustLabel->setAlignment(Qt::AlignCenter);
    dataLayout->addWidget(m_windGustLabel);
    
    dataLayout->addStretch();
    mainLayout->addLayout(dataLayout, 1);
    
    m_mainLayout->addWidget(m_windDataGroup);
    
    m_windHistoryGroup = new QGroupBox("", this);
    m_windHistoryGroup->setStyleSheet(
        "QGroupBox {"
        "    border: 1px solid #555555;"
        "    border-radius: 6px;"
        "    background-color: #3a3a3a;"
        "    margin: 0px;"
        "    padding: 0px;"
        "}"
    );
    
    auto *analysisLayout = new QHBoxLayout(m_windHistoryGroup);
    analysisLayout->setSpacing(25);
    analysisLayout->setContentsMargins(20, 15, 20, 15);
    
    auto *avgSection = new QVBoxLayout();
    auto *avgHeader = new QLabel("AVG");
    avgHeader->setStyleSheet("color: #888888; font-size: 10px; font-weight: bold;");
    avgHeader->setAlignment(Qt::AlignCenter);
    m_avgWindSpeedLabel = new QLabel("--");
    m_avgWindSpeedLabel->setStyleSheet("color: #ffffff; font-size: 14px; font-weight: 500;");
    m_avgWindSpeedLabel->setAlignment(Qt::AlignCenter);
    avgSection->addWidget(avgHeader);
    avgSection->addWidget(m_avgWindSpeedLabel);
    
    auto *maxSection = new QVBoxLayout();
    auto *maxHeader = new QLabel("MAX");
    maxHeader->setStyleSheet("color: #888888; font-size: 10px; font-weight: bold;");
    maxHeader->setAlignment(Qt::AlignCenter);
    m_maxWindGustLabel = new QLabel("--");
    m_maxWindGustLabel->setStyleSheet("color: #ffffff; font-size: 14px; font-weight: 500;");
    m_maxWindGustLabel->setAlignment(Qt::AlignCenter);
    maxSection->addWidget(maxHeader);
    maxSection->addWidget(m_maxWindGustLabel);
    
    auto *trendSection = new QVBoxLayout();
    auto *trendHeader = new QLabel("TREND");
    trendHeader->setStyleSheet("color: #888888; font-size: 10px; font-weight: bold;");
    trendHeader->setAlignment(Qt::AlignCenter);
    m_trendLabel = new QLabel("--");
    m_trendLabel->setStyleSheet("color: #ffffff; font-size: 14px; font-weight: 500;");
    m_trendLabel->setAlignment(Qt::AlignCenter);
    trendSection->addWidget(trendHeader);
    trendSection->addWidget(m_trendLabel);
    
    analysisLayout->addLayout(avgSection);
    analysisLayout->addStretch();
    analysisLayout->addLayout(maxSection);
    analysisLayout->addStretch();
    analysisLayout->addLayout(trendSection);
    
    m_mainLayout->addWidget(m_windHistoryGroup);
    m_mainLayout->addStretch();
    
    setMinimumWidth(500);
    setMaximumWidth(650);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
}

void WindWidget::updateWindData(const WeatherData &data)
{
    m_windSpeedLabel->setText(QString("%1 kts").arg(data.windSpeed, 0, 'f', 0));
    m_windDirectionLabel->setText(formatWindDirection(data.windDirection));
    m_windStrengthLabel->setText(getWindStrength(data.windSpeed));
    
    if (data.windGust > data.windSpeed + 2.0) {
        m_windGustLabel->setText(QString("Gusts %1 kts").arg(data.windGust, 0, 'f', 0));
        m_windGustLabel->show();
    } else {
        m_windGustLabel->hide();
    }
    
    QColor speedColor = getWindSpeedColor(data.windSpeed);
    QString strengthStyle = QString(
        "QLabel {"
        "    color: %1;"
        "    font-size: 14px;"
        "    font-weight: 500;"
        "    padding: 0px;"
        "}").arg(speedColor.name());
    m_windStrengthLabel->setStyleSheet(strengthStyle);
    
    m_windCompass->setWindData(data.windSpeed, data.windDirection, data.windGust);
    
    m_windSpeedHistory.append(data.windSpeed);
    m_windGustHistory.append(data.windGust);
    
    if (m_windSpeedHistory.size() > MAX_HISTORY_SIZE) {
        m_windSpeedHistory.removeFirst();
        m_windGustHistory.removeFirst();
    }
    
    if (!m_windSpeedHistory.isEmpty()) {
        double avgSpeed = 0.0;
        double maxGust = 0.0;
        
        for (double speed : m_windSpeedHistory) {
            avgSpeed += speed;
        }
        avgSpeed /= m_windSpeedHistory.size();
        
        for (double gust : m_windGustHistory) {
            if (gust > maxGust) maxGust = gust;
        }
        
        m_avgWindSpeedLabel->setText(QString("%1 kts").arg(avgSpeed, 0, 'f', 0));
        m_maxWindGustLabel->setText(QString("%1 kts").arg(maxGust, 0, 'f', 0));
        
        if (m_windSpeedHistory.size() > 5) {
            double recentAvg = 0.0;
            double olderAvg = 0.0;
            int halfSize = m_windSpeedHistory.size() / 2;
            
            for (int i = halfSize; i < m_windSpeedHistory.size(); ++i) {
                recentAvg += m_windSpeedHistory[i];
            }
            recentAvg /= (m_windSpeedHistory.size() - halfSize);
            
            for (int i = 0; i < halfSize; ++i) {
                olderAvg += m_windSpeedHistory[i];
            }
            olderAvg /= halfSize;
            
            QString trend;
            QString trendColor = "#ffffff";
            if (recentAvg > olderAvg + 0.5) {
                trend = "↗";
                trendColor = "#ff6600";
            } else if (recentAvg < olderAvg - 0.5) {
                trend = "↘";  
                trendColor = "#4CAF50";
            } else {
                trend = "→";
            }
            
            QString trendStyle = QString(
                "color: %1; font-size: 14px; font-weight: 500; text-align: center;"
            ).arg(trendColor);
            m_trendLabel->setStyleSheet(trendStyle);
            m_trendLabel->setText(trend);
        }
    }
}

QString WindWidget::formatWindSpeed(double speed) const
{
    return QString("%1 kts").arg(speed, 0, 'f', 1);
}

QString WindWidget::formatWindDirection(double degrees) const
{
    const QStringList directions = {"N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE",
                                   "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW"};
    int index = qRound(degrees / 22.5) % 16;
    return QString("%1° (%2)").arg(degrees, 0, 'f', 0).arg(directions[index]);
}

QString WindWidget::getWindStrength(double speed) const
{
    if (speed < 1) return "Calm";
    else if (speed < 6) return "Light";
    else if (speed < 12) return "Moderate";
    else if (speed < 20) return "Strong";
    else if (speed < 30) return "Very Strong";
    else return "Dangerous";
}

QColor WindWidget::getWindSpeedColor(double speed) const
{
    if (speed < 6) return QColor(76, 175, 80);       // Material Green #4CAF50
    else if (speed < 12) return QColor(255, 193, 7); // Material Amber #FFC107
    else if (speed < 20) return QColor(255, 152, 0); // Material Orange #FF9800
    else return QColor(244, 67, 54);                 // Material Red #F44336
}