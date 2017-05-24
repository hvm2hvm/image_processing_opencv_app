#include "../project.h"
#include <vector>
#include <set>

struct Ratio {
    float dx, dy;
    Ratio(float dx, float dy) {
        this->dx = dx;
        this->dy = dy;
    }
};

int objectArea(Mat image, Vec3b color) {
    int result = 0;
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            if (image.at<Vec3b>(i, j) == color) {
                result += 1;
            }
        }
    }
    return result;
}

int objectWidth(Mat image, Vec3b color) {
    int minx = -1, maxx = -1;
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            if (image.at<Vec3b>(i, j) == color) {
                if (minx == -1 || minx > j) {
                    minx = j;
                }
                if (maxx == -1 || maxx < j) {
                    maxx = j;
                }
            }
        }
    }
    return maxx - minx;
}

int objectHeight(Mat image, Vec3b color) {
    int miny = -1, maxy = -1;
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            if (image.at<Vec3b>(i, j) == color) {
                if (miny == -1 || miny > i) {
                    miny = i;
                }
                if (maxy == -1 || maxy < i) {
                    maxy = i;
                }
            }
        }
    }
    return maxy - miny;
}

Point objectCenterOfMass(Mat image, Vec3b color) {
    int xsum=0, ysum=0, area=objectArea(image, color);
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            if (image.at<Vec3b>(i, j) == color) {
                xsum += j; // x=col, j=col
                ysum += i; // y=row, i=row
            }
        }
    }
    return Point(xsum / area, ysum / area);
}

double axisOfElongation(Mat image, Vec3b color) {
    Point com = objectCenterOfMass(image, color);
    int area = objectArea(image, color);
    int64 top=0, bot1=0, bot2=0;

    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            if (image.at<Vec3b>(i, j) == color) {
                top += (j - com.x) * (i - com.y);
                bot1 += (j - com.x) * (j - com.x);
                bot2 += (i - com.y) * (i - com.y);
            }
        }
    }
    return atan2(2 * top, bot1 - bot2) / 2;
}

std::vector<int> horizontalProjection(Mat image, Vec3b color) {
    std::vector<int> projection = std::vector<int>(image.rows);
    for (int i = 0; i < image.rows; i++) {
        projection[i] = 0;
    }
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            if (image.at<Vec3b>(i, j) == color) {
                projection[i] ++;
            }
        }
    }
    return projection;
}

std::vector<int> verticalProjection(Mat image, Vec3b color) {
    std::vector<int> projection = std::vector<int>(image.cols);
    for (int i = 0; i < image.cols; i++) {
        projection[i] = 0;
    }
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            if (image.at<Vec3b>(i, j) == color) {
                projection[j] ++;
            }
        }
    }
    return projection;
}

std::vector<Vec3b> findObjects(Mat image) {
    std::vector<Vec3b> result;
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            Vec3b color = image.at<Vec3b>(i, j);
            if (color != Vec3b(255, 255, 255)) {
                if (std::find(result.begin(), result.end(), color) == result.end()) {
                    result.insert(result.end(), color);
                }
            }
        }
    }
    return result;
}

void copyObject(Mat destination, Mat source, Vec3b color) {
    for (int i = 0; i < source.rows; i++) {
        for (int j = 0; j < source.cols; j++) {
            if (source.at<Vec3b>(i, j) == color) {
                destination.at<Vec3b>(i, j) = color;
            }
        }
    }
}

// drawing stuff

void drawLineFromPoint(Mat image, Point p, double angle) {
    int r = max(image.rows, image.cols);
    int dx = r * cos(angle);
    int dy = r * sin(angle);

    line(image, Point(p.x - dx, p.y - dy), Point(p.x + dx, p.y + dy), CV_RGB(63,63,63));
}

