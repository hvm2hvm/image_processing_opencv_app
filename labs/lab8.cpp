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

int compute_histogram_mean(std::vector<int> histogram) {
    long result = 0, sum = 0;
    for (int i=0; i<histogram.size(); i++) {
        result += histogram[i] * i;
        sum += histogram[i];
    }
    return result / sum;
}

Mat histogram_scale(Mat source, float scale) {
    Mat destination(source.rows, source.cols, CV_8UC1);
    float mean = compute_intensity_mean(source);
    float deviation = compute_intensity_deviation(source);
    int src_min = mean - deviation, src_max = mean + deviation;
    int dest_min = mean - deviation * scale, dest_max = mean + deviation * scale;

    for (int i = 0; i < source.rows; i++) {
       for (int j = 0; j < source.cols; j++) {
           int src = source.at<unsigned char>(i, j);
           int dest = dest_min + (src - src_min) * (dest_max - dest_min) / (src_max - src_min);
           dest = bounded(dest);
           destination.at<unsigned char>(i, j) = dest;
       }
    }

    return destination;
}

Mat histogram_slide(Mat source, int delta) {
    Mat destination(source.rows, source.cols, CV_8UC1);

    for (int i = 0; i < source.rows; i++) {
       for (int j = 0; j < source.cols; j++) {
           destination.at<unsigned char>(i, j) =
                bounded(source.at<unsigned char>(i, j) + delta);
       }
    }

    return destination;
}

Mat gamma_correction(Mat source, float gamma) {
    Mat destination(source.rows, source.cols, CV_8UC1);

    for (int i = 0; i < source.rows; i++) {
       for (int j = 0; j < source.cols; j++) {
           int src = source.at<unsigned char>(i, j);
           int dest = 255 * pow(1.0 * src / 255, gamma);
           destination.at<unsigned char>(i, j) = bounded(dest);
       }
    }

    return destination;
}

Mat histogram_equalization(Mat source) {
    Mat destination(source.rows, source.cols, CV_8UC1);
    std::vector<int> histogram = compute_histogram(source);
    std::vector<int> cumulative = compute_cumulative_histogram(histogram);
    int pixel_count = source.rows * source.cols;

    for (int i = 0; i < source.rows; i++) {
       for (int j = 0; j < source.cols; j++) {
           int src = source.at<unsigned char>(i, j);
           int dest = 255 * cumulative[src] / pixel_count;
           destination.at<unsigned char>(i, j) = bounded(dest);
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

void lab8_histogram_scaling(char *filePath) {
    Mat source = imread(filePath, CV_8UC1);

    Mat destination = histogram_scale(source, 2);

    imshow("source", source);
    display_histogram("source histogram", compute_histogram(source));
    imshow("histogram scaled", destination);
    display_histogram("scaled histogram", compute_histogram(destination));
}

void lab8_histogram_sliding(char *filePath) {
    Mat source = imread(filePath, CV_8UC1);

    Mat destination = histogram_slide(source, 50);

    imshow("source", source);
    display_histogram("source histogram", compute_histogram(source));
    imshow("histogram slided", destination);
    display_histogram("slided histogram", compute_histogram(destination));
}

void lab8_gamma_correction(char *filePath) {
    Mat source = imread(filePath, CV_8UC1);

    Mat destination = gamma_correction(source, 0.5);

    imshow("source", source);
    display_histogram("source histogram", compute_histogram(source));
    imshow("gamma corrected", destination);
    display_histogram("gamma corrected histogram", compute_histogram(destination));
}

void lab8_histogram_equalization(char *filePath) {
    Mat source = imread(filePath, CV_8UC1);

    Mat destination = histogram_equalization(source);

    imshow("source", source);
    display_histogram("source histogram", compute_histogram(source));
    imshow("histogram equalized", destination);
    display_histogram("equalized histogram", compute_histogram(destination));
}
