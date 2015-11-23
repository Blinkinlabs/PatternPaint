#include "preferences.h"
#include "ui_preferences.h"

#include <QSettings>
#include <QDebug>

// TODO: These are redefined from main.cpp
#define OSX_RELEASE_APPCAST_DEFAULT "http://software.blinkinlabs.com/patternpaint/patternpaint-osx.xml"
#define WINDOWS_RELEASE_APPCAST_DEFAULT "http://software.blinkinlabs.com/patternpaint/patternpaint-windows.xml"


Preferences::Preferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);

    QSettings settings;

    ui->gridLayout->setColumnMinimumWidth(0, 150);

    ui->showWelcomeScreen->setChecked(settings.value("MainWindow/showWelcomeScreenAtStartup",true).toBool());

#if defined(Q_OS_MACX)
    ui->automaticUpdateCheck->hide();

    ui->updateURL->setText(settings.value("Updates/releaseAppcastUrl", OSX_RELEASE_APPCAST_DEFAULT).toString());
#elif defined(Q_OS_WIN)
    ui->automaticUpdateCheck->hide();

    ui->updateURL->setText(settings.value("Updates/releaseAppcastUrl", WINDOWS_RELEASE_APPCAST_DEFAULT).toString());
#else
    ui->updateURL->setEnabled(false);
#endif
}

Preferences::~Preferences()
{
    delete ui;
}

void Preferences::accept()
{
    QSettings settings;

    settings.setValue("MainWindow/showWelcomeScreenAtStartup", ui->showWelcomeScreen->isChecked());


#if defined(Q_OS_MACX)
    if(ui->updateURL->text() != OSX_RELEASE_APPCAST_DEFAULT)
        settings.setValue("Updates/releaseAppcastUrl", ui->updateURL->text());

#elif defined(Q_OS_WIN)
    if(ui->updateURL->text() != WINDOWS_RELEASE_APPCAST_DEFAULT)
        settings.setValue("Updates/releaseAppcastUrl", ui->updateURL->text());

#endif


    QDialog::accept();
}
