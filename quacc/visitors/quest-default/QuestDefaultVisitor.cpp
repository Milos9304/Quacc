/***********************************************************************************
 * Copyright (c) 2017, UT-Battelle
 * Copyright (c) 2021, Milos Prokop
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the xacc nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   Initial implementation - Mengsu Chen 2017.7
 *   Modifications to include the Quacc - Milos Prokop 2021.2
 *
 **********************************************************************************/
#include "AllGateVisitor.hpp"
#include <complex>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include "Eigen/Dense"
#include "QuestDefaultVisitor.hpp"

namespace quacc {

	double ipToDouble(xacc::InstructionParameter p) {
	  if (p.which() == 0) {
		return (double) p.as<int>();
	  } else if (p.which() == 1) {
		return p.as<double>();
	  } else {
		std::stringstream s;
		s << p.toString();
		xacc::error("QuestDefaultVisitor: invalid gate parameter " +
					std::to_string(p.which()) + ", " + s.str());
	  }

	  return 0.0;
	}

	/// Constructor
	QuestDefaultVisitor::QuestDefaultVisitor() : n_qbits(0), initialized(false) {}

	void QuestDefaultVisitor::initialize(std::shared_ptr<AcceleratorBuffer> accbuffer_in) {

	  verbose = false;
	  if(xacc::optionExists("quest-verbose"))
		  verbose = xacc::getOption("quest-verbose") == "true";

	  testing = false;
	  if(xacc::optionExists("quest-testing"))
		  testing = xacc::getOption("quest-testing") == "true";

	  buffer = accbuffer_in;
	  n_qbits = accbuffer_in->size();
	  std::srand(std::time(0));
	  cbits.resize(n_qbits);
	  execTime = 0.0;

	  void *tempPointer;
	  std::stringstream env_adress(xacc::getOption("global_env"));
	  env_adress >> tempPointer;
	  env = (QuESTEnv*)tempPointer;

	  if(xacc::optionExists("use_global_qreg") && xacc::getOption("use_global_qreg") == "true"){

		  global_qreg = true;

		  std::stringstream qreg_adress(xacc::getOption("global_qreg"));
		  Qureg *qregPtr;

		  qreg_adress >> tempPointer;
		  qreg = (Qureg*)tempPointer;

		  //perform measurement on qreg2, *qreg still holds the initial statevector
		  if(buffer->hasExtraInfoKey("repeated_measurement_mode") &&
				  buffer->getInformation("repeated_measurement_mode").as<std::string>()=="true"){
			  qreg2 = createQureg(qreg->numQubitsInStateVec, *env);
			  cloneQureg(qreg2, *qreg);
		  }

	  }else{

		  global_qreg = false;
		  qreg2 = createQureg(n_qbits, *env);
		  qreg = &qreg2;

	  }

	  //initZeroState(*qreg);

	  measured_bits.clear();
	  initialized = true;


	}

	void QuestDefaultVisitor::finalize() {

		if(initialized && !global_qreg){
			destroyQureg(qreg2, *env);
			initialized = false;
		}

		destroyQuESTEnv(*env);

	}

	QuestDefaultVisitor::~QuestDefaultVisitor() {}

	void QuestDefaultVisitor::updateStateVectorInfo(Qureg &qreg, std::shared_ptr<AcceleratorBuffer> buffer){

		std::vector<double> stateVectReal;
		std::vector<double> stateVectImag;

		for(size_t i = 0; i < qreg.numAmpsTotal; ++i)
			stateVectReal.push_back(qreg.stateVec.real[i]);

		for(size_t i = 0; i < qreg.numAmpsTotal; ++i)
			stateVectImag.push_back(qreg.stateVec.imag[i]);

		buffer->addExtraInfo("statevect_real", stateVectReal);
		buffer->addExtraInfo("statevect_imag", stateVectImag);

	}

	void QuestDefaultVisitor::visit(Hadamard &gate) {

	  auto iqbit_in = gate.bits()[0];

	  if (verbose) {
		std::cout << "applying " << gate.name() << " @ " << iqbit_in << std::endl;
	  }

	  hadamard(*qreg, iqbit_in);

	  execTime += singleQubitTime;

	  if(testing){
	  		  updateStateVectorInfo(*qreg, buffer);
	  }

	}

