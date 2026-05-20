#include "ShellRunner.h"
#include <QCoreApplication>
#include <QDir>
#include <QStandardPaths>

ShellRunner::ShellRunner(QObject *parent)
    : QObject(parent)
    , m_shellPath(findShell())
{
    setupProcess();
}

QString ShellRunner::findShell()
{
#ifdef Q_OS_WIN
    // Windows: 优先查找 Git Bash
    QStringList candidates = {
        qApp->applicationDirPath() + "/../git/bin/bash.exe",
        "C:/Program Files/Git/bin/bash.exe",
        "C:/Program Files (x86)/Git/bin/bash.exe",
        QStandardPaths::findExecutable("bash"),
        QStandardPaths::findExecutable("sh")
    };
#else
    // Linux/macOS
    QStringList candidates = {
        "/bin/bash",
        "/usr/bin/bash",
        QStandardPaths::findExecutable("bash"),
        QStandardPaths::findExecutable("sh")
    };
#endif

    for (const QString &path : candidates) {
        if (QFile::exists(path))
            return path;
    }
    return QString();
}

int ShellRunner::execute(const QString &command, QString *stdoutOutput, QString *stderrOutput, int timeoutMs)
{
    QProcess proc;

    // 设置环境变量确保 UTF-8 输出
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
#ifdef Q_OS_WIN
    env.insert("LANG", "en_US.UTF-8");
    env.insert("PYTHONIOENCODING", "utf-8");
#endif
    proc.setProcessEnvironment(env);

    if (!m_workingDir.isEmpty())
        proc.setWorkingDirectory(m_workingDir);

    if (m_shellPath.isEmpty()) {
        if (stderrOutput) *stderrOutput = "找不到 shell 解释器";
        return -1;
    }

#ifdef Q_OS_WIN
    proc.start(m_shellPath, {"-c", command});
#else
    proc.start(m_shellPath, {"-c", command});
#endif

    if (!proc.waitForStarted(5000)) {
        if (stderrOutput) *stderrOutput = "无法启动 shell 进程";
        return -1;
    }

    bool finished = proc.waitForFinished(timeoutMs);
    if (!finished) {
        proc.kill();
        proc.waitForFinished(1000);
        if (stderrOutput) *stderrOutput = "命令执行超时";
        return -1;
    }

    if (stdoutOutput) *stdoutOutput = QString::fromLocal8Bit(proc.readAllStandardOutput());
    if (stderrOutput) *stderrOutput = QString::fromLocal8Bit(proc.readAllStandardError());

    return proc.exitCode();
}

void ShellRunner::start(const QString &command)
{
    if (m_shellPath.isEmpty()) {
        emit errorReady("找不到 shell 解释器");
        emit finished(-1, QProcess::CrashExit);
        return;
    }

    setupProcess();

#ifdef Q_OS_WIN
    m_process.start(m_shellPath, {"-c", command});
#else
    m_process.start(m_shellPath, {"-c", command});
#endif
}

void ShellRunner::startScript(const QString &scriptPath, const QStringList &args)
{
    if (m_shellPath.isEmpty()) {
        emit errorReady("找不到 shell 解释器");
        emit finished(-1, QProcess::CrashExit);
        return;
    }

    setupProcess();

    QStringList allArgs;
    allArgs << scriptPath << args;

#ifdef Q_OS_WIN
    m_process.start(m_shellPath, allArgs);
#else
    m_process.start(m_shellPath, allArgs);
#endif
}

void ShellRunner::kill()
{
    if (m_process.state() != QProcess::NotRunning) {
        m_process.kill();
        m_process.waitForFinished(3000);
    }
}

bool ShellRunner::isRunning() const
{
    return m_process.state() != QProcess::NotRunning;
}

QString ShellRunner::workingDirectory() const
{
    return m_workingDir;
}

void ShellRunner::setWorkingDirectory(const QString &dir)
{
    m_workingDir = dir;
}

void ShellRunner::onReadyReadStdout()
{
    QString text = QString::fromLocal8Bit(m_process.readAllStandardOutput());
    emit outputReady(text);
}

void ShellRunner::onReadyReadStderr()
{
    QString text = QString::fromLocal8Bit(m_process.readAllStandardError());
    emit errorReady(text);
}

void ShellRunner::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    emit finished(exitCode, exitStatus);
}

void ShellRunner::setupProcess()
{
    // 断开旧连接
    disconnect(&m_process, nullptr, this, nullptr);

    // 设置环境变量确保 UTF-8 输出
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
#ifdef Q_OS_WIN
    env.insert("LANG", "en_US.UTF-8");
    env.insert("PYTHONIOENCODING", "utf-8");
#endif
    m_process.setProcessEnvironment(env);

    if (!m_workingDir.isEmpty())
        m_process.setWorkingDirectory(m_workingDir);

    connect(&m_process, &QProcess::readyReadStandardOutput, this, &ShellRunner::onReadyReadStdout);
    connect(&m_process, &QProcess::readyReadStandardError, this, &ShellRunner::onReadyReadStderr);
    connect(&m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &ShellRunner::onFinished);
    connect(&m_process, &QProcess::started, this, &ShellRunner::started);
}
