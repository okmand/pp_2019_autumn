// Copyright 2019 Okmyanskiy Andrey
#include <iostream>
#include <random>
#include <vector>
#include <ctime>
#include <stdexcept>
#include "../../../modules/task_3/okmyanskiy_a_cannon_algorithm/cannon_algorithm.h"
std::vector<double> getRandomMatrix(int Size) {
    if (Size <= 0) {
        throw std::runtime_error("The size of the matrix <= 0");
    }
    static int s_count = 0;
    ++s_count;
    std::mt19937 gen;
    gen.seed(static_cast<unsigned int>(time(0)+s_count));
    std::vector<double> vec(Size);
    for (int i = 0; i < Size; i++) {
        vec[i] = gen() % 100;
    }
    return vec;
}
std::vector<double> Add(const std::vector<double> &A, const std::vector<double> B, int Size) {
    std::vector<double> C(Size);
    for (int i = 0; i < Size; i++) {
        C[i] = A[i] + B[i];
    }
    return C;
}
std::vector<double> Multyplication(const std::vector<double> A, const std::vector<double> B) {
    if (fabs(A.size() - B.size()) > DBL_EPSILON * fmax(fabs(A.size()), fabs(B.size()))) {
        throw std::runtime_error("Matrixes have different sizes");
    }
    std::vector<double> C(A.size());
    int root = static_cast<int>(sqrt(A.size()));
    for (int i = 0; i < root; i++) {
        for (int j = 0; j < root; j++) {
            C[i*root + j] = 0;
            for (int k = 0; k < root; k++) {
                C[i*root + j] += A[i*root + k] * B[k*root + j];
            }
        }
    }
    return C;
}

std::vector<double> getParallelMultyplication(const std::vector<double> A, const std::vector<double> B) {
    if (fabs(A.size() - B.size()) > DBL_EPSILON * fmax(fabs(A.size()), fabs(B.size()))) {
        throw std::runtime_error("Matrixes have different sizes");
    }
    int ProcRank, ProcNum;
    MPI_Status Status;
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    if (fabs(sqrt(A.size()) - ProcNum) > DBL_EPSILON * fmax(fabs(sqrt(A.size())), fabs(ProcNum))) {
        throw std::runtime_error("The size of the matrices and the number of processes are different");
    }
    int Size = ProcNum;
    int root = static_cast<int>(sqrt(Size));
    double root2 = sqrt(Size);
    if (fabs(root2 - root) > DBL_EPSILON * fmax(fabs(root2), fabs(root))) {
        throw std::runtime_error("The square root of a size is not an integer");
    }
    std::vector<double> BlockA(Size);
    std::vector<double> BlockB(Size);
    std::vector<double> BlockC(Size);
    std::vector<double> tempC(Size);
    std::vector<double> C(Size*Size);

    MPI_Datatype type, type2;
    MPI_Type_vector(root, root, Size, MPI_DOUBLE, &type);
    MPI_Type_contiguous(Size, MPI_DOUBLE, &type2);
    MPI_Type_commit(&type);
    MPI_Type_commit(&type2);

    MPI_Comm GridComm;
    int GridCoords[2];
    int dims[2] = { root, root };
    int periods[2] = { 1, 1 };
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &GridComm);
    MPI_Cart_coords(GridComm, ProcRank, 2, GridCoords);

    int left_rank, right_rank, up_rank, down_rank;
    MPI_Cart_shift(GridComm, 1, -1, &right_rank, &left_rank);
    MPI_Cart_shift(GridComm, 0, -1, &down_rank, &up_rank);

    if (ProcRank == 0) {
        for (int i = 0; i < root; ++i) {
            for (int j = 0; j < root; ++j) {
                if ((i != 0) || (j != 0)) {
                    int dest, block_coords[2] = { i, j - i };
                    if (block_coords[1] < 0) {
                        block_coords[1] += root;
                    }
                    MPI_Cart_rank(GridComm, block_coords, &dest);
                    MPI_Send(&A[i * Size * root + j * root], 1, type, dest, 1, MPI_COMM_WORLD);
                }
            }
        }
        for (int i = 0; i < root; ++i) {
            for (int j = 0; j < root; ++j) {
                if ((i != 0) || (j != 0)) {
                    int dest, block_coords[2] = { i - j, j };
                    if (block_coords[0] < 0) {
                        block_coords[0] += root;
                    }
                    MPI_Cart_rank(GridComm, block_coords, &dest);
                    MPI_Send(&B[i * Size * root + j * root], 1, type, dest, 2, MPI_COMM_WORLD);
                }
            }
        }
        for (int i = 0, j = -1; i < Size; i++) {
            if (i % root == 0) {
                j++;
            }
            BlockA[i] = A[i + j * root*(root - 1)];
            BlockB[i] = B[i + j * root*(root - 1)];
        }
    } else {
        MPI_Recv(&BlockA[0], 1, type2, 0, 1, MPI_COMM_WORLD, &Status);
        MPI_Recv(&BlockB[0], 1, type2, 0, 2, MPI_COMM_WORLD, &Status);
    }

    BlockC = Multyplication(BlockA, BlockB);
    for (int i = 1; i < root; i++) {
        MPI_Sendrecv_replace(&BlockA[0], 1, type2, left_rank, 0, right_rank, 0, MPI_COMM_WORLD, &Status);
        MPI_Sendrecv_replace(&BlockB[0], 1, type2, up_rank, 1, down_rank, 1, MPI_COMM_WORLD, &Status);
        tempC = Multyplication(BlockA, BlockB);
        BlockC = Add(BlockC, tempC, Size);
    }

    if (ProcRank == 0) {
        for (int i = 0; i < root; ++i) {
            for (int j = 0; j < root; ++j) {
                C[i * Size + j] = BlockC[i * root + j];
            }
        }
        for (int i = 0; i < dims[0]; ++i) {
            for (int j = 0; j < dims[1]; ++j) {
                if ((i != 0) || (j != 0)) {
                    int source, block_coords[2] = { i, j };
                    MPI_Cart_rank(GridComm, block_coords, &source);
                    MPI_Recv(&C[i * Size * root + j * root], 1, type, source, 4, MPI_COMM_WORLD, &Status);
                }
            }
        }
    } else {
        MPI_Send(&BlockC[0], 1, type2, 0, 4, MPI_COMM_WORLD);
    }
    MPI_Type_free(&type);
    MPI_Type_free(&type2);
    MPI_Comm_free(&GridComm);

    return C;
}
