#include "welcomescreen.h"
#include "ui_welcomescreen.h"

#include <QDebug>
#include <QSettings>

WelcomeScreen::WelcomeScreen(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WelcomeScreen)
{
    ui->setupUi(this);

    QSettings settings;

    ui->showWelcomeScreen->setChecked(settings.value("WelcomeScreen/showAtStartup",
                                                     true).toBool());

    ui->loadExamplePatterns->setChecked(settings.value("WelcomeScreen/loadExamplePatterns",
                                                     true).toBool());

    ui->SceneList->setIconSize(QSize(150, 150));

    for (int i = 0; i < SCENE_TEMPLATE_COUNT; i++) {
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

    for (int row = 0; row < ui->SceneList->count(); row++) {
        if (ui->SceneList->item(row)->isSelected())
            selected = row;
    }

    return sceneTemplates[selected];
}

void WelcomeScreen::on_SceneList_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);

    this->accept();
}

void WelcomeScreen::accept()
{
    QDialog::accept();

    QSettings settings;

    settings.setValue("WelcomeScreen/showAtStartup", ui->showWelcomeScreen->isChecked());
    settings.setValue("WelcomeScreen/loadExamplePatterns", ui->loadExamplePatterns->isChecked());

    SceneTemplate scene = getSelectedTemplate();

    if(!ui->loadExamplePatterns->isChecked())
        scene.examples = "";

    // Send scene apply signal here
    emit(sceneSelected(scene));
}
