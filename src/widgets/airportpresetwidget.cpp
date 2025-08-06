#include "airportpresetwidget.h"
#include "../settingsdialog.h"
#include <QSettings>
#include <QDialog>

AirportPresetWidget::AirportPresetWidget(QWidget *parent)
    : QWidget(parent)
    , m_settings(new QSettings("DroneView", "Settings", this))
{
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    m_label = new QLabel("Airport:", this);
    m_label->setMinimumWidth(60);
    
    m_presetCombo = new QComboBox(this);
    m_presetCombo->setMinimumWidth(200);
    m_presetCombo->setToolTip("Select an airport preset for weather data");
    
    m_configureBtn = new QPushButton("Configure...", this);
    m_configureBtn->setMaximumWidth(100);
    m_configureBtn->setToolTip("Open airport settings");
    
    m_layout->addWidget(m_label);
    m_layout->addWidget(m_presetCombo);
    m_layout->addWidget(m_configureBtn);
    m_layout->addStretch();
    
    connect(m_presetCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AirportPresetWidget::onPresetChanged);
    connect(m_configureBtn, &QPushButton::clicked,
            this, &AirportPresetWidget::onConfigureClicked);
    
    loadPresets();
}

void AirportPresetWidget::refreshPresets()
{
    // Block signals to prevent infinite loop during refresh
    bool oldState = m_presetCombo->blockSignals(true);
    loadPresets();
    m_presetCombo->blockSignals(oldState);
}

void AirportPresetWidget::loadPresets()
{
    m_presetCombo->clear();
    
    QString currentAirport = m_settings->value("airport/current", "KDFW").toString();
    bool usePresets = m_settings->value("airport/usePresets", true).toBool();
    
    if (usePresets) {
        int size = m_settings->beginReadArray("airport/presets");
        bool foundCurrent = false;
        
        for (int i = 0; i < size; ++i) {
            m_settings->setArrayIndex(i);
            QString name = m_settings->value("name").toString();
            QString icaoCode = m_settings->value("icaoCode").toString();
            QString description = m_settings->value("description").toString();
            
            if (!name.isEmpty() && !icaoCode.isEmpty()) {
                QString displayText = QString("%1 (%2)").arg(name, icaoCode);
                if (!description.isEmpty()) {
                    displayText += QString(" - %1").arg(description);
                }
                
                m_presetCombo->addItem(displayText, icaoCode);
                
                if (icaoCode == currentAirport) {
                    m_presetCombo->setCurrentIndex(m_presetCombo->count() - 1);
                    foundCurrent = true;
                }
            }
        }
        m_settings->endArray();
        
        if (!foundCurrent && !currentAirport.isEmpty()) {
            QString displayText = QString("Current: %1").arg(currentAirport);
            m_presetCombo->addItem(displayText, currentAirport);
            m_presetCombo->setCurrentIndex(m_presetCombo->count() - 1);
        }
    } else {
        m_presetCombo->addItem(QString("Current: %1").arg(currentAirport), currentAirport);
        m_presetCombo->setCurrentIndex(0);
    }
    
    if (m_presetCombo->count() == 0) {
        m_presetCombo->addItem("Default: KDFW (Dallas/Fort Worth)", "KDFW");
        m_presetCombo->setCurrentIndex(0);
    }
}

QString AirportPresetWidget::getCurrentAirport() const
{
    if (m_presetCombo->currentIndex() >= 0) {
        return m_presetCombo->currentData().toString();
    }
    return "KDFW";
}

void AirportPresetWidget::onPresetChanged()
{
    QString icaoCode = getCurrentAirport();
    if (!icaoCode.isEmpty()) {
        emit airportSelected(icaoCode);
    }
}

void AirportPresetWidget::onConfigureClicked()
{
    SettingsDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        refreshPresets();
        QString newAirport = getCurrentAirport();
        emit airportSelected(newAirport);
    }
}