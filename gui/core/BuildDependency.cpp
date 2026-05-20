#include "BuildDependency.h"
#include <QSet>
#include <queue>

BuildDependency::BuildDependency()
{
    // 默认的 T469 编译依赖
    addTarget("zlib");
    addTarget("alsa-lib", {"zlib"});
    addTarget("alsa-utils", {"alsa-lib"});
    addTarget("qt");
    addTarget("car", {"qt", "alsa-lib"});
    addTarget("deploy", {"car", "alsa-utils"});
}

void BuildDependency::addTarget(const QString &name, const QStringList &deps)
{
    BuildNode node;
    node.name = name;
    node.dependencies = deps;
    node.completed = false;
    node.failed = false;
    m_nodes.append(node);
}

QStringList BuildDependency::buildOrder() const
{
    QStringList order;
    QSet<QString> visited;
    QSet<QString> inProgress;

    // 使用 Kahn 算法进行拓扑排序
    QMap<QString, int> inDegree;
    QMap<QString, QStringList> adjacency;

    for (const auto &node : m_nodes) {
        inDegree[node.name] = 0;
    }

    for (const auto &node : m_nodes) {
        for (const QString &dep : node.dependencies) {
            adjacency[dep].append(node.name);
            inDegree[node.name]++;
        }
    }

    // 找出入度为 0 的节点
    std::queue<QString> queue;
    for (auto it = inDegree.begin(); it != inDegree.end(); ++it) {
        if (it.value() == 0) {
            queue.push(it.key());
        }
    }

    while (!queue.empty()) {
        QString current = queue.front();
        queue.pop();
        order.append(current);

        for (const QString &neighbor : adjacency[current]) {
            inDegree[neighbor]--;
            if (inDegree[neighbor] == 0) {
                queue.push(neighbor);
            }
        }
    }

    return order;
}

void BuildDependency::markCompleted(const QString &name)
{
    int idx = nodeIndex(name);
    if (idx >= 0) {
        m_nodes[idx].completed = true;
        m_nodes[idx].failed = false;
    }
}

void BuildDependency::markFailed(const QString &name)
{
    int idx = nodeIndex(name);
    if (idx >= 0) {
        m_nodes[idx].failed = true;
    }
}

QString BuildDependency::nextTarget() const
{
    for (const auto &node : m_nodes) {
        if (node.completed || node.failed) continue;

        // 检查依赖是否都已完成
        bool depsOk = true;
        for (const QString &dep : node.dependencies) {
            int depIdx = nodeIndex(dep);
            if (depIdx < 0 || !m_nodes[depIdx].completed) {
                depsOk = false;
                break;
            }
        }

        if (depsOk) {
            return node.name;
        }
    }

    return QString();
}

bool BuildDependency::allCompleted() const
{
    for (const auto &node : m_nodes) {
        if (!node.completed) return false;
    }
    return true;
}

void BuildDependency::reset()
{
    for (auto &node : m_nodes) {
        node.completed = false;
        node.failed = false;
    }
}

int BuildDependency::nodeIndex(const QString &name) const
{
    for (int i = 0; i < m_nodes.size(); ++i) {
        if (m_nodes[i].name == name) return i;
    }
    return -1;
}
