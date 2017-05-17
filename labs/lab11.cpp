#include "../project.h"

GradientVectors compute_gradient(Mat horizontal_component, Mat vertical_component) {
    Mat magnitude(horizontal_component.rows, horizontal_component.cols, CV_8UC1);
    Mat direction(horizontal_component.rows, horizontal_component.cols, CV_32FC1);

    for (int i=0; i<horizontal_component.rows; i++) {
        for (int j=0; j<horizontal_component.cols; j++) {
            int h_val = horizontal_component.at<unsigned char>(i, j);
            int v_val = vertical_component.at<unsigned char>(i, j);
            int mag = (int)sqrt(h_val * h_val + v_val * v_val);

            magnitude.at<unsigned char>(i, j) = mag;
            direction.at<float>(i, j) = (float)atan2(v_val, h_val);
        }
    }

    return GradientVectors(magnitude, direction);
}

void lab11_gradient_prewitt(char *fileName) {
    Mat source = imread(fileName, CV_8UC1);
    Mat horizontal_filter = initialize_filter(3, -1,0,1, -1,0,1, -1,0,1);
    Mat vertical_filter = initialize_filter(3, 1,1,1, 0,0,0, -1,-1,-1);
    Mat horizontal = apply_convolution_filter(source, horizontal_filter);
    Mat vertical = apply_convolution_filter(source, vertical_filter);

    imshow("source", source);
    imshow("prewitt horizontal", horizontal);
    imshow("prewitt vertical", vertical);
}

void lab11_gradient_sobel(char *fileName) {
    Mat source = imread(fileName, CV_8UC1);
    Mat horizontal_filter = initialize_filter(3, -1,0,1, -2,0,2, -1,0,1);
    Mat vertical_filter = initialize_filter(3, 1,2,1, 0,0,0, -1,-2,-1);
    Mat horizontal = apply_convolution_filter(source, horizontal_filter);
    Mat vertical = apply_convolution_filter(source, vertical_filter);

    imshow("source", source);
    imshow("sobel horizontal", horizontal);
    imshow("sobel vertical", vertical);
}

void lab11_gradient_roberts(char *fileName) {
    Mat source = imread(fileName, CV_8UC1);
    Mat horizontal_filter = initialize_filter(2, 1,0, 0,-1);
    Mat vertical_filter = initialize_filter(2, 0,-1, 1,0);
    Mat horizontal = apply_convolution_filter(source, horizontal_filter);
    Mat vertical = apply_convolution_filter(source, vertical_filter);

    imshow("source", source);
    imshow("roberts horizontal", horizontal);
    imshow("roberts vertical", vertical);
}

void lab11_gradient_magnitude(char *fileName) {
    Mat source = imread(fileName, CV_8UC1);
    Mat horizontal_filter = initialize_filter(3, -1,0,1, -1,0,1, -1,0,1);
    Mat vertical_filter = initialize_filter(3, 1,1,1, 0,0,0, -1,-1,-1);
    Mat horizontal = apply_convolution_filter(source, horizontal_filter);
    Mat vertical = apply_convolution_filter(source, vertical_filter);
    GradientVectors vectors = compute_gradient(horizontal, vertical);

    imshow("source", source);
    imshow("gradient magnitude", vectors.magnitude);
}
