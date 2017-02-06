#include "sceneconfiguration.h"
#include "ui_sceneconfiguration.h"
#include "firmwareimport.h"

#include <limits>
#include <QDebug>
#include <QFileDialog>


SceneConfiguration::SceneConfiguration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SceneConfiguration)
{
    ui->setupUi(this);

    // Add the scene templates
    for (int i = 0; i < SCENE_TEMPLATE_COUNT; i++)
        ui->sceneTemplate->addItem(sceneTemplates[i].name);
    ui->sceneTemplate->addItem("Custom");
    ui->sceneTemplate->setCurrentText("Custom");

    // Add the controller types
    // TODO: Auto configuration for this?
    ui->controllerType->addItem("BlinkyTape");
    ui->controllerType->addItem("Light Buddy (BlinkyTile)");
    ui->controllerType->addItem("BlinkyPendant");
    ui->controllerType->addItem("Arduino Leonardo");
    ui->controllerType->addItem("Arduino Micro");
    ui->controllerType->addItem("EightByEight");

    // Disable controller selection for now since it doesn't really matter.
// ui->controllerBox->setVisible(false);

    // Add the Firmware types
    ui->firmwareName->addItems(FirmwareStore::listAvailableFirmware());

    // Add the fixture types
    // TODO: Auto configuration for this?
    ui->FixtureType->addItem("Linear");
    ui->FixtureType->addItem("Matrix-Zigzag");
    ui->FixtureType->addItem("Matrix-Rows");

    ui->fixtureHeight->setValidator(new QIntValidator(1, std::numeric_limits<int>::max(), this));
    ui->fixtureWidth->setValidator(new QIntValidator(1, std::numeric_limits<int>::max(), this));

    // Load the available color modes
    for (int i = 0; i < COLOR_MODE_COUNT; i++)
        ui->ColorType->addItem(colorModes[i].name, colorModes[i].colorMode);
}

SceneConfiguration::~SceneConfiguration()
{
    delete ui;
}

void SceneConfiguration::setSceneTemplate(const SceneTemplate sceneTemplate)
{
    setControllerType(sceneTemplate.controllerType);
    setFixtureType(sceneTemplate.fixtureType);
    setFirmwareName(sceneTemplate.firmwareName);
    setColorMode(sceneTemplate.colorMode);
    setFixtureSize(QSize(sceneTemplate.width, sceneTemplate.height));
}

SceneTemplate SceneConfiguration::getSceneTemplate() const
{
    SceneTemplate sceneTemplate;
    sceneTemplate.name = "???";
    sceneTemplate.photo = "";
    sceneTemplate.examples = "";
    sceneTemplate.controllerType = ui->controllerType->currentText();
    sceneTemplate.fixtureType = ui->FixtureType->currentText();
    sceneTemplate.firmwareName = ui->firmwareName->currentText();
    sceneTemplate.colorMode = (ColorMode)ui->ColorType->currentData().toInt();
    sceneTemplate.height = ui->fixtureHeight->text().toInt();
    sceneTemplate.width = ui->fixtureWidth->text().toInt();

    return sceneTemplate;
}

void SceneConfiguration::setColorMode(ColorMode mode)
{
    ui->ColorType->blockSignals(true);

    for (int i = 0; i < ui->ColorType->count(); i++) {
        // TODO: Why not .value<ColorMode>() ?
        // if(ui->ColorType->itemData(i).value<ColorMode>() == mode) {
        if (ui->ColorType->itemData(i).toInt() == mode) {
            ui->ColorType->setCurrentIndex(i);
            break;
        }
    }

    ui->ColorType->blockSignals(false);
}

void SceneConfiguration::setFixtureSize(QSize size)
{
    ui->fixtureHeight->setText(QString::number(size.height()));
    ui->fixtureWidth->setText(QString::number(size.width()));
}

void SceneConfiguration::setControllerType(QString type)
{
    ui->controllerType->blockSignals(true);
    ui->controllerType->setCurrentText(type);
    ui->controllerType->blockSignals(false);
}

void SceneConfiguration::setFixtureType(QString type)
{
    ui->FixtureType->blockSignals(true);
    ui->FixtureType->setCurrentText(type);
    ui->FixtureType->blockSignals(false);
}

void SceneConfiguration::setFirmwareName(QString type)
{
    ui->firmwareName->blockSignals(true);
    ui->firmwareName->setCurrentText(type);
    ui->firmwareName->blockSignals(false);
}

void SceneConfiguration::on_sceneTemplate_activated(const QString &arg1)
{
    for (int i = 0; i < SCENE_TEMPLATE_COUNT; i++) {
        if (sceneTemplates[i].name == arg1)
            setSceneTemplate(sceneTemplates[i]);
    }
}

void SceneConfiguration::sceneCustomized()
{
    if (ui->sceneTemplate->currentText() != "Custom")
        ui->sceneTemplate->setCurrentText("Custom");
}

void SceneConfiguration::on_fixtureHeight_textEdited(const QString &arg1)
{
    Q_UNUSED(arg1);

    sceneCustomized();
}

void SceneConfiguration::on_fixtureWidth_textEdited(const QString &arg1)
{
    Q_UNUSED(arg1);

    sceneCustomized();
}

void SceneConfiguration::on_FixtureType_currentIndexChanged(int index)
{
    Q_UNUSED(index);

    sceneCustomized();
}

void SceneConfiguration::on_ColorType_currentIndexChanged(int index)
{
    Q_UNUSED(index);

    sceneCustomized();
}

void SceneConfiguration::on_controllerType_currentIndexChanged(int index)
{
    Q_UNUSED(index);

    sceneCustomized();
}

void SceneConfiguration::on_firmwareName_currentIndexChanged(int index)
{
    Q_UNUSED(index);

    sceneCustomized();
}
