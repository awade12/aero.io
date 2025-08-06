#include "weatherwidget.h"
#include <QDateTime>
#include <QListWidgetItem>
#include <QSizePolicy>

WeatherWidget::WeatherWidget(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_currentWeatherGroup(nullptr)
    , m_flightConditionsGroup(nullptr)
    , m_forecastGroup(nullptr)
{
    setupUI();
}

void WeatherWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(20);
    m_mainLayout->setContentsMargins(16, 16, 16, 16);
    
    // Modern group box style
    QString modernGroupStyle = 
        "QGroupBox {"
        "    font-family: 'SF Pro Display', 'Segoe UI', 'Arial';"
        "    font-weight: 600;"
        "    font-size: 15px;"
        "    color: #f0f6ff;"
        "    border: 1px solid rgba(200, 220, 255, 0.15);"
        "    border-radius: 12px;"
        "    margin-top: 12px;"
        "    padding-top: 16px;"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                stop:0 rgba(25, 30, 45, 0.95), "
        "                stop:1 rgba(20, 25, 40, 0.95));"
        "    box-shadow: 0 4px 20px rgba(0, 0, 0, 0.3);"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    left: 16px;"
        "    padding: 0 12px 0 12px;"
        "    background-color: transparent;"
        "    color: #f0f6ff;"
        "}";
    
    m_currentWeatherGroup = new QGroupBox("Current Weather", this);
    m_currentWeatherGroup->setMinimumHeight(200);
    m_currentWeatherGroup->setStyleSheet(modernGroupStyle);
    auto *weatherLayout = new QGridLayout(m_currentWeatherGroup);
    weatherLayout->setSpacing(16);
    weatherLayout->setContentsMargins(20, 24, 20, 20);
    
    // Modern label styles
    QString primaryLabelStyle = 
        "QLabel {"
        "    font-family: 'SF Pro Display', 'Segoe UI', 'Arial';"
        "    font-weight: 700;"
        "    font-size: 16px;"
        "    color: #ffffff;"
        "    padding: 6px 8px;"
        "    background: rgba(255, 255, 255, 0.05);"
        "    border-radius: 6px;"
        "}";
        
    QString secondaryLabelStyle = 
        "QLabel {"
        "    font-family: 'SF Pro Text', 'Segoe UI', 'Arial';"
        "    font-weight: 500;"
        "    font-size: 14px;"
        "    color: #f8fafc;"
        "    padding: 4px 6px;"
        "}";
        
    QString dataLabelStyle = 
        "QLabel {"
        "    font-family: 'SF Pro Text', 'Segoe UI', 'Arial';"
        "    font-weight: 400;"
        "    font-size: 13px;"
        "    color: #e2e8f0;"
        "    padding: 4px 6px;"
        "    background: rgba(255, 255, 255, 0.02);"
        "    border-radius: 4px;"
        "}";
    
    m_locationLabel = new QLabel("Location: --", this);
    m_locationLabel->setStyleSheet(primaryLabelStyle);
    weatherLayout->addWidget(m_locationLabel, 0, 0, 1, 2);
    
    m_conditionLabel = new QLabel("Condition: --", this);
    m_conditionLabel->setStyleSheet(secondaryLabelStyle);
    weatherLayout->addWidget(m_conditionLabel, 1, 0, 1, 2);
    
    m_temperatureLabel = new QLabel("Temperature: --", this);
    m_temperatureLabel->setStyleSheet(dataLabelStyle);
    weatherLayout->addWidget(m_temperatureLabel, 2, 0);
    
    m_feelsLikeLabel = new QLabel("Feels like: --", this);
    m_feelsLikeLabel->setStyleSheet(dataLabelStyle);
    weatherLayout->addWidget(m_feelsLikeLabel, 2, 1);
    
    m_humidityLabel = new QLabel("Humidity: --", this);
    m_humidityLabel->setStyleSheet(dataLabelStyle);
    weatherLayout->addWidget(m_humidityLabel, 3, 0);
    
    m_pressureLabel = new QLabel("Pressure: --", this);
    m_pressureLabel->setStyleSheet(dataLabelStyle);
    weatherLayout->addWidget(m_pressureLabel, 3, 1);
    
    m_visibilityLabel = new QLabel("Visibility: --", this);
    m_visibilityLabel->setStyleSheet(dataLabelStyle);
    weatherLayout->addWidget(m_visibilityLabel, 4, 0);
    
    m_cloudCoverLabel = new QLabel("Cloud Cover: --", this);
    m_cloudCoverLabel->setStyleSheet(dataLabelStyle);
    weatherLayout->addWidget(m_cloudCoverLabel, 4, 1);
    
    m_lastUpdatedLabel = new QLabel("Last updated: --", this);
    m_lastUpdatedLabel->setStyleSheet(
        "QLabel {"
        "    font-family: 'SF Pro Text', 'Segoe UI', 'Arial';"
        "    font-weight: 300;"
        "    font-size: 11px;"
        "    color: #94a3b8;"
        "    font-style: italic;"
        "    padding: 4px 6px;"
        "}"
    );
    weatherLayout->addWidget(m_lastUpdatedLabel, 5, 0, 1, 2);
    
    m_mainLayout->addWidget(m_currentWeatherGroup);
    
    m_flightConditionsGroup = new QGroupBox("Flight Conditions", this);
    m_flightConditionsGroup->setMinimumHeight(220);
    m_flightConditionsGroup->setStyleSheet(modernGroupStyle);
    auto *conditionsLayout = new QVBoxLayout(m_flightConditionsGroup);
    conditionsLayout->setSpacing(14);
    conditionsLayout->setContentsMargins(20, 24, 20, 20);
    
    m_overallStatusLabel = new QLabel("Overall: --", this);
    m_overallStatusLabel->setStyleSheet(
        "QLabel {"
        "    font-family: 'SF Pro Display', 'Segoe UI', 'Arial';"
        "    font-weight: 700;"
        "    font-size: 16px;"
        "    padding: 12px 16px;"
        "    border-radius: 8px;"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                stop:0 rgba(255, 255, 255, 0.08), "
        "                stop:1 rgba(255, 255, 255, 0.03));"
        "    border: 1px solid rgba(255, 255, 255, 0.1);"
        "}"
    );
    conditionsLayout->addWidget(m_overallStatusLabel);
    
    auto *statusGrid = new QGridLayout();
    statusGrid->setSpacing(12);
    
    QString statusLabelStyle = 
        "QLabel {"
        "    font-family: 'SF Pro Text', 'Segoe UI', 'Arial';"
        "    font-weight: 600;"
        "    font-size: 13px;"
        "    padding: 10px 12px;"
        "    border-radius: 6px;"
        "    background: rgba(255, 255, 255, 0.04);"
        "    border: 1px solid rgba(255, 255, 255, 0.06);"
        "}";
    
    m_windStatusLabel = new QLabel("Wind: --", this);
    m_windStatusLabel->setStyleSheet(statusLabelStyle);
    m_visibilityStatusLabel = new QLabel("Visibility: --", this);
    m_visibilityStatusLabel->setStyleSheet(statusLabelStyle);
    m_precipitationStatusLabel = new QLabel("Precipitation: --", this);
    m_precipitationStatusLabel->setStyleSheet(statusLabelStyle);
    m_temperatureStatusLabel = new QLabel("Temperature: --", this);
    m_temperatureStatusLabel->setStyleSheet(statusLabelStyle);
    
    statusGrid->addWidget(m_windStatusLabel, 0, 0);
    statusGrid->addWidget(m_visibilityStatusLabel, 0, 1);
    statusGrid->addWidget(m_precipitationStatusLabel, 1, 0);
    statusGrid->addWidget(m_temperatureStatusLabel, 1, 1);
    
    conditionsLayout->addLayout(statusGrid);
    
    auto *warningsLabel = new QLabel("Warnings:", this);
    warningsLabel->setStyleSheet(
        "QLabel {"
        "    font-family: 'SF Pro Text', 'Segoe UI', 'Arial';"
        "    font-weight: 600;"
        "    color: #fbbf24;"
        "    font-size: 13px;"
        "    margin-top: 8px;"
        "    padding: 4px 0px;"
        "}"
    );
    conditionsLayout->addWidget(warningsLabel);
    
    QString modernListStyle = 
        "QListWidget {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                stop:0 rgba(20, 25, 40, 0.6), "
        "                stop:1 rgba(15, 20, 35, 0.6));"
        "    border: 1px solid rgba(200, 220, 255, 0.1);"
        "    border-radius: 6px;"
        "    font-family: 'SF Pro Text', 'Segoe UI', 'Arial';"
        "    font-size: 12px;"
        "    color: #e2e8f0;"
        "    padding: 4px;"
        "}"
        "QListWidget::item {"
        "    padding: 6px 8px;"
        "    border-radius: 4px;"
        "    margin: 2px 0px;"
        "}"
        "QListWidget::item:hover {"
        "    background: rgba(255, 255, 255, 0.08);"
        "}";
    
    m_warningsListWidget = new QListWidget(this);
    m_warningsListWidget->setMaximumHeight(65);
    m_warningsListWidget->setMinimumHeight(45);
    m_warningsListWidget->setStyleSheet(modernListStyle);
    conditionsLayout->addWidget(m_warningsListWidget);
    
    auto *recommendationsLabel = new QLabel("Recommendations:", this);
    recommendationsLabel->setStyleSheet(
        "QLabel {"
        "    font-family: 'SF Pro Text', 'Segoe UI', 'Arial';"
        "    font-weight: 600;"
        "    color: #34d399;"
        "    font-size: 13px;"
        "    margin-top: 6px;"
        "    padding: 4px 0px;"
        "}"
    );
    conditionsLayout->addWidget(recommendationsLabel);
    
    m_recommendationsListWidget = new QListWidget(this);
    m_recommendationsListWidget->setMaximumHeight(55);
    m_recommendationsListWidget->setMinimumHeight(35);
    m_recommendationsListWidget->setStyleSheet(modernListStyle);
    conditionsLayout->addWidget(m_recommendationsListWidget);
    
    m_mainLayout->addWidget(m_flightConditionsGroup);
    
    m_forecastGroup = new QGroupBox("3-Hour Forecast", this);
    m_forecastGroup->setMinimumHeight(140);
    m_forecastGroup->setStyleSheet(modernGroupStyle);
    auto *forecastLayout = new QVBoxLayout(m_forecastGroup);
    forecastLayout->setContentsMargins(20, 24, 20, 20);
    
    m_forecastListWidget = new QListWidget(this);
    m_forecastListWidget->setMaximumHeight(110);
    m_forecastListWidget->setMinimumHeight(90);
    m_forecastListWidget->setStyleSheet(modernListStyle);
    forecastLayout->addWidget(m_forecastListWidget);
    
    m_mainLayout->addWidget(m_forecastGroup);
    
    setMinimumWidth(500);
    setMaximumWidth(700);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
}

