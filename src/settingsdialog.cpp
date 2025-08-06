#include "settingsdialog.h"
#include <QTimer>
#include <QHeaderView>
#include <QMessageBox>
#include <QSplitter>
#include <QDebug>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , m_settings(new QSettings("DroneView", "Settings", this))
{
    setWindowTitle("DroneView Settings");
    setModal(true);
    resize(600, 500);
    
    setupUI();
    setupPredefinedAirports();
    loadSettings();
}

void SettingsDialog::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    
    QWidget *leftPanel = new QWidget;
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    
    m_airportGroup = new QGroupBox("Airport Selection");
    QVBoxLayout *airportLayout = new QVBoxLayout(m_airportGroup);
    
    QLabel *predefinedLabel = new QLabel("Select from major airports:");
    m_predefinedCombo = new QComboBox;
    m_predefinedCombo->setMaximumWidth(300);
    
    QLabel *customLabel = new QLabel("Or enter ICAO code manually:");
    m_customCodeEdit = new QLineEdit;
    m_customCodeEdit->setPlaceholderText("e.g., KDFW, KJFK, KLAX");
    m_customCodeEdit->setMaxLength(4);
    m_customCodeEdit->setMaximumWidth(100);
    
    airportLayout->addWidget(predefinedLabel);
    airportLayout->addWidget(m_predefinedCombo);
    airportLayout->addWidget(customLabel);
    airportLayout->addWidget(m_customCodeEdit);
    airportLayout->addStretch();
    
    leftLayout->addWidget(m_airportGroup);
    leftLayout->addStretch();
    
    QWidget *rightPanel = new QWidget;
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    
    m_presetsGroup = new QGroupBox("Airport Presets");
    QVBoxLayout *presetsLayout = new QVBoxLayout(m_presetsGroup);
    
    m_usePresetMode = new QCheckBox("Enable quick preset switching");
    m_usePresetMode->setChecked(true);
    
    m_presetsList = new QListWidget;
    m_presetsList->setSelectionMode(QAbstractItemView::SingleSelection);
    
    QWidget *addPresetWidget = new QWidget;
    QGridLayout *addLayout = new QGridLayout(addPresetWidget);
    addLayout->addWidget(new QLabel("Name:"), 0, 0);
    m_presetNameEdit = new QLineEdit;
    m_presetNameEdit->setPlaceholderText("My Airport");
    addLayout->addWidget(m_presetNameEdit, 0, 1);
    
    addLayout->addWidget(new QLabel("ICAO:"), 1, 0);
    m_presetCodeEdit = new QLineEdit;
    m_presetCodeEdit->setPlaceholderText("KDFW");
    m_presetCodeEdit->setMaxLength(4);
    addLayout->addWidget(m_presetCodeEdit, 1, 1);
    
    addLayout->addWidget(new QLabel("Description:"), 2, 0);
    m_presetDescEdit = new QLineEdit;
    m_presetDescEdit->setPlaceholderText("Dallas/Fort Worth");
    addLayout->addWidget(m_presetDescEdit, 2, 1);
    
    QHBoxLayout *presetButtonsLayout = new QHBoxLayout;
    m_addPresetBtn = new QPushButton("Add Preset");
    m_removePresetBtn = new QPushButton("Remove Selected");
    m_removePresetBtn->setEnabled(false);
    
    presetButtonsLayout->addWidget(m_addPresetBtn);
    presetButtonsLayout->addWidget(m_removePresetBtn);
    presetButtonsLayout->addStretch();
    
    presetsLayout->addWidget(m_usePresetMode);
    presetsLayout->addWidget(new QLabel("Quick Access Presets:"));
    presetsLayout->addWidget(m_presetsList);
    presetsLayout->addWidget(addPresetWidget);
    presetsLayout->addLayout(presetButtonsLayout);
    
    rightLayout->addWidget(m_presetsGroup);
    
    splitter->addWidget(leftPanel);
    splitter->addWidget(rightPanel);
    splitter->setSizes({250, 350});
    
    m_mainLayout->addWidget(splitter);
    
    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    m_mainLayout->addWidget(m_buttonBox);
    
    connect(m_predefinedCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::updateSelectedAirport);
    connect(m_customCodeEdit, &QLineEdit::textChanged,
            this, &SettingsDialog::onCustomCodeChanged);
    connect(m_presetsList, &QListWidget::itemSelectionChanged,
            this, &SettingsDialog::onPresetSelectionChanged);
    connect(m_addPresetBtn, &QPushButton::clicked,
            this, &SettingsDialog::onAddPreset);
    connect(m_removePresetBtn, &QPushButton::clicked,
            this, &SettingsDialog::onRemovePreset);
    connect(m_usePresetMode, &QCheckBox::toggled,
            this, &SettingsDialog::onPresetModeChanged);
    connect(m_buttonBox, &QDialogButtonBox::accepted,
            this, &SettingsDialog::saveSettings);
    connect(m_buttonBox, &QDialogButtonBox::accepted,
            this, &QDialog::accept);
    connect(m_buttonBox, &QDialogButtonBox::rejected,
            this, &QDialog::reject);
}

