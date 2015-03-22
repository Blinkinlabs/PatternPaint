#include "resizepattern.h"
#include "ui_resizepattern.h"

ResizePattern::ResizePattern(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResizePattern)
{
    ui->setupUi(this);

    ui->patternLength->setValidator(new QDoubleValidator(this));
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
