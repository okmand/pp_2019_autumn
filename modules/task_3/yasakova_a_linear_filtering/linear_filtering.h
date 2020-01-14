// Copyright 2019 Yasakova Anastasia
#ifndef MODULES_TASK_3_YASAKOVA_A_LINEAR_FILTERING_LINEAR_FILTERING_H_
#define MODULES_TASK_3_YASAKOVA_A_LINEAR_FILTERING_LINEAR_FILTERING_H_

#include <vector>


std::vector <int> getImage(int rows, int cols);
int CountPixel(const std::vector <int>& mask, int mask_size, int norm,
    const std::vector <int>& mtx, int i, int j, int rows, int cols);
std::vector <int> ParallelLinearFilter(const std::vector<int> mask, std::vector <int> a, int rows, int cols);
std::vector <int> SequentialLinearFilter(const std::vector<int> mask, std::vector <int> a, int rows, int cols);

#endif  // MODULES_TASK_3_YASAKOVA_A_LINEAR_FILTERING_LINEAR_FILTERING_H_
