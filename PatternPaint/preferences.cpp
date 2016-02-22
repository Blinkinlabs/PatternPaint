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

Preferences::Preferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);

    QSettings settings;

    ui->showWelcomeScreen->setChecked(settings.value("WelcomeScreen/showAtStartup",
                                                     true).toBool());
    ui->gridLayout->setColumnMinimumWidth(0, 150);

#if defined(Q_OS_MACX)
    ui->updateURL->setText(settings.value("Updates/releaseAppcastUrl",
                                          OSX_RELEASE_APPCAST_DEFAULT).toString());
#elif defined(Q_OS_WIN)
    ui->updateURL->setText(settings.value("Updates/releaseAppcastUrl",
                                          WINDOWS_RELEASE_APPCAST_DEFAULT).toString());
#endif

    ui->blinkyTapeMaxBrightness->setMaximum(100);
    ui->blinkyTapeMaxBrightness->setMinimum(1);
    ui->blinkyTapeMaxBrightness->setValue(settings.value("BlinkyTape/maxBrightness", BLINKYTAPE_MAX_BRIGHTNESS_DEFAULT).toInt());

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
        ui->updateURL->setEnabled(false);
        ui->checkForUpdates->setEnabled(false);
    }
    else {
        ui->automaticUpdateCheck->setEnabled(true);
        ui->updateURL->setEnabled(true);
        ui->checkForUpdates->setEnabled(true);

        ui->automaticUpdateCheck->setChecked(autoUpdater->getAutomatic());
    }
}

void Preferences::accept()
{
    QDialog::accept();

    QSettings settings;

    settings.setValue("WelcomeScreen/showAtStartup", ui->showWelcomeScreen->isChecked());

    // Only store the appcast URL if it's different from the default
#if defined(Q_OS_MACX)
    if (ui->updateURL->text() != OSX_RELEASE_APPCAST_DEFAULT)
        settings.setValue("Updates/releaseAppcastUrl", ui->updateURL->text());

#elif defined(Q_OS_WIN)
    if (ui->updateURL->text() != WINDOWS_RELEASE_APPCAST_DEFAULT)
        settings.setValue("Updates/releaseAppcastUrl", ui->updateURL->text());
#endif

    if(autoUpdater != NULL) {
        // TODO: Does the updater store this on or behalf, or should we restore it at program start?
        autoUpdater->setAutomatic(ui->automaticUpdateCheck->isChecked());
    }

    if(ui->blinkyTapeMaxBrightness->value() != settings.value("BlinkyTape/maxBrightness", BLINKYTAPE_MAX_BRIGHTNESS_DEFAULT).toInt())
        settings.setValue("BlinkyTape/maxBrightness", ui->blinkyTapeMaxBrightness->value());
}

void Preferences::on_checkForUpdates_clicked()
{
    if(autoUpdater == NULL)
        return;

    autoUpdater->checkForUpdates();
}