	void QuestDefaultVisitor::visit(CZ &gate) {

		auto iqbit_c = gate.bits()[0];
		auto iqbit_q = gate.bits()[1];

		  if (verbose) {
			std::cout << "applying " << gate.name() << " @ control " << iqbit_c << " to " << iqbit_q << std::endl;
		  }

		  controlledPhaseFlip(*qreg, iqbit_c, iqbit_q);

		  execTime += twoQubitTime;

		  if(testing){
			  updateStateVectorInfo(*qreg, buffer);
		  }
	}

	void QuestDefaultVisitor::visit(CNOT &gate) {

		auto iqbit_c = gate.bits()[0];
		auto iqbit_q = gate.bits()[1];

		  if (verbose) {
			std::cout << "applying " << gate.name() << " @ control " << iqbit_c << " to " << iqbit_q << std::endl;
		  }

		 controlledNot(*qreg, iqbit_c, iqbit_q);

		 execTime += twoQubitTime;

		 if(testing){
			 updateStateVectorInfo(*qreg, buffer);
		 }
	}

	void QuestDefaultVisitor::visit(X &gate) {

		auto iqbit_in = gate.bits()[0];

		if (verbose) {
			std::cout << "applying " << gate.name() << " @ " << iqbit_in << std::endl;
		}

		pauliX(*qreg, iqbit_in);

		execTime += singleQubitTime;

		if(testing){
			updateStateVectorInfo(*qreg, buffer);
		}
	}

	void QuestDefaultVisitor::visit(Y &gate) {

		auto iqbit_in = gate.bits()[0];

		if (verbose) {
			std::cout << "applying " << gate.name() << " @ " << iqbit_in << std::endl;
		}

		pauliY(*qreg, iqbit_in);

		execTime += singleQubitTime;

		if(testing){
			updateStateVectorInfo(*qreg, buffer);
		}
	}

	void QuestDefaultVisitor::visit(Z &gate) {

		auto iqbit_in = gate.bits()[0];

		if (verbose) {
			std::cout << "applying " << gate.name() << " @ " << iqbit_in << std::endl;
		}

		pauliZ(*qreg, iqbit_in);

		execTime += singleQubitTime;

		if(testing){
			updateStateVectorInfo(*qreg, buffer);
		}
	}

	void QuestDefaultVisitor::visit(Measure &gate) {

		auto iqbit_in = gate.bits()[0];
		measured_bits.insert(iqbit_in);

		if (verbose) {
			std::cout << "applying " << gate.name() << " @ " << iqbit_in << std::endl;
		}

		measured_bits.insert(iqbit_in);

		Qureg *active_qreg;
		if(buffer->hasExtraInfoKey("repeated_measurement_mode") &&
						  buffer->getInformation("repeated_measurement_mode").as<std::string>()=="true")
			active_qreg = &qreg2;
		else
			active_qreg = qreg;

		const double expectedValueZ = this -> calcExpectationValueZ(active_qreg->stateVec, measured_bits);
		buffer->addExtraInfo("exp-val-z", expectedValueZ);

		const int measured = measure(*active_qreg, iqbit_in);

		buffer->measure(iqbit_in, measured);


		if(testing){
			updateStateVectorInfo(*active_qreg, buffer);
		}


	}

	const double QuestDefaultVisitor::calcExpectationValueZ(ComplexArray in_stateVec, const std::set<size_t>& in_bits){

		const auto hasEvenParity = [](size_t x, const std::set<size_t>& in_qubitIndices) -> bool {

			size_t count = 0;
			for (const auto& bitIdx : in_qubitIndices) {
				if (x & (1ULL << bitIdx))
					count++;
			}
			return (count % 2) == 0;
		};

		double result = 0.0;

		for(uint64_t i = 0; i < qreg->numAmpsTotal; ++i)
		{
			result += (hasEvenParity(i, in_bits) ? 1.0 : -1.0) * std::norm(std::complex<double>(in_stateVec.real[i], in_stateVec.imag[i]));
		}

		return result;


	}

	const double QuestDefaultVisitor::getExpectationValueZ(std::shared_ptr<CompositeInstruction> function){

		auto cachedStateVec = qreg->stateVec;
		std::set<size_t> measureBitIdxs;

		InstructionIterator it(function);
		while (it.hasNext())
		{
			auto nextInst = it.next();
			if (nextInst->isEnabled() && !nextInst->isComposite())
			{
				if (nextInst->name() == "Measure")
				{
					measureBitIdxs.insert(nextInst->bits()[0]);
				}
				else
				{
					// Apply change-of-basis gates (if any)
					nextInst->accept(this);
				}
			}
		}

		const double result = calcExpectationValueZ(qreg->stateVec, measureBitIdxs);
		// Restore the state vector
		qreg->stateVec = cachedStateVec;
		std::cout<<"WARNING!!!! THIS MIGHT BE VERY INEFFICIENT WAY TO DO IT! BECAUSE WE CREATE A COPY OF STATEVECTOR> THINK ABOUT BETTER ONE!";
		return result;

	}

