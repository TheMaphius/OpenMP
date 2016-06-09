#include "menu.h"

Menu::Menu()
{

    /*
     * Create the "File" menu with his submenus.
     */

    this->file = new QMenu(QString("Options"));
    this->load_img = new QAction(QString("Load images"), this->parent());
    this->compare_img = new QAction(QString("Compare images"), this->parent());
    this->exit = new QAction(QString("Exit"), this->parent());

    this->file->addAction(this->load_img);
    this->file->addAction(this->compare_img);
    this->file->addSeparator();
    this->file->addAction(this->exit);
    this->addMenu(this->file);


    /*
     * Create the "Help" menu with his submenus
     */

    this->help = new QMenu(QString("Help"));
    this->about = new QAction(QString("About"), this->parent());

    this->help->addAction(this->about);
    this->addMenu(this->help);


}

Menu::~Menu()
{
    delete this->file;
    delete this->exit;
}

