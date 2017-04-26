#include "../project.h"

float compute_intensity_mean(Mat source) {
    float result = 0;

    for (int i=0; i<source.rows; i++) {
        for (int j=0; j<source.cols; j++) {
            result += source.at<unsigned char>(i, j);
        }
    }

    return result / source.rows / source.cols;
}

float compute_intensity_deviation(Mat source) {
    float mean = compute_intensity_mean(source);
    float result = 0;

    for (int i=0; i<source.rows; i++) {
        for (int j=0; j<source.cols; j++) {
            int value = source.at<unsigned char>(i, j);
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

int vector_sum(std::vector<int> vec, int start, int end) {
    int result = 0;
    for (int i=start; i<end; i++) {
        result += vec[i];
    }
    return result;
}

Mat segment_with_threshold(Mat source) {
    int max_error = 5;

    Mat destination(source.rows, source.cols, CV_8UC1);
    std::vector<int> histogram = compute_histogram(source);
    int max_intensity = -1, min_intensity = -1;
    for (int i=0; i<histogram.size(); i++) {
        if (min_intensity == -1 && histogram[i] > 0) {
            min_intensity = i;
        }
        if (histogram[i] > 0) {
            max_intensity = i;
        }
    }
    int previous_threshold;
    int threshold;
    int count_lower, count_higher;
    int mean_lower, mean_higher;

    previous_threshold = -1;
    threshold = (min_intensity + max_intensity) / 2;
    while (previous_threshold == -1 || abs(previous_threshold - threshold) > max_error) {
        count_lower = vector_sum(histogram, 0, threshold);
        count_higher = vector_sum(histogram, threshold, 256);
        mean_lower = mean_higher = 0;
        for (int i=0; i<threshold; i++) {
            mean_lower += i * histogram[i];
        }
        for (int i=threshold; i<max_intensity; i++) {
            mean_higher += i * histogram[i];
        }
        mean_lower /= count_lower;
        mean_higher /= count_higher;
        previous_threshold = threshold;
        threshold = (mean_lower + mean_higher) / 2;
    }
    
    for (int i=0; i<source.rows; i++) {
        for (int j=0; j<source.cols; j++) {
            if (source.at<unsigned char>(i, j) < threshold) {
                destination.at<unsigned char>(i, j) = 0;
            } else {
                destination.at<unsigned char>(i, j) = 255;
            }
        }
    }

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

    Mat destination = segment_with_threshold(source);

    imshow("source", source);
    imshow("segmented", destination);
}