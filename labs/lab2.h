#ifndef LAB2_H
#define LAB2_H

#include "../project.h"

void lab2();
Mat convertRGBToHSV(Mat source);
std::vector<Mat> splitChannels(Mat source);
Mat convertHSVToRGB(Mat source);
Mat combineChannels(std::vector<Mat> channels);

#endif//LAB2_H
