#include "welcomescreen.h"
#include "ui_welcomescreen.h"
#include "colormode.h"

#include <QDebug>

#define SCENE_TEMPLATE_COUNT 5
SceneTemplate sceneTemplates[SCENE_TEMPLATE_COUNT] =
{
    {
        "BlinkyTape",
        ":/scenetemplates/images/scenetemplates/blinkytape.jpg",
        ":/examples/blinkytape",
        "BlinkyTape",
        "Linear",
        RGB,
        60,
        1
    },
    {
        "BlinkyTile",
        ":/scenetemplates/images/scenetemplates/blinkytile.jpg",
        ":/examples/blinkytape",
        "LightBuddy",
        "Linear",
        RGB,
        56,
        1
    },
    {
        "Blinky Pixels",
        ":/scenetemplates/images/scenetemplates/pixels.jpg",
        ":/examples/blinkytape",
        "BlinkyTape",
        "Linear",
        GRB,
        50,
        1
    },
    {
        "BlinkyMatrix",
        ":/scenetemplates/images/scenetemplates/matrix.jpg",
        ":/examples/matrix",
        "BlinkyTape",
        "Matrix",
        RGB,
        8,
        8
    },
    {
        "BlinkyPendant",
        ":/scenetemplates/images/scenetemplates/pendant.jpg",
        ":/examples/pendant",
        "BlinkyPendant",
        "Linear",
        RGB,
        10,
        1
    },
};


WelcomeScreen::WelcomeScreen(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WelcomeScreen)
{
    ui->setupUi(this);

    ui->SceneList->setIconSize(QSize(150,150));

    for(int i = 0; i < SCENE_TEMPLATE_COUNT; i++ ) {
        QListWidgetItem *item = new QListWidgetItem(sceneTemplates[i].name, ui->SceneList);
        item->setIcon(QIcon(sceneTemplates[i].photo));
    }

    ui->SceneList->setCurrentRow(0);
}

WelcomeScreen::~WelcomeScreen()
{
    delete ui;
}

SceneTemplate WelcomeScreen::getSelectedTemplate()
{
    // Note that this assumes a 1:1 relationship between the listwidget and the scene template list
    int selected = 0;

    for(int row = 0; row < ui->SceneList->count(); row++) {
        if( ui->SceneList->item(row)->isSelected())
            selected = row;
    }

    return sceneTemplates[selected];
}


void WelcomeScreen::on_SceneList_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);

    this->accept();
}
