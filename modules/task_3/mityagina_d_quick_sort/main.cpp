// Copyright 2019 Mityagina Daria
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <random>
#include "../../../modules/task_3/mityagina_d_quick_sort/quick_sort_p.h"

void testing_lab(int size) {
  int rank, p_size;
  std::vector<int> to_sort = std::vector<int>(size, 0);
  std::vector<int> _vector, result_my, result;
  result_my = std::vector<int>(size, 0);
  result = std::vector<int>(size, 0);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &p_size);
  if (rank == 0) {
    _vector = getRandomVector(size);
    for (int i = 0; i < size; i++)
      result[i] = _vector[i];
  }
  main_work(&result, size);
  if (rank == 0) {
    result.resize(size);
    quickSort(&_vector, size);
  }
  if (rank == 0) {
    ASSERT_EQ(result, _vector);
  }
}

TEST(Quick_Sort_MPI, Test_On_Size_13) {
  int size = 13;
  testing_lab(size);
}

TEST(Quick_Sort_MPI, Test_On_Size_1000) {
  int size = 1000;
  testing_lab(size);
}

// TEST(Quick_Sort_MPI, Test_On_Size_2555) {
//     int size = 2555;
//     testing_lab(size);
// }

// TEST(Quick_Sort_MPI, Test_On_Size_100000) {
//     int size = 100000;
//     testing_lab(size);
// }

// TEST(Quick_Sort_MPI, Test_On_Size_1000000) {
//     int size = 1000000;
//     testing_lab(size);
// }

TEST(Quick_Sort_MPI, Negative_matrix_size) {
  int size = -10;
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    ASSERT_ANY_THROW(std::vector<int> _vector(size, 0));
  }
}

int main(int argc, char **argv) {
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
