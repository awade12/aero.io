#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QProgressBar>
#include <QGroupBox>
#include <QListWidget>
#include "../weatherservice.h"
#include "../flightconditions.h"

class WeatherWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WeatherWidget(QWidget *parent = nullptr);

public slots:
    void updateWeatherData(const WeatherData &data);
    void updateFlightConditions(const FlightAssessment &assessment);

private:
    void setupUI();
    void updateCurrentWeather(const WeatherData &data);
    void updateForecast(const WeatherData &data);
    QString formatTemperature(double temp) const;
    QString formatSpeed(double speed) const;
    QString formatDirection(double degrees) const;
    
    QVBoxLayout *m_mainLayout;
    
    QGroupBox *m_currentWeatherGroup;
    QLabel *m_locationLabel;
    QLabel *m_conditionLabel;
    QLabel *m_temperatureLabel;
    QLabel *m_feelsLikeLabel;
    QLabel *m_humidityLabel;
    QLabel *m_pressureLabel;
    QLabel *m_visibilityLabel;
    QLabel *m_cloudCoverLabel;
    QLabel *m_lastUpdatedLabel;
    
    QGroupBox *m_flightConditionsGroup;
    QLabel *m_overallStatusLabel;
    QLabel *m_windStatusLabel;
    QLabel *m_visibilityStatusLabel;
    QLabel *m_precipitationStatusLabel;
    QLabel *m_temperatureStatusLabel;
    QListWidget *m_warningsListWidget;
    QListWidget *m_recommendationsListWidget;
    
    QGroupBox *m_forecastGroup;
    QListWidget *m_forecastListWidget;
};