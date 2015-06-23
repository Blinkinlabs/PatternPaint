#include "aboutpatternpaint.h"
#include "ui_aboutpatternpaint.h"

#include <QDebug>
#include <QtWidgetsDepends>

AboutPatternPaint::AboutPatternPaint(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutPatternPaint)
{
    ui->setupUi(this);

    QString versionNumber = QString("Version %1.%2.%3").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_BUILD);
    ui->versionLabel->setText(versionNumber);

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
}

void AboutPatternPaint::mousePressEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    close();
}

AboutPatternPaint::~AboutPatternPaint()
{
    delete ui;
}
