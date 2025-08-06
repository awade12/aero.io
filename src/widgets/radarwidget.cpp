#include "radarwidget.h"
#include <QDebug>
#include <QSizePolicy>

RadarWidget::RadarWidget(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_radarGroup(nullptr)
    , m_webView(nullptr)
    , m_controlsLayout(nullptr)
    , m_latitude(37.7749)
    , m_longitude(-122.4194)
    , m_zoomLevel(8)
    , m_currentLayer("ridge-current")
    , m_isAnimating(false)
{
    setupUI();
    loadRadarMap();
}

void RadarWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(12);
    m_mainLayout->setContentsMargins(12, 12, 12, 12);
    
    m_radarGroup = new QGroupBox("Weather Radar", this);
    m_radarGroup->setStyleSheet(
        "QGroupBox {"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "    color: #66ccff;"
        "    border: 2px solid #333333;"
        "    border-radius: 8px;"
        "    margin-top: 8px;"
        "    padding-top: 8px;"
        "    background-color: rgba(30, 30, 30, 0.8);"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    left: 12px;"
        "    padding: 0 8px 0 8px;"
        "    background-color: transparent;"
        "}"
    );
    auto *radarLayout = new QVBoxLayout(m_radarGroup);
    radarLayout->setSpacing(10);
    radarLayout->setContentsMargins(16, 20, 16, 16);
    
    m_controlsLayout = new QHBoxLayout();
    m_controlsLayout->setSpacing(10);
    
    QString controlStyle = 
        "QComboBox, QPushButton {"
        "    background-color: rgba(50, 50, 50, 0.8);"
        "    color: #e0e0e0;"
        "    border: 1px solid #444444;"
        "    border-radius: 4px;"
        "    padding: 6px 12px;"
        "    font-size: 11px;"
        "}"
        "QComboBox:hover, QPushButton:hover {"
        "    background-color: rgba(102, 204, 255, 0.2);"
        "    border-color: #66ccff;"
        "}"
        "QComboBox::drop-down {"
        "    border: none;"
        "}"
        "QComboBox::down-arrow {"
        "    image: none;"
        "    border: none;"
        "}"
        "QPushButton:checked {"
        "    background-color: rgba(102, 204, 255, 0.3);"
        "    color: #66ccff;"
        "}";
    
    m_layerComboBox = new QComboBox(this);
    m_layerComboBox->addItem("Current Precipitation", "ridge-current");
    m_layerComboBox->addItem("MRMS 1-Hour Precip", "mrms-p1h");
    m_layerComboBox->addItem("MRMS 24-Hour Precip", "mrms-p24h");
    m_layerComboBox->addItem("NEXRAD Base Reflectivity", "nexrd2-n0q");
    m_layerComboBox->addItem("NEXRAD Composite", "nexrd2-ncr");
    m_layerComboBox->setStyleSheet(controlStyle);
    m_layerComboBox->setMinimumWidth(160);
    connect(m_layerComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &RadarWidget::onLayerChanged);
    
    m_zoomSlider = new QSlider(Qt::Horizontal, this);
    m_zoomSlider->setRange(3, 15);
    m_zoomSlider->setValue(m_zoomLevel);
    m_zoomSlider->setMaximumWidth(120);
    m_zoomSlider->setStyleSheet(
        "QSlider::groove:horizontal {"
        "    border: 1px solid #444444;"
        "    height: 6px;"
        "    background: rgba(40, 40, 40, 0.8);"
        "    border-radius: 3px;"
        "}"
        "QSlider::handle:horizontal {"
        "    background: #66ccff;"
        "    border: 1px solid #444444;"
        "    width: 16px;"
        "    margin: -5px 0;"
        "    border-radius: 8px;"
        "}"
        "QSlider::handle:horizontal:hover {"
        "    background: #88ddff;"
        "}"
    );
    connect(m_zoomSlider, &QSlider::valueChanged, this, &RadarWidget::onZoomChanged);
    
    m_zoomLabel = new QLabel(QString("Zoom: %1").arg(m_zoomLevel), this);
    m_zoomLabel->setMinimumWidth(70);
    m_zoomLabel->setStyleSheet("color: #e0e0e0; font-size: 11px; font-weight: bold;");
    
    m_refreshButton = new QPushButton("Refresh", this);
    m_refreshButton->setStyleSheet(controlStyle);
    connect(m_refreshButton, &QPushButton::clicked, this, &RadarWidget::refreshRadarData);
    
    m_animateButton = new QPushButton("Animate", this);
    m_animateButton->setCheckable(true);
    m_animateButton->setStyleSheet(controlStyle);
    connect(m_animateButton, &QPushButton::toggled, [this](bool animate) {
        m_isAnimating = animate;
        m_animateButton->setText(animate ? "Stop" : "Animate");
        loadRadarMap();
    });
    
    auto *layerLabel = new QLabel("Layer:", this);
    layerLabel->setStyleSheet("color: #66ccff; font-weight: bold; font-size: 11px;");
    
    m_controlsLayout->addWidget(layerLabel);
    m_controlsLayout->addWidget(m_layerComboBox);
    m_controlsLayout->addWidget(m_zoomLabel);
    m_controlsLayout->addWidget(m_zoomSlider);
    m_controlsLayout->addStretch();
    m_controlsLayout->addWidget(m_refreshButton);
    m_controlsLayout->addWidget(m_animateButton);
    
    radarLayout->addLayout(m_controlsLayout);
    
    m_webView = new QWebEngineView(this);
    m_webView->setMinimumHeight(280);
    m_webView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    radarLayout->addWidget(m_webView);
    
    m_mainLayout->addWidget(m_radarGroup);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void RadarWidget::updateLocation(double latitude, double longitude)
{
    m_latitude = latitude;
    m_longitude = longitude;
    loadRadarMap();
}

void RadarWidget::refreshRadarData()
{
    loadRadarMap();
}

void RadarWidget::onLayerChanged()
{
    m_currentLayer = m_layerComboBox->currentData().toString();
    loadRadarMap();
}

void RadarWidget::onZoomChanged()
{
    m_zoomLevel = m_zoomSlider->value();
    m_zoomLabel->setText(QString("Zoom: %1").arg(m_zoomLevel));
    loadRadarMap();
}

void RadarWidget::onTimeChanged()
{
    loadRadarMap();
}

void RadarWidget::loadRadarMap()
{
    QString baseUrl;
    QString layerName = m_layerComboBox->currentText();
    
    // Set the appropriate IEM tile URL based on layer type
    if (m_currentLayer == "ridge-current") {
        baseUrl = "https://mesonet.agron.iastate.edu/cache/tile.py/1.0.0/ridge::USCOMP-N0Q-0/{z}/{x}/{y}.png";
    } else if (m_currentLayer == "mrms-p1h") {
        baseUrl = "https://mesonet.agron.iastate.edu/cache/tile.py/1.0.0/mrms::p1h-0/{z}/{x}/{y}.png";
    } else if (m_currentLayer == "mrms-p24h") {
        baseUrl = "https://mesonet.agron.iastate.edu/cache/tile.py/1.0.0/mrms::p24h-0/{z}/{x}/{y}.png";
    } else if (m_currentLayer == "nexrd2-n0q") {
        baseUrl = "https://mesonet.agron.iastate.edu/cache/tile.py/1.0.0/nexrd2-n0q-900913/{z}/{x}/{y}.png";
    } else if (m_currentLayer == "nexrd2-ncr") {
        baseUrl = "https://mesonet.agron.iastate.edu/cache/tile.py/1.0.0/nexrd2-ncr-900913/{z}/{x}/{y}.png";
    } else {
        // Fallback to ridge current
        baseUrl = "https://mesonet.agron.iastate.edu/cache/tile.py/1.0.0/ridge::USCOMP-N0Q-0/{z}/{x}/{y}.png";
    }
    
    QString html = QString(R"(
<!DOCTYPE html>
<html>
<head>
    <title>Weather Radar</title>
    <meta charset="utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet" href="https://unpkg.com/leaflet@1.7.1/dist/leaflet.css" />
    <style>
        body { margin: 0; padding: 0; background: #1a1a1a; }
        #map { height: 100vh; width: 100%%; }
        .legend {
            background: rgba(0,0,0,0.8);
            color: white;
            padding: 10px;
            border-radius: 5px;
            font-size: 12px;
            line-height: 1.4;
        }
        .leaflet-control-attribution {
            background: rgba(0,0,0,0.7) !important;
            color: white !important;
        }
        .leaflet-control-attribution a {
            color: #66ccff !important;
        }
    </style>
</head>
<body>
    <div id="map"></div>
    
    <script src="https://unpkg.com/leaflet@1.7.1/dist/leaflet.js"></script>
    <script>
        var map = L.map('map', {
            zoomControl: false
        }).setView([%1, %2], %3);
        
        // Dark tile layer
        L.tileLayer('https://{s}.basemaps.cartocdn.com/dark_all/{z}/{x}/{y}{r}.png', {
            attribution: '&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors &copy; <a href="https://carto.com/attributions">CARTO</a>',
            subdomains: 'abcd',
            maxZoom: 19
        }).addTo(map);
        
        // Add IEM NEXRAD weather radar layer
        var radarLayer = L.tileLayer('%4', {
            attribution: 'Weather data courtesy of <a href="https://mesonet.agron.iastate.edu/">Iowa Environmental Mesonet</a> & NOAA',
            opacity: 0.7,
            maxZoom: 19
        }).addTo(map);
        
        // Add location marker
        var marker = L.marker([%1, %2]).addTo(map);
        marker.bindPopup('<strong>Current Location</strong><br/>%5 View').openPopup();
        
        // Add legend based on radar type
        var legend = L.control({position: 'bottomright'});
        legend.onAdd = function(map) {
            var div = L.DomUtil.create('div', 'legend');
            var layerType = '%6';
            
            if (layerType.includes('MRMS') || layerType.includes('Precip')) {
                div.innerHTML = '<strong>%5</strong><br/>' +
                               '<span style="color: #4169E1;">■</span> Light Rain<br/>' +
                               '<span style="color: #00FF00;">■</span> Moderate Rain<br/>' +
                               '<span style="color: #FFFF00;">■</span> Heavy Rain<br/>' +
                               '<span style="color: #FF8C00;">■</span> Very Heavy<br/>' +
                               '<span style="color: #FF0000;">■</span> Extreme';
            } else if (layerType.includes('Velocity')) {
                div.innerHTML = '<strong>%5</strong><br/>' +
                               '<span style="color: #ff0000;">■</span> Away from radar<br/>' +
                               '<span style="color: #00ff00;">■</span> Toward radar<br/>' +
                               '<span style="color: #ffff00;">■</span> Turbulence';
            } else {
                div.innerHTML = '<strong>%5</strong><br/>' +
                               '<span style="color: #4169E1;">■</span> Light (0-20 dBZ)<br/>' +
                               '<span style="color: #00FF00;">■</span> Moderate (20-35 dBZ)<br/>' +
                               '<span style="color: #FFFF00;">■</span> Heavy (35-50 dBZ)<br/>' +
                               '<span style="color: #FF0000;">■</span> Severe (50+ dBZ)';
            }
            return div;
        };
        legend.addTo(map);
        
        // Auto refresh radar data every 5 minutes
        setInterval(function() {
            radarLayer.redraw();
        }, 300000);
        
        // Add timestamp display
        var timestamp = L.control({position: 'topleft'});
        timestamp.onAdd = function(map) {
            var div = L.DomUtil.create('div', 'legend');
            div.innerHTML = '<strong>Last Updated:</strong><br/>' + new Date().toLocaleTimeString();
            return div;
        };
        timestamp.addTo(map);
    </script>
</body>
</html>
    )").arg(m_latitude)
       .arg(m_longitude)
       .arg(m_zoomLevel)
       .arg(baseUrl)
       .arg(layerName)
       .arg(m_currentLayer);
    
    m_webView->setHtml(html);
}