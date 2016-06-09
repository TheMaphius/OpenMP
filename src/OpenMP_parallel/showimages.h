#pragma once

#include <omp.h>
#include <QPushButton>
#include <QDialog>
#include <QWidget>
#include <QApplication>
#include <QGridLayout>
#include <QFileInfo>
#include <QLabel>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

class ShowImages : public QDialog
{
Q_OBJECT

 public:
   ShowImages(QList<QString> filesList);

private slots:
    void OnShowPressed();

  private:
    QPushButton *show_btn;
    QPushButton *empty_btn;
    QList<QString> filesList;
};
