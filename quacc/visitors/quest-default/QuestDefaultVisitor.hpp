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
#ifndef QUEST_DEFAULT_VISITOR_HPP_
#define QUEST_DEFAULT_VISITOR_HPP_

#include <cstdlib>
#include "Cloneable.hpp"

#include "../../../../quacc/visitors/quest-default/QuEST/include/QuEST.h"
#include "../../QuaccVisitor.hpp"

namespace quacc {

class QuestDefaultVisitor : public xQuaccVisitor {

public:
  QuestDefaultVisitor();
  virtual ~QuestDefaultVisitor();
  
  virtual std::shared_ptr<xQuaccVisitor> clone() {
    return std::make_shared<QuestDefaultVisitor>();
  }

  virtual const double getExpectationValueZ(std::shared_ptr<CompositeInstruction> function);
  virtual const double calcExpectationValueZ(ComplexArray in_stateVec, const std::set<size_t>& in_bits);

  virtual void initialize(std::shared_ptr<AcceleratorBuffer> buffer) override;
  virtual void finalize() override;

  // Service name as defined in manifest.json
  virtual const std::string name() const { return "quest-default"; }

  virtual const std::string description() const { return ""; }

  /**
   * Return all relevant Quacc runtime options.
   */
  virtual OptionPairs getOptions() {

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
  Qureg qreg2;

  bool initialized;
  bool global_qreg;

  double execTime = 0.0;
  double singleQubitTime = 1e-8;
  double twoQubitTime = 1e-7;

  std::vector<int> cbits;

  std::set<size_t> measured_bits; // indecies of qbits to measure

  int n_qbits;
  bool verbose = false, testing = false;

  void updateStateVectorInfo(Qureg &qreg, std::shared_ptr<AcceleratorBuffer> buffer); //used for testing

};

} // namespace quacc
#endif /* QUEST_DEFAULT_VISITOR_HPP_  */
