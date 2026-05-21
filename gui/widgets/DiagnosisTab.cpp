#include "DiagnosisTab.h"
#include "StatusIndicator.h"
#include "utils/ShellRunner.h"
#include "utils/AnsiHtml.h"
#include "core/OutputParser.h"
#include "core/SettingsManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>

DiagnosisTab::DiagnosisTab(QWidget *parent)
    : QWidget(parent), m_runner(new ShellRunner(this)), m_passCount(0), m_failCount(0), m_warnCount(0)
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

    auto *titleLabel = new QLabel("环境诊断");
    titleLabel->setObjectName("titleLabel");
    mainLayout->addWidget(titleLabel);

    auto *subtitleLabel = new QLabel("自动检测交叉编译环境中的常见问题");
    subtitleLabel->setObjectName("subtitleLabel");
    mainLayout->addWidget(subtitleLabel);

    auto *controlLayout = new QHBoxLayout();
    m_runBtn = new QPushButton("开始诊断");
    m_runBtn->setObjectName("primaryButton");
    m_runBtn->setMinimumSize(140, 40);
    m_runBtn->setCursor(Qt::PointingHandCursor);
    connect(m_runBtn, &QPushButton::clicked, this, &DiagnosisTab::runDiagnosis);
    controlLayout->addWidget(m_runBtn);

    m_progress = new QProgressBar();
    m_progress->setRange(0, 0);
    m_progress->setVisible(false);
    m_progress->setMaximumHeight(8);
    controlLayout->addWidget(m_progress, 1);

    m_summaryLabel = new QLabel("");
    m_summaryLabel->setObjectName("subtitleLabel");
    controlLayout->addWidget(m_summaryLabel);
    controlLayout->addStretch();
    mainLayout->addLayout(controlLayout);

    m_resultTable = new QTableWidget();
    m_resultTable->setColumnCount(4);
    m_resultTable->setHorizontalHeaderLabels({"状态", "检查项", "结果", "建议"});
    m_resultTable->horizontalHeader()->setStretchLastSection(true);
    m_resultTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_resultTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_resultTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_resultTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_resultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
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
    m_runner->setWorkingDirectory(s->projectPath());
    m_runner->start("bash \"" + s->diagnoseScript() + "\"");
}

void DiagnosisTab::onStdout(const QString &text) { m_outputBuffer += text; emit logOutput(text); }
void DiagnosisTab::onStderr(const QString &text) { emit logError(text); }

void DiagnosisTab::onFinished(int)
{
    m_runBtn->setEnabled(true);
    m_runBtn->setText("开始诊断");
    m_progress->setVisible(false);
    parseResults(m_outputBuffer);
    QString summary = QString("通过: %1  失败: %2  警告: %3").arg(m_passCount).arg(m_failCount).arg(m_warnCount);
    m_summaryLabel->setText(summary);
    emit logOutput(QString(">>> 诊断完成。%1").arg(summary));
    emit diagnosisComplete(m_passCount, m_failCount, m_warnCount);
}

void DiagnosisTab::parseResults(const QString &output)
{
    QVector<ParsedLine> lines = OutputParser::parseDiagnostic(output);
    for (const ParsedLine &pl : lines) {
        if (pl.type == ParsedLine::Plain) continue;

        StatusIndicator::Status status = StatusIndicator::Unknown;
        switch (pl.type) {
        case ParsedLine::Pass: status = StatusIndicator::Pass; m_passCount++; break;
        case ParsedLine::Fail: status = StatusIndicator::Fail; m_failCount++; break;
        case ParsedLine::Warn: status = StatusIndicator::Warning; m_warnCount++; break;
        case ParsedLine::Info: status = StatusIndicator::Info; break;
        default: break;
        }

        int row = m_resultTable->rowCount();
        m_resultTable->insertRow(row);
        auto *indicator = new StatusIndicator();
        indicator->setStatus(status);
        m_resultTable->setCellWidget(row, 0, indicator);
        m_resultTable->setItem(row, 1, new QTableWidgetItem(pl.category));
        m_resultTable->setItem(row, 2, new QTableWidgetItem(pl.detail));
        m_resultTable->setItem(row, 3, new QTableWidgetItem(pl.suggestion));

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
