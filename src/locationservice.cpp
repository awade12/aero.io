#include "locationservice.h"
#include <QDebug>
#include <QCoreApplication>
#include <QPermissions>

LocationService::LocationService(QObject *parent)
    : QObject(parent)
    , m_positionSource(nullptr)
    , m_currentPosition(32.776667, -96.796944) // default to dallas texas later
    , m_locationTimer(new QTimer(this))
    , m_locationAvailable(false)
{
    qDebug() << "LocationService: Initializing...";
    
    // Setup timeout timer
    connect(m_locationTimer, &QTimer::timeout, this, &LocationService::locationTimeout);
    m_locationTimer->setSingleShot(true);
    
    // Request location permission first
    requestLocationPermission();
}

LocationService::~LocationService()
{
    if (m_positionSource) {
        m_positionSource->stopUpdates();
    }
}

QGeoCoordinate LocationService::currentPosition() const
{
    return m_currentPosition;
}

bool LocationService::isLocationAvailable() const
{
    return m_locationAvailable;
}

void LocationService::requestLocationPermission()
{
    qDebug() << "LocationService: Requesting location permission...";
    
    QLocationPermission permission;
    permission.setAccuracy(QLocationPermission::Precise);
    permission.setAvailability(QLocationPermission::WhenInUse);
    
    Qt::PermissionStatus status = qApp->checkPermission(permission);
    
    switch (status) {
    case Qt::PermissionStatus::Granted:
        qDebug() << "LocationService: Permission already granted";
        initializeLocationSource();
        break;
    case Qt::PermissionStatus::Denied:
        qDebug() << "LocationService: Permission denied";
        emit locationError("Location access denied");
        useDefaultLocation();
        break;
    case Qt::PermissionStatus::Undetermined:
        qDebug() << "LocationService: Requesting permission from user...";
        qApp->requestPermission(permission, [this](const QPermission &result) {
            if (result.status() == Qt::PermissionStatus::Granted) {
                qDebug() << "LocationService: Permission granted by user";
                initializeLocationSource();
            } else {
                qDebug() << "LocationService: Permission denied by user";
                emit locationError("Location access denied");
                useDefaultLocation();
            }
        });
        break;
    }
}



void LocationService::initializeLocationSource()
{
    qDebug() << "LocationService: Creating location source...";
    
    // List available sources first
    QStringList availableSources = QGeoPositionInfoSource::availableSources();
    qDebug() << "LocationService: Available positioning sources:" << availableSources;
    
    // Create location source
    m_positionSource = QGeoPositionInfoSource::createDefaultSource(this);
    
    if (m_positionSource) {
        qDebug() << "LocationService: Found location source:" << m_positionSource->sourceName();
        
        connect(m_positionSource, &QGeoPositionInfoSource::positionUpdated,
                this, &LocationService::positionUpdated);
        connect(m_positionSource, QOverload<QGeoPositionInfoSource::Error>::of(&QGeoPositionInfoSource::errorOccurred),
                this, &LocationService::handleLocationError);
        
        // Configure for best results
        qDebug() << "LocationService: Configuring location source...";
        
        // Check what methods are supported
        auto supportedMethods = m_positionSource->supportedPositioningMethods();
        qDebug() << "LocationService: Supported positioning methods:" << supportedMethods;
        
        if (supportedMethods & QGeoPositionInfoSource::SatellitePositioningMethods) {
            qDebug() << "LocationService: GPS/GNSS is available";
        }
        if (supportedMethods & QGeoPositionInfoSource::NonSatellitePositioningMethods) {
            qDebug() << "LocationService: Network positioning (WiFi/Cell) is available";
        }
        
        // Set preferred methods (prefer GPS but allow network)
        m_positionSource->setPreferredPositioningMethods(
            QGeoPositionInfoSource::AllPositioningMethods);
        
        // Set update interval
        int minInterval = m_positionSource->minimumUpdateInterval();
        int desiredInterval = qMax(5000, minInterval); // At least 5 seconds or minimum supported
        m_positionSource->setUpdateInterval(desiredInterval);
        
        qDebug() << "LocationService: Minimum update interval:" << minInterval << "ms";
        qDebug() << "LocationService: Set update interval:" << desiredInterval << "ms";
        
        startLocationUpdates();
    } else {
        qDebug() << "LocationService: No location source available, checking alternatives...";
        
        // Try creating specific sources if default fails
        if (!availableSources.isEmpty()) {
            qDebug() << "LocationService: Trying first available source:" << availableSources.first();
            m_positionSource = QGeoPositionInfoSource::createSource(availableSources.first(), this);
            if (m_positionSource) {
                qDebug() << "LocationService: Successfully created alternative source";
                // Reconnect and configure
                connect(m_positionSource, &QGeoPositionInfoSource::positionUpdated,
                        this, &LocationService::positionUpdated);
                connect(m_positionSource, QOverload<QGeoPositionInfoSource::Error>::of(&QGeoPositionInfoSource::errorOccurred),
                        this, &LocationService::handleLocationError);
                
                m_positionSource->setPreferredPositioningMethods(QGeoPositionInfoSource::AllPositioningMethods);
                startLocationUpdates();
                return;
            }
        }
        
        qDebug() << "LocationService: No positioning sources available, using default location";
        useDefaultLocation();
    }
}

