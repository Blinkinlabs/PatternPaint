#include "addressprogrammer.h"
#include "ui_addressprogrammer.h"

AddressProgrammer::AddressProgrammer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddressProgrammer)
{
    ui->setupUi(this);
}

AddressProgrammer::~AddressProgrammer()
{
    delete ui;
}
