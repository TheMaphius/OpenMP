#ifndef MENU_H
#define MENU_H

#include <QMenuBar>
#include <QString>

class Menu: public QMenuBar
{

    public:
        Menu();
        ~Menu();

        QMenu *file;
        QAction *load_img;
        QAction *compare_img;
        QAction *exit;

        QMenu *help;
        QAction *about;
};

#endif // MENU_H
