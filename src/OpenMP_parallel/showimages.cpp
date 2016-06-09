#include "showimages.h"

using namespace std;
using namespace cv;

ShowImages::ShowImages(QList<QString> filesList)
{
    this->filesList = filesList;
    OnShowPressed();
}



void ShowImages::OnShowPressed()
{
    QWidget *imagesWidget = new QWidget();
    QGridLayout *grid = new QGridLayout(imagesWidget);

    QImage copy;
    QString tempFileName;
    QList<QImage> images;


    foreach(QFileInfo fileInfo, filesList)
    {
        tempFileName = fileInfo.absoluteFilePath();
        cout << tempFileName.toStdString() << endl;
        Mat img;
        img = imread(tempFileName.toStdString(), CV_LOAD_IMAGE_COLOR);

        QImage qimage = QImage((uchar*) img.data, img.cols, img.rows, img.step, QImage::Format_RGB888);
        QImage dstImg = qimage.rgbSwapped();

        //QImage image(tempFileName);
        copy = dstImg.scaled(250,250,Qt::KeepAspectRatio);
        images.append(copy);
    }

    for (int i = 0; i < images.length(); i++)
    {
        QPixmap p(QPixmap::fromImage(images[i]));
        QLabel *label = new QLabel(imagesWidget);
        label->setPixmap(p);
        grid->addWidget(label, i/5, i%5);
    }
    setLayout(grid);
}



