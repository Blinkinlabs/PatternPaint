#include "resizeanimation.h"
#include "ui_resizeanimation.h"

ResizeAnimation::ResizeAnimation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResizeAnimation)
{
    ui->setupUi(this);

    ui->animationLength->setValidator(new QDoubleValidator(this));
}

ResizeAnimation::~ResizeAnimation()
{
    delete ui;
}

void ResizeAnimation::setLength(int length) {
    ui->animationLength->setText(QString::number(length));
}

int ResizeAnimation::length()
{
    return ui->animationLength->text().toInt();
}

void ResizeAnimation::setLedCount(int ledCount) {
    ui->ledCount->setText(QString::number(ledCount));
}

int ResizeAnimation::ledCount()
{
    return ui->ledCount->text().toInt();
}
