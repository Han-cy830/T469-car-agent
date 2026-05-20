#ifndef BUILDTAB_H
#define BUILDTAB_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QComboBox>

class ShellRunner;

class BuildTab : public QWidget
{
    Q_OBJECT

public:
    explicit BuildTab(QWidget *parent = nullptr);

signals:
    void logOutput(const QString &text);
    void logError(const QString &text);

private slots:
    void startBuild();
    void onStdout(const QString &text);
    void onStderr(const QString &text);
    void onFinished(int exitCode);

private:
    void setupUi();
    void updateTargetStatus(int index, const QString &status, const QString &color);

    QTableWidget *m_targetTable;
    QPushButton *m_buildBtn;
    QComboBox *m_targetCombo;
    QProgressBar *m_progress;
    QLabel *m_statusLabel;
    ShellRunner *m_runner;
    QString m_outputBuffer;
};

#endif // BUILDTAB_H
