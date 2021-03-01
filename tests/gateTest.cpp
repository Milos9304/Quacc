/*
 * gateTest.cpp
 *
 *  Created on: Feb 15, 2021
 *      Author: Milos Prokop
 */

#include <iostream>
#include <gtest/gtest.h>
#include "xacc.hpp"
#include <cmath>

bool differs(double a, double b){

	if(std::abs(a-b) > 1e-3)
		return true;

	return false;

}

bool stateVectorEq(std::vector<double> &realA, std::vector<double> &imagA, std::vector<double> &realB, std::vector<double> &imagB){

	size_t size = realA.size();

	if(size != imagA.size() || size != realB.size() || size != imagB.size())
		return false;

	double prob_sum = 0;
	for(size_t i = 0; i < size; ++i){

		if(differs(realA[i],realB[i]) or differs(imagA[i],imagB[i]))
			return false;

		prob_sum += realA[i] * realA[i] + imagA[i] * imagA[i];

	}

	if(std::abs(1-prob_sum) > 1e-3)
		return false;

	return true;

}

/*00: 1 0 0 0
 *01: 0 1 0 0 X(q[0])
 *10: 0 0 1 0 X(q[1])
 *11: 0 0 0 1
 */

TEST (gateTest, PauliX) {

	double e_statevect_real[] = { 0., 1., 0., 0.};
	double e_statevect_imag[] = { 0., 0., 0., 0.};

	std::vector<double> expected_statevect_real(std::begin(e_statevect_real), std::end(e_statevect_real));
	std::vector<double> expected_statevect_imag(std::begin(e_statevect_imag), std::end(e_statevect_imag));

	auto qubitReg = xacc::qalloc(2);
	auto qpu = xacc::getAccelerator("quest");
	auto compiler = xacc::getCompiler("xasm");

	auto ir = compiler->compile(R"(__qpu__ void test(qbit q) {
		X(q[1]);
		X(q[0]);
		X(q[1]);
	})", qpu);

	auto program = ir->getComposite("test");

	qpu->execute(qubitReg, program);

	std::vector<double> statevect_real = qubitReg->getInformation("statevect_real").as<std::vector<double>>();
	std::vector<double> statevect_imag = qubitReg->getInformation("statevect_imag").as<std::vector<double>>();

	ASSERT_TRUE(stateVectorEq(statevect_real, statevect_imag, expected_statevect_real, expected_statevect_imag));

}

TEST (gateTest, PauliY) {

	double e_statevect_real[] = { 0., 1., 0., 0.};
	double e_statevect_imag[] = { 0., 0., 0., 0.};

	std::vector<double> expected_statevect_real(std::begin(e_statevect_real), std::end(e_statevect_real));
	std::vector<double> expected_statevect_imag(std::begin(e_statevect_imag), std::end(e_statevect_imag));

	auto qubitReg = xacc::qalloc(2);
	auto qpu = xacc::getAccelerator("quest");
	auto compiler = xacc::getCompiler("xasm");

	auto ir = compiler->compile(R"(__qpu__ void test(qbit q) {
		X(q[1]);
		Y(q[0]);
		Y(q[1]);
	})", qpu);

	auto program = ir->getComposite("test");

	qpu->execute(qubitReg, program);

	std::vector<double> statevect_real = qubitReg->getInformation("statevect_real").as<std::vector<double>>();
	std::vector<double> statevect_imag = qubitReg->getInformation("statevect_imag").as<std::vector<double>>();

	ASSERT_TRUE(stateVectorEq(statevect_real, statevect_imag, expected_statevect_real, expected_statevect_imag));

}

TEST (gateTest, PauliZ) {

	double e_statevect_real[] = { 0.,  0., -1., 0.};
	double e_statevect_imag[] = { 0.,  0., 0., 0.};

	std::vector<double> expected_statevect_real(std::begin(e_statevect_real), std::end(e_statevect_real));
	std::vector<double> expected_statevect_imag(std::begin(e_statevect_imag), std::end(e_statevect_imag));

	auto qubitReg = xacc::qalloc(2);
	auto qpu = xacc::getAccelerator("quest");
	auto compiler = xacc::getCompiler("xasm");

	auto ir = compiler->compile(R"(__qpu__ void test(qbit q) {
		X(q[1]);
		Z(q[0]);
		Z(q[1]);
	})", qpu);

	auto program = ir->getComposite("test");

	qpu->execute(qubitReg, program);

	std::vector<double> statevect_real = qubitReg->getInformation("statevect_real").as<std::vector<double>>();
	std::vector<double> statevect_imag = qubitReg->getInformation("statevect_imag").as<std::vector<double>>();

	ASSERT_TRUE(stateVectorEq(statevect_real, statevect_imag, expected_statevect_real, expected_statevect_imag));

}

