#ifndef WELCOMESCREEN_H
#define WELCOMESCREEN_H

#include "colormode.h"
#include "scenetemplate.h"

#include <QDialog>

namespace Ui {
class WelcomeScreen;
}

class WelcomeScreen : public QDialog
{
    Q_OBJECT

public:
    explicit WelcomeScreen(QWidget *parent = 0);
    ~WelcomeScreen();

    SceneTemplate getSelectedTemplate();

private slots:
    void on_SceneList_doubleClicked(const QModelIndex &index);

    void accept();

signals:
    void sceneSelected(SceneTemplate sceneTemplate);

private:
    Ui::WelcomeScreen *ui;
};

#endif // WELCOMESCREEN_H
