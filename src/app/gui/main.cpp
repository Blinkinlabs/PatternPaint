#include "mainwindow.h"
#include "defaults.h"

#include <QApplication>
#include <QSettings>
#include <QString>
#include <QTranslator>
#include <QLibraryInfo>

#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setOrganizationName(ORGANIZATION_NAME);
    app.setOrganizationDomain(ORGANIZATION_DOMAIN);
    app.setApplicationName(APPLICATION_NAME);

    qSetMessagePattern("%{type} %{function}: %{message}");

    QSettings settings;
    QString language = settings.value("PatternPaint/language", DEFAULT_LANGUAGE).toString();

    QString locale;

    if(language == "<System Language>") {
        locale = QLocale::system().name();
    }
    else {
        locale = language;
    }

    qDebug() << "Loading translators for locale:" << locale;

    // Translator for the Qt library
    QTranslator qtTranslator;
    if(qtTranslator.load("qt_" + locale,
                          QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
        app.installTranslator(&qtTranslator);
    }
    else {
        qDebug() << "Could not load translator for qt library";
    }

    // Translator for PatternPaint
    QTranslator appTranslator;
    if(appTranslator.load("patternpaint_" + locale,
                           ":/translations")) {
        app.installTranslator(&appTranslator);
    }
    else {
        qDebug() << "Could not load translator for patternpaint";
    }

    MainWindow w;
    w.show();

    return app.exec();
}
