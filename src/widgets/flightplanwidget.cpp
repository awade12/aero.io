#include "flightplanwidget.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QTimer>
#include <QSizePolicy>
#include <qmath.h>

FlightPlanWidget::FlightPlanWidget(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_currentLatitude(37.7749)
    , m_currentLongitude(-122.4194)
{
    setupUI();
}

void FlightPlanWidget::setupUI()
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
    
    m_planListGroup = new QGroupBox("Saved Flight Plans", this);
    m_planListGroup->setMinimumHeight(140);
    m_planListGroup->setStyleSheet(modernGroupStyle);
    auto *listLayout = new QVBoxLayout(m_planListGroup);
    listLayout->setSpacing(12);
    listLayout->setContentsMargins(20, 24, 20, 20);
    
    QString modernListStyle = 
        "QListWidget {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                stop:0 rgba(20, 25, 40, 0.6), "
        "                stop:1 rgba(15, 20, 35, 0.6));"
        "    border: 1px solid rgba(200, 220, 255, 0.1);"
        "    border-radius: 8px;"
        "    font-family: 'SF Pro Text', 'Segoe UI', 'Arial';"
        "    font-size: 12px;"
        "    color: #e2e8f0;"
        "    padding: 4px;"
        "}"
        "QListWidget::item {"
        "    padding: 8px 12px;"
        "    border-radius: 6px;"
        "    margin: 2px 0px;"
        "    border: none;"
        "}"
        "QListWidget::item:selected {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                stop:0 rgba(200, 220, 255, 0.25), "
        "                stop:1 rgba(200, 220, 255, 0.15));"
        "    color: #f0f6ff;"
        "    border: 1px solid rgba(200, 220, 255, 0.3);"
        "}"
        "QListWidget::item:hover {"
        "    background: rgba(255, 255, 255, 0.08);"
        "}";
    
    m_planListWidget = new QListWidget(this);
    m_planListWidget->setMaximumHeight(70);
    m_planListWidget->setMinimumHeight(50);
    m_planListWidget->setStyleSheet(modernListStyle);
    listLayout->addWidget(m_planListWidget);
    
    QString modernButtonStyle = 
        "QPushButton {"
        "    font-family: 'SF Pro Text', 'Segoe UI', 'Arial';"
        "    font-weight: 500;"
        "    font-size: 12px;"
        "    color: #f0f6ff;"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                stop:0 rgba(255, 255, 255, 0.1), "
        "                stop:1 rgba(255, 255, 255, 0.05));"
        "    border: 1px solid rgba(200, 220, 255, 0.2);"
        "    border-radius: 8px;"
        "    padding: 8px 16px;"
        "    min-width: 70px;"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                stop:0 rgba(200, 220, 255, 0.25), "
        "                stop:1 rgba(200, 220, 255, 0.15));"
        "    border-color: rgba(200, 220, 255, 0.4);"
        "}"
        "QPushButton:pressed {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                stop:0 rgba(200, 220, 255, 0.35), "
        "                stop:1 rgba(200, 220, 255, 0.25));"
        "}";
    
    auto *listButtonsLayout = new QHBoxLayout();
    listButtonsLayout->setSpacing(12);
    
    m_newPlanButton = new QPushButton("New", this);
    m_newPlanButton->setStyleSheet(modernButtonStyle);
    m_loadPlanButton = new QPushButton("Load", this);
    m_loadPlanButton->setStyleSheet(modernButtonStyle);
    m_deletePlanButton = new QPushButton("Delete", this);
    m_deletePlanButton->setStyleSheet(modernButtonStyle);
    
    listButtonsLayout->addWidget(m_newPlanButton);
    listButtonsLayout->addWidget(m_loadPlanButton);
    listButtonsLayout->addWidget(m_deletePlanButton);
    listButtonsLayout->addStretch();
    
    listLayout->addLayout(listButtonsLayout);
    m_mainLayout->addWidget(m_planListGroup);
    
    m_planDetailsGroup = new QGroupBox("Flight Plan Details", this);
    m_planDetailsGroup->setMinimumHeight(180);
    m_planDetailsGroup->setStyleSheet(modernGroupStyle);
    auto *detailsLayout = new QGridLayout(m_planDetailsGroup);
    detailsLayout->setSpacing(14);
    detailsLayout->setContentsMargins(20, 24, 20, 20);
    detailsLayout->setColumnStretch(1, 1);
    
    QString modernLabelStyle = 
        "QLabel {"
        "    font-family: 'SF Pro Text', 'Segoe UI', 'Arial';"
        "    font-weight: 600;"
        "    font-size: 13px;"
        "    color: #f0f6ff;"
        "    padding: 4px 0px;"
        "}";
        
    QString modernInputStyle = 
        "QLineEdit, QComboBox, QDateEdit, QTimeEdit {"
        "    font-family: 'SF Pro Text', 'Segoe UI', 'Arial';"
        "    font-size: 12px;"
        "    color: #f0f6ff;"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                stop:0 rgba(20, 25, 40, 0.8), "
        "                stop:1 rgba(15, 20, 35, 0.8));"
        "    border: 1px solid rgba(200, 220, 255, 0.15);"
        "    border-radius: 8px;"
        "    padding: 8px 12px;"
        "}"
        "QLineEdit:focus, QComboBox:focus, QDateEdit:focus, QTimeEdit:focus {"
        "    border-color: rgba(200, 220, 255, 0.4);"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                stop:0 rgba(25, 30, 45, 0.9), "
        "                stop:1 rgba(20, 25, 40, 0.9));"
        "}"
        "QComboBox::drop-down {"
        "    border: none;"
        "    border-radius: 4px;"
        "}"
        "QComboBox::down-arrow {"
        "    image: none;"
        "    border: none;"
        "}";
    
    auto *planNameLabel = new QLabel("Plan Name:", this);
    planNameLabel->setStyleSheet(modernLabelStyle);
    detailsLayout->addWidget(planNameLabel, 0, 0);
    m_planNameEdit = new QLineEdit(this);
    m_planNameEdit->setStyleSheet(modernInputStyle);
    detailsLayout->addWidget(m_planNameEdit, 0, 1);
    
    auto *descLabel = new QLabel("Description:", this);
    descLabel->setStyleSheet(modernLabelStyle);
    detailsLayout->addWidget(descLabel, 1, 0);
    m_planDescriptionEdit = new QLineEdit(this);
    m_planDescriptionEdit->setStyleSheet(modernInputStyle);
    detailsLayout->addWidget(m_planDescriptionEdit, 1, 1);
    
    auto *droneLabel = new QLabel("Drone Type:", this);
    droneLabel->setStyleSheet(modernLabelStyle);
    detailsLayout->addWidget(droneLabel, 2, 0);
    m_droneTypeComboBox = new QComboBox(this);
    m_droneTypeComboBox->addItems({"DJI Mini", "DJI Air", "DJI Phantom", "DJI Mavic", "Custom"});
    m_droneTypeComboBox->setStyleSheet(modernInputStyle);
    detailsLayout->addWidget(m_droneTypeComboBox, 2, 1);
    
    auto *purposeLabel = new QLabel("Purpose:", this);
    purposeLabel->setStyleSheet(modernLabelStyle);
    detailsLayout->addWidget(purposeLabel, 3, 0);
    m_purposeComboBox = new QComboBox(this);
    m_purposeComboBox->addItems({"Photography", "Surveying", "Inspection", "Recreation", "Training", "Commercial"});
    m_purposeComboBox->setStyleSheet(modernInputStyle);
    detailsLayout->addWidget(m_purposeComboBox, 3, 1);
    
    auto *dateLabel = new QLabel("Planned Date:", this);
    dateLabel->setStyleSheet(modernLabelStyle);
    detailsLayout->addWidget(dateLabel, 4, 0);
    m_plannedDateEdit = new QDateEdit(QDate::currentDate(), this);
    m_plannedDateEdit->setStyleSheet(modernInputStyle);
    detailsLayout->addWidget(m_plannedDateEdit, 4, 1);
    
    auto *timeLabel = new QLabel("Planned Time:", this);
    timeLabel->setStyleSheet(modernLabelStyle);
    detailsLayout->addWidget(timeLabel, 5, 0);
    m_plannedTimeEdit = new QTimeEdit(QTime::currentTime(), this);
    m_plannedTimeEdit->setStyleSheet(modernInputStyle);
    detailsLayout->addWidget(m_plannedTimeEdit, 5, 1);
    
    m_mainLayout->addWidget(m_planDetailsGroup);
    
    m_flightDataGroup = new QGroupBox("Flight Coordinates", this);
    m_flightDataGroup->setMinimumHeight(140);
    m_flightDataGroup->setStyleSheet(modernGroupStyle);
    auto *flightLayout = new QGridLayout(m_flightDataGroup);
    flightLayout->setSpacing(12);
    flightLayout->setContentsMargins(20, 24, 20, 20);
    flightLayout->setColumnStretch(1, 1);
    flightLayout->setColumnStretch(3, 1);
    
    // Modern spin box style
    QString modernSpinBoxStyle = 
        "QDoubleSpinBox {"
        "    font-family: 'SF Pro Text', 'Segoe UI', 'Arial';"
        "    font-size: 12px;"
        "    color: #f0f6ff;"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                stop:0 rgba(20, 25, 40, 0.8), "
        "                stop:1 rgba(15, 20, 35, 0.8));"
        "    border: 1px solid rgba(200, 220, 255, 0.15);"
        "    border-radius: 6px;"
        "    padding: 6px 8px;"
        "}"
        "QDoubleSpinBox:focus {"
        "    border-color: rgba(200, 220, 255, 0.4);"
        "}";
    
    auto *homeLatLabel = new QLabel("Home Lat:", this);
    homeLatLabel->setStyleSheet(modernLabelStyle);
    flightLayout->addWidget(homeLatLabel, 0, 0);
    m_homeLatSpinBox = new QDoubleSpinBox(this);
    m_homeLatSpinBox->setRange(-90.0, 90.0);
    m_homeLatSpinBox->setDecimals(6);
    m_homeLatSpinBox->setValue(m_currentLatitude);
    m_homeLatSpinBox->setStyleSheet(modernSpinBoxStyle);
    flightLayout->addWidget(m_homeLatSpinBox, 0, 1);
    
    auto *homeLonLabel = new QLabel("Home Lon:", this);
    homeLonLabel->setStyleSheet(modernLabelStyle);
    flightLayout->addWidget(homeLonLabel, 0, 2);
    m_homeLonSpinBox = new QDoubleSpinBox(this);
    m_homeLonSpinBox->setRange(-180.0, 180.0);
    m_homeLonSpinBox->setDecimals(6);
    m_homeLonSpinBox->setValue(m_currentLongitude);
    m_homeLonSpinBox->setStyleSheet(modernSpinBoxStyle);
    flightLayout->addWidget(m_homeLonSpinBox, 0, 3);
    
    auto *targetLatLabel = new QLabel("Target Lat:", this);
    targetLatLabel->setStyleSheet(modernLabelStyle);
    flightLayout->addWidget(targetLatLabel, 1, 0);
    m_targetLatSpinBox = new QDoubleSpinBox(this);
    m_targetLatSpinBox->setRange(-90.0, 90.0);
    m_targetLatSpinBox->setDecimals(6);
    m_targetLatSpinBox->setValue(m_currentLatitude);
    m_targetLatSpinBox->setStyleSheet(modernSpinBoxStyle);
    flightLayout->addWidget(m_targetLatSpinBox, 1, 1);
    
    auto *targetLonLabel = new QLabel("Target Lon:", this);
    targetLonLabel->setStyleSheet(modernLabelStyle);
    flightLayout->addWidget(targetLonLabel, 1, 2);
    m_targetLonSpinBox = new QDoubleSpinBox(this);
    m_targetLonSpinBox->setRange(-180.0, 180.0);
    m_targetLonSpinBox->setDecimals(6);
    m_targetLonSpinBox->setValue(m_currentLongitude);
    m_targetLonSpinBox->setStyleSheet(modernSpinBoxStyle);
    flightLayout->addWidget(m_targetLonSpinBox, 1, 3);
    
    auto *altitudeLabel = new QLabel("Altitude (ft):", this);
    altitudeLabel->setStyleSheet(modernLabelStyle);
    flightLayout->addWidget(altitudeLabel, 2, 0);
    m_altitudeSpinBox = new QDoubleSpinBox(this);
    m_altitudeSpinBox->setRange(3.3, 1312.3);
    m_altitudeSpinBox->setValue(328.1);
    m_altitudeSpinBox->setStyleSheet(modernSpinBoxStyle);
    flightLayout->addWidget(m_altitudeSpinBox, 2, 1);
    
    auto *speedLabel = new QLabel("Speed (mph):", this);
    speedLabel->setStyleSheet(modernLabelStyle);
    flightLayout->addWidget(speedLabel, 2, 2);
    m_speedSpinBox = new QDoubleSpinBox(this);
    m_speedSpinBox->setRange(2.2, 44.7);
    m_speedSpinBox->setValue(11.2);
    m_speedSpinBox->setStyleSheet(modernSpinBoxStyle);
    flightLayout->addWidget(m_speedSpinBox, 2, 3);
    
    m_useCurrentLocationButton = new QPushButton("Use Current Location", this);
    m_useCurrentLocationButton->setStyleSheet(modernButtonStyle);
    flightLayout->addWidget(m_useCurrentLocationButton, 3, 0, 1, 2);
    
    m_calculateDistanceButton = new QPushButton("Calculate Distance", this);
    m_calculateDistanceButton->setStyleSheet(modernButtonStyle);
    flightLayout->addWidget(m_calculateDistanceButton, 3, 2, 1, 2);
    
    m_mainLayout->addWidget(m_flightDataGroup);
    
    m_flightAnalysisGroup = new QGroupBox("Flight Analysis", this);
    m_flightAnalysisGroup->setMinimumHeight(80);
    auto *analysisLayout = new QGridLayout(m_flightAnalysisGroup);
    analysisLayout->setSpacing(6);
    
    m_distanceLabel = new QLabel("Distance: --", this);
    analysisLayout->addWidget(m_distanceLabel, 0, 0);
    
    m_estimatedTimeLabel = new QLabel("Est. Time: --", this);
    analysisLayout->addWidget(m_estimatedTimeLabel, 0, 1);
    
    m_batteryUsageLabel = new QLabel("Battery: --", this);
    analysisLayout->addWidget(m_batteryUsageLabel, 1, 0);
    
    m_weatherSuitabilityLabel = new QLabel("Weather: --", this);
    analysisLayout->addWidget(m_weatherSuitabilityLabel, 1, 1);
    
    m_checkWeatherButton = new QPushButton("Check Weather", this);
    analysisLayout->addWidget(m_checkWeatherButton, 2, 0, 1, 2);
    
    m_mainLayout->addWidget(m_flightAnalysisGroup);
    
    m_savePlanButton = new QPushButton("Save Flight Plan", this);
    m_savePlanButton->setStyleSheet("QPushButton { background-color: #0066cc; color: white; font-weight: bold; padding: 8px; }");
    m_mainLayout->addWidget(m_savePlanButton);
    
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    connect(m_newPlanButton, &QPushButton::clicked, this, &FlightPlanWidget::createNewPlan);
    connect(m_loadPlanButton, &QPushButton::clicked, this, &FlightPlanWidget::loadSelectedPlan);
    connect(m_deletePlanButton, &QPushButton::clicked, this, &FlightPlanWidget::deletePlan);
    connect(m_savePlanButton, &QPushButton::clicked, this, &FlightPlanWidget::savePlan);
    connect(m_useCurrentLocationButton, &QPushButton::clicked, [this]() {
        m_homeLatSpinBox->setValue(m_currentLatitude);
        m_homeLonSpinBox->setValue(m_currentLongitude);
    });
    connect(m_calculateDistanceButton, &QPushButton::clicked, this, &FlightPlanWidget::calculateDistance);
    connect(m_checkWeatherButton, &QPushButton::clicked, this, &FlightPlanWidget::checkWeatherForPlan);
    
    connect(m_homeLatSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &FlightPlanWidget::estimateFlightTime);
    connect(m_homeLonSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &FlightPlanWidget::estimateFlightTime);
    connect(m_targetLatSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &FlightPlanWidget::estimateFlightTime);
    connect(m_targetLonSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &FlightPlanWidget::estimateFlightTime);
    connect(m_speedSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &FlightPlanWidget::estimateFlightTime);
}

