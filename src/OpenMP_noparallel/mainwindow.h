#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QtAlgorithms>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QWidget>

#include <iostream>
#include <omp.h>

#include "menu.h"
#include "histogram.h"

using namespace std;

/*namespace Ui {
class MainWindow;
}*/

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:

        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

    private:
        const static QString ImageFolder;

        QTextEdit* edit;

        //QList<QString*> list_images;
        QStringList list_images;
        QStringList imgList;

        const static QString HistFolder;
        QList<Histogram*> list_hist;
        QStringList histList;

        int imageID;

        Menu menu;

        void MenuEvents();
        void copyImage(QStringList listImages, QString path);
        bool containImages(QString path);
        void saveHistogram(QStringList listImages);
        void loadHistograms();
        void writeFile(int value);
        void readFile();

        // LOG Methods
        void writeLog(QString text);

        // COMPARATOR
        template <typename Histogram>
        struct compLess
        {
          bool operator()(Histogram* a, Histogram* b) const
          {
            return a->getCompare() < b->getCompare();
          }
        };

    private slots:
        void getFiles();
        void compareFiles();
        void aboutDialog();
        void close();

/*private:
    Ui::MainWindow *ui;*/
};

#endif // MAINWINDOW_H
