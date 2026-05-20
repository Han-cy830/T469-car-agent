#include "DiagnosisTab.h"
#include "StatusIndicator.h"
#include "utils/ShellRunner.h"
#include "utils/AnsiHtml.h"
#include "core/SettingsManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QGroupBox>
#include <QScrollBar>

DiagnosisTab::DiagnosisTab(QWidget *parent)
    : QWidget(parent)
    , m_runner(new ShellRunner(this))
    , m_passCount(0)
    , m_failCount(0)
    , m_warnCount(0)
{
    setupUi();

    connect(m_runner, &ShellRunner::outputReady, this, &DiagnosisTab::onStdout);
    connect(m_runner, &ShellRunner::errorReady, this, &DiagnosisTab::onStderr);
    connect(m_runner, &ShellRunner::finished, this, &DiagnosisTab::onFinished);
}

void DiagnosisTab::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(16);

    // 标题
    auto *titleLabel = new QLabel("环境诊断");
    titleLabel->setObjectName("titleLabel");
    mainLayout->addWidget(titleLabel);

    auto *subtitleLabel = new QLabel("自动检测交叉编译环境中的常见问题");
    subtitleLabel->setObjectName("subtitleLabel");
    mainLayout->addWidget(subtitleLabel);

    // 控制栏
    auto *controlLayout = new QHBoxLayout();

    m_runBtn = new QPushButton("开始诊断");
    m_runBtn->setObjectName("primaryButton");
    m_runBtn->setMinimumSize(140, 40);
    m_runBtn->setCursor(Qt::PointingHandCursor);
    connect(m_runBtn, &QPushButton::clicked, this, &DiagnosisTab::runDiagnosis);
    controlLayout->addWidget(m_runBtn);

    m_progress = new QProgressBar();
    m_progress->setRange(0, 0); // 不确定进度模式
    m_progress->setVisible(false);
    m_progress->setMaximumHeight(8);
    controlLayout->addWidget(m_progress, 1);

    m_summaryLabel = new QLabel("");
    m_summaryLabel->setObjectName("subtitleLabel");
    controlLayout->addWidget(m_summaryLabel);

    controlLayout->addStretch();
    mainLayout->addLayout(controlLayout);

    // 结果表格
    m_resultTable = new QTableWidget();
    m_resultTable->setColumnCount(4);
    m_resultTable->setHorizontalHeaderLabels({"状态", "检查项", "结果", "建议"});
    m_resultTable->horizontalHeader()->setStretchLastSection(true);
    m_resultTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_resultTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_resultTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_resultTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_resultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_resultTable->setAlternatingRowColors(false);
    m_resultTable->verticalHeader()->setVisible(false);
    m_resultTable->setShowGrid(false);
    mainLayout->addWidget(m_resultTable, 1);
}

void DiagnosisTab::runDiagnosis()
{
    m_resultTable->setRowCount(0);
    m_outputBuffer.clear();
    m_passCount = m_failCount = m_warnCount = 0;

    m_runBtn->setEnabled(false);
    m_runBtn->setText("诊断中...");
    m_progress->setVisible(true);
    m_summaryLabel->setText("正在运行诊断脚本...");

    emit logOutput(">>> 开始环境诊断...");

    SettingsManager *s = SettingsManager::instance();
    QString script = s->diagnoseScript();

    m_runner->setWorkingDirectory(s->projectPath());
    m_runner->start("bash \"" + script + "\"");
}

void DiagnosisTab::onStdout(const QString &text)
{
    m_outputBuffer += text;
    emit logOutput(text);
}

void DiagnosisTab::onStderr(const QString &text)
{
    emit logError(text);
}

void DiagnosisTab::onFinished(int exitCode)
{
    m_runBtn->setEnabled(true);
    m_runBtn->setText("开始诊断");
    m_progress->setVisible(false);

    parseResults(m_outputBuffer);

    QString summary = QString("通过: %1  失败: %2  警告: %3")
        .arg(m_passCount).arg(m_failCount).arg(m_warnCount);
    m_summaryLabel->setText(summary);

    emit logOutput(QString(">>> 诊断完成。%1").arg(summary));
    emit diagnosisComplete(m_passCount, m_failCount, m_warnCount);
}

void DiagnosisTab::parseResults(const QString &output)
{
    QStringList lines = AnsiHtml::strip(output).split('\n');

    for (const QString &line : lines) {
        QString trimmed = line.trimmed();
        if (trimmed.isEmpty()) continue;

        StatusIndicator::Status status = StatusIndicator::Unknown;
        QString checkName;
        QString result;
        QString suggestion;

        if (trimmed.startsWith("[✓]") || trimmed.startsWith("[PASS]")) {
            status = StatusIndicator::Pass;
            m_passCount++;
            result = trimmed.mid(trimmed.indexOf(']') + 1).trimmed();
        } else if (trimmed.startsWith("[✗]") || trimmed.startsWith("[FAIL]")) {
            status = StatusIndicator::Fail;
            m_failCount++;
            result = trimmed.mid(trimmed.indexOf(']') + 1).trimmed();
        } else if (trimmed.startsWith("[!]") || trimmed.startsWith("[WARN]")) {
            status = StatusIndicator::Warning;
            m_warnCount++;
            result = trimmed.mid(trimmed.indexOf(']') + 1).trimmed();
        } else if (trimmed.startsWith("[i]") || trimmed.startsWith("[INFO]")) {
            status = StatusIndicator::Info;
            result = trimmed.mid(trimmed.indexOf(']') + 1).trimmed();
        } else {
            continue; // 跳过非标记行
        }

        // 尝试提取检查项名称（冒号前的部分）
        if (result.contains(':')) {
            int colonPos = result.indexOf(':');
            checkName = result.left(colonPos).trimmed();
            result = result.mid(colonPos + 1).trimmed();
        } else if (result.contains('：')) {
            int colonPos = result.indexOf('：');
            checkName = result.left(colonPos).trimmed();
            result = result.mid(colonPos + 2).trimmed();
        } else {
            checkName = result;
        }

        int row = m_resultTable->rowCount();
        m_resultTable->insertRow(row);

        // 状态指示灯
        auto *indicator = new StatusIndicator();
        indicator->setStatus(status);
        m_resultTable->setCellWidget(row, 0, indicator);

        m_resultTable->setItem(row, 1, new QTableWidgetItem(checkName));
        m_resultTable->setItem(row, 2, new QTableWidgetItem(result));
        m_resultTable->setItem(row, 3, new QTableWidgetItem(suggestion));

        // 根据状态设置行颜色
        QColor rowColor;
        switch (status) {
        case StatusIndicator::Pass:    rowColor = QColor("#a6e3a1"); break;
        case StatusIndicator::Fail:    rowColor = QColor("#f38ba8"); break;
        case StatusIndicator::Warning: rowColor = QColor("#f9e2af"); break;
        default:                       rowColor = QColor("#cdd6f4"); break;
        }

        for (int col = 1; col < 4; ++col) {
            if (m_resultTable->item(row, col))
                m_resultTable->item(row, col)->setForeground(rowColor);
        }
    }
}
