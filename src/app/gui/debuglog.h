#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <QDialog>

namespace Ui {
class DebugLog;
}

class DebugLog : public QDialog
{
    Q_OBJECT

public:
    explicit DebugLog(QWidget *parent = 0);
    ~DebugLog();

    void handleMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);

private:
    Ui::DebugLog *ui;
};

#endif // LOGDIALOG_H
