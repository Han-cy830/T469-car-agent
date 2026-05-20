#ifndef BUILDDEPENDENCY_H
#define BUILDDEPENDENCY_H

#include <QString>
#include <QStringList>
#include <QVector>
#include <QMap>

struct BuildNode {
    QString name;
    QStringList dependencies;
    bool completed;
    bool failed;
};

class BuildDependency
{
public:
    BuildDependency();

    // 添加编译目标
    void addTarget(const QString &name, const QStringList &deps = QStringList());

    // 获取编译顺序（拓扑排序）
    QStringList buildOrder() const;

    // 标记完成
    void markCompleted(const QString &name);
    void markFailed(const QString &name);

    // 获取下一个可编译的目标
    QString nextTarget() const;

    // 是否全部完成
    bool allCompleted() const;

    // 重置
    void reset();

private:
    QVector<BuildNode> m_nodes;
    int nodeIndex(const QString &name) const;
};

#endif // BUILDDEPENDENCY_H
