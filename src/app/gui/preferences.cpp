#include "preferences.h"
#include "ui_preferences.h"

#include <QSettings>
#include <QDebug>

// TODO: These are redefined from MainWindow.cpp
#define OSX_RELEASE_APPCAST_DEFAULT \
    "http://software.blinkinlabs.com/patternpaint/patternpaint-osx.xml"
#define WINDOWS_RELEASE_APPCAST_DEFAULT \
    "http://software.blinkinlabs.com/patternpaint/patternpaint-windows.xml"

// TODO: This comes from avruploaddata.cpp
#define BLINKYTAPE_MAX_BRIGHTNESS_DEFAULT 36

// TODO: This comes from blinkypendantuploader.cpp
#define BLINKYPENDANT_DISPLAYMODE_DEFAULT "POV"

Preferences::Preferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);

    // Set the dialog to delete itself when closed
    this->setAttribute(Qt::WA_DeleteOnClose, true);

    QSettings settings;

    ui->showWelcomeScreen->setChecked(settings.value("WelcomeScreen/showAtStartup",
                                                     true).toBool());
    ui->gridLayout->setColumnMinimumWidth(0, 150);

    ui->blinkyTapeMaxBrightness->setMaximum(100);
    ui->blinkyTapeMaxBrightness->setMinimum(1);
    ui->blinkyTapeMaxBrightness->setValue(settings.value("BlinkyTape/maxBrightness", BLINKYTAPE_MAX_BRIGHTNESS_DEFAULT).toInt());

    ui->blinkyPendantDisplayMode->addItem("POV");
    ui->blinkyPendantDisplayMode->addItem("Timed");

    ui->blinkyPendantDisplayMode->setCurrentText(settings.value("BlinkyPendant/displayMode", BLINKYPENDANT_DISPLAYMODE_DEFAULT).toString());

    setUpdater(NULL);
}

Preferences::~Preferences()
{
    delete ui;
}

void Preferences::setUpdater(AutoUpdater *newAutoUpdater)
{
    autoUpdater = newAutoUpdater;

    if(autoUpdater == NULL) {
        ui->automaticUpdateCheck->setEnabled(false);
        ui->checkForUpdates->setEnabled(false);
    }
    else {
        ui->automaticUpdateCheck->setEnabled(true);
        ui->checkForUpdates->setEnabled(true);

        ui->automaticUpdateCheck->setChecked(autoUpdater->getAutomatic());
    }
}

void Preferences::accept()
{
    QDialog::accept();

    QSettings settings;

    settings.setValue("WelcomeScreen/showAtStartup", ui->showWelcomeScreen->isChecked());

    if(autoUpdater != NULL) {
        // TODO: Does the updater store this on or behalf, or should we restore it at program start?
        autoUpdater->setAutomatic(ui->automaticUpdateCheck->isChecked());
    }

    if(ui->blinkyTapeMaxBrightness->value() != settings.value("BlinkyTape/maxBrightness", BLINKYTAPE_MAX_BRIGHTNESS_DEFAULT).toInt())
        settings.setValue("BlinkyTape/maxBrightness", ui->blinkyTapeMaxBrightness->value());

    if(ui->blinkyPendantDisplayMode->currentText() !=settings.value("BlinkyPendant/displayMode", BLINKYPENDANT_DISPLAYMODE_DEFAULT).toString())
        settings.setValue("BlinkyPendant/displayMode", (ui->blinkyPendantDisplayMode->currentText()));
}

void Preferences::on_checkForUpdates_clicked()
{
    if(autoUpdater == NULL)
        return;

    autoUpdater->checkForUpdates();
}
