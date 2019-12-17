// Copyright 2019 Okmyanskiy Andrey
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <stdio.h>
#include <mpi.h>
#include <iostream>
#include <random>
#include <vector>
#include <ctime>
#include "../../../modules/task_3/okmyanskiy_a_cannon_algorithm/cannon_algorithm.h"

double epsilon = 1E-5;
TEST(Parallel_, Test_Equals_Parallel_And_Sequintial) {
    int ProcRank, ProcNum;
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    int Size = ProcNum;
    std::vector<double> A = getRandomMatrix(Size*Size);
    std::vector<double> B = getRandomMatrix(Size*Size);
    std::vector<double> C_parallel(Size*Size);
    int root = static_cast<int> (sqrt(Size));
    double root2 = sqrt(Size);
    if (fabs(root2 - root) <= epsilon) {
        C_parallel = getParallelMultyplication(A, B);
        if (ProcRank == 0) {
            std::vector<double> C_sequintial = Multyplication(A, B);
            ASSERT_EQ(C_parallel, C_sequintial);
        }
    }
}

TEST(Parallel_, Test_Different_Size_Sequintial) {
    int ProcRank, ProcNum;
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    int Size = ProcNum;
    std::vector<double> A = getRandomMatrix(Size*Size);
    std::vector<double> B = getRandomMatrix(Size*Size + 1);
    if (ProcRank == 0) {
        ASSERT_ANY_THROW(Multyplication(A, B));
    }
}

TEST(Parallel, Test_Different_Size_Parallel) {
    int ProcRank, ProcNum;
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    int Size = ProcNum;
    std::vector<double> A = getRandomMatrix(Size*Size);
    std::vector<double> B = getRandomMatrix(Size*Size + 1);
    if (ProcRank == 0) {
        ASSERT_ANY_THROW(getParallelMultyplication(A, B));
    }
}

TEST(Parallel, Test_Different_ProcNum_And_Size_Matrix) {
    int ProcRank, ProcNum;
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    int Size = ProcNum + 1;
    std::vector<double> A = getRandomMatrix(Size);
    std::vector<double> B = getRandomMatrix(Size);
    if (ProcRank == 0) {
        ASSERT_ANY_THROW(getParallelMultyplication(A, B));
    }
}

TEST(Parallel, Test_Matrix_Size_Zero) {
    int ProcRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    if (ProcRank == 0) {
        ASSERT_ANY_THROW(getRandomMatrix(0));
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    MPI_Init(&argc, &argv);

    ::testing::AddGlobalTestEnvironment(new GTestMPIListener::MPIEnvironment);
    ::testing::TestEventListeners& listeners =
        ::testing::UnitTest::GetInstance()->listeners();

    listeners.Release(listeners.default_result_printer());
    listeners.Release(listeners.default_xml_generator());

    listeners.Append(new GTestMPIListener::MPIMinimalistPrinter);
    return RUN_ALL_TESTS();
}
