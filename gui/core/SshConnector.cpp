#include "SshConnector.h"
#include "utils/ShellRunner.h"
#include <QTemporaryFile>
#include <QDir>
#include <QFile>

SshConnector::SshConnector(QObject *parent)
    : QObject(parent)
    , m_runner(new ShellRunner(this))
    , m_port(22)
{
    connect(m_runner, &ShellRunner::outputReady, this, &SshConnector::outputReady);
    connect(m_runner, &ShellRunner::errorReady, this, &SshConnector::errorReady);
    connect(m_runner, &ShellRunner::finished, this, &SshConnector::finished);
}

void SshConnector::setConnectionInfo(const QString &host, int port, const QString &user, const QString &password)
{
    m_host = host;
    m_port = port;
    m_user = user;
    m_password = password;
}

void SshConnector::executeCommand(const QString &command)
{
    QString sshCmd = buildSshCommand(command);
    m_runner->start(sshCmd);
}

void SshConnector::uploadDirectory(const QString &localDir, const QString &remoteDir)
{
    QString target = m_user + "@" + m_host + ":" + remoteDir;
    QString scpCmd = buildScpCommand(localDir, target);

    // 使用 -r 递归复制
    scpCmd.replace("scp ", "scp -r ");

    m_runner->start(scpCmd);
}

void SshConnector::uploadFile(const QString &localFile, const QString &remoteFile)
{
    QString target = m_user + "@" + m_host + ":" + remoteFile;
    QString scpCmd = buildScpCommand(localFile, target);
    m_runner->start(scpCmd);
}

void SshConnector::kill()
{
    m_runner->kill();
}

QString SshConnector::buildSshCommand(const QString &command) const
{
    // 使用 SSH_ASKPASS 方式传递密码
    QString askpass = askpassScript();
    QString sshCmd = QString("SSH_ASKPASS='%1' SSH_ASKPASS_REQUIRE=force ssh -o StrictHostKeyChecking=no -p %2 %3@%4 \"%5\"")
        .arg(askpass)
        .arg(m_port)
        .arg(m_user)
        .arg(m_host)
        .arg(command);

    return sshCmd;
}

QString SshConnector::buildScpCommand(const QString &local, const QString &remote) const
{
    QString askpass = askpassScript();
    QString scpCmd = QString("SSH_ASKPASS='%1' SSH_ASKPASS_REQUIRE=force scp -o StrictHostKeyChecking=no -P %2 \"%3\" %4")
        .arg(askpass)
        .arg(m_port)
        .arg(local)
        .arg(remote);

    return scpCmd;
}

QString SshConnector::askpassScript() const
{
    // 创建一个临时的 askpass 脚本
    static QString scriptPath;
    if (scriptPath.isEmpty()) {
        // 写入临时文件
        QString path = QDir::tempPath() + "/t469_ssh_askpass.sh";
        QFile f(path);
        if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            f.write("#!/bin/sh\necho '" + m_password.toUtf8() + "'\n");
            f.close();
            QFile::setPermissions(path, QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner);
            scriptPath = path;
        }
    }
    // 更新密码
    QFile f(scriptPath);
    if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        f.write("#!/bin/sh\necho '" + m_password.toUtf8() + "'\n");
        f.close();
    }

    return scriptPath;
}
