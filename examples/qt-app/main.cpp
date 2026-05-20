#include <QApplication>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("Qt ARM Demo");
    window.resize(480, 320);

    QVBoxLayout *layout = new QVBoxLayout;

    QLabel *title = new QLabel("Hello Qt on ARM!");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50;");

    QLabel *info = new QLabel("This Qt application was cross-compiled\nfor ARM Linux embedded device.");
    info->setAlignment(Qt::AlignCenter);
    info->setStyleSheet("font-size: 14px; color: #7f8c8d;");

    layout->addWidget(title);
    layout->addWidget(info);

    window.setLayout(layout);
    window.show();

    return app.exec();
}
