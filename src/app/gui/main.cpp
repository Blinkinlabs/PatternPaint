#include "mainwindow.h"
#include "defaults.h"

#include <QApplication>
#include <QSettings>
#include <QString>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setOrganizationName(ORGANIZATION_NAME);
    a.setOrganizationDomain(ORGANIZATION_DOMAIN);
    a.setApplicationName(APPLICATION_NAME);

    QSettings settings;
    QString language = ":/" + settings.value("PatternPaint/language", DEFAULT_LANGUAGE).toString();

    QTranslator translator;
    translator.load(language);
    a.installTranslator(&translator);

    qSetMessagePattern("%{type} %{function}: %{message}");

    MainWindow w;
    w.show();

    return a.exec();
}
