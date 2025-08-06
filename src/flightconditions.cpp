// this is very basic and needs ALOT of adjusting
// most drones have a 19 sustained wind and 25 gusting and temp ranges from 32 to 104

#include "flightconditions.h"
#include <QDebug>

FlightConditions::FlightConditions(QObject *parent)
    : QObject(parent)
{
}

void FlightConditions::setLimits(const FlightAssessment::Limits &limits)
{
    m_assessment.limits = limits;
}

void FlightConditions::assessConditions(const WeatherData &weather)
{
    m_assessment.warnings.clear();
    m_assessment.recommendations.clear();
    
    m_assessment.wind = assessWindConditions(weather);
    m_assessment.visibility = assessVisibilityConditions(weather);
    m_assessment.precipitation = assessPrecipitationConditions(weather);
    m_assessment.temperature = assessTemperatureConditions(weather);
    
    m_assessment.overall = determineOverallSafety();
    
    switch (m_assessment.overall) {
    case FlightSafety::Safe:
        m_assessment.overallMessage = "Conditions are SAFE for drone operations";
        break;
    case FlightSafety::Caution:
        m_assessment.overallMessage = "CAUTION advised - monitor conditions closely";
        m_assessment.recommendations.append("Consider postponing non-essential flights");
        m_assessment.recommendations.append("Maintain visual line of sight at all times");
        break;
    case FlightSafety::Unsafe:
        m_assessment.overallMessage = "Conditions are UNSAFE - flight not recommended";
        m_assessment.recommendations.append("Wait for improved weather conditions");
        m_assessment.recommendations.append("Monitor weather updates frequently");
        break;
    case FlightSafety::NoFly:
        m_assessment.overallMessage = "NO FLY - Dangerous conditions present";
        m_assessment.recommendations.append("Do not attempt flight operations");
        m_assessment.recommendations.append("Secure all equipment");
        break;
    }
    
    emit assessmentUpdated(m_assessment);
}

FlightSafety FlightConditions::assessWindConditions(const WeatherData &weather)
{
    if (weather.windSpeed > m_assessment.limits.maxWindSpeed * 1.94384) {
        m_assessment.warnings.append(QString("High wind speed: %1 kts (limit: %2 kts)")
                                    .arg(weather.windSpeed, 0, 'f', 1)
                                    .arg(m_assessment.limits.maxWindSpeed * 1.94384, 0, 'f', 1));
        
        if (weather.windSpeed > m_assessment.limits.maxWindSpeed * 1.94384 * 1.5) {
            return FlightSafety::NoFly;
        } else {
            return FlightSafety::Unsafe;
        }
    }
    
    if (weather.windGust > m_assessment.limits.maxWindGust * 1.94384) {
        m_assessment.warnings.append(QString("High wind gusts: %1 kts (limit: %2 kts)")
                                    .arg(weather.windGust, 0, 'f', 1)
                                    .arg(m_assessment.limits.maxWindGust * 1.94384, 0, 'f', 1));
        
        if (weather.windGust > m_assessment.limits.maxWindGust * 1.94384 * 1.3) {
            return FlightSafety::Unsafe;
        } else {
            return FlightSafety::Caution;
        }
    }
    
    if (weather.windSpeed > m_assessment.limits.maxWindSpeed * 1.94384 * 0.7) {
        m_assessment.recommendations.append("Monitor wind conditions closely");
        return FlightSafety::Caution;
    }
    
    return FlightSafety::Safe;
}

FlightSafety FlightConditions::assessVisibilityConditions(const WeatherData &weather)
{
    if (weather.visibility < m_assessment.limits.minVisibility / 1.60934) {
        m_assessment.warnings.append(QString("Low visibility: %1 mi (minimum: %2 mi)")
                                    .arg(weather.visibility, 0, 'f', 1)
                                    .arg(m_assessment.limits.minVisibility / 1.60934, 0, 'f', 1));
        
        if (weather.visibility < m_assessment.limits.minVisibility / 1.60934 * 0.5) {
            return FlightSafety::NoFly;
        } else {
            return FlightSafety::Unsafe;
        }
    }
    
    if (weather.visibility < m_assessment.limits.minVisibility / 1.60934 * 1.5) {
        m_assessment.recommendations.append("Reduced visibility - maintain closer visual contact");
        return FlightSafety::Caution;
    }
    
    return FlightSafety::Safe;
}

