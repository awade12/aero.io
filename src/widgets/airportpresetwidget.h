#pragma once

#include <QWidget>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSettings>

struct AirportPreset;

class AirportPresetWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AirportPresetWidget(QWidget *parent = nullptr);
    
    void refreshPresets();
    QString getCurrentAirport() const;

signals:
    void airportSelected(const QString &icaoCode);

private slots:
    void onPresetChanged();
    void onConfigureClicked();

private:
    void loadPresets();
    
    QHBoxLayout *m_layout;
    QLabel *m_label;
    QComboBox *m_presetCombo;
    QPushButton *m_configureBtn;
    
    QSettings *m_settings;
};