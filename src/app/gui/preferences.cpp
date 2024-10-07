#include "preferences.h"
#include "ui_preferences.h"
#include "defaults.h"

#include <QSettings>
#include <QDebug>
#include <QDir>

// TODO: This comes from blinkytapeuploaddata.cpp
#define BLINKYTAPE_MAX_BRIGHTNESS_DEFAULT 36
#define BLINKYTAPE_BUTTON_SHORT_PRESS_DEFAULT 1
#define BLINKYTAPE_BUTTON_LONG_PRESS_DEFAULT 2

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

    // Note: these must be synchronized with the BlinkyTape firmware
    ui->buttonShortPress->addItem("Disabled", 0);
    ui->buttonShortPress->addItem("Raise/lower brightness", 1);
    ui->buttonShortPress->addItem("Advance to next pattern", 2);
    ui->buttonShortPress->addItem("Toggle single-pattern repeat", 3);
    ui->buttonShortPress->setCurrentIndex(ui->buttonShortPress->findData(settings.value("BlinkyTape/buttonShortPress", BLINKYTAPE_BUTTON_SHORT_PRESS_DEFAULT).toInt()));

    // Note: these must be synchronized with the BlinkyTape firmware
    ui->buttonLongPress->addItem("Disabled", 0);
    ui->buttonLongPress->addItem("Raise/lower brightness", 1);
    ui->buttonLongPress->addItem("Advance to next pattern", 2);
    ui->buttonLongPress->addItem("Toggle single-pattern repeat", 3);
    ui->buttonLongPress->setCurrentIndex(ui->buttonLongPress->findData(settings.value("BlinkyTape/buttonLongPress", BLINKYTAPE_BUTTON_LONG_PRESS_DEFAULT).toInt()));

//    ui->buttonLongPress->setCurrentText(settings.value("BlinkyTape/buttonLongPress", BLINKYTAPE_BUTTON_LONG_PRESS_DEFAULT).toString());

    ui->blinkyPendantDisplayMode->addItem("POV");
    ui->blinkyPendantDisplayMode->addItem("Timed");
    ui->blinkyPendantDisplayMode->setCurrentText(settings.value("BlinkyPendant/displayMode", BLINKYPENDANT_DISPLAYMODE_DEFAULT).toString());

    ui->setLanguage->addItems(getLanguageMap().values());
    ui->setLanguage->setCurrentText(getLanguageMap().value(settings.value("PatternPaint/language", DEFAULT_LANGUAGE).toString()));

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

    if(ui->blinkyPendantDisplayMode->currentText() != settings.value("BlinkyPendant/displayMode", BLINKYPENDANT_DISPLAYMODE_DEFAULT).toString())
        settings.setValue("BlinkyPendant/displayMode", ui->blinkyPendantDisplayMode->currentText());

    if(ui->buttonShortPress->currentData().toInt() != settings.value("BlinkyTape/buttonShortPress", BLINKYTAPE_BUTTON_SHORT_PRESS_DEFAULT).toInt())
        settings.setValue("BlinkyTape/buttonShortPress", ui->buttonShortPress->currentData().toInt());

    if(ui->buttonLongPress->currentData().toInt() != settings.value("BlinkyTape/buttonLongPress", BLINKYTAPE_BUTTON_LONG_PRESS_DEFAULT).toInt())
        settings.setValue("BlinkyTape/buttonLongPress", ui->buttonLongPress->currentData().toInt());


    QString locale = getLanguageMap().key(ui->setLanguage->currentText());
    if(locale != settings.value("PatternPaint/language", DEFAULT_LANGUAGE).toString())
        settings.setValue("PatternPaint/language", locale);

    // TODO: Show warning dialog if restart required to apply settings
}

QMap<QString, QString> Preferences::getLanguageMap()
{
    QMap<QString, QString> languageMap;

    // Stuff some defaults into the list
    languageMap.insert("<System Language>", "<System Language>");
    languageMap.insert("en", QLocale::languageToString(QLocale("en").language()));

    QDir directory(":/translations/");
    for (QString filename : directory.entryList()) {
        // get locale extracted by filename
        filename.truncate(filename.lastIndexOf('.')); // "patternpaint_de"
        filename.remove(0, QString::fromUtf8("patternpaint_").length()); // "de"

        languageMap.insert(filename, QLocale::languageToString(QLocale(filename).language()));
    }

    return languageMap;
}

void Preferences::on_checkForUpdates_clicked()
{
    if(autoUpdater == NULL)
        return;

    autoUpdater->checkForUpdates();
}
