#ifndef DIAGNOSISTAB_H
#define DIAGNOSISTAB_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>

class ShellRunner;

class DiagnosisTab : public QWidget
{
    Q_OBJECT

public:
    explicit DiagnosisTab(QWidget *parent = nullptr);

signals:
    void logOutput(const QString &text);
    void logError(const QString &text);
    void diagnosisComplete(int passCount, int failCount, int warnCount);

private slots:
    void runDiagnosis();
    void onStdout(const QString &text);
    void onStderr(const QString &text);
    void onFinished(int exitCode);

private:
    void setupUi();
    void parseResults(const QString &output);

    QTableWidget *m_resultTable;
    QPushButton *m_runBtn;
    QLabel *m_summaryLabel;
    QProgressBar *m_progress;
    ShellRunner *m_runner;
    QString m_outputBuffer;
    int m_passCount;
    int m_failCount;
    int m_warnCount;
};
#endif
