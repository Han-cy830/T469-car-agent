#ifndef DASHBOARDTAB_H
#define DASHBOARDTAB_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>

class DashboardTab : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardTab(QWidget *parent = nullptr);

private:
    void setupUi();
    QWidget *createInfoCard(const QString &title, const QString &icon, QLabel **valueLabel);

    QLabel *m_toolchainLabel;
    QLabel *m_qtLabel;
    QLabel *m_alsaLabel;
    QLabel *m_deployLabel;
};

#endif // DASHBOARDTAB_H