FlightSafety FlightConditions::assessPrecipitationConditions(const WeatherData &weather)
{
    QString condition = weather.condition.toLower();
    
    if (condition.contains("thunderstorm") || condition.contains("storm")) {
        m_assessment.warnings.append("Thunderstorm conditions detected");
        return FlightSafety::NoFly;
    }
    
    if (condition.contains("rain") || condition.contains("drizzle")) {
        if (condition.contains("heavy")) {
            m_assessment.warnings.append("Heavy precipitation detected");
            return FlightSafety::Unsafe;
        } else {
            m_assessment.warnings.append("Precipitation detected");
            return FlightSafety::Caution;
        }
    }
    
    if (condition.contains("snow") || condition.contains("sleet")) {
        m_assessment.warnings.append("Snow/sleet conditions detected");
        return FlightSafety::Unsafe;
    }
    
    if (condition.contains("fog") || condition.contains("mist")) {
        m_assessment.warnings.append("Reduced visibility due to fog/mist");
        return FlightSafety::Caution;
    }
    
    return FlightSafety::Safe;
}

FlightSafety FlightConditions::assessTemperatureConditions(const WeatherData &weather)
{
    if (weather.temperature < m_assessment.limits.minTemperature) {
        m_assessment.warnings.append(QString("Temperature too low: %1°F (minimum: %2°F)")
                                    .arg((weather.temperature * 9.0 / 5.0) + 32.0, 0, 'f', 1)
                                    .arg((m_assessment.limits.minTemperature * 9.0 / 5.0) + 32.0, 0, 'f', 1));
        return FlightSafety::Unsafe;
    }
    
    if (weather.temperature > m_assessment.limits.maxTemperature) {
        m_assessment.warnings.append(QString("Temperature too high: %1°F (maximum: %2°F)")
                                    .arg((weather.temperature * 9.0 / 5.0) + 32.0, 0, 'f', 1)
                                    .arg((m_assessment.limits.maxTemperature * 9.0 / 5.0) + 32.0, 0, 'f', 1));
        return FlightSafety::Unsafe;
    }
    
    if (weather.humidity > m_assessment.limits.maxHumidity) {
        m_assessment.warnings.append(QString("High humidity: %1%% (maximum: %2%%)")
                                    .arg(weather.humidity, 0, 'f', 0)
                                    .arg(m_assessment.limits.maxHumidity, 0, 'f', 0));
        return FlightSafety::Caution;
    }
    
    return FlightSafety::Safe;
}

FlightSafety FlightConditions::determineOverallSafety() const
{
    if (m_assessment.wind == FlightSafety::NoFly ||
        m_assessment.visibility == FlightSafety::NoFly ||
        m_assessment.precipitation == FlightSafety::NoFly ||
        m_assessment.temperature == FlightSafety::NoFly) {
        return FlightSafety::NoFly;
    }
    
    if (m_assessment.wind == FlightSafety::Unsafe ||
        m_assessment.visibility == FlightSafety::Unsafe ||
        m_assessment.precipitation == FlightSafety::Unsafe ||
        m_assessment.temperature == FlightSafety::Unsafe) {
        return FlightSafety::Unsafe;
    }
    
    if (m_assessment.wind == FlightSafety::Caution ||
        m_assessment.visibility == FlightSafety::Caution ||
        m_assessment.precipitation == FlightSafety::Caution ||
        m_assessment.temperature == FlightSafety::Caution) {
        return FlightSafety::Caution;
    }
    
    return FlightSafety::Safe;
}

QString FlightConditions::getSafetyString(FlightSafety safety) const
{
    switch (safety) {
    case FlightSafety::Safe: return "SAFE";
    case FlightSafety::Caution: return "CAUTION";
    case FlightSafety::Unsafe: return "UNSAFE";
    case FlightSafety::NoFly: return "NO FLY";
    }
    return "UNKNOWN";
}

QString FlightConditions::getSafetyColor(FlightSafety safety) const
{
    switch (safety) {
    case FlightSafety::Safe: return "#00ff00";
    case FlightSafety::Caution: return "#ffaa00";
    case FlightSafety::Unsafe: return "#ff6600";
    case FlightSafety::NoFly: return "#ff0000";
    }
    return "#ffffff";
}