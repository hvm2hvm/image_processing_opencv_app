#ifndef LAB8_H
#define LAB8_H

Mat segment_with_threshold(Mat source);
Mat histogram_scale(Mat source, float scale);
Mat histogram_slide(Mat source, int delta);
Mat histogram_equalization(Mat source);

void lab8_histogram(char *filePath);
void lab8_threshold(char *filePath);
void lab8_histogram_scaling(char *filePath);
void lab8_histogram_sliding(char *filePath);
void lab8_gamma_correction(char *filePath);
void lab8_histogram_equalization(char *filePath);

#endif//LAB8_H