TEST (gateTest, Hadamard) {

	double e_statevect_real[] = { 0.5,  0.5, -0.5, -0.5};
	double e_statevect_imag[] = { 0.,  0., 0., 0.};

	std::vector<double> expected_statevect_real(std::begin(e_statevect_real), std::end(e_statevect_real));
	std::vector<double> expected_statevect_imag(std::begin(e_statevect_imag), std::end(e_statevect_imag));

	auto qubitReg = xacc::qalloc(2);
	auto qpu = xacc::getAccelerator("quest");
	auto compiler = xacc::getCompiler("xasm");

	auto ir = compiler->compile(R"(__qpu__ void test(qbit q) {
		X(q[1]);
		H(q[0]);
		H(q[1]);
	})", qpu);

	auto program = ir->getComposite("test");

	qpu->execute(qubitReg, program);

	std::vector<double> statevect_real = qubitReg->getInformation("statevect_real").as<std::vector<double>>();
	std::vector<double> statevect_imag = qubitReg->getInformation("statevect_imag").as<std::vector<double>>();


	ASSERT_TRUE(stateVectorEq(statevect_real, statevect_imag, expected_statevect_real, expected_statevect_imag));

}

TEST (gateTest, Rx) {

	double e_statevect_real[] = { 0.,  -1./4., 3./4., 0.};
	double e_statevect_imag[] = { -sqrt(3)/4., 0., 0., -sqrt(3)/4.};

	std::vector<double> expected_statevect_real(std::begin(e_statevect_real), std::end(e_statevect_real));
	std::vector<double> expected_statevect_imag(std::begin(e_statevect_imag), std::end(e_statevect_imag));

	auto qubitReg = xacc::qalloc(2);
	auto qpu = xacc::getAccelerator("quest");
	auto compiler = xacc::getCompiler("xasm");

	auto ir = compiler->compile(R"(__qpu__ void test(qbit q, double theta) {
		X(q[1]);
		Rx(q[0], theta);
		Rx(q[1], theta);
	})", qpu);

	auto program = ir->getComposite("test");
	auto evalved = program -> operator ()({M_PI/3});

	qpu->execute(qubitReg, evalved);

	std::vector<double> statevect_real = qubitReg->getInformation("statevect_real").as<std::vector<double>>();
	std::vector<double> statevect_imag = qubitReg->getInformation("statevect_imag").as<std::vector<double>>();

	ASSERT_TRUE(stateVectorEq(statevect_real, statevect_imag, expected_statevect_real, expected_statevect_imag));

}

TEST (gateTest, Ry) {

	double e_statevect_real[] = { -sqrt(3)/4, 3./4., -1./4., sqrt(3)/4};
	double e_statevect_imag[] = { 0.,  0., 0., 0.};

	std::vector<double> expected_statevect_real(std::begin(e_statevect_real), std::end(e_statevect_real));
	std::vector<double> expected_statevect_imag(std::begin(e_statevect_imag), std::end(e_statevect_imag));

	auto qubitReg = xacc::qalloc(2);
	auto qpu = xacc::getAccelerator("quest");
	auto compiler = xacc::getCompiler("xasm");

	auto ir = compiler->compile(R"(__qpu__ void test(qbit q, double theta) {
		X(q[0]);
		Ry(q[0], theta);
		Ry(q[1], theta);
	})", qpu);

	auto program = ir->getComposite("test");
	auto evalved = program -> operator ()({M_PI/3});

	qpu->execute(qubitReg, evalved);

	std::vector<double> statevect_real = qubitReg->getInformation("statevect_real").as<std::vector<double>>();
	std::vector<double> statevect_imag = qubitReg->getInformation("statevect_imag").as<std::vector<double>>();

	ASSERT_TRUE(stateVectorEq(statevect_real, statevect_imag, expected_statevect_real, expected_statevect_imag));

}

