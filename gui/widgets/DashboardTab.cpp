#include "DashboardTab.h"
#include "core/SettingsManager.h"
#include "utils/ShellRunner.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFrame>
#include <QTimer>

DashboardTab::DashboardTab(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void DashboardTab::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(20);

    // 标题
    auto *titleLabel = new QLabel("概览");
    titleLabel->setObjectName("titleLabel");
    mainLayout->addWidget(titleLabel);

    auto *subtitleLabel = new QLabel("T469 车载交叉编译环境状态");
    subtitleLabel->setObjectName("subtitleLabel");
    mainLayout->addWidget(subtitleLabel);

    mainLayout->addSpacing(8);

    // 状态卡片网格
    auto *cardLayout = new QGridLayout();
    cardLayout->setSpacing(16);

    cardLayout->addWidget(createInfoCard("工具链", "🔧", &m_toolchainLabel), 0, 0);
    cardLayout->addWidget(createInfoCard("Qt ARM", "🎨", &m_qtLabel), 0, 1);
    cardLayout->addWidget(createInfoCard("ALSA", "🔊", &m_alsaLabel), 0, 2);
    cardLayout->addWidget(createInfoCard("部署目录", "📦", &m_deployLabel), 0, 3);

    mainLayout->addLayout(cardLayout);

    // 快捷操作
    auto *quickGroup = new QGroupBox("快捷操作");
    auto *quickLayout = new QHBoxLayout(quickGroup);
    quickLayout->setSpacing(12);

    auto *diagBtn = new QPushButton("运行诊断");
    diagBtn->setMinimumSize(120, 40);
    diagBtn->setCursor(Qt::PointingHandCursor);
    quickLayout->addWidget(diagBtn);

    auto *fixBtn = new QPushButton("一键修复");
    fixBtn->setMinimumSize(120, 40);
    fixBtn->setCursor(Qt::PointingHandCursor);
    quickLayout->addWidget(fixBtn);

    auto *buildBtn = new QPushButton("开始编译");
    buildBtn->setMinimumSize(120, 40);
    buildBtn->setCursor(Qt::PointingHandCursor);
    quickLayout->addWidget(buildBtn);

    auto *deployBtn = new QPushButton("部署到设备");
    deployBtn->setMinimumSize(120, 40);
    deployBtn->setCursor(Qt::PointingHandCursor);
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
    QString info = QString(
        "<table style='color:#cdd6f4;'>"
        "<tr><td style='padding:4px 16px 4px 0; color:#89b4fa;'>项目路径:</td><td>%1</td></tr>"
        "<tr><td style='padding:4px 16px 4px 0; color:#89b4fa;'>工具链前缀:</td><td>%2</td></tr>"
        "<tr><td style='padding:4px 16px 4px 0; color:#89b4fa;'>Qt ARM 路径:</td><td>%3</td></tr>"
        "<tr><td style='padding:4px 16px 4px 0; color:#89b4fa;'>SSH 目标:</td><td>%4@%5</td></tr>"
        "</table>"
    ).arg(s->projectPath(), s->toolchainPrefix(), s->qtArmPath(), s->sshUser(), s->sshHost());

    envLabel->setText(info);
    envLayout->addWidget(envLabel);
    mainLayout->addWidget(envGroup);

    mainLayout->addStretch();

    // 异步检测环境状态
    QTimer::singleShot(500, this, [this]() {
        ShellRunner runner;
        QString output;

        // 检测工具链
        int rc = runner.execute("which arm-linux-gnueabi-gcc 2>/dev/null && arm-linux-gnueabi-gcc --version 2>/dev/null | head -1", &output);
        if (rc == 0 && !output.trimmed().isEmpty()) {
            m_toolchainLabel->setText(output.trimmed().split('\n').first());
            m_toolchainLabel->setStyleSheet("color: #a6e3a1;");
        } else {
            m_toolchainLabel->setText("未安装");
            m_toolchainLabel->setStyleSheet("color: #f38ba8;");
        }

        // 检测 Qt
        rc = runner.execute("test -f /opt/qt5-arm/bin/qmake && echo '已安装' || echo '未安装'", &output);
        m_qtLabel->setText(output.trimmed());
        m_qtLabel->setStyleSheet(output.trimmed() == "已安装" ? "color: #a6e3a1;" : "color: #f38ba8;");

        // 检测 ALSA
        SettingsManager *s = SettingsManager::instance();
        rc = runner.execute("test -f " + s->projectPath() + "/compiled/alsa-lib-1.2.2/build/lib/libasound.so && echo '已编译' || echo '未编译'", &output);
        m_alsaLabel->setText(output.trimmed());
        m_alsaLabel->setStyleSheet(output.trimmed() == "已编译" ? "color: #a6e3a1;" : "color: #f9e2af;");

        // 检测部署目录
        rc = runner.execute("test -d " + s->projectPath() + "/deploy && echo '已创建' || echo '未创建'", &output);
        m_deployLabel->setText(output.trimmed());
        m_deployLabel->setStyleSheet(output.trimmed() == "已创建" ? "color: #a6e3a1;" : "color: #f9e2af;");
    });
}

QWidget *DashboardTab::createInfoCard(const QString &title, const QString &icon, QLabel **valueLabel)
{
    auto *card = new QFrame();
    card->setObjectName("infoCard");
    card->setStyleSheet(R"(
        QFrame#infoCard {
            background-color: #313244;
            border-radius: 8px;
            padding: 16px;
        }
    )");
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
