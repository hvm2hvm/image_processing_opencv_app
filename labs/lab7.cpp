#include "../project.h"

#define OPERATION_DILATION 0
#define OPERATION_EROSION 1

bool has_all_neighbors(Mat source, Mat nucleus, int row, int col) {
    bool all = true;
    for (int i=0; i<nucleus.rows; i++) {
        for (int j=0; j<nucleus.cols; j++) {
            int ii = row + i - nucleus.rows/2, jj = col + j - nucleus.cols/2;
            if (ii == 0 && jj == 0) {
                continue;
            }
            if (ii >= 0 && ii < source.rows && jj >= 0 && jj < source.cols) {
                if (nucleus.at<uchar>(i, j) == 1) {
                    if (source.at<uchar>(ii, jj) != COLOR_OBJECT) {
                        all = false;
                        break;
                    }
                }
            }
        }
    }
    return all;
}

void set_neighbors(Mat destination, Mat nucleus, int row, int col) {
    for (int i=0; i<nucleus.rows; i++) {
        for (int j=0; j<nucleus.cols; j++) {
            int ii = row + i - nucleus.rows/2, jj = col + j - nucleus.cols/2;
            if (ii >= 0 && ii < destination.rows && jj >= 0 && jj < destination.cols) {
                if (nucleus.at<uchar>(i, j) == 1) {
                    destination.at<uchar>(ii, jj) = COLOR_OBJECT;
                }
            }
        }
    }
}

Mat morph(Mat source, Mat nucleus, int operation) {
    Mat destination(source.rows, source.cols, CV_8UC1);

    for (int i=0; i<destination.rows; i++) {
        for (int j=0; j<destination.cols; j++) {
            destination.at<uchar>(i, j) = COLOR_CANVAS;
        }
    }

    for (int i=0; i<source.rows; i++) {
        for (int j=0; j<source.cols; j++) {
            uchar current = source.at<uchar>(i, j);

            if (operation == OPERATION_DILATION) {
                if (current == COLOR_OBJECT) {
                    set_neighbors(destination, nucleus, i, j);
                }
            } else if (operation == OPERATION_EROSION) {
                if (current == COLOR_OBJECT &&
                        has_all_neighbors(source, nucleus, i, j)) {
                    destination.at<uchar>(i, j) = COLOR_OBJECT;
                }
            }
        }
    }

    return destination;
}

Mat morph_dilate(Mat source) {
    uchar nucleus_data[] = {0,1,0, 1,1,1, 0,1,0};
    Mat nucleus(3, 3, CV_8UC1);
    nucleus.data = &nucleus_data[0];

    return morph(source, nucleus, OPERATION_DILATION);
}

Mat morph_erode(Mat source) {
    uchar nucleus_data[] = {0,1,0, 1,1,1, 0,1,0};
    Mat nucleus(3, 3, CV_8UC1);
    nucleus.data = &nucleus_data[0];

    return morph(source, nucleus, OPERATION_EROSION);
}

Mat morph_multiple(Mat source, Mat nucleus, int operation, int count) {
    Mat destination = source.clone();

    for (int i=0; i<count; i++) {
        destination = morph(destination, nucleus, operation);
    }

    return destination;
}

Mat morph_open(Mat source, int strength) {
    uchar nucleus_data[] = {0,1,0, 1,1,1, 0,1,0};
    Mat nucleus(3, 3, CV_8UC1);
    nucleus.data = &nucleus_data[0];

    return morph_multiple(
            morph_multiple(source, nucleus, OPERATION_EROSION, strength),
            nucleus, OPERATION_DILATION, strength);
}

Mat morph_close(Mat source, int strength) {
    uchar nucleus_data[] = {0,1,0, 1,1,1, 0,1,0};
    Mat nucleus(3, 3, CV_8UC1);
    nucleus.data = &nucleus_data[0];

    return morph_multiple(
            morph_multiple(source, nucleus, OPERATION_DILATION, strength),
            nucleus, OPERATION_EROSION, strength);
}

Mat object_difference(Mat to_subtract_from, Mat to_subtract) {
    // the objects have to be the same size
    assert(to_subtract_from.rows == to_subtract.rows);
    assert(to_subtract_from.cols == to_subtract.cols);

    Mat destination = to_subtract_from.clone();

    for (int i=0; i<destination.rows; i++) {
        for (int j=0; j<destination.cols; j++) {
            if (to_subtract.at<uchar>(i, j) == COLOR_OBJECT) {
                destination.at<uchar>(i, j) = COLOR_CANVAS;
            }
        }
    }

    return destination;
}

