#include "mainwindow.h"
#include "showimages.h"

const QString MainWindow::ImageFolder = "images";
const QString MainWindow::HistFolder = "histograms";

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    QString *title = new QString("OpenMP");

    this->setWindowTitle(*title);
    this->setMinimumSize(QSize(640, 480));
    this->setMenuBar(&this->menu);

    this->adjustSize();
    this->MenuEvents();

    // LOG
    QWidget *log = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(log);
    QPalette* background = new QPalette();
    background->setColor(QPalette::Base,Qt::black);
    edit = new QTextEdit();
    edit->setTextColor(QColor(Qt::white));
    edit->setPalette(*background);


    edit->append("*******************************************************************");
    edit->append("*                                    Comparador de imagenes                                          *");
    edit->append("*******************************************************************");

    layout->addWidget(edit);
    this->setCentralWidget(log);
    this->loadHistograms();

}

MainWindow::~MainWindow()
{
    this->imgList.clear();
    this->list_images.clear();
    this->histList.clear();
    this->list_hist.clear();
}

void MainWindow::MenuEvents()
{
    this->connect(this->menu.load_img, SIGNAL(triggered()), this, SLOT(getFiles()));
    this->connect(this->menu.compare_img, SIGNAL(triggered()), this, SLOT(compareFiles()));
    this->connect(this->menu.exit, SIGNAL(triggered()), this, SLOT(close()));
    this->connect(this->menu.about, SIGNAL(triggered()), this, SLOT(aboutDialog()));
}

void MainWindow::getFiles()
{
    QDir dir;
    QStringList filtros ;
    filtros << "*.jpg" << ".*png" << "*.bmp";

    QString directorio = QFileDialog::getExistingDirectory(this, tr("Find Images"), QDir::currentPath());

    /**
     * Si el directorio contiene imagenes creamos un directorio
     * para copiar las imagenes al directorio de la aplicación
     * y guardamos en un fichero el numero del ID por el que
     * empezaremos.
     */

    if(directorio != "")
    {
        if(containImages(directorio))
        {
            this->imgList = QDir(directorio).entryList();

            /**
             * Quitamos los directorios ./ y ../
             */
            this->imgList.pop_front();
            this->imgList.pop_front();

            if(!QDir(this->ImageFolder).exists())
            {
                this->writeLog("Create " + this->ImageFolder + " in " + dir.absolutePath());
                QDir().mkdir(this->ImageFolder);
                this->writeFile(0);
                this->imageID = 0;
                this->copyImage(this->imgList, directorio);
            } else {
                this->readFile();
                this->imgList.pop_front();  // <-- Quitamos el fichero id.txt.
                this->copyImage(this->imgList, directorio);
                this->saveHistogram(this->imgList);
            }

            if(!QDir(this->HistFolder).exists())
            {
                QDir().mkdir(this->HistFolder);
                this->saveHistogram(this->list_images);
            }

            this->imageID += this->list_images.length();
            this->writeFile(this->imageID);

            this->imgList.clear(); // <-- Free the list.
        }
    }
}

void MainWindow::compareFiles()
{
    if(this->list_hist.length() != 0)
    {
        QDir dir;
        QStringList filtros ;
        filtros << "*.jpg" << ".*png" << "*.bmp";

        QString fileName = QFileDialog::getOpenFileName(this,
                                                        tr("Load Image to Compare"),
                                                        QDir::currentPath(),
                                                        tr("Images (*.jpg *.png *.bmp)"));

        QDir file = QFileInfo(fileName).absoluteDir();

        if(fileName.toStdString() != "")
        {
            QString image = fileName.split( "/" ).back();
            QString image_path = file.absolutePath() + "/" + image;
            QString xml = "hist_"+ QString("%1").arg(this->imageID +1, 6, 10, QChar('0')) + ".xml";
            QString xml_path = dir.absolutePath() + "/" + this->HistFolder+"/"+ xml;

            // Extract HSV and compare.
            Histogram *h = new Histogram(image.toStdString(), image_path.toStdString(), xml_path.toStdString());

            // BEGIN PARALLEL CODE

            int i;

            // COMPARE HISTOGRAM
            for(i = 0; i < this->list_hist.length(); i++)
                h->compareHistogram(h, this->list_hist.at(i), 1);

            // SORT TOP 15
            QList<Histogram*> ranking;

            for(i = 0; i < this->list_hist.length(); i++)
            {
                if(ranking.length() < 15)
                {
                    ranking.push_back(this->list_hist.at(i));
                }
                else
                {
                    if(this->list_hist.at(i)->getCompare() > ranking.at(0)->getCompare())
                    {
                        ranking.pop_front();
                        ranking.push_back(this->list_hist.at(i));
                    }
                }

                qSort(ranking.begin(), ranking.end(), compLess<Histogram>());

            }

            this->edit->append("\nTOP 15:\n");

            QList<QString> imagesToShow;

            for(i = ranking.length() - 1; i >= 0 ; i--)
            {
                QString text = "Image: " + QString::fromStdString(ranking.at(i)->getName()) + " is " + QString::number(ranking.at(i)->getCompare()) + "% similar";
                imagesToShow.append("images/" + QString::fromStdString(ranking.at(i)->getName()));

                this->edit->append(text);
            }

            ShowImages showImages(imagesToShow);
            showImages.setWindowTitle("View Images");
            showImages.exec();
        }

    }
    else
    {
        QMessageBox msgBox;
        QString stringMsg = "No existe BBDD de imágenes para comparar.";

        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(stringMsg);
        msgBox.exec();
    }
}

