#include "aboutpatternpaint.h"
#include "ui_aboutpatternpaint.h"

#include <QDebug>
#include <QtWidgetsDepends>

void fitTextToLabel(QLabel *label)
{
// qDebug() << "Fiting text to label " << label->text();

    // check the font size - need a better algorithm... this could take awhile
    while (label->fontMetrics().width(label->text()) > label->rect().width()) {
// qDebug() << "Font size: " << label->fontMetrics().width(label->text())
// << " rect size: " << label->rect().width()
// << " point size: " << label->font().pointSize();

        int newsize = label->font().pointSize() - 1;
        label->setFont(QFont(label->font().family(), newsize));
    }
}

AboutPatternPaint::AboutPatternPaint(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutPatternPaint)
{
    ui->setupUi(this);

//    ui->versionLabel->setText(QString("Version %1").arg(VERSION_STRING));
    ui->versionLabel->setText(QString("Version %1").arg(GIT_VERSION));

    // Fit all the label text inside the label
    fitTextToLabel(ui->versionLabel);
    fitTextToLabel(ui->versionLabel_2);
    fitTextToLabel(ui->versionLabel_3);

    // And scale all labels to the same size
    int pointSize = ui->versionLabel->font().pointSize();
    if (pointSize > ui->versionLabel_2->font().pointSize())
        pointSize = ui->versionLabel_2->font().pointSize();
    if (pointSize > ui->versionLabel_3->font().pointSize())
        pointSize = ui->versionLabel_3->font().pointSize();
    ui->versionLabel->setFont(QFont(ui->versionLabel->font().family(), pointSize));
    ui->versionLabel_2->setFont(QFont(ui->versionLabel_2->font().family(), pointSize));
    ui->versionLabel_3->setFont(QFont(ui->versionLabel_3->font().family(), pointSize));

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
}

void AboutPatternPaint::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    close();
}

AboutPatternPaint::~AboutPatternPaint()
{
    delete ui;
}
