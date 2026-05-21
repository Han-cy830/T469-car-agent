#include "MainWindow.h"
#include "SidebarWidget.h"
#include "LogPanel.h"
#include "DashboardTab.h"
#include "DiagnosisTab.h"
#include "FixTab.h"
#include "BuildTab.h"
#include "DeployTab.h"
#include "SettingsTab.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStatusBar>
#include <QStringList>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    setWindowTitle("T469 车载交叉编译助手");
    resize(1200, 800);
    setMinimumSize(900, 600);

    // 状态栏
    statusBar()->showMessage("就绪");
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    auto *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 主体：侧栏 + 内容区
    auto *contentSplitter = new QSplitter(Qt::Horizontal);
    contentSplitter->setChildrenCollapsible(false);

    // 左侧导航
    m_sidebar = new SidebarWidget();
    contentSplitter->addWidget(m_sidebar);

    // 右侧区域：页面 + 日志
    auto *rightWidget = new QWidget();
    auto *rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    // 页面栈
    m_stack = new QStackedWidget();

    m_dashboardTab = new DashboardTab();
    m_diagnosisTab = new DiagnosisTab();
    m_fixTab = new FixTab();
    m_buildTab = new BuildTab();
    m_deployTab = new DeployTab();
    m_settingsTab = new SettingsTab();

    m_stack->addWidget(m_dashboardTab);
    m_stack->addWidget(m_diagnosisTab);
    m_stack->addWidget(m_fixTab);
    m_stack->addWidget(m_buildTab);
    m_stack->addWidget(m_deployTab);
    m_stack->addWidget(m_settingsTab);

    rightLayout->addWidget(m_stack, 1);

    // 日志面板
    m_logPanel = new LogPanel();
    m_logPanel->setMinimumHeight(120);

    auto *logSplitter = new QSplitter(Qt::Vertical);
    logSplitter->setChildrenCollapsible(false);

    // 用一个容器包裹 stack 以便 splitter 使用
    auto *stackContainer = new QWidget();
    auto *stackLayout = new QVBoxLayout(stackContainer);
    stackLayout->setContentsMargins(0, 0, 0, 0);
    stackLayout->addWidget(m_stack);
    logSplitter->addWidget(stackContainer);
    logSplitter->addWidget(m_logPanel);
    logSplitter->setStretchFactor(0, 3);
    logSplitter->setStretchFactor(1, 1);

    rightLayout->addWidget(logSplitter);

    contentSplitter->addWidget(rightWidget);
    contentSplitter->setStretchFactor(0, 0);
    contentSplitter->setStretchFactor(1, 1);

    mainLayout->addWidget(contentSplitter);

    // 连接导航信号
    connect(m_sidebar, &SidebarWidget::pageChanged, this, &MainWindow::switchPage);
    connect(m_dashboardTab, &DashboardTab::navigateToPage, this, &MainWindow::switchPage);

    // 连接各页面的日志输出到日志面板
    connect(m_diagnosisTab, &DiagnosisTab::logOutput, m_logPanel, &LogPanel::appendLog);
    connect(m_diagnosisTab, &DiagnosisTab::logError, m_logPanel, &LogPanel::appendError);
    connect(m_fixTab, &FixTab::logOutput, m_logPanel, &LogPanel::appendLog);
    connect(m_fixTab, &FixTab::logError, m_logPanel, &LogPanel::appendError);
    connect(m_buildTab, &BuildTab::logOutput, m_logPanel, &LogPanel::appendLog);
    connect(m_buildTab, &BuildTab::logError, m_logPanel, &LogPanel::appendError);
    connect(m_deployTab, &DeployTab::logOutput, m_logPanel, &LogPanel::appendLog);
    connect(m_deployTab, &DeployTab::logError, m_logPanel, &LogPanel::appendError);
}

void MainWindow::switchPage(int index)
{
    if (index >= 0 && index < m_stack->count()) {
        m_stack->setCurrentIndex(index);
        static QStringList pageNames = {"概览", "诊断", "修复", "编译", "部署", "设置"};
        if (index < pageNames.size())
            statusBar()->showMessage(pageNames[index]);
    }
}
