#include "FixTab.h"
#include "utils/ShellRunner.h"
#include "core/SettingsManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QGroupBox>

FixTab::FixTab(QWidget *parent)
    : QWidget(parent)
    , m_runner(new ShellRunner(this))
{
    setupUi();
    connect(m_runner, &ShellRunner::outputReady, this, &FixTab::logOutput);
    connect(m_runner, &ShellRunner::errorReady, this, &FixTab::logError);
    connect(m_runner, &ShellRunner::finished, this, &FixTab::onFinished);
}

void FixTab::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(16);

    // 标题
    auto *titleLabel = new QLabel("自动修复");
    titleLabel->setObjectName("titleLabel");
    mainLayout->addWidget(titleLabel);

    auto *subtitleLabel = new QLabel("自动检测并修复交叉编译环境中的常见问题");
    subtitleLabel->setObjectName("subtitleLabel");
    mainLayout->addWidget(subtitleLabel);

    // 控制栏
    auto *controlLayout = new QHBoxLayout();

    m_fixAllBtn = new QPushButton("一键全部修复");
    m_fixAllBtn->setObjectName("primaryButton");
    m_fixAllBtn->setMinimumSize(160, 40);
    m_fixAllBtn->setCursor(Qt::PointingHandCursor);
    connect(m_fixAllBtn, &QPushButton::clicked, this, &FixTab::runFixAll);
    controlLayout->addWidget(m_fixAllBtn);

    m_statusLabel = new QLabel("就绪");
    m_statusLabel->setObjectName("subtitleLabel");
    controlLayout->addWidget(m_statusLabel);

    controlLayout->addStretch();
    mainLayout->addLayout(controlLayout);

    // 修复项表格
    auto *fixGroup = new QGroupBox("可修复项目");
    auto *fixLayout = new QVBoxLayout(fixGroup);

    m_fixTable = new QTableWidget();
    m_fixTable->setColumnCount(4);
    m_fixTable->setHorizontalHeaderLabels({"状态", "问题", "修复操作", "详情"});
    m_fixTable->horizontalHeader()->setStretchLastSection(true);
    m_fixTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_fixTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_fixTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_fixTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_fixTable->verticalHeader()->setVisible(false);
    m_fixTable->setShowGrid(false);

    // 预定义的修复项
    struct FixEntry {
        QString problem;
        QString action;
        QString detail;
    };
    QVector<FixEntry> entries = {
        {"CRLF 换行符", "转换为 Unix 格式", "运行 dos2unix 修复 Windows 换行符"},
        {"递归嵌套目录", "清理嵌套的 alsa 目录", "删除因符号链接丢失导致的递归目录"},
        {"路径过长", "缩短目录名称", "将中文目录名改为英文缩短路径"},
        {"pkg-config 路径", "修复 .pc 文件路径", "重写 alsa-lib.pc 中的硬编码路径"},
        {"环境变量", "生成 env_setup.sh", "创建包含 CC/CXX/AR 等变量的环境脚本"},
        {"符号链接", "检测并报告", "检查源码中的符号链接是否丢失"},
        {"依赖检查", "检查缺失的库", "检查编译产物的动态库依赖"},
    };

    m_fixTable->setRowCount(entries.size());
    for (int i = 0; i < entries.size(); ++i) {
        m_fixTable->setItem(i, 0, new QTableWidgetItem("待检查"));
        m_fixTable->setItem(i, 1, new QTableWidgetItem(entries[i].problem));
        m_fixTable->setItem(i, 2, new QTableWidgetItem(entries[i].action));
        m_fixTable->setItem(i, 3, new QTableWidgetItem(entries[i].detail));
    }

    fixLayout->addWidget(m_fixTable);
    mainLayout->addWidget(fixGroup, 1);
}

void FixTab::runFixAll()
{
    m_fixAllBtn->setEnabled(false);
    m_fixAllBtn->setText("修复中...");
    m_statusLabel->setText("正在执行自动修复...");

    emit logOutput(">>> 开始自动修复...");

    SettingsManager *s = SettingsManager::instance();
    m_runner->setWorkingDirectory(s->projectPath());
    m_runner->start("bash \"" + s->fixScript() + "\" --all");
}

void FixTab::onFinished(int exitCode)
{
    m_fixAllBtn->setEnabled(true);
    m_fixAllBtn->setText("一键全部修复");

    if (exitCode == 0) {
        m_statusLabel->setText("修复完成");
        m_statusLabel->setStyleSheet("color: #a6e3a1;");
        emit logOutput(">>> 自动修复完成");
    } else {
        m_statusLabel->setText("修复过程中有错误");
        m_statusLabel->setStyleSheet("color: #f38ba8;");
        emit logError(">>> 自动修复过程中出现错误");
    }
}
