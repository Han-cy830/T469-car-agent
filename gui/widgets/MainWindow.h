#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QSplitter>

class SidebarWidget;
class LogPanel;
class DashboardTab;
class DiagnosisTab;
class FixTab;
class BuildTab;
class DeployTab;
class SettingsTab;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void switchPage(int index);

private:
    void setupUi();

    SidebarWidget *m_sidebar;
    QStackedWidget *m_stack;
    LogPanel *m_logPanel;

    DashboardTab *m_dashboardTab;
    DiagnosisTab *m_diagnosisTab;
    FixTab *m_fixTab;
    BuildTab *m_buildTab;
    DeployTab *m_deployTab;
    SettingsTab *m_settingsTab;
};

#endif // MAINWINDOW_H
