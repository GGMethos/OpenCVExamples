/*
This example loads an image and displays it in a window, then opens a second window and outputs the image's histogram
Then I use a function to equalize the image, output the equalized image to the screen and output it's histogram
Then I use the OpenCV library function equalizeHist to just do it the easy way to prove the code I wrote is correct

Before we start:

What is a histogram?

A histogram is basically just a graph that shows how many pixels share each other pixel's exact color.

Doesn't really sound fun at all... Except now you realize that you can maybe play with this a little bit and make it so that the entire image has a more equalized trend in pixel color.
Example: You are a biologist studying bee pollen for some reason and you want to see the close up pics your cool nano-machine robot drones took.

Oh no, the image came out like all dark! Stupid nano-bots! I should have bought the ones with the better cameras!

I'll just equalize the histogram with OpenCV it takes like 2 seconds if you use the standard library function

But I'm gonna show you how to equalize it without the built-in library function!
So you can see the algorithm in a programmable way.

The OpenCV library function is WAY easier to type than the one I write but you get to see how the math works.

Btw: Histogram Equalization general algorithm is on the following page.

      http://docs.opencv.org/2.4/doc/tutorials/imgproc/histograms/histogram_equalization/histogram_equalization.html

Have fun


*/


// I/O and C++ Standard Library
#include <iostream>
#include <stdio.h>

//OpenCV libs
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgcodecs.hpp>


void imhist(cv::Mat image, int histogram[]);
void cumhist(int histogram[], int cumhistogram[]);
void histDisplay(int histogram[], const char* name);

