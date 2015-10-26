#include "resizedialog.h"
#include "ui_resizedialog.h"

#include <limits>

ResizeDialog::ResizeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResizeDialog)
{
    ui->setupUi(this);

    ui->displayHeight->setValidator(new QIntValidator(1,std::numeric_limits<int>::max(),this));
    ui->displayWidth->setValidator(new QIntValidator(1,std::numeric_limits<int>::max(),this));
}

ResizeDialog::~ResizeDialog()
{
    delete ui;
}

void ResizeDialog::setOutputSize(QSize size) {
    ui->displayHeight->setText(QString::number(size.height()));
    ui->displayWidth->setText(QString::number(size.width()));
}

QSize ResizeDialog::getOutputSize()
{
    return QSize(ui->displayWidth->text().toInt(), ui->displayHeight->text().toInt());
}
