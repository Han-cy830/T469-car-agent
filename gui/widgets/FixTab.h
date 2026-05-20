#ifndef FIXTAB_H
#define FIXTAB_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>

class ShellRunner;

class FixTab : public QWidget
{
    Q_OBJECT

public:
    explicit FixTab(QWidget *parent = nullptr);

signals:
    void logOutput(const QString &text);
    void logError(const QString &text);

private slots:
    void runFixAll();
    void onFinished(int exitCode);

private:
    void setupUi();

    QTableWidget *m_fixTable;
    QPushButton *m_fixAllBtn;
    QLabel *m_statusLabel;
    ShellRunner *m_runner;
};

#endif // FIXTAB_H
