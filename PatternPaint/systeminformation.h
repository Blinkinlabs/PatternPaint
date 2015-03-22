#ifndef SYSTEMINFORMATION_H
#define SYSTEMINFORMATION_H

#include <QDialog>

namespace Ui {
class SystemInformation;
}

/// Give some information about the user's system, possible helpful when debugging an issue.
class SystemInformation : public QDialog
{
    Q_OBJECT

public:
    explicit SystemInformation(QWidget *parent = 0);
    ~SystemInformation();

private slots:
    void on_copyToClipboard_clicked();

    void on_sendToSupport_clicked();

private:
    Ui::SystemInformation *ui;
};

#endif // SYSTEMINFORMATION_H
