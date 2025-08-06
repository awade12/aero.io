#pragma once

#include <QMainWindow>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QLabel>
#include <QStatusBar>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QTimer>

class WeatherWidget;
class RadarWidget;
class WindWidget;
class WeatherService;
class LocationService;
class FlightConditions;
class SettingsDialog;
class AirportPresetWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateLocation();
    void refreshWeatherData();
    void showAbout();
    void toggleFullScreen();
    void showSettings();
    void onAirportChanged(const QString &icaoCode);

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupStyling();
    void createTabbedInterface();
    
    QWidget *m_centralWidget;
    QTabWidget *m_tabWidget;
    
    WeatherWidget *m_weatherWidget;
    RadarWidget *m_radarWidget;
    WindWidget *m_windWidget;
    
    WeatherService *m_weatherService;
    LocationService *m_locationService;
    FlightConditions *m_flightConditions;
    
    QLabel *m_locationLabel;
    QLabel *m_timeLabel;
    QLabel *m_connectionLabel;
    
    QTimer *m_updateTimer;
    QTimer *m_timeTimer;
    
    QAction *m_refreshAction;
    QAction *m_settingsAction;
    QAction *m_fullScreenAction;
    QAction *m_exitAction;
    
    AirportPresetWidget *m_airportPresetWidget;
};