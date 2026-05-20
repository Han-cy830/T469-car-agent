#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include <QObject>
#include <QString>
#include <QProcess>

class ShellRunner;

class ScriptEngine : public QObject
{
    Q_OBJECT

public:
    explicit ScriptEngine(QObject *parent = nullptr);

    // 执行诊断脚本
    void runDiagnosis(const QString &scriptPath, const QString &workDir);

    // 执行修复脚本（可指定单项）
    void runFix(const QString &scriptPath, const QString &workDir, const QString &fixOption = QString());

    // 执行编译脚本
    void runBuild(const QString &scriptPath, const QString &workDir, const QString &target = "all");

    // 执行自定义命令
    void runCommand(const QString &command, const QString &workDir);

    // 停止
    void kill();

    bool isRunning() const;

signals:
    void outputReady(const QString &text);
    void errorReady(const QString &text);
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void started();

private:
    ShellRunner *m_runner;
};

#endif // SCRIPTENGINE_H
