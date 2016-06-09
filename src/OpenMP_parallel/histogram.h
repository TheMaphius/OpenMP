#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

class Histogram
{
    public:

        Histogram(string image_id, string image_path, string xml_path);
        Histogram(string xml_path);
        //Histogram(string hist1_path, string hist2_path, int method);
        ~Histogram();

        void extractHistogram();
        void compareHistogram(Histogram *img, Histogram *bbdd, int method);
        void readHistogram();

        float getCompare();
        void setCompare(float compare);

        string getName();
        string getImagePath();
        Mat getHue();
        Mat getSaturation();
        Mat getValue();

    private:

        ///File Info.
        string image_id;
        string image_path;
        string xml_path;

        /// HUE.
        Mat hist_h;
        Mat hist_s;
        Mat hist_v;

        float compare;
};

#endif // HISTOGRAM_H
