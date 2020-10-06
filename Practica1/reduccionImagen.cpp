//g++ reduccionImagen.cpp -o x `pkg-config --cflags --libs opencv` -lpthread
#include <bits/stdc++.h>
#include <pthread.h>
#include <opencv2/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat OriginalIimage;
Mat ResizedImage;

const int output_height = 480;
const int output_width = 720;

const int xgap = (OriginalIimage.rows + output_height - 1) / output_height;
const int ygap = (OriginalIimage.cols + output_width - 1) / output_width;
const int total = xgap * ygap;
const double fraq = 1.0 / double(total);

void downSizeImage(){
    double blue, green, red;
    for(int i = 0; i < output_height; ++i){
        for(int j = 0; j < output_width; ++j){
            int xoffset = (OriginalIimage.rows * i) / output_height;
            int yoffset = (OriginalIimage.cols * j) / output_width;
            /*
            ResizedImage.at<Vec3b>(i, j)[0] = OriginalIimage.at<Vec3b>(i, j)[0];
            ResizedImage.at<Vec3b>(i, j)[1] = OriginalIimage.at<Vec3b>(i, j)[1];
            ResizedImage.at<Vec3b>(i, j)[2] = OriginalIimage.at<Vec3b>(i, j)[2];
            */
            blue = OriginalIimage.at<Vec3b>(xoffset, yoffset)[0];
            green = OriginalIimage.at<Vec3b>(xoffset, yoffset)[1];
            red = OriginalIimage.at<Vec3b>(xoffset, yoffset)[2];
            /*
            for(int x = xoffset; x < xoffset + xgap; ++x){
                for(int y = yoffset; y < yoffset + ygap; ++y){
                    
                }
            }
            */
            ResizedImage.at<Vec3b>(i, j)[0] = blue;
            ResizedImage.at<Vec3b>(i, j)[1] = green;
            ResizedImage.at<Vec3b>(i, j)[2] = red;
        }
    }
    return;
}

int main(int argc, char** argv){
    OriginalIimage = imread("landscape.jpg");
    ResizedImage = Mat::zeros(output_height, output_width, CV_8UC3);
    if(!OriginalIimage.data){
        return cout << "Couldn't open or find the image\n", -1;
    }
    //cout << ResizedImage << '\n';
    downSizeImage();
    //namedWindow("Display window", WINDOW_AUTOSIZE);
    imshow("Resized", ResizedImage);
    imshow("Original", OriginalIimage);
    waitKey(0);
    return 0;
}