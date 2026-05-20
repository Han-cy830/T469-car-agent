#include "OutputParser.h"
#include "utils/AnsiHtml.h"
#include <QRegularExpression>

QVector<ParsedLine> OutputParser::parseDiagnostic(const QString &output)
{
    QVector<ParsedLine> results;
    QStringList lines = AnsiHtml::strip(output).split('\n');

    for (const QString &line : lines) {
        QString trimmed = line.trimmed();
        if (trimmed.isEmpty()) continue;

        ParsedLine parsed;
        parsed.type = ParsedLine::Plain;

        if (trimmed.startsWith("[✓]") || trimmed.startsWith("[PASS]")) {
            parsed.type = ParsedLine::Pass;
            parsed.content = trimmed.mid(trimmed.indexOf(']') + 1).trimmed();
        } else if (trimmed.startsWith("[✗]") || trimmed.startsWith("[FAIL]")) {
            parsed.type = ParsedLine::Fail;
            parsed.content = trimmed.mid(trimmed.indexOf(']') + 1).trimmed();
        } else if (trimmed.startsWith("[!]") || trimmed.startsWith("[WARN]")) {
            parsed.type = ParsedLine::Warn;
            parsed.content = trimmed.mid(trimmed.indexOf(']') + 1).trimmed();
        } else if (trimmed.startsWith("[i]") || trimmed.startsWith("[INFO]")) {
            parsed.type = ParsedLine::Info;
            parsed.content = trimmed.mid(trimmed.indexOf(']') + 1).trimmed();
        } else {
            parsed.content = trimmed;
        }

        // 提取类别和详情
        if (parsed.content.contains(':')) {
            int pos = parsed.content.indexOf(':');
            parsed.category = parsed.content.left(pos).trimmed();
            parsed.detail = parsed.content.mid(pos + 1).trimmed();
        } else if (parsed.content.contains('：')) {
            int pos = parsed.content.indexOf('：');
            parsed.category = parsed.content.left(pos).trimmed();
            parsed.detail = parsed.content.mid(pos + 2).trimmed();
        }

        results.append(parsed);
    }

    return results;
}

QString OutputParser::parseBuildProgress(const QString &line)
{
    // 匹配 "Building XXX..." 或 "make[1]: Entering directory"
    static QRegularExpression re("Building\\s+(\\S+)");
    QRegularExpressionMatch match = re.match(line);
    if (match.hasMatch()) {
        return match.captured(1);
    }
    return QString();
}

QStringList OutputParser::extractErrors(const QString &output)
{
    QStringList errors;
    QStringList lines = AnsiHtml::strip(output).split('\n');

    for (const QString &line : lines) {
        QString trimmed = line.trimmed();
        if (trimmed.contains("error:", Qt::CaseInsensitive) ||
            trimmed.contains("Error:", Qt::CaseInsensitive) ||
            trimmed.contains("fatal:", Qt::CaseInsensitive) ||
            trimmed.contains("undefined reference") ||
            trimmed.contains("No such file")) {
            errors.append(trimmed);
        }
    }

    return errors;
}
