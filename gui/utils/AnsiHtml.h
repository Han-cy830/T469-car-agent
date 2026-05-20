#ifndef ANSIHTML_H
#define ANSIHTML_H

#include <QString>

class AnsiHtml
{
public:
    // 将 ANSI 转义序列转换为 HTML
    static QString toHtml(const QString &text);

    // 去除 ANSI 转义序列
    static QString strip(const QString &text);

    // 将 ANSI 文本转换为纯文本（保留内容，去除转义码）
    static QString toPlainText(const QString &text);
};

#endif // ANSIHTML_H
