#pragma once

#include <QObject>
#include <QGeoPositionInfoSource>
#include <QGeoPositionInfo>
#include <QTimer>
#include <QLocationPermission>

class LocationService : public QObject
{
    Q_OBJECT

public:
    explicit LocationService(QObject *parent = nullptr);
    ~LocationService();
    
    QGeoCoordinate currentPosition() const;
    bool isLocationAvailable() const;
    void startLocationUpdates();
    void stopLocationUpdates();
    void requestLocationPermission();

signals:
    void locationUpdated(const QGeoCoordinate &coordinate);
    void locationError(const QString &error);

private slots:
    void positionUpdated(const QGeoPositionInfo &info);
    void locationTimeout();
    void handleLocationError(QGeoPositionInfoSource::Error error);

private:
    void useDefaultLocation();
    void initializeLocationSource();
    
    QGeoPositionInfoSource *m_positionSource;
    QGeoCoordinate m_currentPosition;
    QTimer *m_locationTimer;
    bool m_locationAvailable;
};