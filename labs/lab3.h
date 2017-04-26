#ifndef LAB3_H
#define LAB3_H

#include "../project.h"

std::vector<int> compute_histogram(Mat source);
std::vector<float> compute_normalized_histogram(Mat source);
void display_histogram(const char *name, std::vector<int> histogram);
void display_normalized_histogram(const char *name, std::vector<float> histogram);

void lab3();

#endif//LAB3_H
