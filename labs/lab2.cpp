#include "../project.h"
#include <vector>

std::vector<Mat> splitChannels(Mat source) {
    Mat destRed(source.rows, source.cols, CV_8UC1),
        destGreen(source.rows, source.cols, CV_8UC1),
        destBlue(source.rows, source.cols, CV_8UC1);

    for (int i = 0; i < source.rows; i++) {
        for (int j = 0; j < source.cols; j++) {
            destRed.at<char>(i, j) = source.at<Vec3b>(i, j)[2];
            destGreen.at<char>(i, j) = source.at<Vec3b>(i, j)[1];
            destBlue.at<char>(i, j) = source.at<Vec3b>(i, j)[0];
        }
    }

    std::vector<Mat> result = std::vector<Mat>(3);
    result[0] = destRed;
    result[1] = destGreen;
    result[2] = destBlue;
    return result;
}

Mat combineChannels(std::vector<Mat> channels) {
    Mat dest(channels[0].rows, channels[0].cols, CV_8UC3);

    for (int i = 0; i < dest.rows; i++) {
        for (int j = 0; j < dest.cols; j++) {
            dest.at<Vec3b>(i, j) = Vec3b(
                channels[0].at<uchar>(i, j),
                channels[1].at<uchar>(i, j),
                channels[2].at<uchar>(i, j)
            );
        }
    }

    return dest;
}

Mat colorToGrayscale(Mat source) {
    Mat destination(source.rows, source.cols, CV_8UC1);

    for (int i = 0; i < source.rows; i++) {
        for (int j = 0; j < source.cols; j++) {
            Vec3b srcPixel = source.at<Vec3b>(i, j);
            destination.at<uchar>(i, j) =
                (srcPixel[0] + srcPixel[1] + srcPixel[2]) / 3;
        }
    }

    return destination;
}

Mat grayscaleToBNW(Mat source, int threshold) {
    Mat destination(source.rows, source.cols, CV_8UC1);

    for (int i = 0; i < source.rows; i++) {
        for (int j = 0; j < source.cols; j++) {
            destination.at<uchar>(i, j) =
                source.at<uchar>(i, j) < threshold ? 0 : 255;
        }
    }

    return destination;
}

Vec3b pixelRGBToHSV(Vec3b rgb) {
    Vec3b hsv = Vec3b();

    float r = ((float)rgb[2]) / 255.0,
        g = ((float)rgb[1]) / 255.0,
        b = ((float)rgb[0]) / 255.0,
        M = max(r, max(g, b)),
        m = min(r, min(g, b)),
        C = M - m,
        h, s, v;

    v = M;
    s = C / v;
    if (C > 0) {
        if (r == M) {
            h = 60 * (g - b) / C;
        } else if (g == M) {
            h = 120 + 60 * (b - r) / C;
        } else {
            h = 240 + 60 * (r - g) / C;
        }
        h = h < 0 ? h + 360 : h;
    } else {
        h = 0;
    }

    hsv[0] = v * 255;
    hsv[1] = s * 255;
    hsv[2] = h * 255 / 360;

    return hsv;
}

Vec3b pixelHSVToRGB(Vec3b hsv) {
    Vec3b rgb = Vec3b();
    int h, s, v;
    int r, g, b;

    h = hsv[2];
    s = hsv[1];
    v = hsv[0];

    return rgb;
}

Mat convertRGBToHSV(Mat source) {
    Mat destination(source.rows, source.cols, CV_8UC3);

    for (int i = 0; i < source.rows; i++) {
        for (int j = 0; j < source.cols; j++) {
            Vec3b rgb = source.at<Vec3b>(i, j);

            destination.at<Vec3b>(i, j) = pixelRGBToHSV(rgb);
        }
    }

    return destination;
}

Mat convertHSVToRGB(Mat source) {
    Mat destination(source.rows, source.cols, CV_8UC3);

    cvtColor(source, destination, COLOR_HSV2BGR_FULL);

    return destination;
}

