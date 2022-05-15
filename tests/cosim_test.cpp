#include <iostream>
#include "../co_simulator/co_simulator.h"
#include "gtest/gtest.h"


namespace cosimulator::tests {
class CoSimulatorTest : public ::testing::Test {
public:
    CoSimulatorTest() {};

    virtual void SetUp() {
        cosim.MakeTracesReadable(false);
        cosim.ShouldDeleteTraces(true);
    }

    virtual ~CoSimulatorTest() {};

    RV32ICoSimulator cosim;
};

TEST_F(CoSimulatorTest, EratosthenesSieveTest) {
    int res_code = cosim.TestProgram("../examples/eratosthenes_sieve/eratosthenes_sieve.out");
    ASSERT_EQ(res_code, 0);
}

TEST_F(CoSimulatorTest, FibonacciTest) {
    int res_code = cosim.TestProgram("../examples/fibonacci/fibonacci.out");
    ASSERT_EQ(res_code, 0);
}

TEST_F(CoSimulatorTest, NestedLoopsTest) {
    int res_code = cosim.TestProgram("../examples/nested_loops/nested_loops.out");
    ASSERT_EQ(res_code, 0);
}

TEST_F(CoSimulatorTest, QSortTest) {
    int res_code = cosim.TestProgram("../examples/qsort/qsort.out");
    ASSERT_EQ(res_code, 0);
}

TEST_F(CoSimulatorTest, MatrixMultTest) {
    int res_code = cosim.TestProgram("../examples/matrix_mult/matrix_mult.out");
    ASSERT_EQ(res_code, 0);
}
}   // end namespace pipeline::tests
