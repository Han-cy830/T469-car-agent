#include "SettingsManager.h"
#include "utils/ShellRunner.h"
#include <QCoreApplication>
#include <QDir>
#include <QStandardPaths>
#include <QFileInfo>

SettingsManager *SettingsManager::instance()
{
    static SettingsManager mgr;
    return &mgr;
}

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent)
    , m_settings(QSettings::IniFormat, QSettings::UserScope, "T469", "CarAgent")
{
}

// 项目路径
QString SettingsManager::projectPath() const
{
    // 默认为可执行文件所在目录的上一级（gui/ 的父目录）
    QString defaultPath = QDir::cleanPath(qApp->applicationDirPath() + "/..");
    return m_settings.value("project/path", defaultPath).toString();
}

void SettingsManager::setProjectPath(const QString &path)
{
    m_settings.setValue("project/path", path);
    emit settingsChanged();
}

// 工具链前缀
QString SettingsManager::toolchainPrefix() const
{
    return m_settings.value("toolchain/prefix", "arm-linux-gnueabi").toString();
}

void SettingsManager::setToolchainPrefix(const QString &prefix)
{
    m_settings.setValue("toolchain/prefix", prefix);
    emit settingsChanged();
}

// 工具链路径
QString SettingsManager::toolchainPath() const
{
    return m_settings.value("toolchain/path", "/usr/bin").toString();
}

void SettingsManager::setToolchainPath(const QString &path)
{
    m_settings.setValue("toolchain/path", path);
    emit settingsChanged();
}

// sysroot
QString SettingsManager::sysroot() const
{
    return m_settings.value("toolchain/sysroot", "").toString();
}

void SettingsManager::setSysroot(const QString &path)
{
    m_settings.setValue("toolchain/sysroot", path);
    emit settingsChanged();
}

// Qt ARM 路径
QString SettingsManager::qtArmPath() const
{
    return m_settings.value("qt/arm_path", "/opt/qt5-arm").toString();
}

void SettingsManager::setQtArmPath(const QString &path)
{
    m_settings.setValue("qt/arm_path", path);
    emit settingsChanged();
}

// SSH 配置
QString SettingsManager::sshHost() const
{
    return m_settings.value("ssh/host", "192.168.182.130").toString();
}

void SettingsManager::setSshHost(const QString &host)
{
    m_settings.setValue("ssh/host", host);
    emit settingsChanged();
}

int SettingsManager::sshPort() const
{
    return m_settings.value("ssh/port", 22).toInt();
}

void SettingsManager::setSshPort(int port)
{
    m_settings.setValue("ssh/port", port);
    emit settingsChanged();
}

QString SettingsManager::sshUser() const
{
    return m_settings.value("ssh/user", "hancy").toString();
}

void SettingsManager::setSshUser(const QString &user)
{
    m_settings.setValue("ssh/user", user);
    emit settingsChanged();
}

QString SettingsManager::sshPassword() const
{
    return m_settings.value("ssh/password", "").toString();
}

void SettingsManager::setSshPassword(const QString &password)
{
    m_settings.setValue("ssh/password", password);
    emit settingsChanged();
}

QString SettingsManager::sshDeployDir() const
{
    return m_settings.value("ssh/deploy_dir", "~/deploy").toString();
}

void SettingsManager::setSshDeployDir(const QString &dir)
{
    m_settings.setValue("ssh/deploy_dir", dir);
    emit settingsChanged();
}

// 脚本路径
QString SettingsManager::diagnoseScript() const
{
    return m_settings.value("scripts/diagnose", projectPath() + "/diagnose.sh").toString();
}

void SettingsManager::setDiagnoseScript(const QString &path)
{
    m_settings.setValue("scripts/diagnose", path);
    emit settingsChanged();
}

QString SettingsManager::fixScript() const
{
    return m_settings.value("scripts/fix", projectPath() + "/fix.sh").toString();
}

void SettingsManager::setFixScript(const QString &path)
{
    m_settings.setValue("scripts/fix", path);
    emit settingsChanged();
}

QString SettingsManager::buildScript() const
{
    return m_settings.value("scripts/build", projectPath() + "/build.sh").toString();
}

void SettingsManager::setBuildScript(const QString &path)
{
    m_settings.setValue("scripts/build", path);
    emit settingsChanged();
}

// Shell 路径
QString SettingsManager::shellPath() const
{
    QString defaultShell = ShellRunner::findShell();
    return m_settings.value("system/shell", defaultShell).toString();
}

void SettingsManager::setShellPath(const QString &path)
{
    m_settings.setValue("system/shell", path);
    emit settingsChanged();
}

// 自动检测工具链
QString SettingsManager::autoDetectToolchain()
{
    ShellRunner runner;
    QString output;
    int rc = runner.execute("which arm-linux-gnueabi-gcc 2>/dev/null || which arm-linux-gnueabihf-gcc 2>/dev/null || echo ''", &output);
    if (rc == 0 && !output.trimmed().isEmpty()) {
        QString gccPath = output.trimmed();
        // 提取目录和前缀
        QFileInfo fi(gccPath);
        setToolchainPath(fi.absolutePath());
        QString name = fi.fileName();
        if (name.endsWith("-gcc")) {
            setToolchainPrefix(name.left(name.length() - 4));
        }
        return gccPath;
    }
    return QString();
}
