#include "aboutdialog.h"
#include <QApplication>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
    , m_mainLayout(nullptr)
    , m_contentLayout(nullptr)
    , m_textLayout(nullptr)
    , m_iconLabel(nullptr)
    , m_titleLabel(nullptr)
    , m_versionLabel(nullptr)
    , m_descriptionLabel(nullptr)
    , m_featuresLabel(nullptr)
    , m_creditsLabel(nullptr)
    , m_okButton(nullptr)
{
    setWindowTitle("About droneops");
    setFixedSize(500, 400);
    setModal(true);
    
    setupUI();
    setupStyling();
}

void AboutDialog::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(20);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);
    
    m_contentLayout = new QHBoxLayout();
    m_contentLayout->setSpacing(20);
    
    m_iconLabel = new QLabel();
    m_iconLabel->setFixedSize(64, 64);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    m_iconLabel->setText("🚁");
    m_iconLabel->setStyleSheet("font-size: 48px;");
    
    m_textLayout = new QVBoxLayout();
    m_textLayout->setSpacing(10);
    
    m_titleLabel = new QLabel("droneops");
    m_titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #0066cc;");
    
    m_versionLabel = new QLabel("Version 1.0.0");
    m_versionLabel->setStyleSheet("font-size: 14px; color: #cccccc;");
    
    m_descriptionLabel = new QLabel("A weather monitoring dashboard for drone operations.");
    m_descriptionLabel->setWordWrap(true);
    m_descriptionLabel->setStyleSheet("font-size: 12px; margin-top: 10px;");
    
    m_featuresLabel = new QLabel(
        "<b>Features:</b><br>"
        "• Real-time aviation weather data (METAR/TAF)<br>"
        "• Wind analysis and compass<br>"
        "• Weather radar visualization<br>"
        "• Flight condition assessment"
    );
    m_featuresLabel->setWordWrap(true);
    m_featuresLabel->setStyleSheet("font-size: 11px; margin-top: 15px;");
    
    m_creditsLabel = new QLabel("Built with Qt and Aviation Weather API");
    m_creditsLabel->setWordWrap(true);
    m_creditsLabel->setStyleSheet("font-size: 10px; color: #888888; margin-top: 15px;");
    
    m_textLayout->addWidget(m_titleLabel);
    m_textLayout->addWidget(m_versionLabel);
    m_textLayout->addWidget(m_descriptionLabel);
    m_textLayout->addWidget(m_featuresLabel);
    m_textLayout->addWidget(m_creditsLabel);
    m_textLayout->addStretch();
    
    m_contentLayout->addWidget(m_iconLabel);
    m_contentLayout->addLayout(m_textLayout);
    
    m_okButton = new QPushButton("OK");
    m_okButton->setFixedWidth(80);
    connect(m_okButton, &QPushButton::clicked, this, &AboutDialog::onOkClicked);
    
    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_okButton);
    
    m_mainLayout->addLayout(m_contentLayout);
    m_mainLayout->addLayout(buttonLayout);
}

void AboutDialog::setupStyling()
{
    setStyleSheet(R"(
        QDialog {
            background-color: #2b2b2b;
            color: #ffffff;
        }
        
        QLabel {
            color: #ffffff;
        }
        
        QPushButton {
            background-color: #3a3a3a;
            color: #ffffff;
            border: 1px solid #555555;
            border-radius: 4px;
            padding: 8px 16px;
            font-weight: bold;
        }
        
        QPushButton:hover {
            background-color: #454545;
        }
        
        QPushButton:pressed {
            background-color: #555555;
        }
    )");
}

void AboutDialog::onOkClicked()
{
    accept();
}