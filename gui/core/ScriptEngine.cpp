#include "ScriptEngine.h"
#include "utils/ShellRunner.h"

ScriptEngine::ScriptEngine(QObject *parent)
    : QObject(parent)
    , m_runner(new ShellRunner(this))
{
    connect(m_runner, &ShellRunner::outputReady, this, &ScriptEngine::outputReady);
    connect(m_runner, &ShellRunner::errorReady, this, &ScriptEngine::errorReady);
    connect(m_runner, &ShellRunner::finished, this, &ScriptEngine::finished);
    connect(m_runner, &ShellRunner::started, this, &ScriptEngine::started);
}

void ScriptEngine::runDiagnosis(const QString &scriptPath, const QString &workDir)
{
    m_runner->setWorkingDirectory(workDir);
    m_runner->start("bash \"" + scriptPath + "\"");
}

void ScriptEngine::runFix(const QString &scriptPath, const QString &workDir, const QString &fixOption)
{
    m_runner->setWorkingDirectory(workDir);
    QString cmd = "bash \"" + scriptPath + "\"";
    if (!fixOption.isEmpty())
        cmd += " " + fixOption;
    else
        cmd += " --all";
    m_runner->start(cmd);
}

void ScriptEngine::runBuild(const QString &scriptPath, const QString &workDir, const QString &target)
{
    m_runner->setWorkingDirectory(workDir);
    m_runner->start("bash \"" + scriptPath + "\" " + target);
}

void ScriptEngine::runCommand(const QString &command, const QString &workDir)
{
    m_runner->setWorkingDirectory(workDir);
    m_runner->start(command);
}

void ScriptEngine::kill()
{
    m_runner->kill();
}

bool ScriptEngine::isRunning() const
{
    return m_runner->isRunning();
}