void FlightPlanWidget::updateCurrentLocation(double latitude, double longitude)
{
    m_currentLatitude = latitude;
    m_currentLongitude = longitude;
}

void FlightPlanWidget::createNewPlan()
{
    clearPlanForm();
    m_planNameEdit->setFocus();
}

void FlightPlanWidget::loadSelectedPlan()
{
    auto *currentItem = m_planListWidget->currentItem();
    if (!currentItem) {
        QMessageBox::information(this, "No Selection", "Please select a flight plan to load.");
        return;
    }
    
    QString planName = currentItem->text();
    for (const auto &plan : m_savedPlans) {
        if (plan.name == planName) {
            populatePlanForm(plan);
            break;
        }
    }
}

void FlightPlanWidget::savePlan()
{
    FlightPlan plan = getCurrentPlan();
    
    if (plan.name.isEmpty()) {
        QMessageBox::warning(this, "Invalid Plan", "Please enter a plan name.");
        return;
    }
    
    bool found = false;
    for (auto &existingPlan : m_savedPlans) {
        if (existingPlan.name == plan.name) {
            existingPlan = plan;
            found = true;
            break;
        }
    }
    
    if (!found) {
        m_savedPlans.append(plan);
        m_planListWidget->addItem(plan.name);
    }
    
    QMessageBox::information(this, "Plan Saved", QString("Flight plan '%1' has been saved.").arg(plan.name));
}

