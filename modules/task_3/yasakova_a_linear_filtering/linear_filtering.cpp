// Copyright 2019 Yasakova Anastasia
#include <mpi.h>
#include <vector>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include "../../../modules/task_3/yasakova_a_linear_filtering/linear_filtering.h"



std::vector <int> getImage(int rows, int cols) {
    if (rows <= 0 || cols <= 0) {
        throw - 1;
    }
    std::vector <int> Matrix(rows * cols);
    std::default_random_engine random;
    random.seed(static_cast<unsigned int>(std::time(0)));
    for (int i = 0; i < rows * cols; i++) {
        Matrix[i] = random() % 256;
    }
    return Matrix;
}

int CountPixel(const std::vector <int>& mask, int mask_size, int norm,
    const std::vector <int>& mtx, int i, int j, int rows, int cols) {
    int p = 0;
    for (int l = -1; l < 2; ++l) {
        for (int k = -1; k < 2; ++k) {
            int i_ = i + l;
            int j_ = j + k;
            if (i_ * cols + j_ >= cols * rows) {
                i_ = i;
                j_ = j;
            }
            if (i_ < 0 || i_ > rows - 1) {
                i_ = i;
            }
            if (j_ < 0 || j_ > cols - 1) {
                j_ = j;
            }
            p += mtx[i_ * cols + j_] * mask[(l + 1) * 3 + k + 1];
        }
    }
    if (norm != 0) {
        p = p / norm;
    }
    if (p < 0) {
        p = 0;
    } else if (p > 255) {
        p = 255;
    }
    return p;
}

std::vector <int> ParallelLinearFilter(const std::vector<int> mask, std::vector <int> mtx, int rows, int cols) {
    if (static_cast<int>(mtx.size()) != rows * cols) {
        throw - 1;
    }
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int mask_size = sqrt(mask.size());
    int norm = std::accumulate(mask.begin(), mask.end(), 0);
    std::vector<int> index(size * 4);
    int i_ = 0, j_ = 0;
    int rows_ = 0, cols_ = 0;
    int sqrt_size = floor(sqrt(size));
    float sqrt_size_rest = sqrt(size) - sqrt_size;
    int amount_cols = cols / sqrt_size, rest_cols = cols % sqrt_size;
    int amount_rows = 0, rest_rows = 0;
    if (sqrt_size_rest == 0) {
        amount_rows = rows / sqrt_size;
        rest_rows = rows % sqrt_size;
    } else if (fabs(sqrt_size - sqrt_size_rest) < 1) {
        amount_rows = rows - static_cast<int>((rows / (sqrt_size + 0.5)))* sqrt_size;
        rest_rows = static_cast<int>(rows / (sqrt_size + 0.5)) - amount_rows;
    } else {
        amount_rows = rows / (sqrt_size + 1);
        rest_rows = rows % (sqrt_size + 1);
    }
    if (size < rows * cols) {
        MPI_Bcast(&mtx[0], rows * cols, MPI_INT, 0, MPI_COMM_WORLD);
        int different = size - sqrt_size * sqrt_size;
        if (different > 0 && rank >= sqrt_size * sqrt_size) {
            i_ = amount_rows * sqrt_size;
            sqrt_size = size - sqrt_size * sqrt_size;
            amount_cols = cols / sqrt_size;
            rest_cols = cols % sqrt_size;
            rows_ = amount_rows + rest_rows;
            if (rank % sqrt_size < rest_cols) {
                j_ = (amount_cols + 1) * (rank % sqrt_size);
                cols_ = amount_cols + 1;
            } else {
                j_ = amount_cols * (rank % sqrt_size) + rest_cols;
                cols_ = amount_cols;
            }
        } else {
            if (rank % sqrt_size < rest_cols) {
                j_ = (amount_cols + 1) * (rank % sqrt_size);
                cols_ = amount_cols + 1;
            } else {
                j_ = amount_cols * (rank % sqrt_size) + rest_cols;
                cols_ = amount_cols;
            }
            if (rank >= size - sqrt_size) {
                rows_ = amount_rows + rest_rows;
            } else {
                rows_ = amount_rows;
            }
            i_ = amount_rows * (rank / sqrt_size);
        }
        if (rank == 0) {
            MPI_Status status;
            index[0] = i_;
            index[1] = j_;
            index[2] = rows_;
            index[3] = cols_;
            for (int i = 1; i < size; i++) {
                 MPI_Recv(&index[i * 4], 4, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            }
        } else {
            std::vector<int>ind(4);
            ind[0] = i_;
            ind[1] = j_;
            ind[2] = rows_;
            ind[3] = cols_;
            MPI_Send(&ind[0], 4, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    } else {
        i_ = 0;
        j_ = 0;
        rows_ = rows;
        cols_ = cols;
    }
    std::vector<int> res(rows * cols);
    if (rank == 0) {
        for (int i = i_; i < i_ + rows_; i++) {
            for (int j = j_; j < j_ + cols_; j++) {
                res[i * cols + j] = CountPixel(mask, mask_size, norm, mtx, i, j, rows, cols);
            }
        }
        if (size < rows * cols) {
            MPI_Status status;
            for (int proc = 1; proc < size; proc++) {
                std::vector<int> res_(index[proc * 4 + 2] * index[proc * 4 + 3]);
                MPI_Recv(&res_[0], index[proc * 4 + 2] * index[proc * 4 + 3], MPI_INT, proc, 1, MPI_COMM_WORLD,
                    &status);
                int count = 0;
                for (int i = index[proc * 4]; i < index[proc * 4] + index[proc * 4 + 2]; i++) {
                    for (int j = index[proc * 4 + 1]; j < index[proc * 4 + 1] + index[proc * 4 + 3]; j++) {
                        res[i * cols + j] = res_[count];
                        count++;
                    }
                }
            }
        }
    } else {
        std::vector<int> res_;
        if (size < rows * cols) {
            for (int i = i_; i < i_ + rows_; i++) {
                for (int j = j_; j < j_ + cols_; j++) {
                    res_.push_back(CountPixel(mask, mask_size, norm, mtx, i, j, rows, cols));
                }
            }
            MPI_Send(&res_[0], rows_ * cols_, MPI_INT, 0, 1, MPI_COMM_WORLD);
        }
    }
    return res;
}


std::vector <int> SequentialLinearFilter(const std::vector<int> mask, std::vector <int> mtx, int rows, int cols) {
    if (static_cast<int>(mtx.size()) != rows * cols) {
        throw - 1;
    }
    int mask_size = sqrt(mask.size());
    int norm = std::accumulate(mask.begin(), mask.end(), 0);
    std::vector <int> res;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            res.push_back(CountPixel(mask, mask_size, norm, mtx, i, j, rows, cols));
        }
    }
    return res;
}
