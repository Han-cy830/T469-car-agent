#include "SidebarWidget.h"
#include <QLabel>
#include <QSizePolicy>

SidebarWidget::SidebarWidget(QWidget *parent)
    : QWidget(parent)
    , m_buttonGroup(new QButtonGroup(this))
{
    setFixedWidth(180);
    setObjectName("sidebar");

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 12, 8, 12);
    layout->setSpacing(4);

    // Logo / 标题
    auto *titleLabel = new QLabel("T469 Agent");
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    auto *subtitleLabel = new QLabel("车载交叉编译助手");
    subtitleLabel->setObjectName("subtitleLabel");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(subtitleLabel);

    layout->addSpacing(20);

    // 导航按钮
    QStringList pages = {"概览", "诊断", "修复", "编译", "部署", "设置"};
    for (int i = 0; i < pages.size(); ++i) {
        auto *btn = createNavButton(pages[i], i);
        m_buttonGroup->addButton(btn, i);
        layout->addWidget(btn);
    }

    layout->addStretch();

    // 版本信息
    auto *versionLabel = new QLabel("v1.0.0");
    versionLabel->setAlignment(Qt::AlignCenter);
    versionLabel->setStyleSheet("color: #585b70; font-size: 11px;");
    layout->addWidget(versionLabel);

    // 连接信号
    connect(m_buttonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked),
            this, &SidebarWidget::pageChanged);

    // 默认选中第一个
    m_buttonGroup->button(0)->setChecked(true);
}

QPushButton *SidebarWidget::createNavButton(const QString &text, int index)
{
    auto *btn = new QPushButton(text);
    btn->setCheckable(true);
    btn->setMinimumHeight(36);
    btn->setCursor(Qt::PointingHandCursor);

    // 图标字符映射（使用 Unicode 符号代替图标）
    static QStringList icons = {"📊", "🔍", "🔧", "🔨", "🚀", "⚙"};
    if (index < icons.size()) {
        btn->setText(icons[index] + "  " + text);
    }

    return btn;
}
