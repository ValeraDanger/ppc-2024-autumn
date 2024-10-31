// Copyright 2023 Nesterov Alexander
#include <gtest/gtest.h>

#include <numeric>
#include <vector>

#include <boost/mpi/collectives.hpp>
#include <boost/mpi/communicator.hpp>

#include "mpi/petrov_o_num_of_alternations_signs/include/ops_mpi.hpp"

TEST(Sequential, TestAlternations_Simple) {
  std::vector<int> input = {1, -2, 3, -4, 5};
  std::vector<int> output(1);  // Вектор для результата

  std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
  taskData->inputs.push_back(reinterpret_cast<uint8_t*>(input.data()));
  taskData->inputs_count.push_back(input.size());
  taskData->outputs.push_back(reinterpret_cast<uint8_t*>(output.data()));
  taskData->outputs_count.push_back(output.size());

  petrov_o_num_of_alternations_signs_mpi::SequentialTask task(taskData);

  ASSERT_TRUE(task.validation());
  ASSERT_TRUE(task.pre_processing());
  ASSERT_TRUE(task.run());
  ASSERT_TRUE(task.post_processing());

  ASSERT_EQ(output[0], 4);  // Ожидаемое количество чередований: 4
}

TEST(Sequential, TestAlternations_AllPositive) {
  std::vector<int> input = {1, 2, 3, 4, 5};
  std::vector<int> output(1);

  std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
  taskData->inputs.push_back(reinterpret_cast<uint8_t*>(input.data()));
  taskData->inputs_count.push_back(input.size());
  taskData->outputs.push_back(reinterpret_cast<uint8_t*>(output.data()));
  taskData->outputs_count.push_back(output.size());

  petrov_o_num_of_alternations_signs_mpi::SequentialTask task(taskData);

  ASSERT_TRUE(task.validation());
  ASSERT_TRUE(task.pre_processing());
  ASSERT_TRUE(task.run());
  ASSERT_TRUE(task.post_processing());

  ASSERT_EQ(output[0], 0);  // Ожидаемое количество чередований: 0
}

TEST(Sequential, TestAlternations_AllNegative) {
  std::vector<int> input = {-1, -2, -3, -4, -5};
  std::vector<int> output(1);

  std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
  taskData->inputs.push_back(reinterpret_cast<uint8_t*>(input.data()));
  taskData->inputs_count.push_back(input.size());
  taskData->outputs.push_back(reinterpret_cast<uint8_t*>(output.data()));
  taskData->outputs_count.push_back(output.size());

  petrov_o_num_of_alternations_signs_mpi::SequentialTask task(taskData);

  ASSERT_TRUE(task.validation());
  ASSERT_TRUE(task.pre_processing());
  ASSERT_TRUE(task.run());
  ASSERT_TRUE(task.post_processing());

  ASSERT_EQ(output[0], 0);  // Ожидаемое количество чередований: 0
}

TEST(Sequential, TestAlternations_Empty) {
  std::vector<int> input = {};
  std::vector<int> output(1);

  std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
  taskData->inputs.push_back(reinterpret_cast<uint8_t*>(input.data()));
  taskData->inputs_count.push_back(input.size());
  taskData->outputs.push_back(reinterpret_cast<uint8_t*>(output.data()));
  taskData->outputs_count.push_back(output.size());

  petrov_o_num_of_alternations_signs_mpi::SequentialTask task(taskData);

  ASSERT_TRUE(task.validation());
  ASSERT_TRUE(task.pre_processing());
  ASSERT_TRUE(task.run());
  ASSERT_TRUE(task.post_processing());

  ASSERT_EQ(output[0], 0);  // Ожидаемое количество чередований: 0
}

TEST(Sequential, TestAlternations_OneElement) {
  std::vector<int> input = {1};
  std::vector<int> output(1);

  std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
  taskData->inputs.push_back(reinterpret_cast<uint8_t*>(input.data()));
  taskData->inputs_count.push_back(input.size());
  taskData->outputs.push_back(reinterpret_cast<uint8_t*>(output.data()));
  taskData->outputs_count.push_back(output.size());

  petrov_o_num_of_alternations_signs_mpi::SequentialTask task(taskData);

  ASSERT_TRUE(task.validation());
  ASSERT_TRUE(task.pre_processing());
  ASSERT_TRUE(task.run());
  ASSERT_TRUE(task.post_processing());

  ASSERT_EQ(output[0], 0);  // Ожидаемое количество чередований: 0
}

TEST(Sequential, TestAlternations_LargeInput) {
  const int size = 1000;
  std::vector<int> input(size);
  std::iota(input.begin(), input.end(), 1);  // Заполняем числами от 1 до 1000
  for (size_t i = 0; i < input.size(); ++i) {
    if (i % 2 != 0) {
      input[i] *= -1;
    }
  }

  std::vector<int> output(1);

  std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
  taskData->inputs.push_back(reinterpret_cast<uint8_t*>(input.data()));
  taskData->inputs_count.push_back(input.size());
  taskData->outputs.push_back(reinterpret_cast<uint8_t*>(output.data()));
  taskData->outputs_count.push_back(output.size());

  petrov_o_num_of_alternations_signs_mpi::SequentialTask task(taskData);

  ASSERT_TRUE(task.validation());
  ASSERT_TRUE(task.pre_processing());
  ASSERT_TRUE(task.run());
  ASSERT_TRUE(task.post_processing());

  ASSERT_EQ(output[0], static_cast<int>(input.size() - 1));  // Ожидаемое количество чередований для чередующихся знаков
}

