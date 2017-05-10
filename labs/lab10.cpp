#include "../project.h"

#define PI 3.14159265f

std::vector<unsigned char> getNeighborsAsVector(Mat source, int row, int col, int k_size) {
    int size = 2 * k_size + 1;
    std::vector<unsigned char> result((unsigned long)size * size);

    for (int i=-k_size, pos=0; i<=k_size; i++) {
        for (int j=-k_size; j<=k_size; j++) {
            result[pos++] = source.at<unsigned char>(row + i, col + j);
        }
    }

    return result;
}

Mat apply_median_filter(Mat source, int k_size=1) {
    Mat destination(source.rows, source.cols, CV_8UC1);

    int size = 2 * k_size + 1;
    int median_pos = size * size / 2;
    for (int i=k_size; i<source.rows-k_size; i++) {
        for (int j=k_size; j<source.cols-k_size; j++) {
            std::vector<unsigned char> values = getNeighborsAsVector(source, i, j, k_size);
            std::sort(values.begin(), values.end());
            destination.at<unsigned char>(i, j) = values[median_pos];
        }
    }

    return destination;
}

Mat initialize_gaussian_filter(float sigma) {
    int size = (int)(6 * sigma);
    if (size % 2 == 0) {
        size += 1;
    }
    Mat result(size, size, CV_32SC1);
    int x0, y0;
    x0 = y0 = size / 2;
    for (int i=0; i<size; i++) {
        for (int j=0; j<size; j++) {
            float value = 1.0f / 2.0f / PI / sigma / sigma;
            value *= exp(-((j-x0) * (j-x0) + (i-y0) * (i-y0)) / 2 / sigma / sigma);
            result.at<int>(i, j) = (int)(value * 1000);
        }
    }

    return result;
}


Mat apply_gaussian_filter(Mat source, float sigma) {
    Mat filter = initialize_gaussian_filter(sigma);
    printf("filter: \n");
    print_mat(filter);

    return apply_convolution_filter(source, filter);
}

void lab10_median_filter(char *fileName) {
    Mat source = imread(fileName, CV_8UC1);
    Mat destination = apply_median_filter(source);

    imshow("source", source);
    imshow("with median filter", destination);
}

void lab10_gaussian_filter(char *fileName) {
    Mat source = imread(fileName, CV_8UC1);
    Mat destination = apply_gaussian_filter(source, 2);

    imshow("source", source);
    imshow("with gaussian filter", destination);
}