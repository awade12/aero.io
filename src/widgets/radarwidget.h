#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QWebEngineView>
#include <QGroupBox>
#include <QPushButton>
#include <QComboBox>
#include <QSlider>

class RadarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RadarWidget(QWidget *parent = nullptr);

public slots:
    void updateLocation(double latitude, double longitude);
    void refreshRadarData();

private slots:
    void onLayerChanged();
    void onZoomChanged();
    void onTimeChanged();

private:
    void setupUI();
    void loadRadarMap();
    QString buildRadarUrl() const;
    
    QVBoxLayout *m_mainLayout;
    QGroupBox *m_radarGroup;
    QWebEngineView *m_webView;
    
    QHBoxLayout *m_controlsLayout;
    QComboBox *m_layerComboBox;
    QSlider *m_zoomSlider;
    QSlider *m_timeSlider;
    QLabel *m_zoomLabel;
    QLabel *m_timeLabel;
    QPushButton *m_refreshButton;
    QPushButton *m_animateButton;
    
    double m_latitude;
    double m_longitude;
    int m_zoomLevel;
    QString m_currentLayer;
    bool m_isAnimating;
};