#include "weatherservice.h"
#include <QUrl>
#include <QUrlQuery>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>
#include <QDateTime>
#include <QtMath>
#include <QSettings>

WeatherService::WeatherService(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_dataValid(false)
    , m_metarReply(nullptr)
    , m_tafReply(nullptr)
    , m_stationLookupReply(nullptr)
    , m_settings(new QSettings("DroneView", "Settings", this))
{
}

void WeatherService::fetchWeatherData(double latitude, double longitude)
{
    QString preferredAirport = getPreferredAirport();
    if (!preferredAirport.isEmpty()) {
        fetchWeatherByStation(preferredAirport);
        return;
    }
    
    QString nearestStation = findNearestStation(latitude, longitude);
    if (!nearestStation.isEmpty()) {
        fetchWeatherByStation(nearestStation);
    } else {
        QUrl stationUrl("https://aviationweather.gov/api/data/metar");
        QUrlQuery query;
        query.addQueryItem("format", "json");
        query.addQueryItem("hours", "2");
        query.addQueryItem("bbox", QString("%1,%2,%3,%4")
            .arg(latitude - 1.0, 0, 'f', 4)
            .arg(longitude - 1.0, 0, 'f', 4)
            .arg(latitude + 1.0, 0, 'f', 4)
            .arg(longitude + 1.0, 0, 'f', 4));
        stationUrl.setQuery(query);
        
        QNetworkRequest request(stationUrl);
        request.setHeader(QNetworkRequest::UserAgentHeader, "DroneView/1.0 (contact@droneview.app)");
        
        if (m_stationLookupReply) {
            m_stationLookupReply->deleteLater();
        }
        
        m_stationLookupReply = m_networkManager->get(request);
        connect(m_stationLookupReply, &QNetworkReply::finished, this, &WeatherService::handleStationLookupReply);
        connect(m_stationLookupReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred),
                this, &WeatherService::handleNetworkError);
    }
}

void WeatherService::fetchWeatherByStation(const QString &stationId)
{
    QUrl metarUrl("https://aviationweather.gov/api/data/metar");
    QUrlQuery metarQuery;
    metarQuery.addQueryItem("ids", stationId);
    metarQuery.addQueryItem("format", "json");
    metarQuery.addQueryItem("hours", "2");
    metarUrl.setQuery(metarQuery);
    
    QNetworkRequest metarRequest(metarUrl);
    metarRequest.setHeader(QNetworkRequest::UserAgentHeader, "DroneView/1.0 (contact@droneview.app)");
    
    if (m_metarReply) {
        m_metarReply->deleteLater();
    }
    
    m_metarReply = m_networkManager->get(metarRequest);
    connect(m_metarReply, &QNetworkReply::finished, this, &WeatherService::handleMetarReply);
    connect(m_metarReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred),
            this, &WeatherService::handleNetworkError);
    
    QUrl tafUrl("https://aviationweather.gov/api/data/taf");
    QUrlQuery tafQuery;
    tafQuery.addQueryItem("ids", stationId);
    tafQuery.addQueryItem("format", "json");
    tafUrl.setQuery(tafQuery);
    
    QNetworkRequest tafRequest(tafUrl);
    tafRequest.setHeader(QNetworkRequest::UserAgentHeader, "DroneView/1.0 (contact@droneview.app)");
    
    if (m_tafReply) {
        m_tafReply->deleteLater();
    }
    
    m_tafReply = m_networkManager->get(tafRequest);
    connect(m_tafReply, &QNetworkReply::finished, this, &WeatherService::handleTafReply);
    connect(m_tafReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred),
            this, &WeatherService::handleNetworkError);
}

void WeatherService::handleMetarReply()
{
    if (!m_metarReply) return;
    
    if (m_metarReply->error() == QNetworkReply::NoError) {
        QByteArray data = m_metarReply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        
        if (!doc.isNull() && doc.isArray()) {
            parseMetarData(doc.array());
        } else {
            emit errorOccurred("Invalid JSON response from Aviation Weather METAR API");
        }
    } else {
        emit errorOccurred(QString("Aviation Weather METAR API error: %1").arg(m_metarReply->errorString()));
    }
    
    m_metarReply->deleteLater();
    m_metarReply = nullptr;
}

void WeatherService::handleTafReply()
{
    if (!m_tafReply) return;
    
    if (m_tafReply->error() == QNetworkReply::NoError) {
        QByteArray data = m_tafReply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        
        if (!doc.isNull() && doc.isArray()) {
            parseTafData(doc.array());
        } else {
            emit errorOccurred("Invalid JSON response from Aviation Weather TAF API");
        }
    } else {
        emit errorOccurred(QString("Aviation Weather TAF API error: %1").arg(m_tafReply->errorString()));
    }
    
    m_tafReply->deleteLater();
    m_tafReply = nullptr;
}

