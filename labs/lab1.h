#ifndef LAB1_H
#define LAB1_H

#include "../project.h"

Mat negativeImage(Mat source);
Mat additiveProcess(Mat source, int offset);
Mat variablySubtractiveProcess(Mat source, int offset);
Mat multiplicativeProcess(Mat source, int factor);
Mat scalingProcess(Mat source, float factor_rows, float factor_cols);
Mat variableScalingProcess(Mat source, float factor_rows, float factor_cols, bool resizeImage);
void lab1();
void point5Image();
void point6matrix();

#endif//LAB1_H
