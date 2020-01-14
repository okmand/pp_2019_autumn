// Copyright 2019-2020 Mityagina Daria
#include "../../../modules/task_3/mityagina_d_quick_sort/quick_sort_p.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cstring>
#include <ctime>
#include <random>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <iostream>

std::vector<int> getRandomVector(int size) {
  std::mt19937 gen;
  gen.seed(static_cast<unsigned int>(time(0)));
  std::vector<int> _vector = std::vector<int>(size, 0);
  for (int i = 0; i < size; ++i) {
    _vector[i] = gen() % (100 - 4);
  }
  return _vector;
}

void merge(std::vector<int> array1, std::vector<int> array2, std::vector<int> *merged, int size) {
  int i = 0, j = 0, m = 0;
  while (i < size / 2 && j < size / 2) {
    (*merged)[m++] = array1[i] < array2[j] ? array1[i++] : array2[j++];
  }
  while (i < size / 2) {
    (*merged)[m++] = array1[i++];
  }
  while (j < size / 2) {
    (*merged)[m++] = array2[j++];
  }
}

int partition(std::vector<int> *array, int start, int end) {
  int pivot = (*array)[end];
  int count = start;
  for (int j = start; j < end; j++) {
    if ((*array)[j] <= pivot) {
      int temp = (*array)[j];
      (*array)[j] = (*array)[count];
      (*array)[count] = temp;
      count += 1;
    }
  }
  (*array)[end] = (*array)[count];
  (*array)[count] = pivot;

  return count;
}
/*
  call partition to divide array into two parts
  then recursively call itself
*/
void quickSort(std::vector<int> *array, int start, int end) {
  if (start < end) {
    int pivot = partition(array, start, end);
    quickSort(array, start, pivot - 1);
    quickSort(array, pivot + 1, end);
  }
}

void quickSort(std::vector<int> *array, int size) {
  quickSort(array, 0, size - 1);
}

void main_work(std::vector<int> *A, int size) {
  int my_rank, comm_sz, easier = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
  MPI_Status status;

  if (size / comm_sz > 0 && comm_sz > 1) {
    while (size % comm_sz > 0) {
      A->push_back(7777);
      easier++; size++;
    }
    int merged_size;
    int local_size = size / comm_sz;  // so we can assume size % comm_sz == 0
    std::vector<int> local_A = std::vector<int>(local_size, 0);
    std::vector<int> current = std::vector<int>(local_size, 0);
    std::vector<int> merged = std::vector<int>(local_size, 0);

    MPI_Scatter(&(*A)[0], local_size, MPI_INT, &local_A[0], local_size, MPI_INT, 0, MPI_COMM_WORLD);
    quickSort(&local_A, 0, local_size - 1);

    int merge_tree_depth;
    if (comm_sz && !(comm_sz & (comm_sz - 1))) {
      merge_tree_depth = static_cast<int>(log2(comm_sz));
    } else {
      merge_tree_depth = static_cast<int>(log2(comm_sz * 7));
    }
    int my_index = my_rank;
    int step = 0;

    current = std::vector<int>(local_A.begin(), local_A.end());
    while (step < merge_tree_depth) {
      merged_size = (local_size) * static_cast<int>(pow(2, step + 1));  // depth of merge tree
      if (my_index % 2 == 0) {
        merged.resize(merged_size);
        std::vector<int> recv_array = std::vector<int>(merged_size/2, 0);
        int src_rank = my_rank + static_cast<int>(pow(2, step));
        if (merged_size/2 > 0 && src_rank < comm_sz && src_rank >= 0) {
          MPI_Recv(&recv_array[0], merged_size/2, MPI_INT, src_rank, 10, MPI_COMM_WORLD, &status);
        }
        merge(current, recv_array, &merged, merged_size);
        current.resize(merged_size);
        current = merged;
      } else {
        int des_rank = my_rank - static_cast<int>(pow(2, step));
        if (merged_size/2 > 0 && des_rank < comm_sz && des_rank >= 0) {
          MPI_Send(&current[0], merged_size/2, MPI_INT, des_rank, 10, MPI_COMM_WORLD);
        }
        break;
      }
      step++;
      my_index = my_index/2;
    }
    if (my_rank == 0) {
      for (int i = 0; i < size; i++) {
        (*A)[i] = merged[i + (static_cast<int>(merged.size()) - size)];
      }
    }
  } else {
    if (my_rank == 0) {
      quickSort(A, size);
    }
  }
}
