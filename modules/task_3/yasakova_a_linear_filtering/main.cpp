// Copyright 2019 Yasakova Anastasia
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <vector>
#include <algorithm>
#include "../../../modules/task_3/yasakova_a_linear_filtering/linear_filtering.h"



TEST(Linear_Filtering_MPI, test1) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        ASSERT_ANY_THROW(getImage(-1, 3));
    }
}

TEST(Linear_Filtering_MPI, test2) {
    std::vector <int> a;
    int rank;
    std::vector <int> mask = { 0, 0, 0, 0, 1, 0, 0, 0, 0 };
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        a = getImage(5, 7);
        ASSERT_ANY_THROW(ParallelLinearFilter(mask, a, 3, 2));
    }
}

TEST(Linear_Filtering_MPI, test3) {
    int rows = 4, cols = 5;
    std::vector <int> mask = { 1, 2, 1, 2, 4, 2, 1, 2, 1 };
    std::vector <int> a(rows * cols), ans_seq(rows * cols);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        a = { 150, 128, 100, 200, 175,
            100, 50, 250, 200, 255,
            0, 50, 75, 225, 130,
            145, 200, 30, 230, 100 };

        ans_seq = { 130, 123, 145, 183, 196,
            83, 98, 153, 196, 204,
            67, 85, 130, 175, 172,
            118, 132, 95, 172, 129 };
    }
    std::vector <int> ans(rows * cols);
    ans = ParallelLinearFilter(mask, a, rows, cols);
    if (rank == 0) {
        ASSERT_EQ(ans_seq, ans);
    }
}

TEST(Linear_Filtering_MPI, test4) {
    int rows = 40, cols = 80;
    std::vector <int> mask = { 1, 2, 1, 2, 4, 2, 1, 2, 1 };
    std::vector <int> a(rows * cols);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        a = getImage(rows, cols);
    }
    std::vector <int> ans(rows * cols);
    ans = ParallelLinearFilter(mask, a, rows, cols);
    std::vector <int> ans_seq(rows * cols);
    if (rank == 0) {
        ans_seq = SequentialLinearFilter(mask, a, rows, cols);
        ASSERT_EQ(ans_seq, ans);
    }
}

TEST(Linear_Filtering_MPI, test5) {
    int rows = 400, cols = 320;
    std::vector <int> mask = { 1, 2, 1, 2, 4, 2, 1, 2, 1 };
    std::vector <int> a(rows * cols);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        a = getImage(rows, cols);
    }
    std::vector <int> ans(rows * cols);
    ans = ParallelLinearFilter(mask, a, rows, cols);
    std::vector <int> ans_seq(rows * cols);
    if (rank == 0) {
        ans_seq = SequentialLinearFilter(mask, a, rows, cols);
        ASSERT_EQ(ans_seq, ans);
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