	void QuestDefaultVisitor::visit(Rx &gate) {

		auto iqbit_in = gate.bits()[0];

		const double theta = InstructionParameterToDouble(gate.getParameter(0));

		if (verbose) {
			std::cout << "applying " << gate.name() << " @ " << iqbit_in << "  theta: " << theta << std::endl;
		}

		rotateX(*qreg, iqbit_in, theta);

		execTime += singleQubitTime;

		if(testing){
			updateStateVectorInfo(*qreg, buffer);
		}

	}

	void QuestDefaultVisitor::visit(Ry &gate) {

		auto iqbit_in = gate.bits()[0];
		const double theta = InstructionParameterToDouble(gate.getParameter(0));


		if (verbose) {
			std::cout << "applying " << gate.name() << " @ " << iqbit_in << "  theta: " << theta << std::endl;
		}

		rotateY(*qreg, iqbit_in, theta);

		execTime += singleQubitTime;

		if(testing){
			updateStateVectorInfo(*qreg, buffer);
		}
	}

	void QuestDefaultVisitor::visit(Rz &gate) {

		auto iqbit_in = gate.bits()[0];
		const double theta = InstructionParameterToDouble(gate.getParameter(0));

		if (verbose) {
			std::cout << "applying " << gate.name() << " @ " << iqbit_in << "  theta: " << theta << std::endl;
		}

		rotateZ(*qreg, iqbit_in, theta);

		execTime += singleQubitTime;

		if(testing){
			updateStateVectorInfo(*qreg, buffer);
		}
	}

	void QuestDefaultVisitor::visit(U &gate) {

		auto iqbit_in = gate.bits()[0];

		const double theta = InstructionParameterToDouble(gate.getParameter(0));
		const double pi = InstructionParameterToDouble(gate.getParameter(1));
		const double lambda = InstructionParameterToDouble(gate.getParameter(2));

		ComplexMatrix2 u;

		u.real[0][0] = cos(theta/2.);				u.real[0][1] = -sin(theta/2.) * cos(lambda);
		u.imag[0][0] = 0;							u.imag[0][1] = -sin(theta/2.) * sin(lambda);

		u.real[1][0] = sin(theta/2.) * cos(pi);		u.real[1][1] = cos(theta/2.) * cos(pi+lambda);
		u.imag[1][0] = sin(theta/2.) * sin(pi);		u.imag[1][1] = cos(theta/2.) * sin(pi+lambda);

		if (verbose) {
			std::cout << "applying " << gate.name() << " @ " << iqbit_in << "  theta: "
					<< theta << "   pi: " << pi << "   lambda: " << lambda << std::endl;
		}

		unitary(*qreg, iqbit_in, u);

		if(testing){
			updateStateVectorInfo(*qreg, buffer);
		}

	}

	void QuestDefaultVisitor::visit(CPhase &gate) {

		auto iqbit_c = gate.bits()[0];
		auto iqbit_q = gate.bits()[1];

		const double theta = InstructionParameterToDouble(gate.getParameter(0));

		if (verbose) {
			std::cout << "applying " << gate.name() << " @ control " << iqbit_c << " to " << iqbit_q << "theta:  " << iqbit_q << std::endl;
		}

		controlledPhaseShift(*qreg, iqbit_c, iqbit_q, theta);

		execTime += twoQubitTime;

		if(testing){
			updateStateVectorInfo(*qreg, buffer);
		}

	}

	void QuestDefaultVisitor::visit(Swap &gate) {

		auto iqbit_c = gate.bits()[0];
		auto iqbit_q = gate.bits()[1];

		if (verbose) {
			std::cout << "applying " << gate.name() << " @ control " << iqbit_c << " to " << iqbit_q << std::endl;
		}

		swapGate(*qreg, iqbit_c, iqbit_q);

		execTime += twoQubitTime;

		if(testing){
			updateStateVectorInfo(*qreg, buffer);
		}

	}

} // namespace quacc
