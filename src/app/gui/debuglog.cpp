#include "debuglog.h"
#include "ui_debuglog.h"

QPointer<DebugLog> DebugLog::_instance;

void DebugLog::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if(_instance.isNull())
        return;

    _instance->handleMessage(type, context, msg);
}

DebugLog & DebugLog::instance(QWidget *parent) {

    if ( _instance.isNull() ) {
        _instance = new DebugLog(parent);
    }
    return *_instance;
}

DebugLog::DebugLog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DebugLog)
{
    ui->setupUi(this);

    // Set the dialog to delete itself when closed
    this->setAttribute(Qt::WA_DeleteOnClose, true);

    // And attemt to register to get messages
    qInstallMessageHandler(messageHandler);
}

DebugLog::~DebugLog()
{
    qInstallMessageHandler(0);

    delete ui;
}

void DebugLog::handleMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString formattedMessage = qFormatLogMessage(type, context, msg);
    ui->textBrowser->append(formattedMessage);
}
