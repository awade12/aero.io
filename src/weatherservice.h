#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTimer>
#include <QSettings>

struct WeatherData {
    QString condition;
    QString description;
    double temperature;
    double feelsLike;
    double humidity;
    double pressure;
    double windSpeed;
    double windDirection;
    double windGust;
    double visibility;
    double cloudCover;
    double uvIndex;
    QString location;
    QString stationId;
    QDateTime timestamp;
    
    // Aviation-specific data
    QString metar;
    QString taf;
    double altimeter;
    QString flightCategory;
    QString skyCover;
    double ceiling;
    
    struct Forecast {
        QDateTime time;
        QString condition;
        double temperature;
        double windSpeed;
        double windDirection;
        double precipitation;
    };
    
    QList<Forecast> hourlyForecast;
    QList<Forecast> dailyForecast;
};

class WeatherService : public QObject
{
    Q_OBJECT

public:
    explicit WeatherService(QObject *parent = nullptr);
    
    void fetchWeatherData(double latitude, double longitude);
    void fetchWeatherByStation(const QString &stationId);
    void setPreferredAirport(const QString &icaoCode);
    QString getPreferredAirport() const;
    
    const WeatherData& currentWeather() const { return m_currentWeather; }
    bool isDataValid() const { return m_dataValid; }

signals:
    void weatherDataUpdated(const WeatherData &data);
    void errorOccurred(const QString &error);

private slots:
    void handleMetarReply();
    void handleTafReply();
    void handleStationLookupReply();
    void handleNetworkError(QNetworkReply::NetworkError error);

private:
    void parseMetarData(const QJsonArray &metars);
    void parseTafData(const QJsonArray &tafs);
    QString findNearestStation(double latitude, double longitude);
    QString convertFlightCategory(const QString &category) const;
    QString parseSkyCover(const QJsonArray &skyConditions) const;
    double parseVisibility(const QJsonValue &visibility) const;
    double parseWindSpeed(const QJsonValue &windSpeed) const;
    
    QNetworkAccessManager *m_networkManager;
    WeatherData m_currentWeather;
    bool m_dataValid;
    
    QNetworkReply *m_metarReply;
    QNetworkReply *m_tafReply;
    QNetworkReply *m_stationLookupReply;
    
    QSettings *m_settings;
};