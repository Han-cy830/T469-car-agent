#ifndef SETTINGSTAB_H
#define SETTINGSTAB_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class SettingsTab : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsTab(QWidget *parent = nullptr);

private slots:
    void saveSettings();
    void detectToolchain();
    void browseProjectPath();
    void browseToolchainPath();
    void browseQtPath();

private:
    void setupUi();
    void loadSettings();

    QLineEdit *m_projectPathEdit;
    QLineEdit *m_toolchainPrefixEdit;
    QLineEdit *m_toolchainPathEdit;
    QLineEdit *m_sysrootEdit;
    QLineEdit *m_qtPathEdit;
    QLineEdit *m_shellPathEdit;
    QLineEdit *m_diagnoseScriptEdit;
    QLineEdit *m_fixScriptEdit;
    QLineEdit *m_buildScriptEdit;
    QPushButton *m_saveBtn;
    QLabel *m_statusLabel;
};

#endif // SETTINGSTAB_H