void WeatherService::handleStationLookupReply()
{
    if (!m_stationLookupReply) return;
    
    if (m_stationLookupReply->error() == QNetworkReply::NoError) {
        QByteArray data = m_stationLookupReply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        
        if (!doc.isNull() && doc.isArray()) {
            QJsonArray stations = doc.array();
            if (!stations.isEmpty()) {
                QJsonObject station = stations[0].toObject();
                QString stationId = station["icaoId"].toString();
                if (!stationId.isEmpty()) {
                    fetchWeatherByStation(stationId);
                } else {
                    emit errorOccurred("No aviation weather stations found in the specified area");
                }
            } else {
                emit errorOccurred("No aviation weather stations found in the specified area");
            }
        } else {
            emit errorOccurred("Invalid JSON response from Aviation Weather station lookup");
        }
    } else {
        emit errorOccurred(QString("Aviation Weather station lookup error: %1").arg(m_stationLookupReply->errorString()));
    }
    
    m_stationLookupReply->deleteLater();
    m_stationLookupReply = nullptr;
}

void WeatherService::handleNetworkError(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error)
    emit errorOccurred("Network error occurred while fetching aviation weather data");
}

void WeatherService::parseMetarData(const QJsonArray &metars)
{
    if (metars.isEmpty()) {
        emit errorOccurred("No METAR data available for the specified station");
        return;
    }
    
    QJsonObject metar = metars[0].toObject();
    
    m_currentWeather.stationId = metar["icaoId"].toString();
    m_currentWeather.metar = metar["rawOb"].toString();
    
    if (metar.contains("lat") && metar.contains("lon")) {
        m_currentWeather.location = QString("Station %1 (%2, %3)")
            .arg(m_currentWeather.stationId)
            .arg(metar["lat"].toDouble(), 0, 'f', 4)
            .arg(metar["lon"].toDouble(), 0, 'f', 4);
    }
    
    if (metar.contains("obsTime")) {
        m_currentWeather.timestamp = QDateTime::fromString(metar["obsTime"].toString(), Qt::ISODate);
    }
    
    if (metar.contains("temp")) {
        m_currentWeather.temperature = metar["temp"].toDouble();
    }
    
    if (metar.contains("dewp")) {
        double dewpoint = metar["dewp"].toDouble();
        double temp = m_currentWeather.temperature;
        if (!qIsNaN(temp) && !qIsNaN(dewpoint)) {
            m_currentWeather.humidity = 100.0 * qExp((17.625 * dewpoint) / (243.04 + dewpoint) - (17.625 * temp) / (243.04 + temp));
        }
    }
    
    if (metar.contains("altim")) {
        m_currentWeather.altimeter = metar["altim"].toDouble();
        m_currentWeather.pressure = m_currentWeather.altimeter * 33.8639;
    }
    
    if (metar.contains("wdir") && metar.contains("wspd")) {
        m_currentWeather.windDirection = metar["wdir"].toDouble();
        m_currentWeather.windSpeed = parseWindSpeed(metar["wspd"]);
    }
    
    if (metar.contains("wgst")) {
        m_currentWeather.windGust = parseWindSpeed(metar["wgst"]);
    }
    
    if (metar.contains("visib")) {
        m_currentWeather.visibility = parseVisibility(metar["visib"]);
    }
    
    if (metar.contains("fltcat")) {
        m_currentWeather.flightCategory = metar["fltcat"].toString();
        m_currentWeather.condition = convertFlightCategory(m_currentWeather.flightCategory);
    }
    
    if (metar.contains("cover")) {
        m_currentWeather.skyCover = parseSkyCover(metar["cover"].toArray());
    }
    
    if (metar.contains("ceiling")) {
        m_currentWeather.ceiling = metar["ceiling"].toDouble();
    }
    
    m_dataValid = true;
    emit weatherDataUpdated(m_currentWeather);
}

