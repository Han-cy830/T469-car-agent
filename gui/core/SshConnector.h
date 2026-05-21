#ifndef SSHCONNECTOR_H
#define SSHCONNECTOR_H

#include <QObject>
#include <QString>

class QProcess;

class SshConnector : public QObject
{
    Q_OBJECT

public:
    explicit SshConnector(QObject *parent = nullptr);
    ~SshConnector();

    void setConnectionInfo(const QString &host, int port, const QString &user, const QString &password);
    void executeCommand(const QString &command);
    void uploadDirectory(const QString &localDir, const QString &remoteDir);
    void uploadFile(const QString &localFile, const QString &remoteFile);
    void kill();

signals:
    void outputReady(const QString &text);
    void errorReady(const QString &text);
    void finished(int exitCode);

private slots:
    void onReadyReadStdout();
    void onReadyReadStderr();
    void onProcessFinished(int exitCode);

private:
    QProcess *m_process;
    QString m_host;
    int m_port;
    QString m_user;
    QString m_password;

    void startSshOrScp(const QString &program, const QStringList &extraArgs);
    QString askpassPath() const;
    void writeAskpass();
    void cleanupAskpass();
};
#endif
