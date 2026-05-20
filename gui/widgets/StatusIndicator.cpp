#include "StatusIndicator.h"
#include <QPainter>

StatusIndicator::StatusIndicator(QWidget *parent)
    : QWidget(parent)
    , m_status(Unknown)
{
    setFixedSize(16, 16);
}

StatusIndicator::Status StatusIndicator::status() const
{
    return m_status;
}

void StatusIndicator::setStatus(Status status)
{
    if (m_status != status) {
        m_status = status;
        update();
    }
}

QSize StatusIndicator::sizeHint() const
{
    return QSize(16, 16);
}

QColor StatusIndicator::statusColor() const
{
    switch (m_status) {
    case Pass:     return QColor("#a6e3a1");
    case Fail:     return QColor("#f38ba8");
    case Warning:  return QColor("#f9e2af");
    case Info:     return QColor("#89b4fa");
    case Running:  return QColor("#89dceb");
    default:       return QColor("#585b70");
    }
}

void StatusIndicator::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QColor color = statusColor();

    // 外圈发光效果
    if (m_status == Running) {
        QColor glow = color;
        glow.setAlpha(40);
        p.setBrush(glow);
        p.setPen(Qt::NoPen);
        p.drawEllipse(0, 0, width(), height());
    }

    // 实心圆
    p.setBrush(color);
    p.setPen(Qt::NoPen);
    p.drawEllipse(2, 2, width() - 4, height() - 4);

    // 内部高光
    QColor highlight = Qt::white;
    highlight.setAlpha(60);
    p.setBrush(highlight);
    p.drawEllipse(4, 3, 4, 4);
}