void WeatherWidget::updateWeatherData(const WeatherData &data)
{
    updateCurrentWeather(data);
    updateForecast(data);
}

void WeatherWidget::updateCurrentWeather(const WeatherData &data)
{
    m_locationLabel->setText(QString("Location: %1").arg(data.location));
    m_conditionLabel->setText(QString("Condition: %1").arg(data.description));
    m_temperatureLabel->setText(QString("Temperature: %1").arg(formatTemperature(data.temperature)));
    m_feelsLikeLabel->setText(QString("Feels like: %1").arg(formatTemperature(data.feelsLike)));
    m_humidityLabel->setText(QString("Humidity: %1%").arg(data.humidity, 0, 'f', 0));
    m_pressureLabel->setText(QString("Pressure: %1 inHg").arg(data.pressure / 33.8639, 0, 'f', 2));
    m_visibilityLabel->setText(QString("Visibility: %1 mi").arg(data.visibility, 0, 'f', 1));
    m_cloudCoverLabel->setText(QString("Cloud Cover: %1%").arg(data.cloudCover, 0, 'f', 0));
    m_lastUpdatedLabel->setText(QString("Last updated: %1").arg(data.timestamp.toString("hh:mm:ss")));
}

void WeatherWidget::updateForecast(const WeatherData &data)
{
    m_forecastListWidget->clear();
    
    int count = 0;
    for (const auto &forecast : data.hourlyForecast) {
        if (count >= 8) break; // Show next 24 hours (8 x 3-hour intervals)
        
        QString forecastText = QString("%1 - %2, %3, Wind: %4")
                              .arg(forecast.time.toString("hh:mm"))
                              .arg(formatTemperature(forecast.temperature))
                              .arg(forecast.condition)
                              .arg(formatSpeed(forecast.windSpeed));
        
        auto *item = new QListWidgetItem(forecastText);
        m_forecastListWidget->addItem(item);
        count++;
    }
}

