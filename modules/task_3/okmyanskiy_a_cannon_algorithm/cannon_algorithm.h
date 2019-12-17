// Copyright 2019 Okmyanskiy Andrey
#ifndef MODULES_TASK_3_OKMYANSKIY_A_CANNON_ALGORITHM_CANNON_ALGORITHM_H_
#define MODULES_TASK_3_OKMYANSKIY_A_CANNON_ALGORITHM_CANNON_ALGORITHM_

#include <mpi.h>
#include <vector>

std::vector<double> getRandomMatrix(int size);
std::vector<double> Multyplication(std::vector<double> A, std::vector<double> B);
void Add(std::vector<double> &A, std::vector<double> B, int Size);
std::vector<double> getParallelMultyplication(std::vector<double> A, std::vector<double> B);

#endif  // MODULES_TASK_3_OKMYANSKIY_A_CANNON_ALGORITHM_CANNON_ALGORITHM_H_
