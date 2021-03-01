/*
 * expectationsTest.cpp
 *
 *  Created on: Feb 23, 2021
 *      Author: Milos Prokop
 */

#include <iostream>
#include <gtest/gtest.h>
#include "xacc.hpp"
#include <cmath>

TEST(expectationTest, getExpectationValueZ){

	auto qubitReg = xacc::qalloc(1);
	auto qpu = xacc::getAccelerator("quest");
	auto compiler = xacc::getCompiler("xasm");

	auto ir = compiler->compile(R"(__qpu__ void test(qbit q) {
		H(q[0]);
		Measure(q[0]);
	})", qpu);

	auto program = ir->getComposite("test");

	qpu->execute(qubitReg, program);

	std::cout << qubitReg -> getExpectationValueZ();

}

int main(int argc, char **argv) {

	xacc::Initialize();

	xacc::setOption("quest-verbose", "true");
	xacc::setOption("quest-testing", "true");

	::testing::InitGoogleTest(&argc, argv);
	auto ret = RUN_ALL_TESTS();
	xacc::Finalize();
	return ret;

}