void WeatherWidget::updateFlightConditions(const FlightAssessment &assessment)
{
    auto getSafetyColor = [](FlightSafety safety) -> QString {
        switch (safety) {
        case FlightSafety::Safe: return "#00ff00";
        case FlightSafety::Caution: return "#ffaa00";
        case FlightSafety::Unsafe: return "#ff6600";
        case FlightSafety::NoFly: return "#ff0000";
        }
        return "#ffffff";
    };
    
    auto getSafetyString = [](FlightSafety safety) -> QString {
        switch (safety) {
        case FlightSafety::Safe: return "SAFE";
        case FlightSafety::Caution: return "CAUTION";
        case FlightSafety::Unsafe: return "UNSAFE";
        case FlightSafety::NoFly: return "NO FLY";
        }
        return "UNKNOWN";
    };
    
    m_overallStatusLabel->setText(QString("Overall: %1").arg(getSafetyString(assessment.overall)));
    m_overallStatusLabel->setStyleSheet(QString("font-weight: bold; font-size: 15px; padding: 8px; border-radius: 6px; color: %1; background-color: rgba(40, 40, 40, 0.7); border: 1px solid %2;")
                                       .arg(getSafetyColor(assessment.overall))
                                       .arg(getSafetyColor(assessment.overall)));
    
    m_windStatusLabel->setText(QString("Wind: %1").arg(getSafetyString(assessment.wind)));
    m_windStatusLabel->setStyleSheet(QString("color: %1;").arg(getSafetyColor(assessment.wind)));
    
    m_visibilityStatusLabel->setText(QString("Visibility: %1").arg(getSafetyString(assessment.visibility)));
    m_visibilityStatusLabel->setStyleSheet(QString("color: %1;").arg(getSafetyColor(assessment.visibility)));
    
    m_precipitationStatusLabel->setText(QString("Precipitation: %1").arg(getSafetyString(assessment.precipitation)));
    m_precipitationStatusLabel->setStyleSheet(QString("color: %1;").arg(getSafetyColor(assessment.precipitation)));
    
    m_temperatureStatusLabel->setText(QString("Temperature: %1").arg(getSafetyString(assessment.temperature)));
    m_temperatureStatusLabel->setStyleSheet(QString("color: %1;").arg(getSafetyColor(assessment.temperature)));
    
    m_warningsListWidget->clear();
    for (const QString &warning : assessment.warnings) {
        auto *item = new QListWidgetItem(warning);
        item->setForeground(QColor("#ff6600"));
        m_warningsListWidget->addItem(item);
    }
    
    m_recommendationsListWidget->clear();
    for (const QString &recommendation : assessment.recommendations) {
        auto *item = new QListWidgetItem(recommendation);
        item->setForeground(QColor("#00aa00"));
        m_recommendationsListWidget->addItem(item);
    }
}

QString WeatherWidget::formatTemperature(double temp) const
{
    double fahrenheit = (temp * 9.0 / 5.0) + 32.0;
    return QString("%1°F").arg(fahrenheit, 0, 'f', 1);
}

QString WeatherWidget::formatSpeed(double speed) const
{
    return QString("%1 kts").arg(speed, 0, 'f', 1);
}

QString WeatherWidget::formatDirection(double degrees) const
{
    const QStringList directions = {"N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE",
                                   "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW"};
    int index = qRound(degrees / 22.5) % 16;
    return QString("%1° (%2)").arg(degrees, 0, 'f', 0).arg(directions[index]);
}