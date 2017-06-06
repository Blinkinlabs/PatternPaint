#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <QDialog>
#include <QPointer>

namespace Ui {
class DebugLog;
}

class DebugLog : public QDialog
{
    Q_OBJECT

private:
    DebugLog(QWidget *parent = 0);

    static QPointer<DebugLog> _instance;

    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

public:
    ~DebugLog();
    DebugLog(const DebugLog &) = delete;
    void operator=(const DebugLog &) = delete;

    static DebugLog & instance();

    void handleMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);

private:
    Ui::DebugLog *ui;
};

#endif // LOGDIALOG_H
