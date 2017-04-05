#include "../project.h"
#include <vector>

std::vector<float> grayscaleHistogram(Mat source) {
    std::vector<float> result = std::vector<float>(256);
    float numPixels = source.rows * source.cols;

    for (int i = 0; i < 256; i++) {
        result[i] = 0;
    }

    for (int i = 0; i < source.rows; i++) {
        for (int j = 0; j < source.cols; j++) {
            result[source.at<unsigned char>(i, j)] += 1;
        }
    }

    for (int i = 0; i < 256; i++) {
        result[i] = 1.0 * result[i] / numPixels;
    }

    return result;
}

std::vector<float> histogramMaximums(std::vector<float> histogram) {
    int wh = 5; float threshold = 0.0003;
    std::vector<float> result = std::vector<float>(256);

    for (int i = 0; i < 256; i++) {
        result[i] = 0;
    }

    for (int i = 0 + wh; i < 256 - wh; i++) {
        float max = 0, sum = 0;
        for (int j = -wh; j <= wh; j++) {
            float current = histogram[i + j];
            sum += current;
            if (max < current) {
                max = current;
            }
        }
        if (histogram[i] == max && histogram[i] > threshold + sum / (2 * wh + 1)) {
            result[i] = histogram[i];
        }
    }

    return result;
}

void displayHistogram(char *name, std::vector<float> histogram) {
    int height = 450, width = 512;
    Mat display = Mat(height, width, CV_8UC3);
    float max=0;

    for (int i = 0; i < 256; i++) {
        if (max < histogram[i]) {
            max = histogram[i];
        }
    }

    for (int i = 0; i < display.rows * display.cols * 3; i++) {
        display.data[i] = 0;
    }

    // rectangle(display);

    for (int i = 0; i < 256; i++) {
        Point topLeft = Point(i * width / 256, height * (1.0 - histogram[i]/max));
        Point bottomRight = Point((i + 1) * width / 256 - 1, height);
        rectangle(display, topLeft, bottomRight, CV_RGB(255,255,255), CV_FILLED);
    }

    imshow(name, display);
}

std::vector<unsigned char> thresholdColors(std::vector<float> maximums) {
    std::vector<unsigned char> result = std::vector<unsigned char>(256);
    float amount[256];
    int values[256];
    int num = 1;

    values[0] = 0;
    amount[0] = 0.003;
    for (int i = 0; i < 256; i++) {
        if (maximums[i] > 0) {
            amount[num] = maximums[i];
            values[num] = i;
            num++;
        }
    }
    values[num] = 255;
    amount[num] = 0.003;
    num++;

    for (int c = 0; c < 256; c++) {
        for (int k = 0; k < num - 1; k++) {
            if (c >= values[k] && c <= values[k + 1]) {
                if (amount[k] * (values[k + 1] - c) >
                    amount[k + 1] * (c - values[k])) {
                    result[c] = values[k];
                } else {
                    result[c] = values[k + 1];
                }
            }
        }
    }

    return result;
}

Mat multilevelThreshold(Mat source_param, bool dither=false, bool showHist=false) {
    Mat source = source_param.clone();
    Mat result = Mat(source.rows, source.cols, CV_8UC1);
    std::vector<float> maximums =
        histogramMaximums(grayscaleHistogram(source));
    if (showHist) {
        displayHistogram("maximums", maximums);
    }

    std::vector<unsigned char> colors = thresholdColors(maximums);

    for (int i = 0; i < source.rows; i++) {
        for (int j = 0; j < source.cols; j++) {
            int c = source.at<unsigned char>(i, j);
            result.at<unsigned char>(i, j) = colors[c];

            if (dither) {
                int error = c - colors[c];
                if (j < source.cols-1) {
                    source.at<unsigned char>(i, j + 1) = bounded(source.at<unsigned char>(i, j + 1) + 7 * error / 16);
                }
                if (i < source.rows - 1 && j > 0) {
                    source.at<unsigned char>(i + 1, j - 1) = bounded(source.at<unsigned char>(i + 1, j - 1) + 3 * error / 16);
                }
                if (i < source.rows - 1) {
                    source.at<unsigned char>(i + 1, j) = bounded(source.at<unsigned char>(i + 1, j) + 5 * error / 16);
                }
                if (i < source.rows - 1 && j < source.cols - 1) {
                    source.at<unsigned char>(i + 1, j + 1) = bounded(source.at<unsigned char>(i + 1, j + 1) + error / 16);
                }
            }
        }
    }

    return result;
}

void simpleHistogram() {
    char fname[MAX_PATH];
    while (openFileDlg(fname)) {
        Mat source = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);

        std::vector<float> histogram = grayscaleHistogram(source);
        std::vector<float> maximums = histogramMaximums(histogram);

        displayHistogram("histogram", histogram);
        displayHistogram("maximums", maximums);

        waitKey();
    }
}

void multilevel() {
    char fname[MAX_PATH];
    while (openFileDlg(fname)) {
        Mat source = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);

        Mat dest = multilevelThreshold(source);
        Mat dithered = multilevelThreshold(source, true);

        imshow("source", source);
        imshow("dest", dest);
        imshow("dithered", dithered);

        waitKey();
    }
}

void hsv_multilevel() {
    char fname[MAX_PATH];
    while (openFileDlg(fname)) {
        Mat source = imread(fname, CV_LOAD_IMAGE_COLOR);
        Mat hsv = convertRGBToHSV(source);
        std::vector<Mat> hsv_channels = splitChannels(hsv);

        Mat reduced = multilevelThreshold(hsv_channels[0], false, true);
        Mat dithered = multilevelThreshold(hsv_channels[0], true, false);

        imshow("hue original", hsv_channels[0]);
        imshow("hue reduced", reduced);
        // imshow("hue dithered", dithered);

        hsv_channels[0] = reduced;
        Mat hsv2 = combineChannels(hsv_channels);

        Mat rgb = convertHSVToRGB(hsv2);

        imshow("source", source);
        imshow("dest", rgb);

        waitKey();
    }
}

void lab3() {
    //multilevel();
    hsv_multilevel();
}
