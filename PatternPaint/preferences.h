#ifndef PREFERENCES_H
#define PREFERENCES_H

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

signals:
    void checkForUpdates();

private slots:
    void on_checkForUpdates_clicked();

private:
    Ui::Preferences *ui;

    void accept();
};

#endif // PREFERENCES_H