Mat saturateByColor(Mat source, int color, int width) {
    while (color < 0) color += 256;
    Mat destination(source.rows, source.cols, CV_8UC3);
    Mat hsv = convertRGBToHSV(source);

    for (int i = 0; i < source.rows; i++) {
        for (int j = 0; j < source.cols; j++) {
            Vec3b hsvPixel = hsv.at<Vec3b>(i, j);
            int h = hsvPixel[2],
                s = hsvPixel[1],
                v = hsvPixel[0];
            int delta = min(abs(h - color), min(abs(h-256-color), abs(h+256-color)));
            float factor;
            if (delta <= width && s > 50) {
                factor = 1.0 * (width - delta) / width;
            } else {
                factor = 0;
            }
            for (int k = 0; k < 3; k++) {
                destination.at<Vec3b>(i, j)[k] = source.at<Vec3b>(i, j)[k] * factor;
            }
        }
    }

    return destination;
}

int findHueAroundPixel(Mat source, int row, int col) {
    Vec3b hsv = pixelRGBToHSV(source.at<Vec3b>(row, col));
    return hsv[2];
}

void testColorToChannels() {
    char fname[MAX_PATH];
    while (openFileDlg(fname)) {
        Mat source = imread(fname, CV_LOAD_IMAGE_COLOR);

        std::vector<Mat> channels = splitChannels(source);

        imshow("source", source);
        imshow("red channel", channels[0]);
        imshow("green channel", channels[1]);
        imshow("blue channel", channels[2]);

        waitKey();
    }
}

void testColorToGrayscale() {
    char fname[MAX_PATH];
    while (openFileDlg(fname)) {
        Mat source = imread(fname, CV_LOAD_IMAGE_COLOR);

        Mat grayscale = colorToGrayscale(source);

        imshow("source", source);
        imshow("grayscale", grayscale);

        waitKey();
    }
}

void testColorToBNW() {
    char fname[MAX_PATH];
    while (openFileDlg(fname)) {
        Mat source = imread(fname, CV_LOAD_IMAGE_COLOR);

        Mat grayscale = colorToGrayscale(source);
        Mat bnw = grayscaleToBNW(grayscale, 120);

        imshow("source", source);
        imshow("black and white", bnw);

        waitKey();
    }
}

void testRGBToHSV() {
    char fname[MAX_PATH];
    while (openFileDlg(fname)) {
        Mat source = imread(fname, CV_LOAD_IMAGE_COLOR);

        Mat hsv = convertRGBToHSV(source);
        std::vector<Mat> channels = splitChannels(hsv);

        imshow("source", source);
        imshow("hue", channels[0]);
        imshow("saturation", channels[1]);
        imshow("value", channels[2]);

        waitKey();
    }
}

void testSaturation() {
    char fname[MAX_PATH];
    while (openFileDlg(fname)) {
        Mat source = imread(fname, CV_LOAD_IMAGE_COLOR);

        Mat saturated = saturateByColor(source, 40, 30);

        imshow("saturated", saturated);

        waitKey();
    }
}

void customSaturationMouseCallback(int event, int x, int y, int flags, void* param) {
    Mat* source = (Mat*)param;
    if (event == CV_EVENT_LBUTTONDOWN) {
        int color = findHueAroundPixel(*source, y, x);
        Mat saturated = saturateByColor(*source, color, 30);

        imshow("saturated", saturated);
    }
}

void testCustomSaturation() {
    char fname[MAX_PATH];
    while (openFileDlg(fname)) {
        Mat source = imread(fname, CV_LOAD_IMAGE_COLOR);

        namedWindow("source");
        setMouseCallback("source", customSaturationMouseCallback, &source);
        imshow("source", source);

        waitKey();
    }
}


void lab2() {
    // testColorToChannels();
    // testColorToGrayscale();
    // testColorToBNW();
    // testRGBToHSV();
    // testSaturation();
    testCustomSaturation();
}