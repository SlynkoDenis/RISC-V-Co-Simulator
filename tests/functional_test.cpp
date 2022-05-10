#include <iostream>
#include "../models/functional/model.h"
#include "gtest/gtest.h"


namespace functional::tests {
class FunctionalTest : public ::testing::Test {
public:
    FunctionalTest() {};

    virtual ~FunctionalTest() {};
};

TEST_F(FunctionalTest, EratosthenesSieveTest) {
    auto model = FunctionalModel();
    model.RunProgram("../examples/eratosthenes_sieve/eratosthenes_sieve.out");
    const auto &regfile = model.GetRegFile();
    ASSERT_EQ(regfile.ReadReg(10), 97);
}

TEST_F(FunctionalTest, FibonacciTest) {
    auto model = FunctionalModel();
    model.RunProgram("../examples/fibonacci/fibonacci.out");
    const auto &regfile = model.GetRegFile();
    ASSERT_EQ(regfile.ReadReg(10), 55);
}

TEST_F(FunctionalTest, NestedLoopsTest) {
    auto model = FunctionalModel();
    model.RunProgram("../examples/nested_loops/nested_loops.out");
    const auto &regfile = model.GetRegFile();
    ASSERT_EQ(regfile.ReadReg(10), -11);
}
}   // end namespace pipeline::tests
