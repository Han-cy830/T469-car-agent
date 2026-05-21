#include "BuildTab.h"
#include "utils/ShellRunner.h"
#include "core/SettingsManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QGroupBox>

BuildTab::BuildTab(QWidget *parent)
    : QWidget(parent)
    , m_runner(new ShellRunner(this))
{
    setupUi();
    connect(m_runner, &ShellRunner::outputReady, this, &BuildTab::onStdout);
    connect(m_runner, &ShellRunner::errorReady, this, &BuildTab::onStderr);
    connect(m_runner, &ShellRunner::finished, this, &BuildTab::onFinished);
}

void BuildTab::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(16);

    // 标题
    auto *titleLabel = new QLabel("交叉编译");
    titleLabel->setObjectName("titleLabel");
    mainLayout->addWidget(titleLabel);

    auto *subtitleLabel = new QLabel("按依赖顺序编译各组件");
    subtitleLabel->setObjectName("subtitleLabel");
    mainLayout->addWidget(subtitleLabel);

    // 控制栏
    auto *controlLayout = new QHBoxLayout();

    m_targetCombo = new QComboBox();
    m_targetCombo->addItem("全部 (all)", "all");
    m_targetCombo->addItem("Qt (qt)", "qt");
    m_targetCombo->addItem("ALSA (alsa)", "alsa");
    m_targetCombo->addItem("车载应用 (car)", "car");
    m_targetCombo->addItem("部署包 (deploy)", "deploy");
    m_targetCombo->setMinimumWidth(160);
    controlLayout->addWidget(m_targetCombo);

    m_buildBtn = new QPushButton("开始编译");
    m_buildBtn->setObjectName("primaryButton");
    m_buildBtn->setMinimumSize(120, 40);
    m_buildBtn->setCursor(Qt::PointingHandCursor);
    connect(m_buildBtn, &QPushButton::clicked, this, &BuildTab::startBuild);
    controlLayout->addWidget(m_buildBtn);

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

    // 编译目标表格
    auto *targetGroup = new QGroupBox("编译目标");
    auto *targetLayout = new QVBoxLayout(targetGroup);

    m_targetTable = new QTableWidget();
    m_targetTable->setColumnCount(4);
    m_targetTable->setHorizontalHeaderLabels({"状态", "目标", "依赖", "说明"});
    m_targetTable->horizontalHeader()->setStretchLastSection(true);
    m_targetTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_targetTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_targetTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_targetTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_targetTable->verticalHeader()->setVisible(false);
    m_targetTable->setShowGrid(false);

    struct TargetEntry {
        QString name;
        QString deps;
        QString desc;
    };
    QVector<TargetEntry> targets = {
        {"zlib", "无", "压缩库，基础依赖"},
        {"alsa-lib", "zlib", "ALSA 音频库"},
        {"alsa-utils", "alsa-lib", "ALSA 工具集"},
        {"Qt 5.12.12", "无", "Qt 框架（约2小时）"},
        {"car-app", "Qt, alsa-lib", "车载应用"},
        {"deploy", "全部", "打包部署目录"},
    };

    m_targetTable->setRowCount(targets.size());
    for (int i = 0; i < targets.size(); ++i) {
        m_targetTable->setItem(i, 0, new QTableWidgetItem("待编译"));
        m_targetTable->setItem(i, 1, new QTableWidgetItem(targets[i].name));
        m_targetTable->setItem(i, 2, new QTableWidgetItem(targets[i].deps));
        m_targetTable->setItem(i, 3, new QTableWidgetItem(targets[i].desc));
    }

    targetLayout->addWidget(m_targetTable);
    mainLayout->addWidget(targetGroup, 1);
}

void BuildTab::startBuild()
{
    m_outputBuffer.clear();
    m_buildBtn->setEnabled(false);
    m_buildBtn->setText("编译中...");
    m_progress->setVisible(true);

    QString target = m_targetCombo->currentData().toString();
    if (target.isEmpty()) target = "all";

    m_statusLabel->setText("正在编译...");
    m_statusLabel->setStyleSheet("color: #89b4fa;");

    emit logOutput(QString(">>> 开始编译目标: %1").arg(target));

    SettingsManager *s = SettingsManager::instance();
    m_runner->setWorkingDirectory(s->projectPath());
    m_runner->start("bash \"" + s->buildScript() + "\" " + target);
}

void BuildTab::onStdout(const QString &text)
{
    m_outputBuffer += text;
    emit logOutput(text);

    // 解析进度
    QString plain = text.trimmed();
    if (plain.contains("Building")) {
        // 更新对应目标状态
        for (int i = 0; i < m_targetTable->rowCount(); ++i) {
            QString name = m_targetTable->item(i, 1)->text().toLower();
            if (plain.toLower().contains(name)) {
                updateTargetStatus(i, "编译中", "#89b4fa");
            }
        }
    }
}

void BuildTab::onStderr(const QString &text)
{
    emit logError(text);
}

void BuildTab::onFinished(int exitCode)
{
    m_buildBtn->setEnabled(true);
    m_buildBtn->setText("开始编译");
    m_progress->setVisible(false);

    if (exitCode == 0) {
        m_statusLabel->setText("编译完成");
        m_statusLabel->setStyleSheet("color: #a6e3a1;");
        emit logOutput(">>> 编译完成");
    } else {
        m_statusLabel->setText("编译失败");
        m_statusLabel->setStyleSheet("color: #f38ba8;");
        emit logError(">>> 编译失败");
    }
}

void BuildTab::updateTargetStatus(int index, const QString &status, const QString &color)
{
    if (index >= 0 && index < m_targetTable->rowCount()) {
        QTableWidgetItem *item = m_targetTable->item(index, 0);
        if (item) {
            item->setText(status);
            item->setForeground(QColor(color));
        }
    }
}
