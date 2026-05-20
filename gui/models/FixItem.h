#ifndef FIXITEM_H
#define FIXITEM_H

#include <QString>
#include <QVector>

struct FixItem {
    enum Status { Pending, Running, Success, Failed, Skipped };

    Status status;
    QString problem;      // 问题描述
    QString action;       // 修复操作
    QString detail;       // 详细说明
    QString scriptArg;    // 对应 fix.sh 的参数

    FixItem() : status(Pending) {}

    static QString statusText(Status s) {
        switch (s) {
        case Pending: return "待处理";
        case Running: return "修复中";
        case Success: return "已修复";
        case Failed:  return "失败";
        case Skipped: return "跳过";
        default:      return "未知";
        }
    }
};

using FixItems = QVector<FixItem>;

#endif // FIXITEM_H
