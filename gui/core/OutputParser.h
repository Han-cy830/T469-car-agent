#ifndef OUTPUTPARSER_H
#define OUTPUTPARSER_H

#include <QString>
#include <QStringList>
#include <QVector>

struct ParsedLine {
    enum Type { Pass, Fail, Warn, Info, Plain };
    Type type;
    QString content;
    QString category;
    QString detail;
};

class OutputParser
{
public:
    // 解析 diagnose.sh 输出
    static QVector<ParsedLine> parseDiagnostic(const QString &output);

    // 解析 build.sh 输出中的进度
    static QString parseBuildProgress(const QString &line);

    // 提取错误信息
    static QStringList extractErrors(const QString &output);
};

#endif // OUTPUTPARSER_H
