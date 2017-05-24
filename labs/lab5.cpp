#include "../project.h"
#include <vector>
#include <queue>
#include <unordered_map>

#define LABEL_NOT_COMPUTED -1
#define LABEL_NO_OBJECT 0
#define LABEL_FIRST 1

std::vector<Point> getValidNeighbors(Point point, Mat computed, bool getObjectNeighbors=false) {
    std::vector<Point> result;

    int ISTART = point.y > 0 ? -1 : 0,
        IEND = point.y < computed.rows-1 ? 1 : 0,
        JSTART = point.x > 0 ? -1 : 0,
        JEND = point.x < computed.cols-1 ? 1 : 0;

    for (int i = ISTART; i <= IEND; i++) {
        for (int j = JSTART; j <= JEND; j++) {
            if (i == 0 && j == 0) {
                continue;
            }
            int ii = point.y + i, jj = point.x + j;
            if (getObjectNeighbors) {
                if (computed.at<int>(ii, jj) >= LABEL_FIRST) {
                    result.push_back(Point(jj, ii));
                }
            } else {
                if (computed.at<int>(ii, jj) == LABEL_NOT_COMPUTED) {
                    result.push_back(Point(jj, ii));
                }
            }
        }
    }

    return result;
}

std::vector<Point> getObjectNeighbors(Point point, Mat source) {
    std::vector<Point> result;

    int ISTART = point.y > 0 ? -1 : 0,
        IEND = point.y < source.rows - 1 ? 1 : 0,
        JSTART = point.x > 0 ? -1 : 0,
        JEND = point.x < source.cols - 1 ? 1 : 0;

    for (int i = ISTART; i <= IEND; i++) {
        for (int j = JSTART; j <= JEND; j++) {
            if (i == 0 && j == 0) {
                continue;
            }
            int ii = point.y + i, jj = point.x + j;
            if (source.at<uchar>(ii, jj) == COLOR_OBJECT) {
                result.push_back(Point(jj, ii));
            }
        }
    }

    return result;
}

Mat initComputationMatrix(Mat source) {
    Mat destination(source.rows, source.cols, CV_32SC1);

    for (int i = 0; i < source.rows; i++) {
        for (int j = 0; j < source.cols; j++) {
            destination.at<int>(i, j) = LABEL_NOT_COMPUTED;
        }
    }

    return destination;
}

Mat labelObjectsBFS(Mat source) {
    Mat destination = initComputationMatrix(source);
    std::queue<Point> entries;
    int nextLabel = LABEL_FIRST;

    for (int i = 0; i < source.rows; i++) {
        for (int j = 0; j < source.cols; j++) {
            int value = source.at<uchar>(i, j);
            if (destination.at<int>(i, j) == LABEL_NOT_COMPUTED) {
                if (value == COLOR_OBJECT) {
                    int currentLabel = nextLabel++;
                    destination.at<int>(i, j) = currentLabel;
                    printf("found at (%d, %d), object: %d\n", i, j, currentLabel);

                    entries.push(Point(j, i));

                    while (!entries.empty()) {
                        Point currentPoint = entries.front();
                        entries.pop();

                        std::vector<Point> neighbors = getValidNeighbors(currentPoint, destination);

                        for (int k = 0; k < neighbors.size(); k++) {
                            int nvalue = source.at<uchar>(neighbors[k].y, neighbors[k].x);
                            if (nvalue == COLOR_OBJECT) {
                                destination.at<int>(neighbors[k].y, neighbors[k].x) = currentLabel;
                                entries.push(neighbors[k]);
                            }
                        }
                    }
                } else {
                    destination.at<int>(i, j) = LABEL_NO_OBJECT;
                }
            }
        }
    }

    return destination;
}

Mat labelObjectsMultipass(Mat source) {
    Mat destination = initComputationMatrix(source);
    int nextLabel = LABEL_FIRST;
    std::unordered_map<int, int> labelAssociation;

    for (int i = 0; i < source.rows; i++) {
        for (int j = 0; j < source.cols; j++) {
            int value = source.at<uchar>(i, j);
            if (destination.at<int>(i, j) == LABEL_NOT_COMPUTED) {
                if (value == COLOR_OBJECT) {
                    std::vector<Point> neighbors = getValidNeighbors(Point(j, i), destination, true);
                    int currentLabel;
                    if (neighbors.size() > 0) {
                        int minLabel = -1;
                        int labels[8];
                        int size = neighbors.size();
                        for (int k = 0; k < size; k++) {
                            int nlabel = destination.at<int>(neighbors[k].y, neighbors[k].x);
                            labels[k] = nlabel;
                            if (minLabel == -1 || nlabel < minLabel) {
                                minLabel = nlabel;
                            }
                        }
                        for (int k = 0; k < neighbors.size(); k++) {
                            if (labels[k] != minLabel) {
                                labelAssociation[labels[k]] = minLabel;
                            }
                        }
                        currentLabel = minLabel;
                    } else {
                        currentLabel = nextLabel++;
                    }
                    destination.at<int>(i, j) = currentLabel;
                } else {
                    destination.at<int>(i, j) = LABEL_NO_OBJECT;
                }
            }
        }
    }

    for (int i = 0; i < destination.rows; i++) {
        for (int j = 0; j < destination.cols; j++) {
            int label = destination.at<int>(i, j);
            while (labelAssociation.count(label)) {
                label = labelAssociation[label];
            }
            destination.at<int>(i, j) = label;
        }
    }

    return destination;
}

Vec3b labelToColor(int label) {
    if (label == LABEL_NO_OBJECT) {
        return Vec3b(0, 0, 0);
    }

    return Vec3b(100+label*10, label*10, label*10);
}

void displayLabels(String name, Mat computed) {
    Mat destination(computed.rows, computed.cols, CV_8UC3);

    for (int i = 0; i < computed.rows; i++) {
        for (int j = 0; j < computed.cols; j++) {
            destination.at<Vec3b>(i, j) = labelToColor(computed.at<int>(i, j));
        }
    }

    imshow(name, destination);
}

void lab5_bfs(char *fname) {
    Mat source = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
    Mat labelled = labelObjectsBFS(source);
    imshow("source", source);
    displayLabels("labelled", labelled);
}

void lab5_multipass(char *fname) {
    Mat source = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
    Mat labelled = labelObjectsMultipass(source);
    imshow("source", source);
    displayLabels("labelled", labelled);
}
