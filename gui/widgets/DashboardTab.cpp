#include "DashboardTab.h"
#include "core/SettingsManager.h"
#include "utils/ShellRunner.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFrame>
#include <QTimer>

DashboardTab::DashboardTab(QWidget *parent)
    : QWidget(parent), m_runner(new ShellRunner(this)), m_checkIndex(0)
{
    setupUi();
    connect(m_runner, &ShellRunner::finished, this, [this](int exitCode) {
        QString output;
        // 读取已缓冲的输出
        QLabel *labels[] = {m_toolchainLabel, m_qtLabel, m_alsaLabel, m_deployLabel};
        if (m_checkIndex >= 0 && m_checkIndex < 4) {
            QLabel *lbl = labels[m_checkIndex];
            if (exitCode == 0 && m_runner->property("lastOutput").toString().trimmed().isEmpty() == false) {
                QString val = m_runner->property("lastOutput").toString().trimmed().split('\n').first();
                lbl->setText(val);
                lbl->setStyleSheet("color: #a6e3a1;");
            }
        }
        m_checkIndex++;
        QTimer::singleShot(100, this, &DashboardTab::runNextCheck);
    });
    connect(m_runner, &ShellRunner::outputReady, this, [this](const QString &text) {
        m_runner->setProperty("lastOutput", m_runner->property("lastOutput").toString() + text);
    });
}

void DashboardTab::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(20);

    auto *titleLabel = new QLabel("概览");
    titleLabel->setObjectName("titleLabel");
    mainLayout->addWidget(titleLabel);

    auto *subtitleLabel = new QLabel("T469 车载交叉编译环境状态");
    subtitleLabel->setObjectName("subtitleLabel");
    mainLayout->addWidget(subtitleLabel);
    mainLayout->addSpacing(8);

    auto *cardLayout = new QGridLayout();
    cardLayout->setSpacing(16);
    cardLayout->addWidget(createInfoCard("工具链", "\xF0\x9F\x94\xA7", &m_toolchainLabel), 0, 0);
    cardLayout->addWidget(createInfoCard("Qt ARM", "\xF0\x9F\x8E\xA8", &m_qtLabel), 0, 1);
    cardLayout->addWidget(createInfoCard("ALSA", "\xF0\x9F\x94\x8A", &m_alsaLabel), 0, 2);
    cardLayout->addWidget(createInfoCard("部署目录", "\xF0\x9F\x93\xA6", &m_deployLabel), 0, 3);
    mainLayout->addLayout(cardLayout);

    // 快捷操作
    auto *quickGroup = new QGroupBox("快捷操作");
    auto *quickLayout = new QHBoxLayout(quickGroup);
    quickLayout->setSpacing(12);

    auto *diagBtn = new QPushButton("运行诊断");
    diagBtn->setMinimumSize(120, 40);
    diagBtn->setCursor(Qt::PointingHandCursor);
    connect(diagBtn, &QPushButton::clicked, this, [this]() { emit navigateToPage(1); });
    quickLayout->addWidget(diagBtn);

    auto *fixBtn = new QPushButton("一键修复");
    fixBtn->setMinimumSize(120, 40);
    fixBtn->setCursor(Qt::PointingHandCursor);
    connect(fixBtn, &QPushButton::clicked, this, [this]() { emit navigateToPage(2); });
    quickLayout->addWidget(fixBtn);

    auto *buildBtn = new QPushButton("开始编译");
    buildBtn->setMinimumSize(120, 40);
    buildBtn->setCursor(Qt::PointingHandCursor);
    connect(buildBtn, &QPushButton::clicked, this, [this]() { emit navigateToPage(3); });
    quickLayout->addWidget(buildBtn);

    auto *deployBtn = new QPushButton("部署到设备");
    deployBtn->setMinimumSize(120, 40);
    deployBtn->setCursor(Qt::PointingHandCursor);
    connect(deployBtn, &QPushButton::clicked, this, [this]() { emit navigateToPage(4); });
    quickLayout->addWidget(deployBtn);

    quickLayout->addStretch();
    mainLayout->addWidget(quickGroup);

    // 环境信息
    auto *envGroup = new QGroupBox("环境信息");
    auto *envLayout = new QVBoxLayout(envGroup);
    auto *envLabel = new QLabel();
    envLabel->setTextFormat(Qt::RichText);
    envLabel->setWordWrap(true);
    SettingsManager *s = SettingsManager::instance();
    envLabel->setText(QString(
        "<table style='color:#cdd6f4;'>"
        "<tr><td style='padding:4px 16px 4px 0; color:#89b4fa;'>项目路径:</td><td>%1</td></tr>"
        "<tr><td style='padding:4px 16px 4px 0; color:#89b4fa;'>工具链前缀:</td><td>%2</td></tr>"
        "<tr><td style='padding:4px 16px 4px 0; color:#89b4fa;'>Qt ARM 路径:</td><td>%3</td></tr>"
        "<tr><td style='padding:4px 16px 4px 0; color:#89b4fa;'>SSH 目标:</td><td>%4@%5</td></tr>"
        "</table>"
    ).arg(s->projectPath(), s->toolchainPrefix(), s->qtArmPath(), s->sshUser(), s->sshHost()));
    envLayout->addWidget(envLabel);
    mainLayout->addWidget(envGroup);
    mainLayout->addStretch();

    // 异步检测环境状态
    QTimer::singleShot(500, this, &DashboardTab::runNextCheck);
}

void DashboardTab::runNextCheck()
{
    SettingsManager *s = SettingsManager::instance();
    m_runner->setProperty("lastOutput", QString());

    switch (m_checkIndex) {
    case 0:
        m_toolchainLabel->setText("检测中...");
        m_runner->start("which arm-linux-gnueabi-gcc 2>/dev/null && arm-linux-gnueabi-gcc --version 2>/dev/null | head -1 || echo '未安装'");
        break;
    case 1:
        m_qtLabel->setText("检测中...");
        m_runner->start("test -f /opt/qt5-arm/bin/qmake && echo '已安装' || echo '未安装'");
        break;
    case 2:
        m_alsaLabel->setText("检测中...");
        m_runner->start("test -f \"" + s->projectPath() + "/compiled/alsa-lib-1.2.2/build/lib/libasound.so\" && echo '已编译' || echo '未编译'");
        break;
    case 3:
        m_deployLabel->setText("检测中...");
        m_runner->start("test -d \"" + s->projectPath() + "/deploy\" && echo '已创建' || echo '未创建'");
        break;
    default:
        break;
    }
}

QWidget *DashboardTab::createInfoCard(const QString &title, const QString &icon, QLabel **valueLabel)
{
    auto *card = new QFrame();
    card->setObjectName("infoCard");
    card->setStyleSheet("QFrame#infoCard { background-color: #313244; border-radius: 8px; padding: 16px; }");
    card->setMinimumSize(160, 100);
    auto *layout = new QVBoxLayout(card);
    layout->setSpacing(8);

    auto *iconLabel = new QLabel(icon);
    iconLabel->setStyleSheet("font-size: 24px;");
    layout->addWidget(iconLabel);

    auto *titleLabel = new QLabel(title);
    titleLabel->setStyleSheet("color: #a6adc8; font-size: 12px;");
    layout->addWidget(titleLabel);

    *valueLabel = new QLabel("检测中...");
    (*valueLabel)->setStyleSheet("font-size: 14px; font-weight: bold;");
    layout->addWidget(*valueLabel);
    return card;
}
