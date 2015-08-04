#include "aboutpatternpaint.h"
#include "ui_aboutpatternpaint.h"

#include <QDebug>
#include <QtWidgetsDepends>

AboutPatternPaint::AboutPatternPaint(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutPatternPaint)
{
    ui->setupUi(this);

    ui->versionLabel->setText(QString("Version %1").arg(VERSION_STRING));

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
