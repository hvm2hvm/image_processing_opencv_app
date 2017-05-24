#include <queue>
#include "../project.h"

#define PI 3.14159265

GradientVectors compute_gradient(Mat horizontal_component, Mat vertical_component) {
    Mat magnitude(horizontal_component.rows, horizontal_component.cols, CV_8UC1);
    Mat direction(horizontal_component.rows, horizontal_component.cols, CV_32FC1);

    for (int i=0; i<horizontal_component.rows; i++) {
        for (int j=0; j<horizontal_component.cols; j++) {
            int h_val = horizontal_component.at<unsigned char>(i, j) - 128;
            int v_val = vertical_component.at<unsigned char>(i, j) - 128;
            int mag = (int)sqrt(h_val * h_val + v_val * v_val);

            magnitude.at<unsigned char>(i, j) = mag;
            direction.at<float>(i, j) = (float)atan2(v_val, h_val);
        }
    }

    return GradientVectors(magnitude, direction);
}

Mat encode_gradient_direction(Mat gradient_direction) {
    Mat destination(gradient_direction.rows, gradient_direction.cols, CV_8UC1);

    for (int i=0; i<gradient_direction.rows; i++) {
        for (int j=0; j<gradient_direction.cols; j++) {
            float dir = gradient_direction.at<float>(i, j);
            int value;
            if ((dir <= PI/8 && dir >= -PI/8) ||
                (dir >= 7*PI/8 || dir <= -7*PI/8)) {
                value = 0;
            } else if ((dir > PI/8 && dir < 3*PI/8) ||
                        (dir > -7*PI/8 && dir < -5*PI/8)) {
                value = 1;
            } else if ((dir >= 3*PI/8 && dir <= 5*PI/8) ||
                        (dir >= -5*PI/8 && dir <= -3*PI/8)) {
                value = 2;
            } else {
                value = 3;
            }
            destination.at<unsigned char>(i, j) = value;
        }
    }

    return destination;
}

Mat colorize_gradient_direction(Mat gradient_direction) {
    Mat encoded = encode_gradient_direction(gradient_direction);
    Mat destination(gradient_direction.rows, gradient_direction.cols, CV_8UC3);

    for (int i=0; i<encoded.rows; i++) {
        for (int j=0; j<encoded.cols; j++) {
            int value=encoded.at<unsigned char>(i, j);
            Vec3b color;
            switch (value) {
                case 0: color = Vec3b(255, 0, 0); break;
                case 1: color = Vec3b(0, 255, 0); break;
                case 2: color = Vec3b(0, 0, 255); break;
                default: color = Vec3b(255, 255, 0); break;
            }
            destination.at<Vec3b>(i, j) = color;
        }
    }

    return destination;
}

Mat non_maxima_suppression(GradientVectors vectors) {
    Mat encoded = encode_gradient_direction(vectors.direction);
    Mat destination(vectors.magnitude.rows, vectors.magnitude.cols, CV_8UC1);
    int dx[] = {1, 1, 0, 1};
    int dy[] = {0, 1, 1, 1};

    for (int i=1; i<vectors.magnitude.rows-1; i++) {
        for (int j=1; j<vectors.magnitude.cols-1; j++) {
            int mag = vectors.magnitude.at<unsigned char>(i, j);
            int dir = encoded.at<unsigned char>(i, j);
            int ii = i + dy[dir], ii2 = i - dy[dir];
            int jj = j + dx[dir], jj2 = j - dx[dir];
            int mm = vectors.magnitude.at<unsigned char>(ii, jj);
            int mm2 = vectors.magnitude.at<unsigned char>(ii2, jj2);

            if (mag >= mm && mag >= mm2) {
                destination.at<unsigned char>(i, j) = mag;
            } else {
                destination.at<unsigned char>(i, j) = 0;
            }
        }
    }

    return destination;
}

