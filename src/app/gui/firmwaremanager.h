#ifndef FIRMWAREMANAGER_H
#define FIRMWAREMANAGER_H

#include <QDialog>

namespace Ui {
class FirmwareManager;
}

class FirmwareManager : public QDialog
{
    Q_OBJECT

public:
    explicit FirmwareManager(QWidget *parent = 0);
    ~FirmwareManager();

private slots:

    void on_addFirmware_clicked();

    void on_removeFirmware_clicked();

    void on_FirmwareList_clicked(const QModelIndex &index);

private:
    Ui::FirmwareManager *ui;

    void fillFirmwareList();
};

#endif // FIRMWAREMANAGER_H
