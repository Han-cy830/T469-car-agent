#ifndef BUILDTARGET_H
#define BUILDTARGET_H

#include <QString>
#include <QStringList>
#include <QVector>

struct BuildTarget {
    enum Status { Pending, Building, Success, Failed, Skipped };

    Status status;
    QString name;         // 目标名称
    QString scriptArg;    // build.sh 参数
    QStringList depends;  // 依赖
    QString description;  // 描述
    qint64 elapsedMs;     // 耗时

    BuildTarget() : status(Pending), elapsedMs(0) {}

    static QString statusText(Status s) {
        switch (s) {
        case Pending:  return "待编译";
        case Building: return "编译中";
        case Success:  return "完成";
        case Failed:   return "失败";
        case Skipped:  return "跳过";
        default:       return "未知";
        }
    }
};

using BuildTargets = QVector<BuildTarget>;

#endif // BUILDTARGET_H
