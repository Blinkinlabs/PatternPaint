#ifndef PREFERENCES_H
#define PREFERENCES_H

#include "autoupdater.h"

#include <QDialog>

namespace Ui {
class Preferences;
}

class Preferences : public QDialog
{
    Q_OBJECT

public:
    explicit Preferences(QWidget *parent = 0);
    ~Preferences();

    void setUpdater(AutoUpdater* newAutoUpdater);

signals:
    void checkForUpdates();

private slots:
    void on_checkForUpdates_clicked();

private:
    AutoUpdater* autoUpdater;

    Ui::Preferences *ui;

    void accept();

    static QStringList listAvailableLanguages();

    static QStringList listAvailableLanguagesFiles();

    QString getSelectetLanguageFile();

    int getSavedLanguageIndex();

    QStringList languageFile;

};

#endif // PREFERENCES_H
