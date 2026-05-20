#ifndef SHELLRUNNER_H
#define SHELLRUNNER_H

#include <QObject>
#include <QProcess>
#include <QStringList>

class ShellRunner : public QObject
{
    Q_OBJECT

public:
    explicit ShellRunner(QObject *parent = nullptr);

    // 查找可用的 shell 解释器
    static QString findShell();

    // 同步执行命令，返回退出码
    int execute(const QString &command, QString *stdoutOutput = nullptr, QString *stderrOutput = nullptr, int timeoutMs = 30000);

    // 异步执行命令
    void start(const QString &command);

    // 执行脚本文件
    void startScript(const QString &scriptPath, const QStringList &args = QStringList());

    // 停止当前进程
    void kill();

    // 是否正在运行
    bool isRunning() const;

    // 获取当前工作目录
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

#endif // SHELLRUNNER_H
