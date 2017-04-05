#include "../project.h"

Mat negativeImage(Mat source) {
    Mat destination = source.clone();

    for (int i = 0; i < destination.rows; i++) {
        for (int j = 0; j < destination.cols; j++) {
            Vec3b vec = destination.at<Vec3b>(i, j);
            for (int k = 0; k < 3; k++) {
                vec[k] = 255 - vec[k];
            }
            destination.at<Vec3b>(i, j) = vec;
        }
    }

    imshow("negative image", destination);

    return destination;
}

Mat additiveProcess(Mat source, int offset) {
    Mat destination = source.clone();

    for (int i = 0; i < destination.rows; i++) {
        for (int j = 0; j < destination.cols; j++) {
            Vec3b vec = destination.at<Vec3b>(i, j);
            for (int k = 0; k < 3; k++) {
                vec[k] = bounded(vec[k] + offset);
            }
            destination.at<Vec3b>(i, j) = vec;
        }
    }

    imshow("additive image", destination);

    return destination;
}

Mat variablySubtractiveProcess(Mat source, int offset) {
    Mat destination = source.clone();

    int center_i = destination.rows / 2, center_j = destination.cols / 2,
        delta_max = sqrt(center_i * center_i + center_j * center_j);

    for (int i = 0; i < destination.rows; i++) {
        for (int j = 0; j < destination.cols; j++) {
            Vec3b vec = destination.at<Vec3b>(i, j);
            int delta = sqrt((i - center_i) * (i - center_i) + (j - center_j) * (j - center_j));
            for (int k = 0; k < 3; k++) {
                vec[k] = bounded(vec[k] - offset * delta / delta_max * delta / delta_max);
            }
            destination.at<Vec3b>(i, j) = vec;
        }
    }

    imshow("subtractive image", destination);

    return destination;
}

Mat multiplicativeProcess(Mat source, int factor) {
    Mat destination = source.clone();

    for (int i = 0; i < destination.rows; i++) {
        for (int j = 0; j < destination.cols; j++) {
            Vec3b vec = destination.at<Vec3b>(i, j);
            for (int k = 0; k < 3; k++) {
                vec[k] = bounded(vec[k] * factor);
            }
            destination.at<Vec3b>(i, j) = vec;
        }
    }

    imshow("multiplicative image", destination);

    return destination;
}

Mat zoomingProcess(Mat source, float factor_rows, float factor_cols) {
    Mat destination = source.clone();
    int center_i = destination.rows / 2,
        center_j = destination.cols / 2;

    for (int i = 0; i < destination.rows; i++) {
        for (int j = 0; j < destination.cols; j++) {
            int si = (i - center_i) / factor_rows + center_i,
                sj = (j - center_j) / factor_cols + center_j;
            if (si >= 0 && si < source.rows &&
                sj >= 0 && sj < source.cols) {
                destination.at<Vec3b>(i, j) = source.at<Vec3b>(si, sj);
            }
            else {
                destination.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
            }
        }
    }

    imshow("zoomed image", destination);

    return destination;
}

Mat scalingProcess(Mat source, float factor_rows, float factor_cols) {
    int source_rows = source.rows,
        source_cols = source.cols,
        destination_rows = source_rows * factor_rows,
        destination_cols = source_cols * factor_cols;

    Mat destination = Mat(destination_rows, destination_cols, CV_8UC3);

    for (int i = 0; i < destination_rows; i++) {
        for (int j = 0; j < destination_cols; j++) {
            int si = i * source_rows / destination_rows,
                ji = j * source_cols / destination_cols;
            destination.at<Vec3b>(i, j) = source.at<Vec3b>(si, ji);
        }
    }

    imshow("scaled image", destination);

    return destination;
}

Mat variableScalingProcess(Mat source, float factor_rows, float factor_cols, bool resizeImage) {
    int source_rows = source.rows,
        source_cols = source.cols,
        destination_rows = source_rows * factor_rows,
        destination_cols = source_cols * factor_cols,
        delta_max = sqrt(destination_rows / 2 * destination_rows / 2 + destination_cols / 2 * destination_cols / 2);

    if (!resizeImage) {
        destination_rows = source_rows;
        destination_cols = source_cols;
    }

    Mat destination = Mat(destination_rows, destination_cols, CV_8UC3);

    for (int i = 0; i < destination_rows; i++) {
        for (int j = 0; j < destination_cols; j++) {
            int di = (i - destination_rows / 2),
                dj = (j - destination_cols / 2),
                delta = sqrt(di * di + dj * dj);
            int si = di / (factor_rows * (1 + delta / delta_max)) + source_rows / 2,
                ji = dj / (factor_cols * (1 + delta / delta_max)) + source_cols / 2;

            if (si >= 0 && si < source_rows &&
                ji >= 0 && ji < source_cols) {

                destination.at<Vec3b>(i, j) = source.at<Vec3b>(si, ji);
            }
            else {
                destination.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
            }
        }
    }

    imshow("variably scaled image", destination);

    return destination;
}

void point5Image() {
    char fname[MAX_PATH];
    while (1) {
        int size = 1024;
        Mat source = Mat(size, size, CV_8UC3);

        Vec3b colors[2][2] = {
            { Vec3b(255, 255, 255), Vec3b(0, 0, 255) },
            { Vec3b(0, 255, 0), Vec3b(0, 255, 255) }
        };

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                // source.at<Vec3b>(i, j) = colors[i / (size / 2)][j / (size / 2)];
                /*
                source.at<Vec3b>(i, j) = Vec3b(
                (255 - (255 * i /size) + 255 - (255 * j / size)) / 2,
                255 * i / size,
                255 * j / size
                );
                */
                if (j % 4 == 0) {
                    source.at<Vec3b>(i, j) = Vec3b(0, 0, 255);
                }
                else if (j % 4 == 1) {
                    source.at<Vec3b>(i, j) = Vec3b(0, 255, 0);
                }
                else if (j % 4 == 2) {
                    source.at<Vec3b>(i, j) = Vec3b(255, 0, 0);
                }
                else {
                    source.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
                }
            }
        }

        imshow("source", source);
        waitKey();
    }
}

void point6matrix() {
    Mat initial = Mat(3, 3, CV_32FC1);

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            initial.at<float>(i, j) = 0;
        }
    }

    for (int k = 0; k < 3; k++) {
        initial.at<float>(k, k) = 1;
    }
    initial.at<float>(0, 2) = 12;

    printf("Initial: \n");

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%5.2f, ", initial.at<float>(i, j));
        }
        printf("\n");
    }

    Mat inverse = initial.clone();
    inverse.inv();

    printf("Inverse: \n");
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%5.2f, ", inverse.at<float>(i, j));
        }
        printf("\n");
    }

    waitKey();
    waitKey();
}

void lab1()
{
    char fname[256];
    while (openFileDlg(fname))
    {
        Mat source = imread(fname, CV_LOAD_IMAGE_COLOR);

        // negativeImage(source);
        // additiveProcess(source, 50);
        // subtractiveProcess(source, -255);
        // multiplicativeProcess(source, 3);
        // zoomingProcess(source, 0.2, 0.9);
        // scalingProcess(source, 2, 3);
        Mat variablyScaled = variableScalingProcess(source, 2, 2, true);
        variablySubtractiveProcess(variablyScaled, 255);

        imshow("source", source);
        waitKey();
    }
}