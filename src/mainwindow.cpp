#include "mainwindow.h"
#include "weatherservice.h"
#include "locationservice.h"
#include "flightconditions.h"
#include "widgets/weatherwidget.h"
#include "widgets/radarwidget.h"
#include "widgets/windwidget.h"
#include "widgets/airportpresetwidget.h"
#include "settingsdialog.h"
#include "aboutdialog.h"


#include <QApplication>
#include <QMessageBox>
#include <QDateTime>
#include <QSplitter>
#include <QGeoCoordinate>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_tabWidget(nullptr)
    , m_weatherWidget(nullptr)
    , m_radarWidget(nullptr)
    , m_windWidget(nullptr)
    , m_weatherService(nullptr)
    , m_locationService(nullptr)
    , m_flightConditions(nullptr)
    , m_locationLabel(nullptr)
    , m_timeLabel(nullptr)
    , m_connectionLabel(nullptr)
    , m_updateTimer(nullptr)
    , m_timeTimer(nullptr)
    , m_airportPresetWidget(nullptr)
{
    setWindowTitle("DroneView - Flight Operations Dashboard");
    setMinimumSize(1600, 1000);
    resize(1800, 1100);
    
    m_weatherService = new WeatherService(this);
    m_locationService = new LocationService(this);
    m_flightConditions = new FlightConditions(this);
    
    setupUI();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    setupStyling();
    
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &MainWindow::refreshWeatherData);
    m_updateTimer->start(300000); // Update every 5 minutes
    
    m_timeTimer = new QTimer(this);
    connect(m_timeTimer, &QTimer::timeout, [this]() {
        m_timeLabel->setText(QDateTime::currentDateTime().toString("hh:mm:ss UTC"));
    });
    m_timeTimer->start(1000);
    
    refreshWeatherData();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    
    createTabbedInterface();
}



