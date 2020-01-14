// Copyright 2019-2020 Mityagina Daria
#ifndef MODULES_TASK_3_MITYAGINA_D_QUICK_SORT_QUICK_SORT_P_H_
#define MODULES_TASK_3_MITYAGINA_D_QUICK_SORT_QUICK_SORT_P_H_
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

std::vector<int> getRandomVector(int size);
void merge(std::vector<int> array1, std::vector<int> array2, std::vector<int> *merged_array, int size);
int partition(std::vector<int> *array, int start, int end);
void quickSort(std::vector<int> *array, int start, int end);
void quickSort(std::vector<int> *array, int size);
void main_work(std::vector<int> *A, int size);

#endif  // MODULES_TASK_3_MITYAGINA_D_QUICK_SORT_QUICK_SORT_P_H_