void LocationService::startLocationUpdates()
{
    if (m_positionSource) {
        qDebug() << "LocationService: Starting location updates...";
        
        // Try requesting a single update first
        m_positionSource->requestUpdate(10000); // 10 second timeout for single update
        
        // Also start continuous updates
        m_positionSource->startUpdates();
        
        qDebug() << "LocationService: Location updates started";
        
        // Set timeout for first position  
        m_locationTimer->start(20000); // 20 second timeout (give it more time)
    }
}

void LocationService::stopLocationUpdates()
{
    if (m_positionSource) {
        m_positionSource->stopUpdates();
        qDebug() << "Location updates stopped";
    }
}

void LocationService::positionUpdated(const QGeoPositionInfo &info)
{
    if (info.isValid()) {
        m_currentPosition = info.coordinate();
        m_locationAvailable = true;
        m_locationTimer->stop();
        
        qDebug() << "LocationService: *** LOCATION UPDATED ***";
        qDebug() << "LocationService: Latitude:" << m_currentPosition.latitude();
        qDebug() << "LocationService: Longitude:" << m_currentPosition.longitude();
        qDebug() << "LocationService: Accuracy:" << info.attribute(QGeoPositionInfo::HorizontalAccuracy) << "meters";
        qDebug() << "LocationService: Timestamp:" << info.timestamp();
        
        emit locationUpdated(m_currentPosition);
    } else {
        qDebug() << "LocationService: Received invalid position info";
    }
}

void LocationService::locationTimeout()
{
    qDebug() << "LocationService: Initial location request timed out after 20 seconds";
    
    if (m_positionSource && !m_locationAvailable) {
        qDebug() << "LocationService: No location received yet, continuing GPS in background...";
        qDebug() << "LocationService: Using default location (Dallas) for now";
        
        // Use default location for now but keep GPS running
        m_locationAvailable = true;
        emit locationUpdated(m_currentPosition); // Emit Dallas coordinates
        
        // GPS will keep running and update when it gets a real fix
    } else if (!m_positionSource) {
        qDebug() << "LocationService: No location source available";
        useDefaultLocation();
    }
}

void LocationService::useDefaultLocation()
{
    m_locationAvailable = true; // Mark as available since we have a default
    qDebug() << "LocationService: Using default location (Dallas):" << m_currentPosition.latitude() << m_currentPosition.longitude();
    emit locationUpdated(m_currentPosition);
}

void LocationService::handleLocationError(QGeoPositionInfoSource::Error error)
{
    QString errorString;
    switch (error) {
    case QGeoPositionInfoSource::AccessError:
        errorString = "Location access denied";
        qDebug() << "LocationService: ERROR - Location access denied";
        qDebug() << "LocationService: This might be a permission issue";
        break;
    case QGeoPositionInfoSource::ClosedError:
        errorString = "Location service closed";
        qDebug() << "LocationService: ERROR - Location service closed";
        break;
    case QGeoPositionInfoSource::NoError:
        return;
    default:
        errorString = "Unknown location error";
        qDebug() << "LocationService: ERROR - Unknown location error:" << error;
        break;
    }
    
    qDebug() << "LocationService: Continuing with GPS running despite error...";
    // Don't stop trying - GPS might work later
}

