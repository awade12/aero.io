#pragma once

#include <QObject>
#include "weatherservice.h"

enum class FlightSafety {
    Safe,
    Caution,
    Unsafe,
    NoFly
};

struct FlightAssessment {
    FlightSafety overall;
    FlightSafety wind;
    FlightSafety visibility;
    FlightSafety precipitation;
    FlightSafety temperature;
    
    QString overallMessage;
    QStringList warnings;
    QStringList recommendations;
    
    struct Limits {
        double maxWindSpeed = 10.0; // m/s
        double maxWindGust = 15.0;  // m/s
        double minVisibility = 3.0; // km
        double minTemperature = -10.0; // °C
        double maxTemperature = 40.0;  // °C
        double maxHumidity = 95.0;     // %
    } limits;
};

class FlightConditions : public QObject
{
    Q_OBJECT

public:
    explicit FlightConditions(QObject *parent = nullptr);
    
    const FlightAssessment& currentAssessment() const { return m_assessment; }
    void setLimits(const FlightAssessment::Limits &limits);

public slots:
    void assessConditions(const WeatherData &weather);

signals:
    void assessmentUpdated(const FlightAssessment &assessment);

private:
    FlightSafety assessWindConditions(const WeatherData &weather);
    FlightSafety assessVisibilityConditions(const WeatherData &weather);
    FlightSafety assessPrecipitationConditions(const WeatherData &weather);
    FlightSafety assessTemperatureConditions(const WeatherData &weather);
    FlightSafety determineOverallSafety() const;
    QString getSafetyString(FlightSafety safety) const;
    QString getSafetyColor(FlightSafety safety) const;
    
    FlightAssessment m_assessment;
};