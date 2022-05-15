#include <iostream>
#include "../models/pipeline/pipeline/pipeline.h"
#include "gtest/gtest.h"


namespace pipeline::tests {
class PipelineTest : public ::testing::Test {
public:
    PipelineTest() {};

    virtual ~PipelineTest() {};
};

TEST_F(PipelineTest, EratosthenesSieveTest) {
    auto model = PipelineModel();
    model.RunProgram("../examples/eratosthenes_sieve/eratosthenes_sieve.out");
    const auto &regfile = model.GetRegFile();
    ASSERT_EQ(regfile.GetRegDirectly(10), 97);
}

TEST_F(PipelineTest, FibonacciTest) {
    auto model = PipelineModel();
    model.RunProgram("../examples/fibonacci/fibonacci.out");
    const auto &regfile = model.GetRegFile();
    ASSERT_EQ(regfile.GetRegDirectly(10), 55);
}

TEST_F(PipelineTest, NestedLoopsTest) {
    auto model = PipelineModel();
    model.RunProgram("../examples/nested_loops/nested_loops.out");
    const auto &regfile = model.GetRegFile();
    ASSERT_EQ(regfile.GetRegDirectly(10), -11);
}

TEST_F(PipelineTest, QSortTest) {
    auto model = PipelineModel();
    model.RunProgram("../examples/qsort/qsort.out");
    const auto &regfile = model.GetRegFile();
    ASSERT_EQ(regfile.GetRegDirectly(10), 0);
}

TEST_F(PipelineTest, MatrixMultTest) {
    auto model = PipelineModel();
    model.RunProgram("../examples/matrix_mult/matrix_mult.out");
    const auto &regfile = model.GetRegFile();
    ASSERT_EQ(regfile.GetRegDirectly(10), 0);
}
}   // end namespace pipeline::tests
