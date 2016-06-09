#include "histogram.h"

Histogram::Histogram(string image_id, string image_path, string xml_path)
{
    this->image_id = image_id;
    this->image_path = image_path;
    this->xml_path = xml_path;
    this->extractHistogram();
}

Histogram::Histogram(string xml_path)
{
    this->xml_path = xml_path;
    this->readHistogram();
}

/*Histogram::Histogram(string hist1_path, string hist2_path, int method = 1)
{
    this->compareHistogram(hist1_path, hist2_path, method);
}*/

Histogram::~Histogram()
{
    // Destructor
}

void Histogram::extractHistogram()
{

    Mat src_test, hsv_test;
    vector<Mat> hsv_planes;

    // Load image
    src_test = imread(this->image_path, CV_LOAD_IMAGE_COLOR);

    /// Convert to HSV
    cvtColor(src_test, hsv_test, CV_BGR2HSV );

    // Extract HSV planes
    split(hsv_test, hsv_planes);

    // Save the vector HSV.
    //this->hsv = hsv_planes;

    /// Bins to use
    int h_bins = 50; int s_bins = 50; int v_bins = 100;

    // Ranges
    float hrang[] = {0, 180};
    const float *h_ranges = { hrang };

    float srang[] = {0, 256};
    const float *s_ranges = { srang };

    float vrang[] = {0, 256};
    const float *v_ranges = { vrang };

    /// Histograms
    //Mat hist_h, hist_s, hist_v;

    /// Calculate the histogram for the H image
    calcHist( &hsv_planes[0], 1, 0, Mat(), this->hist_h, 1, &h_bins, &h_ranges, true, false );
    normalize( this->hist_h, this->hist_h, 0, 1, NORM_MINMAX, -1, Mat() );

    calcHist( &hsv_planes[1], 1, 0, Mat(), this->hist_s, 1, &s_bins, &s_ranges, true, false );
    normalize( this->hist_s, this->hist_s, 0, 1, NORM_MINMAX, -1, Mat() );

    calcHist( &hsv_planes[2], 1, 0, Mat(), this->hist_v, 1, &v_bins, &v_ranges, true, false );
    normalize( this->hist_v, this->hist_v, 0, 1, NORM_MINMAX, -1, Mat() );

    // Store histograms on disc
    FileStorage fs(this->xml_path, FileStorage::WRITE);

    fs << "imageName" << this->image_id;
    fs << "hist_h" << hist_h;
    fs << "hist_s" << hist_s;
    fs << "hist_v" << hist_v;

    fs.release();   // <-- Close the FileStorage.*/

}

void Histogram::compareHistogram(Histogram *img, Histogram *bbdd, int method)
{

    /// Load two images with different environment settings
    /*if( argc < 4 )
    {
        printf("** Error. Usage: ./comparehist <hist1.xml> <hist2.xml> <compare_method>\n");
        return -1;
    }*/

    int /*method, */compare_method;

    //method = atoi(argv[3]);
    if (method == 1)
        compare_method = CV_COMP_CORREL; // Correlation
    else if (method == 2)
        compare_method = CV_COMP_CHISQR; // Chi-Square
    else if (method == 3)
        compare_method = CV_COMP_INTERSECT; // Intersection
    else if (method == 4)
        compare_method = CV_COMP_BHATTACHARYYA; // Bhattacharyya distance
    else
    {
        printf("ERROR: no valid value for compare_method\n");
        exit(1);
    }

    /*Mat hist_h1, hist_s1, hist_v1;
    Mat hist_h2, hist_s2, hist_v2;

    // Read histogram1
    FileStorage fs1(img->getImagePath(), FileStorage::READ);

    string fname1;
    fs1["imageName"] >> fname1;
    fs1["hist_h"] >> hist_h1;
    fs1["hist_s"] >> hist_s1;
    fs1["hist_v"] >> hist_v1;

    fs1.release();

    // Read histogram2
    FileStorage fs2(hist2_path, FileStorage::READ);
    string fname2;
    fs2["imageName"] >> fname2;
    fs2["hist_h"] >> hist_h2;
    fs2["hist_s"] >> hist_s2;
    fs2["hist_v"] >> hist_v2;

    fs2.release();*/

    /// Histogram comparison
    /*double comphist_h = compareHist( hist_h1, hist_h2, compare_method);
    double comphist_s = compareHist( hist_s1, hist_s2, compare_method);
    double comphist_v = compareHist( hist_v1, hist_v2, compare_method);

    // Compare
    cout << "Comparing " << fname1 << " and " << fname2 << endl;
    cout << "Method " << method << ", Result " << comphist_h + comphist_s + comphist_v << endl;*/

    double comphist_h = compareHist( img->getHue(), bbdd->getHue(), compare_method);
    double comphist_s = compareHist( img->getSaturation(), bbdd->getSaturation(), compare_method);
    double comphist_v = compareHist( img->getValue(), bbdd->getValue(), compare_method);

    float percent = (comphist_h + comphist_s + comphist_v) / 0.03f; // 100%
    bbdd->setCompare(percent);  // <-- Save the value to extract later the 15 best comparisions

    // Compare
    //cout << "Comparing " << img->getName() << " and " << bbdd->getName() << endl;
    //cout << "Method " << method << ", Result " << percent << endl;

}

void Histogram::readHistogram()
{
    FileStorage fs(this->xml_path, FileStorage::READ);

    fs["imageName"] >> this->image_id;
    fs["hist_h"] >> this->hist_h;
    fs["hist_s"] >> this->hist_s;
    fs["hist_v"] >> this->hist_v;

    fs.release();
}

void Histogram::setCompare(float compare)
{
    this->compare = compare;
}

float Histogram::getCompare()
{
    return this->compare;
}

string Histogram::getName()
{
    return this->image_id;
}

string Histogram::getImagePath()
{
    return this->image_path;
}

Mat Histogram::getHue()
{
    return this->hist_h;
}

Mat Histogram::getSaturation()
{
    return this->hist_s;
}

Mat Histogram::getValue()
{
    return this->hist_v;
}