void FlightPlanWidget::deletePlan()
{
    auto *currentItem = m_planListWidget->currentItem();
    if (!currentItem) {
        QMessageBox::information(this, "No Selection", "Please select a flight plan to delete.");
        return;
    }
    
    QString planName = currentItem->text();
    int ret = QMessageBox::question(this, "Confirm Delete",
                                   QString("Are you sure you want to delete flight plan '%1'?").arg(planName),
                                   QMessageBox::Yes | QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        m_savedPlans.removeIf([planName](const FlightPlan &plan) {
            return plan.name == planName;
        });
        
        delete m_planListWidget->takeItem(m_planListWidget->currentRow());
        QMessageBox::information(this, "Plan Deleted", QString("Flight plan '%1' has been deleted.").arg(planName));
    }
}

void FlightPlanWidget::calculateDistance()
{
    double distance = calculateDistanceBetweenPoints(
        m_homeLatSpinBox->value(), m_homeLonSpinBox->value(),
        m_targetLatSpinBox->value(), m_targetLonSpinBox->value()
    );
    
    m_distanceLabel->setText(QString("Distance: %1").arg(formatDistance(distance)));
    estimateFlightTime();
}

void FlightPlanWidget::estimateFlightTime()
{
    double distance = calculateDistanceBetweenPoints(
        m_homeLatSpinBox->value(), m_homeLonSpinBox->value(),
        m_targetLatSpinBox->value(), m_targetLonSpinBox->value()
    );
    
    double speed = m_speedSpinBox->value();
    int flightTime = (int)(distance / speed); // seconds
    int totalTime = flightTime * 2; // Round trip
    
    m_distanceLabel->setText(QString("Distance: %1").arg(formatDistance(distance)));
    m_estimatedTimeLabel->setText(QString("Est. Time: %1").arg(formatDuration(totalTime)));
    
    int batteryUsage = (int)((totalTime / 60.0) / 20.0 * 100); // Assuming 20 min flight time
    m_batteryUsageLabel->setText(QString("Battery: %1%").arg(qMin(100, batteryUsage)));
    
    if (batteryUsage > 80) {
        m_batteryUsageLabel->setStyleSheet("color: #ff0000;");
    } else if (batteryUsage > 60) {
        m_batteryUsageLabel->setStyleSheet("color: #ff6600;");
    } else {
        m_batteryUsageLabel->setStyleSheet("color: #00aa00;");
    }
}

