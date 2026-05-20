#ifndef DIAGNOSTICRESULT_H
#define DIAGNOSTICRESULT_H

#include <QString>
#include <QVector>

struct DiagnosticResult {
    enum Status { Pass, Fail, Warning, Info, Unknown };

    Status status;
    QString name;        // 检查项名称
    QString detail;      // 详细信息
    QString suggestion;  // 修复建议
    QString rawLine;     // 原始输出行

    DiagnosticResult() : status(Unknown) {}

    static Status fromMarker(const QString &marker) {
        if (marker == "[✓]" || marker == "[PASS]") return Pass;
        if (marker == "[✗]" || marker == "[FAIL]") return Fail;
        if (marker == "[!]" || marker == "[WARN]") return Warning;
        if (marker == "[i]" || marker == "[INFO]") return Info;
        return Unknown;
    }

    static QString statusText(Status s) {
        switch (s) {
        case Pass:    return "通过";
        case Fail:    return "失败";
        case Warning: return "警告";
        case Info:    return "信息";
        default:      return "未知";
        }
    }
};

using DiagnosticResults = QVector<DiagnosticResult>;

#endif // DIAGNOSTICRESULT_H