void drawCross(Mat image, Point p) {
    int dx = 5, dy = 5;
    line(image, Point(p.x - dx, p.y), Point(p.x + dx, p.y), CV_RGB(0, 0, 0));
    line(image, Point(p.x, p.y - dy), Point(p.x, p.y + dy), CV_RGB(0, 0, 0));
}

void drawHorizontalProjection(Mat image, std::vector<int> proj, int offset, int direction, float scale, Vec3b color) {
    for (int i = 0; i < image.rows; i++) {
        Point start(offset, i), end(offset + proj[i] * direction * scale, i);
        line(image, start, end, CvScalar(color));
    }
}

void drawVerticalProjection(Mat image, std::vector<int> proj, int offset, int direction, float scale, Vec3b color) {
    for (int i = 0; i < image.cols; i++) {
        Point start(i, offset), end(i, offset + proj[i] * direction * scale);
        line(image, start, end, CvScalar(color));
    }
}

void drawObjectProperties(Mat image, Vec3b color) {
    Mat dest = image.clone(), projections = Mat::zeros(image.rows, image.cols, CV_8UC3);
    Point centerOfMass = objectCenterOfMass(image, color);
    double axis = axisOfElongation(image, color);
    int width = objectWidth(image, color);
    int height = objectHeight(image, color);
    std::vector<int> hproj = horizontalProjection(image, color);
    std::vector<int> vproj = verticalProjection(image, color);

    printf("value under cursor: %d, %d, %d\n", color[0], color[1], color[2]);
    printf("area = %d\n", objectArea(image, color));
    printf("center of mass: x = %d, y = %d\n", centerOfMass.x, centerOfMass.y);
    printf("axis of elongation: %f\n", axis);
    printf("aspect ratio: %d/%d = %5.2f\n", width + 1, height + 1, 1.0 * (width + 1) / (height + 1));

    drawCross(dest, centerOfMass);
    drawLineFromPoint(dest, centerOfMass, axis);
    if (centerOfMass.x > image.cols / 2) {
        drawHorizontalProjection(projections, hproj, 0, 1, 1.0, color);
    } else {
        drawHorizontalProjection(projections, hproj, dest.cols, -1, 1.0, color);
    }
    if (centerOfMass.y > image.rows / 2) {
        drawVerticalProjection(projections, vproj, 0, 1, 1.0, color);
    } else {
        drawVerticalProjection(projections, vproj, dest.rows, -1, 1.0, color);
    }
    imshow("properties", dest);
    imshow("projections", projections);
}

void filterAndShowObjects(Mat source) {
    Mat destination = Mat::ones(source.rows, source.cols, CV_8UC3);
    std::vector<Vec3b> objects = findObjects(source);
    int areasum = 0, averagearea;
    std::vector<int> areas(objects.size());
    printf("objects: \n");
    for (int i = 0; i < objects.size(); i++) {
        areas[i] = objectArea(source, objects[i]);
        printf("  %d, %d, %d - area = %d\n", objects[i][0], objects[i][1], objects[i][2], areas[i]);
        areasum += areas[i];
    }
    averagearea = areasum / objects.size();
    printf("average area: %d\n", averagearea);
    for (int i = 0; i < objects.size(); i++) {
        if (areas[i] > averagearea) {
            copyObject(destination, source, objects[i]);
        }
    }
    imshow("filtered objects", destination);
}

void objectClickCallback(int event, int x, int y, int flags, void* param) {
    Mat* source = (Mat*)param;
    if (event == CV_EVENT_LBUTTONDOWN) {
        Vec3b color = source->at<Vec3b>(y, x);
        drawObjectProperties(*source, color);
    }
}

void lab4_something(char *fname) {

}

void lab4_process_objects(char *fname) {
    Mat *source = new Mat();
    *source = cvLoadImage(fname, CV_LOAD_IMAGE_COLOR);

    namedWindow("source");
    setMouseCallback("source", objectClickCallback, source);
    imshow("source", *source);

    filterAndShowObjects(*source);
}
