#include "welcomescreen.h"
#include "ui_welcomescreen.h"

WelcomeScreen::WelcomeScreen(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WelcomeScreen)
{
    ui->setupUi(this);
}

WelcomeScreen::~WelcomeScreen()
{
    delete ui;
}