void SettingsDialog::setupPredefinedAirports()
{
    m_predefinedAirports = {
        {"KATL", "Atlanta Hartsfield-Jackson (KATL)"},
        {"KBOS", "Boston Logan (KBOS)"},
        {"KORD", "Chicago O'Hare (KORD)"},
        {"KDFW", "Dallas/Fort Worth (KDFW)"},
        {"KDEN", "Denver International (KDEN)"},
        {"KDTW", "Detroit Metro (KDTW)"},
        {"KIAH", "Houston Bush (KIAH)"},
        {"KJFK", "New York JFK (KJFK)"},
        {"KLAS", "Las Vegas McCarran (KLAS)"},
        {"KLAX", "Los Angeles LAX (KLAX)"},
        {"KMIA", "Miami International (KMIA)"},
        {"KMSP", "Minneapolis-St. Paul (KMSP)"},
        {"KPHL", "Philadelphia (KPHL)"},
        {"KPHX", "Phoenix Sky Harbor (KPHX)"},
        {"KSEA", "Seattle-Tacoma (KSEA)"},
        {"KSFO", "San Francisco (KSFO)"}
    };
    
    for (auto it = m_predefinedAirports.begin(); it != m_predefinedAirports.end(); ++it) {
        m_predefinedCombo->addItem(it.value(), it.key());
    }
    
    int dallasIndex = m_predefinedCombo->findData("KDFW");
    if (dallasIndex >= 0) {
        m_predefinedCombo->setCurrentIndex(dallasIndex);
    }
}

void SettingsDialog::onPresetSelectionChanged()
{
    bool hasSelection = !m_presetsList->selectedItems().isEmpty();
    m_removePresetBtn->setEnabled(hasSelection);
    
    if (hasSelection) {
        QListWidgetItem *item = m_presetsList->currentItem();
        if (item) {
            QString icaoCode = item->data(Qt::UserRole).toString();
            m_customCodeEdit->setText(icaoCode);
            
            int comboIndex = m_predefinedCombo->findData(icaoCode);
            if (comboIndex >= 0) {
                m_predefinedCombo->setCurrentIndex(comboIndex);
                m_customCodeEdit->clear();
            }
        }
    }
}

void SettingsDialog::onAddPreset()
{
    QString name = m_presetNameEdit->text().trimmed();
    QString code = m_presetCodeEdit->text().trimmed().toUpper();
    QString desc = m_presetDescEdit->text().trimmed();
    
    if (name.isEmpty() || code.isEmpty()) {
        QMessageBox::warning(this, "Invalid Preset", 
                           "Please enter both a name and ICAO code for the preset.");
        return;
    }
    
    if (code.length() != 4) {
        QMessageBox::warning(this, "Invalid ICAO Code", 
                           "ICAO codes must be exactly 4 characters.");
        return;
    }
    
    for (const auto &preset : m_presets) {
        if (preset.icaoCode == code) {
            QMessageBox::warning(this, "Duplicate Preset", 
                               "A preset for this airport already exists.");
            return;
        }
    }
    
    AirportPreset preset(name, code, desc);
    m_presets.append(preset);
    addPresetToList(preset);
    
    m_presetNameEdit->clear();
    m_presetCodeEdit->clear();
    m_presetDescEdit->clear();
}

