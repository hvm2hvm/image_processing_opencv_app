#ifndef LAB10_H
#define LAB10_H

struct Gaussian1DFilters {
    Mat vertical;
    Mat horizontal;

    Gaussian1DFilters(Mat vertical, Mat horizontal) {
        this->vertical = vertical;
        this->horizontal = horizontal;
    }
};

Gaussian1DFilters initialize_gaussian_1d_filters(float sigma);
Mat apply_gaussian_1d_filters(Mat source, float sigma);

void lab10_median_filter(char *fileName);
void lab10_gaussian_filter(char *fileName);
void lab10_gaussian_1d_filter(char *fileName);

#endif//LAB10_H