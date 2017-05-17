#ifndef LAB11_H
#define LAB11_H

#define COLOR_EDGE 255
#define COLOR_HARD_EDGE COLOR_EDGE
#define COLOR_SOFT_EDGE 128
#define COLOR_NO_EDGE 0

struct GradientVectors {
    Mat magnitude;
    Mat direction;

    GradientVectors(Mat magnitude, Mat direction) {
        this->magnitude = magnitude;
        this->direction = direction;
    }
};

void lab11_gradient_prewitt(char *fileName);
void lab11_gradient_sobel(char *fileName);
void lab11_gradient_roberts(char *fileName);
void lab11_gradient_magnitude(char *fileName);
void lab11_gradient_thresholding(char *fileName);
void lab11_canny_edge_detection(char *fileName);

#endif//LAB11_H