void SettingsDialog::onRemovePreset()
{
    QListWidgetItem *item = m_presetsList->currentItem();
    if (!item) return;
    
    QString icaoCode = item->data(Qt::UserRole).toString();
    
    m_presets.removeIf([icaoCode](const AirportPreset &preset) {
        return preset.icaoCode == icaoCode;
    });
    
    delete item;
    m_removePresetBtn->setEnabled(false);
}

void SettingsDialog::onCustomCodeChanged()
{
    QString code = m_customCodeEdit->text().trimmed().toUpper();
    m_customCodeEdit->setText(code);
    
    if (!code.isEmpty()) {
        m_predefinedCombo->setCurrentIndex(-1);
    }
    
    updateSelectedAirport();
}

void SettingsDialog::onPresetModeChanged()
{
    bool enabled = m_usePresetMode->isChecked();
    m_presetsList->setEnabled(enabled);
    m_addPresetBtn->setEnabled(enabled);
    m_removePresetBtn->setEnabled(enabled && !m_presetsList->selectedItems().isEmpty());
    m_presetNameEdit->setEnabled(enabled);
    m_presetCodeEdit->setEnabled(enabled);
    m_presetDescEdit->setEnabled(enabled);
}

void SettingsDialog::updateSelectedAirport()
{
    QString airport;
    
    if (!m_customCodeEdit->text().isEmpty()) {
        airport = m_customCodeEdit->text().trimmed().toUpper();
    } else if (m_predefinedCombo->currentIndex() >= 0) {
        airport = m_predefinedCombo->currentData().toString();
    }
    
    m_currentAirport = airport;
}

void SettingsDialog::addPresetToList(const AirportPreset &preset)
{
    QListWidgetItem *item = new QListWidgetItem(m_presetsList);
    item->setText(QString("%1 (%2)").arg(preset.name, preset.icaoCode));
    item->setData(Qt::UserRole, preset.icaoCode);
    item->setToolTip(preset.description);
}

void SettingsDialog::saveSettings()
{
    m_settings->setValue("airport/current", m_currentAirport);
    m_settings->setValue("airport/usePresets", m_usePresetMode->isChecked());
    
    m_settings->beginWriteArray("airport/presets");
    for (int i = 0; i < m_presets.size(); ++i) {
        m_settings->setArrayIndex(i);
        m_settings->setValue("name", m_presets[i].name);
        m_settings->setValue("icaoCode", m_presets[i].icaoCode);
        m_settings->setValue("description", m_presets[i].description);
    }
    m_settings->endArray();
    
    // Delay signal emission to avoid segfault during dialog destruction
    QTimer::singleShot(0, this, [this]() {
        emit airportChanged(m_currentAirport);
    });
}

void SettingsDialog::loadSettings()
{
    QString savedAirport = m_settings->value("airport/current", "KDFW").toString();
    bool usePresets = m_settings->value("airport/usePresets", true).toBool();
    
    m_usePresetMode->setChecked(usePresets);
    onPresetModeChanged();
    
    int size = m_settings->beginReadArray("airport/presets");
    for (int i = 0; i < size; ++i) {
        m_settings->setArrayIndex(i);
        AirportPreset preset;
        preset.name = m_settings->value("name").toString();
        preset.icaoCode = m_settings->value("icaoCode").toString();
        preset.description = m_settings->value("description").toString();
        
        if (!preset.name.isEmpty() && !preset.icaoCode.isEmpty()) {
            m_presets.append(preset);
            addPresetToList(preset);
        }
    }
    m_settings->endArray();
    
    int comboIndex = m_predefinedCombo->findData(savedAirport);
    if (comboIndex >= 0) {
        m_predefinedCombo->setCurrentIndex(comboIndex);
    } else {
        m_customCodeEdit->setText(savedAirport);
    }
    
    m_currentAirport = savedAirport;
}

QString SettingsDialog::getSelectedAirport() const
{
    return m_currentAirport.isEmpty() ? "KDFW" : m_currentAirport;
}

QList<AirportPreset> SettingsDialog::getAirportPresets() const
{
    return m_presets;
}