TEST (gateTest, Rz) {

	double e_statevect_real[] = { sqrt(3)/2,  0.};
	double e_statevect_imag[] = { -1/2., 0.};

	std::vector<double> expected_statevect_real(std::begin(e_statevect_real), std::end(e_statevect_real));
	std::vector<double> expected_statevect_imag(std::begin(e_statevect_imag), std::end(e_statevect_imag));

	auto qubitReg = xacc::qalloc(1);
	auto qpu = xacc::getAccelerator("quest");
	auto compiler = xacc::getCompiler("xasm");

	auto ir = compiler->compile(R"(__qpu__ void test(qbit q, double theta) {
		Rz(q[0], theta);
	})", qpu);

	auto program = ir->getComposite("test");
	auto evalved = program -> operator ()({M_PI/3});

	qpu->execute(qubitReg, evalved);

	std::vector<double> statevect_real = qubitReg->getInformation("statevect_real").as<std::vector<double>>();
	std::vector<double> statevect_imag = qubitReg->getInformation("statevect_imag").as<std::vector<double>>();

	ASSERT_TRUE(stateVectorEq(statevect_real, statevect_imag, expected_statevect_real, expected_statevect_imag));

}

TEST (gateTest, U) {

	// U(x,-pi/2,pi/2)=Rx(x)

	auto qubitReg = xacc::qalloc(1);
	auto qpu = xacc::getAccelerator("quest");
	auto compiler = xacc::getCompiler("xasm");

	auto ir = compiler->compile(R"(__qpu__ void test(qbit q, double theta) {
		Rx(q[0], theta);
	})", qpu);

	auto program = ir->getComposite("test");
	auto evalved = program -> operator ()({M_PI/3});

	qpu->execute(qubitReg, evalved);

	std::vector<double> expected_statevect_real = qubitReg->getInformation("statevect_real").as<std::vector<double>>();
	std::vector<double> expected_statevect_imag = qubitReg->getInformation("statevect_imag").as<std::vector<double>>();

	auto qubitReg2 = xacc::qalloc(1);
	auto qpu2 = xacc::getAccelerator("quest");
	auto compiler2 = xacc::getCompiler("xasm");

	auto ir2 = compiler->compile(R"(__qpu__ void test2(qbit q, double x, double y, double z) {
		U(q[0], x,y,z);
	})", qpu);

	auto program2 = ir2->getComposite("test2");
	auto evalved2 = program2 -> operator ()({M_PI/3, -M_PI/2, M_PI/2});

	qpu->execute(qubitReg2, evalved2);

	std::vector<double> statevect_real = qubitReg2->getInformation("statevect_real").as<std::vector<double>>();
	std::vector<double> statevect_imag = qubitReg2->getInformation("statevect_imag").as<std::vector<double>>();

	ASSERT_TRUE(stateVectorEq(statevect_real, statevect_imag, expected_statevect_real, expected_statevect_imag));

}

TEST (gateTest, CNOT) {

	double e_statevect_real[] = { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 1., 0., 0., 0., 0., 0.};
	double e_statevect_imag[] = { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.};

	std::vector<double> expected_statevect_real(std::begin(e_statevect_real), std::end(e_statevect_real));
	std::vector<double> expected_statevect_imag(std::begin(e_statevect_imag), std::end(e_statevect_imag));

	auto qubitReg = xacc::qalloc(4);
	auto qpu = xacc::getAccelerator("quest");
	auto compiler = xacc::getCompiler("xasm");

	auto ir = compiler->compile(R"(__qpu__ void test(qbit q) {
		X(q[1]);
		CNOT(q[0], q[2]); 
		CNOT(q[1], q[3]);
	})", qpu);

	auto program = ir->getComposite("test");

	qpu->execute(qubitReg, program);

	std::vector<double> statevect_real = qubitReg->getInformation("statevect_real").as<std::vector<double>>();
	std::vector<double> statevect_imag = qubitReg->getInformation("statevect_imag").as<std::vector<double>>();

	/*for(auto &m: statevect_real)
			std::cout<<m<<std::endl;

		std::cout<<std::endl;

		for(auto &m: statevect_imag)
				std::cout<<m<<std::endl;*/

	ASSERT_TRUE(stateVectorEq(statevect_real, statevect_imag, expected_statevect_real, expected_statevect_imag));

}

