#ifndef DEPLOYTAB_H
#define DEPLOYTAB_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QProgressBar>
#include <QTextEdit>

class ShellRunner;
class SshConnector;

class DeployTab : public QWidget
{
    Q_OBJECT

public:
    explicit DeployTab(QWidget *parent = nullptr);

signals:
    void logOutput(const QString &text);
    void logError(const QString &text);

private slots:
    void testConnection();
    void deployFiles();
    void onSshOutput(const QString &text);
    void onSshError(const QString &text);
    void onSshFinished(int exitCode);

private:
    void setupUi();

    QLineEdit *m_hostEdit;
    QSpinBox *m_portSpin;
    QLineEdit *m_userEdit;
    QLineEdit *m_passwordEdit;
    QLineEdit *m_deployDirEdit;
    QPushButton *m_testBtn;
    QPushButton *m_deployBtn;
    QProgressBar *m_progress;
    QLabel *m_statusLabel;
    QTextEdit *m_outputView;
    SshConnector *m_ssh;
};

#endif // DEPLOYTAB_H