int main(int argc, char *argv[]){
    // Load the image
    cv::Mat image;
    cv::Mat dst;
    if (argc<2){
        image = cv::imread("pollen.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    }
    if (argc==2){
        image = cv::imread((argv[1]), CV_LOAD_IMAGE_GRAYSCALE);
    }
    if (image.empty() ){
        std::cout<<"Could not find image"<<std::endl;
        std::cout<<"Error detected. Exiting."<<std::endl;
        return -1;
    }
    // Generate the histogram
    int histogram[256];
    imhist(image, histogram);
    // Caluculate the size of image
    int size = image.rows * image.cols;
    float alpha = 255.0/size;
    // Calculate the probability of each intensity
    float PrRk[256];
    for(int i = 0; i < 256; i++){
        PrRk[i] = (double)histogram[i] / size;
    }
    // Generate cumulative frequency histogram
    int cumhistogram[256];
    cumhist(histogram,cumhistogram );
    // Scale the histogram
    int Sk[256];
    for(int i = 0; i < 256; i++){
        Sk[i] = cvRound((double)cumhistogram[i] * alpha);
    }
    // Generate the equalized histogram
    float PsSk[256];
    for(int i = 0; i < 256; i++){
        PsSk[i] = 0;
    }
    for(int i = 0; i < 256; i++){
        PsSk[Sk[i]] += PrRk[i];
    }
    int final[256];
    for(int i = 0; i < 256; i++){
        final[i] = cvRound(PsSk[i]*255);
    }
    // Generate the equalized image
    cv::Mat new_image = image.clone();
    for(int y = 0; y < image.rows; y++)
        for(int x = 0; x < image.cols; x++)
            new_image.at<uchar>(y,x) = cv::saturate_cast<uchar>(Sk[image.at<uchar>(y,x)]);
    // Display the original image
    cv::namedWindow("Original Image");
    cv::imshow("Original Image", image);
    // Display the original histogram
    histDisplay(histogram, "Original Histogram");
    // Display equalized image
    cv::namedWindow("Equalized Image");
    cv::imshow("Equalized Image", new_image);
    // Display the equalized histogram
    histDisplay(final, "Equalized Histogram");

    //Now to verify that my implementation of the equalized histogram is correct...
    cv::equalizeHist(image,dst);
    cv::namedWindow("Using equalizeHist from OpenCV lib:", cv::WINDOW_AUTOSIZE);
    cv::imshow("Using equalizeHist from OpenCV lib:", dst);

    int eqzHistSize=256;
    float eqzHistRange[] = {0,256};
    const float *eqzHistRanges[]= { eqzHistRange};
    cv::MatND eqzHist2;
    calcHist(&dst,1,0,cv::Mat(),eqzHist2,1,&eqzHistSize,eqzHistRanges,true,false);

    double eqzTotal;
    eqzTotal = dst.rows * dst.cols;
    std::cout<<""<<std::endl;
    std::cout<<"Equalized Histogram with OpenCV Library:"<<std::endl;
    std::cout<<""<<std::endl;
    for(int j =0;j<eqzHistSize;j++){
        float eqzHistVal=eqzHist2.at<float>(j);
        std::cout << " " << eqzHistVal;
    }

    // Plot the histogram
    int eqzHistw2 = 512; int eqzHisth2 = 400;
    int eqzbinw2 = cvRound( (double) eqzHistw2/eqzHistSize);
    cv::Mat eqzHistImage2( eqzHisth2, eqzHistw2, CV_8UC1, cv::Scalar( 0,0,0) );
    normalize(eqzHist2, eqzHist2, 0, eqzHistImage2.rows, cv::NORM_MINMAX, -1, cv::Mat() );
    for( int i = 1; i < eqzHistSize; i++ ){
        line( eqzHistImage2, cv::Point( eqzbinw2*(i-1), eqzHisth2 - cvRound(eqzHist2.at<float>(i-1)) ) ,
                         cv::Point( eqzbinw2*(i), eqzHisth2 - cvRound(eqzHist2.at<float>(i)) ),
                         cv::Scalar( 255, 0, 0), 2, 8, 0  );
      }

    cv::namedWindow( "Equalized Histogram using OpenCV function equalizeHist():", 1);
    cv::imshow( "Equalized Histogram using OpenCV function equalizeHist():", eqzHistImage2);

    char k;
    for(int x = 1; x < 5; x++){
        k=cvWaitKey(0);
    }
    return 0;
}

void imhist(cv::Mat image, int histogram[]){
    // initialize all intensity values to 0
    for(int i = 0; i < 256; i++){
        histogram[i] = 0;
    }
    // calculate the no of pixels for each intensity values
    for(int y = 0; y < image.rows; y++)
        for(int x = 0; x < image.cols; x++)
            histogram[(int)image.at<uchar>(y,x)]++;
}

void cumhist(int histogram[], int cumhistogram[]){
    cumhistogram[0] = histogram[0];
    for(int i = 1; i < 256; i++){
        cumhistogram[i] = histogram[i] + cumhistogram[i-1];
    }
}

void histDisplay(int histogram[], const char* name)
{
    int hist[256];
    for(int i = 0; i < 256; i++)
        hist[i]=histogram[i];
    // draw the histograms
    int hist_w = 512; int hist_h = 400;
    int bin_w = cvRound((double) hist_w/256);

    cv::Mat histImage(hist_h, hist_w, CV_8UC1, cv::Scalar(255, 255, 255));

    // find the maximum intensity element from histogram
    int max = hist[0];
    for(int i = 1; i < 256; i++){
        if(max < hist[i]){
            max = hist[i];
        }
    }

    // normalize the histogram between 0 and histImage.rows

    for(int i = 0; i < 256; i++)
        hist[i] = ((double)hist[i]/max)*histImage.rows;

    // draw the intensity line for histogram
    for(int i = 0; i < 256; i++){
        line(histImage, cv::Point(bin_w*(i), hist_h),
                cv::Point(bin_w*(i), hist_h - hist[i]),
                cv::Scalar(0,0,0), 1, 8, 0);
    }

    // display histogram
    cv::namedWindow(name, CV_WINDOW_AUTOSIZE);
    cv::imshow(name, histImage);
}

