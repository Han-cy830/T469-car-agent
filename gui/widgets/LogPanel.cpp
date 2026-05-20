#include "LogPanel.h"
#include "utils/AnsiHtml.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QClipboard>
#include <QApplication>

LogPanel::LogPanel(QWidget *parent)
    : QWidget(parent)
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 标题栏
    auto *headerLayout = new QHBoxLayout();
    headerLayout->setContentsMargins(8, 4, 8, 4);

    auto *titleLabel = new QLabel("输出日志");
    titleLabel->setObjectName("subtitleLabel");
    headerLayout->addWidget(titleLabel);

    headerLayout->addStretch();

    m_copyBtn = new QPushButton("复制");
    m_copyBtn->setMaximumHeight(24);
    connect(m_copyBtn, &QPushButton::clicked, this, [this]() {
        QApplication::clipboard()->setText(toPlainText());
    });
    headerLayout->addWidget(m_copyBtn);

    m_clearBtn = new QPushButton("清空");
    m_clearBtn->setMaximumHeight(24);
    connect(m_clearBtn, &QPushButton::clicked, this, &LogPanel::clear);
    headerLayout->addWidget(m_clearBtn);

    mainLayout->addLayout(headerLayout);

    // 分隔线
    auto *separator = new QWidget();
    separator->setFixedHeight(1);
    separator->setStyleSheet("background-color: #45475a;");
    mainLayout->addWidget(separator);

    // 日志文本区
    m_textEdit = new QTextEdit();
    m_textEdit->setReadOnly(true);
    m_textEdit->setAcceptRichText(true);
    m_textEdit->setMinimumHeight(100);
    mainLayout->addWidget(m_textEdit);
}

void LogPanel::appendLog(const QString &text)
{
    m_textEdit->append(AnsiHtml::toHtml(text));
    // 自动滚动到底部
    QScrollBar *sb = m_textEdit->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void LogPanel::appendError(const QString &text)
{
    QString html = "<span style=\"color:#f38ba8;\">" + text.toHtmlEscaped() + "</span>";
    m_textEdit->append(html);
    QScrollBar *sb = m_textEdit->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void LogPanel::appendInfo(const QString &text)
{
    QString html = "<span style=\"color:#89b4fa;\">[i] " + text.toHtmlEscaped() + "</span>";
    m_textEdit->append(html);
    QScrollBar *sb = m_textEdit->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void LogPanel::appendSuccess(const QString &text)
{
    QString html = "<span style=\"color:#a6e3a1;\">[✓] " + text.toHtmlEscaped() + "</span>";
    m_textEdit->append(html);
    QScrollBar *sb = m_textEdit->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void LogPanel::appendWarning(const QString &text)
{
    QString html = "<span style=\"color:#f9e2af;\">[!] " + text.toHtmlEscaped() + "</span>";
    m_textEdit->append(html);
    QScrollBar *sb = m_textEdit->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void LogPanel::clear()
{
    m_textEdit->clear();
}

QString LogPanel::toPlainText() const
{
    return m_textEdit->toPlainText();
}

void LogPanel::onStdoutReady(const QString &text)
{
    appendLog(text);
}

void LogPanel::onStderrReady(const QString &text)
{
    appendError(text);
}
