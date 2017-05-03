#include "../project.h"

void print_mat(Mat mat) {
    for (int i=0; i<mat.rows; i++) {
        for (int j=0; j<mat.cols; j++) {
            printf("%d ", mat.at<unsigned char>(i, j));
        }
        printf("\n");
    }
}

int apply_filter_on_pixel(Mat source, int row, int col, Mat filter) {
    int edge = filter.rows / 2;
    int result = 0;
    for (int i=-edge; i<=edge; i++) {
        for (int j=-edge; j<=edge; j++) {
            int filter_ii = edge+i;
            int filter_jj = edge+j;
            int source_value = source.at<unsigned char>(row+i, col+j);
            int filter_value = filter.at<unsigned char>(filter_ii, filter_jj);
            result += source_value * filter_value;
        }
    }
    return result;
}

Mat apply_lowpass_filter(Mat source, Mat filter) {
    Mat destination(source.rows, source.cols, CV_8UC1);
    int factor=0, edge;
    assert(filter.rows == filter.cols); // ensure filter is square
    assert(filter.rows % 2);            // ensure filter size is odd (2 * edge + 1)

    for (int i=0; i<filter.rows; i++) {
        for (int j=0; j<filter.cols; j++) {
            factor += filter.at<unsigned char>(i, j);
        }
    }
    edge = filter.rows / 2;

    for (int i=edge; i<source.rows-edge; i++) {
        for (int j=edge; j<source.cols-edge; j++) {
            destination.at<unsigned char>(i, j) = bounded(
                apply_filter_on_pixel(source, i, j, filter) / factor);
        }
    }

    return destination;
}

Mat initialize_filter(int size, ...) {
    Mat filter(size, size, CV_8UC1);
    assert(size % 2); // make sure the size is odd (2 * edge + 1)
    va_list args;
    va_start(args, size);
    int array[size*size];
    for (int i=0; i<size*size; i++) {
        array[i] = va_arg(args, int);
    }
    va_end(args);

    for (int i=0; i<size; i++) {
        for (int j=0; j<size; j++) {
            filter.at<unsigned char>(i, j) = array[i*size+j];
        }
    }

    return filter;
}

void lab9_convolution_custom(char *fileName) {
    Mat source = imread(fileName, CV_8UC1);
    Mat filter = initialize_filter(3, 1,1,1, 1,1,1, 1,1,1);
    Mat with_lowpass = apply_lowpass_filter(source, filter);

    imshow("source", source);
    imshow("with lowpass", with_lowpass);
}
