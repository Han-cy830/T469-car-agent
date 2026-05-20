#include <QApplication>
#include <QFile>
#include <QTextCodec>
#include "widgets/MainWindow.h"

static QString darkTheme()
{
    return QStringLiteral(R"(
        * {
            font-family: "Microsoft YaHei", "Segoe UI", "Noto Sans CJK SC", sans-serif;
            font-size: 13px;
        }
        QWidget {
            background-color: #1e1e2e;
            color: #cdd6f4;
        }
        QMainWindow {
            background-color: #1e1e2e;
        }
        QSplitter::handle {
            background-color: #313244;
            width: 2px;
        }
        QPushButton {
            background-color: #313244;
            color: #cdd6f4;
            border: 1px solid #45475a;
            border-radius: 6px;
            padding: 6px 16px;
            min-height: 28px;
        }
        QPushButton:hover {
            background-color: #45475a;
            border-color: #89b4fa;
        }
        QPushButton:pressed {
            background-color: #585b70;
        }
        QPushButton:disabled {
            background-color: #181825;
            color: #585b70;
        }
        QPushButton#primaryButton {
            background-color: #89b4fa;
            color: #1e1e2e;
            font-weight: bold;
            border: none;
        }
        QPushButton#primaryButton:hover {
            background-color: #74c7ec;
        }
        QPushButton#dangerButton {
            background-color: #f38ba8;
            color: #1e1e2e;
            border: none;
        }
        QPushButton#successButton {
            background-color: #a6e3a1;
            color: #1e1e2e;
            border: none;
        }
        QLineEdit, QTextEdit, QPlainTextEdit {
            background-color: #181825;
            color: #cdd6f4;
            border: 1px solid #45475a;
            border-radius: 4px;
            padding: 4px;
            selection-background-color: #89b4fa;
        }
        QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus {
            border-color: #89b4fa;
        }
        QComboBox {
            background-color: #313244;
            color: #cdd6f4;
            border: 1px solid #45475a;
            border-radius: 4px;
            padding: 4px 8px;
        }
        QComboBox::drop-down {
            border: none;
            width: 20px;
        }
        QComboBox QAbstractItemView {
            background-color: #313244;
            color: #cdd6f4;
            selection-background-color: #45475a;
        }
        QTableWidget {
            background-color: #181825;
            color: #cdd6f4;
            gridline-color: #313244;
            border: 1px solid #313244;
            border-radius: 4px;
        }
        QTableWidget::item {
            padding: 4px;
        }
        QTableWidget::item:selected {
            background-color: #45475a;
        }
        QHeaderView::section {
            background-color: #313244;
            color: #cdd6f4;
            border: none;
            border-bottom: 1px solid #45475a;
            padding: 6px;
            font-weight: bold;
        }
        QProgressBar {
            background-color: #181825;
            border: 1px solid #45475a;
            border-radius: 4px;
            text-align: center;
            color: #cdd6f4;
            height: 20px;
        }
        QProgressBar::chunk {
            background-color: #89b4fa;
            border-radius: 3px;
        }
        QScrollBar:vertical {
            background-color: #1e1e2e;
            width: 8px;
            margin: 0;
        }
        QScrollBar::handle:vertical {
            background-color: #45475a;
            border-radius: 4px;
            min-height: 20px;
        }
        QScrollBar::handle:vertical:hover {
            background-color: #585b70;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }
        QScrollBar:horizontal {
            background-color: #1e1e2e;
            height: 8px;
            margin: 0;
        }
        QScrollBar::handle:horizontal {
            background-color: #45475a;
            border-radius: 4px;
            min-width: 20px;
        }
        QScrollBar::handle:horizontal:hover {
            background-color: #585b70;
        }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0;
        }
        QGroupBox {
            border: 1px solid #45475a;
            border-radius: 6px;
            margin-top: 8px;
            padding-top: 16px;
            font-weight: bold;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 6px;
            color: #89b4fa;
        }
        QLabel {
            color: #cdd6f4;
        }
        QLabel#titleLabel {
            font-size: 18px;
            font-weight: bold;
            color: #89b4fa;
        }
        QLabel#subtitleLabel {
            font-size: 14px;
            color: #a6adc8;
        }
        QTabWidget::pane {
            border: 1px solid #45475a;
            border-radius: 4px;
        }
        QTabBar::tab {
            background-color: #313244;
            color: #a6adc8;
            padding: 8px 16px;
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
            margin-right: 2px;
        }
        QTabBar::tab:selected {
            background-color: #45475a;
            color: #cdd6f4;
        }
        QCheckBox {
            spacing: 8px;
        }
        QCheckBox::indicator {
            width: 16px;
            height: 16px;
            border: 1px solid #45475a;
            border-radius: 3px;
            background-color: #181825;
        }
        QCheckBox::indicator:checked {
            background-color: #89b4fa;
            border-color: #89b4fa;
        }
        QSpinBox {
            background-color: #181825;
            color: #cdd6f4;
            border: 1px solid #45475a;
            border-radius: 4px;
            padding: 2px 4px;
        }
        QStatusBar {
            background-color: #181825;
            color: #a6adc8;
            border-top: 1px solid #313244;
        }
        QMenuBar {
            background-color: #1e1e2e;
            color: #cdd6f4;
        }
        QMenuBar::item:selected {
            background-color: #45475a;
        }
        QMenu {
            background-color: #313244;
            color: #cdd6f4;
            border: 1px solid #45475a;
        }
        QMenu::item:selected {
            background-color: #45475a;
        }
        QToolTip {
            background-color: #313244;
            color: #cdd6f4;
            border: 1px solid #45475a;
            padding: 4px;
        }
    )");
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("T469 Car Agent");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("T469");

    // Windows 下设置控制台编码为 UTF-8
#if defined(Q_OS_WIN)
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
#endif

    // 应用暗色主题
    app.setStyleSheet(darkTheme());

    MainWindow w;
    w.show();

    return app.exec();
}