Mat object_intersection(Mat obj1, Mat obj2) {
    assert(obj1.rows == obj2.rows);
    assert(obj1.cols == obj2.cols);

    Mat destination(obj1.rows, obj1.cols, CV_8UC1);

    for (int i=0; i<destination.rows; i++) {
        for (int j = 0; j < destination.cols; j++) {
            if (obj1.at<uchar>(i, j) == COLOR_OBJECT &&
                obj2.at<uchar>(i, j) == COLOR_OBJECT) {
                destination.at<uchar>(i, j) = COLOR_OBJECT;
            } else {
                destination.at<uchar>(i, j) = COLOR_CANVAS;
            }
        }
    }

    return destination;
}

Mat object_union(Mat obj1, Mat obj2) {
    assert(obj1.rows == obj2.rows);
    assert(obj1.cols == obj2.cols);

    Mat destination(obj1.rows, obj1.cols, CV_8UC1);

    for (int i=0; i<destination.rows; i++) {
        for (int j = 0; j < destination.cols; j++) {
            if (obj1.at<uchar>(i, j) == COLOR_OBJECT ||
                obj2.at<uchar>(i, j) == COLOR_OBJECT) {
                destination.at<uchar>(i, j) = COLOR_OBJECT;
            } else {
                destination.at<uchar>(i, j) = COLOR_CANVAS;
            }
        }
    }

    return destination;
}

bool object_equals(Mat obj1, Mat obj2) {
    assert(obj1.rows == obj2.rows);
    assert(obj1.cols == obj2.cols);

    for (int i=0; i<obj1.rows; i++) {
        for (int j=0; j<obj1.cols; j++) {
            if (obj1.at<uchar>(i, j) != obj2.at<uchar>(i, j)) {
                return false;
            }
        }
    }

    return true;
}

Mat object_complement(Mat object) {
    Mat destination(object.rows, object.cols, CV_8UC1);

    for (int i=0; i<destination.rows; i++) {
        for (int j = 0; j < destination.cols; j++) {
            if (object.at<uchar>(i, j) == COLOR_OBJECT) {
                destination.at<uchar>(i, j) = COLOR_CANVAS;
            } else {
                destination.at<uchar>(i, j) = COLOR_OBJECT;
            }
        }
    }
    return destination;
}

Mat boundary_extraction(Mat source) {
    uchar nucleus_data[] = {0,1,0, 1,1,1, 0,1,0};
    Mat nucleus(3, 3, CV_8UC1);
    nucleus.data = &nucleus_data[0];

    return object_difference(morph(source, nucleus, OPERATION_DILATION), source);
}

Mat region_filling(Mat source, int row, int col) {
    uchar nucleus_data[] = {0,1,0, 1,1,1, 0,1,0};
    Mat nucleus(3, 3, CV_8UC1);
    nucleus.data = &nucleus_data[0];

    Mat complement = object_complement(source);
    Mat prev(source.rows, source.cols, CV_8UC1);
    for (int i=0; i<prev.rows; i++) {
        for (int j=0; j<prev.cols; j++) {
            prev.at<uchar>(i, j) = COLOR_CANVAS;
        }
    }
    prev.at<uchar>(row, col) = COLOR_OBJECT;
    for(;;) {
        Mat next = object_intersection(complement, morph_dilate(prev));
        if (object_equals(next, prev)) {
            return object_union(next, source);
        }
        prev = next;
    }
}

void lab7_dilation(char *fileName) {
    Mat source = imread(fileName, CV_8UC1);
    Mat nucleus = initialize_filter(3, 1,1,1, 1,1,1, 1,1,1);
    Mat dilated = morph(source, nucleus, OPERATION_DILATION);

    imshow("source", source);
    imshow("dilated", dilated);
}

void lab7_erosion(char *fileName) {
    Mat source = imread(fileName, CV_8UC1);
    Mat nucleus = initialize_filter(3, 1,1,1, 1,1,1, 1,1,1);
    Mat dilated = morph(source, nucleus, OPERATION_DILATION);
    Mat eroded = morph(source, nucleus, OPERATION_EROSION);

    imshow("source", source);
    imshow("eroded", eroded);
}

void lab7_open(char *fileName) {
    Mat source = imread(fileName, CV_8UC1);
    Mat dest_open = morph_open(source, 5);

    imshow("source", source);
    imshow("open", dest_open);
}

void lab7_close(char *fileName) {
    Mat source = imread(fileName, CV_8UC1);
    Mat dest_closed = morph_close(source, 5);

    imshow("source", source);
    imshow("closed", dest_closed);
}

void lab7_boundary(char *fileName) {
    Mat source = imread(fileName, CV_8UC1);
    Mat boundary = boundary_extraction(source);

    imshow("source", source);
    imshow("boundary", boundary);
}

void lab7_region_filling(char *fileName) {
    Mat source = imread(fileName, CV_8UC1);
    Mat filled = region_filling(source, source.rows/2, source.cols/2);

    imshow("source", source);
    imshow("region filled", filled);
}