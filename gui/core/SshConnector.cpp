#include "SshConnector.h"
#include <QProcess>
#include <QDir>
#include <QFile>

SshConnector::SshConnector(QObject *parent)
    : QObject(parent), m_process(new QProcess(this)), m_port(22)
{
    connect(m_process, &QProcess::readyReadStandardOutput, this, &SshConnector::onReadyReadStdout);
    connect(m_process, &QProcess::readyReadStandardError, this, &SshConnector::onReadyReadStderr);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &SshConnector::onProcessFinished);
}

SshConnector::~SshConnector() { cleanupAskpass(); }

void SshConnector::setConnectionInfo(const QString &host, int port, const QString &user, const QString &password)
{
    m_host = host; m_port = port; m_user = user; m_password = password;
}

void SshConnector::executeCommand(const QString &command)
{
    startSshOrScp("ssh", {"-o", "StrictHostKeyChecking=no", "-p", QString::number(m_port),
                           m_user + "@" + m_host, command});
}

void SshConnector::uploadDirectory(const QString &localDir, const QString &remoteDir)
{
    startSshOrScp("scp", {"-r", "-o", "StrictHostKeyChecking=no", "-P", QString::number(m_port),
                           localDir, m_user + "@" + m_host + ":" + remoteDir});
}

void SshConnector::uploadFile(const QString &localFile, const QString &remoteFile)
{
    startSshOrScp("scp", {"-o", "StrictHostKeyChecking=no", "-P", QString::number(m_port),
                           localFile, m_user + "@" + m_host + ":" + remoteFile});
}

void SshConnector::kill()
{
    if (m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        m_process->waitForFinished(3000);
    }
}

void SshConnector::onReadyReadStdout() { emit outputReady(QString::fromLocal8Bit(m_process->readAllStandardOutput())); }
void SshConnector::onReadyReadStderr() { emit errorReady(QString::fromLocal8Bit(m_process->readAllStandardError())); }
void SshConnector::onProcessFinished(int exitCode) { cleanupAskpass(); emit finished(exitCode); }

void SshConnector::startSshOrScp(const QString &program, const QStringList &extraArgs)
{
    writeAskpass();
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("SSH_ASKPASS", askpassPath());
    env.insert("SSH_ASKPASS_REQUIRE", "force");
    env.insert("SSH_PASS", m_password);
    m_process->setProcessEnvironment(env);
    m_process->start(program, extraArgs);
}

QString SshConnector::askpassPath() const
{
    return QDir::tempPath() + "/t469_askpass_" + QString::number((quintptr)this) + ".sh";
}

void SshConnector::writeAskpass()
{
    QFile f(askpassPath());
    if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        f.write("#!/bin/sh\necho \"$SSH_PASS\"\n");
        f.close();
        QFile::setPermissions(askpassPath(), QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner);
    }
}

void SshConnector::cleanupAskpass() { QFile::remove(askpassPath()); }
