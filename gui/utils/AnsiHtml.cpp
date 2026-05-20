#include "AnsiHtml.h"
#include <QRegularExpression>
#include <QMap>

// ANSI 颜色映射
static QMap<int, QString> initColorMap()
{
    QMap<int, QString> map;
    // 标准色
    map[30] = "#45475a";  // 黑 → 深灰
    map[31] = "#f38ba8";  // 红
    map[32] = "#a6e3a1";  // 绿
    map[33] = "#f9e2af";  // 黄
    map[34] = "#89b4fa";  // 蓝
    map[35] = "#f5c2e7";  // 品红
    map[36] = "#94e2d5";  // 青
    map[37] = "#bac2de";  // 白 → 浅灰
    // 亮色
    map[90] = "#585b70";
    map[91] = "#f38ba8";
    map[92] = "#a6e3a1";
    map[93] = "#f9e2af";
    map[94] = "#89b4fa";
    map[95] = "#f5c2e7";
    map[96] = "#94e2d5";
    map[97] = "#cdd6f4";
    return map;
}

static QMap<int, QString> s_colorMap = initColorMap();

QString AnsiHtml::toHtml(const QString &text)
{
    // 匹配 ANSI 转义序列: ESC[ ... m
    static QRegularExpression re("\x1b\\[([0-9;]*)m");

    QString result;
    int lastEnd = 0;
    bool bold = false;
    QString fgColor;

    QRegularExpressionMatchIterator it = re.globalMatch(text);

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();

        // 添加转义序列之前的文本
        QString before = text.mid(lastEnd, match.capturedStart() - lastEnd);
        if (!before.isEmpty()) {
            // HTML 转义
            before.replace("&", "&amp;");
            before.replace("<", "&lt;");
            before.replace(">", "&gt;");

            if (!fgColor.isEmpty() || bold) {
                QString style;
                if (bold) style += "font-weight:bold;";
                if (!fgColor.isEmpty()) style += "color:" + fgColor + ";";
                result += "<span style=\"" + style + "\">" + before + "</span>";
            } else {
                result += before;
            }
        }

        // 解析 SGR 参数
        QString params = match.captured(1);
        if (params.isEmpty()) params = "0";

        QStringList codes = params.split(";");
        for (const QString &code : codes) {
            int c = code.toInt();
            if (c == 0) {
                // 重置
                bold = false;
                fgColor.clear();
            } else if (c == 1) {
                bold = true;
            } else if (c == 22) {
                bold = false;
            } else if (s_colorMap.contains(c)) {
                fgColor = s_colorMap[c];
            }
        }

        lastEnd = match.capturedEnd();
    }

    // 添加剩余文本
    QString remaining = text.mid(lastEnd);
    if (!remaining.isEmpty()) {
        remaining.replace("&", "&amp;");
        remaining.replace("<", "&lt;");
        remaining.replace(">", "&gt;");

        if (!fgColor.isEmpty() || bold) {
            QString style;
            if (bold) style += "font-weight:bold;";
            if (!fgColor.isEmpty()) style += "color:" + fgColor + ";";
            result += "<span style=\"" + style + "\">" + remaining + "</span>";
        } else {
            result += remaining;
        }
    }

    // 将换行符转换为 <br>
    result.replace("\n", "<br>");

    return result;
}

QString AnsiHtml::strip(const QString &text)
{
    static QRegularExpression re("\x1b\\[[0-9;]*m");
    QString result = text;
    result.replace(re, QString());
    return result;
}

QString AnsiHtml::toPlainText(const QString &text)
{
    static QRegularExpression re("\x1b\\[[0-9;]*m");
    QString result = text;
    result.replace(re, QString());
    return result;
}
