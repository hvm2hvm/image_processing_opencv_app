#include "../project.h"

float compute_intensity_mean(Mat source) {
    float result = 0;

    for (int i=0; i<source.rows; i++) {
        for (int j=0; j<source.cols; j++) {
            result += source.at<uchar>(i, j);
        }
    }

    return result / source.rows / source.cols;
}

float compute_intensity_deviation(Mat source) {
    float mean = compute_intensity_mean(source);
    float result = 0;

    for (int i=0; i<source.rows; i++) {
        for (int j=0; j<source.cols; j++) {
            int value = source.at<uchar>(i, j);
            result += (value - mean) * (value - mean);
        }
    }

    return sqrt(result / source.rows / source.cols);
}

std::vector<int> compute_cumulative_histogram(std::vector<int> histogram) {
    std::vector<int> result(histogram.size());
    int h = 0;
    for (int i=0; i<histogram.size(); i++) {
        h += histogram[i];
        result[i] = h;
    }
    return result;
}

std::vector<float> compute_normalized_cumulative_histogram(std::vector<float> histogram) {
    std::vector<float> result(histogram.size());
    float h = 0;
    for (int i=0; i<histogram.size(); i++) {
        h += histogram[i];
        result[i] = h;
    }
    return result;
}

Mat segment_with_threshold(Mat source) {
    Mat destination(source.rows, source.cols, CV_8UC1);
    std::vector<float> histogram = compute_normalized_histogram(source);
    int max_intensity = -1, min_intensity = -1;
    for (int i=0; i<histogram.size(); i++) {
        if (min_intensity == -1 && histogram[i] > 0) {
            min_intensity = i;
        }
        if (histogram[i] > 0) {
            max_intensity = i;
        }
    }
    int threshold = (min_intensity + max_intensity) / 2;
    int count_lower=0, count_higher=0;
    for (int i=0; i<source.rows; i++) {
        for (int j=0; j<source.cols; j++) {
            if (source.at<uchar>(i, j) < threshold) {
                count_lower += 1;
            } else {
                count_higher += 1;
            }
        };
    }
    float mean_lower, mean_higher;

    return destination;
}

void lab8_histogram(char *filePath) {
    Mat source = imread(filePath, CV_8UC1);

    std::vector<int> histogram = compute_histogram(source);
    std::vector<int> cumulative_histogram = compute_cumulative_histogram(histogram);

    printf("intensity mean: %5.2f\n", compute_intensity_mean(source));
    printf("intensity deviation: %5.2f\n", compute_intensity_deviation(source));
    display_histogram("histogram", histogram);
    display_histogram("cumulative histogram", cumulative_histogram);
}

void lab8_threshold(char *filePath) {
    Mat source = imread(filePath, CV_8UC1);
}