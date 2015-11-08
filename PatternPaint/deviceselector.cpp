#include "deviceselector.h"
#include "ui_deviceselector.h"

DeviceSelector::DeviceSelector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceSelector)
{
    ui->setupUi(this);

    ui->DeviceWidget->addItem("BlinkyTape");
    ui->DeviceWidget->addItem("BlinkyTile");
    ui->DeviceWidget->addItem("BlinkyPendant");
    ui->DeviceWidget->addItem("Blinky Pixels");
    ui->DeviceWidget->addItem("Blinky Matrix");
    ui->DeviceWidget->addItem("Custom");
}

DeviceSelector::~DeviceSelector()
{
    delete ui;
}
