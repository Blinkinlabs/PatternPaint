#include "debuglog.h"
#include "ui_debuglog.h"

#include <QPointer>

// TODO: Make this a proper singleton
QPointer<DebugLog> activeDialog;

// Redirector to the current active dialog
static void logDialogHandleMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    if(!activeDialog.isNull()) {
        activeDialog->handleMessage(type, context, msg);
    }
}

static void logDialogRegister(QPointer<DebugLog> dialog) {
    if(activeDialog.isNull()) {
        activeDialog = dialog;
        qInstallMessageHandler(&logDialogHandleMessage);
    }
}

static void logDialogUnregister(QPointer<DebugLog> dialog) {
    if(activeDialog == dialog) {
        qInstallMessageHandler(0);
        activeDialog.clear();
    }
}

DebugLog::DebugLog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DebugLog)
{
    ui->setupUi(this);

    // Set the dialog to delete itself when closed
    this->setAttribute(Qt::WA_DeleteOnClose, true);

    // And attemt to register to get messages
    logDialogRegister(this);
}

void DebugLog::handleMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString formattedMessage = qFormatLogMessage(type, context, msg);
    ui->textBrowser->append(formattedMessage);
}

DebugLog::~DebugLog()
{
    logDialogUnregister(this);
    delete ui;
}
