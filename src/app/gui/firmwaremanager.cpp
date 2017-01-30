#include "firmwaremanager.h"
#include "ui_firmwaremanager.h"
#include "firmwareimport.h"

#include <QDebug>
#include <QStandardPaths>
#include <QFileDialog>
#include <QtWidgets>


QString selectedFirmwareName;


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

    QDir firmwareDirSource(dirSource);
    if (firmwareDirSource.exists()){
        // create firmwarefolder
        QString dirDestination = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        dirDestination.append(FIRMWARE_FOLDER);
        dirDestination.append(firmwareDirSource.dirName());
        QDir firmwareDirDestination(dirDestination);
        if (!firmwareDirDestination.exists()){
            qDebug() << "Firmware folder is created";
            firmwareDirDestination.mkpath(".");
        }else{
            qDebug() << "Firmware folder already exists";
        }

        // copy hex file
        QString fileSource = dirSource;
        fileSource.append("/");
        fileSource.append(firmwareDirSource.dirName());
        fileSource.append(".hex");

        QString fileDestination = dirDestination;
        fileDestination.append("/");
        fileDestination.append(firmwareDirSource.dirName());
        fileDestination.append(".hex");

        QFile hexFile(fileSource);
        if (hexFile.open(QIODevice::ReadOnly))
        {
            qDebug() << "copy hex file" ;
            QFile::copy(fileSource, fileDestination);
        }else{
            qDebug() << "hex file not found";
        }

        // copy README.md file
        fileSource = dirSource;
        fileSource.append("/README.md");

        fileDestination = dirDestination;
        fileDestination.append("/README.md");

        QFile readmeFile(fileSource);
        if (readmeFile.open(QIODevice::ReadOnly))
        {
            qDebug() << "copy README.md";
            QFile::copy(fileSource, fileDestination);
        }else{
            qDebug() << "README.md not found";
        }

        fillFirmwareList();

    }

}

void FirmwareManager::on_removeFirmware_clicked()
{
    // remove firmware
    if(selectedFirmwareName!=DEFAULT_FIRMWARE_NAME){
        qDebug() << "remove Firmware:" << selectedFirmwareName;

        QString documents = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        documents.append(FIRMWARE_FOLDER);
        documents.append(selectedFirmwareName);
        QDir firmwareDir(documents);
        if (firmwareDir.exists()){
            firmwareDir.removeRecursively();
        }

        fillFirmwareList();
    }

}




void FirmwareManager::on_FirmwareList_clicked(const QModelIndex &index)
{
    selectedFirmwareName = index.data(Qt::DisplayRole).toString();

    ui->FirmwareText->clear();
    if(selectedFirmwareName==DEFAULT_FIRMWARE_NAME){
        ui->FirmwareText->append("Default BlinkyTape Firmware");
        ui->FirmwareText->append("Use this for all standard functions");
    }else{

        // read README.md
        QString documents = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        documents.append(FIRMWARE_FOLDER);
        documents.append(selectedFirmwareName);
        documents.append("/README.md");

        QFile inputFile(documents);
        if (inputFile.open(QIODevice::ReadOnly))
        {
           QTextStream in(&inputFile);
           while (!in.atEnd())
           {
              QString line = in.readLine();
              ui->FirmwareText->append(line);

           }
           inputFile.close();

           QTextCursor cursor = ui->FirmwareText->textCursor();
           cursor.setPosition(0);
           ui->FirmwareText->setTextCursor(cursor);

        }else{
            ui->FirmwareText->append("no description available");
        }

    }


}


void FirmwareManager::fillFirmwareList()
{

    ui->FirmwareList->clear();
    ui->FirmwareText->clear();

    // add Firmware
    ui->FirmwareList->addItem(DEFAULT_FIRMWARE_NAME);
    // search for third party Firmware
    QString documents = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    documents.append(FIRMWARE_FOLDER);
    QDir firmwareDir(documents);
    if (firmwareDir.exists()){
        QStringList firmwarelist = firmwareDir.entryList(QDir::Dirs);
        firmwarelist.removeFirst();
        firmwarelist.removeFirst();
        ui->FirmwareList->addItems(firmwarelist);
    }

    selectedFirmwareName = DEFAULT_FIRMWARE_NAME;
}
