#ifndef STATUSINDICATOR_H
#define STATUSINDICATOR_H

#include <QWidget>

class StatusIndicator : public QWidget
{
    Q_OBJECT

public:
    enum Status {
        Unknown,
        Pass,
        Fail,
        Warning,
        Info,
        Running
    };

    explicit StatusIndicator(QWidget *parent = nullptr);

    Status status() const;
    void setStatus(Status status);

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Status m_status;
    QColor statusColor() const;
};

#endif // STATUSINDICATOR_H
