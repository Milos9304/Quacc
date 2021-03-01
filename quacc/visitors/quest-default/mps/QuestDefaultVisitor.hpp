 /***********************************************************************************
 * Copyright (c) 2017, UT-Battelle
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
#ifndef QUEST_DEFAULT_VISITOR_HPP_
#define QUEST_DEFAULT_VISITOR_HPP_

#include <cstdlib>
#include "Cloneable.hpp"

#include "../../../../quacc/visitors/quest-default/QuEST/include/QuEST.h"
#include "../../QuaccVisitor.hpp"

namespace quacc {

class QuestDefaultVisitor : public xQuaccVisitor {

  //using ITensor = itensor::ITensor;
  //using Index = itensor::Index;
  //using IndexVal = itensor::IndexVal;

public:
  QuestDefaultVisitor();
  virtual ~QuestDefaultVisitor();
  
  virtual std::shared_ptr<xQuaccVisitor> clone() {
    return std::make_shared<QuestDefaultVisitor>();
  }

  virtual const double getExpectationValueZ(std::shared_ptr<CompositeInstruction> function);
  virtual const double calcExpectationValueZ(ComplexArray in_stateVec, const std::set<size_t>& in_bits);

  virtual void setCustomState();

  virtual void initialize(std::shared_ptr<AcceleratorBuffer> buffer) override;
  virtual void finalize() {}

  //virtual void getState(double **&stateVec);

  // Service name as defined in manifest.json
  virtual const std::string name() const { return "quest-default"; }

  virtual const std::string description() const { return ""; }

  /**
   * Return all relevant Quacc runtime options.
   */
  virtual OptionPairs getOptions() {
	//OptionPairs desc{{"itensor-svd-cutoff",
    //                    "Provide the cutoff (default 1e-4) for the singular "
    //                    "value decomposition."}};
	OptionPairs desc{{"foobar","foobar"}};
    return desc;
  }

   // one-qubit gates
  void visit(Identity &gate) {}  //implemented-here
  void visit(Hadamard &gate);    //implemented, tested
  void visit(X &gate);			 //implemented, tested
  void visit(Y &gate);			 //implemented, tested
  void visit(Z &gate);			 //implemented, tested
  void visit(Rx &gate);			 //implemented, tested
  void visit(Ry &gate);			 //implemented, tested
  void visit(Rz &gate);			 //implemented, tested
  void visit(U& u);				 //implemented, tested

  // two-qubit gates
  void visit(CNOT &gate);		 //implemented, tested
  void visit(Swap &gate);		 //implemented, tested
  void visit(CZ &gate);			 //implemented, tested
  void visit(CPhase &gate);		 //implemented, tested

  // others
  void visit(Measure &gate);	 //implemented
//   void visit(Circuit &f);

private:

  QuESTEnv *env;
  Qureg *qreg;

  QuESTEnv env2;
  Qureg qreg2;

  bool initialized;

  double execTime = 0.0;
  double singleQubitTime = 1e-8;
  double twoQubitTime = 1e-7;
  //double svdCutoff = 1e-16;

  //itensor::ITensor wavefunc;
  //std::vector<int> iqbit2iind;
  std::vector<int> cbits;

  /*std::vector<ITensor> bondMats; // singular matricies
  std::vector<ITensor> legMats;  // matricies with physical legs

  std::vector<ITensor> bondMats_m; // the snapshot for measurement
  std::vector<ITensor> legMats_m;*/

  std::set<size_t> measured_bits; // indecies of qbits to measure

  //itensor::IndexSet legs; // physical degree of freedom
  int n_qbits;
  bool verbose = false, testing = false;

  void updateStateVectorInfo(Qureg &qreg, std::shared_ptr<AcceleratorBuffer> buffer); //used for testing

  /*

  /// init the wave function tensor
  void initWavefunc(int n_qbits);
  void initWavefunc_bysvd(int n_qbits);
  void reduce_to_MPS();
  //Index ind_for_qbit(int iqbit) const;
  void printWavefunc() const;
  void permute_to(int iqbit, int iqbit_to);
  //void kickback_ind(ITensor &tensor, const Index &ind);
  double wavefunc_inner();
  //double average(int iqbit, const ITensor &op_tensor);
  //itensor::ITensor tZ_measure_on(int iqbit_measured);
  double averZs(std::set<int> iqbits);
  void snap_wavefunc();

  */
};

} // namespace quacc
#endif /* QUEST_DEFAULT_VISITOR_HPP_  */
