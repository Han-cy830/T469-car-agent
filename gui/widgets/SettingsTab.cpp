#include "SettingsTab.h"
#include "core/SettingsManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QFileDialog>

SettingsTab::SettingsTab(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    loadSettings();
}

void SettingsTab::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(16);

    // 标题
    auto *titleLabel = new QLabel("设置");
    titleLabel->setObjectName("titleLabel");
    mainLayout->addWidget(titleLabel);

    auto *subtitleLabel = new QLabel("配置交叉编译环境参数");
    subtitleLabel->setObjectName("subtitleLabel");
    mainLayout->addWidget(subtitleLabel);

    // 项目设置
    auto *projectGroup = new QGroupBox("项目设置");
    auto *projectForm = new QFormLayout(projectGroup);

    auto *projectLayout = new QHBoxLayout();
    m_projectPathEdit = new QLineEdit();
    projectLayout->addWidget(m_projectPathEdit);
    auto *browseProjectBtn = new QPushButton("浏览...");
    connect(browseProjectBtn, &QPushButton::clicked, this, &SettingsTab::browseProjectPath);
    projectLayout->addWidget(browseProjectBtn);
    projectForm->addRow("项目路径:", projectLayout);

    mainLayout->addWidget(projectGroup);

    // 工具链设置
    auto *toolchainGroup = new QGroupBox("工具链设置");
    auto *toolchainForm = new QFormLayout(toolchainGroup);

    auto *prefixLayout = new QHBoxLayout();
    m_toolchainPrefixEdit = new QLineEdit();
    prefixLayout->addWidget(m_toolchainPrefixEdit);
    auto *detectBtn = new QPushButton("自动检测");
    detectBtn->setObjectName("primaryButton");
    connect(detectBtn, &QPushButton::clicked, this, &SettingsTab::detectToolchain);
    prefixLayout->addWidget(detectBtn);
    toolchainForm->addRow("工具链前缀:", prefixLayout);

    auto *tcPathLayout = new QHBoxLayout();
    m_toolchainPathEdit = new QLineEdit();
    tcPathLayout->addWidget(m_toolchainPathEdit);
    auto *browseTcBtn = new QPushButton("浏览...");
    connect(browseTcBtn, &QPushButton::clicked, this, &SettingsTab::browseToolchainPath);
    tcPathLayout->addWidget(browseTcBtn);
    toolchainForm->addRow("工具链路径:", tcPathLayout);

    m_sysrootEdit = new QLineEdit();
    toolchainForm->addRow("Sysroot:", m_sysrootEdit);

    mainLayout->addWidget(toolchainGroup);

    // Qt 设置
    auto *qtGroup = new QGroupBox("Qt 设置");
    auto *qtForm = new QFormLayout(qtGroup);

    auto *qtPathLayout = new QHBoxLayout();
    m_qtPathEdit = new QLineEdit();
    qtPathLayout->addWidget(m_qtPathEdit);
    auto *browseQtBtn = new QPushButton("浏览...");
    connect(browseQtBtn, &QPushButton::clicked, this, &SettingsTab::browseQtPath);
    qtPathLayout->addWidget(browseQtBtn);
    qtForm->addRow("Qt ARM 路径:", qtPathLayout);

    mainLayout->addWidget(qtGroup);

    // 脚本设置
    auto *scriptGroup = new QGroupBox("脚本设置");
    auto *scriptForm = new QFormLayout(scriptGroup);

    m_shellPathEdit = new QLineEdit();
    scriptForm->addRow("Shell 路径:", m_shellPathEdit);

    m_diagnoseScriptEdit = new QLineEdit();
    scriptForm->addRow("诊断脚本:", m_diagnoseScriptEdit);

    m_fixScriptEdit = new QLineEdit();
    scriptForm->addRow("修复脚本:", m_fixScriptEdit);

    m_buildScriptEdit = new QLineEdit();
    scriptForm->addRow("编译脚本:", m_buildScriptEdit);

    mainLayout->addWidget(scriptGroup);

    // 保存按钮
    auto *btnLayout = new QHBoxLayout();
    m_saveBtn = new QPushButton("保存设置");
    m_saveBtn->setObjectName("primaryButton");
    m_saveBtn->setMinimumSize(140, 40);
    m_saveBtn->setCursor(Qt::PointingHandCursor);
    connect(m_saveBtn, &QPushButton::clicked, this, &SettingsTab::saveSettings);
    btnLayout->addWidget(m_saveBtn);

    m_statusLabel = new QLabel("");
    btnLayout->addWidget(m_statusLabel);
    btnLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    mainLayout->addStretch();
}

void SettingsTab::loadSettings()
{
    SettingsManager *s = SettingsManager::instance();
    m_projectPathEdit->setText(s->projectPath());
    m_toolchainPrefixEdit->setText(s->toolchainPrefix());
    m_toolchainPathEdit->setText(s->toolchainPath());
    m_sysrootEdit->setText(s->sysroot());
    m_qtPathEdit->setText(s->qtArmPath());
    m_shellPathEdit->setText(s->shellPath());
    m_diagnoseScriptEdit->setText(s->diagnoseScript());
    m_fixScriptEdit->setText(s->fixScript());
    m_buildScriptEdit->setText(s->buildScript());
}

void SettingsTab::saveSettings()
{
    SettingsManager *s = SettingsManager::instance();
    s->setProjectPath(m_projectPathEdit->text());
    s->setToolchainPrefix(m_toolchainPrefixEdit->text());
    s->setToolchainPath(m_toolchainPathEdit->text());
    s->setSysroot(m_sysrootEdit->text());
    s->setQtArmPath(m_qtPathEdit->text());
    s->setShellPath(m_shellPathEdit->text());
    s->setDiagnoseScript(m_diagnoseScriptEdit->text());
    s->setFixScript(m_fixScriptEdit->text());
    s->setBuildScript(m_buildScriptEdit->text());

    m_statusLabel->setText("已保存");
    m_statusLabel->setStyleSheet("color: #a6e3a1;");
}

void SettingsTab::detectToolchain()
{
    SettingsManager *s = SettingsManager::instance();
    QString gcc = s->autoDetectToolchain();
    if (!gcc.isEmpty()) {
        m_toolchainPrefixEdit->setText(s->toolchainPrefix());
        m_toolchainPathEdit->setText(s->toolchainPath());
        m_statusLabel->setText("检测到: " + gcc);
        m_statusLabel->setStyleSheet("color: #a6e3a1;");
    } else {
        m_statusLabel->setText("未检测到 ARM 工具链");
        m_statusLabel->setStyleSheet("color: #f38ba8;");
    }
}

void SettingsTab::browseProjectPath()
{
    QString dir = QFileDialog::getExistingDirectory(this, "选择项目路径", m_projectPathEdit->text());
    if (!dir.isEmpty()) m_projectPathEdit->setText(dir);
}

void SettingsTab::browseToolchainPath()
{
    QString dir = QFileDialog::getExistingDirectory(this, "选择工具链路径", m_toolchainPathEdit->text());
    if (!dir.isEmpty()) m_toolchainPathEdit->setText(dir);
}

void SettingsTab::browseQtPath()
{
    QString dir = QFileDialog::getExistingDirectory(this, "选择 Qt ARM 路径", m_qtPathEdit->text());
    if (!dir.isEmpty()) m_qtPathEdit->setText(dir);
}