void WeatherService::parseTafData(const QJsonArray &tafs)
{
    if (tafs.isEmpty()) {
        return;
    }
    
    QJsonObject taf = tafs[0].toObject();
    m_currentWeather.taf = taf["rawTAF"].toString();
    
    m_currentWeather.hourlyForecast.clear();
    m_currentWeather.dailyForecast.clear();
    
    if (taf.contains("fcsts") && taf["fcsts"].isArray()) {
        QJsonArray forecasts = taf["fcsts"].toArray();
        
        for (const auto &fcstValue : forecasts) {
            QJsonObject fcst = fcstValue.toObject();
            
            WeatherData::Forecast forecast;
            
            if (fcst.contains("fcstTime")) {
                forecast.time = QDateTime::fromString(fcst["fcstTime"].toString(), Qt::ISODate);
            }
            
            if (fcst.contains("temp")) {
                forecast.temperature = fcst["temp"].toDouble();
            }
            
            if (fcst.contains("wdir") && fcst.contains("wspd")) {
                forecast.windDirection = fcst["wdir"].toDouble();
                forecast.windSpeed = parseWindSpeed(fcst["wspd"]);
            }
            
            if (fcst.contains("fltcat")) {
                forecast.condition = convertFlightCategory(fcst["fltcat"].toString());
            }
            
            if (m_currentWeather.hourlyForecast.size() < 24) {
                m_currentWeather.hourlyForecast.append(forecast);
            }
        }
    }
    
    emit weatherDataUpdated(m_currentWeather);
}

QString WeatherService::findNearestStation(double latitude, double longitude)
{
    QMap<QString, QPair<double, double>> majorStations = {
        {"KJFK", {40.6398, -73.7789}},  // New York JFK
        {"KLAX", {33.9425, -118.4081}}, // Los Angeles
        {"KORD", {41.9786, -87.9048}},  // Chicago O'Hare
        {"KDFW", {32.8968, -97.0380}},  // Dallas/Fort Worth
        {"KDEN", {39.8617, -104.6731}}, // Denver
        {"KIAH", {29.9844, -95.3414}},  // Houston
        {"KSEA", {47.4502, -122.3088}}, // Seattle
        {"KPHX", {33.4343, -112.0116}}, // Phoenix
        {"KMIA", {25.7932, -80.2906}},  // Miami
        {"KBOS", {42.3656, -71.0096}},  // Boston
        {"KSFO", {37.6213, -122.3790}}, // San Francisco
        {"KATL", {33.6367, -84.4281}},  // Atlanta
        {"KMSP", {44.8848, -93.2223}},  // Minneapolis
        {"KLAS", {36.0840, -115.1537}}, // Las Vegas
        {"KDTW", {42.2124, -83.3534}},  // Detroit
        {"KPHL", {39.8719, -75.2411}},  // Philadelphia
    };
    
    QString nearestStation;
    double minDistance = std::numeric_limits<double>::max();
    
    for (auto it = majorStations.begin(); it != majorStations.end(); ++it) {
        double stationLat = it.value().first;
        double stationLon = it.value().second;
        
        double distance = qSqrt(qPow(latitude - stationLat, 2) + qPow(longitude - stationLon, 2));
        
        if (distance < minDistance) {
            minDistance = distance;
            nearestStation = it.key();
        }
    }
    
    return nearestStation;
}

QString WeatherService::convertFlightCategory(const QString &category) const
{
    if (category == "VFR") {
        return "Clear";
    } else if (category == "MVFR") {
        return "Partly Cloudy";
    } else if (category == "IFR") {
        return "Cloudy";
    } else if (category == "LIFR") {
        return "Poor Visibility";
    }
    return category;
}

QString WeatherService::parseSkyCover(const QJsonArray &skyConditions) const
{
    if (skyConditions.isEmpty()) {
        return "Clear";
    }
    
    QStringList conditions;
    for (const auto &condition : skyConditions) {
        QJsonObject sky = condition.toObject();
        QString cover = sky["cover"].toString();
        if (sky.contains("base")) {
            conditions.append(QString("%1 at %2 ft").arg(cover).arg(sky["base"].toInt()));
        } else {
            conditions.append(cover);
        }
    }
    
    return conditions.join(", ");
}

double WeatherService::parseVisibility(const QJsonValue &visibility) const
{
    if (visibility.isDouble()) {
        return visibility.toDouble();
    } else if (visibility.isString()) {
        QString visStr = visibility.toString();
        bool ok;
        double vis = visStr.toDouble(&ok);
        if (ok) {
            return vis;
        }
    }
    return 0.0;
}

double WeatherService::parseWindSpeed(const QJsonValue &windSpeed) const
{
    if (windSpeed.isDouble()) {
        return windSpeed.toDouble();
    } else if (windSpeed.isString()) {
        QString speedStr = windSpeed.toString();
        bool ok;
        double speed = speedStr.toDouble(&ok);
        if (ok) {
            return speed;
        }
    }
    return 0.0;
}

void WeatherService::setPreferredAirport(const QString &icaoCode)
{
    QString cleanCode = icaoCode.trimmed().toUpper();
    m_settings->setValue("airport/current", cleanCode);
}

QString WeatherService::getPreferredAirport() const
{
    return m_settings->value("airport/current", "KDFW").toString();
}