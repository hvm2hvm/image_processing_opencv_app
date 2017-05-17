#ifndef LAB11_H
#define LAB11_H

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

#endif//LAB11_H