Mat compute_edge_types(Mat edges, float p, float k) {
    std::vector<int> histogram = compute_histogram(edges);
    int number_non_edge = (int)((1-p) * (edges.rows * edges.cols - histogram[0]));
    int threshold_high = 0, threshold_low;
    for (int sum=0,i=1; i<255; i++) {
        sum += histogram[i];
        if (sum >= number_non_edge) {
            threshold_high = i;
            break;
        }
    }
    threshold_low = (int)(k * threshold_high);
    Mat destination(edges.rows, edges.cols, CV_8UC1);
    for (int i=0; i<edges.rows; i++) {
        for (int j=0; j<edges.cols; j++) {
            unsigned char mag = edges.at<unsigned char>(i, j);
            if (mag >= threshold_high) {
                destination.at<unsigned char>(i, j) = COLOR_HARD_EDGE;
            } else if (mag >= threshold_low) {
                destination.at<unsigned char>(i, j) = COLOR_SOFT_EDGE;
            } else {
                destination.at<unsigned char>(i, j) = COLOR_NO_EDGE;
            }
        }
    }
    return destination;
}


int neighbor_dx[] = {-1,0,1,1,1,0,-1,-1};
int neighbor_dy[] = {1,1,1,0,-1,-1,-1,0};

void extend_from_point(Mat edges, Point p) {
    std::queue<Point> queue;
    for (int k=0; k<8; k++) {
        int xx = p.x + neighbor_dx[k];
        int yy = p.y + neighbor_dy[k];
        if (edges.at<unsigned char>(yy, xx) == COLOR_SOFT_EDGE) {
            edges.at<unsigned char>(yy, xx) = COLOR_HARD_EDGE;
            queue.push(Point(xx, yy));
        }
    }
    while (queue.size() > 0) {
        Point currentPoint = queue.front();
//        printf("working on point %d,%d\n", currentPoint.x, currentPoint.y);
        queue.pop();
        for (int k=0; k<8; k++) {
            int xx = currentPoint.x + neighbor_dy[k];
            int yy = currentPoint.y + neighbor_dx[k];
            if (edges.at<unsigned char>(yy, xx) == COLOR_SOFT_EDGE) {
                edges.at<unsigned char>(yy, xx) = COLOR_HARD_EDGE;
                queue.push(Point(xx, yy));
            }
        }
    }
}

Mat compute_with_edge_extension(Mat edge_types) {
    Mat destination = edge_types.clone();

    for (;;) {
        bool changed = false;
        for (int i=1; i<destination.rows-1; i++) {
            for (int j=1; j<destination.cols-1; j++) {
                if (destination.at<unsigned char>(i, j) == COLOR_HARD_EDGE) {
                    for (int k=0; k<8; k++) {
                        int xx = j+neighbor_dx[k];
                        int yy = i+neighbor_dy[k];
                        if (destination.at<unsigned char>(yy, xx) == COLOR_SOFT_EDGE) {
//                            printf("found hard edge next to soft edge: %d,%d - %d,%d\n",
//                                   j, i, xx, yy);
                            extend_from_point(destination, Point(j, i));
                            changed = true;
                            break;
                        }
                    }
                }
            }
        }
        if (!changed) {
            break;
        }
    }
    for (int i=0; i<destination.rows; i++) {
        for (int j=0; j<destination.cols; j++) {
            if (destination.at<unsigned char>(i, j) == COLOR_SOFT_EDGE) {
                destination.at<unsigned char>(i, j) = COLOR_NO_EDGE;
            }
        }
    }
    return destination;
}

void lab11_gradient_prewitt(char *fileName) {
    Mat source = imread(fileName, CV_8UC1);
    Mat horizontal_filter = initialize_filter(3, -1,0,1, -1,0,1, -1,0,1);
    Mat vertical_filter = initialize_filter(3, 1,1,1, 0,0,0, -1,-1,-1);
    Mat horizontal = apply_convolution_filter(source, horizontal_filter);
    Mat vertical = apply_convolution_filter(source, vertical_filter);

    imshow("source", source);
    imshow("prewitt horizontal", horizontal);
    imshow("prewitt vertical", vertical);
}

