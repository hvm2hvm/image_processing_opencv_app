#include "../project.h"

#define PI 3.14159265

GradientVectors compute_gradient(Mat horizontal_component, Mat vertical_component) {
    Mat magnitude(horizontal_component.rows, horizontal_component.cols, CV_8UC1);
    Mat direction(horizontal_component.rows, horizontal_component.cols, CV_32FC1);

    for (int i=0; i<horizontal_component.rows; i++) {
        for (int j=0; j<horizontal_component.cols; j++) {
            int h_val = horizontal_component.at<unsigned char>(i, j) - 128;
            int v_val = vertical_component.at<unsigned char>(i, j) - 128;
            int mag = (int)sqrt(h_val * h_val + v_val * v_val);

            magnitude.at<unsigned char>(i, j) = mag;
            direction.at<float>(i, j) = (float)atan2(v_val, h_val);
        }
    }

    return GradientVectors(magnitude, direction);
}

Mat colorize_gradient_direction(Mat gradient_direction) {
    Mat destination(gradient_direction.rows, gradient_direction.cols, CV_8UC3);

    for (int i=0; i<gradient_direction.rows; i++) {
        for (int j=0; j<gradient_direction.cols; j++) {
            float dir = gradient_direction.at<float>(i, j);
            Vec3b color;
            if ((dir < PI/8 && dir > -PI/8) ||
                (dir > 7*PI/8 || dir < -7*PI/8)) {
                color = Vec3b(255, 0, 0);
            } else if ((dir > PI/8 && dir < 3*PI/8) ||
                        (dir < -PI/7 && dir > -3*PI/8)) {
                color = Vec3b(0, 255, 0);
            } else if ((dir > 3*PI/8 && dir < 5*PI/8) ||
                        (dir < -3*PI/8 && dir > -5*PI/8)) {
                color = Vec3b(0, 0, 255);
            } else {
                color = Vec3b(255, 255, 0);
            }
            destination.at<Vec3b>(i, j) = color;
        }
    }

    return destination;
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
    // display_float_data("gradient direction", vectors.direction);
    imshow("gradient direction", colorize_gradient_direction(vectors.direction));
}

void lab11_gradient_thresholding(char *fileName) {
    Mat source = imread(fileName, CV_8UC1);
    Mat horizontal_filter = initialize_filter(3, -1,0,1, -1,0,1, -1,0,1);
    Mat vertical_filter = initialize_filter(3, 1,1,1, 0,0,0, -1,-1,-1);
    Mat horizontal = apply_convolution_filter(source, horizontal_filter);
    Mat vertical = apply_convolution_filter(source, vertical_filter);
    GradientVectors vectors = compute_gradient(horizontal, vertical);

    Mat thresholded = segment_with_threshold(vectors.magnitude);

    imshow("source", source);
    imshow("magnitude", vectors.magnitude);
    imshow("thresholded", thresholded);
}