void MainWindow::setupMenuBar()
{
    auto *fileMenu = menuBar()->addMenu("&File");
    
    m_refreshAction = new QAction("&Refresh Data", this);
    m_refreshAction->setShortcut(QKeySequence::Refresh);
    connect(m_refreshAction, &QAction::triggered, this, &MainWindow::refreshWeatherData);
    fileMenu->addAction(m_refreshAction);
    
    fileMenu->addSeparator();
    
    m_settingsAction = new QAction("&Settings...", this);
    m_settingsAction->setShortcut(QKeySequence::Preferences);
    connect(m_settingsAction, &QAction::triggered, this, &MainWindow::showSettings);
    fileMenu->addAction(m_settingsAction);
    
    fileMenu->addSeparator();
    
    m_exitAction = new QAction("E&xit", this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    connect(m_exitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(m_exitAction);
    
    auto *viewMenu = menuBar()->addMenu("&View");
    
    m_fullScreenAction = new QAction("&Full Screen", this);
    m_fullScreenAction->setShortcut(QKeySequence::FullScreen);
    m_fullScreenAction->setCheckable(true);
    connect(m_fullScreenAction, &QAction::triggered, this, &MainWindow::toggleFullScreen);
    viewMenu->addAction(m_fullScreenAction);
    
    auto *helpMenu = menuBar()->addMenu("&Help");
    
    auto *aboutAction = new QAction("&About", this);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
    helpMenu->addAction(aboutAction);
}

void MainWindow::setupToolBar()
{
    auto *toolbar = addToolBar("Main");
    toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    
    toolbar->addAction(m_refreshAction);
    toolbar->addSeparator();
    
    m_airportPresetWidget = new AirportPresetWidget(this);
    connect(m_airportPresetWidget, &AirportPresetWidget::airportSelected,
            this, &MainWindow::onAirportChanged);
    toolbar->addWidget(m_airportPresetWidget);
    toolbar->addSeparator();
}

void MainWindow::setupStatusBar()
{
    m_locationLabel = new QLabel("Location: Unknown");
    m_timeLabel = new QLabel(QDateTime::currentDateTime().toString("hh:mm:ss UTC"));
    m_connectionLabel = new QLabel("Status: Disconnected");
    
    statusBar()->addWidget(m_locationLabel);
    statusBar()->addPermanentWidget(m_connectionLabel);
    statusBar()->addPermanentWidget(m_timeLabel);
    
    connect(m_locationService, &LocationService::locationUpdated,
            this, &MainWindow::updateLocation);
}

void MainWindow::updateLocation()
{
    auto position = m_locationService->currentPosition();
    m_locationLabel->setText(QString("Location: %1, %2")
                            .arg(position.latitude(), 0, 'f', 4)
                            .arg(position.longitude(), 0, 'f', 4));
}

void MainWindow::refreshWeatherData()
{
    m_connectionLabel->setText("Status: Updating...");
    
    auto position = m_locationService->currentPosition();
    if (position.isValid()) {
        m_weatherService->fetchWeatherData(position.latitude(), position.longitude());
    } else {
        m_weatherService->fetchWeatherData(37.7749, -122.4194); // Default to San Francisco
    }
    
    m_connectionLabel->setText("Status: Connected");
}
void MainWindow::showAbout()
{
    AboutDialog dialog(this);
    dialog.exec();
}

void MainWindow::createTabbedInterface()
{
    auto *mainLayout = new QVBoxLayout(m_centralWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setTabPosition(QTabWidget::North);
    
    // WEATHER AND FLIGHT CONDITIONS TAB
    auto *weatherTab = new QWidget();
    auto *weatherLayout = new QHBoxLayout(weatherTab);
    weatherLayout->setSpacing(15);
    weatherLayout->setContentsMargins(15, 15, 15, 15);
    
    m_weatherWidget = new WeatherWidget(this);
    m_windWidget = new WindWidget(this);
    
    weatherLayout->addWidget(m_weatherWidget, 1);
    weatherLayout->addWidget(m_windWidget, 1);
    
    m_tabWidget->addTab(weatherTab, "Weather & Flight Conditions");
    
    // WEATHER RADAR TAB
    auto *radarTab = new QWidget();
    auto *radarLayout = new QVBoxLayout(radarTab);
    radarLayout->setSpacing(10);
    radarLayout->setContentsMargins(15, 15, 15, 15);
    
    m_radarWidget = new RadarWidget(this);
    radarLayout->addWidget(m_radarWidget);
    
    m_tabWidget->addTab(radarTab, "Weather Radar");
    

    
    mainLayout->addWidget(m_tabWidget);
    
    // Connect signals
    connect(m_weatherService, &WeatherService::weatherDataUpdated,
            m_weatherWidget, &WeatherWidget::updateWeatherData);
    connect(m_weatherService, &WeatherService::weatherDataUpdated,
            m_windWidget, &WindWidget::updateWindData);
    connect(m_weatherService, &WeatherService::weatherDataUpdated,
            m_flightConditions, &FlightConditions::assessConditions);
    connect(m_flightConditions, &FlightConditions::assessmentUpdated,
            m_weatherWidget, &WeatherWidget::updateFlightConditions);
    connect(m_locationService, &LocationService::locationUpdated,
            [this](const QGeoCoordinate &coord) {
                m_radarWidget->updateLocation(coord.latitude(), coord.longitude());
            });
}

void MainWindow::setupStyling()
{
    setStyleSheet(R"(
        QMainWindow {
            background-color: #2b2b2b;
            color: #ffffff;
        }
        
        QTabWidget {
            background-color: #2b2b2b;
        }
        
        QTabWidget::pane {
            border: 1px solid #555555;
            background-color: #2b2b2b;
        }
        
        QTabWidget::tab-bar {
            alignment: center;
        }
        
        QTabBar::tab {
            background-color: #3a3a3a;
            color: #ffffff;
            padding: 8px 20px;
            margin-right: 2px;
            border: 1px solid #555555;
            border-bottom: none;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
            min-width: 120px;
        }
        
        QTabBar::tab:selected {
            background-color: #2b2b2b;
            border-bottom: 2px solid #0066cc;
        }
        
        QTabBar::tab:hover {
            background-color: #454545;
        }
        
        QGroupBox {
            font-weight: bold;
            font-size: 12px;
            border: 2px solid #555555;
            border-radius: 8px;
            margin-top: 1ex;
            padding-top: 8px;
            background-color: #3a3a3a;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
            color: #ffffff;
        }
        
        QLabel {
            color: #ffffff;
            padding: 2px;
        }
        
        QProgressBar {
            border: 2px solid #555555;
            border-radius: 5px;
            text-align: center;
            background-color: #2b2b2b;
        }
        
        QProgressBar::chunk {
            background-color: #4CAF50;
            border-radius: 3px;
        }
        
        QListWidget {
            background-color: #2b2b2b;
            border: 1px solid #555555;
            border-radius: 4px;
            color: #ffffff;
            selection-background-color: #555555;
        }
        
        QListWidget::item {
            padding: 4px;
            border-bottom: 1px solid #444444;
        }
        
        QListWidget::item:selected {
            background-color: #555555;
        }
        
        QSplitter::handle {
            background-color: #555555;
        }
        
        QSplitter::handle:horizontal {
            width: 3px;
        }
        
        QSplitter::handle:vertical {
            height: 3px;
        }
        
        QStatusBar {
            background-color: #333333;
            border-top: 1px solid #555555;
        }
        
        QMenuBar {
            background-color: #333333;
            color: #ffffff;
        }
        
        QMenuBar::item {
            background-color: transparent;
            padding: 4px 8px;
        }
        
        QMenuBar::item:selected {
            background-color: #555555;
        }
        
        QToolBar {
            background-color: #333333;
            border: none;
            spacing: 3px;
        }
        
        QPushButton {
            background-color: #3a3a3a;
            color: #ffffff;
            border: 1px solid #555555;
            border-radius: 4px;
            padding: 6px 12px;
            font-weight: bold;
        }
        
        QPushButton:hover {
            background-color: #454545;
        }
        
        QPushButton:pressed {
            background-color: #555555;
        }
        
        QComboBox {
            background-color: #3a3a3a;
            color: #ffffff;
            border: 1px solid #555555;
            border-radius: 4px;
            padding: 4px 8px;
        }
        
        QComboBox::drop-down {
            border: none;
        }
        
        QComboBox::down-arrow {
            width: 12px;
            height: 12px;
        }
        
        QLineEdit, QSpinBox, QDoubleSpinBox, QDateEdit, QTimeEdit {
            background-color: #3a3a3a;
            color: #ffffff;
            border: 1px solid #555555;
            border-radius: 4px;
            padding: 4px;
        }
        
        QSlider::groove:horizontal {
            border: 1px solid #555555;
            height: 6px;
            background-color: #3a3a3a;
            border-radius: 3px;
        }
        
        QSlider::handle:horizontal {
            background-color: #0066cc;
            border: 1px solid #555555;
            width: 16px;
            height: 16px;
            border-radius: 8px;
            margin: -5px 0;
        }
    )");
}

void MainWindow::toggleFullScreen()
{
    if (isFullScreen()) {
        showNormal();
        m_fullScreenAction->setChecked(false);
    } else {
        showFullScreen();
        m_fullScreenAction->setChecked(true);
    }
}

void MainWindow::showSettings()
{
    // Create a new dialog each time to avoid reuse issues
    SettingsDialog dialog(this);
    connect(&dialog, &SettingsDialog::airportChanged,
            this, &MainWindow::onAirportChanged);
    
    dialog.exec();
}

void MainWindow::onAirportChanged(const QString &icaoCode)
{
    if (m_weatherService) {
        m_weatherService->setPreferredAirport(icaoCode);
    }
    if (m_airportPresetWidget) {
        m_airportPresetWidget->refreshPresets();
    }
    refreshWeatherData();
}