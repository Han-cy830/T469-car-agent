#include "DeployTab.h"
#include "core/SshConnector.h"
#include "core/SettingsManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>

DeployTab::DeployTab(QWidget *parent)
    : QWidget(parent)
    , m_ssh(new SshConnector(this))
{
    setupUi();

    connect(m_ssh, &SshConnector::outputReady, this, &DeployTab::onSshOutput);
    connect(m_ssh, &SshConnector::errorReady, this, &DeployTab::onSshError);
    connect(m_ssh, &SshConnector::finished, this, &DeployTab::onSshFinished);
}

void DeployTab::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(16);

    // 标题
    auto *titleLabel = new QLabel("部署到设备");
    titleLabel->setObjectName("titleLabel");
    mainLayout->addWidget(titleLabel);

    auto *subtitleLabel = new QLabel("通过 SSH 将编译产物传输到 ARM 设备");
    subtitleLabel->setObjectName("subtitleLabel");
    mainLayout->addWidget(subtitleLabel);

    // SSH 配置
    auto *sshGroup = new QGroupBox("SSH 连接配置");
    auto *formLayout = new QFormLayout(sshGroup);

    SettingsManager *s = SettingsManager::instance();

    m_hostEdit = new QLineEdit(s->sshHost());
    formLayout->addRow("主机地址:", m_hostEdit);

    m_portSpin = new QSpinBox();
    m_portSpin->setRange(1, 65535);
    m_portSpin->setValue(s->sshPort());
    formLayout->addRow("端口:", m_portSpin);

    m_userEdit = new QLineEdit(s->sshUser());
    formLayout->addRow("用户名:", m_userEdit);

    m_passwordEdit = new QLineEdit(s->sshPassword());
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    formLayout->addRow("密码:", m_passwordEdit);

    m_deployDirEdit = new QLineEdit(s->sshDeployDir());
    formLayout->addRow("远程目录:", m_deployDirEdit);

    mainLayout->addWidget(sshGroup);

    // 操作按钮
    auto *controlLayout = new QHBoxLayout();

    m_testBtn = new QPushButton("测试连接");
    m_testBtn->setMinimumSize(120, 40);
    m_testBtn->setCursor(Qt::PointingHandCursor);
    connect(m_testBtn, &QPushButton::clicked, this, &DeployTab::testConnection);
    controlLayout->addWidget(m_testBtn);

    m_deployBtn = new QPushButton("开始部署");
    m_deployBtn->setObjectName("primaryButton");
    m_deployBtn->setMinimumSize(120, 40);
    m_deployBtn->setCursor(Qt::PointingHandCursor);
    connect(m_deployBtn, &QPushButton::clicked, this, &DeployTab::deployFiles);
    controlLayout->addWidget(m_deployBtn);

    m_progress = new QProgressBar();
    m_progress->setRange(0, 0);
    m_progress->setVisible(false);
    m_progress->setMaximumHeight(8);
    controlLayout->addWidget(m_progress, 1);

    m_statusLabel = new QLabel("就绪");
    m_statusLabel->setObjectName("subtitleLabel");
    controlLayout->addWidget(m_statusLabel);

    controlLayout->addStretch();
    mainLayout->addLayout(controlLayout);

    // 输出
    auto *outputGroup = new QGroupBox("输出");
    auto *outputLayout = new QVBoxLayout(outputGroup);
    m_outputView = new QTextEdit();
    m_outputView->setReadOnly(true);
    m_outputView->setMinimumHeight(120);
    outputLayout->addWidget(m_outputView);
    mainLayout->addWidget(outputGroup, 1);
}

void DeployTab::testConnection()
{
    m_outputView->clear();
    m_testBtn->setEnabled(false);
    m_progress->setVisible(true);
    m_statusLabel->setText("正在测试连接...");

    // 保存配置
    SettingsManager *s = SettingsManager::instance();
    s->setSshHost(m_hostEdit->text());
    s->setSshPort(m_portSpin->value());
    s->setSshUser(m_userEdit->text());
    s->setSshPassword(m_passwordEdit->text());

    m_ssh->setConnectionInfo(m_hostEdit->text(), m_portSpin->value(),
                             m_userEdit->text(), m_passwordEdit->text());
    m_ssh->executeCommand("echo 'SSH 连接成功! $(uname -a)'");
}

void DeployTab::deployFiles()
{
    m_outputView->clear();
    m_deployBtn->setEnabled(false);
    m_progress->setVisible(true);
    m_statusLabel->setText("正在部署...");

    SettingsManager *s = SettingsManager::instance();
    s->setSshDeployDir(m_deployDirEdit->text());

    emit logOutput(">>> 开始部署到 " + m_hostEdit->text() + "...");

    QString deployDir = m_deployDirEdit->text();
    QString localDir = s->projectPath() + "/deploy";

    m_ssh->setConnectionInfo(m_hostEdit->text(), m_portSpin->value(),
                             m_userEdit->text(), m_passwordEdit->text());
    m_ssh->uploadDirectory(localDir, deployDir);
}

void DeployTab::onSshOutput(const QString &text)
{
    m_outputView->append(text);
    emit logOutput(text);
}

void DeployTab::onSshError(const QString &text)
{
    m_outputView->append("<span style='color:#f38ba8;'>" + text + "</span>");
    emit logError(text);
}

void DeployTab::onSshFinished(int exitCode)
{
    m_testBtn->setEnabled(true);
    m_deployBtn->setEnabled(true);
    m_progress->setVisible(false);

    if (exitCode == 0) {
        m_statusLabel->setText("操作成功");
        m_statusLabel->setStyleSheet("color: #a6e3a1;");
    } else {
        m_statusLabel->setText("操作失败");
        m_statusLabel->setStyleSheet("color: #f38ba8;");
    }
}
