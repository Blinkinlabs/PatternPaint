#include "preferences.h"
#include "ui_preferences.h"

#include <QSettings>
#include <QDebug>
#include <QFileDialog>

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

    // Set the dialog to delete itself when closed
    this->setAttribute(Qt::WA_DeleteOnClose, true);

    QSettings settings;

    ui->showWelcomeScreen->setChecked(settings.value("WelcomeScreen/showAtStartup",
                                                     true).toBool());
    ui->gridLayout->setColumnMinimumWidth(0, 150);

    ui->blinkyTapeMaxBrightness->setMaximum(100);
    ui->blinkyTapeMaxBrightness->setMinimum(1);
    ui->blinkyTapeMaxBrightness->setValue(settings.value("BlinkyTape/maxBrightness", BLINKYTAPE_MAX_BRIGHTNESS_DEFAULT).toInt());
    ui->setLanguage->addItems(listAvailableLanguages());
    ui->setLanguage->setCurrentIndex(getSavedLanguageIndex());

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

    settings.setValue("PatternPaint/language", getSelectetLanguageFile());

}

QStringList Preferences::listAvailableLanguages()
{
    QDir directory(":/");

    QStringList qmfilter;
    qmfilter << "*.qm";
    QStringList filename = directory.entryList(qmfilter);

    QStringList languagesList;
    languagesList << QLocale::languageToString(QLocale(DEFAULT_LANGUAGE).language());

    for (int i = 0; i < filename.size(); ++i){
        // get locale extracted by filename
        QString locale;
        locale = filename[i]; // "patternpaint_de.qm"
        locale.truncate(locale.lastIndexOf('.')); // "patternpaint_de"
        locale.remove(0, QString::fromUtf8("patternpaint_").length()); // "de"

        languagesList << QLocale::languageToString(QLocale(locale).language());

    }

    return languagesList;
}

QStringList Preferences::listAvailableLanguagesFiles()
{
    QDir directory(":/");

    QStringList qmfilter;
    qmfilter << "*.qm";
    QStringList filename = directory.entryList(qmfilter);

    QStringList languagesList;
    languagesList << QLocale::languageToString(QLocale(DEFAULT_LANGUAGE).language());

    for (int i = 0; i < filename.size(); ++i){
        languagesList << filename[i]; // "patternpaint_de.qm"
    }

    return languagesList;
}

QString Preferences::getSelectetLanguageFile()
{
    QString selectedLanguage;

    QStringList languagesList;
    languagesList = listAvailableLanguagesFiles();

    selectedLanguage = languagesList[ui->setLanguage->currentIndex()];

    return selectedLanguage;
}

int Preferences::getSavedLanguageIndex()
{
    QSettings settings;

    QString languageFile = settings.value("PatternPaint/language", DEFAULT_LANGUAGE).toString();

    QStringList languagesList;
    languagesList = listAvailableLanguagesFiles();

    int index = 0;

    for (int i = 0; i < languagesList.size(); ++i){
        if(languagesList[i] == languageFile){
            index = i;
            break;
        }
    }

    return index;
}

void Preferences::on_checkForUpdates_clicked()
{
    if(autoUpdater == NULL)
        return;

    autoUpdater->checkForUpdates();
}
