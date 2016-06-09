#include "mainwindow.h"
#include "showimages.h"

const QString MainWindow::ImageFolder = "images";
const QString MainWindow::HistFolder = "histograms";
//QList<Histogram*> list_hist;
//QStringList list_images;
QString *list_images;
//Histogram *list_hist;

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
    //free(list_images);
    //list_images->clear();
    this->histList.clear();
    list_hist.clear();
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
                this->edit->append("Create " + this->ImageFolder + " in " + dir.absolutePath());
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
                this->saveHistogram(this->imgList);
            }

            this->imageID += list_images->length();
            this->writeFile(this->imageID);

            this->imgList.clear(); // <-- Free the list.
        }
    }
}

void MainWindow::compareFiles()
{
    if(list_hist.size() != 0)
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

            // COMPARE HISTOGRAM

            // PARALLEL CODE
            int i;
            QList<Histogram*> list_hist;
            list_hist = this->list_hist;

            #pragma omp parallel firstprivate(list_hist) shared(h)
            {
                #pragma omp for private(i)
                for(i = 0; i < list_hist.length(); i++)
                    h->compareHistogram(h, list_hist.at(i), 1);

                #pragma omp barrier
            }

            // END PARALLEL CODE

            // SORT TOP 15
            QList<Histogram*> ranking;

            for(i = 0; i < list_hist.length(); i++)
            {
                if(ranking.length() < 15)
                {
                    ranking.push_back(list_hist.at(i));
                }
                else
                {
                    if(list_hist.at(i)->getCompare() > ranking.at(0)->getCompare())
                    {
                        ranking.pop_front();
                        ranking.push_back(list_hist.at(i));
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
    this->edit->append("Copying images.");

    QElapsedTimer timer;
    timer.start();

    // PARALLEL CODE
    int i;
    int size = listImages.length();
    int img_size;
    QString origen;
    QString destino;
    QString image;
    list_images = new QString[size];

    #pragma omp parallel firstprivate(list_images)
    {
        #pragma omp for private(i, img_size, origen, destino, image)
        for(i = 0; i < size; i++)
        {
            img_size = listImages.at(i).length();
            origen = path+"/"+listImages.at(i);
            image = "img_"+ QString("%1").arg(this->imageID + (i+1), 6, 10, QChar('0')) + "." +listImages.at(i).mid(img_size - 3, 3);

            // Save the string img_XXXXXX.extension
            list_images[i] = image;
            destino = this->ImageFolder+"/" + image;
            QFile::copy(origen, destino);
        }
        //cout << "Fi de la tasca pel fil " << omp_get_thread_num() << endl;
        #pragma omp barrier
    }
    // END PARALLEL CODE


    this->edit->append("Copy complete.");
    this->edit->append("Time: " + QString::number(timer.elapsed()) + "milliseconds");
    cout << "End copy images" << endl;

}

void MainWindow::saveHistogram(QStringList listImages)
{
    QDir dir;
    int size = listImages.length();

    this->edit->append("Saving histograms");

    QElapsedTimer timer;
    timer.start();

    // PARALLEL CODE

    int i;
    QString xml;
    QString xml_path;
    QString image;
    QString image_path;
    Histogram *h;
    QList<Histogram*> list_hist;

    //#pragma omp parallel firstprivate(list_hist) shared(list_images)
    #pragma omp parallel shared(list_images, list_hist)
    {
        #pragma omp for private(i, xml, xml_path, image, image_path, h)
        for(i = 0; i < size; i++)
        {
            xml = "hist_"+ QString("%1").arg(this->imageID + (i+1), 6, 10, QChar('0')) + ".xml";
            xml_path = dir.absolutePath() + "/" + this->HistFolder+"/"+ xml;

            image = list_images[i];
            image_path = dir.absolutePath() + "/" + this->ImageFolder + "/" + image;

            h = new Histogram(image.toStdString(), image_path.toStdString(), xml_path.toStdString());
            list_hist.append(h);
        }
        //cout << "Fi de la tasca pel fil " << omp_get_thread_num() << endl;
        #pragma omp barrier
    }
    this->list_hist = list_hist;

    // END PARALLEL CODE

    this->edit->append("Save complete.");
    this->edit->append("Time: " + QString::number(timer.elapsed()) + "milliseconds");
    cout << "End copy histograms" << endl;
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

        // PARALLEL CODE.
        int i;
        int size = this->histList.size();
        string xml_path;
        Histogram *h;
        QList<Histogram*> list_hist;

        #pragma omp parallel firstprivate(list_hist)
        {
            #pragma omp parallel for private(i, xml_path, h)
            for(i = 0; i < size; i++)
            {
                xml_path = path.toStdString() + "/" + this->histList.at(i).toStdString();
                h = new Histogram(xml_path);
                list_hist.append(h);
            }
            //cout << "Fi de la tasca pel fil " << omp_get_thread_num() << endl;
            #pragma omp barrier
            this->list_hist = list_hist;
        }

        // END PARALLEL CODE

        this->readFile();
        this->edit->append("\nSe ha cargado la BBDD en RAM.");
    }
    else
        this->edit->append("\nNo existe ninguna BBDD en el sistema.");
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

bool comp(Histogram* left, Histogram *right) {
  return left->getCompare() < right->getCompare();
}

void MainWindow::close()
{
    QCoreApplication::quit();
}


