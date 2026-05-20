#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>
#include <QString>

class SettingsManager : public QObject
{
    Q_OBJECT

public:
    static SettingsManager *instance();

    // 项目路径
    QString projectPath() const;
    void setProjectPath(const QString &path);

    // 工具链前缀 (如 arm-linux-gnueabi)
    QString toolchainPrefix() const;
    void setToolchainPrefix(const QString &prefix);

    // 工具链路径
    QString toolchainPath() const;
    void setToolchainPath(const QString &path);

    // sysroot 路径
    QString sysroot() const;
    void setSysroot(const QString &path);

    // Qt ARM 安装路径
    QString qtArmPath() const;
    void setQtArmPath(const QString &path);

    // SSH 配置
    QString sshHost() const;
    void setSshHost(const QString &host);

    int sshPort() const;
    void setSshPort(int port);

    QString sshUser() const;
    void setSshUser(const QString &user);

    QString sshPassword() const;
    void setSshPassword(const QString &password);

    QString sshDeployDir() const;
    void setSshDeployDir(const QString &dir);

    // 诊断脚本路径
    QString diagnoseScript() const;
    void setDiagnoseScript(const QString &path);

    // 修复脚本路径
    QString fixScript() const;
    void setFixScript(const QString &path);

    // 编译脚本路径
    QString buildScript() const;
    void setBuildScript(const QString &path);

    // Shell 路径
    QString shellPath() const;
    void setShellPath(const QString &path);

    // 自动检测工具链
    QString autoDetectToolchain();

signals:
    void settingsChanged();

private:
    explicit SettingsManager(QObject *parent = nullptr);
    QSettings m_settings;
};

#endif // SETTINGSMANAGER_H