void FlightPlanWidget::addWaypoint()
{
}

void FlightPlanWidget::removeWaypoint()
{
}

void FlightPlanWidget::checkWeatherForPlan()
{
    m_weatherSuitabilityLabel->setText("Weather: Checking...");
    m_weatherSuitabilityLabel->setStyleSheet("color: #ffaa00;");
    
    QTimer::singleShot(2000, [this]() {
        m_weatherSuitabilityLabel->setText("Weather: Good for flight");
        m_weatherSuitabilityLabel->setStyleSheet("color: #00aa00;");
    });
}

void FlightPlanWidget::clearPlanForm()
{
    m_planNameEdit->clear();
    m_planDescriptionEdit->clear();
    m_droneTypeComboBox->setCurrentIndex(0);
    m_purposeComboBox->setCurrentIndex(0);
    m_plannedDateEdit->setDate(QDate::currentDate());
    m_plannedTimeEdit->setTime(QTime::currentTime());
    m_homeLatSpinBox->setValue(m_currentLatitude);
    m_homeLonSpinBox->setValue(m_currentLongitude);
    m_targetLatSpinBox->setValue(m_currentLatitude);
    m_targetLonSpinBox->setValue(m_currentLongitude);
    m_altitudeSpinBox->setValue(100.0);
    m_speedSpinBox->setValue(5.0);
    m_distanceLabel->setText("Distance: --");
    m_estimatedTimeLabel->setText("Est. Time: --");
    m_batteryUsageLabel->setText("Battery: --");
    m_weatherSuitabilityLabel->setText("Weather: --");
}

