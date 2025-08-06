#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QGroupBox>
#include <QPainter>
#include <QTimer>
#include "../weatherservice.h"

class WindCompass : public QWidget
{
    Q_OBJECT

public:
    explicit WindCompass(QWidget *parent = nullptr);
    
    void setWindData(double speed, double direction, double gust = 0.0);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    double m_windSpeed;
    double m_windDirection;
    double m_windGust;
    QTimer *m_animationTimer;
    int m_animationFrame;
};

class WindWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WindWidget(QWidget *parent = nullptr);

public slots:
    void updateWindData(const WeatherData &data);

private:
    void setupUI();
    QString formatWindSpeed(double speed) const;
    QString formatWindDirection(double degrees) const;
    QString getWindStrength(double speed) const;
    QColor getWindSpeedColor(double speed) const;
    
    QVBoxLayout *m_mainLayout;
    
    QGroupBox *m_windDataGroup;
    QLabel *m_windSpeedLabel;
    QLabel *m_windDirectionLabel;
    QLabel *m_windGustLabel;
    QLabel *m_windStrengthLabel;
    
    WindCompass *m_windCompass;
    
    QGroupBox *m_windHistoryGroup;
    QLabel *m_avgWindSpeedLabel;
    QLabel *m_maxWindGustLabel;
    QLabel *m_trendLabel;
    
    QList<double> m_windSpeedHistory;
    QList<double> m_windGustHistory;
    static constexpr int MAX_HISTORY_SIZE = 20;
};