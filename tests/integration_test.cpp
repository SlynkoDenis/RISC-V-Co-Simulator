#include <iostream>
#include "../models/functional/model.h"
#include "gtest/gtest.h"


namespace pipeline::tests {
class IntegrationTest : public ::testing::Test {
public:
    IntegrationTest() {};

    virtual ~IntegrationTest() {};
};

TEST_F(IntegrationTest, EratosthenesSieveTest) {
    auto model = functional::FunctionalModel();
    model.RunProgram("../examples/eratosthenes_sieve/eratosthenes_sieve.out");
    const auto &regfile = model.GetRegFile();
    ASSERT_EQ(regfile.ReadReg(10), 97);
}

TEST_F(IntegrationTest, FibonacciTest) {
    auto model = functional::FunctionalModel();
    model.RunProgram("../examples/fibonacci/fibonacci.out");
    const auto &regfile = model.GetRegFile();
    ASSERT_EQ(regfile.ReadReg(10), 55);
}

TEST_F(IntegrationTest, NestedLoopsTest) {
    auto model = functional::FunctionalModel();
    model.RunProgram("../examples/nested_loops/nested_loops.out");
    const auto &regfile = model.GetRegFile();
    ASSERT_EQ(regfile.ReadReg(10), -11);
}
}   // end namespace pipeline::tests