void FlightPlanWidget::populatePlanForm(const FlightPlan &plan)
{
    m_planNameEdit->setText(plan.name);
    m_planDescriptionEdit->setText(plan.description);
    m_droneTypeComboBox->setCurrentText(plan.droneType);
    m_purposeComboBox->setCurrentText(plan.purpose);
    m_plannedDateEdit->setDate(plan.plannedStart.date());
    m_plannedTimeEdit->setTime(plan.plannedStart.time());
    m_homeLatSpinBox->setValue(plan.homeLatitude);
    m_homeLonSpinBox->setValue(plan.homeLongitude);
    m_targetLatSpinBox->setValue(plan.targetLatitude);
    m_targetLonSpinBox->setValue(plan.targetLongitude);
    m_altitudeSpinBox->setValue(plan.altitude * 3.28084);
    m_speedSpinBox->setValue(plan.speed * 2.23694);
    
    calculateDistance();
}

FlightPlan FlightPlanWidget::getCurrentPlan() const
{
    FlightPlan plan;
    plan.name = m_planNameEdit->text();
    plan.description = m_planDescriptionEdit->text();
    plan.droneType = m_droneTypeComboBox->currentText();
    plan.purpose = m_purposeComboBox->currentText();
    plan.plannedStart = QDateTime(m_plannedDateEdit->date(), m_plannedTimeEdit->time());
    plan.homeLatitude = m_homeLatSpinBox->value();
    plan.homeLongitude = m_homeLonSpinBox->value();
    plan.targetLatitude = m_targetLatSpinBox->value();
    plan.targetLongitude = m_targetLonSpinBox->value();
    plan.altitude = m_altitudeSpinBox->value() / 3.28084;
    plan.speed = m_speedSpinBox->value() / 2.23694;
    
    return plan;
}

