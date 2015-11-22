#include "sceneconfiguration.h"
#include "ui_sceneconfiguration.h"

#include <limits>
#include <QDebug>

SceneConfiguration::SceneConfiguration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SceneConfiguration)
{
    ui->setupUi(this);

    // TODO: Auto configuration for this?
    ui->FixtureType->addItem("LED strip", "RGB");
    ui->FixtureType->addItem("LED matrix", "RGB");

    ui->fixtureHeight->setValidator(new QIntValidator(1,std::numeric_limits<int>::max(),this));
    ui->fixtureWidth->setValidator(new QIntValidator(1,std::numeric_limits<int>::max(),this));

    // Load the available color modes
    for(int i = 0; i < COLOR_MODE_COUNT; i++) {
        qDebug() << "adding:" << colorModes[i].name << colorModes[i].colorMode;
        ui->ColorType->addItem(colorModes[i].name, colorModes[i].colorMode);
    }
}

SceneConfiguration::~SceneConfiguration()
{
    delete ui;
}

void SceneConfiguration::setColorMode(ColorMode mode) {
    for(int i = 0; i < ui->ColorType->count(); i++) {
        // TODO: Why not .value<ColorMode>() ?
        //if(ui->ColorType->itemData(i).value<ColorMode>() == mode) {
        if(ui->ColorType->itemData(i).toInt() == mode) {
            ui->ColorType->setCurrentIndex(i);
            break;
        }
    }
}

ColorMode SceneConfiguration::getColorMode() const {
    //return ui->ColorType->currentData().value<ColorMode();
    return (ColorMode)ui->ColorType->currentData().toInt();
}

void SceneConfiguration::setOutputSize(QSize size) {
    ui->fixtureHeight->setText(QString::number(size.height()));
    ui->fixtureWidth->setText(QString::number(size.width()));
}

QSize SceneConfiguration::getOutputSize() const
{
    return QSize(ui->fixtureWidth->text().toInt(), ui->fixtureHeight->text().toInt());
}
