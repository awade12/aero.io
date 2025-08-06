#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QListWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QTimeEdit>
#include <QDateEdit>

struct FlightPlan {
    QString name;
    QString description;
    double homeLatitude;
    double homeLongitude;
    double targetLatitude;
    double targetLongitude;
    double altitude;
    double speed;
    QTime duration;
    QDateTime plannedStart;
    QString droneType;
    QString purpose;
    
    struct Waypoint {
        double latitude;
        double longitude;
        double altitude;
        QString action;
        int duration; // seconds
    };
    
    QList<Waypoint> waypoints;
};

class FlightPlanWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FlightPlanWidget(QWidget *parent = nullptr);

public slots:
    void updateCurrentLocation(double latitude, double longitude);

private slots:
    void createNewPlan();
    void loadSelectedPlan();
    void savePlan();
    void deletePlan();
    void calculateDistance();
    void estimateFlightTime();
    void addWaypoint();
    void removeWaypoint();
    void checkWeatherForPlan();

private:
    void setupUI();
    void clearPlanForm();
    void populatePlanForm(const FlightPlan &plan);
    FlightPlan getCurrentPlan() const;
    double calculateDistanceBetweenPoints(double lat1, double lon1, double lat2, double lon2) const;
    QString formatDistance(double distance) const;
    QString formatDuration(int seconds) const;
    
    QVBoxLayout *m_mainLayout;
    
    QGroupBox *m_planListGroup;
    QListWidget *m_planListWidget;
    QPushButton *m_newPlanButton;
    QPushButton *m_loadPlanButton;
    QPushButton *m_deletePlanButton;
    
    QGroupBox *m_planDetailsGroup;
    QLineEdit *m_planNameEdit;
    QLineEdit *m_planDescriptionEdit;
    QComboBox *m_droneTypeComboBox;
    QComboBox *m_purposeComboBox;
    QDateEdit *m_plannedDateEdit;
    QTimeEdit *m_plannedTimeEdit;
    
    QGroupBox *m_flightDataGroup;
    QDoubleSpinBox *m_homeLatSpinBox;
    QDoubleSpinBox *m_homeLonSpinBox;
    QDoubleSpinBox *m_targetLatSpinBox;
    QDoubleSpinBox *m_targetLonSpinBox;
    QDoubleSpinBox *m_altitudeSpinBox;
    QDoubleSpinBox *m_speedSpinBox;
    QPushButton *m_useCurrentLocationButton;
    QPushButton *m_calculateDistanceButton;
    
    QGroupBox *m_flightAnalysisGroup;
    QLabel *m_distanceLabel;
    QLabel *m_estimatedTimeLabel;
    QLabel *m_batteryUsageLabel;
    QLabel *m_weatherSuitabilityLabel;
    QPushButton *m_checkWeatherButton;
    
    QGroupBox *m_waypointsGroup;
    QListWidget *m_waypointsListWidget;
    QPushButton *m_addWaypointButton;
    QPushButton *m_removeWaypointButton;
    
    QPushButton *m_savePlanButton;
    
    QList<FlightPlan> m_savedPlans;
    double m_currentLatitude;
    double m_currentLongitude;
};