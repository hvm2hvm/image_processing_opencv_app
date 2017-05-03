#include "../project.h"

void print_mat(Mat mat) {
    for (int i=0; i<mat.rows; i++) {
        for (int j=0; j<mat.cols; j++) {
            printf("%d ", mat.at<char>(i, j));
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
            int filter_value = filter.at<char>(filter_ii, filter_jj);
            result += source_value * filter_value;
        }
    }
    return result;
}

Mat apply_convolution_filter(Mat source, Mat filter) {
    Mat destination(source.rows, source.cols, CV_8UC1);
    int factor=0, edge;
    assert(filter.rows == filter.cols); // ensure filter is square
    assert(filter.rows % 2);            // ensure filter size is odd (2 * edge + 1)

    for (int i=0; i<filter.rows; i++) {
        for (int j=0; j<filter.cols; j++) {
            factor += filter.at<char>(i, j);
        }
    }
    edge = filter.rows / 2;

    for (int i=edge; i<source.rows-edge; i++) {
        for (int j=edge; j<source.cols-edge; j++) {
            int convoluted = apply_filter_on_pixel(source, i, j, filter);
            if (factor != 0) {
                convoluted /= factor;
            } else {
                convoluted += 128;
            }
            destination.at<unsigned char>(i, j) = bounded(convoluted);
        }
    }

    return destination;
}

Mat initialize_filter(int size, ...) {
    Mat filter(size, size, CV_8SC1);
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
            filter.at<char>(i, j) = (char)array[i*size+j];
        }
    }

    return filter;
}

Mat normalize_float_data(Mat float_data, bool use_log) {
    Mat intermediate(float_data.rows, float_data.cols, CV_32FC1);
    float maximum = -1;

    for (int i=0; i<float_data.rows; i++) {
        for (int j=0; j<float_data.cols; j++) {
            float src_value = float_data.at<float>(i, j);
            float dest_value = use_log ? log(1 + src_value) : src_value;
            intermediate.at<float>(i, j) = dest_value;
            if (maximum < dest_value) {
                maximum = dest_value;
            }
        }
    }

    Mat destination(float_data.rows, float_data.cols, CV_8UC1);

    for (int i=0; i<float_data.rows; i++) {
        for (int j = 0; j < float_data.cols; j++) {
            destination.at<unsigned char>(i, j) =
                bounded(255 * intermediate.at<float>(i, j) / maximum);
        }
    }

    return destination;
}

void display_float_data(const char *name, Mat float_data) {
    Mat for_display = normalize_float_data(float_data, true);
    imshow(name, for_display);
}

Mat centering_transformation(Mat source) {
    Mat destination(source.rows, source.cols, CV_32FC1);

    for (int i=0; i<source.rows; i++) {
        for (int j=0; j<source.cols; j++) {
            destination.at<float>(i, j) =
                ((i + j) & 1) ? -source.at<float>(i, j) : source.at<float>(i, j);
        }
    }

    return destination;
}

FrequencyDomain convert_to_frequency_domain(Mat source) {
    Mat float_source;
    source.convertTo(float_source, CV_32FC1);

    float_source = centering_transformation(float_source);

    Mat fourier;
    dft(float_source, fourier, DFT_COMPLEX_OUTPUT);

    Mat fourier_channels[] = {
            Mat::zeros(source.size(), CV_32F),
            Mat::zeros(source.size(), CV_32F)
    };
    split(fourier, fourier_channels);
    return FrequencyDomain(fourier_channels[0], fourier_channels[1]);
}

Mat compute_difference(Mat a, Mat b) {
    assert(a.rows == b.rows);
    assert(a.cols == b.cols);

    Mat destination(a.rows, a.cols, CV_8UC1);

    for (int i=0; i<a.rows; i++) {
        for (int j=0; j<a.cols; j++) {
            int a_value = a.at<unsigned char>(i, j);
            int b_value = b.at<unsigned char>(i, j);
            destination.at<unsigned char>(i, j) =
                bounded(128 + (a_value - b_value) / 2);
        }
    }

    return destination;
}