double FlightPlanWidget::calculateDistanceBetweenPoints(double lat1, double lon1, double lat2, double lon2) const
{
    const double R = 6371000; // Earth's radius in meters
    
    double lat1Rad = qDegreesToRadians(lat1);
    double lat2Rad = qDegreesToRadians(lat2);
    double deltaLatRad = qDegreesToRadians(lat2 - lat1);
    double deltaLonRad = qDegreesToRadians(lon2 - lon1);
    
    double a = qSin(deltaLatRad / 2) * qSin(deltaLatRad / 2) +
               qCos(lat1Rad) * qCos(lat2Rad) *
               qSin(deltaLonRad / 2) * qSin(deltaLonRad / 2);
    double c = 2 * qAtan2(qSqrt(a), qSqrt(1 - a));
    
    return R * c;
}

QString FlightPlanWidget::formatDistance(double distance) const
{
    double feet = distance * 3.28084;
    double miles = distance / 1609.34;
    
    if (feet < 5280) {
        return QString("%1 ft").arg(feet, 0, 'f', 0);
    } else {
        return QString("%1 mi").arg(miles, 0, 'f', 2);
    }
}

QString FlightPlanWidget::formatDuration(int seconds) const
{
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs = seconds % 60;
    
    if (hours > 0) {
        return QString("%1h %2m %3s").arg(hours).arg(minutes).arg(secs);
    } else if (minutes > 0) {
        return QString("%1m %2s").arg(minutes).arg(secs);
    } else {
        return QString("%1s").arg(secs);
    }
}