void lab11_gradient_sobel(char *fileName) {
    Mat source = imread(fileName, CV_8UC1);
    Mat horizontal_filter = initialize_filter(3, -1,0,1, -2,0,2, -1,0,1);
    Mat vertical_filter = initialize_filter(3, 1,2,1, 0,0,0, -1,-2,-1);
    Mat horizontal = apply_convolution_filter(source, horizontal_filter);
    Mat vertical = apply_convolution_filter(source, vertical_filter);

    imshow("source", source);
    imshow("sobel horizontal", horizontal);
    imshow("sobel vertical", vertical);
}

void lab11_gradient_roberts(char *fileName) {
    Mat source = imread(fileName, CV_8UC1);
    Mat horizontal_filter = initialize_filter(2, 1,0, 0,-1);
    Mat vertical_filter = initialize_filter(2, 0,-1, 1,0);
    Mat horizontal = apply_convolution_filter(source, horizontal_filter);
    Mat vertical = apply_convolution_filter(source, vertical_filter);

    imshow("source", source);
    imshow("roberts horizontal", horizontal);
    imshow("roberts vertical", vertical);
}

void lab11_gradient_magnitude(char *fileName) {
    Mat source = imread(fileName, CV_8UC1);
    Mat horizontal_filter = initialize_filter(3, -1,0,1, -1,0,1, -1,0,1);
    Mat vertical_filter = initialize_filter(3, 1,1,1, 0,0,0, -1,-1,-1);
    Mat horizontal = apply_convolution_filter(source, horizontal_filter);
    Mat vertical = apply_convolution_filter(source, vertical_filter);
    GradientVectors vectors = compute_gradient(horizontal, vertical);

    imshow("source", source);
    imshow("gradient magnitude", vectors.magnitude);
    // display_float_data("gradient direction", vectors.direction);
    imshow("gradient direction", colorize_gradient_direction(vectors.direction));
}

void lab11_gradient_thresholding(char *fileName) {
    Mat source = imread(fileName, CV_8UC1);
    Mat horizontal_filter = initialize_filter(3, -1,0,1, -1,0,1, -1,0,1);
    Mat vertical_filter = initialize_filter(3, 1,1,1, 0,0,0, -1,-1,-1);
    Mat horizontal = apply_convolution_filter(source, horizontal_filter);
    Mat vertical = apply_convolution_filter(source, vertical_filter);
    GradientVectors vectors = compute_gradient(horizontal, vertical);

    Mat thresholded = segment_with_threshold(vectors.magnitude);

    imshow("source", source);
    imshow("magnitude", vectors.magnitude);
    imshow("thresholded", thresholded);
}

void lab11_canny_edge_detection(char *fileName) {
    Mat source = imread(fileName, CV_8UC1);
    source = apply_gaussian_1d_filters(source, 1.0);
    Mat horizontal_filter = initialize_filter(3, -1,0,1, -2,0,2, -1,0,1);
    Mat vertical_filter = initialize_filter(3, 1,2,1, 0,0,0, -1,-2,-1);
    Mat horizontal = apply_convolution_filter(source, horizontal_filter);
    Mat vertical = apply_convolution_filter(source, vertical_filter);
    GradientVectors vectors = compute_gradient(horizontal, vertical);
    Mat non_maxima = non_maxima_suppression(vectors);
    Mat edge_types = compute_edge_types(non_maxima, 0.2, 0.7);
    Mat extended_edges = compute_with_edge_extension(edge_types);

    imshow("source", source);
//    imshow("sobel horizontal", horizontal);
//    imshow("sobel vertical", vertical);
    imshow("non maxima", non_maxima);
    imshow("edge types", edge_types);
    imshow("extended edges", extended_edges);
}
