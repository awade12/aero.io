#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QSettings>

struct AirportPreset {
    QString name;
    QString icaoCode;
    QString description;
    
    AirportPreset() = default;
    AirportPreset(const QString &n, const QString &code, const QString &desc)
        : name(n), icaoCode(code), description(desc) {}
};

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    
    QString getSelectedAirport() const;
    QList<AirportPreset> getAirportPresets() const;
    
signals:
    void airportChanged(const QString &icaoCode);

private slots:
    void onPresetSelectionChanged();
    void onAddPreset();
    void onRemovePreset();
    void onCustomCodeChanged();
    void onPresetModeChanged();
    void saveSettings();
    void loadSettings();

private:
    void setupUI();
    void setupPredefinedAirports();
    void addPresetToList(const AirportPreset &preset);
    void updateSelectedAirport();
    
    QVBoxLayout *m_mainLayout;
    QGroupBox *m_airportGroup;
    QGroupBox *m_presetsGroup;
    
    QComboBox *m_predefinedCombo;
    QLineEdit *m_customCodeEdit;
    QCheckBox *m_usePresetMode;
    
    QListWidget *m_presetsList;
    QPushButton *m_addPresetBtn;
    QPushButton *m_removePresetBtn;
    
    QLineEdit *m_presetNameEdit;
    QLineEdit *m_presetCodeEdit;
    QLineEdit *m_presetDescEdit;
    
    QDialogButtonBox *m_buttonBox;
    
    QSettings *m_settings;
    QList<AirportPreset> m_presets;
    QString m_currentAirport;
    
    QMap<QString, QString> m_predefinedAirports;
};