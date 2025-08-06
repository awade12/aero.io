#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);

private slots:
    void onOkClicked();

private:
    void setupUI();
    void setupStyling();
    
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_contentLayout;
    QVBoxLayout *m_textLayout;
    QLabel *m_iconLabel;
    QLabel *m_titleLabel;
    QLabel *m_versionLabel;
    QLabel *m_descriptionLabel;
    QLabel *m_featuresLabel;
    QLabel *m_creditsLabel;
    QPushButton *m_okButton;
};

#endif // ABOUTDIALOG_H