/*Параллельные тесты*/
TEST(Parallel, TestAlternations_Simple) {
  boost::mpi::communicator world;

  std::vector<int> input = {1, -2, 3, -4, 5};
  std::vector<int> output(1);  // Вектор для результата

  std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
  if (world.rank() == 0) {
    taskData->inputs.push_back(reinterpret_cast<uint8_t*>(input.data()));
    taskData->inputs_count.push_back(input.size());
    taskData->outputs.push_back(reinterpret_cast<uint8_t*>(output.data()));
    taskData->outputs_count.push_back(output.size());
  }

  petrov_o_num_of_alternations_signs_mpi::ParallelTask task(taskData);

  ASSERT_TRUE(task.validation());
  ASSERT_TRUE(task.pre_processing());
  ASSERT_TRUE(task.run());
  ASSERT_TRUE(task.post_processing());
  if (world.rank() == 0) 
    {ASSERT_EQ(output[0], 4);}  // Ожидаемое количество чередований: 4
}

TEST(Parallel, TestAlternations_AllPositive) {
  boost::mpi::communicator world;

  std::vector<int> input = {1, 2, 3, 4, 5};
  std::vector<int> output(1);

  std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
  if (world.rank() == 0) { 
    taskData->inputs.push_back(reinterpret_cast<uint8_t*>(input.data()));
    taskData->inputs_count.push_back(input.size());
    taskData->outputs.push_back(reinterpret_cast<uint8_t*>(output.data()));
    taskData->outputs_count.push_back(output.size());
  }

  petrov_o_num_of_alternations_signs_mpi::ParallelTask task(taskData);

  ASSERT_TRUE(task.validation());
  ASSERT_TRUE(task.pre_processing());
  ASSERT_TRUE(task.run());
  ASSERT_TRUE(task.post_processing());
  if (world.rank() == 0) 
    {ASSERT_EQ(output[0], 0);}  // Ожидаемое количество чередований: 0
}

TEST(Parallel, TestAlternations_AllNegative) {
  boost::mpi::communicator world;

  std::vector<int> input = {-1, -2, -3, -4, -5};
  std::vector<int> output(1);

  std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
  if (world.rank() == 0) {
    taskData->inputs.push_back(reinterpret_cast<uint8_t*>(input.data()));
    taskData->inputs_count.push_back(input.size());
    taskData->outputs.push_back(reinterpret_cast<uint8_t*>(output.data()));
    taskData->outputs_count.push_back(output.size());
  }

  petrov_o_num_of_alternations_signs_mpi::ParallelTask task(taskData);

  ASSERT_TRUE(task.validation());
  ASSERT_TRUE(task.pre_processing());
  ASSERT_TRUE(task.run());
  ASSERT_TRUE(task.post_processing());
  if (world.rank() == 0) 
    {ASSERT_EQ(output[0], 0);}  // Ожидаемое количество чередований: 0
}

TEST(Parallel, TestAlternations_Empty) {
  boost::mpi::communicator world;

  std::vector<int> input = {};
  std::vector<int> output(1);

  std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
  if (world.rank() == 0) {
    taskData->inputs.push_back(reinterpret_cast<uint8_t*>(input.data()));
    taskData->inputs_count.push_back(input.size());
    taskData->outputs.push_back(reinterpret_cast<uint8_t*>(output.data()));
    taskData->outputs_count.push_back(output.size());
  }

  petrov_o_num_of_alternations_signs_mpi::ParallelTask task(taskData);

  ASSERT_TRUE(task.validation());
  ASSERT_TRUE(task.pre_processing());
  ASSERT_TRUE(task.run());
  ASSERT_TRUE(task.post_processing());
  if (world.rank() == 0) 
    {ASSERT_EQ(output[0], 0);}  // Ожидаемое количество чередований: 0
}

TEST(Parallel, TestAlternations_OneElement) {
  boost::mpi::communicator world;

  std::vector<int> input = {1};
  std::vector<int> output(1);

  std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
  if (world.rank() == 0) {
    taskData->inputs.push_back(reinterpret_cast<uint8_t*>(input.data()));
    taskData->inputs_count.push_back(input.size());
    taskData->outputs.push_back(reinterpret_cast<uint8_t*>(output.data()));
    taskData->outputs_count.push_back(output.size());
  }

  petrov_o_num_of_alternations_signs_mpi::ParallelTask task(taskData);

  ASSERT_TRUE(task.validation());
  ASSERT_TRUE(task.pre_processing());
  ASSERT_TRUE(task.run());
  ASSERT_TRUE(task.post_processing());
  if (world.rank() == 0) 
    {ASSERT_EQ(output[0], 0);}  // Ожидаемое количество чередований: 0
}

TEST(Parallel, TestAlternations_LargeInput) {
  boost::mpi::communicator world;

  const int size = 1000;
  std::vector<int> input(size);
  std::iota(input.begin(), input.end(), 1);  // Заполняем числами от 1 до 1000
  for (size_t i = 0; i < input.size(); ++i) {
    if (i % 2 != 0) {
      input[i] *= -1;
    }
  }

  std::vector<int> output(1);

  std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
  if (world.rank() == 0) {
    taskData->inputs.push_back(reinterpret_cast<uint8_t*>(input.data()));
    taskData->inputs_count.push_back(input.size());
    taskData->outputs.push_back(reinterpret_cast<uint8_t*>(output.data()));
    taskData->outputs_count.push_back(output.size());
  }

  petrov_o_num_of_alternations_signs_mpi::ParallelTask task(taskData);

  ASSERT_TRUE(task.validation());
  ASSERT_TRUE(task.pre_processing());
  ASSERT_TRUE(task.run());
  ASSERT_TRUE(task.post_processing());
  if (world.rank() == 0) 
    {ASSERT_EQ(output[0], static_cast<int>(input.size() - 1));}  // Ожидаемое количество чередований для чередующихся знаков
}
