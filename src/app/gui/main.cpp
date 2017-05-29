#include "mainwindow.h"
#include "defaults.h"

#include <QApplication>
#include <QSettings>
#include <QString>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setOrganizationName(ORGANIZATION_NAME);
    app.setOrganizationDomain(ORGANIZATION_DOMAIN);
    app.setApplicationName(APPLICATION_NAME);

    qSetMessagePattern("%{type} %{function}: %{message}");


    QSettings settings;
    QString language = ":/" + settings.value("PatternPaint/language", DEFAULT_LANGUAGE).toString();

    QTranslator translator;
    translator.load(language);
    app.installTranslator(&translator);


    MainWindow w;
    w.show();

    return app.exec();
}
