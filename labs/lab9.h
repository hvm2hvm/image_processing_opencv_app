#ifndef LAB9_H
#define LAB9_H

struct FrequencyDomain {
    Mat real;
    Mat img;

    FrequencyDomain(Mat real, Mat img) {
        this->real = real;
        this->img = img;
    }
};

void print_mat(Mat mat);
Mat initialize_filter(int size, int array[]);
Mat initialize_filter(int size, ...);

void display_float_data(const char *name, Mat float_data);

Mat apply_convolution_filter(Mat source, Mat filter);

void lab9_convolution_custom(char *fileName);
void lab9_mean_filter(char *fileName);
void lab9_gaussian_filter(char *fileName);
void lab9_laplace_4neighbors(char *fileName);
void lab9_laplace_8neighbors(char *fileName);
void lab9_laplace_4neighbors_lowpass(char *fileName);
void lab9_laplace_8neighbors_lowpass(char *fileName);
void lab9_fourier_transform(char *fileName);
void lab9_fourier_parameters(char *fileName);
void lab9_ideal_lowhighpass(char *fileName);
void lab9_ideal_gaussian_lowhighpass(char *fileName);

#endif//LAB9_H