TEST (gateTest, SWAP) {

	double e_statevect_real[] = { 0., 1., 0., 0.};
	double e_statevect_imag[] = { 0., 0., 0., 0.};

	std::vector<double> expected_statevect_real(std::begin(e_statevect_real), std::end(e_statevect_real));
	std::vector<double> expected_statevect_imag(std::begin(e_statevect_imag), std::end(e_statevect_imag));

	auto qubitReg = xacc::qalloc(2);
	auto qpu = xacc::getAccelerator("quest");
	auto compiler = xacc::getCompiler("xasm");

	auto ir = compiler->compile(R"(__qpu__ void test(qbit q) {
		X(q[1]);
		Swap(q[0], q[1]); 
	})", qpu);

	auto program = ir->getComposite("test");

	qpu->execute(qubitReg, program);

	std::vector<double> statevect_real = qubitReg->getInformation("statevect_real").as<std::vector<double>>();
	std::vector<double> statevect_imag = qubitReg->getInformation("statevect_imag").as<std::vector<double>>();

	/*for(auto &m: statevect_real)
			std::cout<<m<<std::endl;

		std::cout<<std::endl;

		for(auto &m: statevect_imag)
				std::cout<<m<<std::endl;*/

	ASSERT_TRUE(stateVectorEq(statevect_real, statevect_imag, expected_statevect_real, expected_statevect_imag));

}

TEST (gateTest, CZ) {

	double e_statevect_real[] = { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., -1., 0., 0., 0., 0., 0.};
	double e_statevect_imag[] = { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.};

	std::vector<double> expected_statevect_real(std::begin(e_statevect_real), std::end(e_statevect_real));
	std::vector<double> expected_statevect_imag(std::begin(e_statevect_imag), std::end(e_statevect_imag));

	auto qubitReg = xacc::qalloc(4);
	auto qpu = xacc::getAccelerator("quest");
	auto compiler = xacc::getCompiler("xasm");

	auto ir = compiler->compile(R"(__qpu__ void test(qbit q) {
		X(q[1]);X(q[3]);
		CZ(q[0], q[2]); 
		CZ(q[1], q[3]);
	})", qpu);

	auto program = ir->getComposite("test");

	qpu->execute(qubitReg, program);

	std::vector<double> statevect_real = qubitReg->getInformation("statevect_real").as<std::vector<double>>();
	std::vector<double> statevect_imag = qubitReg->getInformation("statevect_imag").as<std::vector<double>>();

	/*for(auto &m: statevect_real)
			std::cout<<m<<std::endl;

		std::cout<<std::endl;

		for(auto &m: statevect_imag)
				std::cout<<m<<std::endl;*/

	ASSERT_TRUE(stateVectorEq(statevect_real, statevect_imag, expected_statevect_real, expected_statevect_imag));

}

TEST (gateTest, CPhase) {

	double e_statevect_real[] = { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,      1./2., 0., 0., 0., 0., 0.};
	double e_statevect_imag[] = { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., sqrt(3)/2., 0., 0., 0., 0., 0.};

	std::vector<double> expected_statevect_real(std::begin(e_statevect_real), std::end(e_statevect_real));
	std::vector<double> expected_statevect_imag(std::begin(e_statevect_imag), std::end(e_statevect_imag));

	auto qubitReg = xacc::qalloc(4);
	auto qpu = xacc::getAccelerator("quest");
	auto compiler = xacc::getCompiler("xasm");

	auto ir = compiler->compile(R"(__qpu__ void test(qbit q, double theta) {
		X(q[1]);X(q[3]);
		CPhase(q[0], q[2], theta); 
		CPhase(q[1], q[3], theta);
	})", qpu);

	auto program = ir->getComposite("test");
	auto evalved = program -> operator ()({M_PI/3});

	qpu->execute(qubitReg, evalved);

	std::vector<double> statevect_real = qubitReg->getInformation("statevect_real").as<std::vector<double>>();
	std::vector<double> statevect_imag = qubitReg->getInformation("statevect_imag").as<std::vector<double>>();

	ASSERT_TRUE(stateVectorEq(statevect_real, statevect_imag, expected_statevect_real, expected_statevect_imag));

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
