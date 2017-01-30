#include "firmwaremanager.h"
#include "ui_firmwaremanager.h"
#include "firmwareimport.h"

#include <QDebug>
#include <QStandardPaths>
#include <QFileDialog>
#include <QtWidgets>


FirmwareManager::FirmwareManager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FirmwareManager)
{
    ui->setupUi(this);

    fillFirmwareList();
}

FirmwareManager::~FirmwareManager()
{
    delete ui;
}


void FirmwareManager::on_addFirmware_clicked()
{
    // Import new Firmware
    QSettings settings;
    QString lastDirectory = settings.value("File/LoadDirectory").toString();

    QDir dir(lastDirectory);
    if (!dir.isReadable())
        lastDirectory = QDir::homePath();

    QString dirSource = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                lastDirectory,
                                                QFileDialog::ShowDirsOnly
                                                | QFileDialog::DontResolveSymlinks);

    // TODO: Store the new directory here if successful.

    firmwareimport::addFirmware(dirSource);

    fillFirmwareList();
}

void FirmwareManager::on_removeFirmware_clicked()
{
    for(QListWidgetItem *item : ui->FirmwareList->selectedItems()) {
        firmwareimport::removeFirmware(item->text());
    }

    fillFirmwareList();
}

void FirmwareManager::fillFirmwareList()
{
    ui->FirmwareList->clear();
    ui->FirmwareList->addItems(firmwareimport::listAvailableFirmware());

    on_FirmwareList_itemSelectionChanged();
}

void FirmwareManager::on_FirmwareList_itemSelectionChanged()
{
    if(ui->FirmwareList->selectedItems().count() == 0) {
        if(ui->FirmwareList->count() == 0) {
            ui->FirmwareText->clear();
            return;
        }

        ui->FirmwareList->setCurrentRow(0);
    }

    QString selectedFirmwareName = ui->FirmwareList->selectedItems().at(0)->text();

    ui->FirmwareText->setText(firmwareimport::getFirmwareDescription(selectedFirmwareName));

    QTextCursor cursor = ui->FirmwareText->textCursor();
    cursor.setPosition(0);
    ui->FirmwareText->setTextCursor(cursor);
}
