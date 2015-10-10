#include "resizepattern.h"
#include "ui_resizepattern.h"

#include <limits>

ResizePattern::ResizePattern(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResizePattern)
{
    ui->setupUi(this);

    ui->frameCount->setValidator(new QIntValidator(1,std::numeric_limits<int>::max(),this));
    ui->displayHeight->setValidator(new QIntValidator(1,std::numeric_limits<int>::max(),this));
    ui->displayWidth->setValidator(new QIntValidator(1,std::numeric_limits<int>::max(),this));
}

ResizePattern::~ResizePattern()
{
    delete ui;
}

void ResizePattern::setFrameCount(int length) {
    ui->frameCount->setText(QString::number(length));
}

int ResizePattern::getFrameCount()
{
    return ui->frameCount->text().toInt();
}

void ResizePattern::setOutputSize(QSize size) {
    ui->displayHeight->setText(QString::number(size.height()));
    ui->displayWidth->setText(QString::number(size.width()));
}

QSize ResizePattern::getOutputSize()
{
    return QSize(ui->displayWidth->text().toInt(), ui->displayHeight->text().toInt());
}
