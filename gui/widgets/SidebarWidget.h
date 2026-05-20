#ifndef SIDEBARWIDGET_H
#define SIDEBARWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QButtonGroup>

class SidebarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SidebarWidget(QWidget *parent = nullptr);

signals:
    void pageChanged(int index);

private:
    QButtonGroup *m_buttonGroup;
    QPushButton *createNavButton(const QString &text, int index);
};

#endif // SIDEBARWIDGET_H
