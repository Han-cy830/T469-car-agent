#ifndef SSHCONNECTOR_H
#define SSHCONNECTOR_H

#include <QObject>
#include <QString>

class ShellRunner;

class SshConnector : public QObject
{
    Q_OBJECT

public:
    explicit SshConnector(QObject *parent = nullptr);

    void setConnectionInfo(const QString &host, int port, const QString &user, const QString &password);

    // 执行远程命令
    void executeCommand(const QString &command);

    // 上传目录
    void uploadDirectory(const QString &localDir, const QString &remoteDir);

    // 上传单个文件
    void uploadFile(const QString &localFile, const QString &remoteFile);

    // 停止当前操作
    void kill();

signals:
    void outputReady(const QString &text);
    void errorReady(const QString &text);
    void finished(int exitCode);

private:
    ShellRunner *m_runner;
    QString m_host;
    int m_port;
    QString m_user;
    QString m_password;

    QString buildSshCommand(const QString &command) const;
    QString buildScpCommand(const QString &local, const QString &remote) const;
    QString askpassScript() const;
};

#endif // SSHCONNECTOR_H