void display_frequency_parameters(FrequencyDomain frequency_domain) {
    Mat fourier_magnitude, fourier_phase;
    magnitude(frequency_domain.real, frequency_domain.img, fourier_magnitude);
    phase(frequency_domain.real, frequency_domain.img, fourier_phase);
    display_float_data("real", frequency_domain.real);
    display_float_data("imag", frequency_domain.img);
    display_float_data("magnitude", fourier_magnitude);
    display_float_data("phase", fourier_phase);
}

Mat convert_from_frequency_domain(FrequencyDomain frequency) {
    Mat fourier_channels[] = {frequency.real, frequency.img};

    Mat fourier_merged;
    merge(fourier_channels, 2, fourier_merged);
    Mat destination, destination_float;
    dft(fourier_merged, destination_float, DFT_INVERSE | DFT_REAL_OUTPUT | DFT_SCALE);

    destination_float = centering_transformation(destination_float);

    destination = normalize_float_data(destination_float, false);

    return destination;
}

void lab9_convolution_custom(char *fileName) {
    Mat source = imread(fileName, CV_8UC1);
    Mat filter = initialize_filter(3, 1,1,1, 1,1,1, 1,1,1);
    Mat with_filter = apply_convolution_filter(source, filter);

    imshow("source", source);
    imshow("with lowpass", with_filter);
}

void lab9_mean_filter(char *fileName) {
    Mat source = imread(fileName, CV_8UC1);
    Mat filter = initialize_filter(3, 1,1,1, 1,1,1, 1,1,1);
    Mat with_filter = apply_convolution_filter(source, filter);

    imshow("source", source);
    imshow("with mean filter", with_filter);
}

void lab9_gaussian_filter(char *fileName) {
    Mat source = imread(fileName, CV_8UC1);
    Mat filter = initialize_filter(3, 1,2,1, 2,4,2, 1,2,1);
    Mat with_filter = apply_convolution_filter(source, filter);

    imshow("source", source);
    imshow("with gaussian filter", with_filter);
}

void lab9_laplace_4neighbors(char *fileName) {
    Mat source = imread(fileName, CV_8UC1);
    Mat filter = initialize_filter(3, 0,-1,0, -1,4,-1, 0,-1,0);
    Mat with_filter = apply_convolution_filter(source, filter);

    imshow("source", source);
    imshow("with laplace 4 neighbors", with_filter);
}

void lab9_laplace_8neighbors(char *fileName) {
    Mat source = imread(fileName, CV_8UC1);
    Mat filter = initialize_filter(3, -1,-1,-1, -1,8,-1, -1,-1,-1);
    Mat with_filter = apply_convolution_filter(source, filter);

    imshow("source", source);
    imshow("with laplace 8 neighbors", with_filter);
}

void lab9_laplace_4neighbors_lowpass(char *fileName) {
    Mat source = imread(fileName, CV_8UC1);
    Mat filter = initialize_filter(3, 0,-1,0, -1,5,-1, 0,-1,0);
    Mat with_filter = apply_convolution_filter(source, filter);

    imshow("source", source);
    imshow("with laplace 4 neighbors", with_filter);
}

void lab9_laplace_8neighbors_lowpass(char *fileName) {
    Mat source = imread(fileName, CV_8UC1);
    Mat filter = initialize_filter(3, -1,-1,-1, -1,9,-1, -1,-1,-1);
    Mat with_filter = apply_convolution_filter(source, filter);

    imshow("source", source);
    imshow("with laplace 8 neighbors", with_filter);
}

void lab9_fourier_transform(char *fileName) {
    Mat source = imread(fileName, CV_8UC1);
    FrequencyDomain frequency_domain = convert_to_frequency_domain(source);
    Mat destination = convert_from_frequency_domain(frequency_domain);
    Mat difference = compute_difference(destination, source);

    imshow("source", source);
    imshow("destination", destination);
    imshow("difference", difference);
}

void lab9_fourier_parameters(char *fileName) {
    Mat source = imread(fileName, CV_8UC1);
    FrequencyDomain frequency_domain = convert_to_frequency_domain(source);

    imshow("source", source);
    display_frequency_parameters(frequency_domain);
}