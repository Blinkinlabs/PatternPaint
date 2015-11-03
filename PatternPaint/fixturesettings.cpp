#include "fixturesettings.h"
#include "ui_resizedialog.h"

#include <limits>

FixtureSettings::FixtureSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResizeDialog)
{
    ui->setupUi(this);

    // TODO: Auto configuration for this?
    ui->FixtureType->addItem("LED strip", "RGB");
    ui->FixtureType->addItem("LED matrix", "RGB");

    ui->fixtureHeight->setValidator(new QIntValidator(1,std::numeric_limits<int>::max(),this));
    ui->fixtureWidth->setValidator(new QIntValidator(1,std::numeric_limits<int>::max(),this));

    // Load the available color modes
    for(int i = 0; i < PatternWriter::COLOR_MODE_COUNT; i++) {
        ui->ColorType->addItem(colorModes[i].name, colorModes[i].colorMode);
    }
}

FixtureSettings::~FixtureSettings()
{
    delete ui;
}

void FixtureSettings::setColorMode(PatternWriter::ColorMode mode) {
    for(int i = 0; i < ui->FixtureType->count(); i++) {
        if(ui->FixtureType->itemData(i).value<PatternWriter::ColorMode>() == mode) {
            ui->FixtureType->setCurrentIndex(i);
            break;
        }
    }
}

PatternWriter::ColorMode FixtureSettings::getColorMode() const {
    return PatternWriter::GRB;
}

void FixtureSettings::setOutputSize(QSize size) {
    ui->fixtureHeight->setText(QString::number(size.height()));
    ui->fixtureWidth->setText(QString::number(size.width()));
}

QSize FixtureSettings::getOutputSize() const
{
    return QSize(ui->fixtureWidth->text().toInt(), ui->fixtureHeight->text().toInt());
}
