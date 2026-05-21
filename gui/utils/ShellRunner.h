#ifndef SHELLRUNNER_H
#define SHELLRUNNER_H

#include <QObject>
#include <QProcess>

class ShellRunner : public QObject
{
    Q_OBJECT

public:
    explicit ShellRunner(QObject *parent = nullptr);
    static QString findShell();

    int execute(const QString &command, QString *stdoutOutput = nullptr, QString *stderrOutput = nullptr, int timeoutMs = 30000);
    void start(const QString &command);
    void startScript(const QString &scriptPath, const QStringList &args = QStringList());
    void kill();
    bool isRunning() const;
    QString workingDirectory() const;
    void setWorkingDirectory(const QString &dir);

signals:
    void outputReady(const QString &text);
    void errorReady(const QString &text);
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void started();

private slots:
    void onReadyReadStdout();
    void onReadyReadStderr();
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QProcess m_process;
    QString m_shellPath;
    QString m_workingDir;
    void setupProcess();
};
#endif
