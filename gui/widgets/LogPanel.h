#ifndef LOGPANEL_H
#define LOGPANEL_H

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QScrollBar>

class LogPanel : public QWidget
{
    Q_OBJECT

public:
    explicit LogPanel(QWidget *parent = nullptr);

    // 添加日志（支持 ANSI 彩色）
    void appendLog(const QString &text);
    void appendError(const QString &text);
    void appendInfo(const QString &text);
    void appendSuccess(const QString &text);
    void appendWarning(const QString &text);

    // 清空日志
    void clear();

    // 获取纯文本内容
    QString toPlainText() const;

public slots:
    void onStdoutReady(const QString &text);
    void onStderrReady(const QString &text);

private:
    QTextEdit *m_textEdit;
    QPushButton *m_clearBtn;
    QPushButton *m_copyBtn;
};

#endif // LOGPANEL_H
