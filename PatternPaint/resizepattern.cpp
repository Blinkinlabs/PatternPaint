#include "resizepattern.h"
#include "ui_resizepattern.h"

#include <limits>

ResizePattern::ResizePattern(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResizePattern)
{
    ui->setupUi(this);

    ui->patternLength->setValidator(new QIntValidator(1,std::numeric_limits<int>::max(),this));
    ui->ledCount->setValidator(new QIntValidator(1,std::numeric_limits<int>::max(),this));
}

ResizePattern::~ResizePattern()
{
    delete ui;
}

void ResizePattern::setLength(int length) {
    ui->patternLength->setText(QString::number(length));
}

int ResizePattern::length()
{
    return ui->patternLength->text().toInt();
}

void ResizePattern::setLedCount(int ledCount) {
    ui->ledCount->setText(QString::number(ledCount));
}

int ResizePattern::ledCount()
{
    return ui->ledCount->text().toInt();
}