void MainWindow::aboutDialog()
{
    QMessageBox msgBox;
    QString stringMsg = "<h4>Buscador de imágenes</h4>"
                        "<h6>Universitat de Barcelona</h6>"
                        "<h6>Software Concurrent</h6>"
                        "<p>Autores: Joshua García Palacios, Iván Álvarez Ferreiro</p>"
                        "Aplicación que permite buscar imagenes utilizando el histograma"
                        "para describirlas.";

    msgBox.setText(stringMsg);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
}

/**
 * Función para determinar si un directorio contiene imágenes
 */
bool MainWindow::containImages(QString path)
{
    QStringList filtros ;
    filtros << "*.jpg" << ".*png" << "*.bmp";

    int numFile = QDir(path).entryList(filtros).length();

    if(numFile == 0 && path != "")
    {
        QMessageBox msgBox;
        QString stringMsg = "Este directorio no contiene ninguna imágen";

        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(stringMsg);
        msgBox.exec();
        return false;
    }
    return true;
}

void MainWindow::copyImage(QStringList listImages, QString path)
{
    int size = listImages.length();

    this->writeLog("Copying images.");

    QElapsedTimer timer;
    timer.start();


    for(int i = 0; i < size; i++)
    {
        int img_size = listImages.at(i).length();
        QString origen = path+"/"+listImages.at(i);
        QString extension = "img_"+ QString("%1").arg(this->imageID + (i+1), 6, 10, QChar('0')) + "." +listImages.at(i).mid(img_size - 3, 3);

        // Save the string img_XXXXXX.extension
        this->list_images << extension;
        QString destino = this->ImageFolder+"/" + extension;
        QFile::copy(origen, destino);
    }


    this->writeLog("Copy complete.");
    this->writeLog("Time: " + QString::number(timer.elapsed()) + "milliseconds");

}

void MainWindow::saveHistogram(QStringList listImages)
{
    QDir dir;
    int size = listImages.length();

    this->writeLog("Saving histograms");

    QElapsedTimer timer;
    timer.start();

    for(int i = 0; i < size; i++)
    {
        QString xml = "hist_"+ QString("%1").arg(this->imageID + (i+1), 6, 10, QChar('0')) + ".xml";
        QString xml_path = dir.absolutePath() + "/" + this->HistFolder+"/"+ xml;

        QString image = this->list_images.at(i);//"img_"+ QString("%1").arg(this->imageID + (i+1), 6, 10, QChar('0')) + "." +listImages.at(i).mid(listImages.at(i).size() - 3, 3);
        QString image_path = dir.absolutePath() + "/" + this->ImageFolder + "/" + image;

        Histogram *h = new Histogram(image.toStdString(), image_path.toStdString(), xml_path.toStdString());
        this->list_hist.append(h);
    }

    this->writeLog("Save complete.");
    this->writeLog("Time: " + QString::number(timer.elapsed()) + "milliseconds");
}


void MainWindow::loadHistograms()
{
    if(QDir(this->HistFolder).exists())
    {
        QDir dir;
        QString path = dir.absolutePath() + "/" + this->HistFolder;

        this->histList = QDir(path).entryList();

        /**
         * Quitamos los directorios ./ y ../
         */
        this->histList.pop_front();
        this->histList.pop_front();

        int size = this->histList.size();

        for(int i = 0; i < size; i++)
        {
            string xml_path = path.toStdString() + "/" + this->histList.at(i).toStdString();
            Histogram *h = new Histogram(xml_path);
            this->list_hist.append(h);
        }

        this->readFile();

        this->writeLog("\nSe ha cargado la BBDD en RAM.");
    }
    else
        this->writeLog("\nNo existe ninguna BBDD en el sistema.");
}

void MainWindow::writeFile(int value)
{
    QFile file(this->ImageFolder + "/id.txt");
    QString id = QString("%1").arg(value, 6, 10, QChar('0'));
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream write(&file);
    write << id << endl;
    file.close();
}

void MainWindow::readFile()
{
    QFile file(this->ImageFolder + "/id.txt");
    file.open(QIODevice::ReadOnly);
    QTextStream input(&file);
    this->imageID = (int)input.readLine().toInt();
    file.close();
}

void MainWindow::writeLog(QString text)
{
    edit->append(text);
}

bool comp(Histogram* left, Histogram *right) {
  return left->getCompare() < right->getCompare();
}

void MainWindow::close()
{
    QCoreApplication::quit();
}


